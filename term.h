//
//
//	term.h
//
//	(C) R.P.Bellis 1994
//

#ifndef __term_h__
#define __term_h__

#include <cstdio>
#include "typedefs.h"

#if defined (__APPLE__) && !defined(__unix)
# define __unix 1
#endif

#ifdef __unix
#include <termios.h>
#endif

class Terminal {

#ifdef __unix
	int input_fd, output_fd;
	struct termios oattr, nattr;
#endif // __unix

public:
    int	poll(void);
    int poll_in(void);
    int poll_out(void);
	void write(Byte);
	Byte read(void);

// Public constructor and destructor

    Terminal(int in_fd = 0, int out_fd = 1);
    ~Terminal();

};

#endif // __term_h__
