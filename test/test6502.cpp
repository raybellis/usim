//
// test6502.cpp
// Functional test runner for the MOS 6502 emulator using
// Klaus Dormann's 6502 functional test (MIT licence).
//
// The test binary is a flat 64KB image loaded at 0x0000.
// Success is indicated by the CPU trapping at 0x3469.
// Any other trap address indicates a test failure.
//
// vim: ts=8 sw=8 noet:
//

#include <cstdio>
#include <cstdlib>

#include "mos6502.h"
#include "memory.h"

static constexpr Word success_addr = 0x3469;

class Test6502 : public mos6502 {
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
	const char *binfile = (argc == 2) ? argv[1] : "test/6502_functional_test.bin";

	Test6502 cpu;

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

	// the binary sets the reset vector to a trap to catch unexpected resets;
	// override it to point to the actual test entry point
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
