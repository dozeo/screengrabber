#pragma once
#include "GrabberOptions.h"
#include "VideoSenderOptions.h"
#include <boost/program_options.hpp>

/// Command line options of grabsend
struct GrabSendOptions {
	GrabSendOptions ();

	/// Parse command line arguments
	/// Returns 1 on error and already display error message
	int parse (int argc, char ** argv);

	/// Print command line help
	void doPrintHelp ();

	bool printHelp;
	bool printScreens;
	bool printWindows;
	bool printProcesses;
	int  statLevel;
	GrabberOptions grabberOptions;
	VideoSenderOptions videoSenderOptions;
private:
	/// Apply variables map to this and sub options
	void apply (const boost::program_options::variables_map & vm);
	boost::program_options::options_description desc;
	GrabberOptionsParser grabberOptionsParser;
	VideoSenderOptionsParser videoSenderOptionsParser;
};
