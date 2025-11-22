//
//	term.cpp
//	(C) R.P.Bellis 1994 - 2025
//
//      vim: ts=8 sw=8 noet:
//

#include <cstdlib>
#include <cassert>
#include "term.h"

//------------------------------------------------------------------------
// Machine dependent Terminal implementations
//------------------------------------------------------------------------

//------------------------------------------------------------------------
#if defined(_POSIX_SOURCE)

#include <cstdio>
#include <unistd.h>
#include <ncurses.h>
#include <termios.h>
#include <sys/select.h>
#include <sys/types.h>
#include <string.h>

Terminal::Terminal(USim& sys) : sys(sys)
{
	input = stdin;
	output = stdout;
	input_fd = fileno(input);

	// Set input and output to be unbuffered
	setbuf(input, (char *)0);
	setbuf(output, (char *)0);

	// Get copies of startup terminal attributes
	tcgetattr(input_fd, &oattr);
	tcgetattr(input_fd, &nattr);

	setup();
}

Terminal::~Terminal()
{
	reset();
}

void Terminal::setup()
{
	nattr.c_lflag &= ~ICANON;
	nattr.c_lflag &= ~ISIG;

	nattr.c_lflag &= ~ECHO;
	nattr.c_lflag |= ECHOE;

	nattr.c_iflag &= ~ICRNL;
	nattr.c_oflag &= ~ONLCR;

	tcsetattr(input_fd, TCSANOW, &nattr);
}

void Terminal::reset()
{
	tcsetattr(input_fd, TCSANOW, &oattr);
}

bool Terminal::real_poll_read()
{
	fd_set			fds;

	// Uses minimal (10us) delay in select(2) call to
	// ensure that idling simulations don't chew
	// up masses of CPU time
	static struct timeval	tv = { 0L, 10L };

	FD_ZERO(&fds);
	FD_SET(input_fd, &fds);
	(void)select(FD_SETSIZE, &fds, NULL, NULL, &tv);

	return FD_ISSET(input_fd, &fds);
}

Byte Terminal::real_read()
{
	return fgetc(input);
}

void Terminal::write(Byte ch)
{
	fputc(ch, output);
}

//------------------------------------------------------------------------
#elif defined(__MSDOS__) || defined(__BORLANDC__)

#include <conio.h>

Terminal::Terminal()
{
}

Terminal::~Terminal()
{
}

bool Terminal::real_poll_read()
{
	return kbhit();
}

Byte Terminal::real_read()
{
	return getch();
}

void Terminal::write(Byte ch)
{
	putch(ch);
}

void Terminal::setup()
{
}

void Terminal::reset()
{
}

#endif
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// Machine independent part, handles ssh-like tilde-escapes
//------------------------------------------------------------------------

void Terminal::tilde_escape_help()
{
	fprintf(stderr, "\r\nSupported escape sequences:\r\n");
	fprintf(stderr, " ~. - terminate emulator\r\n");
	fprintf(stderr, " ~? - this message\r\n");
	fprintf(stderr, " ~~ - send the escape character by typing it twice\r\n");
	tilde_escape_help_other();
	fprintf(stderr, "(Note that escapes are only recognized immediately after newline.)\r\n");
}

bool Terminal::poll_read()
{
	if (read_data_available) {
		return true;
	}

	bool ready = real_poll_read();
	if (!ready) {
		read_data_available = false;
		return read_data_available;
	}

	Byte ch = read_data = real_read();
	read_data_available = true;

	switch (tilde_state) {
		case normal:
			if (ch == 0x0a || ch == 0x0d) {
				tilde_state = looking;
			}
			break;

		case looking:
			if (ch == '~') {
				tilde_state = handling;
				read_data_available = false;
			} else {
				tilde_state = normal;
			}
			break;

		case handling:
			tilde_state = normal;
			read_data_available = false;

			switch (ch) {
				case '~':
					read_data_available = true;
					break;
				case '.':
					sys.halt();
					break;
				case '?':
					tilde_escape_help();
					break;
				default:
					tilde_escape_do_other(ch);
					break;
			}
			break;

		default:
			assert(false);
			break;
	}

	return read_data_available;
}

void Terminal::tilde_escape_do_other(char ch)
{
	(void)ch;
}

void Terminal::tilde_escape_help_other()
{
}

Byte Terminal::read()
{
	read_data_available = false;
	return read_data;
}
