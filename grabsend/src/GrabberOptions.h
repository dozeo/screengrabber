#pragma once
#include <boost/program_options.hpp>
#include <grabber/Grabber.h>
#include <ostream>
#include <vector>
#include <stdio.h>

/// Options for the screen grabber
struct GrabberOptions {
	GrabberOptions ()
	: grabScreen (-1),
	  grabPid (-1),
	  grabWid (-1),
	  grabFollow (false),
      grabCursor (false),
	  grabberType (dz::GT_DEFAULT)
	   {}

	std::ostream& operator<< (std::ostream& s) const {
		return s << "grect: " << grabRect << " gscreen: " << grabScreen << " gpid: " << grabPid << " gwid: " << grabWid << " gfollow: " << grabFollow << " gcursor: " << grabCursor << " type: " << grabberType;
	}

	/// Generate command line arguments compatible with parser
	inline std::vector<std::string> packCommandLine () const;

	dz::Rect grabRect;  ///< != empty if set
	int grabScreen;     ///< >= 0 if set
	int64_t grabPid;    ///<   >0 if set
	int64_t grabWid;    ///<   >0 if set
	bool grabFollow;    ///<   if true, follow the grabbed region (valid on grabScreen, grabPid or grabWid)
    bool grabCursor;    ///<   Also grab mouse cursor (if possible)
	dz::GrabberType grabberType; ///< Grabber type to use
};

inline std::ostream & operator<< (std::ostream & s, const GrabberOptions& o) { return o.operator <<(s); }

inline std::vector<std::string> GrabberOptions::packCommandLine () const {
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
	if (grabberType != dz::GT_DEFAULT) {
		const char * nameType = "invalid";
		switch (grabberType) {
		case dz::GT_DIRECTX:
			nameType = "DirectX";
			break;
		case dz::GT_DEFAULT:
			nameType = "Default";
			break;
		case dz::GT_NULL:
			nameType = "Null";
			break;
		default:
			assert (!"should not come here, a new type?");
			break;
		}
		result.push_back ("--gtype");
		result.push_back (nameType);
	}
	return result;
}


/// Boost program_options parser for Grabber options
struct GrabberOptionsParser  {
	GrabberOptionsParser (GrabberOptions * _target) : target (_target), desc ("Grabber Options") {
		desc.add_options ()
				("grect", boost::program_options::value<std::string>(), "Select grabbing rect x,y,w,h")
				("gscreen", boost::program_options::value<int>(), "Select grabbing screens id")
				("gpid", boost::program_options::value<int64_t>(), "Select grabbing around a specific pid")
				("gwid", boost::program_options::value<int64_t>(), "Select grabbing around a specific wid")
				("gfollow", "Follows grabbed region")
				("gcursor", "Grab mouse cursor, if possible")
				("gtype", boost::program_options::value<std::string>(), "Select grabber type (Null|DirectX|Default)");
	}

	void apply (const boost::program_options::variables_map & po) {
		if (po.count("grect") > 0) {
			std::string arg = po["grect"].as<std::string>();
			int ret = sscanf (arg.c_str(), "%d,%d,%d,%d", &target->grabRect.x, &target->grabRect.y, &target->grabRect.w, &target->grabRect.h);
			if (ret != 4) {
				throw boost::program_options::invalid_option_value ("Invalid grect");
			}
		}
		if (po.count ("gscreen") > 0) {
			target->grabScreen = po["gscreen"].as<int> ();
		}
		if (po.count ("gpid") > 0) {
			target->grabPid = po["gpid"].as<int64_t> ();
		}
		if (po.count ("gwid") > 0) {
			target->grabWid = po["gwid"].as<int64_t> ();
		}
		if (po.count ("gfollow") > 0){
			target->grabFollow = true;
		}
        if (po.count ("gcursor") > 0) {
            target->grabCursor = true;
        }
		if (po.count ("gtype") > 0) {
			std::string type = po["gtype"].as<std::string>();
			if (type == "Null") target->grabberType = dz::GT_NULL;
			else if (type == "DirectX") target->grabberType = dz::GT_DIRECTX;
			else if (type == "Default") target->grabberType = dz::GT_DEFAULT;
			else throw boost::program_options::invalid_option_value(std::string ("Invalid grabber Type: " + type));
		}
	}

	GrabberOptions * target;
	boost::program_options::options_description desc;
};
