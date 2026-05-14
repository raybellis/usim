# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build

```bash
make          # build libusim.a, usim09 (MC6809), usim02 (NMOS 6502)
make clean
```

Compiles with C++20, `-Wall -Wextra -Werror`. There is no separate lint target; a clean build is the lint check.

## Testing

```bash
make test       # run both CPU functional tests; exits 0 on PASS
```

Two automated tests:

- **MOS 6502** (`cpu/65xx/test6502.cpp`): runs Klaus Dormann's 6502 functional test. Loads a flat 64KB binary, runs until PC traps on itself, checks for success address `0x3469`. Exercises every legal opcode including BCD arithmetic.

- **MC6809** (`cpu/6809/test6809.cpp` + `cpu/6809/test6809.asm`): a custom MC6809 functional test suite. Assembled with `asm6809 -B`, loaded at `$0400`. Tests ALU, flags, shifts, EXG/TFR, PSHS/PULS, branches, BSR/JSR/RTS, RTI (E=0 and E=1 via SWI), SWI/SWI2/SWI3, MUL, DAA, SEX, ABX, LEA, and indexed addressing. Success address is `0x0986`.

`cmos6502` has no automated test yet. `tests/65C02_extended_opcodes_test.bin` (Klaus Dormann's 65C02 extended test, success PC `0x24F1`) is checked in for use once a concrete subclass adds RMB/SMB/BBR/BBS support.

Interactive ROM images (no automated pass/fail):

```bash
./usim09 samples/test_main.hex    # MC6809 test ROM
./usim09 samples/tbasic09.hex     # TinyBASIC for MC6809
./usim02 samples/bbcbasic02.hex   # BBC BASIC v2 for MOS 6502
```

## Architecture

USIM is a C++20 library for composing 8-bit processor emulations. The design is built around three layers:

### Base CPU (`core/usim.h/cpp`)

`USim` is the abstract base class for all CPUs. It owns the memory-mapped device registry and provides byte/word read-write methods that dispatch to registered devices. Two subclasses handle endianness: `USimBE` (Motorola/big-endian, used by MC6809) and `USimLE` (Intel/little-endian, used by MOS 6502).

### Device Abstraction (`core/device.h`, `core/memory.h`, `core/wiring.h`)

- **`MappedDevice`** — anything with memory-mapped registers (read/write interface)
- **`ActiveDevice`** — anything that needs clock ticks (e.g., ACIA polling)
- **`ActiveMappedDevice`** — combines both
- `RAM`, `ROM`, `ROM_Data` are concrete `MappedDevice` implementations in `core/memory.h`
- `core/wiring.h` provides `InputPin`/`OutputPin`/`InputPort`/`OutputPort` templates for wiring CPU interrupt pins to peripheral IRQ signals; lambdas are the typical binding mechanism

### CPU Implementations

Each CPU splits across three files (e.g., `cpu/6809/mc6809.h`, `cpu/6809/mc6809.cpp`, `cpu/6809/mc6809in.cpp`):
- `.h` — register model, mode enums, instruction declarations
- `.cpp` — reset, tick loop, interrupt handling, addressing mode helpers
- `in.cpp` — individual instruction implementations

**MC6809** (`cpu/6809/mc6809.*`) — full register set (A, B, D, X, Y, U, S, DP, CC, PC), all addressing modes, NMI/FIRQ/IRQ/SWI interrupts. Big-endian. Inherits `USimBE`.

**MOS 6502** (`cpu/65xx/mos6502.*`) — full register set (A, X, Y, S, P, PC), all addressing modes, NMI/IRQ/BRK. Little-endian. Inherits `USimLE`. Internals are protected/virtual so derived CMOS classes can extend dispatch and override the handful of instructions whose semantics differ.

**Common 65C02** (`cpu/65xx/cmos6502.*`) — derives from `mos6502` and adds the CMOS additions shared by all 65C02 variants: BRA, JMP (abs,X), CMOS-clean JMP (abs), STZ, PHX/PHY/PLX/PLY, INC A / DEC A, BIT immediate/`zp,X`/`abs,X` (with the immediate-mode flag quirk), `(zp)` addressing for the ALU ops, TRB/TSB, decimal-mode ADC/SBC with N/V/Z from the corrected result, and D-clear on interrupt entry. Vendor-specific extensions (RMB/SMB/BBR/BBS, WAI, STP) are explicitly **not** in this class — they belong on concrete subclasses (R65C02, W65C02S, etc.) that derive from `cmos6502`. Two CMOS-only addressing modes (`zpindirect`, `absxindirect`) were added to `mos6502::mode_t` so they're available to the derived class.

### Peripheral: MC6850 ACIA (`peripherals/mc6850.h/cpp`)

Serial I/O chip used by both demo systems. The concrete I/O backend is abstracted behind `mc6850_impl`; `peripherals/term.h/cpp` provides a POSIX termios implementation.

### Demo System Layouts

| System | RAM | Peripheral | ROM |
|--------|-----|-----------|-----|
| `usim09` (MC6809) | `0x0000–0x7FFF` | ACIA @ `0xC000`, FIRQ wired to ACIA IRQ | `0xE000–0xFFFF` |
| `usim02` (MOS 6502) | `0x0000–0x7FFF` | ACIA @ `0xA000`, IRQ wired to ACIA IRQ | `0xC000–0xFFFF` |

`cpu/6809/main09.cpp` and `cpu/65xx/main02.cpp` are the canonical examples of how to wire a complete system using `cpu.attach()` and pin lambdas.

### Utilities

- `core/bits.h` — `btst`, `bset`, `bclr`, sign-extend helpers
- `core/registers.h` — C++20 template for bit-field register access
- `core/typedefs.h` — `Byte` (uint8_t), `Word` (uint16_t), `DWord` (uint32_t)
