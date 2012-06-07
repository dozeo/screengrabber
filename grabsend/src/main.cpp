#ifndef WIN32
#include <unistd.h>
#include <sys/time.h>
#else
#include <windows.h>
#endif

#include <string>
#include <stdio.h>
#include <iostream>
#include <signal.h>

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

std::string getVideoSenderError (int error) {
	switch (error) {
		case dz::VideoSender::VE_INVALID_RESOLUTION: return std::string("invalid resolution");
		case dz::VideoSender::VE_INVALID_TARGET:     return std::string("invalid target");
		case dz::VideoSender::VE_CODEC_NOT_FOUND:    return std::string("codec not found");
		case dz::VideoSender::VE_INVALID_CONVERSION: return std::string("invalid image conversion");
		case dz::VideoSender::VE_FAILED_OPEN_STREAM: return std::string("failed to open stream");
		case dz::VideoSender::VE_OK:                 return std::string("ok");
	}
	return std::string("unknown");
}

void millisleep (int timeMs) {
#ifdef WIN32
	::Sleep (timeMs);
#else
	usleep (timeMs * 1000);
#endif
}

/// Note: following snippet is from Schneeflocke (github.com/nob13/schneeflocke)
/// I hereby license this as public domain (Norbert Schultz)
/// Returns a time stamp with millisecond to microsecond exactness
/// Note: it is for measuring time intervals, NOT exact time of day.
double microtime (){
#ifdef WIN32
	FILETIME time;
	GetSystemTimeAsFileTime (&time);
	int64_t full = 0;
	full |= time.dwHighDateTime;
	full <<= 32;
	full |= time.dwLowDateTime;
	// is in 100nano-seconds intervals...
	static int64_t first = full;
	int64_t use = (full - first);
	return (use / 10000000.0);
#else
	struct timeval t;
	gettimeofday (&t, 0);
	return t.tv_sec + t.tv_usec / 1000000.0;
#endif
}

void printScreens (const dz::Grabber * grabber) {
	int count = grabber->screenCount();
	std::cout << "Screen Count: " << count << std::endl;
	for (int i = 0; i < count; i++) {
		dz::Rect r = grabber->screenResolution(i);
		std::cout << "Screen " << i << ": " << r << std::endl;
	}
}

void printWindows (const dz::Grabber * grabber) {
	typedef std::vector<dz::WindowInfo> WindowVec;
	WindowVec windows;
	int result = dz::WindowInfo::populate (&windows);
	if (result) {
		std::cerr << "Error: Could not populate windows " << result << std::endl;
		return;
	}
	std::cout << "Window count: " << windows.size() << std::endl;
	for (WindowVec::const_iterator i = windows.begin(); i != windows.end(); i++) {
		const dz::WindowInfo & win (*i);
		std::cout << "Window " << win.id << " pid: " << win.pid << " title: " << win.title << " area: " << win.area << std::endl;
	}
}

void printProcesses (const dz::Grabber * grabber) {
	typedef std::vector<dz::ProcessInfo> ProcessVec;
	ProcessVec processes;
	int result = dz::ProcessInfo::populate (&processes);
	if (result) {
		std::cerr << "Error: Could not populate processes " << result << std::endl;
		return;
	}
	std::cout << "Process count: " << processes.size() << std::endl;
	for (ProcessVec::const_iterator i = processes.begin(); i != processes.end(); i++) {
		const dz::ProcessInfo & pinfo (*i);
		std::cout << "Process " << pinfo.pid << " exec: " << pinfo.exec << std::endl;
	}
}

/// Stop the grabbingLoop
volatile bool gShutdown = false;
void signalHandler (int id) {
	printf ("Catched signal %d, will shut down\n", id);
	assert (id == SIGINT || id == SIGTERM);
	gShutdown = true;
}

/// Installs signal handler for SIGINT
void installSigHandler () {
	::signal(SIGINT, &signalHandler);
	::signal(SIGTERM, &signalHandler);
}

/// Waits for readed "QUIT\n" line to quit application
/// This is needed because there are not many reliable 
/// Ways to send messages to child console applications in Win32.
void lineReader () {
	while (std::cin && !gShutdown) {
		std::string line;
		std::getline(std::cin, line);
		if (line == "QUIT"){
			printf ("Read QUIT, shutting down\n");
			break;
		}
	}
	gShutdown = true;
}

void installLineReader () {
	boost::thread t (&lineReader);
}


/// Implements main grabbing loop
/// Starts / Stop the video and does signal handling
/// Note: grabber and sender must be completely initialized.
int grabbingLoop (GrabbingPipeline * grabbingPipeline, const VideoSenderOptions & videoSenderOptions, dz::VideoSender * sender) {
	int result = sender->open();
	if (result) {
		std::cerr << "Error: Could not open output stream: " << getVideoSenderError(result) << std::endl;
		return 1;
	}
	double t0 = microtime();
	gShutdown = false;
	installSigHandler ();
	installLineReader ();
	while (!gShutdown) {
		double t1 = microtime();
		double dt = t1 - t0;
		result = grabbingPipeline->grab();
		if (result) {
			std::cerr << "Error: could not grab " << result << std::endl;
			return 1;
		}
		const dz::Buffer * buffer = grabbingPipeline->buffer();
		result = sender->putFrame(buffer->data, buffer->width, buffer->height, buffer->rowLength, dt);
		if (result) {
			std::cerr << "Error: could not send: " << getVideoSenderError(result) << std::endl;
			return 1;
		}
#if QT_GUI_LIB
		if (QApplication::instance()){
            QApplication::processEvents();
            QApplication::sendPostedEvents();
        }
#endif
		double t2 = microtime ();
		double timeToWait = (1.0f / videoSenderOptions.fps) - (t2 - t1);
		if (timeToWait < 0) {
			std::cerr << "Warning: can not grab and send in time, will miss frames!" << std::endl;
		} else {
			std::cout << "Debug: Sleeping for " << timeToWait << "s" << std::endl;
			millisleep ( (int) (timeToWait * 1000));
		}
	}
	sender->close();
	return 0;
}

#ifdef MAC_OSX
extern "C" {
    // Initialize NSApplication
    // This is necessary to use 
    // Some calls to the window manager (e.g. Current Cursor)
    void initalizeNSApplication ();
}
#endif

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
