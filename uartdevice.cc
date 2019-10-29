#include "uartdevice.h"

Byte uartdevice::read(Word addr)
{
	Byte		ret = 0;

	if ((addr & 0xfffe) == 0xc000) {
		ret = uart.read(addr);
	} else {
        ret = INHERIT::read(addr);
	}
	return ret;
}

void uartdevice::write(Word addr, Byte c)
{
	if ((addr & 0xfffe) == 0xc000) {
		uart.write(addr, c);
	} else {
        INHERIT::write(addr, c);
	}
}
