//
//	main02.cpp
//	(C) R.P.Bellis 2025 -
//
//	vim: ts=8 sw=8 noet:
//
//	Example of a simple MOS 6502 system with an MC6850 ACIA at
//	address 0xa000, 32kB of RAM at 0x0000 and 16kB of ROM at 0xe000
//

#include <cstdlib>
#include <cstdio>
#include <csignal>

#include "mos6502.h"
#include "mc6850.h"
#include "term.h"
#include "memory.h"

int main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "usage: usim02 <hexfile>\n");
		return EXIT_FAILURE;
	}

	(void)signal(SIGINT, SIG_IGN);

	const Word ram_size = 0x8000;
	const Word rom_base = 0xc000;
	const Word rom_size = 0x10000 - rom_base;

	mos6502			cpu;
	Terminal 		term(cpu);

	auto ram = std::make_shared<RAM>(ram_size);
	auto rom = std::make_shared<ROM>(rom_size);
	auto acia = std::make_shared<mc6850>(term);

	cpu.attach(ram, 0x0000, ~(ram_size - 1));
	cpu.attach(rom, rom_base, ~(rom_size - 1));
	cpu.attach(acia, 0xa000, 0xfffe);

	cpu.IRQ.bind([&]() {
		return acia->IRQ;
	});

	rom->load_intelhex(argv[1], rom_base);

	cpu.reset();
	// cpu.tron();
	cpu.run();

	return EXIT_SUCCESS;
}
