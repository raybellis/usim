//
//
//	term.h
//
//	(C) R.P.Bellis 1994
//

#ifndef __term_h__
#define __term_h__

#include <stdio.h>
#include "typedefs.h"

#ifdef __unix
#include <termios.h>
#endif

class Terminal {

#ifdef __unix
	FILE			*input, *output;
	int			 input_fd;
	struct termios		 oattr, nattr;
#endif // __unix

public:

	int			 poll(void);
	void			 write(Byte);
	Byte			 read(void);

// Public constructor and destructor

				 Terminal();
				~Terminal();

};

#endif // __term_h__
