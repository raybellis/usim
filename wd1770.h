//
//	WD1770/2 emulation
//
//	(C) R.P.Bellis 2023-
//

#pragma once

#include "usim.h"

class wd1770 : virtual public ActiveMappedDevice {

public:

	class delegate_t {

	protected:
		enum mode_t : uint8_t {
			read = 0,
			write = 1
		} mode;

	public:
		bool				write_protected();

		bool 				sector_read_start(uint8_t track, uint8_t sector);
		bool				sector_write_start(uint8_t track, uint8_t sector);

		bool				sector_read_next(uint8_t&);
		bool				sector_write_next(uint8_t);
	};

protected:
	delegate_t&			delegate;

protected:
	enum reg_t {
		REG_SR			= 0,
		REG_CMD			= 0,
		REG_TRACK		= 1,
		REG_SECTOR		= 2,
		REG_DATA		= 3
	};

	enum sr_flags_t : Byte {
		SR_BUSY			= 0x01,			// Busy					(type 1 / 2 / 3)
		SR_DRQ	 		= 0x02,			// Data Request			(type 1 / 2 / 3)
		SR_NOT_ZERO		= 0x04,			// Not track 0			(type 1)
		SR_LOST_DATA	= 0x04,			// Lost data / byte		(type 2 / 3)
		SR_CRC_ERROR	= 0x08,			// CRC error			(type 1 / 2 / 3)
		SR_NOT_FOUND	= 0x10,			// Record not found		(type 1 / 2 / 3)
		SR_MOTOR_OK		= 0x20,			// Motor spin-up		(type 1)
		SR_DELETED		= 0x20,			// Deleted data mark	(type 2 / 3)
		SR_WRITE_PROT	= 0x40,			// Disk write-protected	(type 2 / 3)
		SR_MOTOR		= 0x80			// Motor on				(type 1 / 2 / 3)
	};

	enum command_t {
		CMD_RESTORE,
		CMD_SEEK,
		CMD_STEP,
		CMD_STEP_IN,
		CMD_STEP_OUT,
		CMD_SECTOR_READ,
		CMD_SECTOR_WRITE,
		CMD_ADDRESS_READ,
		CMD_TRACK_READ,
		CMD_TRACK_WRITE,
		CMD_FORCE_INTERRUPT
	} cmd;

	enum state_t {
		inactive,
		starting,
		executing,
		ending,
	} state;

	Byte				sr;				// status register
	Byte				cr;				// command register
	Byte				track;
	Byte				sector;
	Byte				dr;				// data register
	Byte				dsr;			// data shift register

	uint8_t				type;
	uint16_t			offset;
	bool				dir;
	bool				intrq;

protected:
	virtual void		reset();
	virtual void		tick(uint8_t);
	uint16_t			cycles;

	void				command_type_1_next();

	void				command_start();
	void				command_next();
	void				command_end();

public:
	virtual Byte		read(Word offset);
	virtual void		write(Word offset, Byte val);

public:
	OutputPin			INTRQ;
	OutputPinReg		DRQ;

public:

						wd1770(delegate_t& delegate);
	virtual				~wd1770();

};
