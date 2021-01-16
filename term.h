//
//
//	term.h
//
//	(C) R.P.Bellis 1994
//

#pragma once

#include <cstdio>
#include "typedefs.h"

#ifdef _POSIX_SOURCE
#include <termios.h>
#endif

class Terminal {

#ifdef _POSIX_SOURCE
	FILE			*input, *output;
	int			 input_fd;
	struct termios		 oattr, nattr;
#endif // _POSIX_SOURCE

public:

	int			 poll();
	void			 write(Byte);
	Byte			 read();

// Public constructor and destructor

				 Terminal();
				~Terminal();

};
