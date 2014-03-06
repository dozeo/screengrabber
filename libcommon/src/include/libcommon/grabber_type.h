#pragma once

#include <string>

namespace dz
{
	namespace GrabberType
	{
		enum Enum
		{
			Null,
			Default,
			GrabWindow
		};

		std::string ToString(Enum type);
		Enum FromString(const std::string &value);
	}
}
