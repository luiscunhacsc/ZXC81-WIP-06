/* ------------------------------------------------------------------
   zx81.c – Emulador Sinclair ZX-81 (SDL-1.2)
   Versão Final: Injeção Estável + Auto-RUN
   ------------------------------------------------------------------ */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <SDL/SDL.h>

#include "z80.h"
#include "zx81rom.h"
#include "zx81_tape.h"

typedef uint8_t  BYTE;
typedef uint16_t WORD;
#define D_FILE 0x400c
#define SYSVAR_MODE 0x4006

// ZX81 timing (PAL): CPU ~3.25MHz, frame ~50Hz.
// We render video ourselves (ROM display routine is patched out), but real ZX81
// SLOW mode loses a large share of CPU time to display generation.
// To avoid programs running "too fast", we approximate this by reducing the
// number of Z80 cycles executed per frame in SLOW mode.
#define ZX81_CPU_HZ 3250000u
#define ZX81_FRAME_HZ 50u
#define ZX81_CYCLES_PER_FRAME (ZX81_CPU_HZ / ZX81_FRAME_HZ)

/* --- Globais --- */
static z80  cpu;
static BYTE memory  [ 65536 ];
static BYTE keyboard[ 9 ];          
static BYTE sdlk2scan[ SDLK_LAST ]; 
static SDL_Surface *screen;
static SDL_Surface *charset;

/* Flag para sinalizar ao Main que deve escrever RUN */
static int request_auto_run = 0;

/* --- Callbacks --- */
static uint8_t zx_read(void *ud, uint16_t addr) { (void)ud; return memory[ addr ]; }
static void zx_write(void *ud, uint16_t addr, uint8_t val) { (void)ud; if (addr >= 0x4000) memory[ addr ] = val; }
static uint8_t zx_in(z80 *z, uint8_t port) {
    if ((port & 1) == 0) {
        uint8_t row_bits = z->b;    
        for (int row = 0; row < 8; ++row) {
            if ((row_bits & 1) == 0) return keyboard[ row ];
            row_bits >>= 1;
        }
        return 0xFF;
    }
    return 0xFF;
}
static void zx_out(z80 *z, uint8_t port, uint8_t val) { (void)z; (void)port; (void)val; }

/* --- Video --- */
static int create_charset(void) {
    SDL_Surface *tmp;
    Uint32 rm, gm, bm, black, white;
    Uint32 *pix;
    int addr = 0x1e00;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rm = 0xFF000000; gm = 0x00FF0000; bm = 0x0000FF00;
#else
    rm = 0x000000FF; gm = 0x0000FF00; bm = 0x00FF0000;
#endif
    tmp = SDL_CreateRGBSurface(SDL_SWSURFACE, 4096, 16, 32, rm, gm, bm, 0);
    if (!tmp) return 0;

    black = SDL_MapRGB(tmp->format, 0, 0, 0);
    white = SDL_MapRGB(tmp->format, 255, 255, 255);
    pix = (Uint32*)tmp->pixels;
    int pitch = tmp->pitch / 4;

    for (int ch = 0; ch < 64; ++ch) {
        for (int row = 0; row < 8; ++row) {
            int b = rom[ addr++ ];
            for (int col = 0; col < 8; ++col) {
                Uint32 ink = (b & 0x80) ? black : white;
                Uint32 pap = (b & 0x80) ? white : black;
                pix[0] = ink; pix[1] = ink; pix[pitch] = ink; pix[pitch+1] = ink;
                pix[2048] = pap; pix[2049] = pap; pix[pitch+2048] = pap; pix[pitch+2049] = pap;
                pix += 2; b <<= 1;
            }
            pix += pitch * 2 - 16;
        }
        pix -= pitch * 16 - 16;
    }
    charset = SDL_DisplayFormat(tmp);
    SDL_FreeSurface(tmp);
    return 1;
}

static void setup_emulation(void) {
    memcpy(memory + 0x0000, rom, 0x2000);
    memcpy(memory + 0x2000, rom, 0x2000);
    memory[0x02B5] = 0xC9; memory[0x22B5] = 0xC9; // Patch Display
    memset(keyboard, 0xFF, sizeof keyboard);
    memset(sdlk2scan, 8 << 5, sizeof sdlk2scan);
    
    // Mapa de teclas essencial
    sdlk2scan[SDLK_LSHIFT] = 0 << 5 | 1; sdlk2scan[SDLK_RSHIFT] = 0 << 5 | 1;
    sdlk2scan[SDLK_z]=0<<5|2; sdlk2scan[SDLK_x]=0<<5|4; sdlk2scan[SDLK_c]=0<<5|8; sdlk2scan[SDLK_v]=0<<5|16;
    sdlk2scan[SDLK_a]=1<<5|1; sdlk2scan[SDLK_s]=1<<5|2; sdlk2scan[SDLK_d]=1<<5|4; sdlk2scan[SDLK_f]=1<<5|8; sdlk2scan[SDLK_g]=1<<5|16;
    sdlk2scan[SDLK_q]=2<<5|1; sdlk2scan[SDLK_w]=2<<5|2; sdlk2scan[SDLK_e]=2<<5|4; sdlk2scan[SDLK_r]=2<<5|8; sdlk2scan[SDLK_t]=2<<5|16;
    sdlk2scan[SDLK_1]=3<<5|1; sdlk2scan[SDLK_2]=3<<5|2; sdlk2scan[SDLK_3]=3<<5|4; sdlk2scan[SDLK_4]=3<<5|8; sdlk2scan[SDLK_5]=3<<5|16;
    sdlk2scan[SDLK_0]=4<<5|1; sdlk2scan[SDLK_9]=4<<5|2; sdlk2scan[SDLK_8]=4<<5|4; sdlk2scan[SDLK_7]=4<<5|8; sdlk2scan[SDLK_6]=4<<5|16;
    sdlk2scan[SDLK_p]=5<<5|1; sdlk2scan[SDLK_o]=5<<5|2; sdlk2scan[SDLK_i]=5<<5|4; sdlk2scan[SDLK_u]=5<<5|8; sdlk2scan[SDLK_y]=5<<5|16;
    sdlk2scan[SDLK_RETURN]=6<<5|1; sdlk2scan[SDLK_l]=6<<5|2; sdlk2scan[SDLK_k]=6<<5|4; sdlk2scan[SDLK_j]=6<<5|8; sdlk2scan[SDLK_h]=6<<5|16;
    sdlk2scan[SDLK_SPACE]=7<<5|1; sdlk2scan[SDLK_PERIOD]=7<<5|2; sdlk2scan[SDLK_m]=7<<5|4; sdlk2scan[SDLK_n]=7<<5|8; sdlk2scan[SDLK_b]=7<<5|16;

    z80_init(&cpu);
    cpu.read_byte  = zx_read;
    cpu.write_byte = zx_write;
    cpu.port_in    = zx_in;
    cpu.port_out   = zx_out;
}

/* --- LOOP DE EXECUÇÃO --- */
static uint32_t zx81_cycles_this_frame(void)
{
    // MODE==0 => SLOW. Non-zero => FAST.
    // Typical effective speed difference is ~4x (SLOW is much slower).
    if (memory[SYSVAR_MODE] == 0) return (uint32_t)(ZX81_CYCLES_PER_FRAME / 4u);
    return (uint32_t)ZX81_CYCLES_PER_FRAME;
}

static void run_some(uint32_t cycles_to_run)
{
    uint64_t target = cpu.cyc + (uint64_t)cycles_to_run;
    static int log_limit = 0;

    while (cpu.cyc < target) {
        z80_step(&cpu);

        // Verifica se a ROM está na rotina de LOAD (Ecrã Branco)
        if (cpu.pc >= 0x0340 && cpu.pc <= 0x03F0) {
            
            if (tape_should_inject(&cpu, memory)) {
                
                if (tape_inject(&cpu, memory)) {
                    // 1. Limpa Teclado
                    memset(keyboard, 0xFF, sizeof(keyboard));

                    // Pede ao Main para escrever "RUN" (quando aplicável)
                    request_auto_run = 1;
                    
                    tape_log("ZX81.C: Injection Success. Requesting Auto-RUN.");
                }
            }
            
            // Log anti-spam para monitorizar
            log_limit++;
            if (log_limit > 5000) {
                // tape_log("STALL: Waiting..."); // Comentado para não encher o log
                log_limit = 0;
            }
        }
    }
}

static int consume_events(void) {
    SDL_Event ev; BYTE scan;
    while (SDL_PollEvent(&ev)) {
        switch (ev.type) {
        case SDL_KEYDOWN:
            if (ev.key.keysym.sym == SDLK_F12) {
                tape_log("[F12] Manual Force.");
                if (!tape_is_loaded()) tape_set_filename("PROGRAM.P");
                tape_load(&cpu, memory);
                if(tape_inject(&cpu, memory)) {
                    request_auto_run = 1;
                }
            }
            else if (ev.key.keysym.sym == SDLK_BACKSPACE) {
                keyboard[0] &= ~1; keyboard[4] &= ~1;
            } else {
                scan = sdlk2scan[ ev.key.keysym.sym ];
                keyboard[ scan >> 5 ] &= ~(scan & 0x1F);
            }
            break;
        case SDL_KEYUP:
            if (ev.key.keysym.sym == SDLK_BACKSPACE) {
                keyboard[0] |= 1; keyboard[4] |= 1;
            } else {
                scan = sdlk2scan[ ev.key.keysym.sym ];
                keyboard[ scan >> 5 ] |= (scan & 0x1F);
            }
            break;
        case SDL_QUIT: return 0;
        }
    }
    return 1;
}

static void update_screen(void) {
    WORD dfile = memory[D_FILE] | (memory[D_FILE + 1] << 8);
    SDL_Rect src = {0,0,16,16}, dst = {0,0,0,0};
    for (int row=0; row<24; ++row) {
        dst.x = 0;
        for (int col=0; col<32; ++col) {
            src.x = memory[++dfile] << 4;
            SDL_BlitSurface(charset, &src, screen, &dst);
            dst.x += 16;
        }
        ++dfile; dst.y += 16;
    }
    SDL_UpdateRect(screen, 0, 0, 0, 0);
}

int main(int argc, char *argv[]) {
    setbuf(stdout, NULL); 
    tape_log("--- SESSION START ---");
    
    // Variáveis para a automação do teclado (RUN)
    int auto_run_counter = 0;
    int auto_run_step = 0;
    const BYTE run_keys[4] = { 
        0b11110, // R (Linha 2, Bit 3? Não, mapa ZX: R é row 2 bit 3 -> 0xFB)
                 // Wait, zx81 map is specific. 
                 // R is Row 2, bit 3 (mask 0x08). So byte is ~0x08 = 0xF7.
                 // Let's use the scan codes from init to be safe.
        0,0,0,0 // Filled below
    };
    
    // Configuraçao do emulador
    if (argc > 1 && argv[1][0] != '\0') {
        tape_set_filename(argv[1]);
    }

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) return 1;
    screen = SDL_SetVideoMode(512, 384, 0, SDL_SWSURFACE);
    if (!screen) return 1;
    if (!create_charset()) return 1;
    
    setup_emulation();
    tape_load(&cpu, memory); 
    
    #if SDL_MAJOR_VERSION == 1
    SDL_EnableKeyRepeat(0, 0);
    #endif

    // Mapeamento manual para a sequencia R-U-N-Enter
    // R = Linha 2, Bit 3 -> Byte[2] &= ~8
    // U = Linha 5, Bit 3 -> Byte[5] &= ~8
    // N = Linha 7, Bit 3 -> Byte[7] &= ~8
    // Enter = Linha 6, Bit 0 -> Byte[6] &= ~1
    
    int running = 1;
    do {
        uint32_t t0 = SDL_GetTicks();

        // ZX81 NMIs are only present in SLOW mode.
        // Approximation: generate an NMI when the CPU is in HALT (display wait).
        if (memory[SYSVAR_MODE] == 0 && cpu.halted) {
            z80_gen_nmi(&cpu);
        }

        run_some(zx81_cycles_this_frame());
        
        // --- AUTO-RUN LOGIC ---
        if (request_auto_run) {
            request_auto_run = 0; // Ack
            auto_run_counter = 25; // Inicia sequência (duração total)
            auto_run_step = 0;
            tape_log("MAIN: Starting Auto-RUN sequence.");
        }

        if (auto_run_counter > 0) {
            // Liberta todas as teclas primeiro
            memset(keyboard, 0xFF, sizeof(keyboard));
            
            // Aplica tecla baseada no passo
            // ZX81: em modo "K", a tecla R insere o keyword RUN (não é preciso U/N).
            // Sequência: RUN (R) + ENTER.
            // Passo 0: R (Frames 25-14)
            // Passo 1: Enter (Frames 13-2)

            if (auto_run_counter > 14)      keyboard[2] &= ~8; // R (RUN)
            else if (auto_run_counter > 2)  keyboard[6] &= ~1; // Enter
            
            auto_run_counter--;
        }

        // Save detection
        if (cpu.pc == 0x04D0) {
            static int saved = 0;
            if (!saved) {
                tape_save("saved", memory, &cpu);
                saved = 1;
            }
        }

        running = consume_events();
        update_screen();

        // Debug leve: se ficarmos presos em HALT por muito tempo, regista.
        static uint32_t halted_frames = 0;
        if (cpu.halted) {
            halted_frames++;
            if (halted_frames == 50) {
                tape_log("DEBUG: CPU halted for ~1s at PC=%04X", cpu.pc);
            }
        } else {
            halted_frames = 0;
        }

        uint32_t elapsed = SDL_GetTicks() - t0;
        if (elapsed < 20) SDL_Delay(20 - elapsed);
    } while (running);

    SDL_Quit();
    return 0;
}