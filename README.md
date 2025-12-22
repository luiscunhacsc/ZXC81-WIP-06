# Sinclair ZX81 Emulator (Windows)

Emulator for the **Sinclair ZX81**, written in C, with an included Z80 CPU core and a graphical interface using **SDL 1.2**.

This version loads **`.P`** programs (ZX81 memory snapshots) from the `tapes/` folder and logs the loading process to `tape_log.txt`.

Important: at this stage, program loading is done **via the command line** (as a startup argument). The `LOAD "..."` command in ZX81 BASIC **is not the supported flow**.

---

## Part 1 — Windows user (quick)

If you only want to **run programs** (without worrying about compilation), follow this flow.

This section assumes you are using the **Windows Command Prompt (cmd.exe)** (not PowerShell).

1) Copy the `.P` files into this folder:

```text
tapes/
```

2) Open the **Command Prompt (cmd)** in the emulator folder.

Easy ways:

- In Windows Explorer, open the emulator folder and type `cmd` in the address bar, then press Enter.
- Or open “Command Prompt” and `cd` to the emulator folder.

3) Run the emulator with the program name (example):

```bash
zx81 zorlac.p
```

If you prefer using PowerShell, go to the folder where `zx81.exe` is located and run (for example, to load the flight simulator):

```bash
./zx81.exe flight.p
```

Notes:

- The file must be inside the `tapes/` folder (for example `tapes\zorlac.p`).

---

## Part 2 — Build with MSYS2 (Windows)

1) Install MSYS2: https://www.msys2.org/

2) Open the **“MSYS2 MinGW x64”** terminal and install the dependencies:

```bash
pacman -S --needed mingw-w64-x86_64-toolchain mingw-w64-x86_64-SDL make
```

3) In the project folder:

Example path (adjust to yours):

```bash
cd /e/zx81emu/ZXC81-WIP-06
```

Build:

```bash
mingw32-make
```

4) To run after building:

- You can run it from **cmd.exe** (see Part 1), or from the MSYS2 terminal itself.
- Example (MSYS2):

```bash
./zx81.exe zorlac.p
```

---

### Logs and troubleshooting

The emulator writes a log to:

```text
tape_log.txt
```

This file is useful to:

- confirm that the `.P` file was opened successfully
- confirm that RAM “injection” occurred
