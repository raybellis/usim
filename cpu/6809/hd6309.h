//
//	hd6309.h
//	(C) R.P.Bellis 2026 -
//	Hitachi HD6309: superset of the MC6809. Adds the E, F, W, V and MD
//	registers, the Q (D:W) 32-bit accumulator view, the W (E:F) 16-bit
//	view, plus ~70 new instructions and a native operating mode.
//	After reset the CPU is in emulation mode (MD bit 0 = 0) and behaves
//	exactly like an MC6809.
//	vim: ts=8 sw=8 noet:
//

#pragma once

#include "mc6809.h"

union hd6309_md {
	uint8_t		value;
	template <int offset> using bit = register_bits<uint8_t, offset, bool, 1>;
	bit<0>		nm;	// 0 = emulation mode, 1 = native mode
	bit<1>		fm;	// FIRQ uses entire stack frame
	bit<6>		il;	// illegal-instruction trap occurred
	bit<7>		dz;	// divide-by-zero trap occurred
	operator uint8_t() const { return value; }
	uint8_t operator =(uint8_t n) { return value = n; }
};

class hd6309 : virtual public mc6809 {

protected:	// 6309-only registers

	// W combines E (high) and F (low), mirroring mc6809's d / a / b.
	union {
		Word			w;
		struct {
#if __BYTE_ORDER == __LITTLE_ENDIAN
			Byte		f;
			Byte		e;
#elif __BYTE_ORDER == __BIG_ENDIAN
			Byte		e;
			Byte		f;
#endif
		};
	};

	Word			v;	// 16-bit scratch register
	hd6309_md		md;	// mode / trap-status register

protected:	// 32-bit Q (D:W) accessors
	DWord			getq() const { return ((DWord)d << 16) | w; }
	void			setq(DWord val) {
					d = (Word)(val >> 16);
					w = (Word)val;
				}

protected:	// scratch for the "0 register" (TFR/EXG codes $C and $D, byte
		// position only). Reset to 0 before each reference return so
		// reads see zero; any write through the returned reference is
		// discarded by the next reset.
	Byte			zero_byte = 0;

protected:	// dispatch overrides
	virtual void		execute_instruction() override;
	virtual void		tfr() override;
	virtual void		exg() override;

protected:	// interrupt entry / exit (native mode adds E and F to the
		// entire stack frame; FIRQ uses the entire frame when MD.fm
		// is set)
	virtual void		do_nmi() override;
	virtual void		do_firq() override;
	virtual void		do_irq() override;
	virtual void		swi() override;
	virtual void		swi2() override;
	virtual void		swi3() override;
	virtual void		cwai() override;
	virtual void		rti() override;

protected:	// shared entire-frame push/pull, 6309-aware
	void			push_entire(Word& sp);
	void			pull_entire(Word& sp);

protected:	// MD register access
	void			ldmd();
	void			bitmd();

protected:	// load/store on the 6309 accumulators
	void			lde(), ldf(), ldw(), ldq();
	void			ste(), stf(), stw(), stq();

protected:	// ADD/SUB/CMP on the 6309 accumulators
	void			adde(), addf(), addw();
	void			sube(), subf(), subw();
	void			cmpe(), cmpf(), cmpw();

protected:	// INC/DEC/NEG/COM/TST/CLR on the 6309 accumulators
	void			incd(), ince(), incf(), incw();
	void			decd(), dece(), decf(), decw();
	void			negd();
	void			comd(), come(), comf(), comw();
	void			tstd(), tste(), tstf(), tstw();
	void			clrd(), clre(), clrf(), clrw();

protected:	// shift/rotate on D and W; sign-extend W into Q
	void			asld(), asrd(), lsrd(), rold(), rord();
	void			aslw(), asrw(), lsrw(), rolw(), rorw();
	void			sexw();

protected:	// push/pull the W register on either stack
	void			pshsw(), pulsw();
	void			pshuw(), puluw();

protected:	// in-memory bit logic: AIM/OIM/EIM/TIM
	void			aim(), oim(), eim(), tim();

protected:	// signed multiply and divide
	void			muld();
	void			divd();
	void			divq();

protected:	// block transfer (TFM r+,r+ / r-,r- / r+,r / r,r+)
	void			tfm();

protected:	// CC/memory single-bit transfer family
		// (BAND, BIAND, BOR, BIOR, BEOR, BIEOR, LDBT, STBT)
	void			bit_transfer();

protected:	// 16-bit forms of the byte helpers (6309-only). The using
		// declarations re-import the base's Byte overloads so callers
		// in this class see both.
	using mc6809::help_clr;
	using mc6809::help_com;
	using mc6809::help_dec;
	using mc6809::help_inc;
	using mc6809::help_neg;
	using mc6809::help_tst;
	using mc6809::help_asr;
	using mc6809::help_lsl;
	using mc6809::help_lsr;
	using mc6809::help_rol;
	using mc6809::help_ror;

	void			help_clr(Word&);
	void			help_com(Word&);
	void			help_dec(Word&);
	void			help_inc(Word&);
	void			help_neg(Word&);
	void			help_tst(Word);

	void			help_asr(Word&);
	void			help_lsl(Word&);
	void			help_lsr(Word&);
	void			help_rol(Word&);
	void			help_ror(Word&);

public:
				hd6309();		// public constructor
	virtual			~hd6309();		// public destructor

	virtual void		reset() override;	// CPU reset

	virtual void		print_regs() override;

	virtual Byte&		byterefreg(int r) override;
	virtual Word&		wordrefreg(int r) override;
};
