//
//	mos6502.h
//	(C) R.P.Bellis 2025 -
//	Class definition for Rockwell 6502 microprocessor
//	vim: ts=8 sw=8 noet:
//

#pragma once

#include <string>
#include "wiring.h"
#include "usim.h"
#include "bits.h"

union mos6502_status {
	uint8_t         value;
	template <int offset> using bit = ByteBits<mos6502_status, offset>;

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

class mos6502 : virtual public USimLE {

protected: // Processor addressing modes

	enum {
				accumulator,
				immediate,
				absolute,
				zeropage,
				zpindexed,
				xindexed,
				yindexed,
				implied,
				relative,
				xindirect,
				yindirect,
				absindirect
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
	mos6502_status		p;		// Processor status
	Byte			ir;		// Instruction register

private:	// internal processor state
	// placeholder for future use

private:	// instruction and operand fetch and decode
	void			execute_instruction();
        void			fetch_instruction();
        void			decode_mode();
        Byte			fetch_operand();
	Word			fetch_effective_address();

private:	// instruction implementations
	void			adc();
	void			and_();
	void			asl();
	void			bcc(), bcs();
	void			beq();
	void			bit();
	void			bmi(), bne();
	void			bpl();
	void			brk();
	void			bvc(), bvs();
	void			clc(), cld(), cli(), clv();
	void			cmp(), cpx(), cpy();
	void			dec(), dex(), dey();
	void			eor();
	void			inc(), inx(), iny();
	void			jmp(), jsr();
        void			lda(), ldx(), ldy();
        void			lsr();
	void			nop();
	void			ora();
	void			pha(), php();
	void			pla(), plp();
        void			rol(), ror();
        void			rti(), rts();
	void			sbc();
	void			sec(), sed(), sei();
	void			sta(), stx(), sty();
	void			tax(), tay();
	void			txa(), tya();
	void			tsx(), txs();

protected:	// helper functions
        void			set_nz(Byte val);
        void			help_asl(Byte &val);
        void			help_cmp(Byte reg, Byte value);
	void			help_ld(Byte& reg);
	void			help_lsr(Byte& val);
	void			help_rol(Byte &val);
	void			help_ror(Byte &val);

protected:	// overloadable functions (e.g. for breakpoints)
	virtual void		do_br(const char *, bool);

	virtual void		do_psh(Byte);
	virtual void		do_psh(Word);
	virtual void		do_pul(Byte&);
	virtual void		do_pul(Word&);

	virtual void		do_nmi();
	virtual void		do_irq();
	virtual void		do_brk();

	virtual void		pre_exec();
	virtual void		post_exec();

protected: 	// instruction tracing
	Word			insn_pc;
	// Word			operand;

	const char *		disasm_opcode(Word addr);
	// std::string		disasm_operand();
	// std::string		disasm_indexed();

public:		// external signal pins
	InputPin		IRQ, NMI;

public:
				mos6502();		// public constructor
	virtual			~mos6502();		// public destructor

	virtual void		reset();		// CPU reset
	virtual void		tick();

	virtual void		print_regs();
};

inline void mos6502::do_br(const char *mnemonic, bool test)
{
	(void)mnemonic;
	Word next = fetch_effective_address();
	if (test) {
		pc = next;
		++cycles;
	}
}

inline void mos6502::do_psh(Byte val)
{
	write(stack_base + s, val);
	--s;
}

inline void mos6502::do_psh(Word val)
{
	do_psh((Byte)val);
	do_psh((Byte)(val >> 8));
}

inline void mos6502::do_pul(Byte& val)
{
	++s;
	val = read(0x100 + s);
}

inline void mos6502::do_pul(Word& val)
{
	uint8_t lsb, msb;
	do_pul(msb);
	do_pul(lsb);
	val = (msb << 8) | lsb;
}
