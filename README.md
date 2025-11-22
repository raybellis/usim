# USIM

A Motorola MC6809 emulator written in C++

NB: version 0.91 is historic (written 1993 - 1994) and had some serious
known bugs in the handling of some instructions and addressing modes.
Many thanks are due to B. Armstrong and Soren Roug for finding and
resolving those.

The master branch on the primary Github repository
(https://github.com/raybellis/usim/) now contains fixes for those
issues.

As of January 2021 (!) support for interrupts has been implemented, and
the code has been refactored so that complete machines are generated
by creating instances of a CPU, memory devices and peripherals and then
"attaching" those to each other.   See main.cpp for an example of how
this is done.
