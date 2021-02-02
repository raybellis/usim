//
//
//	device.h
//
//	(C) R.P.Bellis 2021
//
//

#pragma once

#include <functional>
#include <vector>
#include "typedefs.h"

/*
 * an abstract unmapped device that only handles reset and tick
 */
class UnmappedDevice {

public:
	virtual void		reset() = 0;
	virtual void		tick(uint8_t cycles) = 0;

};

/*
 * an abstract memory mapped device
 */
class Device : virtual public UnmappedDevice {

public:
	virtual Byte		read(Word offset) = 0;
	virtual void		write(Word offset, Byte val) = 0;

};

/*
 * a device with no-op functions for reset and tick
 */
class PassiveDevice : public Device {

public:
	virtual void		reset() final {};
	virtual void		tick(uint8_t cycles) final {};

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
typedef std::vector<UnmappedDevice*> UnmappedDevices;

/*
 * IO pins - currently only used for interrupts
 */

class OutputPin {

public:
	using Function = std::function<bool()>;

protected:
	Function	f;

public:
	OutputPin(const Function& f) : f(f) { }

	operator		bool() const {
					return f();
				}
};

class InputPin {

protected:
	std::vector<OutputPin>	inputs;

public:
	void			attach(OutputPin& input) {
					inputs.push_back(input);
				}

	operator		bool() const {
					for (auto& i : inputs) {
						if (!i) {
							return false;
						}
					}
					return true;
				}
};
