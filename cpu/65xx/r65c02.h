//
//	r65c02.h
//	(C) R.P.Bellis 2026 -
//	Rockwell R65C02: cmos6502 + RMB/SMB/BBR/BBS bit-manipulation ops.
//	Also matches CMD/GTE G65SC02-mask variants that include the bit ops,
//	and serves as the base for WDC's W65C02S (which adds WAI/STP).
//	vim: ts=8 sw=8 noet:
//

#pragma once

#include "cmos6502.h"

class r65c02 : virtual public cmos6502 {

protected:	// dispatch overrides
	virtual mode_t		decode_mode(Byte ir) override;
	virtual void		execute_instruction() override;
	virtual Word		fetch_effective_address() override;
	virtual const char*	disasm_opcode(Byte ir) override;

protected:	// Rockwell bit-op implementations
	void			rmb(Byte bit);
	void			smb(Byte bit);
	void			bbr(Byte bit);
	void			bbs(Byte bit);

public:
				r65c02();
	virtual			~r65c02();
};
