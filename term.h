//
//
//	term.h
//
//	(C) R.P.Bellis 1994
//

#pragma once

#include <functional>
#include <cstdio>
#include "mc6850.h"

#ifdef _POSIX_SOURCE
#include <termios.h>
#endif

class Terminal :  virtual public mc6850_impl {

protected:
	Byte				read_data;
	bool				read_data_available = false;
	int					tilde_escape_phase = 0;

	void				tilde_escape_help();
	bool				real_poll_read();
	Byte				real_read();

#ifdef _POSIX_SOURCE
	FILE*				input;
	FILE*				output;
	int			 		input_fd;
	struct termios		oattr, nattr;
#endif // _POSIX_SOURCE

public:

	virtual bool		poll_read();
	virtual void		write(Byte);
	virtual Byte		read();


// Public constructor and destructor
public:
						 Terminal();
	virtual				~Terminal();

};
