#include "Buffer.h"
#include <assert.h>
#include <string.h>

namespace dz {

void Buffer::init (int _w, int _h, int _rowLength) {
	if (managed) {
		delete [] data;
	}
	if (_rowLength == 0) _rowLength = _w * 4;
	width     = _w;
	height    = _h;
	rowLength = _rowLength;
	data = new uint8_t[height * rowLength] ();
	managed = true;
}

void Buffer::clear () {
	for (int y = 0; y < height; y++){
		uint8_t * pos = data + (y * rowLength);
		memset (pos, 0, rowLength);
	}
}

void Buffer::initAsSubBufferFrom (Buffer * source, int _x, int _y, int _width, int _height) {
    assert (_x + _width <= source->width);
    assert (_y + _height <= source->height);
    if (managed) {
        delete [] data;
    }
    managed   = false;
    width     = _width;
    height    = _height;
    rowLength = source->rowLength;
    data      = source->data + (_y * rowLength) + _x * 4;
}

}
