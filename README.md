# Emulador Sinclair ZX81 (Windows)

Emulador do **Sinclair ZX81**, escrito em C, com CPU Z80 incluída e interface gráfica em **SDL 1.2**.

Esta versão carrega programas **`.P`** (snapshots de memória do ZX81) a partir da pasta `tapes/` e regista o processo de carregamento no ficheiro `tape_log.txt`.

Importante: nesta fase, o carregamento de programas é feito **pela linha de comandos** (argumento ao arrancar). O comando `LOAD "..."` no BASIC do ZX81 **não é o fluxo suportado**.

---

## Parte 1 — Utilizador Windows (rápido)

Se só queres **correr programas** (sem te preocupares com compilação), o fluxo é este.

Esta secção assume que estás a usar a **Linha de Comandos do Windows (cmd.exe)** (não PowerShell).

1) Copia os ficheiros `.P` para a pasta:

```text
tapes/
```

2) Abre a **Linha de Comandos (cmd)** na pasta do emulador.

Formas simples:

- No Explorador do Windows, abre a pasta do emulador e escreve `cmd` na barra de endereço, depois carrega Enter.
- Ou abre o “Prompt de Comando” e faz `cd` até à pasta do emulador.

Se preferires usar o PowerShell, vai para pasta onde está o ficheiro zx81.exe e faz (por exemplo, para carregar o simulador de voo):

./zx81.exe flight.p


3) Executa o emulador com o nome do programa (exemplo):

```bash
zx81 zorlac.p
```

Notas:

- O ficheiro deve estar na pasta `tapes/` (por exemplo `tapes\zorlac.p`).




## Parte 2 — Compilar com MSYS2 (Windows)

1) Instala o MSYS2: https://www.msys2.org/

2) Abre o terminal **“MSYS2 MinGW x64”** e instala as dependências:

```bash
pacman -S --needed mingw-w64-x86_64-toolchain mingw-w64-x86_64-SDL make
```

3) Na pasta do projeto:

Exemplo de caminho (ajusta ao teu):

```bash
cd /e/zx81emu/ZXC81-WIP-06
```

Compilar:

```bash
mingw32-make
```

4) Para correr depois de compilar:

- Podes correr a partir do **cmd.exe** (ver Parte 1), ou a partir do próprio terminal do MSYS2.
- Exemplo (MSYS2):

```bash
./zx81.exe zorlac.p
```

---

### Logs e diagnóstico

O emulador escreve um log em:

```text
tape_log.txt
```

Este ficheiro é útil para:

- confirmar que o `.P` foi aberto com sucesso
- confirmar que houve “injeção” para RAM

