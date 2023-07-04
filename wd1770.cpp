//
//	wd1770.cpp
//
//	(C) R.P.Bellis 2023
//

#include "wd1770.h"

wd1770::wd1770(impl_t& impl)
  :	impl(impl),
	INTRQ(intrq),
	DRQ(sr, 1)
{
}

wd1770::~wd1770()
{
}

void wd1770::reset()
{
}

void wd1770::tick(uint8_t ticks)
{
}

void wd1770::command_type1(Byte cmd)
{
	uint8_t rate = (cmd >> 0) & 0b11;
	bool v = (cmd >> 2) & 1;
	bool h = (cmd >> 3) & 1;

	// step commands
	if ((cmd & 0x60) != 0x00) {
		bool u = (cmd >> 4) & 1;
	}
}

void wd1770::command_type2(Byte cmd)
{
}

void wd1770::command_type3(Byte cmd)
{
}

void wd1770::force_interrupt(Byte cmd)
{
}

void wd1770::command(Byte cmd)
{
	if ((cmd & 0x80) == 0x00) {
		command_type1(cmd);
	} else if ((cmd & 0xc0) == 0x80) {
		command_type2(cmd);
	} else if ((cmd & 0xf0) == 0xd0) {
		force_interrupt(cmd);
	} else {
		command_type3(cmd);
	}
	this->cmd = cmd;
}

Byte wd1770::read(Word offset)
{
	uint8_t reg = (offset & 0x03);
	switch (reg) {
		case REG_SR:
			intrq = false;
			return sr;
			break;
		case REG_TRACK:
			return track;
			break;
		case REG_SECTOR:
			return sector;
			break;
		case REG_DATA:
			return data;
			break;
		default:
			assert(false);
	}
}

void wd1770::write(Word offset, Byte val)
{
	uint8_t reg = (offset & 0x03);
	switch (reg) {
		case REG_CMD:
			command(val);
			break;
		case REG_TRACK:
			track = val;
			break;
		case REG_SECTOR:
			sector = val;
			break;
		case REG_DATA:
			data = val;
			break;
	}
}
