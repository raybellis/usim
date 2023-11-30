//
//	main.cpp
//

#include <cstdlib>
#include <cstdio>
#include <csignal>
#include <unistd.h>

#include "mc6809.h"
#include "mc6850.h"
#include "term.h"
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

	mc6809			cpu;
	Terminal 		term(cpu);

	auto ram = std::make_shared<RAM>(ram_size);
	auto rom = std::make_shared<ROM>(rom_size);
	auto acia = std::make_shared<mc6850>(term);

	cpu.attach(ram, 0x0000, ~(ram_size - 1));
	cpu.attach(rom, rom_base, ~(rom_size - 1));
	cpu.attach(acia, 0xc000, 0xfffe);

	cpu.FIRQ.bind([&]() {
		return acia->IRQ;
	});

	rom->load_intelhex(argv[1], rom_base);

	cpu.reset();
	cpu.run();

	return EXIT_SUCCESS;
}
