# USIM

A framework for emulating 8-bit processors, written in C++.

The currently supported processors are:
- Motorola MC6809
- MOS 6502 (NMOS version)

The primary repository for the code is
https://github.com/raybellis/usim/.

The code requires a C++20 compatible compiler.

## History

This code was first started in 1993 and was specifically for the Motorola
6809 processor.  The initial released versions had some serious known bugs
in the handling of some 6809 instructions and addressing modes.  Many
thanks are due to B. Armstrong and Soren Roug for finding and resolving
those.

As of January 2021 support for interrupts was implemented, and
the code was reorganised so that complete machines are generated
by creating instances of a CPU, memory devices and peripherals and then
"attaching" those to each other.  See main.cpp for an example of how
this is done.

(In earler versions it was necessary to create a subclass of the CPU
object and overload the `read` and `write` functions to direct memory
and I/O accesses accordingly).

While this framework was always intended to be extensible for other CPU
types, this didn't happen until December 2025 when MOS 6502 support was
added.
