# 🕹️ Emulador Sinclair ZX81 (Windows • Linha de Comandos)

Este repositório contém um emulador do **Sinclair ZX81**, escrito em C, com CPU Z80 incluída e interface gráfica em **SDL 1.2**.

O emulador suporta carregar programas **`.P`** (snapshots de RAM do ZX81) a partir da pasta `tapes/`, e regista toda a atividade do “tape loader” no ficheiro `tape_log.txt`.

---

## ✅ O que precisas (Windows)

Para compilar e correr o emulador no Windows, precisas de:

1. **MinGW-w64 (gcc)** e **mingw32-make**
2. **SDL 1.2** (bibliotecas e headers de desenvolvimento)

### Opção A (recomendada): MSYS2

1) Instala o MSYS2: https://www.msys2.org/

2) Abre o terminal **“MSYS2 MinGW x64”** e instala as dependências:

```bash
pacman -S --needed mingw-w64-x86_64-toolchain mingw-w64-x86_64-SDL make
```

3) Compila a partir da pasta do projeto:

```bash
mingw32-make
```

### Opção B: MinGW-w64 “standalone” + SDL 1.2

Se já tens MinGW-w64 instalado fora do MSYS2, garante que:

- `gcc` e `mingw32-make` estão no `PATH`
- tens os headers e libs da SDL 1.2 acessíveis

O `Makefile` compila por defeito com:

```text
-lmingw32 -lSDLmain -lSDL
```

Se a SDL não estiver no caminho padrão do linker, podes compilar assim (exemplo):

```bash
mingw32-make SDL_CFLAGS=-IC:\SDL\include SDL_LIBS="-LC:\SDL\lib -lmingw32 -lSDLmain -lSDL"
```

---

## 🔧 Compilar

Na pasta do projeto:

```bash
mingw32-make
```

Isto gera o executável:

```text
zx81.exe
```

---

## ▶️ Correr o emulador (linha de comandos)

Podes arrancar o emulador de duas formas:

### 1) Arrancar “limpo” (sem ficheiro)

```bash
zx81.exe
```

Depois, no ecrã do ZX81, escreve o comando de BASIC:

```text
LOAD "ZORLAC.P"
```

Nota: o emulador tenta detetar o nome do ficheiro a partir da RAM durante o loop de LOAD.

### 2) Arrancar já a apontar para um `.P`

```bash
zx81.exe ZORLAC.P
```

Isto define o nome do ficheiro a carregar logo ao arrancar. A seguir, no ZX81, faz:

```text
LOAD "ZORLAC.P"
```

---

## 📼 Onde colocar os ficheiros `.P`

O emulador procura o ficheiro nesta ordem:

1) na pasta atual (onde está o `zx81.exe`)
2) na pasta `tapes/` (ex.: `tapes/ZORLAC.P`)

Exemplos incluídos em `tapes/`:

- `ZORLAC.P`
- `GRAFFITY.P`
- `CLCKFREQ.P`

---

## 📝 Logs do tape loader

Sempre que corres o emulador, ele escreve um log em:

```text
tape_log.txt
```

Se um programa “fica branco”, não carrega, ou parece encravar, este ficheiro é a primeira coisa a verificar.

---

## 🖱️ Teclas

- O teclado está mapeado para a matriz do ZX81 (via SDL)
- Usa letras, `SHIFT`, `RETURN`, `SPACE`, etc.

---

## 🧯 Problemas comuns (Windows)

### “Permission denied” ao compilar

Se `mingw32-make` falhar com `cannot open output file zx81.exe: Permission denied`, é quase sempre o Windows a bloquear o executável porque ele ainda está a correr.

Solução:

1) Fecha a janela do emulador
2) Volta a compilar

### O emulador corre “rápido demais” / timing

O emulador inclui limitação de velocidade por frame para aproximar o comportamento real do ZX81 (SLOW vs FAST). Se quiseres ajustar o “feeling”, o ponto de entrada para timing está no `zx81.c` (ciclos por frame / modo).

---

## 📁 Estrutura do projeto (resumo)

- `zx81.c` — loop principal, SDL, teclado, vídeo
- `z80.c` / `z80.h` — core da CPU Z80
- `zx81_tape.c` / `zx81_tape.h` — loader de `.P`, injeção de RAM e logging
- `tapes/` — exemplos de programas `.P`

