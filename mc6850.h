//
//
//	mc6850.h
//
//	(C) R.P.Bellis 1994
//

#pragma once

#include "device.h"
#include "term.h"

class mc6850 : public Device {

// Internal registers

protected:

	Byte			td, rd, cr, sr;

// Access to real IO device

	uint64_t		next_poll;
	Terminal		term;

// Initialisation functions

protected:
	virtual void		tick(uint64_t);
	virtual void		reset();
	bool			irq_pin_status() const;

// Read and write functions
public:

	virtual Byte		read(Word offset);
	virtual void		write(Word offset, Byte val);

// Other exposed interfaces
public:
	OutputPin		irq;

// Public constructor and destructor

				mc6850();
	virtual			~mc6850();

};
