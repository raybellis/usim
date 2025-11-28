#include "mos6502.h"

//
// refactored helper functions
//


void mos6502::set_nz(Byte val)
{
	p.n = (val & 0x80) != 0;
	p.z = (val == 0);
}

void mos6502::help_asl(Byte& val)
{
	p.c = (val & 0x80) != 0;
	val <<= 1;
	set_nz(val);
}

void mos6502::help_cmp(Byte reg, Byte val)
{
    uint16_t diff = (uint16_t)reg - (uint16_t)val;
    p.c = (reg >= val);
    Byte result = (Byte)(diff & 0xff);
    set_nz(result);

    // Overflow occurs if the sign bits of reg and val are different,
    // and the sign bit of the result is different from the sign bit of reg
    p.v = (((uint16_t)reg ^ (uint16_t)val) & ((uint16_t)reg ^ (uint16_t)result) & 0x80) != 0;
}

void mos6502::help_ld(Byte& reg)
{
	reg = fetch_operand();
	set_nz(reg);
}

void mos6502::help_lsr(Byte& val)
{
	p.c = (val & 0x01) != 0;
	val >>= 1;
	p.z = (val == 0);
	p.n = false; // LSR always clears the negative flag
}

void mos6502::help_rol(Byte &val)
{
	bool old_c = p.c;
	p.c = (val & 0x80) != 0;
	val = (val << 1) | (old_c ? 0x01 : 0x00);
	set_nz(val);
}

void mos6502::help_ror(Byte &val)
{
        bool old_c = p.c;
        p.c = (val & 0x01) != 0;
        val = (val >> 1) | (old_c ? 0x80 : 0x00);
        set_nz(val);
}

//
// mos6502 instruction implementations
//

void mos6502::adc()
{
	auto val = fetch_operand();
	uint16_t sum = (uint16_t)a + (uint16_t)val + (uint16_t)(p.c ? 1 : 0);
	p.c = (sum > 0xff);
	Byte result = (Byte)(sum & 0xff);
	set_nz(result);

	// Overflow occurs if the sign bits of a and val are the same,
	// and the sign bit of the result is different
	p.v = (~((uint16_t)a ^ (uint16_t)val) & ((uint16_t)a ^ (uint16_t)result) & 0x80) != 0;
	a = result;
}

void mos6502::and_()
{
	Byte val = fetch_operand();
	a = a & val;
	set_nz(a);
}

void mos6502::asl()
{
	if (mode == accumulator) {
		help_asl(a);
	} else {
		auto m = fetch_effective_address();
		Byte val = read(m);
		help_asl(val);
		write(m, val);
	}
}

void mos6502::bcc()
{
	do_br("BCC", !p.c);
}

void mos6502::bcs()
{
	do_br("BCS", p.c);
}

void mos6502::beq()
{
	do_br("BEQ", p.z);
}

void mos6502::bit()
{
	auto m = fetch_effective_address();
	Byte val = read(m);
	Byte result = a & val;
	p.z = (result == 0);
	p.n = (val & 0x80) != 0;
	p.v = (val & 0x40) != 0;
}

void mos6502::bmi()
{
	do_br("BMI", p.n);
}

void mos6502::bne()
{
	do_br("BNE", !p.z);
}

void mos6502::bpl()
{
	do_br("BPL", !p.n);
}

void mos6502::brk()
{
	// Placeholder for BRK instruction implementation
}

void mos6502::bvc()
{
	do_br("BVC", !p.v);
}

void mos6502::bvs()
{
	do_br("BVS", p.v);
}

void mos6502::clc()
{
	p.c = false;
}

void mos6502::cld()
{
	p.d = false;
}

void mos6502::cli()
{
	p.i = false;
}

void mos6502::clv()
{
	p.v = false;
}

void mos6502::cmp()
{
	help_cmp(a, fetch_operand());
}

void mos6502::cpx()
{
	help_cmp(x, fetch_operand());
}

void mos6502::cpy()
{
	help_cmp(y, fetch_operand());
}

void mos6502::dec()
{
	auto m = fetch_effective_address();
	Byte val = read(m);
	--val;
	set_nz(val);
	write(m, val);
}

void mos6502::dex()
{
	--x;
	set_nz(x);
}

void mos6502::dey()
{
	--y;
	set_nz(y);
}

void mos6502::eor()
{
	Byte val = fetch_operand();
	a = a ^ val;
	set_nz(a);
}

void mos6502::inc()
{
	auto m = fetch_effective_address();
	Byte val = read(m);
	++val;
	set_nz(val);
	write(m, val);
}

void mos6502::inx()
{
	++x;
	set_nz(x);
}

void mos6502::iny()
{
	++y;
	set_nz(y);
}

void mos6502::jmp()
{
	pc = fetch_effective_address();
}

void mos6502::jsr()
{
	Word addr = fetch_effective_address();
	do_psh((Word)(pc - 1));
	pc = addr;
}

void mos6502::lda()
{
	help_ld(a);
}

void mos6502::ldx()
{
	help_ld(x);
}

void mos6502::ldy()
{
	help_ld(y);
}

void mos6502::lsr()
{
	if (mode == accumulator) {
		help_lsr(a);
	} else {
		auto m  = fetch_effective_address();
		Byte val = read(m);
		help_lsr(val);
		write(m, val);
	}
}

void mos6502::nop()
{
}

void mos6502::ora()
{
	Byte val = fetch_operand();
	a = a | val;
	set_nz(a);
}

void mos6502::pha()
{
	do_psh(a);
}

void mos6502::php()
{
	do_psh(p.value);
}

void mos6502::pla()
{
	do_pul(a);
	set_nz(a);
}

void mos6502::plp()
{
	do_pul(p.value);
}

void mos6502::rol()
{
	if (mode == accumulator) {
		help_rol(a);
	} else {
		auto m = fetch_effective_address();
		Byte val = read(m);
		help_rol(val);
		write(m, val);
	}
}

void mos6502::ror()
{
	if (mode == accumulator) {
		help_ror(a);
	} else {
		auto m = fetch_effective_address();
		Byte val = read(m);
		help_ror(val);
		write(m, val);
	}
}

void mos6502::rti()
{
	do_pul(p.value);
	do_pul(pc);
}

void mos6502::rts()
{
	do_pul(pc);
	++pc;
}

void mos6502::sbc()
{
	auto val = fetch_operand();
	uint16_t diff = (uint16_t)a - (uint16_t)val - (uint16_t)(p.c ? 0 : 1);
	p.c = (diff < 0x100);
	Byte result = (Byte)(diff & 0xff);
	set_nz(result);
	// Overflow occurs if the sign bits of a and val are different,
	// and the sign bit of the result is different from the sign bit of a
	p.v = (((uint16_t)a ^ (uint16_t)val) & ((uint16_t)a ^ (uint16_t)result) & 0x80) != 0;
	a = result;
}

void mos6502::sec()
{
	p.c = true;
}

void mos6502::sed()
{
	p.d = true;
}

void mos6502::sei()
{
	p.i = true;
}

void mos6502::sta()
{
	auto m = fetch_effective_address();
	write(m, a);
}

void mos6502::stx()
{
	auto m = fetch_effective_address();
	write(m, x);
}

void mos6502::sty()
{
	auto m = fetch_effective_address();
	write(m, y);
}

void mos6502::tax()
{
	x = a;
	set_nz(x);
}

void mos6502::tay()
{
	y = a;
	set_nz(y);
}

void mos6502::txa()
{
	a = x;
	set_nz(a);
}

void mos6502::tya()
{
	a = y;
	set_nz(a);
}

void mos6502::tsx()
{
	x = s;
	set_nz(x);
}

void mos6502::txs()
{
	// flags are not affected
	s = x;
}
