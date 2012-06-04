#include "Rect.h"
#include <iostream>

namespace dz {

std::ostream& operator << (std::ostream & s, const dz::Rect & r) {
	return s << r.x << "," << r.y << " " << r.w << "x" << r.h;
}

}
