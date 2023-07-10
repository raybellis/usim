//
//	wd1770.cpp
//
//	(C) R.P.Bellis 2023
//

#include <cassert>
#include "wd1770.h"

wd1770::wd1770(delegate_t& delegate)
  :	delegate(delegate),
	state(inactive),
	intrq(false),
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
	cycles += ticks;
	if (cycles < 20) return;
	cycles = 0;

	switch (state) {
		case inactive:
			break;

		case starting:
			command_start();
			break;

		case executing:
			command_next();
			break;

		case ending:
			command_end();
			break;

		default:
			break;
	}
}

void wd1770::command_start()
{
	switch ((cr & 0xf0) >> 4) {
		case 0x0:
			cmd = CMD_RESTORE;
			type = 1;
			track = 80;
			dsr = 0;
			break;
		case 0x1:
			cmd = CMD_SEEK;
			type = 1;
			dsr = dr;
			break;
		case 0x2:
		case 0x3:
			cmd = CMD_STEP;
			type = 1;
			break;
		case 0x4:
		case 0x5:
			cmd = CMD_STEP_IN;
			dir = 1;
			type = 1;
			break;
		case 0x6:
		case 0x7:
			cmd = CMD_STEP_OUT;
			dir = 0;
			type = 1;
			break;
		case 0x8:
		case 0x9:
			cmd = CMD_SECTOR_READ;
			type = 2;
			break;
		case 0xa:
		case 0xb:
			cmd = CMD_SECTOR_WRITE;
			type = 2;
			break;
		case 0xc:
			cmd = CMD_ADDRESS_READ;
			type = 3;
			break;
		case 0xd:
			cmd = CMD_FORCE_INTERRUPT;
			type = 4;
			break;
		case 0xe:
			cmd = CMD_TRACK_READ;
			type = 3;
			break;
		case 0xf:
			cmd = CMD_TRACK_WRITE;
			type = 3;
			break;
	}

	sr |= SR_BUSY;
	state = executing;
}

void wd1770::command_type_1_next()
{
	sr |= SR_MOTOR_OK;
	sr |= SR_MOTOR;

	switch (cmd) {

		case CMD_STEP_IN:
		case CMD_STEP_OUT:
		case CMD_STEP:
			if (dir) {
				++track;
			} else {
				if (track) {
					--track;
				}
			}
			state = ending;
			break;

		case CMD_RESTORE:
		case CMD_SEEK:
			if (track == dsr) {
				state = ending;
			} else {
				if (track > dsr) {
					dir = false;
					if (track) {
						--track;
					}
				} else {
					dir = true;
					++track;
				}
			}
			break;

		default:
			assert(false);
	}

	// update track 0 status
	if (track) {
		sr |= SR_NOT_ZERO;
	} else {
		sr &= ~SR_NOT_ZERO;
	}
}

void wd1770::command_next()
{
	switch (type) {

		case 1:
			command_type_1_next();
			break;

		case 2:
			break;

		case 3:
			break;

		default:
			break;
	}
}

void wd1770::command_end()
{
	state = inactive;
	sr &= ~SR_BUSY;
	intrq = true;
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
			sr &= ~SR_DRQ;
			return dr;
			break;
	}

	assert(false);
}

void wd1770::write(Word offset, Byte val)
{
	uint8_t reg = (offset & 0x03);

	// registers can't be written if busy, except force interrupt
	bool force_irq = (reg == REG_CMD) && ((val & 0xf0) == 0xd0);

	if (sr & SR_BUSY) {
		if (force_irq) {
			state = ending;
		}
		return;
	}

	switch (reg) {
		case REG_CMD:
			cr = val;
			intrq = false;
			state = starting;
			break;
		case REG_TRACK:
			track = val;
			break;
		case REG_SECTOR:
			sector = val;
			break;
		case REG_DATA:
			dr = val;
			break;
	}
}
