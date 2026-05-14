//
// test65c02.cpp
// Functional test runner for the Rockwell R65C02 emulator using
// Klaus Dormann's 65C02 extended opcodes test (MIT licence).
//
// The test binary is a flat 64KB image. Success is indicated by the
// CPU trapping at 0x24F1. Any other trap address indicates a failure.
//
// vim: ts=8 sw=8 noet:
//

#include <cstdio>
#include <cstdlib>

#include "r65c02.h"
#include "memory.h"

static constexpr Word success_addr = 0x24f1;

class Test65C02 : public r65c02 {
public:
	Word trap_pc = 0;

protected:
	void post_exec() override {
		if (pc == insn_pc) {
			trap_pc = insn_pc;
			halt();
		}
	}
};

int main(int argc, char *argv[])
{
	const char *binfile = (argc == 2) ? argv[1] : "tests/65C02_extended_opcodes_test.bin";

	Test65C02 cpu;

	// flat 64KB RAM covers entire address space
	auto ram = std::make_shared<RAM>(0x10000);
	cpu.attach(ram, 0x0000, 0x0000);

	// load flat binary at address 0x0000
	{
		FILE *f = fopen(binfile, "rb");
		if (!f) {
			fprintf(stderr, "error: cannot open %s\n", binfile);
			return EXIT_FAILURE;
		}
		Word addr = 0;
		int ch;
		while ((ch = fgetc(f)) != EOF) {
			cpu.write(addr++, (Byte)ch);
		}
		fclose(f);
	}

	// override reset vector to point at the standard test entry (0x0400)
	cpu.write(0xfffc, 0x00);
	cpu.write(0xfffd, 0x04);

	cpu.reset();
	cpu.run();

	if (cpu.trap_pc == success_addr) {
		printf("PASS (PC=%04X)\n", cpu.trap_pc);
		return EXIT_SUCCESS;
	} else {
		printf("FAIL (PC=%04X)\n", cpu.trap_pc);
		return EXIT_FAILURE;
	}
}
