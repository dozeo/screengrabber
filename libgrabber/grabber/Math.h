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

/// Round x up to next multiple of 'of'.
static inline int toNextMultiple (int x, int of) { int rm = x % of; return rm == 0 ? x : x + (of - rm); }

}
