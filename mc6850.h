//
//
//	mc6850.h
//
//	(C) R.P.Bellis 1994
//

#pragma once

#include "misc.h"

class mc6850 {

// Internal registers

protected:

	Byte			 td, rd, cr, sr;

// Access to real IO device

	Terminal		 term;

// Initialisation functions

protected:

	void			 reset(void);

// Read and write functions
public:

	Byte			 read(Word offset);
	void			 write(Word offset, Byte val);

// Public constructor and destructor

				 mc6850();
				~mc6850();

};
