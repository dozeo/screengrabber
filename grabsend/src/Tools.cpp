#ifndef WIN32
#include <unistd.h>
#include <sys/time.h>
#else
#include <windows.h>
#endif
#include <iostream>
#include <boost/thread.hpp>
#include <stdio.h>
#include <grabber/WindowInfo.h>
#include <grabber/ProcessInfo.h>
#include <assert.h>
#include <signal.h>
#include "Tools.h"

void millisleep (int timeMs) {
#ifdef WIN32
	::Sleep (timeMs);
#else
	usleep (timeMs * 1000);
#endif
}

double microtime (){
	// Note: following snippet is from Schneeflocke (github.com/nob13/schneeflocke)
	// I hereby license this as public domain (Norbert Schultz)
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

std::ostream& printLastFrame (std::ostream & o, const dz::VideoSender::Statistic & s) {
	o
		<< "[" << s.framesWritten << "] last: sc:" << (s.lastScaleTime / 1000) << "ms e:" << (s.lastEncodeTime / 1000) << "ms tx: " << (s.lastSendTime / 1000) << "ms";
	return o;
}

std::ostream& printSum (std::ostream & o, const dz::VideoSender::Statistic & s) {
	o
		<< "[" << s.framesWritten << "]  " << (s.bytesSent / 1024) << "kb "
		<< "sum: sc:" << (s.sumScaleTime / 1000) << "ms e:" << (s.sumEncodeTime / 1000) << "ms tx: " << (s.sumSendTime / 1000) << "ms";
	return o;
}

std::ostream & printAvg (std::ostream & o, const dz::VideoSender::Statistic & s) {
	o
		<< "[" << s.framesWritten << "] " << (s.bytesSent / 1024) << "kb "
		<< "avg: sc:" << (s.sumScaleTime / s.framesWritten / 1000) << "ms e:" << (s.sumEncodeTime / s.framesWritten / 1000) << "ms tx: " << (s.sumSendTime / s.framesWritten / 1000) << "ms";
	return o;
}

/// Returns true if a is significant (more than 10% of a) greater than b
static bool sigGreater (double a, double b) {
	return (a - b) / a > 0.1;
}

std::ostream & analyseFrameDropCause (std::ostream & o, int frame, double grabTimeSum, const dz::VideoSender::Statistic & s) {
	double avgGrabTime           = grabTimeSum / (frame);
	double avgScaleAndEncodeTime = (s.sumScaleTime + s.sumEncodeTime) / 1000000.0 / frame;
	double avgSendTime           = (s.sumSendTime) / 1000000.0 / frame;
	assert (avgGrabTime >= 0);
	assert (avgScaleAndEncodeTime >=0);
	assert (avgSendTime >= 0);

	if (sigGreater (avgGrabTime, avgScaleAndEncodeTime) && sigGreater (avgGrabTime, avgSendTime)) {
		return o << "DCGRAB cannot grab in time, try to reduce fps or grabbing area";
	}
	if (sigGreater (avgScaleAndEncodeTime, avgGrabTime) && sigGreater (avgScaleAndEncodeTime, avgSendTime)) {
		return o << "DCENCD cannot encode in time, try to reduce fps or grabbing area";
	}
	if (sigGreater (avgSendTime, avgGrabTime) && sigGreater (avgSendTime, avgScaleAndEncodeTime)) {
		return o << "DCSEND cannot send in time, try to reduce bitrate";
	}
	// unknown
	return o << "DCUNKN cannot grab,encode and send in time, try to reduce fps and bitrate";
}

volatile static bool gShutdown = false;
bool shutDownLoop () { return gShutdown; }

static void signalHandler (int id) {
	// using printf as we are in a signal handler method
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
static void lineReader () {
	while (std::cin && !gShutdown) {
		std::string line;
		std::getline(std::cin, line);
		if (line == "QUIT"){
			// using printf as we are in a second thread
			printf ("Read QUIT, shutting down\n");
			break;
		}
	}
	gShutdown = true;
}

void installLineReader () {
	boost::thread t (&lineReader);
}

