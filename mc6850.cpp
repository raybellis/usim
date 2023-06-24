//
//
//	mc6850.cpp
//
//	(C) R.P.Bellis 1994
//

#include "mc6850.h"
#include "bits.h"

mc6850::mc6850(mc6850_impl& impl, uint16_t interval)
	: impl(impl),
	  interval(interval),
	  IRQ(sr, 7, true)	// ~IRQ = SR bit 7
{
	reset();
}

mc6850::~mc6850()
{
}

void mc6850::reset()
{
	cr = 0;		// Clear all control flags
	sr = TDRE;	// Clear all status bits except TDRE
	cycles = 0;
}

void mc6850::tick(uint8_t ticks)
{
	if ((sr & IRQB) == 0) {
		if (((sr & TDRE) && ((cr & 0x60) == 0x20)) ||
			((sr & RDRF) && ((cr & 0x80) == 0x80)))
		{
			sr |= IRQB;
		}
	}

	cycles += ticks;
	if (cycles < interval) return;
	cycles = 0;

	// Check for a received character if one isn't available
	if ((sr & RDRF) == 0) {
		// If input is ready read a character
		if (impl.poll_read()) {
			rd = impl.read();
			sr |= RDRF;
		}
	}

	if ((sr & TDRE) == 0) {
		impl.write(td);
		sr |= TDRE;
	}
}

Byte mc6850::read(Word offset)
{
	switch (offset & 1) {
		case 0: // status register
			return sr;
			break;
		case 1:	// read data
		default:
			sr &= ~(RDRF | IRQB);
			return rd;
			break;
	}
}

void mc6850::write(Word offset, Byte val)
{
	switch (offset & 1) {
		case 0:	// control register
			cr = val;
			if ((cr & 0x03) == 0x03) {
				reset();
			}
			break;
		case 1:	// data register
			td = val;
			sr &= ~(IRQB | TDRE);
			break;
	}
}
