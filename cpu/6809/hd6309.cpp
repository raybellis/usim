//
// hd6309.cpp
// Hitachi HD6309 emulation: mc6809 superset.
// (C) R.P.Bellis 2026 -
// vim: ts=8 sw=8 noet:
//

#include <cstdio>
#include <algorithm>

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

Word& hd6309::wordrefreg(int r)
{
	switch (r) {
		case 6: return w;
		case 7: return v;
	}
	return mc6809::wordrefreg(r);
}

Byte& hd6309::byterefreg(int r)
{
	switch (r) {
		case 12: case 13:
			// 0 register: reads as 0, writes discarded
			zero_byte = 0;
			return zero_byte;
		case 14: return e;
		case 15: return f;
	}
	return mc6809::byterefreg(r);
}

// TFR/EXG postbyte size classification: the high bit of each 4-bit code
// distinguishes 16-bit registers (bit clear) from 8-bit (bit set).
static inline bool is_word_code(int r)
{
	return (r & 0x08) == 0;
}

void hd6309::tfr()
{
	insn = "TFR";
	Byte post = fetch_operand();
	int r1 = (post & 0xf0) >> 4;
	int r2 = (post & 0x0f);

	bool w1 = is_word_code(r1);
	bool w2 = is_word_code(r2);

	if (w1 && w2) {
		wordrefreg(r2) = wordrefreg(r1);
	} else if (!w1 && !w2) {
		byterefreg(r2) = byterefreg(r1);
	} else if (w1 && !w2) {
		// 16 -> 8: low byte of source
		byterefreg(r2) = (Byte)(wordrefreg(r1) & 0xff);
	} else {
		// 8 -> 16: high byte filled with $FF (per HD6309 datasheet)
		wordrefreg(r2) = 0xff00 | byterefreg(r1);
	}

	cycles += 4;
}

void hd6309::exg()
{
	insn = "EXG";
	Byte post = fetch_operand();
	int r1 = (post & 0xf0) >> 4;
	int r2 = (post & 0x0f);

	bool w1 = is_word_code(r1);
	bool w2 = is_word_code(r2);

	if (w1 && w2) {
		std::swap(wordrefreg(r1), wordrefreg(r2));
	} else if (!w1 && !w2) {
		std::swap(byterefreg(r1), byterefreg(r2));
	} else {
		// Mismatched-size EXG. Snapshot both operands as 16-bit values
		// (extending 8-bit ones per the TFR 8->16 rule) before writing,
		// then write each side back per its native size.
		Word v1 = w1 ? wordrefreg(r1) : (Word)(0xff00 | byterefreg(r1));
		Word v2 = w2 ? wordrefreg(r2) : (Word)(0xff00 | byterefreg(r2));
		if (w1) {
			wordrefreg(r1) = v2;
		} else {
			byterefreg(r1) = (Byte)(v2 & 0xff);
		}
		if (w2) {
			wordrefreg(r2) = v1;
		} else {
			byterefreg(r2) = (Byte)(v1 & 0xff);
		}
	}

	cycles += 8;
}
