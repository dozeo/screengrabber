#include "GrabSendOptions.h"
namespace po = boost::program_options;
#include <iostream>
#include <gitdescribe.h>

#include <dzlib/dzexception.h>

using namespace dz;

GrabSendOptions::GrabSendOptions(int argc, char* argv[]) : desc ("General Configuration"), /*grabberOptionsParser(&grabberOptions), */videoSenderOptionsParser (&videoSenderOptions),
	m_bWaitOnException(false)
{
	printHelp = false;
	printScreens = false;
	printWindows = false;
	printProcesses = false;
	statLevel = 1;

	// grabber options
	desc.add_options()
		("grect", boost::program_options::value<std::string>(), "Select grabbing rect x,y,w,h")
		("gscreen", boost::program_options::value<int>(), "Select grabbing screens id")
		("gpid", boost::program_options::value<int64_t>(), "Select grabbing around a specific pid")
		("gwid", boost::program_options::value<int64_t>(), "Select grabbing around a specific wid")
		("gfollow", "Follows grabbed region")
		("gcursor", "Grab mouse cursor, if possible")
		("gtype", boost::program_options::value<std::string>(), "Select grabber type (Null|DirectX|Default)");

	desc.add_options()
		("help", "Show help message")
		("screens", "Print screens and exit")
		("windows", "Print windows and exit")
		("processes", "Print processes and exit")
		("waitonerror", "Waits for keyboard input when an exception occures - use for debugging")
		("stat", boost::program_options::value<int>(&statLevel)->default_value(1), "Statistics level (0 .. 2)");

	//desc.add(grabberOptionsParser.desc);
	desc.add(videoSenderOptionsParser.desc);

	try
	{
		po::variables_map vm;
		po::store (po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);

		ApplyGrabSendOptions(vm);
	} 
	catch (po::error & e)
	{
		throw exception(strstream() << "GrabSendOptions parse exception: " << e.what());
	}
}

void GrabSendOptions::doPrintHelp() const
{
	std::cout << "grabsend: Grab desktop content and stream it with ffmpeg" << std::endl;
	std::cout << "Version:  " << GIT_DESCRIBE << std::endl;
	std::cout << desc << std::endl;
	std::cout << "grabsend can be stopped by CTRL+C or with entering \"QUIT\\n\"." << std::endl;
}

void GrabSendOptions::ApplyGrabSendOptions(const boost::program_options::variables_map & vm)
{
	videoSenderOptionsParser.apply(vm);

	if (vm.count("help") > 0)
		printHelp = true;
	
	if (vm.count ("screens") > 0)
		printScreens = true;
	
	if (vm.count ("windows") > 0)
		printWindows = true;
	
	if (vm.count ("processes") > 0)
		printProcesses = true;

	m_bWaitOnException = vm.count("waitonerror") > 0;

	ApplyGrabberOptions(vm);
}

void GrabSendOptions::ApplyGrabberOptions(const boost::program_options::variables_map& vm)
{
	if (vm.count("grect") > 0)
	{
		std::string arg = vm["grect"].as<std::string>();
		int ret = sscanf (arg.c_str(), "%d,%d,%d,%d", &m_grabRect.x, &m_grabRect.y, &m_grabRect.width, &m_grabRect.height);
		if (ret != 4)
			throw boost::program_options::invalid_option_value ("Invalid grect");
	}

	if (vm.count ("gscreen") > 0)
		m_grabScreen = vm["gscreen"].as<int>();
	//	
	//if (vm.count ("gpid") > 0)
	//	m_grabPid = vm["gpid"].as<int64_t>();

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

//friend 
std::ostream& operator<< (std::ostream& s, const GrabSendOptions& o)
{
	s << ((GrabberOptions)o).ToString();

	return s;
}
