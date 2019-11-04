//
//	main.cc
//

#include <cstdlib>
#include <cstdio>
#include <csignal>
#include "mc6809_X.h"
#include "mc6809.h"
#include "mc6850.h"
#include "uartdevice.h"
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
# define DEVICE uartdevice
#endif

class DEVICE *sys;

#ifndef DEBUG
#ifdef SIGALRM
#ifdef sun
void update(int, ...)
#else
void update(int)
#endif
{
    if (sys) {
        sys->status();
        (void)signal(SIGALRM, update);
        alarm(1);
    }
}
#endif // SIGALRM
#endif

void killed(int sig) {
    fprintf(stderr, "\r\nKilled with %d\r\n", sig);
    if (sys) {
        sys->halt();
    } else {
        exit (EXIT_FAILURE);
    }
}

int main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "usage: usim <hexfile>\r\n");
		return EXIT_FAILURE;
	}
    sys = new DEVICE();
	(void)signal(SIGINT, killed);
    (void)signal(SIGTERM, killed);
    (void)signal(SIGHUP, killed);
#ifndef DEBUG
#ifdef SIGALRM
	(void)signal(SIGALRM, update);
	alarm(1);
#endif
#endif
    if (sys) {
        sys->load_intelmotorolahex(argv[1]);
        sys->run();
        delete sys;
        sys = NULL;
        return EXIT_SUCCESS;
    } else {
        fprintf(stderr, "Failed to create device\n");
        return EXIT_FAILURE;
    }
}
