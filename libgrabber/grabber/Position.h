
#pragma once

namespace dz
{

class Position2
{
public:
	Position2() : x(0), y(0) {}
	Position2(int xval, int yval) : x(xval), y(yval) {}

	inline Position2& operator=(const Position2& rhs)
	{
		x = rhs.x;
		y = rhs.y;
		return *this;
	}

public:
	int x;
	int y;
};

}
