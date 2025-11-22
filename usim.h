//
//	usim.h
//	(C) R.P.Bellis 1994 - 2025
//
//	vim: ts=8
//

#pragma once

#include <cstdlib>
#include "device.h"
#include "memory.h"
#include "wiring.h"
#include "bits.h"

/*
 * main system wide base class for CPU emulators
 *
 * assumes an 8 bit Von-Neumann architecture with a 16 bit
 * address space and memory mapped peripherals
 */
class USim {

// Generic processor state
protected:

		bool		m_trace = false;
		bool		halted = true;
		uint8_t		cycles = 0;

// Generic internal registers that we assume all CPUs have

		Word		ir;
		Word		pc;

// Generic read/write/execute functions
public:

	virtual Byte		read(Word offset);
	virtual Word		read_word(Word offset) = 0;
	virtual void		write(Word offset, Byte val);
	virtual void		write_word(Word offset, Word val) = 0;
	virtual Byte		fetch();

// Device handling:
protected:
	ActiveDevList	        dev_active;
	MappedDevList   	dev_mapped;

	virtual void		attach(const MappedDevice::shared_ptr& dev, Word base, Word mask, rank<0>);
	virtual void		attach(const ActiveMappedDevice::shared_ptr& dev, Word base, Word mask, rank<1>);

public:
	virtual void		attach(const ActiveDevice::shared_ptr& dev);

	template<typename T>
		void		attach(const std::shared_ptr<T>& dev, Word base, Word mask) {
					attach(dev, base, mask, rank<2>{});
				};

// Functions to start and stop the virtual processor
public:

	std::function<void()>	abort = ::abort;
	virtual void		invalid(const char*);
	virtual void		run();
	virtual void		tick();
	virtual void		halt();
	virtual void		reset();

// Debugging
		void		tron() { m_trace = true; };
		void		troff() { m_trace = false; };

};

class USimMotorola : virtual public USim {

// Memory access functions taking target byte order into account
public:
	virtual Word		fetch_word();

	virtual Word		read_word(Word offset);
	virtual void		write_word(Word offset, Word val);

};

class USimIntel : virtual public USim {

// Memory access functions taking target byte order into account
public:
	virtual Word		fetch_word();

	virtual Word		read_word(Word offset);
	virtual void		write_word(Word offset, Word val);

};
