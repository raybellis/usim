//
//
//	mc6850.h
//
//	(C) R.P.Bellis 1994
//

#pragma once

#include "usim.h"
#include "misc.h"
#include "term.h"

class mc6850 : public Device {

// Internal registers

protected:

	Byte			td, rd, cr, sr;
	OutputPin		irq;

// Access to real IO device

	Word			tickcount;
	Terminal		term;

// Initialisation functions

protected:
	void			tick(void);
	void			reset(void);

// Read and write functions
public:

	virtual Byte		read(Word offset);
	virtual void		write(Word offset, Byte val);

// Public constructor and destructor

				mc6850();
	virtual			~mc6850();

};
