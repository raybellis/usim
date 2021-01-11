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

	const Word ram_size = 0x8000;
	const Word rom_base = 0xe000;
	const Word rom_size = 0x10000 - rom_base;

	mc6809	sys;
	RAM	ram(ram_size);
	ROM	rom(rom_size);
	mc6850	acia;

	sys.attach(ram, 0x0000, ~(ram_size - 1));
	sys.attach(rom, rom_base, ~(rom_size - 1));
	sys.attach(acia, 0xc000, 0xfffe);

	rom.load_intelhex(argv[1], rom_base);

	sys.reset();
	sys.run();

	return EXIT_SUCCESS;
}
