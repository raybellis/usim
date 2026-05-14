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
	virtual void		tfr() override;
	virtual void		exg() override;

public:
				hd6309();		// public constructor
	virtual			~hd6309();		// public destructor

	virtual void		reset() override;	// CPU reset

	virtual void		print_regs() override;

	virtual Byte&		byterefreg(int r) override;
	virtual Word&		wordrefreg(int r) override;
};
