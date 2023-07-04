//
//	WD1770/2 emulation
//
//	(C) R.P.Bellis 2023-
//

#pragma once

#include "usim.h"

class wd1770 : virtual public ActiveMappedDevice {

public:
	class impl_t {
	};

protected:
	impl_t&				impl;

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

	Byte				sr;				// status register
	Byte				cmd;			// command register
	Byte				track;
	Byte				sector;
	Byte				data;
	bool				intrq;

protected:
	virtual void		reset();
	virtual void		tick(uint8_t);

	void				command_type1(Byte);
	void				command_type2(Byte);
	void				command_type3(Byte);
	void				force_interrupt(Byte);

	void				command(Byte);

public:
	virtual Byte		read(Word offset);
	virtual void		write(Word offset, Byte val);

public:
	OutputPin			INTRQ;
	OutputPinReg		DRQ;

public:

						wd1770(impl_t& impl);
	virtual				~wd1770();

};
