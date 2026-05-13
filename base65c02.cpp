//
// base65c02.cpp
// Common WDC 65C02 CMOS core emulation
// (C) R.P.Bellis 2026 -
// vim: ts=8 sw=8 noet:
//

#include "base65c02.h"

base65c02::base65c02()
{
}

base65c02::~base65c02()
{
}

mos6502::mode_t base65c02::decode_mode(Byte ir)
{
	switch (ir) {
	case 0x80:		// BRA
		return relative;
	case 0x7c:		// JMP (abs,X)
		return absxindirect;
	case 0x9c:		// STZ abs (NMOS decoder would say xindexed)
		return absolute;
	default:
		return mos6502::decode_mode(ir);
	}
}

void base65c02::execute_instruction()
{
	switch (ir) {
	case 0x80:
		bra(); break;
	case 0x7c:
		jmp(); break;
	case 0x64: case 0x74: case 0x9c: case 0x9e:
		stz(); break;
	default:
		mos6502::execute_instruction();
		break;
	}
}

Word base65c02::fetch_effective_address()
{
	switch (mode) {
	case absindirect:
		// CMOS removes the NMOS $xxFF page-wrap bug and spends one
		// additional internal cycle resolving the indirect address.
		operand = fetch_word();
		++cycles;
		return read_word(operand);
	case absxindirect:
		// CMOS-only addressing mode: one extra internal cycle for
		// the X-index addition.
		operand = fetch_word();
		++cycles;
		return read_word((Word)(operand + x));
	default:
		return mos6502::fetch_effective_address();
	}
}

const char* base65c02::disasm_opcode(Byte ir)
{
	switch (ir) {
	case 0x80:
		return "BRA";
	case 0x7c:
		return "JMP";
	case 0x64: case 0x74: case 0x9c: case 0x9e:
		return "STZ";
	default:
		return mos6502::disasm_opcode(ir);
	}
}

void base65c02::bra()
{
	do_br("BRA", true);
}

void base65c02::stz()
{
	auto m = fetch_effective_address();
	write(m, 0);
}
