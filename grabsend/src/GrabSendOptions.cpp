#include "GrabSendOptions.h"
namespace po = boost::program_options;
#include <iostream>
#include <gitdescribe.h>

#include <dzlib/dzexception.h>

using namespace dz;

GrabSendOptions::GrabSendOptions (int argc, char ** argv) : desc ("General Configuration"), grabberOptionsParser (&grabberOptions), videoSenderOptionsParser (&videoSenderOptions)
{
	printHelp      = false;
	printScreens   = false;
	printWindows   = false;
	printProcesses = false;
	m_bWantOnException = false;
	statLevel      = 1;

	desc.add_options()
				("help", "Show help message")
				("screens", "Print screens and exit")
				("windows", "Print windows and exit")
				("processes", "Print processes and exit")
				("waitonerror", "Waits for keyboard input when an exception occures - use for debugging")
				("stat", boost::program_options::value<int>(&statLevel)->default_value(1), "Statistics level (0 .. 2)");
	desc.add (grabberOptionsParser.desc);
	desc.add (videoSenderOptionsParser.desc);

	try
	{
		po::variables_map vm;
		po::store (po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);
		apply (vm);
	} 
	catch (po::error & e)
	{
		throw exception(strstream() << "GrabSendOptions parse exception: " << e.what());
	}
}

void GrabSendOptions::parse(int argc, char ** argv)
{
}

void GrabSendOptions::doPrintHelp() const
{
	std::cout << "grabsend: Grab desktop content and stream it with ffmpeg" << std::endl;
	std::cout << "Version:  " << GIT_DESCRIBE << std::endl;
	std::cout << desc << std::endl;
	std::cout << "grabsend can be stopped by CTRL+C or with entering \"QUIT\\n\"." << std::endl;
}

void GrabSendOptions::apply (const boost::program_options::variables_map & vm)
{
	grabberOptionsParser.apply (vm);
	videoSenderOptionsParser.apply (vm);

	if (vm.count("help") > 0){
		printHelp = true;
	}
	if (vm.count ("screens") > 0) {
		printScreens = true;
	}
	if (vm.count ("windows") > 0) {
		printWindows = true;
	}
	if (vm.count ("processes") > 0) {
		printProcesses = true;
	}

	m_bWantOnException = vm.count("waitonerror") > 0;

	grabberOptionsParser.apply (vm);
	videoSenderOptionsParser.apply (vm);
}
