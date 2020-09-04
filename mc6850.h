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

// Initialisation functions

protected:

	void			 reset(void);

// Public constructor and destructor

				 mc6850();
				~mc6850();

};
