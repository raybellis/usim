//
// test6309.cpp
// Functional test runner for the HD6309 emulator.
//
// The test binary covers addresses $0400–$FFFF.
// It is loaded at $0400; the rest of RAM is zero.
// Success is indicated by the CPU trapping at the 'pass' label.
// Any other trap address indicates a test failure.
//
// vim: ts=8 sw=8 noet:
//

#include <cstdio>
#include <cstdlib>
#include <algorithm>

#include "hd6309.h"
#include "memory.h"

static constexpr Word success_addr = 0x07da;

static constexpr size_t HISTORY = 16;

class Test6309 : public hd6309 {
public:
	Word trap_pc = 0;
	Word history[HISTORY] = {};
	size_t hist_idx = 0;

protected:
	void post_exec() override {
		history[hist_idx++ % HISTORY] = insn_pc;
		if (pc == insn_pc) {
			// TFM (block transfer) deliberately rewinds PC after
			// each byte until W reaches zero. Don't mistake that
			// for a self-branch trap.
			if (ir >= 0x1138 && ir <= 0x113b) {
				return;
			}
			trap_pc = insn_pc;
			halt();
		}
	}
};

int main(int argc, char *argv[])
{
	const char *binfile = (argc == 2) ? argv[1] : "tests/test6309.bin";

	Test6309 cpu;

	// flat 64KB RAM
	auto ram = std::make_shared<RAM>(0x10000);
	cpu.attach(ram, 0x0000, 0x0000);

	// load binary at $0400
	{
		FILE *f = fopen(binfile, "rb");
		if (!f) {
			fprintf(stderr, "error: cannot open %s\n", binfile);
			return EXIT_FAILURE;
		}
		Word addr = 0x0400;
		int ch;
		while ((ch = fgetc(f)) != EOF) {
			cpu.write(addr++, (Byte)ch);
		}
		fclose(f);
	}

	cpu.reset();
	cpu.run();

	if (cpu.trap_pc == success_addr) {
		printf("PASS (PC=%04X)\n", cpu.trap_pc);
		return EXIT_SUCCESS;
	} else {
		printf("FAIL (PC=%04X)\n", cpu.trap_pc);
		cpu.print_regs();
		size_t n = std::min(cpu.hist_idx, HISTORY);
		size_t start = cpu.hist_idx - n;
		printf("Last %zu instructions:\n", n);
		for (size_t i = 0; i < n; ++i) {
			printf("  %04X\n", cpu.history[(start + i) % HISTORY]);
		}
		return EXIT_FAILURE;
	}
}
