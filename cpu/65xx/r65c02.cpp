//
// r65c02.cpp
// Rockwell R65C02 emulation: cmos6502 + RMB/SMB/BBR/BBS.
// (C) R.P.Bellis 2026 -
// vim: ts=8 sw=8 noet:
//

#include "r65c02.h"

r65c02::r65c02()
{
}

r65c02::~r65c02()
{
}

mos6502::mode_t r65c02::decode_mode(Byte ir)
{
	// RMBn/SMBn: $07, $17, ..., $F7 — zero-page operand
	// BBRn/BBSn: $0F, $1F, ..., $FF — zero-page + signed offset
	switch (ir & 0x0f) {
	case 0x07:
		return zeropage;
	case 0x0f:
		return zprel;
	default:
		return cmos6502::decode_mode(ir);
	}
}

void r65c02::execute_instruction()
{
	switch (ir & 0x0f) {
	case 0x07:
		if (ir & 0x80) {
			smb((ir >> 4) & 0x07);
		} else {
			rmb((ir >> 4) & 0x07);
		}
		return;
	case 0x0f:
		if (ir & 0x80) {
			bbs((ir >> 4) & 0x07);
		} else {
			bbr((ir >> 4) & 0x07);
		}
		return;
	}
	cmos6502::execute_instruction();
}

Word r65c02::fetch_effective_address()
{
	if (mode == zprel) {
		// Two-byte operand: zero-page address then signed offset.
		Byte zp = fetch();
		Byte offset = fetch();
		operand = zp | ((Word)offset << 8);
		return zp & 0x00ff;
	}
	return cmos6502::fetch_effective_address();
}

const char* r65c02::disasm_opcode(Byte ir)
{
	static const char* rmb_names[8] = {
		"RMB0", "RMB1", "RMB2", "RMB3",
		"RMB4", "RMB5", "RMB6", "RMB7"
	};
	static const char* smb_names[8] = {
		"SMB0", "SMB1", "SMB2", "SMB3",
		"SMB4", "SMB5", "SMB6", "SMB7"
	};
	static const char* bbr_names[8] = {
		"BBR0", "BBR1", "BBR2", "BBR3",
		"BBR4", "BBR5", "BBR6", "BBR7"
	};
	static const char* bbs_names[8] = {
		"BBS0", "BBS1", "BBS2", "BBS3",
		"BBS4", "BBS5", "BBS6", "BBS7"
	};

	switch (ir & 0x0f) {
	case 0x07:
		return (ir & 0x80) ? smb_names[(ir >> 4) & 7]
				   : rmb_names[(ir >> 4) & 7];
	case 0x0f:
		return (ir & 0x80) ? bbs_names[(ir >> 4) & 7]
				   : bbr_names[(ir >> 4) & 7];
	default:
		return cmos6502::disasm_opcode(ir);
	}
}

void r65c02::rmb(Byte bit)
{
	auto m = fetch_effective_address();
	Byte val = read(m);
	write(m, val & ~(1 << bit));
	++cycles;
}

void r65c02::smb(Byte bit)
{
	auto m = fetch_effective_address();
	Byte val = read(m);
	write(m, val | (1 << bit));
	++cycles;
}

void r65c02::bbr(Byte bit)
{
	auto m = fetch_effective_address();
	Byte val = read(m);
	Byte offset = (Byte)(operand >> 8);
	bool taken = ((val & (1 << bit)) == 0);
	if (taken) {
		Word next = pc + extend8(offset);
		cycles += ((pc & 0xff00) != (next & 0xff00)) ? 2 : 1;
		pc = next;
	}
	++cycles;
}

void r65c02::bbs(Byte bit)
{
	auto m = fetch_effective_address();
	Byte val = read(m);
	Byte offset = (Byte)(operand >> 8);
	bool taken = ((val & (1 << bit)) != 0);
	if (taken) {
		Word next = pc + extend8(offset);
		cycles += ((pc & 0xff00) != (next & 0xff00)) ? 2 : 1;
		pc = next;
	}
	++cycles;
}
