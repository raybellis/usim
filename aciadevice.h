#ifndef ACIADEVICE_H
#define ACIADEVICE_H

#include "mc6809_X.h"
#include "mc6850.h"

class aciadevice : public mc6809_X {
protected:

	virtual Byte			 read(Word);
    virtual void			 write(Word, Byte);

private:
    mc6850			 uart;
};

#endif // ACIADEVICE_H
