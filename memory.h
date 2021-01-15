//
//
//	memory.h
//
//	(C) R.P.Bellis 2021
//
//

#pragma once

#include "device.h"

/*
 * generic memory interface, dynamically assigned space
 */
class GenericMemory : public PassiveDevice {

protected:
	std::vector<Byte>	memory;
	size_t			size;

public:
				GenericMemory(size_t size) : memory(size), size(size) {};

public:
	virtual Byte		read(Word offset) {
					return (offset < size) ? memory[offset] : 0xff;
				};

	virtual void		write(Word offset, Byte val) {
					if (offset < size) {
						memory[offset] = val;
					}
				};
};

/*
 * RAM: can be written to
 */
class RAM : public GenericMemory {

public:
				RAM(size_t size) : GenericMemory(size) {};

};

/*
 * ROM: can't be written
 *      can be pre-loaded from a hex file
 */
class ROM : public GenericMemory {

public:
	virtual void		write(Word offset, Byte val) {};	// no-op
				ROM(size_t size) : GenericMemory(size) {};

public:
		void		load_intelhex(const char *filename, Word base);

};

/*
 * ROM_Data
 *
 * For use when embedding static firmware data inside a compiled image
 */
class ROM_Data : public PassiveDevice {

protected:
	const uint8_t*		memory;
	size_t			size;					// size of virtual device
	size_t			memsize;				// size of allocated data

public:
				ROM_Data(const uint8_t* p, size_t size) : memory(p), size(size), memsize(size) {};

				ROM_Data(const uint8_t* p, size_t size, size_t memsize) : memory(p), size(size), memsize(memsize) {};

	virtual Byte		read(Word offset) {
					if (offset < size && offset < memsize) {
						return memory[offset];
					} else {
						return 0xff;
					}
				}

	virtual void		write(Word offset, Byte val) {};	// no-op
};
