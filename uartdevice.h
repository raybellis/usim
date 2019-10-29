#ifndef UARTDEVICE_H
#define UARTDEVICE_H

#include "mc6850.h"
#ifdef DEBUG
# include "mc6809_debug.h"
# define INHERIT mc6809_debug
#else
# include "mc6809_X.h"
# define INHERIT mc6809_X
#endif
class uartdevice : public INHERIT {
protected:

	virtual Byte			 read(Word);
    virtual void			 write(Word, Byte);

private:
    mc6850			 uart;
};

#endif // UARTDEVICE_H
