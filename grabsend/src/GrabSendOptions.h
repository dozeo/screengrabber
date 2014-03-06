#pragma once

#include "GrabberOptions.h"
#include "VideoSenderOptions.h"
#include <boost/program_options.hpp>

/// Command line options of grabsend
class GrabSendOptions : public GrabberOptions
{
	public:
		GrabSendOptions(int argc, char* argv[]);

		/// Print command line help
		void doPrintHelp() const;

		bool printHelp;
		bool printScreens;
		bool printWindows;
		bool printProcesses;
		bool m_bWaitOnException;
		int statLevel;

		//GrabberOptions grabberOptions;
		VideoSenderOptions videoSenderOptions;

		friend std::ostream& operator<< (std::ostream& s, const GrabSendOptions& o);

	private:
		/// Apply variables map to this and sub options
		void ApplyGrabSendOptions(const boost::program_options::variables_map & vm);
		void ApplyGrabberOptions(const boost::program_options::variables_map& vm);

		boost::program_options::options_description desc;
		//GrabberOptionsParser grabberOptionsParser;
		VideoSenderOptionsParser videoSenderOptionsParser;
};
