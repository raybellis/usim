//
//
//	misc.h
//
//	(C) R.P.Bellis 1994
//
//

#ifndef __misc_h__
#define __misc_h__

#include "typedefs.h"

extern int	btst(Byte x, int n);
extern void	bset(Byte& x, int n);
extern void	bclr(Byte& x, int n);

extern int	btst(Word x, int n);
extern void	bset(Word& x, int n);
extern void	bclr(Word& x, int n);

extern int	btst(DWord x, int n);
extern void	bset(DWord& x, int n);
extern void	bclr(DWord& x, int n);

extern Word	extend5(Byte x);
extern Word	extend8(Byte x);

extern char	*binstr(Byte x);
extern char	*hexstr(Byte x);
extern char	*hexstr(Word x);
extern char	*ascchr(Byte x);

#endif // __misc_h__
