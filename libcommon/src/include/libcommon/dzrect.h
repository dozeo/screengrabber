#pragma once

#include <libcommon/math_helpers.h>
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
			static Rect cornered (int x0, int y0, int x1, int y1) { return Rect (x0, y0, x1 - x0, y1 - y0).normalized(); }

			const int left() const { return x; }
			const int top() const { return y; }
			const int right() const { return x + w; }
			const int bottom() const { return y + h; }

			/// Handles Rect as it would be a boundingbox and increases this box with another rect
			void addToBoundingRect(const Rect & r);

			bool intersects(const Rect& other, Rect* intersect = 0) const;

			bool empty() const { return w == 0 || h == 0; }

			bool contains (int _x, int _y) const { return (_x >= x && _y >= y && _x < right() && _y < bottom()); }

			Rect normalized() const
			{
				int nx = w  < 0 ? x + w : x;
				int ny = h < 0 ? y + h : y;
				int nw = abs(w);
				int nh = abs(h);

				return Rect (nx, ny, nw, nh);
			}

			friend bool operator== (const Rect& a, const Rect & b)
			{
				return a.x == b.x && a.y == b.y && a.w == b.w && a.h == b.h;
			}

			friend bool operator!= (const Rect& a, const Rect & b)
			{
				return (!operator==(a, b));
			}

			friend std::ostream& operator<< (std::ostream & s, const dz::Rect & r)
			{
				return s << r.x << "," << r.y << " " << r.w << "x" << r.h;
			}

		public:
			int x;
			int y;
			int w;
			int h;
	};
}
