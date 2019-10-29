//
//	main.cc
//

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include "mc6809_X.h"
#include "mc6850.h"
#include "aciadevice.h"
#include "sidforth.h"

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

#ifndef DEVICE
# define DEVICE aciadevice
#endif

class DEVICE sys;

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
		fprintf(stderr, "usage: usim <hexfile>\r\n");
		return EXIT_FAILURE;
	}

	(void)signal(SIGINT, SIG_IGN);
#ifdef SIGALRM
	(void)signal(SIGALRM, update);
	alarm(1);
#endif

	sys.load_intelmotorolahex(argv[1]);
	sys.run();

	return EXIT_SUCCESS;
}
