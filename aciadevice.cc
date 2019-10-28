#include "aciadevice.h"

Byte aciadevice::read(Word addr)
{
	Byte		ret = 0;

	if ((addr & 0xfffe) == 0xc000) {
		ret = uart.read(addr);
	} else {
		ret = mc6809_X::read(addr);
	}
	return ret;
}

void aciadevice::write(Word addr, Byte c)
{
	if ((addr & 0xfffe) == 0xc000) {
		uart.write(addr, c);
	} else {
		mc6809_X::write(addr, c);
	}
}
