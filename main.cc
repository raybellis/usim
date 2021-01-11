//
//	main.cc
//

#include <cstdlib>
#include <cstdio>
#include <csignal>
#include <unistd.h>

#include "mc6809.h"
#include "mc6850.h"
#include "memory.h"

int main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "usage: usim <hexfile>\n");
		return EXIT_FAILURE;
	}

	(void)signal(SIGINT, SIG_IGN);

	mc6809	sys;
	RAM	ram(0x8000);
	RAM	rom(0x2000);
	mc6850	acia;

	sys.attach(ram, 0x0000, 0x8000);
	sys.attach(rom, 0xe000, 0xe000);
	sys.attach(acia, 0xc000, 0xfffe);

	sys.load_intelhex(argv[1]);
	sys.run();

	return EXIT_SUCCESS;
}
