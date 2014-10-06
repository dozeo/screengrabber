#pragma once

#include <libgrabber/src/igrabber.h>
#include <boost/program_options.hpp>
#include <ostream>
#include <vector>
#include <stdio.h>

#include <libcommon/grabber_options.h>

/// Boost program_options parser for Grabber options
class GrabberOptionsCmdLine
{
	public:
		static std::vector<std::string> packCommandLine(GrabberOptions& options)
		{
			std::vector<std::string> result;

			if (!options.m_grabRect.empty())
			{
				result.push_back("--grect");
				result.push_back(
					boost::lexical_cast<std::string>(options.m_grabRect.x) + "," +
					boost::lexical_cast<std::string>(options.m_grabRect.y) + "," +
					boost::lexical_cast<std::string>(options.m_grabRect.width) + "," +
					boost::lexical_cast<std::string>(options.m_grabRect.height));
			}

			if (options.m_grabScreen >= 0)
			{
				result.push_back("--gscreen");
				result.push_back(boost::lexical_cast<std::string>(options.m_grabScreen));
			}

/*			if (target.m_grabPid > 0)
			{
				result.push_back("--gpid");
				result.push_back(boost::lexical_cast<std::string>(target.m_grabPid));
			}*/

			if (options.m_grabWindowId > 0)
			{
				result.push_back("--gwid");
				result.push_back(boost::lexical_cast<std::string>(options.m_grabWindowId));
			}

			if (options.m_grabFollow)
				result.push_back("--gfollow");

			if (options.m_grabCursor)
				result.push_back("--gcursor");

			if (options.m_grabberType != dz::GrabberType::Default)
			{
				result.push_back("--gtype");
				result.push_back(dz::GrabberType::ToString(options.m_grabberType));
			}

			return result;
		}
};
