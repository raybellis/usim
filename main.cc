//
//	main.cc
//

#include <cstdlib>
#include <cstdio>
#include <csignal>
#include <unistd.h>

#include "mc6809.h"
#include "mc6850.h"

class sys : virtual public mc6809 {

protected:

	virtual Byte			 read(Word);
	virtual void			 write(Word, Byte);

protected:

		mc6850			 uart;

} sys;

Byte sys::read(Word addr)
{
	Byte		ret = 0;

	if ((addr & 0xfffe) == 0xc000) {
		ret = uart.read(addr);
	} else {
		ret = mc6809::read(addr);
	}

	return ret;
}

void sys::write(Word addr, Byte x)
{
	if ((addr & 0xfffe) == 0xc000) {
		uart.write(addr, x);
	} else {
		mc6809::write(addr, x);
	}
}

#ifdef SIGALRM
void update(int)
{
	sys.status();
	(void)signal(SIGALRM, update);
	alarm(1);
}
#endif // SIGALRM

int main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "usage: usim <hexfile>\n");
		return EXIT_FAILURE;
	}

	(void)signal(SIGINT, SIG_IGN);
#ifdef SIGALRM
	(void)signal(SIGALRM, update);
	alarm(1);
#endif

	sys.load_intelhex(argv[1]);
	sys.run();

	return EXIT_SUCCESS;
}
