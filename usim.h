//
//
//	usim.h
//
//	(C) R.P.Bellis 1994
//
//

#ifndef __usim_h__
#define __usim_h__

#include "machdep.h"
#include "typedefs.h"
#include "misc.h"

class USim {

// Generic processor state
protected:

		int		 halted;
		Byte		*memory;
		Byte		*port;

// Generic internal registers that we assume all CPUs have

		Word		ir;
		Word		pc;

// Generic read/write/execute functions
protected:

	virtual Byte		read(Word offset);
	virtual Word		read_word(Word offset) = 0;
	virtual void		write(Word offset, Byte val);
	virtual void		write_word(Word offset, Word val) = 0;
	virtual Byte		fetch(void);
	virtual Word		fetch_word(void);
	virtual void		execute(void) = 0;

// Functions to start and stop the virtual processor
public:

	virtual void		 run(void);
	virtual void		 step(void);
	virtual void		 halt(void);
	virtual void		 reset(void) = 0;
	virtual void		 status(void) = 0;
	virtual void		 invalid(const char * = 0);

// Function to load the processor state
public:

		void		 load_intelhex(const char *filename);

};

class USimMotorola : virtual public USim {

// Memory access functions taking target byte order into account
protected:

	virtual Word		read_word(Word offset);
	virtual void		write_word(Word offset, Word val);

};

class USimIntel : virtual public USim {

// Memory access functions taking target byte order into account
protected:

	virtual Word		read_word(Word offset);
	virtual void		write_word(Word offset, Word val);

};

#endif // __usim_h__
