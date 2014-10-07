#include <string>
#include <stdio.h>
#include <iostream>
#include <signal.h>

#include "Tools.h"

#include <libcommon/videoframepool.h>
#include <libvideosend/src/VideoSender.h>
#include "GrabSendOptions.h"
#include <grabsend_version.h>

#include <boost/program_options.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>

#include <dzlib/dzexception.h>
#include <slog/slog.h>

namespace po = boost::program_options;
using namespace dz;

using slog::error;

/// Implements main grabbing loop
/// Starts / Stop the video and does signal handling
/// Note: grabbbingPipeline and sender must be completely initialized.
void GrabbingLoop(GrabSendOptions& options)
{
	std::unique_ptr<IGrabber> grabber(IGrabber::CreateGrabber(options));

	installSigHandler();
	installLineReader();

	dz::Rect grabSize = grabber->GetCaptureRect();
	options.videowidth = grabSize.width;
	options.videoheight = grabSize.height;
	std::unique_ptr<dz::VideoSender> sender(dz::VideoSender::CreateVideoSender(options));

	double startTime = microtime();
	double timeToGrabSum = 0;
	int frame = 0;

	while (!shutDownLoop())
	{
		double frameStartTime = microtime();
		double dt = frameStartTime - startTime;
		
		//std::cout << "Start frame " << frame << std::endl;

		Timing grabTime("grab");
		auto videoFrame = grabber->GrabVideoFrame();
		//grabTime.Output();

		double t2 = microtime();

		{
			Timing putTime("putFrame");
			sender->SendFrame(std::move(videoFrame), dt);
			//putTime.Output();
		}

		frame++;
		double t3 = microtime();
		double timeToWait = (1.0f / sender->GetFPS()) - (t3 - frameStartTime);
		double timeToGrab = t2 - frameStartTime;
		double timeToEncodeAndSend = t3 - t2;
		timeToGrabSum += timeToGrab;
		double timeToGrabAvg = (timeToGrabSum / frame);

		const dz::VideoSender::Statistic* stat = sender->statistic();

		std::cerr.precision(3);
		if (options.statLevel >= 1) 
		{
			// on each 10th frame print average information
			if ((frame % 1000) == 0 && frame > 0) 
			{
				if (stat)
				{
					printAvg(std::cerr << "Info: ", *stat) << " grab: " << (timeToGrabAvg * 1000) << "ms " << std::endl;
				}
				else
				{
					std::cerr << "Info: avg grab: " << (timeToGrabAvg * 1000) << "ms, no more information available" << std::endl;
				}
			}
		}

		if (options.statLevel >= 2)
		{
			// on each frame print time about grabing and sending (scale, encode + send)
			std::cerr << "Info: " << dt << "s grab: " << (timeToGrab * 1000) << "ms encodeAndSend: " << (timeToEncodeAndSend * 1000) << " wait: " << (timeToWait * 1000) << "ms" << std::endl;
		}
		
		if (options.statLevel >= 3 && stat)
		{
			// on each frame print time for scaling, encoding, sending
			printLastFrame(std::cerr << "Info: ", *stat) << std::endl << std::endl;
		}
		
		if (timeToWait < 0)
		{
			// oh, do not have time, what takes so long?
			if (stat)
			{ // can make assumption only if statistic
				analyseFrameDropCause (std::cerr << "Warning: ", frame, timeToGrabSum, *stat) << std::endl;
			}
			else
			{
				// no statistic, cannot analyze
				std::cerr << "Warning: can not grab and send in time, will miss frames!" << std::endl;
			}
		}
		else
		{
			millisleep ( (int) (timeToWait * 1000));
		}
	}
}

// --grect 1739,210,161,120 --gcursor --quality Medium --vsize 161,120 --url "rtmp://streaming.dozeo.biz:80 app=screenshare/0dc3eed0-9005-0131-5a6d-027e5843a57f playpath=5d516580-9858-0130-5069-75a07fdc4ce0/BBB0D2C2-4AB3-85F1-6A6B-919291438593 igct=1 live=1 buffer=500"
// --grect 1739,210,161,120 --gcursor --quality medium --url "rtmp://streaming.dozeo.biz:80 app=screenshare/0729dc60-90ac-0131-353a-027e5843a57f playpath=5d516580-9858-0130-5069-75a07fdc4ce0/BBB0D2C2-4AB3-85F1-6A6B-919291438593 igct=1 live=1 buffer=500"--grect 1739,210,161,120 --gcursor --quality medium --url "rtmp://streaming.dozeo.biz:80 app=screenshare/0729dc60-90ac-0131-353a-027e5843a57f playpath=5d516580-9858-0130-5069-75a07fdc4ce0/BBB0D2C2-4AB3-85F1-6A6B-919291438593 igct=1 live=1 buffer=500"
// --grect 1739,210,161,120 --gcursor --quality medium --url "rtmp://streaming.dozeo.biz:80 app=screenshare/0729dc60-90ac-0131-353a-027e5843a57f playpath=5d516580-9858-0130-5069-75a07fdc4ce0/BBB0D2C2-4AB3-85F1-6A6B-919291438593 igct=1 live=1 buffer=500"
// --gwid 459958 --gcursor --quality medium --url "rtmp://streaming.dozeo.biz:80 app=screenshare/e81e5680-933f-0131-5a6d-027e5843a57f playpath=5d516580-9858-0130-5069-75a07fdc4ce0/BBB0D2C2-4AB3-85F1-6A6B-919291438593 igct=1 live=1 buffer=500"

void doGrabSend(GrabSendOptions& options)
{
	//options.parse(argc, argv);

	VideoFramePool singleton(5);

	std::cout << "Version: " << GRABSEND_VERSION << std::endl;
	if (options.m_bPrintVersion)
		return;

	if (options.printHelp)
	{
		options.doPrintHelp();
		return;
	}

	// Debug!
	std::cout << "Options: " << options << std::endl;

	if (options.printScreens || options.printWindows || options.printProcesses)
	{
		if (options.printScreens) printScreens();
		if (options.printWindows) printWindows();
		if (options.printProcesses) printProcesses();
		
		return;
	}
	
#ifdef MAC_OSX
	initalizeNSApplication();
#endif

	GrabbingLoop(options);
}

#ifndef _WIN32
#define OutputDebugString(_STR)
#else
#include <Windows.h>
#endif

int main (int argc, char * argv[])
{
	try
	{
		GrabSendOptions options(argc, argv);
		doGrabSend(options);
		return 0;
	}
	catch (dz::exception e)
	{
		std::string msg = strobj() << "Exception on grabsend: " << e.what();
		error() << msg;

#if _WIN32
		if (IsDebuggerPresent())
		{
			OutputDebugString(msg.c_str());
			DebugBreak();
			throw;
		}
#endif
	}

	return -1;
}
