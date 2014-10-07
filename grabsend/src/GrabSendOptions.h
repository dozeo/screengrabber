#pragma once

#include "GrabberOptions.h"
#include "VideoSenderOptions.h"
#include <boost/program_options.hpp>

/// Command line options of grabsend
class GrabSendOptions : public GrabberOptions, public VideoSenderOptions
{
	public:
		GrabSendOptions(int argc, char* argv[]);

		/// Print command line help
		void doPrintHelp() const;

		bool printHelp;
		bool printScreens;
		bool printWindows;
		bool printProcesses;
		bool m_bPrintVersion;
		int statLevel;

		friend std::ostream& operator<< (std::ostream& s, const GrabSendOptions& o);

	private:
		/// Apply variables map to this and sub options
		void ApplyGrabSendOptions(const boost::program_options::variables_map & vm);
		void ApplyGrabberOptions(const boost::program_options::variables_map& vm);
		void ApplySenderOptions(const boost::program_options::variables_map& vm);

		boost::program_options::options_description desc;
};
