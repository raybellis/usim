//
// w65c02s.cpp
// WDC W65C02S emulation: r65c02 + WAI/STP.
// (C) R.P.Bellis 2026 -
// vim: ts=8 sw=8 noet:
//

#include "w65c02s.h"

w65c02s::w65c02s() : waiting(false), stopped(false)
{
}

w65c02s::~w65c02s()
{
}

void w65c02s::reset()
{
	waiting = false;
	stopped = false;
	mos6502::reset();
}

mos6502::mode_t w65c02s::decode_mode(Byte ir)
{
	switch (ir) {
	case 0xcb:	// WAI
	case 0xdb:	// STP
		return implied;
	default:
		return r65c02::decode_mode(ir);
	}
}

void w65c02s::execute_instruction()
{
	switch (ir) {
	case 0xcb:
		wai(); return;
	case 0xdb:
		stp(); return;
	}
	r65c02::execute_instruction();
}

const char* w65c02s::disasm_opcode(Byte ir)
{
	switch (ir) {
	case 0xcb:	return "WAI";
	case 0xdb:	return "STP";
	default:
		return r65c02::disasm_opcode(ir);
	}
}

void w65c02s::wai()
{
	waiting = true;
	// WAI is documented as 3 cycles. Initial opcode fetch already
	// accounts for one; add the remaining two.
	cycles += 2;
}

void w65c02s::stp()
{
	stopped = true;
	cycles += 2;
}

void w65c02s::tick()
{
	if (stopped) {
		// Only RESET wakes the CPU; just keep peripherals running.
		USim::tick();
		--cycles;
		return;
	}

	if (waiting) {
		USim::tick();

		bool c_nmi = NMI;
		bool c_irq = IRQ;
		bool nmi_triggered = !c_nmi && nmi_previous;
		nmi_previous = c_nmi;

		if (nmi_triggered) {
			waiting = false;
			do_nmi();
		} else if (!c_irq) {
			// Any IRQ wakes WAI; service it only if interrupts
			// are enabled, otherwise just resume after WAI.
			waiting = false;
			if (!p.i) {
				do_irq();
			}
		}
		--cycles;
		return;
	}

	mos6502::tick();
}
