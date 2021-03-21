//
//
//	mc6850.cpp
//
//	(C) R.P.Bellis 1994
//

#include "mc6850.h"
#include "bits.h"

mc6850::mc6850()
	: irq(sr, 7, true)	// ~IRQ = SR bit 7
{
	reset();
}

mc6850::~mc6850()
{
}

void mc6850::reset()
{
	cr = 0;		// Clear all control flags
	sr = 0;		// Clear all status bits

	bset(sr, 1);	// Set TDRE to true
	cycles = 0;
}

void mc6850::tick(uint8_t ticks)
{
	cycles += ticks;
	if (cycles < 1000) return;

	cycles = 0;

	// Check for a received character if one isn't available
	if (!btst(sr, 0)) {
		Byte			ch;

		// If input is ready read a character
		if (term.poll()) {
			ch = term.read();
			rd = ch;

			// Check for IRQ
			if (btst(cr, 7)) {	// If CR7
				bset(sr, 7);	// Set IRQ
			}

			bset(sr, 0);		// Set RDRF
		}
	}
}

Byte mc6850::read(Word offset)
{
	// Now return the relevant value
	if (offset & 1) {
		bclr(sr, 0);		// Clear RDRF
		bclr(sr, 7);		// Clear IRQ
		return rd;
	} else {
		return sr;
	}
}

void mc6850::write(Word offset, Byte val)
{
	if (offset & 1) {
		bclr(sr, 7);		// Clear IRQ

		term.write(val);
		bset(sr, 1);		// Set TDRE to true (pretend it's sent)

		if (!btst(cr, 6) && btst(cr, 5)) {
			bset(sr, 7);	// Set IRQ
		}
	} else {
		cr = val;

		// Check for master reset
		if (btst(cr, 0) && btst(cr, 1)) {
			reset();
		}
	}
}
