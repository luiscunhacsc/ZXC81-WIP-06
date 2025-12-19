#ifndef ZX81_TAPE_H
#define ZX81_TAPE_H

#include <stdint.h>
struct z80;

void tape_log(const char *fmt, ...);
void tape_set_filename(const char *filename);

int tape_load(struct z80 *cpu, uint8_t *memory);
int tape_should_inject(struct z80 *cpu, uint8_t *memory);
int tape_inject(struct z80 *cpu, uint8_t *memory);
int tape_is_loaded(void); // <--- NOVA

// Tenta encontrar "NOME.P" na memória do ZX81
void tape_autoload_from_memory(uint8_t *memory);

int tape_should_save(const uint8_t *memory);
int tape_save(const char *name, uint8_t *memory, struct z80 *cpu);

#endif