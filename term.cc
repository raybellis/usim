//
//
//	term.cc
//
//	(C) R.P.Bellis 1994
//

#include <cstdlib>
#include "term.h"

//------------------------------------------------------------------------
// Machine dependent Terminal implementations
//------------------------------------------------------------------------
//------------------------------------------------------------------------
#if defined(__unix)

#include <unistd.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/time.h>
#include <cstdio>
#include <cstring>
#include <fcntl.h>

#ifdef sun
extern "C" int select(int, fd_set *, fd_set *, fd_set *,
		const struct timeval *);
#else
extern "C" int select(int, fd_set *, fd_set *, fd_set *, struct timeval *);
#endif

Terminal::Terminal(int in_fd, int out_fd) :
    input_fd(in_fd),
    output_fd(out_fd)
{
	// Get copies of current terminal attributes
	tcgetattr(input_fd, &oattr);
	tcgetattr(input_fd, &nattr);

	nattr.c_lflag &= ~ICANON;
	nattr.c_lflag &= ~ISIG;

	nattr.c_lflag &= ~ECHO;
	nattr.c_lflag |= ECHOE;

	nattr.c_iflag &= ~ICRNL;
	nattr.c_oflag &= ~ONLCR;

	tcsetattr(input_fd, TCSANOW, &nattr);
}

Terminal::~Terminal()
{
	tcsetattr(input_fd, TCSANOW, &oattr);
}

int Terminal::poll_in(void)
{
	fd_set			fds;

	// No delay in select(2) call
	// Used to be 1 ms, caused issues with fifos
	static struct timeval	tv = { 0L, 0 };

	FD_ZERO(&fds);
	FD_SET(input_fd, &fds);
	(void)select(input_fd + 1, &fds, NULL, NULL, &tv);

	return FD_ISSET(input_fd, &fds);
}
int Terminal::poll_out(void)
{
	fd_set			fds;

	// Uses zero delay in select(2) call to allow fluid output and minimal poll()
	static struct timeval	tv = { 0L, 0 };

	FD_ZERO(&fds);
	FD_SET(output_fd, &fds);
	(void)select(output_fd + 1, NULL, &fds, NULL, &tv);

	return FD_ISSET(output_fd, &fds);
}

int Terminal::poll(void) {
    return poll_in();
}

Byte Terminal::read()
{
    Byte c = 0;
    ::read(input_fd, &c, 1);
	return c;
}

void Terminal::write(Byte ch)
{
    ::write(output_fd, &ch, 1);
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

int Terminal::poll()
{
	return kbhit();
}

Byte Terminal::read()
{
	return (Byte)getch();
}

void Terminal::write(Byte ch)
{
	putch(ch);
}

#endif
//------------------------------------------------------------------------
