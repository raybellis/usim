//
//	mc6809ins.cc
//
//	(C) R.P.Bellis
//
//	Updated from BDA and Soren Roug 12/2003 primarily
//	with fixes for SBC / CMP / NEG instructions with
//	incorrect carry flag settings
//

#include "mc6809.h"
#include "bits.h"

void mc6809::do_br(bool test)
{
	Word offset = extend8(fetch_operand());
	if (test) pc += offset;
}

void mc6809::do_lbr(bool test)
{
	Word offset = fetch_word_operand();
	if (test) pc += offset;
}

void mc6809::do_psh(Word& sp, Byte val)
{
	write(--sp, val);
}

void mc6809::do_psh(Word& sp, Word val)
{
	write(--sp, (Byte)val);
	write(--sp, (Byte)(val >> 8));
}

void mc6809::do_pul(Word& sp, Byte& val)
{
	val = read(sp++);
}

void mc6809::do_pul(Word& sp, Word& val)
{
	val  = read(sp++) << 8;
	val |= read(sp++);
}

void mc6809::abx()
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

void mc6809::adca()
{
	help_adc(a);
}

void mc6809::adcb()
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

void mc6809::adda()
{
	help_add(a);
}

void mc6809::addb()
{
	help_add(b);
}

void mc6809::addd()
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

void mc6809::anda()
{
	help_and(a);
}

void mc6809::andb()
{
	help_and(b);
}

void mc6809::andcc()
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

void mc6809::asra()
{
	help_asr(a);
}

void mc6809::asrb()
{
	help_asr(b);
}

void mc6809::asr()
{
	Word	addr = fetch_effective_address();
	Byte	m = read(addr);

	help_asr(m);
	write(addr, m);
}

void mc6809::bcc()
{
	do_br(!cc.bit.c);
}

void mc6809::lbcc()
{
	do_lbr(!cc.bit.c);
}

void mc6809::bcs()
{
	do_br(cc.bit.c);
}

void mc6809::lbcs()
{
	do_lbr(cc.bit.c);
}

void mc6809::beq()
{
	do_br(cc.bit.z);
}

void mc6809::lbeq()
{
	do_lbr(cc.bit.z);
}

void mc6809::bge()
{
	do_br(!(cc.bit.n ^ cc.bit.v));
}

void mc6809::lbge()
{
	do_lbr(!(cc.bit.n ^ cc.bit.v));
}

void mc6809::bgt()
{
	do_br(!(cc.bit.z | (cc.bit.n ^ cc.bit.v)));
}

void mc6809::lbgt()
{
	do_lbr(!(cc.bit.z | (cc.bit.n ^ cc.bit.v)));
}

void mc6809::bhi()
{
	do_br(!(cc.bit.c | cc.bit.z));
}

void mc6809::lbhi()
{
	do_lbr(!(cc.bit.c | cc.bit.z));
}

void mc6809::bita()
{
	help_bit(a);
}

void mc6809::bitb()
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

void mc6809::ble()
{
	do_br(cc.bit.z | (cc.bit.n ^ cc.bit.v));
}

void mc6809::lble()
{
	do_lbr(cc.bit.z | (cc.bit.n ^ cc.bit.v));
}

void mc6809::bls()
{
	do_br(cc.bit.c | cc.bit.z);
}

void mc6809::lbls()
{
	do_lbr(cc.bit.c | cc.bit.z);
}

void mc6809::blt()
{
	do_br(cc.bit.n ^ cc.bit.v);
}

void mc6809::lblt()
{
	do_lbr(cc.bit.n ^ cc.bit.v);
}

void mc6809::bmi()
{
	do_br(cc.bit.n);
}

void mc6809::lbmi()
{
	do_lbr(cc.bit.n);
}

void mc6809::bne()
{
	do_br(!cc.bit.z);
}

void mc6809::lbne()
{
	do_lbr(!cc.bit.z);
}

void mc6809::bpl()
{
	do_br(!cc.bit.n);
}

void mc6809::lbpl()
{
	do_lbr(!cc.bit.n);
}

void mc6809::bra()
{
	do_br(1);
}

void mc6809::lbra()
{
	do_lbr(1);
}

void mc6809::brn()
{
	do_br(0);
}

void mc6809::lbrn()
{
	do_lbr(0);
}

void mc6809::bsr()
{
	Byte	x = fetch();
	do_psh(s, pc);
	pc += extend8(x);
}

void mc6809::lbsr()
{
	Word	x = fetch_word();
	do_psh(s, pc);
	pc += x;
}

void mc6809::bvc()
{
	do_br(!cc.bit.v);
}

void mc6809::lbvc()
{
	do_lbr(!cc.bit.v);
}

void mc6809::bvs()
{
	do_br(cc.bit.v);
}

void mc6809::lbvs()
{
	do_lbr(cc.bit.v);
}

void mc6809::clra()
{
	help_clr(a);
}

void mc6809::clrb()
{
	help_clr(b);
}

void mc6809::clr()
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

void mc6809::cmpa()
{
	help_cmp(a);
}

void mc6809::cmpb()
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
	cc.bit.z = !(t & 0xff);
}

void mc6809::cmpd()
{
	help_cmp(d);
}

void mc6809::cmpx()
{
	help_cmp(x);
}

void mc6809::cmpy()
{
	help_cmp(y);
}

void mc6809::cmpu()
{
	help_cmp(u);
}

void mc6809::cmps()
{
	help_cmp(s);
}

void mc6809::cwai()
{
	Byte	n = fetch();
	cc.all &= n;
	cc.bit.e = 1;
	help_psh(0xff, s, u);
	waiting_cwai = true;
}

void mc6809::help_cmp(Word x)
{
	Word	m = fetch_word_operand();
	long	t = x - m;

	cc.bit.v = btst((DWord)(x ^ m ^ t ^ (t >> 1)), 15);
	cc.bit.c = btst((DWord)t, 16);
	cc.bit.n = btst((DWord)t, 15);
	cc.bit.z = !(t & 0xffff);
}

void mc6809::coma()
{
	help_com(a);
}

void mc6809::comb()
{
	help_com(b);
}

void mc6809::com()
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

void mc6809::daa()
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
		cc.bit.c |= btst(t, 8);
		a = (Byte)t;
	}

	cc.bit.n = btst(a, 7);
	cc.bit.z = !a;
}

void mc6809::deca()
{
	help_dec(a);
}

void mc6809::decb()
{
	help_dec(b);
}

void mc6809::dec()
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

void mc6809::eora()
{
	help_eor(a);
}

void mc6809::eorb()
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

void mc6809::exg()
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

void mc6809::inca()
{
	help_inc(a);
}

void mc6809::incb()
{
	help_inc(b);
}

void mc6809::inc()
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

void mc6809::jmp()
{
	pc = fetch_effective_address();
}

void mc6809::jsr()
{
	Word	addr = fetch_effective_address();
	do_psh(s, pc);
	pc = addr;
}

void mc6809::lda()
{
	help_ld(a);
}

void mc6809::ldb()
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

void mc6809::ldd()
{
	help_ld(d);
}

void mc6809::ldx()
{
	help_ld(x);
}

void mc6809::ldy()
{
	help_ld(y);
}

void mc6809::lds()
{
	help_ld(s);
}

void mc6809::ldu()
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

void mc6809::leax()
{
	x = fetch_effective_address();
	cc.bit.z = !x;
}

void mc6809::leay()
{
	y = fetch_effective_address();
	cc.bit.z = !y;
}

void mc6809::leas()
{
	s = fetch_effective_address();
}

void mc6809::leau()
{
	u = fetch_effective_address();
}

void mc6809::lsla()
{
	help_lsl(a);
}

void mc6809::lslb()
{
	help_lsl(b);
}

void mc6809::lsl()
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

void mc6809::lsra()
{
	help_lsr(a);
}

void mc6809::lsrb()
{
	help_lsr(b);
}

void mc6809::lsr()
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

void mc6809::mul()
{
	d = a * b;
	cc.bit.c = btst(b, 7);
	cc.bit.z = !d;
}

void mc6809::nega()
{
	help_neg(a);
}

void mc6809::negb()
{
	help_neg(b);
}

void mc6809::neg()
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
	x = t & 0xff;
	cc.bit.z = !x;
}

void mc6809::nop()
{
}

void mc6809::ora()
{
	help_or(a);
}

void mc6809::orb()
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

void mc6809::orcc()
{
	cc.all |= fetch_operand();
}

void mc6809::pshs()
{
	help_psh(fetch(), s, u);
}

void mc6809::pshu()
{
	help_psh(fetch(), u, s);
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

void mc6809::puls()
{
	Byte	w = fetch();
	help_pul(w, s, u);
}

void mc6809::pulu()
{
	Byte	w = fetch();
	help_pul(w, u, s);
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

void mc6809::rola()
{
	help_rol(a);
}

void mc6809::rolb()
{
	help_rol(b);
}

void mc6809::rol()
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

void mc6809::rora()
{
	help_ror(a);
}

void mc6809::rorb()
{
	help_ror(b);
}

void mc6809::ror()
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

void mc6809::rti()
{
	help_pul(0x01, s, u);
	if (cc.bit.e) {
		help_pul(0xfe, s, u);
	} else {
		help_pul(0x80, s, u);
	}
}

void mc6809::rts()
{
	pc = read_word(s);
	s += 2;
}

void mc6809::sbca()
{
	help_sbc(a);
}

void mc6809::sbcb()
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
	x = t & 0xff;
	cc.bit.z = !x;
}

void mc6809::sex()
{
	cc.bit.n = btst(b, 7);
	cc.bit.z = !b;
	a = cc.bit.n ? 255 : 0;
}

void mc6809::sta()
{
	help_st(a);
}

void mc6809::stb()
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

void mc6809::std()
{
	help_st(d);
}

void mc6809::stx()
{
	help_st(x);
}

void mc6809::sty()
{
	help_st(y);
}

void mc6809::sts()
{
	help_st(s);
}

void mc6809::stu()
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

void mc6809::suba()
{
	help_sub(a);
}

void mc6809::subb()
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
	x = t & 0xff;
	cc.bit.z = !x;
}

void mc6809::subd()
{
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
	cc.bit.e = 1;
	help_psh(0xff, s, u);
	cc.bit.f = cc.bit.i = 1;
	pc = read_word(0xfffa);
}

void mc6809::swi2()
{
	cc.bit.e = 1;
	help_psh(0xff, s, u);
	pc = read_word(0xfff4);
}

void mc6809::swi3()
{
	cc.bit.e = 1;
	help_psh(0xff, s, u);
	pc = read_word(0xfff2);
}

void mc6809::sync()
{
	waiting_sync = true;
}

void mc6809::tfr()
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

void mc6809::tsta()
{
	help_tst(a);
}

void mc6809::tstb()
{
	help_tst(b);
}

void mc6809::tst()
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
