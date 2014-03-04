#include "libcommon/dzrect.h"

namespace dz
{
	void Rect::addToBoundingRect(const Rect & r)
	{
		if (r.empty()) return;
		if (empty()){
			*this = r;
			return;
		}

		int lx = x;
		int rx = r.x;
		int ly = y;
		int ry = r.y;

		int lx1 = right();
		int rx1 = r.right();

		int ly1 = bottom();
		int ry1 = r.bottom();

		x = minimum<int>(lx, rx);
		y = minimum<int>(ly, ry);
		w = maximum<int>(lx1, rx1) - x;
		h = maximum<int>(ly1, ry1) - y;
	}

	bool Rect::intersects(const Rect& other, Rect* intersect) const
	{
		Rect i;
		i.x = maximum<int>(x, other.x);
		i.y = maximum<int>(y, other.y);
		int x1,y1;
		x1 = minimum<int>(right(), other.right());
		y1 = minimum<int>(bottom(), other.bottom());
		i.w = x1 - i.x;
		i.h = y1 - i.y;
		bool check = i.w > 0 && i.h > 0;
		if (check && intersect) {
			*intersect = i;
		}
		return check;
	}
};