#include "libcommon/dzrect.h"

namespace dz
{
	Rect::Rect() : x(0), y(0), width(0), height(0)
	{
	}
	
	Rect::Rect(uint32_t w, uint32_t h) : x(0), y(0), width(w), height(h)
	{
	}
	
	Rect::Rect(int x, int y, uint32_t w, uint32_t h) : x(x), y(y), width(w), height(h)
	{
	}

	//static 
	Rect Rect::cornered(int x0, int y0, int x1, int y1)
	{
		int right = maximum<int>(x0, x1);
		int left = minimum<int>(x0, x1);
		int top = minimum<int>(y0, y1);
		int bottom = maximum<int>(y0, y1);

		uint32_t width = right - left;
		uint32_t height = bottom - top;

		return Rect(left, top, width, height);
	}

	void Rect::addToBoundingRect(const Rect& r)
	{
		if (r.empty())
			return;
		
		if (empty())
		{
			*this = r;
			return;
		}

		(*this) = Rect::cornered(minimum<int>(x, r.x), minimum<int>(y, r.y), maximum<int>(right(), r.right()), maximum<int>(bottom(), r.bottom()));
	}

	bool Rect::intersects(const Rect& other, Rect* intersect) const
	{
		if (empty() || other.empty())
			return false;

		int x0 = maximum<int>(x, other.x);
		int y0 = maximum<int>(y, other.y);

		int x1 = minimum<int>(right(), other.right());
		int y1 = minimum<int>(bottom(), other.bottom());

		if (x0 >= x1 ||
			y0 >= y1)
			return false;

		uint32_t width = x1-x0;
		uint32_t height = y1-y0;

		if (intersect != nullptr)
			*intersect = Rect(x0, y0, width, height);
		
		return true;
	}
};