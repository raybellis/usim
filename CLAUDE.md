# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build

```bash
make          # build libusim.a, usim09 (MC6809 simulator), usim02 (MOS 6502 simulator)
make clean
```

Compiles with C++20, `-Wall -Wextra -Werror`. There is no separate lint target; a clean build is the lint check.

## Testing

There is no automated test suite. Tests are ROM images run through the simulators:

```bash
./usim09 samples/test_main.hex    # MC6809 test ROM
./usim09 samples/tbasic09.hex     # TinyBASIC for MC6809
./usim02 samples/bbcbasic02.hex   # BBC BASIC v2 for MOS 6502
```

## Architecture

USIM is a C++20 library for composing 8-bit processor emulations. The design is built around three layers:

### Base CPU (`usim.h/cpp`)

`USim` is the abstract base class for all CPUs. It owns the memory-mapped device registry and provides byte/word read-write methods that dispatch to registered devices. Two subclasses handle endianness: `USimBE` (Motorola/big-endian, used by MC6809) and `USimLE` (Intel/little-endian, used by MOS 6502).

### Device Abstraction (`device.h`, `memory.h`, `wiring.h`)

- **`MappedDevice`** — anything with memory-mapped registers (read/write interface)
- **`ActiveDevice`** — anything that needs clock ticks (e.g., ACIA polling)
- **`ActiveMappedDevice`** — combines both
- `RAM`, `ROM`, `ROM_Data` are concrete `MappedDevice` implementations in `memory.h`
- `wiring.h` provides `InputPin`/`OutputPin`/`InputPort`/`OutputPort` templates for wiring CPU interrupt pins to peripheral IRQ signals; lambdas are the typical binding mechanism

### CPU Implementations

Each CPU splits across three files (e.g., `mc6809.h`, `mc6809.cpp`, `mc6809in.cpp`):
- `.h` — register model, mode enums, instruction declarations
- `.cpp` — reset, tick loop, interrupt handling, addressing mode helpers
- `in.cpp` — individual instruction implementations

**MC6809** (`mc6809.*`) — full register set (A, B, D, X, Y, U, S, DP, CC, PC), all addressing modes, NMI/FIRQ/IRQ/SWI interrupts. Big-endian. Inherits `USimBE`.

**MOS 6502** (`mos6502.*`) — full register set (A, X, Y, S, P, PC), all addressing modes, NMI/IRQ/BRK. Little-endian. Inherits `USimLE`.

### Peripheral: MC6850 ACIA (`mc6850.h/cpp`)

Serial I/O chip used by both demo systems. The concrete I/O backend is abstracted behind `mc6850_impl`; `term.h/cpp` provides a POSIX termios implementation.

### Demo System Layouts

| System | RAM | Peripheral | ROM |
|--------|-----|-----------|-----|
| `usim09` (MC6809) | `0x0000–0x7FFF` | ACIA @ `0xC000`, FIRQ wired to ACIA IRQ | `0xE000–0xFFFF` |
| `usim02` (MOS 6502) | `0x0000–0x7FFF` | ACIA @ `0xA000`, IRQ wired to ACIA IRQ | `0xC000–0xFFFF` |

`main09.cpp` and `main02.cpp` are the canonical examples of how to wire a complete system using `cpu.attach()` and pin lambdas.

### Utilities

- `bits.h` — `btst`, `bset`, `bclr`, sign-extend helpers
- `registers.h` — C++20 template for bit-field register access
- `typedefs.h` — `Byte` (uint8_t), `Word` (uint16_t), `DWord` (uint32_t)
