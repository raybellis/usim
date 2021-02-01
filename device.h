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

template<std::size_t N>
class OutputPinGroup {
public:
	using Function = std::function<bool(size_t)>;

protected:
	Function	f;
	OutputPin	outputs[N];

public:
	OutputPinGroup(const Function&f) : f(f) { }

	const OutputPin&	operator[](size_t n) {
					return outputs[n];
				}
};

class InputPin {

protected:
	OutputPin*		input = nullptr;

public:
	void			attach(OutputPin& input) {
					this->input = &input;
				}

	operator		bool() const {
					return input == nullptr ? true : *input;
				}
};

template<std::size_t M>
class LogicSimple {

protected:
	InputPin		inputs[M];
	virtual bool		logic() = 0;

public:
	OutputPin		output;

public:
	LogicSimple(InputPin inputs[M]) : inputs(inputs), output([&]() { return logic();}) { };
	LogicSimple(InputPin input) : inputs({ inputs }), output([&]() { return logic();}) { };

};

template<std::size_t M, std::size_t N>
class Logic {

protected:
	InputPin		inputs[M];
	virtual bool		logic(size_t n) = 0;

public:
	OutputPinGroup<N>	outputs;

public:
	Logic(InputPin inputs[N]) : inputs(inputs), outputs([&](size_t n) { return logic(n);}) { };
	Logic(InputPin input) : inputs({ inputs }), outputs([&](size_t n) { return logic(n);}) { };

};

class LogicInverter : virtual public LogicSimple<1> {
protected:
	virtual bool		logic() {
					return !inputs[0];
				}
};

template<std::size_t N>
class LogicAND : virtual public LogicSimple<N> {

protected:
	virtual bool		logic() {
					for (auto& i : this->inputs) {
						if (!i) return false;
					}
					return true;
				}
};

template<std::size_t N>
class LogicOR : virtual public LogicSimple<N> {

protected:
	bool			logic() {
					for (auto& i : this->inputs) {
						if (i) return true;
					}
					return true;
				}
};
