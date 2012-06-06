#include "GrabSendOptions.h"
namespace po = boost::program_options;
#include <iostream>
#include <gitdescribe.h>

GrabSendOptions::GrabSendOptions () :
	desc ("General Configuration"),
	grabberOptionsParser (&grabberOptions),
	videoSenderOptionsParser (&videoSenderOptions) {

	printHelp      = false;
	printScreens   = false;
	printWindows   = false;
	printProcesses = false;

	desc.add_options()
				("help", "Show help message")
				("screens", "Print screens and exit")
				("windows", "Print windows and exit")
				("processes", "Print processes and exit");
	desc.add (grabberOptionsParser.desc);
	desc.add (videoSenderOptionsParser.desc);
}

int GrabSendOptions::parse (int argc, char ** argv){
	try {
		po::variables_map vm;
		po::store (po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);
		apply (vm);
	} catch (po::error & e) {
		std::cerr << "Error: " << e.what () << std::endl;
		return 1;
	}
	return 0;
}

void GrabSendOptions::doPrintHelp () {
	std::cout << "grabsend: Grab desktop content and stream it with ffmpeg" << std::endl;
	std::cout << "Version:  " << GIT_DESCRIBE << std::endl;
	std::cout << desc << std::endl;
	std::cout << "grabsend can be stopped by CTRL+C or with entering \"QUIT\\n\"." << std::endl;
}

void GrabSendOptions::apply (const boost::program_options::variables_map & vm) {
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
	grabberOptionsParser.apply (vm);
	videoSenderOptionsParser.apply (vm);
}
