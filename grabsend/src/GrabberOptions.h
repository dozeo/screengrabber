#pragma once

#include <libgrabber/src/Grabber.h>
#include <boost/program_options.hpp>
#include <ostream>
#include <vector>
#include <stdio.h>

/// Options for the screen grabber
struct GrabberOptions
{
	GrabberOptions() : grabScreen (-1), grabPid (-1), grabWid (-1), grabFollow (true), grabCursor (false), grabberType(dz::GrabberType::Default) {}

	friend std::ostream & operator<< (std::ostream & s, const GrabberOptions& o)
	{
		s << "grect: " << o.grabRect << " gscreen: " << o.grabScreen << " gpid: " << o.grabPid << " gwid: " << o.grabWid << " gfollow: " << o.grabFollow << " gcursor: " << o.grabCursor << " type: " << o.grabberType;
		return s;
	}

	/// Generate command line arguments compatible with parser
	inline std::vector<std::string> packCommandLine() const
	{
		std::vector<std::string> result;
		if (!grabRect.empty()){
			result.push_back ("--grect");
			result.push_back (
				boost::lexical_cast<std::string> (grabRect.x) + "," +
				boost::lexical_cast<std::string> (grabRect.y) + "," +
				boost::lexical_cast<std::string> (grabRect.w) + "," +
				boost::lexical_cast<std::string> (grabRect.h));
		}
		if (grabScreen >= 0){
			result.push_back ("--gscreen");
			result.push_back (boost::lexical_cast<std::string> (grabScreen));
		}
		if (grabPid > 0) {
			result.push_back ("--gpid");
			result.push_back (boost::lexical_cast<std::string> (grabPid));
		}
		if (grabWid > 0) {
			result.push_back ("--gwid");
			result.push_back (boost::lexical_cast<std::string> (grabWid));
		}
		if (grabFollow) {
			result.push_back ("--gfollow");
		}
		if (grabCursor) {
			result.push_back ("--gcursor");
		}

		if (grabberType != dz::GrabberType::Default)
		{
			result.push_back ("--gtype");
			result.push_back ( dz::GrabberType::ToString(grabberType) );
		}

		return result;
	}

	bool IsGrabWindow() const { return grabWid > 0; }

	dz::Rect grabRect;  ///< != empty if set
	int grabScreen;     ///< >= 0 if set
	int64_t grabPid;    ///<   >0 if set
	int64_t grabWid;    ///<   >0 if set
	bool grabFollow;    ///<   if true, follow the grabbed region (valid on grabScreen, grabPid or grabWid)
	bool grabCursor;    ///<   Also grab mouse cursor (if possible)
	dz::GrabberType::Enum grabberType; ///< Grabber type to use
};

/// Boost program_options parser for Grabber options
struct GrabberOptionsParser
{
	GrabberOptionsParser(GrabberOptions * _target) : target (_target), desc ("Grabber Options")
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
			int ret = sscanf (arg.c_str(), "%d,%d,%d,%d", &target->grabRect.x, &target->grabRect.y, &target->grabRect.w, &target->grabRect.h);
			if (ret != 4)
				throw boost::program_options::invalid_option_value ("Invalid grect");
		}

		if (po.count ("gscreen") > 0)
			target->grabScreen = po["gscreen"].as<int> ();
		
		if (po.count ("gpid") > 0)
			target->grabPid = po["gpid"].as<int64_t> ();

		if (po.count ("gwid") > 0)
			target->grabWid = po["gwid"].as<int64_t> ();
		
		if (po.count ("gfollow") > 0)
			target->grabFollow = true;
		
		if (po.count ("gcursor") > 0)
			target->grabCursor = true;
		
		if (po.count ("gtype") > 0)
		{
			std::string type = po["gtype"].as<std::string>();
			target->grabberType = dz::GrabberType::FromString(type);
		}
	}

	GrabberOptions * target;
	boost::program_options::options_description desc;
};
