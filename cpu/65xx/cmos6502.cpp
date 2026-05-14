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

	// CMOS-defined NOP slots. NMOS had undocumented opcodes here;
	// CMOS turns every unused encoding into a deterministic NOP of
	// the documented length and cycle count.
	//
	// 1-byte NOPs at the $x3 and $xB columns (32 opcodes; w65c02s
	// reuses $CB and $DB for WAI/STP, r65c02 reuses the $x7 and
	// $xF columns).
	case 0x03: case 0x13: case 0x23: case 0x33:
	case 0x43: case 0x53: case 0x63: case 0x73:
	case 0x83: case 0x93: case 0xa3: case 0xb3:
	case 0xc3: case 0xd3: case 0xe3: case 0xf3:
	case 0x0b: case 0x1b: case 0x2b: case 0x3b:
	case 0x4b: case 0x5b: case 0x6b: case 0x7b:
	case 0x8b: case 0x9b: case 0xab: case 0xbb:
	case 0xcb: case 0xdb: case 0xeb: case 0xfb:
	// 1-byte NOPs at the $x7 and $xF columns (overridden in r65c02
	// where they become RMB/SMB/BBR/BBS).
	case 0x07: case 0x17: case 0x27: case 0x37:
	case 0x47: case 0x57: case 0x67: case 0x77:
	case 0x87: case 0x97: case 0xa7: case 0xb7:
	case 0xc7: case 0xd7: case 0xe7: case 0xf7:
	case 0x0f: case 0x1f: case 0x2f: case 0x3f:
	case 0x4f: case 0x5f: case 0x6f: case 0x7f:
	case 0x8f: case 0x9f: case 0xaf: case 0xbf:
	case 0xcf: case 0xdf: case 0xef: case 0xff:
		nop(); break;

	// 2-byte NOPs (immediate-style operand).
	case 0x02: case 0x22: case 0x42: case 0x62:
	case 0x82: case 0xc2: case 0xe2:
	// 2-byte NOPs (zero-page style operand).
	case 0x44: case 0x54: case 0xd4: case 0xf4:
		fetch(); nop(); break;

	// 3-byte NOPs (absolute-style operand).
	case 0x5c: case 0xdc: case 0xfc:
		fetch_word(); nop(); break;

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
