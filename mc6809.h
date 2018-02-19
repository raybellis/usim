//
//	mc6809.h
//
//	Class definition for Motorola MC6809 microprocessor
//
//	(C) R.P.Bellis 1993
//

#ifndef __mc6809_h__
#define __mc6809_h__

#include "usim.h"
#include "machdep.h"

class mc6809 : virtual public USimMotorola {

// Processor addressing modes
protected:

	enum {
				immediate = 0,
				relative = 0,
				inherent,
				extended,
				direct,
				indexed
	} mode;

// Processor registers
protected:

	Word			u, s;		// Stack pointers
	Word			x, y;		// Index registers
	Byte			dp;		// Direct Page register
	union {
		Word			d;	// Combined accumulator
		struct {
#ifdef MACH_BYTE_ORDER_MSB_FIRST
			Byte		a;	// Accumulator a
			Byte		b;	// Accumulator b
#else
			Byte		b;	// Accumulator b
			Byte		a;	// Accumulator a
#endif
		} byte;
	} acc;
	Byte&			a;
	Byte&			b;
	Word&			d;
	union {
		Byte			all;	// Condition code register
		struct {
#ifdef MACH_BITFIELDS_LSB_FIRST
			Byte		c : 1;	// Carry
			Byte		v : 1;	// Overflow
			Byte		z : 1;	// Zero
			Byte		n : 1;	// Negative
			Byte		i : 1;	// IRQ disable
			Byte		h : 1;	// Half carry
			Byte		f : 1;	// FIRQ disable
			Byte		e : 1;	// Entire
#else
			Byte		e : 1;	// Entire
			Byte		f : 1;	// FIRQ disable
			Byte		h : 1;	// Half carry
			Byte		i : 1;	// IRQ disable
			Byte		n : 1;	// Negative
			Byte		z : 1;	// Zero
			Byte		v : 1;	// Overflow
			Byte		c : 1;	// Carry
#endif
		} bit;
	} cc;

private:

	Word&			refreg(Byte);
	Byte&			byterefreg(int);
	Word&			wordrefreg(int);

	Byte			fetch_operand(void);
	Word			fetch_word_operand(void);
	Word			fetch_effective_address(void);
	Word			do_effective_address(Byte);
	void			do_predecrement(Byte);
	void			do_postincrement(Byte);

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

	void			do_br(int);
	void			do_lbr(int);

	void			do_nmi(void);
	void			do_firq(void);
	void			do_irq(void);

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

protected:
	virtual void		execute(void);

public:
				mc6809();		// public constructor
	virtual			~mc6809();		// public destructor

	virtual void		reset(void);		// CPU reset
	virtual void		status(void);

};

#endif // __mc6809_h__
