# --------------------------------------------------
# Makefile – ZX-81 + novo core Z80
# --------------------------------------------------

SRCS   = zx81.c z80.c zx81_tape.c
TARGET = zx81.exe

# SDL 1.2 build notes (Windows/MinGW):
# - sdl-config is often a shell script that requires sh, which may not be present
#   when building from PowerShell.
# - Override SDL_CFLAGS/SDL_LIBS if your setup needs custom include/lib paths.
SDL_CFLAGS ?=
SDL_LIBS   ?= -lmingw32 -lSDLmain -lSDL

CFLAGS = -std=c99 -Wall -O2

# alvo principal: compila o emulador
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SDL_CFLAGS) -o $@ $(SRCS) $(SDL_LIBS)

.PHONY: run clean

# compila (se necessário) e executa o emulador
run: $(TARGET)
	.\\$(TARGET)

# apaga o executável e ficheiros temporários
clean:
	-del /f /q $(TARGET) 2>NUL
	-del /f /q zx81 2>NUL
