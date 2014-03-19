#include "libcommon/videotypes.h"

#include <dzlib/dzexception.h>

#include <algorithm>

namespace dz
{
	namespace VideoQualityLevel
	{
		std::string ToString(Enum type)
		{
			switch (type)
			{
				case Low: return "low";
				case Medium: return "medium";
				case High: return "high";
				default:
					throw exception(strstream() << "Unknown VideoQualityLevel enum found " << type << ". Forgot to add the new one on the list (videosender.cpp)");
			}
		}

		Enum FromString(std::string text)
		{
			std::transform(text.begin(), text.end(), text.begin(), ::tolower);

			if (text.compare("low") == 0)
				return Low;

			if (text.compare("medium") == 0)
				return Medium;

			if (text.compare("high") == 0)
				return High;

			throw exception(strstream() << "Unknown VideoQualityLevel value '" << text << "' encountered. Valid values are 'low', 'medium' and 'high'");
		}
	}

	namespace VideoSenderType
	{
		std::string ToString(Enum type)
		{
			switch (type)
			{
				case Null: return "null";
				case Default: return "default";
			}

			throw exception(strstream() << "Invalid VideoSenderType given to ToString");
		}

		Enum FromString(std::string name)
		{
			if (name.compare("null") == 0)
				return Null;
			
			if (name.compare("default") == 0)
				return Default;

			throw exception(strstream() << "Invalid string given to VideoSenderType::FromString(" << name << ")");
		}
	};

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
	};
};

std::ostream& operator<< (std::ostream& out, dz::VideoSenderType::Enum type)
{
	out << dz::VideoSenderType::ToString(type);
	return out;
}

std::ostream& operator<< (std::ostream& out, dz::VideoQualityLevel::Enum type)
{
	out << dz::VideoQualityLevel::ToString(type);
	return out;
}

std::ostream& operator<< (std::ostream& out, dz::GrabberType::Enum type)
{
	out << dz::GrabberType::ToString(type);
	return out;
}