#pragma once

namespace dz {

static inline int minimum(int left, int right)
{
	return (left < right) ? left : right;
}

static inline int maximum(int left, int right)
{
	return (left > right) ? left : right;
}

static inline int abs(int val)
{
	return (val < 0) ? -val : val;
}

}