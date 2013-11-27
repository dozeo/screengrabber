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

namespace po = boost::program_options;

/// Implements main grabbing loop
/// Starts / Stop the video and does signal handling
/// Note: grabbbingPipeline and sender must be completely initialized.
int grabbingLoop (GrabbingPipeline * grabbingPipeline, const GrabSendOptions & options, dz::VideoSender * sender) {
	installSigHandler ();
	installLineReader ();

	int result = sender->open();
	if (result) {
		std::cerr << "Error: Could not open output stream " << result << std::endl;
		return 1;
	}
	double t0 = microtime();
	double timeToGrabSum = 0;
	int frame = 0;
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
		frame++;
		double t3 = microtime ();
		double timeToWait = (1.0f / options.videoSenderOptions.fps) - (t3 - t1);
		double timeToGrab          = t2 - t1;
		double timeToEncodeAndSend = t3 - t2;
		timeToGrabSum += timeToGrab;
		double timeToGrabAvg = (timeToGrabSum / frame);

		const dz::VideoSender::Statistic * stat = sender->statistic();

		std::cerr.precision(3);
		if (options.statLevel >= 1) {
			// on each 10th frame print average information
			if ((frame % 10) == 0 && frame > 0) {
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

int main (int argc, char * argv[]) {
	GrabSendOptions options;

	int result = options.parse(argc, argv);
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

	if (options.videoSenderOptions.cutSize){
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
	if (options.videoSenderOptions.width % 16 != 0 || options.videoSenderOptions.height % 16 != 0) {
		std::cerr << "Warning: video size is not a multiple of 16" << std::endl;
	}
	
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
			options.videoSenderOptions.keyframe,
			options.videoSenderOptions.quality);
	if (result) {
		std::cerr << "Error: Could not set video sender settings (w,h,fps,bitrate) " << result << std::endl;
		return 1;
	}

	result = grabbingLoop (&grabbingPipeline, options, sender.get());
	if (result) {
		std::cerr << "Error: Grabbing loop ended with error " << result << std::endl;
		return result;
	}
	return 0;
}
