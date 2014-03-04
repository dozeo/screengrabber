#pragma once

#include <cstdint>

namespace dz
{
	template<typename T>
	T minimum(T a, T b)
	{
		return (a < b) ? a : b;
	}

	template<typename T>
	T maximum(T a, T b)
	{
		return (a > b) ? a : b;
	}

	template<typename T>
	T abs(T a)
	{
		return (a < 0) ? (-a) : a;
	}

	/// Round x up to next multiple of 'of'.
	static inline int toNextMultiple(int x, int of)
	{
		int rm = x % of;
		return rm == 0 ? x : x + (of - rm);
	}
}
