//
//
//	device.h
//
//	(C) R.P.Bellis 2021
//
//

#pragma once

#include <vector>
#include "typedefs.h"

/*
 * an abstract memory mapped device
 */
class Device {

public:
	virtual void		reset() = 0;
	virtual void		tick(uint64_t cycles) = 0;

public:
	virtual Byte		read(Word offset) = 0;
	virtual void		write(Word offset, Byte val) = 0;

};

/*
 * an abstract device with no-op functions for reset and tick
 */
class PassiveDevice : public Device {

public:
	virtual void		reset() final {};
	virtual void		tick(uint64_t cycles) final {};

};

/*
 * a mapping from memory locations to Devices
 */
struct DeviceEntry {
	Device*			device;
	Word			base;
	Word			mask;
};

typedef std::vector<DeviceEntry> Devices;

/*
 * IO pins - currently only used for interrupts
 *
 * code assumes wire-and, i.e. any low input forces output low
 */

class OutputPin {
protected:
	bool			state = true;

public:
	operator		bool() const {
					return state;
				}

	void			set(bool _state) {
					state = _state;
				}
};

typedef std::vector<OutputPin*> OutputPins;

class InputPin {

protected:
	OutputPins		inputs;

public:
	void			attach(OutputPin& input) {
					inputs.push_back(&input);
				}

	operator		bool() const {
					for (auto& i : inputs) {
						if (!(*i)) {
							return false;
						}
					}
					return true;
				}
};
