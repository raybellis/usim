#ifndef SIDFORTH_H
#define SIDFORTH_H

#include "mc6809_X.h"
#include "diskio.h"
#include "ttyio.h"

/*
 * virtual sidforth device
 */
class sidforth : public mc6809_X
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
