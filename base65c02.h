//
//	base65c02.h
//	(C) R.P.Bellis 2026 -
//	Class definition for the common WDC 65C02 CMOS core
//	(no RMB/SMB/BBR/BBS/WAI/STP).
//	vim: ts=8 sw=8 noet:
//

#pragma once

#include "mos6502.h"

class base65c02 : virtual public mos6502 {

protected:	// dispatch overrides
	virtual mode_t		decode_mode(Byte ir) override;
	virtual void		execute_instruction() override;
	virtual Word		fetch_effective_address() override;
	virtual const char*	disasm_opcode(Byte ir) override;

protected:	// CMOS-specific instruction implementations
	void			bra();
	void			stz();

public:
				base65c02();
	virtual			~base65c02();
};
