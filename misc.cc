//
//
//	misc.cc
//
//	(C) R.P.Bellis 1994
//
//

#include "usim.h"

//  8 bit operations
int btst(Byte x, int n)
{
	return (x & (1 << n)) ? 1 : 0;
}

void bset(Byte& x, int n)
{
	x |= (1 << n);
}

void bclr(Byte& x, int n)
{
	x &= ~(1 << n);
}

// 16 bit operations
int btst(Word x, int n)
{
	return (x & (1 << n)) ? 1 : 0;
}

void bset(Word& x, int n)
{
	x |= (1 << n);
}

void bclr(Word& x, int n)
{
	x &= ~(1 << n);
}

// 32 bit operations
int btst(DWord x, int n)
{
	return (x & (1L << n)) ? 1 : 0;
}

void bset(DWord& x, int n)
{
	x |= (1L << n);
}

void bclr(DWord& x, int n)
{
	x &= ~(1L << n);
}

// Bit extend operations
Word extend5(Byte x)
{
	if (x & 0x10) {
		return (Word)x | 0xffe0;
	} else {
		return (Word)x;
	}
}

Word extend8(Byte x)
{
	if (x & 0x80) {
		return (Word)x | 0xff00;
	} else {
		return (Word)x;
	}
}

// Base 2 and Base 16 conversion functions
char *binstr(Byte x)
{
	static char		tmp[9] = "        ";

	for (int i = 7; i >= 0; --i) {
		tmp[i] = (x & 1) + '0';
		x >>= 1;
	}

	return tmp;
}

static char hex_digit(Byte x)
{
	x &= 0x0f;
	if (x <= 9) {
		return '0' + x;
	} else {
		return 'a' + x - 10;
	}
}

char *hexstr(Byte x)
{
	static char		tmp[3] = "  ";

	tmp[1] = hex_digit(x);	x >>= 4;
	tmp[0] = hex_digit(x);

	return tmp;
}

char *hexstr(Word x)
{
	static char		tmp[5] = "    ";

	tmp[3] = hex_digit(x);	x >>= 4;
	tmp[2] = hex_digit(x);	x >>= 4;
	tmp[1] = hex_digit(x);	x >>= 4;
	tmp[0] = hex_digit(x);

	return tmp;
}

char *ascchr(Byte x)
{
	static char		tmp[2] = " ";

	x &= 0x7f;
	tmp[0] = ((x >= 0x20) && (x < 0x7f)) ? x : '.';

	return tmp;
}
