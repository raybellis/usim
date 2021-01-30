//
//	mc6809ins.cpp
//
//	(C) R.P.Bellis
//
//	Updated from BDA and Soren Roug 12/2003 primarily
//	with fixes for SBC / CMP / NEG instructions with
//	incorrect carry flag settings
//

#include <utility>
#include "mc6809.h"

//-- helper functions

void mc6809::help_adc(Byte& x)
{
	Byte	m = fetch_operand();

	{
		Byte	t = (x & 0x0f) + (m & 0x0f) + cc.bit.c;
		cc.bit.h = btst(t, 4);		// Half carry
	}

	{
		Byte	t = (x & 0x7f) + (m & 0x7f) + cc.bit.c;
		cc.bit.v = btst(t, 7);		// Bit 7 carry in
	}

	{
		Word	t = x + m + cc.bit.c;
		cc.bit.c = btst(t, 8);		// Bit 7 carry out
		x = t & 0xff;
	}

	cc.bit.v ^= cc.bit.c;
	cc.bit.n = btst(x, 7);
	cc.bit.z = !x;
}

void mc6809::help_add(Byte& x)
{
	Byte	m = fetch_operand();

	{
		Byte	t = (x & 0x0f) + (m & 0x0f);
		cc.bit.h = btst(t, 4);		// Half carry
	}

	{
		Byte	t = (x & 0x7f) + (m & 0x7f);
		cc.bit.v = btst(t, 7);		// Bit 7 carry in
	}

	{
		Word	t = x + m;
		cc.bit.c = btst(t, 8);		// Bit 7 carry out
		x = t & 0xff;
	}

	cc.bit.v ^= cc.bit.c;
	cc.bit.n = btst(x, 7);
	cc.bit.z = !x;
}

void mc6809::help_and(Byte& x)
{
	x = x & fetch_operand();
	cc.bit.n = btst(x, 7);
	cc.bit.z = !x;
	cc.bit.v = 0;
}

void mc6809::help_asr(Byte& x)
{
	cc.bit.c = btst(x, 0);
	x >>= 1;	/* Shift word right */
	if ((cc.bit.n = btst(x, 6)) != 0) {
		bset(x, 7);
	}
	cc.bit.z = !x;
	++cycles;
}

void mc6809::help_bit(Byte x)
{
	Byte t = x & fetch_operand();
	cc.bit.n = btst(t, 7);
	cc.bit.v = 0;
	cc.bit.z = !t;
}

void mc6809::help_clr(Byte& x)
{
	cc.all &= 0xf0;
	cc.all |= 0x04;
	x = 0;
	++cycles;
}

void mc6809::help_cmp(Byte x)
{
	Byte	m = fetch_operand();
	int	t = x - m;

	cc.bit.v = btst((Byte)(x ^ m ^ t ^ (t >> 1)), 7);
	cc.bit.c = btst((Word)t, 8);
	cc.bit.n = btst((Byte)t, 7);
	cc.bit.z = !(t & 0xff);
}

void mc6809::help_cmp(Word x)
{
	Word	m = fetch_word_operand();
	long	t = x - m;

	cc.bit.v = btst((DWord)(x ^ m ^ t ^ (t >> 1)), 15);
	cc.bit.c = btst((DWord)t, 16);
	cc.bit.n = btst((DWord)t, 15);
	cc.bit.z = !(t & 0xffff);
	++cycles;
}

void mc6809::help_com(Byte& x)
{
	x = ~x;
	cc.bit.c = 1;
	cc.bit.v = 0;
	cc.bit.n = btst(x, 7);
	cc.bit.z = !x;
	++cycles;
}

void mc6809::help_dec(Byte& x)
{
	cc.bit.v = (x == 0x80);
	x = x - 1;
	cc.bit.n = btst(x, 7);
	cc.bit.z = !x;
	++cycles;
}

void mc6809::help_eor(Byte& x)
{
	x = x ^ fetch_operand();
	cc.bit.v = 0;
	cc.bit.n = btst(x, 7);
	cc.bit.z = !x;
}

void mc6809::help_inc(Byte& x)
{
	cc.bit.v = (x == 0x7f);
	x = x + 1;
	cc.bit.n = btst(x, 7);
	cc.bit.z = !x;
	++cycles;
}

void mc6809::help_ld(Byte& x)
{
	x = fetch_operand();
	cc.bit.n = btst(x, 7);
	cc.bit.v = 0;
	cc.bit.z = !x;
}

void mc6809::help_ld(Word& x)
{
	x = fetch_word_operand();
	cc.bit.n = btst(x, 15);
	cc.bit.v = 0;
	cc.bit.z = !x;
}

void mc6809::help_lsl(Byte& x)
{
	cc.bit.c = btst(x, 7);
	cc.bit.v = btst(x, 7) ^ btst(x, 6);
	x <<= 1;
	cc.bit.n = btst(x, 7);
	cc.bit.z = !x;
	++cycles;
}

void mc6809::help_lsr(Byte& x)
{
	cc.bit.c = btst(x, 0);
	x >>= 1;	/* Shift word right */
	cc.bit.n = 0;
	cc.bit.z = !x;
	++cycles;
}

void mc6809::help_neg(Byte& x)
{
	int	t = 0 - x;

	cc.bit.v = btst((Byte)(x ^ t ^ (t >> 1)), 7);
	cc.bit.c = btst((Word)t, 8);
	cc.bit.n = btst((Byte)t, 7);
	x = t & 0xff;
	cc.bit.z = !x;
	++cycles;
}

void mc6809::help_or(Byte& x)
{
	x = x | fetch_operand();
	cc.bit.v = 0;
	cc.bit.n = btst(x, 7);
	cc.bit.z = !x;
}

void mc6809::help_psh(Byte w, Word& s, Word& u)
{
	if (btst(w, 7)) do_psh(s, pc);
	if (btst(w, 6)) do_psh(s, u);
	if (btst(w, 5)) do_psh(s, y);
	if (btst(w, 4)) do_psh(s, x);
	if (btst(w, 3)) do_psh(s, dp);
	if (btst(w, 2)) do_psh(s, b);
	if (btst(w, 1)) do_psh(s, a);
	if (btst(w, 0)) do_psh(s, cc.all);
}

void mc6809::help_pul(Byte w, Word& s, Word& u)
{
	if (btst(w, 0)) do_pul(s, cc.all);
	if (btst(w, 1)) do_pul(s, a);
	if (btst(w, 2)) do_pul(s, b);
	if (btst(w, 3)) do_pul(s, dp);
	if (btst(w, 4)) do_pul(s, x);
	if (btst(w, 5)) do_pul(s, y);
	if (btst(w, 6)) do_pul(s, u);
	if (btst(w, 7)) do_pul(s, pc);
}

void mc6809::help_rol(Byte& x)
{
	int	oc = cc.bit.c;
	cc.bit.v = btst(x, 7) ^ btst(x, 6);
	cc.bit.c = btst(x, 7);
	x = x << 1;
	if (oc) bset(x, 0);
	cc.bit.n = btst(x, 7);
	cc.bit.z = !x;
	++cycles;
}

void mc6809::help_ror(Byte& x)
{
	int	oc = cc.bit.c;
	cc.bit.c = btst(x, 0);
	x = x >> 1;
	if (oc) bset(x, 7);
	cc.bit.n = btst(x, 7);
	cc.bit.z = !x;
	++cycles;
}

void mc6809::help_sbc(Byte& x)
{
	Byte    m = fetch_operand();
	int t = x - m - cc.bit.c;

	cc.bit.v = btst((Byte)(x ^ m ^ t ^ (t >> 1)), 7);
	cc.bit.c = btst((Word)t, 8);
	cc.bit.n = btst((Byte)t, 7);
	x = t & 0xff;
	cc.bit.z = !x;
}

void mc6809::help_st(Byte x)
{
	Word	addr = fetch_effective_address();
	write(addr, x);
	cc.bit.v = 0;
	cc.bit.n = btst(x, 7);
	cc.bit.z = !x;
}

void mc6809::help_st(Word x)
{
	Word	addr = fetch_effective_address();
	write_word(addr, x);
	cc.bit.v = 0;
	cc.bit.n = btst(x, 15);
	cc.bit.z = !x;
}

void mc6809::help_sub(Byte& x)
{
	Byte    m = fetch_operand();
	int t = x - m;

	cc.bit.v = btst((Byte)(x^m^t^(t>>1)),7);
	cc.bit.c = btst((Word)t,8);
	cc.bit.n = btst((Byte)t, 7);
	x = t & 0xff;
	cc.bit.z = !x;
}

void mc6809::help_tst(Byte x)
{
	cc.bit.v = 0;
	cc.bit.n = btst(x, 7);
	cc.bit.z = !x;
	++cycles;
}

//-- individual instructions

void mc6809::abx()
{
	insn = "ABX";
	x += b;
	++cycles;
}

void mc6809::adca()
{
	insn = "ADCA";
	help_adc(a);
}

void mc6809::adcb()
{
	insn = "ADCB";
	help_adc(b);
}

void mc6809::adda()
{
	insn = "ADDA";
	help_add(a);
}

void mc6809::addb()
{
	insn = "ADDB";
	help_add(b);
}

void mc6809::addd()
{
	insn = "ADDD";
	Word	m = fetch_word_operand();

	{
		Word	t = (d & 0x7fff) + (m & 0x7fff);
		cc.bit.v = btst(t, 15);
	}

	{
		DWord	t = (DWord)d + m;
		cc.bit.c = btst(t, 16);
		d = (Word)(t & 0xffff);
	}

	cc.bit.v ^= cc.bit.c;
	cc.bit.n = btst(d, 15);
	cc.bit.z = !d;
	++cycles;
}

void mc6809::anda()
{
	insn = "ANDA";
	help_and(a);
}

void mc6809::andb()
{
	insn = "ANDB";
	help_and(b);
}

void mc6809::andcc()
{
	insn = "ANDCC";
	cc.all &= fetch_operand();
	++cycles;
}

void mc6809::asra()
{
	insn = "ASRA";
	help_asr(a);
}

void mc6809::asrb()
{
	insn = "ASRB";
	help_asr(b);
}

void mc6809::asr()
{
	insn = "ASR";
	Word	addr = fetch_effective_address();
	Byte	m = read(addr);

	help_asr(m);
	write(addr, m);
}

void mc6809::bcc()
{
	insn = "BCC";
	do_br("cc", !cc.bit.c);
}

void mc6809::lbcc()
{
	insn = "LBCC";
	do_lbr("cc", !cc.bit.c);
}

void mc6809::bcs()
{
	insn = "BCS";
	do_br("cs", cc.bit.c);
}

void mc6809::lbcs()
{
	insn = "LBCS";
	do_lbr("cs", cc.bit.c);
}

void mc6809::beq()
{
	insn = "BEQ";
	do_br("eq", cc.bit.z);
}

void mc6809::lbeq()
{
	insn = "LBEQ";
	do_lbr("eq", cc.bit.z);
}

void mc6809::bge()
{
	insn = "BGE";
	do_br("ge", !(cc.bit.n ^ cc.bit.v));
}

void mc6809::lbge()
{
	insn = "LGBE";
	do_lbr("ge", !(cc.bit.n ^ cc.bit.v));
}

void mc6809::bgt()
{
	insn = "BGT";
	do_br("gt", !(cc.bit.z | (cc.bit.n ^ cc.bit.v)));
}

void mc6809::lbgt()
{
	insn = "LBGT";
	do_lbr("gt", !(cc.bit.z | (cc.bit.n ^ cc.bit.v)));
}

void mc6809::bhi()
{
	insn = "BHI";
	do_br("hi", !(cc.bit.c | cc.bit.z));
}

void mc6809::lbhi()
{
	insn = "LBHI";
	do_lbr("hi", !(cc.bit.c | cc.bit.z));
}

void mc6809::bita()
{
	insn = "BITA";
	help_bit(a);
}

void mc6809::bitb()
{
	insn = "BITB";
	help_bit(b);
}

void mc6809::ble()
{
	insn = "BLE";
	do_br("le", cc.bit.z | (cc.bit.n ^ cc.bit.v));
}

void mc6809::lble()
{
	insn = "LBLE";
	do_lbr("le", cc.bit.z | (cc.bit.n ^ cc.bit.v));
}

void mc6809::bls()
{
	insn = "BLS";
	do_br("ls", cc.bit.c | cc.bit.z);
}

void mc6809::lbls()
{
	insn = "LBLS";
	do_lbr("ls", cc.bit.c | cc.bit.z);
}

void mc6809::blt()
{
	insn = "BLT";
	do_br("lt", cc.bit.n ^ cc.bit.v);
}

void mc6809::lblt()
{
	insn = "LBLT";
	do_lbr("lt", cc.bit.n ^ cc.bit.v);
}

void mc6809::bmi()
{
	insn = "BMI";
	do_br("mi", cc.bit.n);
}

void mc6809::lbmi()
{
	insn = "LBMI";
	do_lbr("mi", cc.bit.n);
}

void mc6809::bne()
{
	insn = "BNE";
	do_br("ne", !cc.bit.z);
}

void mc6809::lbne()
{
	insn = "LBNE";
	do_lbr("ne", !cc.bit.z);
}

void mc6809::bpl()
{
	insn = "BPL";
	do_br("pl", !cc.bit.n);
}

void mc6809::lbpl()
{
	insn = "LBPL";
	do_lbr("pl", !cc.bit.n);
}

void mc6809::bra()
{
	insn = "BRA";
	do_br("ra", 1);
}

void mc6809::lbra()
{
	insn = "LBRA";
	do_lbr("ra", 1);
}

void mc6809::brn()
{
	insn = "BRN";
	do_br("rn", 0);
}

void mc6809::lbrn()
{
	insn = "LBRN";
	do_lbr("rn", 0);
}

void mc6809::bsr()
{
	insn = "BSR";
	Byte	x = fetch_operand();
	do_psh(s, pc);
	pc += extend8(x);
	cycles += 3;
}

void mc6809::lbsr()
{
	insn = "LBSR";
	Word	x = fetch_word_operand();
	do_psh(s, pc);
	pc += x;
	cycles += 4;
}

void mc6809::bvc()
{
	insn = "BVC";
	do_br("vc", !cc.bit.v);
}

void mc6809::lbvc()
{
	insn = "LBVC";
	do_lbr("vc", !cc.bit.v);
}

void mc6809::bvs()
{
	insn = "BVS";
	do_br("vs", cc.bit.v);
}

void mc6809::lbvs()
{
	insn = "LBVS";
	do_lbr("vs", cc.bit.v);
}

void mc6809::clra()
{
	insn = "CLRA";
	help_clr(a);
}

void mc6809::clrb()
{
	insn = "CLRB";
	help_clr(b);
}

void mc6809::clr()
{
	insn = "CLR";
	Word	addr = fetch_effective_address();
	Byte	m = read(addr);
	help_clr(m);
	write(addr, m);
}

void mc6809::cmpa()
{
	insn = "CMPA";
	help_cmp(a);
}

void mc6809::cmpb()
{
	insn = "CMPB";
	help_cmp(b);
}

void mc6809::cmpd()
{
	insn = "CMPD";
	help_cmp(d);
}

void mc6809::cmpx()
{
	insn = "CMPX";
	help_cmp(x);
}

void mc6809::cmpy()
{
	insn = "CMPY";
	help_cmp(y);
}

void mc6809::cmpu()
{
	insn = "CMPU";
	help_cmp(u);
}

void mc6809::cmps()
{
	insn = "CMPS";
	help_cmp(s);
}

void mc6809::cwai()
{
	insn = "CWAI";
	Byte	n = fetch_operand();
	cc.all &= n;
	cc.bit.e = 1;
	help_psh(0xff, s, u);
	cycles += 2;
	waiting_cwai = true;
}

void mc6809::coma()
{
	insn = "COMA";
	help_com(a);
}

void mc6809::comb()
{
	insn = "COMB";
	help_com(b);
}

void mc6809::com()
{
	insn = "COM";
	Word	addr = fetch_effective_address();
	Byte	m = read(addr);
	help_com(m);
	write(addr, m);
}

void mc6809::daa()
{
	insn = "DAA";
	Byte	c = 0;
	Byte	lsn = (a & 0x0f);
	Byte	msn = (a & 0xf0) >> 4;

	if (cc.bit.h || (lsn > 9)) {
		c |= 0x06;
	}

	if (cc.bit.c ||
	    (msn > 9) ||
	    ((msn > 8) && (lsn > 9))) {
		c |= 0x60;
	}

	{
		Word	t = (Word)a + c;
		cc.bit.c |= btst(t, 8);
		a = (Byte)t;
	}

	cc.bit.n = btst(a, 7);
	cc.bit.z = !a;
	++cycles;
}

void mc6809::deca()
{
	insn = "DECA";
	help_dec(a);
}

void mc6809::decb()
{
	insn = "DECB";
	help_dec(b);
}

void mc6809::dec()
{
	insn = "DEC";
	Word	addr = fetch_effective_address();
	Byte	m = read(addr);
	help_dec(m);
	write(addr, m);
}

void mc6809::eora()
{
	insn = "EORA";
	help_eor(a);
}

void mc6809::eorb()
{
	insn = "EORB";
	help_eor(b);
}

static const char* regnames[] = {
  "D", "X", "Y", "U", "S", "PC", "", "", "",
  "A", "B", "CC", "DP", "", "", "", ""
};

void mc6809::exg()
{
	insn = "EXG";
	Byte	w = fetch_operand();
	int r1 = (w & 0xf0) >> 4;
	int r2 = (w & 0x0f) >> 0;
	if (r1 <= 5 && r2 <= 5) {
		std::swap(wordrefreg(r2), wordrefreg(r1));
	} else if (r1 >= 8 && r1 <= 11 && r2 >= 8 && r2 <= 11) {
		std::swap(byterefreg(r2), byterefreg(r1));
	} else {
		throw execution_error("invalid EXG operand");
	}

	if (m_trace) {
		op = std::string(regnames[r1]) + "," + std::string(regnames[r2]);
	}

	cycles += 6;
}

void mc6809::inca()
{
	insn = "INCA";
	help_inc(a);
}

void mc6809::incb()
{
	insn = "INCB";
	help_inc(b);
}

void mc6809::inc()
{
	insn = "INC";
	Word	addr = fetch_effective_address();
	Byte	m = read(addr);
	help_inc(m);
	write(addr, m);
}

void mc6809::jmp()
{
	insn = "JMP";
	pc = fetch_effective_address();
}

void mc6809::jsr()
{
	insn = "JSR";
	Word	addr = fetch_effective_address();
	do_psh(s, pc);
	pc = addr;
	cycles += 2;
}

void mc6809::lda()
{
	insn = "LDA";
	help_ld(a);
}

void mc6809::ldb()
{
	insn = "LDB";
	help_ld(b);
}

void mc6809::ldd()
{
	insn = "LDD";
	help_ld(d);
}

void mc6809::ldx()
{
	insn = "LDX";
	help_ld(x);
}

void mc6809::ldy()
{
	insn = "LDY";
	help_ld(y);
}

void mc6809::lds()
{
	insn = "LDS";
	help_ld(s);
}

void mc6809::ldu()
{
	insn = "LDU";
	help_ld(u);
}

void mc6809::leax()
{
	insn = "LEAX";
	x = fetch_effective_address();
	cc.bit.z = !x;
	++cycles;
}

void mc6809::leay()
{
	insn = "LEAY";
	y = fetch_effective_address();
	cc.bit.z = !y;
	++cycles;
}

void mc6809::leas()
{
	insn = "LEAS";
	s = fetch_effective_address();
	++cycles;
}

void mc6809::leau()
{
	insn = "LEAU";
	u = fetch_effective_address();
	++cycles;
}

void mc6809::lsla()
{
	insn = "LSLA";
	help_lsl(a);
}

void mc6809::lslb()
{
	insn = "LSLB";
	help_lsl(b);
}

void mc6809::lsl()
{
	insn = "LSL";
	Word	addr = fetch_effective_address();
	Byte	m = read(addr);
	help_lsl(m);
	write(addr, m);
}

void mc6809::lsra()
{
	insn = "LSRA";
	help_lsr(a);
}

void mc6809::lsrb()
{
	insn = "LSRB";
	help_lsr(b);
}

void mc6809::lsr()
{
	insn = "LSR";
	Word	addr = fetch_effective_address();
	Byte	m = read(addr);
	help_lsr(m);
	write(addr, m);
}

void mc6809::mul()
{
	insn = "MUL";
	d = a * b;
	cc.bit.c = btst(b, 7);
	cc.bit.z = !d;
	cycles += 10;
}

void mc6809::nega()
{
	insn = "NEGA";
	help_neg(a);
}

void mc6809::negb()
{
	insn = "NEGB";
	help_neg(b);
}

void mc6809::neg()
{
	insn = "NEG";
	Word 	addr = fetch_effective_address();
	Byte	m = read(addr);
	help_neg(m);
	write(addr, m);
}

void mc6809::nop()
{
	insn = "NOP";
	++cycles;
}

void mc6809::ora()
{
	insn = "ORA";
	help_or(a);
}

void mc6809::orb()
{
	insn = "ORB";
	help_or(b);
}

void mc6809::orcc()
{
	insn = "ORCC";
	cc.all |= fetch_operand();
	++cycles;
}

void mc6809::pshs()
{
	insn = "PSHS";
	help_psh(fetch_operand(), s, u);
	cycles += 3;
}

void mc6809::pshu()
{
	insn = "PSHU";
	help_psh(fetch_operand(), u, s);
	cycles += 3;
}

void mc6809::puls()
{
	insn = "PULS";
	Byte	w = fetch_operand();
	help_pul(w, s, u);
	cycles += 3;
}

void mc6809::pulu()
{
	insn = "PULU";
	Byte	w = fetch_operand();
	help_pul(w, u, s);
	cycles += 3;
}

void mc6809::rola()
{
	insn = "ROLA";
	help_rol(a);
}

void mc6809::rolb()
{
	insn = "ROLB";
	help_rol(b);
}

void mc6809::rol()
{
	insn = "ROL";
	Word	addr = fetch_effective_address();
	Byte	m = read(addr);
	help_rol(m);
	write(addr, m);
}

void mc6809::rora()
{
	insn = "RORA";
	help_ror(a);
}

void mc6809::rorb()
{
	insn = "RORB";
	help_ror(b);
}

void mc6809::ror()
{
	insn = "ROR";
	Word	addr = fetch_effective_address();
	Byte	m = read(addr);
	help_ror(m);
	write(addr, m);
}

void mc6809::rti()
{
	insn = "RTI";
	help_pul(0x01, s, u);
	if (cc.bit.e) {
		help_pul(0xfe, s, u);
	} else {
		help_pul(0x80, s, u);
	}
	cycles += 2;
}

void mc6809::rts()
{
	insn = "RTS";
	do_pul(s, pc);
	cycles += 2;
}

void mc6809::sbca()
{
	insn = "SBCA";
	help_sbc(a);
}

void mc6809::sbcb()
{
	insn = "SBCB";
	help_sbc(b);
}

void mc6809::sex()
{
	insn = "SEX";
	cc.bit.n = btst(b, 7);
	cc.bit.z = !b;
	a = cc.bit.n ? 255 : 0;
	++cycles;
}

void mc6809::sta()
{
	insn = "STA";
	help_st(a);
}

void mc6809::stb()
{
	insn = "STB";
	help_st(b);
}

void mc6809::std()
{
	insn = "STD";
	help_st(d);
}

void mc6809::stx()
{
	insn = "STX";
	help_st(x);
}

void mc6809::sty()
{
	insn = "STY";
	help_st(y);
}

void mc6809::sts()
{
	insn = "STS";
	help_st(s);
}

void mc6809::stu()
{
	insn = "STU";
	help_st(u);
}

void mc6809::suba()
{
	insn = "SUBA";
	help_sub(a);
}

void mc6809::subb()
{
	insn = "SUBB";
	help_sub(b);
}

void mc6809::subd()
{
	insn = "SUBD";
	Word    m = fetch_word_operand();
	int t = d - m;

	cc.bit.v = btst((DWord)(d ^ m ^ t ^(t >> 1)), 15);
	cc.bit.c = btst((DWord)t, 16);
	cc.bit.n = btst((DWord)t, 15);
	d = t & 0xffff;
	cc.bit.z = !d;
}

void mc6809::swi()
{
	insn = "SWI";
	cc.bit.e = 1;
	help_psh(0xff, s, u);
	cc.bit.f = cc.bit.i = 1;
	pc = read_word(0xfffa);
	cycles += 4;
}

void mc6809::swi2()
{
	insn = "SWI2";
	cc.bit.e = 1;
	help_psh(0xff, s, u);
	pc = read_word(0xfff4);
	cycles += 4;
}

void mc6809::swi3()
{
	insn = "SWI3";
	cc.bit.e = 1;
	help_psh(0xff, s, u);
	pc = read_word(0xfff2);
	cycles += 4;
}

void mc6809::sync()
{
	insn = "SYNC";
	waiting_sync = true;
	++cycles;
}

void mc6809::tfr()
{
	insn = "TFR";
	Byte	w = fetch_operand();
	int r1 = (w & 0xf0) >> 4;
	int r2 = (w & 0x0f) >> 0;
	if (r1 <= 5 && r2 <= 5) {
		wordrefreg(r2) = wordrefreg(r1);
	} else if (r1 >= 8 && r1 <= 11 && r2 >= 8 && r2 <= 11) {
		byterefreg(r2) = byterefreg(r1);
	} else {
		throw execution_error("invalid TFR operand");
	}

	if (m_trace) {
		op = std::string(regnames[r1]) + "," + std::string(regnames[r2]);
	}

	cycles += 4;
}

void mc6809::tsta()
{
	insn = "TSTA";
	help_tst(a);
}

void mc6809::tstb()
{
	insn = "TSTB";
	help_tst(b);
}

void mc6809::tst()
{
	insn = "TST";
	Word	addr = fetch_effective_address();
	Byte	m = read(addr);
	help_tst(m);
	++cycles;
}
