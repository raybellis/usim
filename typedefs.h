//
//
//	typedefs.h
//
//	(C) R.P.Bellis 1994
//
//

#ifndef __typedefs_h__
#define __typedefs_h__

#include "machdep.h"

typedef unsigned char	Byte;
typedef unsigned short	Word;

#if (MACH_INT_SIZE == 4)
typedef unsigned int	DWord;
#else
typedef unsigned long	DWord;
#endif

#endif // __typedefs_h__
