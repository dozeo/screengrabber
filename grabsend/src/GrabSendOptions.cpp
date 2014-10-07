#include "GrabSendOptions.h"
namespace po = boost::program_options;
#include <iostream>
#include <grabsend_version.h>

#include <dzlib/dzexception.h>
#include <slog/slog.h>

using namespace dz;

GrabSendOptions::GrabSendOptions(int argc, char* argv[]) : desc ("General Configuration"), /*grabberOptionsParser(&grabberOptions), videoSenderOptionsParser (&videoSenderOptions),*/
	m_bPrintVersion(false), printHelp(false), printScreens(false), printWindows(false), printProcesses(false), statLevel(1)
{
	// grabber options
	desc.add_options()
		("grect", boost::program_options::value<std::string>(), "Select grabbing rect x,y,w,h")
		("gscreen", boost::program_options::value<int>(), "Select grabbing screens id")
		("gpid", boost::program_options::value<int64_t>(), "Select grabbing around a specific pid")
		("gwid", boost::program_options::value<int64_t>(), "Select grabbing around a specific wid")
		("gfollow", "Follows grabbed region")
		("gcursor", "Grab mouse cursor, if possible")
		("gtype", boost::program_options::value<std::string>(), "Select grabber type (Null|DirectX|Default)");

	// video sender options
	desc.add_options()
		("vsize", boost::program_options::value<std::string>(), "Video Resolution w,h")
		("vtype", boost::program_options::value<std::string>(), "Sender Type (Null|Default|Qt)")
		("quality", boost::program_options::value<std::string>(), "Quality Level (Low|Medium|High)")
		("url", boost::program_options::value<std::string>(&url)->default_value("screencast.flv"),
			"URL where to send, e.g.\n"
			"  RTMP: rtmp://[host]/[app]/[playpath]"
			"        rtmp://[host]/[app] playpath=test"
			"        rtmp://[host] app=appname/1234 playpath=1234/1234"
			"  TCP: tcp://[host]:[port]/[app]/[playpath]"
			"  or just the filename to write to");

	// grabsend
	desc.add_options()
		("help", "Show help message")
		("version", "Show version and exit")
		("screens", "Print screens and exit")
		("windows", "Print windows and exit")
		("processes", "Print processes and exit")
		("waitonerror", "Waits for keyboard input when an exception occures - use for debugging")
		("stat", boost::program_options::value<int>(&statLevel)->default_value(1), "Statistics level (0 .. 2)");


	try
	{
		po::variables_map vm;
		po::store (po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);

		ApplyGrabSendOptions(vm);
	} 
	catch (po::error & e)
	{
		throw exception(strobj() << "GrabSendOptions parse exception: " << e.what());
	}
}

void GrabSendOptions::doPrintHelp() const
{
	std::cout << "grabsend: Grab desktop content and stream it with ffmpeg" << std::endl;
	std::cout << "Version:  " << GRABSEND_VERSION << std::endl;
	std::cout << desc << std::endl;
	std::cout << "grabsend can be stopped by CTRL+C or with entering \"QUIT\\n\"." << std::endl;
}

void GrabSendOptions::ApplyGrabSendOptions(const boost::program_options::variables_map & vm)
{
	if (vm.count("version") > 0)
		m_bPrintVersion = true;

	if (vm.count("help") > 0)
		printHelp = true;
	
	if (vm.count ("screens") > 0)
		printScreens = true;
	
	if (vm.count ("windows") > 0)
		printWindows = true;
	
	if (vm.count ("processes") > 0)
		printProcesses = true;

	ApplyGrabberOptions(vm);
	ApplySenderOptions(vm);
}

void GrabSendOptions::ApplyGrabberOptions(const boost::program_options::variables_map& vm)
{
	if (vm.count("grect") > 0)
	{
		std::string arg = vm["grect"].as<std::string>();
		int ret = sscanf (arg.c_str(), "%d,%d,%d,%d", &m_grabRect.x, &m_grabRect.y, &m_grabRect.width, &m_grabRect.height);
		if (ret != 4)
			throw boost::program_options::invalid_option_value ("Invalid grect");

		if (videowidth == 0)
			videowidth = m_grabRect.width;

		if (videoheight == 0)
			videoheight = m_grabRect.height;
	}

	if (vm.count ("gscreen") > 0)
		m_grabScreen = vm["gscreen"].as<int>();

	if (vm.count ("gwid") > 0)
		m_grabWindowId = vm["gwid"].as<int64_t>();

	if (vm.count ("gfollow") > 0)
		m_grabFollow = true;

	if (vm.count ("gcursor") > 0)
		m_grabCursor = true;

	if (vm.count ("gtype") > 0)
	{
		std::string type = vm["gtype"].as<std::string>();
		m_grabberType = dz::GrabberType::FromString(type);
	}
}

void GrabSendOptions::ApplySenderOptions(const boost::program_options::variables_map& vm)
{
	if (vm.count ("vsize") > 0)
	{
		std::string arg = vm["vsize"].as<std::string>();
		int count = sscanf(arg.c_str(), "%d,%d", &videowidth, &videoheight);
		
		if (count != 2 || videowidth < 1 || videoheight < 1)
			throw boost::program_options::invalid_option_value("Invalid vsize");
	}

	if (vm.count ("vtype") > 0)
	{
		std::string arg = vm["vtype"].as<std::string>();
		senderType = dz::VideoSenderType::FromString(arg);
	}

	if (vm.count ("quality") > 0)
	{
		std::string arg = vm["quality"].as<std::string>();
		quality = dz::VideoQualityLevel::FromString(arg);
	}
}

//friend 
std::ostream& operator<< (std::ostream& s, const GrabSendOptions& o)
{
	s << std::endl;
	s << "  GrabberOptions: " << ((GrabberOptions)o) << std::endl;
	s << "  VideoSenderOptions: " << ((VideoSenderOptions)o);

	return s;
}
