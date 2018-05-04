//
//	mc6809ins.cc
//
//	(C) R.P.Bellis
//
//	Updated from BDA and Soren Roug 12/2003 primarily
//	with fixes for SBC / CMP / NEG instructions with
//	incorrect carry flag settings
//

#include <stdio.h>
#include "usim.h"
#include "mc6809.h"

void mc6809::abx(void)
{
	x += b;
}

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

void mc6809::adca(void)
{
	help_adc(a);
}

void mc6809::adcb(void)
{
	help_adc(b);
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

void mc6809::adda(void)
{
	help_add(a);
}

void mc6809::addb(void)
{
	help_add(b);
}

void mc6809::addd(void)
{
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
}

void mc6809::help_and(Byte& x)
{
	x = x & fetch_operand();
	cc.bit.n = btst(x, 7);
	cc.bit.z = !x;
	cc.bit.v = 0;
}

void mc6809::anda(void)
{
	help_and(a);
}

void mc6809::andb(void)
{
	help_and(b);
}

void mc6809::andcc(void)
{
	cc.all &= fetch();
}

void mc6809::help_asr(Byte& x)
{
	cc.bit.c = btst(x, 0);
	x >>= 1;	/* Shift word right */
	if ((cc.bit.n = btst(x, 6)) != 0) {
		bset(x, 7);
	}
	cc.bit.z = !x;
}

void mc6809::asra(void)
{
	help_asr(a);
}

void mc6809::asrb(void)
{
	help_asr(b);
}

void mc6809::asr(void)
{
	Word	addr = fetch_effective_address();
	Byte	m = read(addr);

	help_asr(m);
	write(addr, m);
}

void mc6809::bcc(void)
{
	do_br(!cc.bit.c);
}

void mc6809::lbcc(void)
{
	do_lbr(!cc.bit.c);
}

void mc6809::bcs(void)
{
	do_br(cc.bit.c);
}

void mc6809::lbcs(void)
{
	do_lbr(cc.bit.c);
}

void mc6809::beq(void)
{
	do_br(cc.bit.z);
}

void mc6809::lbeq(void)
{
	do_lbr(cc.bit.z);
}

void mc6809::bge(void)
{
	do_br(!(cc.bit.n ^ cc.bit.v));
}

void mc6809::lbge(void)
{
	do_lbr(!(cc.bit.n ^ cc.bit.v));
}

void mc6809::bgt(void)
{
	do_br(!(cc.bit.z | (cc.bit.n ^ cc.bit.v)));
}

void mc6809::lbgt(void)
{
	do_lbr(!(cc.bit.z | (cc.bit.n ^ cc.bit.v)));
}

void mc6809::bhi(void)
{
	do_br(!(cc.bit.c | cc.bit.z));
}

void mc6809::lbhi(void)
{
	do_lbr(!(cc.bit.c | cc.bit.z));
}

void mc6809::bita(void)
{
	help_bit(a);
}

void mc6809::bitb(void)
{
	help_bit(b);
}

void mc6809::help_bit(Byte x)
{
	Byte t = x & fetch_operand();
	cc.bit.n = btst(t, 7);
	cc.bit.v = 0;
	cc.bit.z = !t;
}

void mc6809::ble(void)
{
	do_br(cc.bit.z | (cc.bit.n ^ cc.bit.v));
}

void mc6809::lble(void)
{
	do_lbr(cc.bit.z | (cc.bit.n ^ cc.bit.v));
}

void mc6809::bls(void)
{
	do_br(cc.bit.c | cc.bit.z);
}

void mc6809::lbls(void)
{
	do_lbr(cc.bit.c | cc.bit.z);
}

void mc6809::blt(void)
{
	do_br(cc.bit.n ^ cc.bit.v);
}

void mc6809::lblt(void)
{
	do_lbr(cc.bit.n ^ cc.bit.v);
}

void mc6809::bmi(void)
{
	do_br(cc.bit.n);
}

void mc6809::lbmi(void)
{
	do_lbr(cc.bit.n);
}

void mc6809::bne(void)
{
	do_br(!cc.bit.z);
}

void mc6809::lbne(void)
{
	do_lbr(!cc.bit.z);
}

void mc6809::bpl(void)
{
	do_br(!cc.bit.n);
}

void mc6809::lbpl(void)
{
	do_lbr(!cc.bit.n);
}

void mc6809::bra(void)
{
	do_br(1);
}

void mc6809::lbra(void)
{
	do_lbr(1);
}

void mc6809::brn(void)
{
	do_br(0);
}

void mc6809::lbrn(void)
{
	do_lbr(0);
}

void mc6809::bsr(void)
{
	Byte	x = fetch();
	write(--s, (Byte)pc);
	write(--s, (Byte)(pc >> 8));
	pc += extend8(x);
}

void mc6809::lbsr(void)
{
	Word	x = fetch_word();
	write(--s, (Byte)pc);
	write(--s, (Byte)(pc >> 8));
	pc += x;
}

void mc6809::bvc(void)
{
	do_br(!cc.bit.v);
}

void mc6809::lbvc(void)
{
	do_lbr(!cc.bit.v);
}

void mc6809::bvs(void)
{
	do_br(cc.bit.v);
}

void mc6809::lbvs(void)
{
	do_lbr(cc.bit.v);
}

void mc6809::clra(void)
{
	help_clr(a);
}

void mc6809::clrb(void)
{
	help_clr(b);
}

void mc6809::clr(void)
{
	Word	addr = fetch_effective_address();
	Byte	m = read(addr);
	help_clr(m);
	write(addr, m);
}

void mc6809::help_clr(Byte& x)
{
	cc.all &= 0xf0;
	cc.all |= 0x04;
	x = 0;
}

void mc6809::cmpa(void)
{
	help_cmp(a);
}

void mc6809::cmpb(void)
{
	help_cmp(b);
}

void mc6809::help_cmp(Byte x)
{
	Byte	m = fetch_operand();
	int	t = x - m;

	cc.bit.v = btst((Byte)(x ^ m ^ t ^ (t >> 1)), 7);
	cc.bit.c = btst((Word)t, 8);
	cc.bit.n = btst((Byte)t, 7);
	cc.bit.z = !t;
}

void mc6809::cmpd(void)
{
	help_cmp(d);
}

void mc6809::cmpx(void)
{
	help_cmp(x);
}

void mc6809::cmpy(void)
{
	help_cmp(y);
}

void mc6809::cmpu(void)
{
	help_cmp(u);
}

void mc6809::cmps(void)
{
	help_cmp(s);
}

void mc6809::help_cmp(Word x)
{
	Word	m = fetch_word_operand();
	long	t = x - m;

	cc.bit.v = btst((DWord)(x ^ m ^ t ^ (t >> 1)), 15);
	cc.bit.c = btst((DWord)t, 16);
	cc.bit.n = btst((DWord)t, 15);
	cc.bit.z = !t;
}

void mc6809::coma(void)
{
	help_com(a);
}

void mc6809::comb(void)
{
	help_com(b);
}

void mc6809::com(void)
{
	Word	addr = fetch_effective_address();
	Byte	m = read(addr);
	help_com(m);
	write(addr, m);
}

void mc6809::help_com(Byte& x)
{
	x = ~x;
	cc.bit.c = 1;
	cc.bit.v = 0;
	cc.bit.n = btst(x, 7);
	cc.bit.z = !x;
}

void mc6809::daa(void)
{
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
		cc.bit.c = btst(t, 8);
		a = (Byte)t;
	}

	cc.bit.n = btst(a, 7);
	cc.bit.z = !a;
}

void mc6809::deca(void)
{
	help_dec(a);
}

void mc6809::decb(void)
{
	help_dec(b);
}

void mc6809::dec(void)
{
	Word	addr = fetch_effective_address();
	Byte	m = read(addr);
	help_dec(m);
	write(addr, m);
}

void mc6809::help_dec(Byte& x)
{
	cc.bit.v = (x == 0x80);
	x = x - 1;
	cc.bit.n = btst(x, 7);
	cc.bit.z = !x;
}

void mc6809::eora(void)
{
	help_eor(a);
}

void mc6809::eorb(void)
{
	help_eor(b);
}

void mc6809::help_eor(Byte& x)
{
	x = x ^ fetch_operand();
	cc.bit.v = 0;
	cc.bit.n = btst(x, 7);
	cc.bit.z = !x;
}

static void swap(Byte& r1, Byte &r2)
{
	Byte	t;
	t = r1; r1 = r2; r2 = t;
}

static void swap(Word& r1, Word &r2)
{
	Word	t;
	t = r1; r1 = r2; r2 = t;
}

void mc6809::exg(void)
{
	int	r1, r2;
	Byte	w = fetch();
	r1 = (w & 0xf0) >> 4;
	r2 = (w & 0x0f) >> 0;
	if (r1 <= 5) {
		if (r2 > 5) {
			invalid("exchange register");
			return;
		}
		swap(wordrefreg(r2), wordrefreg(r1));
	} else if (r1 >= 8 && r1 <= 11) {
		if (r2 < 8 || r2 > 11) {
			invalid("exchange register");
			return;
		}
		swap(byterefreg(r2), byterefreg(r1));
	} else  {
		invalid("exchange register");
		return;
	}
}

void mc6809::inca(void)
{
	help_inc(a);
}

void mc6809::incb(void)
{
	help_inc(b);
}

void mc6809::inc(void)
{
	Word	addr = fetch_effective_address();
	Byte	m = read(addr);
	help_inc(m);
	write(addr, m);
}

void mc6809::help_inc(Byte& x)
{
	cc.bit.v = (x == 0x7f);
	x = x + 1;
	cc.bit.n = btst(x, 7);
	cc.bit.z = !x;
}

void mc6809::jmp(void)
{
	pc = fetch_effective_address();
}

void mc6809::jsr(void)
{
	Word	addr = fetch_effective_address();
	write(--s, (pc >> 0) & 0xff);
	write(--s, (pc >> 8) & 0xff);
	pc = addr;
}

void mc6809::lda(void)
{
	help_ld(a);
}

void mc6809::ldb(void)
{
	help_ld(b);
}

void mc6809::help_ld(Byte& x)
{
	x = fetch_operand();
	cc.bit.n = btst(x, 7);
	cc.bit.v = 0;
	cc.bit.z = !x;
}

void mc6809::ldd(void)
{
	help_ld(d);
}

void mc6809::ldx(void)
{
	help_ld(x);
}

void mc6809::ldy(void)
{
	help_ld(y);
}

void mc6809::lds(void)
{
	help_ld(s);
}

void mc6809::ldu(void)
{
	help_ld(u);
}

void mc6809::help_ld(Word& x)
{
	x = fetch_word_operand();
	cc.bit.n = btst(x, 15);
	cc.bit.v = 0;
	cc.bit.z = !x;
}

void mc6809::leax(void)
{
	x = fetch_effective_address();
	cc.bit.z = !x;
}

void mc6809::leay(void)
{
	y = fetch_effective_address();
	cc.bit.z = !y;
}

void mc6809::leas(void)
{
	s = fetch_effective_address();
}

void mc6809::leau(void)
{
	u = fetch_effective_address();
}

void mc6809::lsla(void)
{
	help_lsl(a);
}

void mc6809::lslb(void)
{
	help_lsl(b);
}

void mc6809::lsl(void)
{
	Word	addr = fetch_effective_address();
	Byte	m = read(addr);
	help_lsl(m);
	write(addr, m);
}

void mc6809::help_lsl(Byte& x)
{
	cc.bit.c = btst(x, 7);
	cc.bit.v = btst(x, 7) ^ btst(x, 6);
	x <<= 1;
	cc.bit.n = btst(x, 7);
	cc.bit.z = !x;
}

void mc6809::lsra(void)
{
	help_lsr(a);
}

void mc6809::lsrb(void)
{
	help_lsr(b);
}

void mc6809::lsr(void)
{
	Word	addr = fetch_effective_address();
	Byte	m = read(addr);
	help_lsr(m);
	write(addr, m);
}

void mc6809::help_lsr(Byte& x)
{
	cc.bit.c = btst(x, 0);
	x >>= 1;	/* Shift word right */
	cc.bit.n = 0;
	cc.bit.z = !x;
}

void mc6809::mul(void)
{
	d = a * b;
	cc.bit.c = btst(b, 7);
	cc.bit.z = !d;
}

void mc6809::nega(void)
{
	help_neg(a);
}

void mc6809::negb(void)
{
	help_neg(b);
}

void mc6809::neg(void)
{
	Word 	addr = fetch_effective_address();
	Byte	m = read(addr);
	help_neg(m);
	write(addr, m);
}

void mc6809::help_neg(Byte& x)
{
	int	t = 0 - x;

	cc.bit.v = btst((Byte)(x ^ t ^ (t >> 1)), 7);
	cc.bit.c = btst((Word)t, 8);
	cc.bit.n = btst((Byte)t, 7);
	cc.bit.z = !t;

	x = t & 0xff;
}

void mc6809::nop(void)
{
}

void mc6809::ora(void)
{
	help_or(a);
}

void mc6809::orb(void)
{
	help_or(b);
}

void mc6809::help_or(Byte& x)
{
	x = x | fetch_operand();
	cc.bit.v = 0;
	cc.bit.n = btst(x, 7);
	cc.bit.z = !x;
}

void mc6809::orcc(void)
{
	cc.all |= fetch_operand();
}

void mc6809::pshs(void)
{
	help_psh(fetch(), s, u);
}

void mc6809::pshu(void)
{
	help_psh(fetch(), u, s);
}

void mc6809::help_psh(Byte w, Word& s, Word& u)
{
	if (btst(w, 7)) {
		write(--s, (Byte)pc);
		write(--s, (Byte)(pc >> 8));
	}
	if (btst(w, 6)) {
		write(--s, (Byte)u);
		write(--s, (Byte)(u >> 8));
	}
	if (btst(w, 5)) {
		write(--s, (Byte)y);
		write(--s, (Byte)(y >> 8));
	}
	if (btst(w, 4)) {
		write(--s, (Byte)x);
		write(--s, (Byte)(x >> 8));
	}
	if (btst(w, 3)) write(--s, (Byte)dp);
	if (btst(w, 2)) write(--s, (Byte)b);
	if (btst(w, 1)) write(--s, (Byte)a);
	if (btst(w, 0)) write(--s, (Byte)cc.all);
}

void mc6809::puls(void)
{
	Byte	w = fetch();
	help_pul(w, s, u);
}

void mc6809::pulu(void)
{
	Byte	w = fetch();
	help_pul(w, u, s);
}

void mc6809::help_pul(Byte w, Word& s, Word& u)
{
	if (btst(w, 0)) cc.all = read(s++);
	if (btst(w, 1)) a = read(s++);
	if (btst(w, 2)) b = read(s++);
	if (btst(w, 3)) dp = read(s++);
	if (btst(w, 4)) {
		x = read_word(s); s += 2;
	}
	if (btst(w, 5)) {
		y = read_word(s); s += 2;
	}
	if (btst(w, 6)) {
		u = read_word(s); s += 2;
	}
	if (btst(w, 7)) {
		pc = read_word(s); s += 2;
	}
}

void mc6809::rola(void)
{
	help_rol(a);
}

void mc6809::rolb(void)
{
	help_rol(b);
}

void mc6809::rol(void)
{
	Word	addr = fetch_effective_address();
	Byte	m = read(addr);
	help_rol(m);
	write(addr, m);
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
}

void mc6809::rora(void)
{
	help_ror(a);
}

void mc6809::rorb(void)
{
	help_ror(b);
}

void mc6809::ror(void)
{
	Word	addr = fetch_effective_address();
	Byte	m = read(addr);
	help_ror(m);
	write(addr, m);
}

void mc6809::help_ror(Byte& x)
{
	int	oc = cc.bit.c;
	cc.bit.c = btst(x, 0);
	x = x >> 1;
	if (oc) bset(x, 7);
	cc.bit.n = btst(x, 7);
	cc.bit.z = !x;
}

void mc6809::rti(void)
{
	help_pul(0x01, s, u);
	if (cc.bit.e) {
		help_pul(0xfe, s, u);
	} else {
		help_pul(0x80, s, u);
	}
}

void mc6809::rts(void)
{
	pc = read_word(s);
	s += 2;
}

void mc6809::sbca(void)
{
	help_sbc(a);
}

void mc6809::sbcb(void)
{
	help_sbc(b);
}

void mc6809::help_sbc(Byte& x)
{
	Byte    m = fetch_operand();
	int t = x - m - cc.bit.c;

	cc.bit.v = btst((Byte)(x ^ m ^ t ^ (t >> 1)), 7);
	cc.bit.c = btst((Word)t, 8);
	cc.bit.n = btst((Byte)t, 7);
	cc.bit.z = !t;
	x = t & 0xff;
}

void mc6809::sex(void)
{
	cc.bit.n = btst(b, 7);
	cc.bit.z = !b;
	a = cc.bit.n ? 255 : 0;
}

void mc6809::sta(void)
{
	help_st(a);
}

void mc6809::stb(void)
{
	help_st(b);
}

void mc6809::help_st(Byte x)
{
	Word	addr = fetch_effective_address();
	write(addr, x);
	cc.bit.v = 0;
	cc.bit.n = btst(x, 7);
	cc.bit.z = !x;
}

void mc6809::std(void)
{
	help_st(d);
}

void mc6809::stx(void)
{
	help_st(x);
}

void mc6809::sty(void)
{
	help_st(y);
}

void mc6809::sts(void)
{
	help_st(s);
}

void mc6809::stu(void)
{
	help_st(u);
}

void mc6809::help_st(Word x)
{
	Word	addr = fetch_effective_address();
	write_word(addr, x);
	cc.bit.v = 0;
	cc.bit.n = btst(x, 15);
	cc.bit.z = !x;
}

void mc6809::suba(void)
{
	help_sub(a);
}

void mc6809::subb(void)
{
	help_sub(b);
}

void mc6809::help_sub(Byte& x)
{
	Byte    m = fetch_operand();
	int t = x - m;

	cc.bit.v = btst((Byte)(x^m^t^(t>>1)),7);
	cc.bit.c = btst((Word)t,8);
	cc.bit.n = btst((Byte)t, 7);
	cc.bit.z = !t;
	x = t & 0xff;
}

void mc6809::subd(void)
{
	Word    m = fetch_word_operand();
	int t = d - m;

	cc.bit.v = btst((DWord)(d ^ m ^ t ^(t >> 1)), 15);
	cc.bit.c = btst((DWord)t, 16);
	cc.bit.n = btst((DWord)t, 15);
	cc.bit.z = !t;
	d = t & 0xffff;
}

void mc6809::swi(void)
{
	cc.bit.e = 1;
	help_psh(0xff, s, u);
	cc.bit.f = cc.bit.i = 1;
	pc = read_word(0xfffa);
}

void mc6809::swi2(void)
{
	cc.bit.e = 1;
	help_psh(0xff, s, u);
	pc = read_word(0xfff4);
}

void mc6809::swi3(void)
{
	cc.bit.e = 1;
	help_psh(0xff, s, u);
	pc = read_word(0xfff2);
}

void mc6809::tfr(void)
{
	int	r1, r2;
	Byte	w = fetch();
	r1 = (w & 0xf0) >> 4;
	r2 = (w & 0x0f) >> 0;
	if (r1 <= 5) {
		if (r2 > 5) {
			invalid("transfer register");
			return;
		}
		wordrefreg(r2) = wordrefreg(r1);
	} else if (r1 >= 8 && r1 <= 11) {
		if (r2 < 8 || r2 > 11) {
			invalid("transfer register");
			return;
		}
		byterefreg(r2) = byterefreg(r1);
	} else  {
		invalid("transfer register");
		return;
	}
}

void mc6809::tsta(void)
{
	help_tst(a);
}

void mc6809::tstb(void)
{
	help_tst(b);
}

void mc6809::tst(void)
{
	Word	addr = fetch_effective_address();
	Byte	m = read(addr);
	help_tst(m);
}

void mc6809::help_tst(Byte x)
{
	cc.bit.v = 0;
	cc.bit.n = btst(x, 7);
	cc.bit.z = !x;
}

void mc6809::do_br(int test)
{
	Word offset = extend8(fetch_operand());
	if (test) pc += offset;
}

void mc6809::do_lbr(int test)
{
	Word offset = fetch_word_operand();
	if (test) pc += offset;
}
