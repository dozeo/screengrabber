#pragma once

#include "Math.h"
#include <iostream>

namespace dz
{

class Rect
{
public:
	Rect() : x(0), y(0), w(0), h(0) {}
	Rect(int _w, int _h) : x(0), y(0), w(_w), h(_h) {}
	Rect(int _x, int _y, int _w, int _h) : x(_x), y(_y), w(_w), h(_h) {}

	/// Create rect with topleft and bottom right corner points
	static Rect cornered (int x0, int y0, int x1, int y1) {
		return Rect (x0, y0, x1 - x0, y1 - y0).normalized();
	}

	inline const int left() const
	{
		return x;
	}

	inline const int top() const
	{
		return y;
	}

	inline const int right() const
	{
		return x + w;
	}

	inline const int bottom() const
	{
		return y + h;
	}
    
    /// Handles Rect as it would be a boundingbox and increases this box with another rect
    inline void addToBoundingRect (const Rect & r) {
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

		x = minimum (lx, rx);
		y = minimum (ly, ry);
		w = maximum (lx1, rx1) - x;
		h = maximum (ly1, ry1) - y;
    }
    
	inline bool intersects(const Rect& other, Rect* intersect = 0) const
	{
        Rect i;
        i.x = maximum (x, other.x);
        i.y = maximum (y, other.y);
        int x1,y1;
        x1 = minimum (right(), other.right());
        y1 = minimum (bottom(), other.bottom());
        i.w = x1 - i.x;
        i.h = y1 - i.y;
        bool check = i.w > 0 && i.h > 0;
        if (check && intersect) {
            *intersect = i;
        }
        return check;
	}

	inline bool empty() const {
		return w == 0 || h == 0;
	}

	inline bool contains (int _x, int _y) const {
		return (_x >= x && _y >= y && _x < right() && _y < bottom());
	}

	inline Rect normalized () const {
		int nx = w  < 0 ? x + w : x;
		int ny = h < 0 ? y + h : y;
		int nw = abs(w);
		int nh = abs(h);
		return Rect (nx, ny, nw, nh);
	}

public:
	int x;
	int y;
	int w;
	int h;
};
    
inline bool operator== (const Rect& a, const Rect & b) {
    return a.x == b.x && a.y == b.y && a.w == b.w && a.h == b.h;
}

inline std::ostream& operator << (std::ostream & s, const dz::Rect & r) {
	return s << r.x << "," << r.y << " " << r.w << "x" << r.h;
}

}
