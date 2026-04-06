# gusgb

A Game Boy and Game Boy Color emulator written in C, using SDL2 for video, audio, and input. Includes `gbas`, a Game Boy assembler, and an `objdump` ROM disassembler.

## Dependencies

- SDL2
- SDL2_ttf (only required when building with the debugger)
- CMake (or GNU Make)

## Building

### CMake (recommended)

```
mkdir -p build
cd build
cmake ..
make
```

This produces three executables: `gusgb`, `gbas`, and `objdump`.

### Make (alternative)

```
make
```

Optional build flags:

| Flag | Effect |
|------|--------|
| `DEBUGGER=y` | Enable visual debugger (tile/BG map/palette viewers). Requires SDL2_ttf. |
| `CPU_DEBUG=y` | Enable CPU trace logging. |

Example: `make DEBUGGER=y`

## Usage

### Emulator

```
gusgb [options] <romfile>
```

| Option | Description |
|--------|-------------|
| `-s <scale>` | Scale video output (1-10, default 4) |
| `-f` | Start in fullscreen mode |
| `-c` | Print keyboard controls |
| `-h` | Print help |

#### Controls

| Key | Action |
|-----|--------|
| Arrow keys | D-pad |
| A | A button |
| S | B button |
| Enter | Start |
| Left Shift | Select |
| Alt + Enter | Toggle fullscreen |
| P | Pause (debugger builds only) |
| O | Dump CPU state (debugger builds only) |
| Esc / Q | Quit |

### Assembler

```
gbas -i <input.s> -o <output.gb> [-g]
```

The `-g` flag generates a valid cartridge header (zeroes 32KB, writes the entry point at `$100`, and places code at `$150`). Hex literals use the `$` prefix (e.g., `$FF44`). See `src/as/examples/` for sample programs.

### ROM disassembler

```
objdump <romfile>
```

## Tests

```
cd build
cmake ..
make
ctest
```

Two test suites: `cart_test` (cartridge/MBC3) and `cpu_test` (CPU instructions via the assembler).

## License

MIT
