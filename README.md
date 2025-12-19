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

## 🖱️ Controlos

- Teclado mapeado para o layout original do ZX81 (via SDL)
- Usa as teclas: letras, `SHIFT`, `RETURN`, `SPACE`, etc.
- Sem suporte a som ou ficheiros `.P` (por enquanto).

---

## 🧠 Sobre este projeto

Este emulador foi criado com foco em:

- **Fidelidade ao comportamento do ZX81 original**
- Código simples, direto e fácil de expandir
- Sem dependências externas além da SDL

---

## 🛠️ Próximos passos

- Suporte a ficheiros `.P` e `SAVE/LOAD`
- Melhoria na emulação de vídeo
- Otimizações de desempenho e timing

---

## ❓ Dúvidas ou sugestões?

Sente-te à vontade para contactar ou contribuir. Boas emulações! 🚀
