# Emulador Sinclair ZX81 (Windows)

Emulador do **Sinclair ZX81**, escrito em C, com CPU Z80 incluída e interface gráfica em **SDL 1.2**.

Esta versão carrega programas **`.P`** (snapshots de memória do ZX81) a partir da pasta `tapes/` (ou da pasta atual) e regista o processo de carregamento no ficheiro `tape_log.txt`.

Importante: nesta fase, o carregamento de programas é feito **pela linha de comandos** (argumento ao arrancar) e **tecla F12** para forçar a injeção. O comando `LOAD "..."` no BASIC do ZX81 **não é o fluxo suportado**.

---

## Parte 1 — Utilizador Windows (rápido)

Se só queres **correr programas** (sem te preocupares com compilação), o fluxo é este:

1) Copia os ficheiros `.P` para a pasta:

```text
tapes/
```

2) Abre a linha de comandos na pasta do projeto e corre:

```bash
zx81 ZORLAC.P
```

Notas:

- No Windows, `zx81` normalmente significa `zx81.exe` (podes usar `zx81.exe ZORLAC.P` se preferires).
- O emulador procura o ficheiro na pasta atual e em `tapes/`.

3) Com a janela do emulador aberta, pressiona:

- **F12** (força a injeção/carregamento do snapshot)

Depois disso, o emulador tenta iniciar automaticamente o programa (Auto-RUN).

---

## Parte 2 — Compilação e detalhes (Windows)

---

### Pré-requisitos

Precisas de:

- **MinGW-w64 (gcc)** + **mingw32-make**
- **SDL 1.2** (headers + libs)

### Opção A (recomendada): MSYS2

1) Instala o MSYS2: https://www.msys2.org/

2) Abre o terminal **“MSYS2 MinGW x64”** e instala:

```bash
pacman -S --needed mingw-w64-x86_64-toolchain mingw-w64-x86_64-SDL make
```

3) Na pasta do projeto:

```bash
mingw32-make
```

### Opção B: MinGW-w64 “standalone” + SDL 1.2

Se já tens MinGW-w64 fora do MSYS2, garante que:

- `gcc` e `mingw32-make` estão no `PATH`
- a SDL 1.2 está disponível para o compilador/linker

O `Makefile` usa por defeito:

```text
-lmingw32 -lSDLmain -lSDL
```

Se precisares de indicar caminhos manualmente:

```bash
mingw32-make SDL_CFLAGS=-IC:\SDL\include SDL_LIBS="-LC:\SDL\lib -lmingw32 -lSDLmain -lSDL"
```

---

### Como correr programas `.P`

### 1) Colocar o ficheiro

Podes colocar o `.P` em qualquer um destes locais:

1) na pasta atual (onde está o `zx81.exe`)
2) na pasta `tapes/` (ex.: `tapes/ZORLAC.P`)

### 2) Arrancar o emulador com o nome do ficheiro

Exemplo:

```bash
zx81.exe ZORLAC.P
```

Notas:

- O nome pode estar em minúsculas (ex.: `zorlac.p`) — no Windows costuma funcionar.
- Se o ficheiro não existir, consulta o `tape_log.txt` para veres exatamente onde o emulador procurou.

### 3) Forçar o carregamento (injeção)

Com o emulador aberto, pressiona:

- **F12** — força o carregamento/injeção do snapshot atualmente selecionado

Depois disto, o emulador tenta iniciar automaticamente o programa (Auto-RUN).

---

### Logs e diagnóstico

O emulador escreve um log em:

```text
tape_log.txt
```

Este ficheiro é útil para:

- confirmar que o `.P` foi aberto com sucesso
- confirmar que houve “injeção” para RAM
- perceber por que motivo um programa não arrancou

---

### Problemas comuns

### Erro ao compilar: “Permission denied” no `zx81.exe`

Se aparecer algo do género `cannot open output file zx81.exe: Permission denied`, é porque o executável ainda está a correr (o Windows bloqueia o ficheiro).

Solução:

1) Fecha o emulador
2) Volta a correr `mingw32-make`

### O emulador fecha logo ao arrancar / não encontra SDL

Normalmente significa que falta a SDL 1.2 no ambiente (DLLs/runtime). Usa MSYS2 (recomendado) ou garante que as DLLs necessárias estão disponíveis.

---

### Estrutura do projeto

- `zx81.c` — loop principal (SDL), teclado, vídeo, timing
- `z80.c` / `z80.h` — core da CPU Z80
- `zx81_tape.c` / `zx81_tape.h` — loader/injeção de `.P` + logging
- `tapes/` — exemplos de programas `.P`

