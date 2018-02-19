//
//
//	mc6850.cc
//
//	(C) R.P.Bellis 1994
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mc6850.h"
#include "term.h"

mc6850::mc6850()
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
}

Byte mc6850::read(Word offset)
{
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
		bset(sr, 1);		// Set TDRE to true
		term.write(val);
	} else {
		cr = val;

		// Check for master reset
		if (btst(cr, 0) && btst(cr, 1)) {
			reset();
		}
	}
}

