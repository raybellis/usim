//
//
//	mc6850.h
//
//	(C) R.P.Bellis 1994
//

#pragma once

#include "device.h"
#include "wiring.h"
#include "term.h"

class mc6850 : virtual public ActiveMappedDevice {

// Internal registers

protected:

	Byte			td, rd, cr, sr;

// Access to real IO device

	Terminal		term;
	uint16_t		cycles;		// cycles since last terminal poll

// Initialisation functions

protected:
	virtual void		tick(uint8_t);
	virtual void		reset();

// Read and write functions
public:

	virtual Byte		read(Word offset);
	virtual void		write(Word offset, Byte val);

// Other exposed interfaces
public:
	OutputPinReg		IRQ;

// Public constructor and destructor

				mc6850();
	virtual			~mc6850();

};
