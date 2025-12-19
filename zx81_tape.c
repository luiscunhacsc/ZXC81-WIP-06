#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <stdarg.h>
#include "z80.h"
#include "zx81_tape.h"

#define TAPE_DIR "tapes/"
static char tape_filename[128] = "";
static uint8_t tape_buffer[65536];
static size_t tape_size = 0;
static int tape_loaded = 0;
static int tape_used = 0;

static int is_zx81_filename_char(uint8_t v) {
    v &= 0x7F;
    if (v >= 0x26 && v <= 0x3F) return 1; // A-Z
    if (v == 0x1B) return 1; // '.'
    if (v >= 0x1C && v <= 0x25) return 1; // 0-9 (heuristic)
    return 0;
}

static char zx81_char_to_ascii(uint8_t v) {
    v &= 0x7F;
    if (v >= 0x26 && v <= 0x3F) return (char)('A' + (v - 0x26));
    if (v == 0x1B) return '.';
    if (v >= 0x1C && v <= 0x25) return (char)('0' + (v - 0x1C));
    return '\0';
}

static int extract_p_filename_from_ram(const uint8_t *memory, char *out, size_t out_cap) {
    if (!memory || !out || out_cap < 8) return 0;
    out[0] = '\0';

    // Heuristic scan: find ".P" (ZX81 '.'=0x1B, 'P'=0x35) anywhere in RAM.
    // We mask bit7 because ZX81 sets bit7 on last character of a line.
    const uint8_t dot = 0x1B;
    const uint8_t pch = 0x35;
    const uint16_t start = 0x4000;
    const uint16_t end = 0x8000; // typical max RAM area for ZX81 + RAM pack

    for (uint32_t i = start; i + 1 < end; ++i) {
        uint8_t b0 = memory[i] & 0x7F;
        uint8_t b1 = memory[i + 1] & 0x7F;
        if (b0 != dot || b1 != pch) continue;

        // Walk backwards to find start of filename.
        uint32_t s = i;
        uint32_t back_limit = (i > 80) ? (i - 80) : start;
        while (s > back_limit && is_zx81_filename_char(memory[s - 1])) {
            --s;
        }

        // Walk forwards to capture filename.
        size_t w = 0;
        for (uint32_t j = s; j < end && is_zx81_filename_char(memory[j]); ++j) {
            if (w + 1 >= out_cap) break;
            char c = zx81_char_to_ascii(memory[j]);
            if (!c) break;
            out[w++] = c;
        }
        out[w] = '\0';

        // Require extension .P
        size_t len = strlen(out);
        if (len >= 2 && out[len - 2] == '.' && (out[len - 1] == 'P' || out[len - 1] == 'p')) {
            out[len - 1] = 'P';
            return 1;
        }
    }
    return 0;
}

void tape_log(const char *fmt, ...) {
    FILE *lf = fopen("tape_log.txt", "a");
    if (!lf) return;
    time_t now = time(NULL);
    char buf[32];
    strftime(buf, sizeof(buf), "%H:%M:%S", localtime(&now));
    fprintf(lf, "[%s] ", buf);
    va_list ap;
    va_start(ap, fmt);
    vfprintf(lf, fmt, ap);
    va_end(ap);
    fprintf(lf, "\n");
    fclose(lf);
}

void tape_set_filename(const char *filename) {
    if (filename && filename[0]) {
        strncpy(tape_filename, filename, sizeof(tape_filename)-1);
        tape_filename[sizeof(tape_filename)-1] = '\0';
        tape_log("SETUP: Filename set to '%s'", tape_filename);
    }
}

int tape_is_loaded(void) {
    return tape_loaded;
}

int tape_load(struct z80 *cpu, uint8_t *memory) {
    (void)cpu; (void)memory;
    tape_used = 0;
    tape_loaded = 0;
    tape_size = 0;

    if (tape_filename[0] == '\0') {
        tape_log("LOAD: No filename defined.");
        return 0;
    }

    char path[256];
    FILE *f = fopen(tape_filename, "rb");
    if (!f) {
        snprintf(path, sizeof(path), "%s%s", TAPE_DIR, tape_filename);
        f = fopen(path, "rb");
        if (!f) {
            tape_log("LOAD: Error opening '%s' or '%s'", tape_filename, path);
            return 0;
        }
    }

    size_t read_bytes = fread(tape_buffer, 1, sizeof(tape_buffer), f);
    fclose(f);

    if (read_bytes < 14) {
        tape_log("LOAD: File too small (%zu bytes)", read_bytes);
        return 0;
    }

    // Heurística útil para debug (formato comum: base RAM=0x4009):
    // E_LINE (0x4014) costuma apontar para o fim do bloco guardado.
    uint16_t e_line = tape_buffer[0x0B] | (tape_buffer[0x0C] << 8);
    size_t expected_len = 0;
    if (e_line > 0x4009) expected_len = (size_t)(e_line - 0x4009);

    tape_size = read_bytes;
    tape_loaded = 1;
    
    tape_log("LOAD: Success. '%s' (%zu bytes) ready in buffer. E_LINE=%04X expected_len=%zu", tape_filename, tape_size, e_line, expected_len);
    return 1;
}

// Analisa a memória do ZX81 à procura do comando LOAD "..."
void tape_autoload_from_memory(uint8_t *memory) {
    if (tape_loaded) return; // Já temos um

    // O ZX81 guarda a linha atual (BASIC) apontada por NXTLIN? Não, isso é para execução.
    // O buffer de edição ou a linha atual estão algures.
    // Vamos fazer um scan "bruto" na área de variáveis e BASIC (0x4000 a 0x4200)
    // Procuramos o token de LOAD (0xF5 ou J keyword) seguido de aspas? 
    // É mais simples procurar qualquer string entre aspas que pareça um ficheiro .P
    
    // Procura simples: uma string que termine em .P ou .p
    // O ZX81 usa codificação própria, mas ".P" (ASCII) mapeia para os caracteres gráficos.
    // Vamos assumir que o utilizador escreveu o nome.
    // Caracteres ZX81: '.' = 0x1B, 'P' = 0x35
    
    // Método alternativo robusto:
    // Se o CPU está no loop de LOAD, o utilizador acabou de executar a linha.
    // Vamos tentar carregar o ficheiro default "PROGRAM.P" se nada for encontrado.
    
    char found[128];
    if (extract_p_filename_from_ram(memory, found, sizeof(found))) {
        tape_log("AUTOLOAD: Found requested file '%s' in ZX81 RAM.", found);
        tape_set_filename(found);
        if (!tape_load(NULL, NULL)) {
            tape_log("AUTOLOAD: Failed to open '%s' (also tried tapes/).", found);
        }
        return;
    }

    // No fallback here: if we didn't find a filename in RAM yet, keep waiting.
    // The LOAD command may not have been typed/executed when we first entered the ROM loop.
    tape_log("AUTOLOAD: No filename found in RAM yet.");
}

int tape_should_inject(struct z80 *cpu, uint8_t *memory) {
    static uint32_t autoload_poll = 0;
    // 1. Verificar zona da ROM (Load routine: 0x0340 - 0x03F0)
    if (cpu->pc >= 0x0340 && cpu->pc <= 0x03F0) {
        // Estamos na zona de carregamento.
        
        if (!tape_loaded) {
            // Se estamos na zona de load mas não temos ficheiro, tentamos encontrar um agora!
            autoload_poll++;
            if (autoload_poll == 1 || (autoload_poll % 20000u) == 0) {
                tape_log("DETECT: CPU waiting for tape, but buffer empty. Attempting Autoload...");
                tape_autoload_from_memory(memory);
            }
            if (!tape_loaded) return 0; // Ainda vazio
        }

        if (tape_used) return 0; // Já usámos este

        tape_log("TRIGGER: CPU at %04X, Tape Ready. Injecting!", cpu->pc);
        return 1;
    }
    autoload_poll = 0;
    return 0;
}

int tape_inject(struct z80 *cpu, uint8_t *memory) {
    if (!tape_loaded) return 0;

    tape_log("INJECT: Writing to RAM...");
    
    // ZX81 .P (P-file) costuma conter a imagem de RAM a partir de 0x4009 até E_LINE.
    // O nosso ZORLAC.P bate certo com isso: expected_len = E_LINE - 0x4009 == 7214.
    memcpy(memory + 0x4009, tape_buffer, tape_size);

    // IMPORTANT: .P files typically omit the first 9 bytes of system variables (0x4000..0x4008).
    // If we don't reconstruct them, ROM routines after LOAD can misbehave or hang.
    // Use a pragmatic "16K+" configuration (stack at 0x8000-4), matching common RAM packs.
    uint16_t sp = 0x8000 - 4;
    memory[0x4000] = 0xFF;                 // ERR_NR
    memory[0x4001] = 0x80;                 // FLAGS
    memory[0x4002] = (uint8_t)(sp & 0xFF); // ERR_SP lo
    memory[0x4003] = (uint8_t)(sp >> 8);   // ERR_SP hi
    memory[0x4004] = (uint8_t)((sp + 4) & 0xFF); // RAMTOP lo
    memory[0x4005] = (uint8_t)((sp + 4) >> 8);   // RAMTOP hi
    memory[0x4006] = 0x00;                 // MODE
    memory[0x4007] = 0xFE;                 // PPC lo
    memory[0x4008] = 0xFF;                 // PPC hi

    // Minimal stack contents used by ROM after LOAD in some implementations.
    memory[sp + 0] = 0x76;
    memory[sp + 1] = 0x06;
    memory[sp + 2] = 0x00;
    memory[sp + 3] = 0x3E;

    // Debug/sanity: pointers used by emulator and ROM.
    // D_FILE address in this emulator is 0x400C.
    uint16_t d_file = memory[0x400C] | (memory[0x400D] << 8);
    // From file header: E_LINE is stored at offset 0x0B relative to 0x4009 -> 0x4014.
    uint16_t e_line = memory[0x4014] | (memory[0x4015] << 8);
    tape_log("INJECT: Post-copy D_FILE=%04X (mem[400C..400D]=%02X %02X)", d_file, memory[0x400C], memory[0x400D]);
    tape_log("INJECT: Post-copy E_LINE=%04X (mem[4014..4015]=%02X %02X)", e_line, memory[0x4014], memory[0x4015]);
    if (d_file >= 0x4000 && d_file < 0x8000) {
        tape_log("INJECT: D_FILE first bytes: %02X %02X %02X %02X", memory[d_file], memory[d_file + 1], memory[d_file + 2], memory[d_file + 3]);
    } else {
        tape_log("INJECT: WARNING D_FILE out of range: %04X", d_file);
    }

    tape_used = 1;

    // Saltar para Main Loop
    if (cpu) {
        // Initialize CPU state similarly to known-good ZX81 LOAD behaviour.
        cpu->a = 0x0B; cpu->f = 0x00;
        cpu->b = 0x00; cpu->c = 0x02;
        cpu->d = 0x43; cpu->e = 0x9B;
        cpu->h = 0x43; cpu->l = 0x99;
        cpu->a_ = 0xEC; cpu->f_ = 0xA9;
        cpu->b_ = 0x81; cpu->c_ = 0x02;
        cpu->d_ = 0x00; cpu->e_ = 0x2B;
        cpu->h_ = 0x00; cpu->l_ = 0x00;
        cpu->ix = 0x0281;
        cpu->iy = 0x4000;
        cpu->i = 0x1E;
        cpu->r = 0xDD;
        cpu->interrupt_mode = 2;
        cpu->iff1 = 0;
        cpu->iff2 = 0;
        cpu->iff_delay = 0;
        cpu->sp = sp;
        cpu->pc = 0x0207;
        cpu->halted = 0;
        tape_log("INJECT: Init CPU state. PC=%04X SP=%04X IM=%d", cpu->pc, cpu->sp, cpu->interrupt_mode);
        tape_log("INJECT: Sysvars ERR_SP=%02X%02X RAMTOP=%02X%02X MODE=%02X PPC=%02X%02X",
                 memory[0x4003], memory[0x4002], memory[0x4005], memory[0x4004], memory[0x4006], memory[0x4008], memory[0x4007]);
    }
    return 1;
}

// Stub para SAVE
int tape_should_save(const uint8_t *memory) {
    if (memory[0x4009] == 0x3A) return 1;
    return 0;
}
int tape_save(const char *name, uint8_t *memory, struct z80 *cpu) {
    (void)name; (void)memory; (void)cpu;
    return 0;
}