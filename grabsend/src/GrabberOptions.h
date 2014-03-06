#pragma once

#include <libgrabber/src/igrabber.h>
#include <boost/program_options.hpp>
#include <ostream>
#include <vector>
#include <stdio.h>

#include <dzlib/strstream.h>
#include <libcommon/grabber_options.h>

/// Boost program_options parser for Grabber options
class GrabberOptionsCmdLine
{
	public:
		GrabberOptionsCmdLine(GrabberOptions& options) : m_options(options), desc("Grabber Options")
		{
			desc.add_options()
				("grect", boost::program_options::value<std::string>(), "Select grabbing rect x,y,w,h")
				("gscreen", boost::program_options::value<int>(), "Select grabbing screens id")
				("gpid", boost::program_options::value<int64_t>(), "Select grabbing around a specific pid")
				("gwid", boost::program_options::value<int64_t>(), "Select grabbing around a specific wid")
				("gfollow", "Follows grabbed region")
				("gcursor", "Grab mouse cursor, if possible")
				("gtype", boost::program_options::value<std::string>(), "Select grabber type (Null|DirectX|Default)");
		}

		void apply(const boost::program_options::variables_map & po)
		{
			if (po.count("grect") > 0)
			{
				std::string arg = po["grect"].as<std::string>();
				int ret = sscanf (arg.c_str(), "%d,%d,%d,%d", &m_options.m_grabRect.x, &m_options.m_grabRect.y, &m_options.m_grabRect.width, &m_options.m_grabRect.height);
				if (ret != 4)
					throw boost::program_options::invalid_option_value ("Invalid grect");
			}

			if (po.count ("gscreen") > 0)
				m_options.m_grabScreen = po["gscreen"].as<int> ();
		
			//if (po.count ("gpid") > 0)
			//	target.m_grabPid = po["gpid"].as<int64_t> ();

			if (po.count ("gwid") > 0)
				m_options.m_grabWindowId = po["gwid"].as<int64_t> ();
		
			if (po.count ("gfollow") > 0)
				m_options.m_grabFollow = true;
		
			if (po.count ("gcursor") > 0)
				m_options.m_grabCursor = true;
		
			if (po.count ("gtype") > 0)
			{
				std::string type = po["gtype"].as<std::string>();
				m_options.m_grabberType = dz::GrabberType::FromString(type);
			}
		}

		std::vector<std::string> packCommandLine() const
		{
			std::vector<std::string> result;

			if (!m_options.m_grabRect.empty())
			{
				result.push_back("--grect");
				result.push_back(
					boost::lexical_cast<std::string>(m_options.m_grabRect.x) + "," +
					boost::lexical_cast<std::string>(m_options.m_grabRect.y) + "," +
					boost::lexical_cast<std::string>(m_options.m_grabRect.width) + "," +
					boost::lexical_cast<std::string>(m_options.m_grabRect.height));
			}

			if (m_options.m_grabScreen >= 0)
			{
				result.push_back("--gscreen");
				result.push_back(boost::lexical_cast<std::string>(m_options.m_grabScreen));
			}

/*			if (target.m_grabPid > 0)
			{
				result.push_back("--gpid");
				result.push_back(boost::lexical_cast<std::string>(target.m_grabPid));
			}*/

			if (m_options.m_grabWindowId > 0)
			{
				result.push_back("--gwid");
				result.push_back(boost::lexical_cast<std::string>(m_options.m_grabWindowId));
			}

			if (m_options.m_grabFollow)
				result.push_back("--gfollow");

			if (m_options.m_grabCursor)
				result.push_back("--gcursor");

			if (m_options.m_grabberType != dz::GrabberType::Default)
			{
				result.push_back("--gtype");
				result.push_back(dz::GrabberType::ToString(m_options.m_grabberType));
			}

			return result;
		}

	private:
		GrabberOptions& m_options;
		boost::program_options::options_description desc;
};
