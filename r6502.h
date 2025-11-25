//
//	r6502.h
//	(C) R.P.Bellis 2025 -
//	Class definition for Rockwell 6502 microprocessor
//	vim: ts=8 sw=8 noet:
//

#pragma once

#include <string>
#include "wiring.h"
#include "usim.h"
#include "bits.h"

union r6502_status {

private:
	uint8_t         value;
	template <int offset> using bit = ByteBits<r6502_status, offset>;

public:
	bit<0>          c;
	bit<1>          z;
	bit<2>          i;
	bit<3>          d;
	bit<4>          b;
	bit<5>          r;
	bit<6>          v;
	bit<7>          n;
	operator uint8_t() const { return value; }
	uint8_t operator =(uint8_t n) { return value = n; }
};

class r6502 : virtual public USimLE {

protected: // Processor addressing modes

	enum {
				immediate,
				direct,
				indexed,
				extended,
				inherent,
				relative
	} mode;

	enum : uint16_t {
		stack_base	= 0x0100,
		vector_nmi	= 0xfffa,
		vector_reset	= 0xfffc,
		vector_irq	= 0xfffe
	};

protected:	// Processor registers

	Byte			a;		// Accumulator
	Byte			x, y;		// Index registers
	Byte			s;		// Stack pointer
	r6502_status		p;		// Processor status

private:	// internal processor state
	// bool			waiting_sync;
	bool			nmi_previous;

private:	// instruction and operand fetch and decode
	void			fetch_instruction();
	Byte			fetch_operand();
	Word			fetch_word_operand();
	Word			fetch_effective_address();
	Word			fetch_indexed_operand();
	void			execute_instruction();

private:	// instruction implementations
	// void			abx();
	// void			adca(), adcb();
	// void			adda(), addb(), addd();
	// void			anda(), andb(), andcc();
	// void			asra(), asrb(), asr();
	// void			bcc(), lbcc();
	// void			bcs(), lbcs();
	// void			beq(), lbeq();
	// void			bge(), lbge();
	// void			bgt(), lbgt();
	// void			bhi(), lbhi();
	// void			bita(), bitb();
	// void			ble(), lble();
	// void			bls(), lbls();
	// void			blt(), lblt();
	// void			bmi(), lbmi();
	// void			bne(), lbne();
	// void			bpl(), lbpl();
	// void			bra(), lbra();
	// void			brn(), lbrn();
	// void			bsr(), lbsr();
	// void			bvc(), lbvc();
	// void			bvs(), lbvs();
	// void			clra(), clrb(), clr();
	// void			cmpa(), cmpb();
	// void			cmpd(), cmpx(), cmpy(), cmpu(), cmps();
	// void			coma(), comb(), com();
	// void			cwai();
	// void			daa();
	// void			deca(), decb(), dec();
	// void			eora(), eorb();
	// void			exg();
	// void			inca(), incb(), inc();
	// void			jmp();
	// void			jsr();
	// void			lda(), ldb();
	// void			ldd(), ldx(), ldy(), lds(), ldu();
	// void			leax(), leay(), leas(), leau();
	// void			lsla(), lslb(), lsl();
	// void			lsra(), lsrb(), lsr();
	// void			mul();
	// void			nega(), negb(), neg();
	// void			nop();
	// void			ora(), orb(), orcc();
	// void			pshs(), pshu();
	// void			puls(), pulu();
	// void			rola(), rolb(), rol();
	// void			rora(), rorb(), ror();
	// void			rti(), rts();
	// void			sbca(), sbcb();
	// void			sex();
	// void			sta(), stb();
	// void			std(), stx(), sty(), sts(), stu();
	// void			suba(), subb();
	// void			subd();
	// void			swi(), swi2(), swi3();
	// void			sync();
	// void			tfr();
	// void			tsta(), tstb(), tst();

protected:	// helper functions
	// void			help_adc(Byte&);
	// void			help_add(Byte&);
	// void			help_and(Byte&);
	// void			help_asr(Byte&);
	// void			help_bit(Byte);
	// void			help_clr(Byte&);
	// void			help_cmp(Byte);
	// void			help_cmp(Word);
	// void			help_com(Byte&);
	// void			help_dec(Byte&);
	// void			help_eor(Byte&);
	// void			help_inc(Byte&);
	// void			help_ld(Byte&);
	// void			help_ld(Word&);
	// void			help_lsr(Byte&);
	// void			help_lsl(Byte&);
	// void			help_neg(Byte&);
	// void			help_or(Byte&);
	// void			help_psh(Byte, Word&, Word&);
	// void			help_pul(Byte, Word&, Word&);
	// void			help_ror(Byte&);
	// void			help_rol(Byte&);
	// void			help_sbc(Byte&);
	// void			help_st(Byte);
	// void			help_st(Word);
	// void			help_sub(Byte&);
	// void			help_sub(Word&);
	// void			help_tst(Byte);

protected:	// overloadable functions (e.g. for breakpoints)
	virtual void		do_br(const char *, bool);

	virtual void		do_psh(Byte);
	virtual void		do_psh(Word);
	virtual void		do_pul(Byte&);
	virtual void		do_pul(Word&);

	virtual void		do_nmi();
	virtual void		do_irq();

	virtual void		pre_exec();
	virtual void		post_exec();

protected: 	// instruction tracing
	Word			insn_pc;
	const char*		insn;
	Word			operand;

	// std::string		disasm_operand();
	// std::string		disasm_indexed();

public:		// external signal pins
	InputPin		IRQ, NMI;

public:
				r6502();		// public constructor
	virtual			~r6502();		// public destructor

	virtual void		reset();		// CPU reset
	virtual void		tick();

	virtual void		print_regs();
};

inline void r6502::do_br(const char *mnemonic, bool test)
{
	(void)mnemonic;
	Word offset = extend8(fetch_operand());
	if (test) pc += offset;
	++cycles;
}

inline void r6502::do_psh(Byte val)
{
	--s;
	write(stack_base + s, val);
}

inline void r6502::do_psh(Word val)
{
	do_psh((Byte)val);
	do_psh((Byte)(val >> 8));
}

inline void r6502::do_pul(Byte& val)
{
	val = read(0x100 + s);
	s++;
}

inline void r6502::do_pul(Word& val)
{
	uint8_t lsb, msb;
	do_pul(msb);
	do_pul(lsb);
	val = (msb << 8) | lsb;
}
