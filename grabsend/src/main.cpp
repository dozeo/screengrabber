#include <string>
#include <stdio.h>
#include <iostream>
#include <signal.h>

#include "Tools.h"
#include "GrabbingPipeline.h"

#include <libvideosend/src/VideoSender.h>
#include "GrabSendOptions.h"
#include <gitdescribe.h>

#include <boost/program_options.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>

#ifdef QT_GUI_LIB
#include <QApplication>
#include <QWidget>
#endif

#include <dzlib/dzexception.h>

namespace po = boost::program_options;
using namespace dz;

/// Implements main grabbing loop
/// Starts / Stop the video and does signal handling
/// Note: grabbbingPipeline and sender must be completely initialized.
int grabbingLoop(GrabbingPipeline& grabbingPipeline, const GrabSendOptions& options, dz::VideoSender* sender)
{
	installSigHandler();
	installLineReader();

	sender->OpenVideoStream();

	double startTime = microtime();
	double timeToGrabSum = 0;
	int frame = 0;

	while (!shutDownLoop())
	{
		double frameStartTime = microtime();
		double dt = frameStartTime - startTime;
		
		{
			Timing grabTime("grab");
			grabbingPipeline.grab();
		}

		double t2 = microtime();

		const dz::Buffer* buffer = grabbingPipeline.buffer();
		{
			Timing grabTime("putFrame");
			sender->putFrame(buffer->data, buffer->width, buffer->height, buffer->rowLength, dt);
		}

#if QT_GUI_LIB
		if (QApplication::instance())
		{
			QApplication::processEvents();
			QApplication::sendPostedEvents();
		}
#endif

		frame++;
		double t3 = microtime ();
		double timeToWait = (1.0f / options.videoSenderOptions.fps) - (t3 - frameStartTime);
		double timeToGrab          = t2 - frameStartTime;
		double timeToEncodeAndSend = t3 - t2;
		timeToGrabSum += timeToGrab;
		double timeToGrabAvg = (timeToGrabSum / frame);

		const dz::VideoSender::Statistic * stat = sender->statistic();

		std::cerr.precision(3);
		if (options.statLevel >= 1) {
			// on each 10th frame print average information
			if ((frame % 1000) == 0 && frame > 0) {
				if (stat) {
					printAvg(std::cerr << "Info: ", *stat) << " grab: " << (timeToGrabAvg * 1000) << "ms " << std::endl;
				} else {
					std::cerr << "Info: avg grab: " << (timeToGrabAvg * 1000) << "ms, no more information available" << std::endl;
				}
			}
		}
		if (options.statLevel >= 2) {
			// on each frame print time about grabing and sending (scale, encode + send)
			std::cerr << "Info: " << dt << "s grab: " << (timeToGrab * 1000) << "ms encodeAndSend: " << (timeToEncodeAndSend * 1000) << " wait: " << (timeToWait * 1000) << "ms" << std::endl;
		}
		if (options.statLevel >= 3 && stat) {
			// on each frame print time for scaling, encoding, sending
			printLastFrame(std::cerr << "Info: ", *stat) << std::endl << std::endl;
		}
		if (timeToWait < 0) {
			// oh, do not have time, what takes so long?
			if (stat) { // can make assumption only if statistic
				analyseFrameDropCause (std::cerr << "Warning: ", frame, timeToGrabSum, *stat) << std::endl;
			} else {
				// no statistic, cannot analyze
				std::cerr << "Warning: can not grab and send in time, will miss frames!" << std::endl;
			}
		} else {
			millisleep ( (int) (timeToWait * 1000));
		}
	}
	sender->close();
	return 0;
}

void doGrabSend(GrabSendOptions& options)
{
	//options.parse(argc, argv);

	if (options.printHelp)
	{
		options.doPrintHelp();
		return;
	}

	// Debug!
	std::cout << "Version: " << GIT_DESCRIBE << std::endl;
	std::cout << "GrabberOptions:     " << options.grabberOptions << std::endl;
	std::cout << "VideoSenderOptions: " << options.videoSenderOptions << std::endl;

	//int result = grabbingPipeline.reinit();
	//if (result)
	//{
	//	std::cerr << "Error: could not initialize grabbing pipeline " << result << std::endl;
	//	return 1;
	//}

	if (options.printScreens || options.printWindows || options.printProcesses)
	{
		if (options.printScreens) printScreens();
		if (options.printWindows) printWindows();
		if (options.printProcesses) printProcesses();
		
		return;
	}
	
#ifdef QT_GUI_LIB
	boost::scoped_ptr<QApplication> qApplication;
	if (options.videoSenderOptions.type == dz::VT_QT)
	{
		int argc = 0;
		char** argv = { NULL };
		qApplication.reset (new QApplication (argc, argv));
	}
#ifdef MAC_OSX
	else
	{
		initalizeNSApplication();
	}
#endif
#endif

	GrabbingPipeline grabbingPipeline(&options.grabberOptions, options.videoSenderOptions.correctAspect, options.videoSenderOptions.width, options.videoSenderOptions.height);
	dz::Rect grabRect = grabbingPipeline.grabRect();
	std::cout << "Final grabRect: " << grabRect << std::endl;

	boost::scoped_ptr<dz::VideoSender> sender(dz::VideoSender::create(options.videoSenderOptions.type));

	if (!options.videoSenderOptions.url.empty())
	{
		sender->setTargetUrl(options.videoSenderOptions.url);
	}
	else
	{
		sender->setTargetFile(options.videoSenderOptions.file);
	}

	if (options.videoSenderOptions.cutSize)
	{
		double aspect  = (double) grabRect.w / (double) grabRect.h;
		double vaspect = (double) options.videoSenderOptions.width / (double) options.videoSenderOptions.height;
		if (aspect > vaspect) {
			// Letter Boxing up and down
			int bh = (int) options.videoSenderOptions.width / aspect;
			options.videoSenderOptions.height = dz::minimum (dz::toNextMultiple(bh, 16), options.videoSenderOptions.height);
		} else {
			// Letter Boxing left and right
			int bw  = (int) options.videoSenderOptions.height * aspect;
			options.videoSenderOptions.width = dz::minimum (dz::toNextMultiple (bw, 16), options.videoSenderOptions.width);
		}
		std::cout << "Final video size after cutting: " << options.videoSenderOptions.width << "x" << options.videoSenderOptions.height << std::endl;
	}

	if (options.videoSenderOptions.width % 16 != 0 || options.videoSenderOptions.height % 16 != 0)
	{
		std::cerr << "Warning: video size is not a multiple of 16" << std::endl;
	}

	// Set Options again, as video width/height could be changed
	grabbingPipeline.setOptions (
		&options.grabberOptions, 
		options.videoSenderOptions.correctAspect, 
		options.videoSenderOptions.width, 
		options.videoSenderOptions.height);

	sender->setVideoSettings(
		options.videoSenderOptions.width,
		options.videoSenderOptions.height,
		options.videoSenderOptions.fps,
		options.videoSenderOptions.kiloBitrate * 1000,
		options.videoSenderOptions.keyframe,
		options.videoSenderOptions.quality);

	grabbingLoop(grabbingPipeline, options, sender.get());
}

int main (int argc, char * argv[])
{
	bool bWaitOnError = false;

	try
	{
		GrabSendOptions options(argc, argv);
		bWaitOnError = options.m_bWantOnException;
		doGrabSend(options);
		return 0;
	}
	catch (dz::exception e)
	{
		std::cerr << "Exception on grabsend: " << e.msg() << std::endl;
		
		if (bWaitOnError)
		{
			char t;
			std::cin >> t;
		}
	}

	return -1;
}
