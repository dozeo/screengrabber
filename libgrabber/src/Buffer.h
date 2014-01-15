#pragma once

#include <tr1/cstdint>

namespace dz
{
	/// Simple destination buffer for 32bit color space
	struct Buffer
	{
		Buffer () {
			width     = 0;
			height    = 0;
			rowLength = 0;
			data      = 0;
			managed   = false;
		}

		Buffer (int _w, int _h, int _rowLength = 0) : managed (false) {
			init (_w, _h, _rowLength);
		}

		~Buffer () {
			if (managed) {
				delete [] data;
			}
		}

		/// (Re) initializes the buffer
		void init (int _w, int _h, int _rowLength = 0);

		// clears content of buffer to black color
		void clear ();

		/// Init the buffer so that it becomes a sub image from another buffer
		/// Note: they both will share the same memory region
		/// So you have to keep the source
		void initAsSubBufferFrom (Buffer * source, int _x, int _y, int _width, int _height);

		uint8_t * data;		///< Contains pixel data
		int width;			///< Width of Buffer
		int height;			///< Height of Buffer
		int rowLength;		///< Rowlength in bytes
		bool managed;		///< Buffer manages lifecycle by itself

	private:
		// Forbidden
		Buffer (const Buffer &);
		Buffer & operator= (const Buffer &);
	};
}
