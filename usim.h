//
//
//	usim.h
//
//	(C) R.P.Bellis 1994
//
//

#pragma once

#include "device.h"
#include <stdexcept>

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
		uint64_t	cycles = 0;

// Generic internal registers that we assume all CPUs have

		Word		ir;
		Word		pc;

// Generic read/write/execute functions
protected:

	virtual Byte		read(Word offset);
	virtual Word		read_word(Word offset) = 0;
	virtual void		write(Word offset, Byte val);
	virtual void		write_word(Word offset, Word val) = 0;
	virtual Byte		fetch();
	virtual void		execute() = 0;

// Device handling:
protected:
		Devices		devices;

public:
	virtual void		attach(Device& dev, Word base, Word mask);

// Functions to start and stop the virtual processor
public:

	virtual void		run();
	virtual void		tick();
	virtual void		halt();
	virtual void		reset() = 0;

// Debugging
		void		tron() { m_trace = true; };
		void		troff() { m_trace = false; };

// Exceptions
public:
	class execution_error : virtual public std::runtime_error {
	public:
		execution_error(const char *msg) : std::runtime_error(msg) { };
	};

};

class USimMotorola : virtual public USim {

// Memory access functions taking target byte order into account
protected:
	virtual Word		fetch_word();

	virtual Word		read_word(Word offset);
	virtual void		write_word(Word offset, Word val);

};

class USimIntel : virtual public USim {

// Memory access functions taking target byte order into account
protected:
	virtual Word		fetch_word();

	virtual Word		read_word(Word offset);
	virtual void		write_word(Word offset, Word val);

};
