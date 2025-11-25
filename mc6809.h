//
//	mc6809.h
//	(C) R.P.Bellis 1993 - 2025
//	Class definition for Motorola MC6809 microprocessor
//	vim: ts=8
//

#pragma once

#include <string>
#include <bit>
#include "wiring.h"
#include "usim.h"
#include "bits.h"

union mc6809_cc {
	uint8_t         value;
	template <int offset> using bit = ByteBits<mc6809_cc, offset>;
	bit<0>          c;      // Carry
	bit<1>          v;      // Overflow
	bit<2>          z;      // Zero
	bit<3>          n;      // Negative
	bit<4>          i;      // IRQ disable
	bit<5>          h;      // Half carry
	bit<6>          f;      // FIRQ disable
	bit<7>          e;      // Entire
	operator uint8_t() const { return value; }
	uint8_t operator =(uint8_t n) { return value = n; }
};

class mc6809 : virtual public USimBE {

protected: // Processor addressing modes

	enum {
				immediate,
				direct,
				indexed,
				extended,
				inherent,
				relative
	} mode;

protected:	// Processor registers

	Word			u, s;		// Stack pointers
	Word			x, y;		// Index registers
	Byte			dp;		// Direct Page register
        mc6809_cc               cc;

        // NB: intentional UB type-aliasing
        union {
		Word			d;	// Combined accumulator
		struct {
#if __BYTE_ORDER == __LITTLE_ENDIAN
			Byte		b;	// Accumulator b
			Byte		a;	// Accumulator a
#elif __BYTE_ORDER == __BIG_ENDIAN
			Byte		a;	// Accumulator a
			Byte		b;	// Accumulator b
#else
#error "target byte order cannot be determined"
#endif
		};
	};

private:	// internal processor state
	bool			waiting_sync;
	bool			waiting_cwai;
	bool			nmi_previous;

private:	// instruction and operand fetch and decode
	Word&			ix_refreg(Byte);

	void			fetch_instruction();
	Byte			fetch_operand();
	Word			fetch_word_operand();
	Word			fetch_effective_address();
	Word			fetch_indexed_operand();
	void			execute_instruction();

	void			do_predecrement();
	void			do_postincrement();

private:	// instruction implementations
	void			abx();
	void			adca(), adcb();
	void			adda(), addb(), addd();
	void			anda(), andb(), andcc();
	void			asra(), asrb(), asr();
	void			bcc(), lbcc();
	void			bcs(), lbcs();
	void			beq(), lbeq();
	void			bge(), lbge();
	void			bgt(), lbgt();
	void			bhi(), lbhi();
	void			bita(), bitb();
	void			ble(), lble();
	void			bls(), lbls();
	void			blt(), lblt();
	void			bmi(), lbmi();
	void			bne(), lbne();
	void			bpl(), lbpl();
	void			bra(), lbra();
	void			brn(), lbrn();
	void			bsr(), lbsr();
	void			bvc(), lbvc();
	void			bvs(), lbvs();
	void			clra(), clrb(), clr();
	void			cmpa(), cmpb();
	void			cmpd(), cmpx(), cmpy(), cmpu(), cmps();
	void			coma(), comb(), com();
	void			cwai();
	void			daa();
	void			deca(), decb(), dec();
	void			eora(), eorb();
	void			exg();
	void			inca(), incb(), inc();
	void			jmp();
	void			jsr();
	void			lda(), ldb();
	void			ldd(), ldx(), ldy(), lds(), ldu();
	void			leax(), leay(), leas(), leau();
	void			lsla(), lslb(), lsl();
	void			lsra(), lsrb(), lsr();
	void			mul();
	void			nega(), negb(), neg();
	void			nop();
	void			ora(), orb(), orcc();
	void			pshs(), pshu();
	void			puls(), pulu();
	void			rola(), rolb(), rol();
	void			rora(), rorb(), ror();
	void			rti(), rts();
	void			sbca(), sbcb();
	void			sex();
	void			sta(), stb();
	void			std(), stx(), sty(), sts(), stu();
	void			suba(), subb();
	void			subd();
	void			swi(), swi2(), swi3();
	void			sync();
	void			tfr();
	void			tsta(), tstb(), tst();

protected:	// helper functions
	void			help_adc(Byte&);
	void			help_add(Byte&);
	void			help_and(Byte&);
	void			help_asr(Byte&);
	void			help_bit(Byte);
	void			help_clr(Byte&);
	void			help_cmp(Byte);
	void			help_cmp(Word);
	void			help_com(Byte&);
	void			help_dec(Byte&);
	void			help_eor(Byte&);
	void			help_inc(Byte&);
	void			help_ld(Byte&);
	void			help_ld(Word&);
	void			help_lsr(Byte&);
	void			help_lsl(Byte&);
	void			help_neg(Byte&);
	void			help_or(Byte&);
	void			help_psh(Byte, Word&, Word&);
	void			help_pul(Byte, Word&, Word&);
	void			help_ror(Byte&);
	void			help_rol(Byte&);
	void			help_sbc(Byte&);
	void			help_st(Byte);
	void			help_st(Word);
	void			help_sub(Byte&);
	void			help_sub(Word&);
	void			help_tst(Byte);

protected:	// overloadable functions (e.g. for breakpoints)
	virtual void		do_br(const char *, bool);
	virtual void		do_lbr(const char *, bool);

	virtual void		do_psh(Word& sp, Byte);
	virtual void		do_psh(Word& sp, Word);
	virtual void		do_pul(Word& sp, Byte&);
	virtual void		do_pul(Word& sp, Word&);

	virtual void		do_nmi();
	virtual void		do_firq();
	virtual void		do_irq();

	virtual void		pre_exec();
	virtual void		post_exec();

protected: 	// instruction tracing
	Word			insn_pc;
	const char*		insn;
	Byte			post;
	Word			operand;

	std::string		disasm_operand();
	std::string		disasm_indexed();

public:		// external signal pins
	InputPin		IRQ, FIRQ, NMI;

public:
				mc6809();		// public constructor
	virtual			~mc6809();		// public destructor

	virtual void		reset();		// CPU reset
	virtual void		tick();

	virtual void		print_regs();

	Byte&			byterefreg(int);
	Word&			wordrefreg(int);

};

inline void mc6809::do_br(const char *mnemonic, bool test)
{
	(void)mnemonic;
	Word offset = extend8(fetch_operand());
	if (test) pc += offset;
	++cycles;
}

inline void mc6809::do_lbr(const char *mnemonic, bool test)
{
	(void)mnemonic;
	Word offset = fetch_word_operand();
	if (test) {
		pc += offset;
		++cycles;
	}
	++cycles;
}

inline void mc6809::do_psh(Word& sp, Byte val)
{
	write(--sp, val);
}

inline void mc6809::do_psh(Word& sp, Word val)
{
	write(--sp, (Byte)val);
	write(--sp, (Byte)(val >> 8));
}

inline void mc6809::do_pul(Word& sp, Byte& val)
{
	val = read(sp++);
}

inline void mc6809::do_pul(Word& sp, Word& val)
{
	val  = read(sp++) << 8;
	val |= read(sp++);
}
