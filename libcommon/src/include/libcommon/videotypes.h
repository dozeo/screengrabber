#pragma once

#include <string>

namespace dz
{
	namespace VideoQualityLevel
	{
		enum Enum
		{
			Low,
			Medium,
			High
		};

		std::string ToString(Enum type);
		Enum FromString(std::string text);
	};

	namespace VideoSenderType
	{
		enum Enum
		{
			Null,
			Default
		};

		std::string ToString(Enum type);
		Enum FromString(std::string name);
	};

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
	};
}

std::ostream& operator<< (std::ostream& out, dz::VideoSenderType::Enum type);
std::ostream& operator<< (std::ostream& out, dz::VideoQualityLevel::Enum type);
std::ostream& operator<< (std::ostream& out, dz::GrabberType::Enum type);
