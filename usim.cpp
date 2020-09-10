//
//
//	usim.cc
//
//	(C) R.P.Bellis 1994
//
//

#include <stdlib.h>
#include "usim.h"

//----------------------------------------------------------------------------
// Generic processor run state routines
//----------------------------------------------------------------------------
void USim::run(void)
{
	halted = 0;
	while (!halted) {
		execute();
	}
	status();
}

void USim::step(void)
{
	execute();
	status();
}

void USim::halt(void)
{
	halted = 1;
}

Byte USim::fetch(void)
{
	Byte		val = read(pc);
	pc += 1;

	return val;
}

Word USim::fetch_word(void)
{
	Word		val = read_word(pc);
	pc += 2;

	return val;
}

//----------------------------------------------------------------------------
// Word memory access routines for big-endian (Motorola type)
//----------------------------------------------------------------------------

Word USimMotorola::read_word(Word offset)
{
	Word		tmp;

	tmp = read(offset++);
	tmp <<= 8;
	tmp |= read(offset);

	return tmp;
}

void USimMotorola::write_word(Word offset, Word val)
{
	write(offset++, (Byte)(val >> 8));
	write(offset, (Byte)val);
}

//----------------------------------------------------------------------------
// Word memory access routines for little-endian (Intel type)
//----------------------------------------------------------------------------

Word USimIntel::read_word(Word offset)
{
	Word		tmp;

	tmp = read(offset++);
	tmp |= (read(offset) << 8);

	return tmp;
}

void USimIntel::write_word(Word offset, Word val)
{
	write(offset++, (Byte)val);
	write(offset, (Byte)(val >> 8));
}
