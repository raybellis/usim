//
//
//	device.h
//
//	(C) R.P.Bellis 2021
//
//

#pragma once

#include <memory>
#include <vector>
#include <functional>
#include "typedefs.h"

/*
 * an abstract device that responds to CPU cycle ticks and might be reset
 */
class ActiveDevice {

public:
	virtual void		reset() = 0;
	virtual void		tick(uint8_t cycles) = 0;

public:
	using shared_ptr = std::shared_ptr<ActiveDevice>;

	virtual			~ActiveDevice() {};
};

/*
 * an abstract memory mapped device
 */
class MappedDevice {

public:
	virtual Byte		read(Word offset) = 0;
	virtual void		write(Word offset, Byte val) = 0;

public:
	using shared_ptr = std::shared_ptr<MappedDevice>;

	virtual			~MappedDevice() {};
};

/*
 * an abstract class combining the above two features
 */
class ActiveMappedDevice : virtual public ActiveDevice, virtual public MappedDevice {
public:
	using shared_ptr = std::shared_ptr<ActiveMappedDevice>;

	virtual			~ActiveMappedDevice() {};
};

/*
 * a container for mapping from memory locations to MappedDevices
 */
struct MappedDeviceEntry {
	MappedDevice::shared_ptr	device;
	Word				base;
	Word				mask;
};

/*
 * a container for ActiveDevices {
 */
struct ActiveDeviceEntry {
	ActiveDevice::shared_ptr	device;
};

typedef std::vector<ActiveDeviceEntry> ActiveDevList;
typedef std::vector<MappedDeviceEntry> MappedDevList;

/*
 * template to resolve smart point ambiguity issues
 * see https://stackoverflow.com/questions/66032442/
 */
template<int n> struct rank : rank<n - 1> {};
template<>      struct rank<0> {};

/*
 * IO pins - currently only used for interrupts
 */

static inline bool default_true() {
	return true;
}

class OutputPin {

public:
	using Function = std::function<bool()>;

protected:
	Function	f = default_true;

public:
			OutputPin() { }

			OutputPin(const Function& f) : f(f) { }

	void		bind(const Function& _f) {
				f = _f;
			}

			operator bool() const {
				return f();
			}

	OutputPin	operator !() const {
				return OutputPin([&]() {
					return !f();
				});
			}
};

class InputPin {

protected:
	OutputPin	input;

public:
	void		attach(const OutputPin& _input) {
				input = _input;
			}

	operator	bool() const {
				return input;
			}
};

inline void operator<<(InputPin& in, const OutputPin& out)
{
	in.attach(out);
}

inline OutputPin operator&(const OutputPin& a, const OutputPin& b)
{
	return OutputPin([=]() {
		return (bool)a & (bool)b;
	});
}

inline OutputPin operator|(const OutputPin& a, const OutputPin& b)
{
	return OutputPin([=]() {
		return (bool)a | (bool)b;
	});
}

inline OutputPin operator^(const OutputPin& a, const OutputPin& b)
{
	return OutputPin([=]() {
		return (bool)a ^ (bool)b;
	});
}
