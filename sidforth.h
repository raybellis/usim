#ifndef SIDFORTH_H
#define SIDFORTH_H

#include "diskio.h"
#include "ttyio.h"

#ifdef DEBUG
#include "mc6809_debug.h"
# define INHERIT mc6809_debug
#else
#include "mc6809_X.h"
# define INHERIT mc6809_X
#endif
/*
 * virtual sidforth device
 */
class sidforth : public INHERIT
{
protected:
    virtual Byte			 read(Word);
    virtual void			 write(Word, Byte);
private:
    static const Word disk_address = 0xFC00;
    static const Word tty_address =  disk_address + 0x10;
    diskio disk;
    ttyio terminal;
};

#endif // SIDFORTH_H
