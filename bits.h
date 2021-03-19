//
//
//	bits.h
//
//	(C) R.P.Bellis 1993 - 2021
//
//

#pragma once

#include "typedefs.h"

//  8 bit operations
inline bool btst(Byte x, int n)
{
	return (x & (1 << n)) ? true : false;
}

inline void bset(Byte& x, int n)
{
	x |= (1 << n);
}

inline void bclr(Byte& x, int n)
{
	x &= ~(1 << n);
}

// 16 bit operations
inline bool btst(Word x, int n)
{
	return (x & (1 << n)) ? true : false;
}

inline void bset(Word& x, int n)
{
	x |= (1 << n);
}

inline void bclr(Word& x, int n)
{
	x &= ~(1 << n);
}

// 32 bit operations
inline bool btst(DWord x, int n)
{
	return (x & (1L << n)) ? true : false;
}

inline void bset(DWord& x, int n)
{
	x |= (1L << n);
}

inline void bclr(DWord& x, int n)
{
	x &= ~(1L << n);
}

// Bit extend operations
inline Word extend5(Byte x)
{
	if (x & 0x10) {
		return (Word)x | 0xffe0;
	} else {
		return (Word)x;
	}
}

inline Word extend8(Byte x)
{
	if (x & 0x80) {
		return (Word)x | 0xff00;
	} else {
		return (Word)x;
	}
}
