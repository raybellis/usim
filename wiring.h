//
//
//	wiring.h
//
//	(C) R.P.Bellis 2021
//
//

#pragma once

#include <functional>

//---------------------------------------------------------------------
//
// helper functions for InputPin and Input Port
//
static inline constexpr bool default_true() {
	return true;
}

static inline constexpr uint8_t default_high() {
	return 0xff;
}

//---------------------------------------------------------------------
//
// OutputPin:
//
// An object providing read-only access to a reference
// to a bool, representing the state of an IO pin.
//
// If the `invert` flag is set then it indicates a pin
// that outputs the opposite of the referenced variable.
//
class OutputPin {

protected:
	bool&			val;
	const bool		invert;

public:
				OutputPin(bool& val, bool invert = false)
					: val(val), invert(invert)
				{ }

				operator bool() const { return val ^ invert; }
};

//---------------------------------------------------------------------
//
// OutputPinReg:
//
// An object providing read-only access to a single bit
// within uint8_t, representing the state of an IO pin.
//
// Use this to handle cases like the 6522 VIA, where the
// ~IRQ pin always represents the value of IFR bit 7.
//
// If the `invert` flag is set then it indicates a pin
// that outputs the opposite of the referenced bit.
//
class OutputPinReg {

protected:
	uint8_t&		val;
	const uint8_t		mask;
	const bool		invert;

public:
				OutputPinReg(uint8_t& val, uint8_t bit, bool invert = false)
					: val(val), mask(1 << bit), invert(invert)
				{ }

				operator bool() const {
					return (bool)(val & mask) ^ invert;
				}
};

//---------------------------------------------------------------------
//
// OutputPort:
//
// An object providing read-only access to the N least
// significant bits of a uint8_t
//
template<size_t N = 8>
class OutputPort {

	static_assert(N <= 8, "OutputPort too large");

protected:
	uint8_t&		data;
	const uint8_t		mask = (1 << N) - 1;

public:
				OutputPort(uint8_t& data)
					: data(data)
				{ }

				operator uint8_t() const {
					return data & mask;
				}
};

//---------------------------------------------------------------------
//
// InputPin:
//
// An object that represents a single input pin, where
// the current value of that pin is determined by calling
// a developer-supplied function that typically will
// poll the state of one or more Output* objects
//
class InputPin {

protected:
	using Function = std::function<bool()>;

protected:
	Function		f = default_true;

public:
	void			bind(const Function& _f) {
					f = _f;
				}

	bool			get() const {
					return f();
				}

	operator		bool() const {
					return get();
				}
};

//---------------------------------------------------------------------
//
// InputPort<N>
//
// An object that represents an N-bit input port, where
// the current value of the port is determined by calling
// a developer-supplied function that typically will
// poll the state of one or more Output* objects
//
// The value may also be read by casting to (uint8_t), and
// will be masked to be within the expected legal range.
//
template<size_t N>
class InputPort {

	static_assert(N <= 8, "InputPort too large");
	using Function = std::function<uint8_t()>;

protected:
	Function		f = default_high;

protected:
	const uint8_t		mask = (1 << N) - 1;


public:
	void			bind(const Function& _f) {
					f = _f;
				}

	uint8_t			get() const {
					return f() & mask;
				}

				operator uint8_t() const {
					return get();
				}
};
