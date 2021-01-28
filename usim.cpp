//
//
//	usim.cpp
//
//	(C) R.P.Bellis 1994
//
//

#include <cstdlib>
#include <cstdio>
#include "usim.h"

//----------------------------------------------------------------------------
// Generic processor run state routines
//----------------------------------------------------------------------------

void USim::run()
{
	halted = false;
	while (!halted) {
		tick();
	}
}

void USim::tick()
{
	// update all attached devices
	for (auto& d : devices) {
		d.device->tick(cycles);
	}
}

void USim::halt()
{
	halted = true;
}

Byte USim::fetch()
{
	return read(pc++);
}

void USim::invalid(const char *msg)
{
	fprintf(stderr, "\r\ninvalid %s : pc = [%04x], ir = [%04x]\r\n",
		msg ? msg : "",
		pc, ir);
	halt();
}

//----------------------------------------------------------------------------
// Device handling
//----------------------------------------------------------------------------

void USim::attach(Device& dev, Word base, Word mask)
{
	devices.push_back(DeviceEntry { &dev, base, mask });
}

// Single byte read
Byte USim::read(Word offset)
{
	++cycles;
	for (auto& d : devices) {
		if ((offset & d.mask) == d.base) {
			return d.device->read(offset - d.base);
		}
	}
	return 0xff;
}

// Single byte write
void USim::write(Word offset, Byte val)
{
	++cycles;
	for (auto& d : devices) {
		if ((offset & d.mask) == d.base) {
			d.device->write(offset - d.base, val);
			break;
		}
	}
}

//----------------------------------------------------------------------------
// Word memory access routines for big-endian (Motorola type)
//----------------------------------------------------------------------------

Word USimMotorola::fetch_word()
{
	Word		tmp;

	tmp  = fetch() << 8;
	tmp |= fetch();

	return tmp;
}

Word USimMotorola::read_word(Word offset)
{
	Word		tmp;

	tmp  = read(offset++) << 8;
	tmp |= read(offset);

	return tmp;
}

void USimMotorola::write_word(Word offset, Word val)
{
	write(offset++, (Byte)(val >> 8));
	write(offset, (Byte)val);
}

//----------------------------------------------------------------------------
// Word memory access routines for little-endian (Intel type)
//----------------------------------------------------------------------------

Word USimIntel::fetch_word()
{
	Word		tmp;

	tmp  = fetch();
	tmp |= fetch() << 8;

	return tmp;
}

Word USimIntel::read_word(Word offset)
{
	Word		tmp;

	tmp = read(offset++);
	tmp |= (read(offset) << 8);

	return tmp;
}

void USimIntel::write_word(Word offset, Word val)
{
	write(offset++, (Byte)val);
	write(offset, (Byte)(val >> 8));
}
