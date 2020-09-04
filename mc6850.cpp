//
//
//	mc6850.cc
//
//	(C) R.P.Bellis 1994
//

#include "mc6850.h"

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
