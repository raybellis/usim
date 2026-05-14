//
//	w65c02s.h
//	(C) R.P.Bellis 2026 -
//	WDC W65C02S: r65c02 + WAI (wait-for-interrupt) and STP (stop-the-clock).
//	vim: ts=8 sw=8 noet:
//

#pragma once

#include "r65c02.h"

class w65c02s : virtual public r65c02 {

protected:	// halt state
	bool			waiting;
	bool			stopped;

protected:	// dispatch overrides
	virtual mode_t		decode_mode(Byte ir) override;
	virtual void		execute_instruction() override;
	virtual const char*	disasm_opcode(Byte ir) override;

protected:	// instruction implementations
	void			wai();
	void			stp();

public:
				w65c02s();
	virtual			~w65c02s();

	virtual void		reset() override;
	virtual void		tick() override;
};
