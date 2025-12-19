# 🕹️ Emulador Sinclair ZX81 (Windows • Linha de Comandos)

Este repositório contém um emulador do **Sinclair ZX81**, escrito em C, com CPU Z80 incluída e interface gráfica em **SDL 1.2**.

O emulador suporta carregar programas **`.P`** (snapshots de RAM do ZX81) a partir da pasta `tapes/`, e regista toda a atividade do “tape loader” no ficheiro `tape_log.txt`.

Neste momento, o fluxo suportado é **carregar o programa via linha de comandos** (argumento do executável). O comando `LOAD "..."` dentro do BASIC do ZX81 **não é o método suportado** nesta versão.

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

### 1) Carregar um `.P` diretamente (recomendado)

Na pasta do projeto (onde está o `zx81.exe`):

```bash
zx81.exe ZORLAC.P
```

O emulador vai abrir a janela e preparar o ficheiro para injeção.

Para carregar imediatamente o snapshot para RAM, usa:

- **`F12`** (força o carregamento/injeção do programa)

Depois do carregamento, o emulador tenta iniciar automaticamente o programa (Auto-RUN).

### 2) Notas importantes

- Se deres o nome do ficheiro em minúsculas (ex.: `zorlac.p`) também deve funcionar no Windows.
- Se executares `zx81.exe` sem argumentos, o `F12` tenta usar `PROGRAM.P` (se existir). Em geral, é mais simples passar sempre o nome do `.P` na linha de comandos.

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

## 🖱️ Controlos

- Teclado mapeado para o layout original do ZX81 (via SDL)
- Usa as teclas: letras, `SHIFT`, `RETURN`, `SPACE`, etc.
- O carregamento de `.P` nesta versão é feito via **linha de comandos + F12** (ver secção acima).

---

## 🧠 Sobre este projeto

Este emulador foi criado com foco em:

- **Fidelidade ao comportamento do ZX81 original**
- Código simples, direto e fácil de expandir
- Sem dependências externas além da SDL

---

## 🛠️ Próximos passos

- Melhorar o fluxo de carregamento via `LOAD` (BASIC)
- Melhoria na emulação de vídeo
- Otimizações de desempenho e timing

---

## ❓ Dúvidas ou sugestões?

Sente-te à vontade para contactar ou contribuir. Boas emulações! 🚀
