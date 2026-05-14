//
// cmos6502.cpp
// Common 65C02 CMOS core emulation
// (C) R.P.Bellis 2026 -
// vim: ts=8 sw=8 noet:
//

#include "cmos6502.h"

cmos6502::cmos6502()
{
}

cmos6502::~cmos6502()
{
}

mos6502::mode_t cmos6502::decode_mode(Byte ir)
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
	case 0x14:		// TRB zp (NMOS decoder would say zpxindexed)
		return zeropage;
	case 0x1c:		// TRB abs (NMOS decoder would say xindexed)
		return absolute;
	default:
		return mos6502::decode_mode(ir);
	}
}

void cmos6502::execute_instruction()
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
	case 0x14: case 0x1c:
		trb(); break;
	case 0x04: case 0x0c:
		tsb(); break;
	default:
		mos6502::execute_instruction();
		break;
	}
}

Word cmos6502::fetch_effective_address()
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

const char* cmos6502::disasm_opcode(Byte ir)
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
	case 0x14: case 0x1c:
		return "TRB";
	case 0x04: case 0x0c:
		return "TSB";
	default:
		return mos6502::disasm_opcode(ir);
	}
}

void cmos6502::bra()
{
	do_br("BRA", true);
}

void cmos6502::stz()
{
	auto m = fetch_effective_address();
	write(m, 0);
}

void cmos6502::phx()
{
	do_psh(x);
}

void cmos6502::phy()
{
	do_psh(y);
}

void cmos6502::plx()
{
	do_pul(x);
	set_nz(x);
}

void cmos6502::ply()
{
	do_pul(y);
	set_nz(y);
}

void cmos6502::ina()
{
	++a;
	set_nz(a);
}

void cmos6502::dea()
{
	--a;
	set_nz(a);
}

void cmos6502::bit()
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

void cmos6502::trb()
{
	auto m = fetch_effective_address();
	Byte val = read(m);
	p.z = ((a & val) == 0);
	write(m, val & ~a);
}

void cmos6502::tsb()
{
	auto m = fetch_effective_address();
	Byte val = read(m);
	p.z = ((a & val) == 0);
	write(m, val | a);
}

void cmos6502::adc()
{
	// Binary mode is identical to NMOS.
	if (!p.d) {
		mos6502::adc();
		return;
	}

	// CMOS decimal: N, V, Z come from the BCD-corrected result, and the
	// instruction takes one extra cycle.
	Byte val = fetch_operand();
	Byte oa = a;
	uint16_t c = p.c ? 1 : 0;

	uint16_t al = (a & 0x0f) + (val & 0x0f) + c;
	if (al > 9) al += 6;
	uint16_t ah = (a >> 4) + (val >> 4) + (al > 15 ? 1 : 0);
	if (ah > 9) ah += 6;
	p.c = (ah > 15);
	a = (Byte)(((ah & 0x0f) << 4) | (al & 0x0f));

	set_nz(a);
	p.v = (~((uint16_t)oa ^ (uint16_t)val) & ((uint16_t)oa ^ (uint16_t)a) & 0x80) != 0;
	++cycles;
}

void cmos6502::sbc()
{
	if (!p.d) {
		mos6502::sbc();
		return;
	}

	// CMOS decimal: N, V, Z come from the BCD-corrected result; C from
	// the binary subtraction; one extra cycle.
	Byte val = fetch_operand();
	Byte oa = a;
	uint16_t borrow = p.c ? 0 : 1;
	uint16_t diff = (uint16_t)a - (uint16_t)val - borrow;
	p.c = (diff < 0x100);

	int16_t al = (int16_t)(a & 0x0f) - (int16_t)(val & 0x0f) - (int16_t)borrow;
	if (al < 0) al = ((al - 6) & 0x0f) - 0x10;
	int16_t ah = (int16_t)(a >> 4) - (int16_t)(val >> 4) + (al >> 4);
	if (ah < 0) ah -= 6;
	a = (Byte)(((ah & 0x0f) << 4) | (al & 0x0f));

	set_nz(a);
	p.v = (((uint16_t)oa ^ (uint16_t)val) & ((uint16_t)oa ^ (uint16_t)diff) & 0x80) != 0;
	++cycles;
}

void cmos6502::do_nmi()
{
	mos6502::do_nmi();
	p.d = false;
}

void cmos6502::do_irq()
{
	mos6502::do_irq();
	p.d = false;
}

void cmos6502::do_brk()
{
	mos6502::do_brk();
	p.d = false;
}
