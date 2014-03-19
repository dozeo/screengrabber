#pragma once

namespace dz
{
	class Dimension2
	{
		public:
			Dimension2() : width(0), height(0) { }
			Dimension2(int w, int h) : width(w) , height(h) { }

			Dimension2(const Dimension2& rhs)
			{
				width = rhs.width;
				height = rhs.height;
			}

			inline Dimension2& operator=(const Dimension2& rhs)
			{
				width = rhs.width;
				height = rhs.height;
				return *this;
			}

			bool operator== (const Dimension2& rhs) const
			{
				return width == rhs.width && height == rhs.height;
			}

			bool operator!= (const Dimension2& rhs) const
			{
				return width != rhs.width || height != rhs.height;
			}

			int width;
			int height;
	};
}
