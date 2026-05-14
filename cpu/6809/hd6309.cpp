//
// hd6309.cpp
// Hitachi HD6309 emulation: mc6809 superset.
// (C) R.P.Bellis 2026 -
// vim: ts=8 sw=8 noet:
//

#include <cstdio>

#include "hd6309.h"

hd6309::hd6309()
{
}

hd6309::~hd6309()
{
}

void hd6309::reset()
{
	w = 0;
	v = 0;
	md.value = 0;	// emulation mode after reset; no traps pending

	mc6809::reset();
}

void hd6309::print_regs()
{
	mc6809::print_regs();
	fprintf(stderr, " E:%02X F:%02X W:%04X V:%04X MD:%02X\r\n",
		e, f, w, v, md.value);
}
