#include "Grabber.h"

namespace dz
{
	namespace GrabberType
	{
		std::string ToString(Enum type)
		{
			switch (type)
			{
				case Null: return "null";
				case Default: return "default";
				//case DirectX: return "directx";
				case GrabWindow: return "grabwindow";
				default: return "unknown";
			}
		}

		Enum FromString(const std::string &value)
		{
			if (value.compare("null") == 0) return Null;
			if (value.compare("default") == 0) return Default;
			//if (value.compare("directx") == 0) return DirectX;
			if (value.compare("grabwindow") == 0) return GrabWindow;
			if (value.compare("") == 0) return Default;

			throw exception(strstream() << "GrabberType::FromString() - Invalid GrabberType value '" << value << "'");
		}
	}
}