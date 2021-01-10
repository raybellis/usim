//
//
//	memory.h
//
//	(C) R.P.Bellis 2021
//
//

#pragma once

#include "usim.h"

class Memory : public Device {

protected:
	std::vector<uint8_t>	memory;
	Word			size;

public:
				Memory(Word size) : memory(size) {};
	virtual void		reset() {};
	virtual void		tick() {};

public:
	virtual Byte		read(Word offset) { return memory[offset]; };

};

class RAM : public Memory {

public:
				RAM(Word size);
	virtual void		write(Word offset, Byte val) { memory[offset] = val; };
};

class ROM : public Memory {

public:
	virtual void		write(Word offset, Byte val) {};

public:
				ROM(Word size);
		void		load_intelhex(const char *filename);

};
