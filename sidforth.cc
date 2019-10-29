#include "sidforth.h"

Byte sidforth::read(Word addr)
{
    switch (addr & 0xfff0) {
    case disk_address: // Disk
		return disk.get(addr - disk_address);
    case tty_address:
        return terminal.get(addr - tty_address);
    default:
        return INHERIT::read(addr);
    }
}

void sidforth::write(Word addr, Byte c)
{
    switch (addr & 0xfff0) {
    case disk_address: // Disk
		disk.set(addr - disk_address, c); return;
    case tty_address:
        terminal.set(addr - tty_address, c); return;
    default:
        INHERIT::write(addr, c); return;
    }
}
