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
	case 0x12: case 0x32: case 0x52: case 0x72:	// ORA/AND/EOR/ADC (zp)
	case 0x92: case 0xb2: case 0xd2: case 0xf2:	// STA/LDA/CMP/SBC (zp)
		return zpindirect;
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
	case 0xda:
		phx(); break;
	case 0x5a:
		phy(); break;
	case 0xfa:
		plx(); break;
	case 0x7a:
		ply(); break;
	case 0x1a:
		ina(); break;
	case 0x3a:
		dea(); break;
	case 0x34: case 0x3c: case 0x89:
		bit(); break;
	case 0x12: ora(); break;
	case 0x32: and_(); break;
	case 0x52: eor(); break;
	case 0x72: adc(); break;
	case 0x92: sta(); break;
	case 0xb2: lda(); break;
	case 0xd2: cmp(); break;
	case 0xf2: sbc(); break;
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
	case 0xda:
		return "PHX";
	case 0x5a:
		return "PHY";
	case 0xfa:
		return "PLX";
	case 0x7a:
		return "PLY";
	case 0x1a:
		return "INC";
	case 0x3a:
		return "DEC";
	case 0x34: case 0x3c: case 0x89:
		return "BIT";
	case 0x12: return "ORA";
	case 0x32: return "AND";
	case 0x52: return "EOR";
	case 0x72: return "ADC";
	case 0x92: return "STA";
	case 0xb2: return "LDA";
	case 0xd2: return "CMP";
	case 0xf2: return "SBC";
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

void base65c02::phx()
{
	do_psh(x);
}

void base65c02::phy()
{
	do_psh(y);
}

void base65c02::plx()
{
	do_pul(x);
	set_nz(x);
}

void base65c02::ply()
{
	do_pul(y);
	set_nz(y);
}

void base65c02::ina()
{
	++a;
	set_nz(a);
}

void base65c02::dea()
{
	--a;
	set_nz(a);
}

void base65c02::bit()
{
	// CMOS BIT #imm sets only Z, leaving N and V untouched.
	// Every other addressing form (existing zp/abs and the new zp,X
	// and abs,X) is handled by the inherited memory-mode logic.
	if (mode == immediate) {
		Byte val = fetch_operand();
		p.z = ((a & val) == 0);
	} else {
		mos6502::bit();
	}
}
