//
//	main.cc
//

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include "mc6809_X.h"
#include "mc6850.h"

#ifdef __unix
#include <unistd.h>
#endif

#ifdef __osf__
extern "C" unsigned int alarm(unsigned int);
#endif

//#ifndef sun
//typedef void SIG_FUNC_TYP(int);
//typedef SIG_FUNC_TYPE *SIG_FP;
//#endif

class sys : virtual public mc6809_X {

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
		ret = mc6809_X::read(addr);
	}

	return ret;
}

void sys::write(Word addr, Byte x)
{
	if ((addr & 0xfffe) == 0xc000) {
		uart.write(addr, x);
	} else {
		mc6809_X::write(addr, x);
	}
}

#ifdef SIGALRM
#ifdef sun
void update(int, ...)
#else
void update(int)
#endif
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
