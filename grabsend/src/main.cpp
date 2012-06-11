#include <string>
#include <stdio.h>
#include <iostream>
#include <signal.h>

#include "Tools.h"
#include "GrabbingPipeline.h"

#include <videosend/VideoSender.h>
#include "GrabSendOptions.h"
#include <gitdescribe.h>

#include <boost/program_options.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>

#ifdef QT_GUI_LIB
#include <QApplication>
#include <QWidget>
#endif

namespace po = boost::program_options;

/// Implements main grabbing loop
/// Starts / Stop the video and does signal handling
/// Note: grabbbingPipeline and sender must be completely initialized.
int grabbingLoop (GrabbingPipeline * grabbingPipeline, const VideoSenderOptions & videoSenderOptions, dz::VideoSender * sender) {
	int result = sender->open();
	if (result) {
		std::cerr << "Error: Could not open output stream " << result << std::endl;
		return 1;
	}
	double t0 = microtime();
	installSigHandler ();
	installLineReader ();
	while (!shutDownLoop()) {
		double t1 = microtime();
		double dt = t1 - t0;
		result = grabbingPipeline->grab();
		double t2 = microtime ();
		if (result) {
			std::cerr << "Error: could not grab " << result << std::endl;
			return 1;
		}
		const dz::Buffer * buffer = grabbingPipeline->buffer();
		result = sender->putFrame(buffer->data, buffer->width, buffer->height, buffer->rowLength, dt);
		if (result) {
			std::cerr << "Error: could not send " << result << std::endl;
			return 1;
		}
#if QT_GUI_LIB
		if (QApplication::instance()){
            QApplication::processEvents();
            QApplication::sendPostedEvents();
        }
#endif
		double t3 = microtime ();
		double timeToWait = (1.0f / videoSenderOptions.fps) - (t3 - t1);
		double timeToGrab          = t2 - t1;
		double timeToEncodeAndSend = t3 - t2;
		std::cerr.precision(3);
		std::cerr << "Debug: dt=" << (dt) << "s grab=" << (timeToGrab * 1000) << "ms encode=" << (timeToEncodeAndSend * 1000) << "ms wait=" << (timeToWait * 1000) << "ms" << std::endl;
		const dz::VideoSender::Statistic * stat = sender->statistic();
		if (stat) {
			printLastFrame(std::cerr << "Debug Statistics " , *stat) << std::endl;
		}
		if (timeToWait < 0) {
			std::cerr << "Warning: can not grab and send in time, will miss frames!" << std::endl;
		} else {
			millisleep ( (int) (timeToWait * 1000));
		}
	}
	sender->close();
	return 0;
}

int main (int argc, char * argv[]) {
	GrabSendOptions options;

	int result = options.parse (argc, argv);
	if (result) return result;

	if (options.printHelp) {
		options.doPrintHelp();
		return 1;
	}
	// Debug!
	std::cout << "Version: " << GIT_DESCRIBE << std::endl;
	std::cout << "GrabberOptions:     " << options.grabberOptions << std::endl;
	std::cout << "VideoSenderOptions: " << options.videoSenderOptions << std::endl;

	GrabbingPipeline grabbingPipeline;
	grabbingPipeline.setOptions(
			&options.grabberOptions,
			options.videoSenderOptions.correctAspect,
			options.videoSenderOptions.width,
			options.videoSenderOptions.height);
	result = grabbingPipeline.reinit();
	if (result) {
		std::cerr << "Error: could not initialize grabbing pipeline " << result << std::endl;
		return 1;
	}

	bool doQuitImmediately = false;
	if (options.printScreens) {
		printScreens (grabbingPipeline.grabber());
		doQuitImmediately = true;
	}
	if (options.printWindows) {
		printWindows (grabbingPipeline.grabber());
		doQuitImmediately = true;
	}
	if (options.printProcesses) {
		printProcesses (grabbingPipeline.grabber());
		doQuitImmediately = true;
	}
	if (doQuitImmediately) return 0;
#ifdef QT_GUI_LIB
    boost::scoped_ptr<QApplication> qApplication;
	if (options.videoSenderOptions.type == dz::VT_QT) {
        qApplication.reset (new QApplication (argc, argv));
    } else {
#ifdef MAC_OSX
        initalizeNSApplication();
#endif
    }
#endif

	boost::scoped_ptr<dz::VideoSender> sender (dz::VideoSender::create (options.videoSenderOptions.type));
	if (!sender) {
		std::cerr << "Error: could not initialize sender" << std::endl;
		return 1;
	}

	dz::Rect grabRect = grabbingPipeline.grabRect();
    std::cout << "Final grabRect: " << grabRect << std::endl;
    
	if (!options.videoSenderOptions.url.empty()) {
		result = sender->setTargetUrl(options.videoSenderOptions.url);
		if (result) {
			std::cerr << "Error: Could not set target URL " << options.videoSenderOptions.url << " " << result << std::endl;
			return 1;
		}
	} else {
		result = sender->setTargetFile(options.videoSenderOptions.file);
		if (result) {
			std::cerr << "Error: Could not set target file " << options.videoSenderOptions.file << " " << result << std::endl;
			return 1;
		}
	}

	if (options.videoSenderOptions.correctAspect) {
		options.videoSenderOptions.width = (int)  ((float) grabRect.w / (float) grabRect.h * (float) options.videoSenderOptions.height);
	}
	std::cout << "Final video size: " << options.videoSenderOptions.width << "x" << options.videoSenderOptions.height << std::endl;
	
	// Set Options again, as video width/height could be changed
	grabbingPipeline.setOptions (
		&options.grabberOptions, 
		options.videoSenderOptions.correctAspect, 
		options.videoSenderOptions.width, 
		options.videoSenderOptions.height);

	result = sender->setVideoSettings(
			options.videoSenderOptions.width,
			options.videoSenderOptions.height,
			options.videoSenderOptions.fps,
			options.videoSenderOptions.kiloBitrate * 1000,
			options.videoSenderOptions.quality);
	if (result) {
		std::cerr << "Error: Could not set video sender settings (w,h,fps,bitrate) " << result << std::endl;
		return 1;
	}

	result = grabbingLoop (&grabbingPipeline, options.videoSenderOptions, sender.get());
	if (result) {
		std::cerr << "Error: Grabbing loop ended with error " << result << std::endl;
		return result;
	}
	return 0;
}
