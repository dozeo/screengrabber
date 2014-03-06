#pragma once

#include <libcommon/math_helpers.h>
#include <iostream>

namespace dz
{
	class Rect
	{
		public:
			Rect();
			Rect(uint32_t w,uint32_t h);
			Rect(int x, int y, uint32_t w, uint32_t h);

			/// Create rect with topleft and bottom right corner points
			static Rect cornered(int x0, int y0, int x1, int y1);

			const int left() const { return x; }
			const int top() const { return y; }
			const int right() const { return x + width; }
			const int bottom() const { return y + height; }

			const uint32_t GetWidth() const { return width; }
			const uint32_t GetHeight() const { return height; }

			/// Handles Rect as it would be a boundingbox and increases this box with another rect
			void addToBoundingRect(const Rect& r);

			bool intersects(const Rect& other, Rect* intersect = 0) const;

			bool empty() const { return width == 0 || height == 0; }

			bool contains(int _x, int _y) const { return (_x >= x && _y >= y && _x < right() && _y < bottom()); }

			friend bool operator== (const Rect& a, const Rect & b)
			{
				return a.x == b.x && a.y == b.y && a.width == b.width && a.height == b.height;
			}

			friend bool operator!= (const Rect& a, const Rect & b)
			{
				return (!operator==(a, b));
			}

			friend std::ostream& operator<< (std::ostream & s, const dz::Rect & r)
			{
				return s << r.x << "," << r.y << " " << r.width << "x" << r.height;
			}

			int x;
			int y;
			uint32_t width;
			uint32_t height;
	};
}
