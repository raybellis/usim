//
//	term.h
//	(C) R.P.Bellis 1994 - 2025
//
//	vim: ts=8
//

#pragma once

#include <cstdio>
#include "usim.h"
#include "mc6850.h"

#ifdef _POSIX_SOURCE
#include <termios.h>
#endif

class Terminal :  virtual public mc6850_impl {

protected:
	USim&			sys;
	Byte			read_data;
	bool			read_data_available = false;
	int			tilde_escape_phase = 0;

	void			tilde_escape_help();
	virtual void		tilde_escape_help_other();
	virtual void 		tilde_escape_do_other(char ch);
	bool			real_poll_read();
	Byte			real_read();

#ifdef _POSIX_SOURCE
	FILE*			input;
	FILE*			output;
	int			input_fd;
	struct termios		oattr, nattr;
#endif // _POSIX_SOURCE

public:

	virtual bool		poll_read();
	virtual void		write(Byte);
	virtual Byte		read();

public:
	virtual void		setup();
	virtual void		reset();

// Public constructor and destructor
public:
				Terminal(USim& sys);
	virtual			~Terminal();

};
