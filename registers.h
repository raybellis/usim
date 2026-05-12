//
//	registers.h
//	(C) R.P.Bellis 2025 -
//
//	vim: ts=8 sw=8 noet:
//

#pragma once

#include <type_traits>

//
//	a template to allow direct access to the individual bits or
//	bytes of a word, without needing explicit getters and setters
//
//	partially derived from https://andreashohmann.com/cpp-bitfields/
//

template<typename T, unsigned offset = 0, typename R = bool, unsigned size = 8 * sizeof(R)>
	requires std::is_unsigned_v<T> &&
		 std::is_unsigned_v<R> &&
		 (size <= 8 * sizeof(R)) &&
		 (offset + size <= 8 * sizeof(T))
class register_bits  {

private:
	constexpr T mask() const {
		return (1ULL << size) - 1;
	}

	T& value() {
		return *reinterpret_cast<T*>(this);
	}

	const T& value() const {
		return *reinterpret_cast<const T*>(this);
	}

public:
	operator R() const {
		const auto m = mask();
		const auto v = value();
		return (v >> offset) & m;
	}

	register_bits& operator=(R n) {
		const auto m = mask();
		auto& v = value();
		n &= m;
		v &= ~(m << offset);
		v |= (n << offset);
		return *this;
	}

	register_bits& operator ^=(R n) {
		const auto m = mask();
		auto& v = value();
		n &= m;
		v ^= (n << offset);
		return *this;
	}

	register_bits& operator |=(R n) {
		const auto m = mask();
		auto& v = value();
		n &= m;
		v |= (n << offset);
		return *this;
	}

	register_bits& operator &=(R n) {
		const auto m = mask();
		auto& v = value();
		n &= m;
		v &= (~m | (n << offset));
		return *this;
	}
};
