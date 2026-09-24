//
// cycletest09.cpp
// MC6809 cycle-count regression test.
//
// Verifies the *hardware* cycle count -- the value each mc6809::tick()
// call hands to attached devices via USim::tick() -- against the MC6809
// data sheet (docs/MC6809.pdf), for the cases investigated in
// NOTES-cycle-counts-for-upstream.md: ABX, plus the already-fixed SUBD
// and 16-bit n,PCR indexed cases as regression coverage, plus a direct
// check that idle SYNC ticks charge exactly one cycle each.
//
// Because usim hands each instruction's cycles to devices on the *next*
// tick() call (mc6809.cpp: the final "--cycles" cancels the base cycle
// USim::tick() just added, and the delayed handoff makes up the
// difference), this harness records, for tick call k, both which
// instruction was fetched (via post_exec()) and what cycle count was
// handed to devices at the start of that call. The cost attributed to
// the instruction executed at position i is therefore the value
// recorded at position i+1.
//
// vim: ts=8 sw=8 noet:
//

#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

#include "mc6809.h"
#include "memory.h"

namespace {

class CycleRecorder : public ActiveDevice {
public:
	std::vector<uint8_t> costs;

	void reset() override { costs.clear(); }
	void tick(uint8_t cycles) override { costs.push_back(cycles); }
};

class CycleTest6809 : public mc6809 {
public:
	std::vector<std::string> insns;
	Word trap_pc = 0;

	void set_pc(Word p) { pc = p; }

protected:
	void post_exec() override {
		insns.push_back(insn ? insn : "?");
		if (pc == insn_pc) {
			trap_pc = insn_pc;
			halt();
		}
	}
};

struct Check {
	size_t index;		// position of the instruction in execution order
	const char *mnemonic;	// expected mnemonic, for a readable failure message
	int expected;		// expected cycle count per the data sheet
};

int run_sequence_test()
{
	CycleTest6809 cpu;
	auto recorder = std::make_shared<CycleRecorder>();
	auto ram = std::make_shared<RAM>(0x10000);

	cpu.attach(ram, 0x0000, 0x0000);
	cpu.attach(recorder);

	{
		FILE *f = fopen("tests/cycletest09.bin", "rb");
		if (!f) {
			fprintf(stderr, "error: cannot open tests/cycletest09.bin\n");
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
	cpu.set_pc(0x0400);
	cpu.run();

	// Execution order (see tests/cycletest09.asm):
	//   0 LDS#  1 NOP  2 LDA#  3 LDA dir  4 LDA ext  5 LDX#  6 LDB#
	//   7 ABX   8 BRA  9 BSR  10 RTS  11 JSR ext  12 RTS
	//   13 SUBD ext  14 LDA n16,PCR  15 BRA (trap)
	static const Check checks[] = {
		{ 2,  "LDA #",       2 },
		{ 3,  "LDA dir",     4 },
		{ 4,  "LDA ext",     5 },
		{ 7,  "ABX",         3 },
		{ 8,  "BRA taken",   3 },
		{ 9,  "BSR",         7 },
		{ 10, "RTS",         5 },
		{ 11, "JSR ext",     8 },
		{ 12, "RTS",         5 },
		{ 13, "SUBD ext",    7 },
		{ 14, "LDA n16,PCR", 9 },
	};

	if (cpu.insns.size() < 16 || recorder->costs.size() < 16) {
		printf("FAIL: expected 16 executed instructions, got %zu insns / %zu cost samples\n",
			cpu.insns.size(), recorder->costs.size());
		return EXIT_FAILURE;
	}

	bool ok = true;
	for (const auto& c : checks) {
		int actual = recorder->costs[c.index + 1];
		if (actual != c.expected) {
			printf("FAIL: %-12s (insns[%zu]=%s) expected %d cycles, got %d\n",
				c.mnemonic, c.index, cpu.insns[c.index].c_str(),
				c.expected, actual);
			ok = false;
		} else {
			printf("PASS: %-12s %d cycles\n", c.mnemonic, actual);
		}
	}

	return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}

int run_idle_test()
{
	// Directly probe SYNC idle-tick charging without needing an
	// assembled fixture: a single SYNC opcode ($13) is enough, and
	// with IRQ/FIRQ/NMI left unbound (permanently deasserted, see
	// wiring.h's default_true()), the CPU stays in waiting_sync for
	// as many ticks as we choose to run.
	CycleTest6809 cpu;
	auto recorder = std::make_shared<CycleRecorder>();
	auto ram = std::make_shared<RAM>(0x10000);

	cpu.attach(ram, 0x0000, 0x0000);
	cpu.attach(recorder);

	cpu.write(0x0400, 0x13);	// SYNC

	cpu.reset();
	cpu.set_pc(0x0400);

	cpu.tick();			// executes SYNC, enters waiting_sync
	cpu.tick();			// flushes SYNC's own cost (delayed handoff); not an idle tick

	static constexpr int idle_ticks = 5;
	for (int i = 0; i < idle_ticks; ++i) {
		cpu.tick();
	}

	bool ok = true;
	size_t n = recorder->costs.size();
	for (size_t i = n - idle_ticks; i < n; ++i) {
		if (recorder->costs[i] != 1) {
			printf("FAIL: idle SYNC tick[%zu] expected 1 cycle, got %u\n",
				i, recorder->costs[i]);
			ok = false;
		}
	}
	if (ok) {
		printf("PASS: %d idle SYNC ticks each charged 1 cycle\n", idle_ticks);
	}

	return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}

} // namespace

int main()
{
	int seq_result = run_sequence_test();
	int idle_result = run_idle_test();

	return (seq_result == EXIT_SUCCESS && idle_result == EXIT_SUCCESS)
		? EXIT_SUCCESS : EXIT_FAILURE;
}
