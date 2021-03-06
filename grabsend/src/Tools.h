#pragma once

#include <libgrabber/src/Grabber.h>
#include <libvideosend/src/VideoSender.h>

/**
 * @file
 * Various tool functions used in grabsend
 * mostly print-out functions
 */

/// Sleep for a given time in milliseconds
void millisleep (int timeMs);

/// Returns a time stamp with millisecond to microsecond exactness
/// Note: it is for measuring time intervals, NOT exact time of day.
double microtime ();

/// Print all identified screens
void printScreens (const dz::Grabber * grabber);

/// Print all identified windows
void printWindows (const dz::Grabber * grabber);

/// Print all identified processes
void printProcesses (const dz::Grabber * grabber);

/// Print statistic about last frame
std::ostream& printLastFrame (std::ostream & o, const dz::VideoSender::Statistic & s);

/// Print statistic about summarized timings
std::ostream& printSum (std::ostream & o, const dz::VideoSender::Statistic & s);

/// Print statisitc about average timings
/// Note: sent bytes is not averaged
std::ostream & printAvg (std::ostream & o, const dz::VideoSender::Statistic & s);

/// Analyze the cause of framedrops (the part of the encoding chain which takes too much time)
std::ostream & analyseFrameDropCause (std::ostream & o, int frame, double grabTimeSum, const dz::VideoSender::Statistic & s);

//@name  Endless Loop Handlers
//@{

/// The loop is to be shut down (CTRL+C or QUIT\n entered if handlers are installed)
bool shutDownLoop ();

/// Installs signal handler for SIGINT which will set shutDownLoop to true if pressed
void installSigHandler ();

/// Install a second thread which is reading std::cin and waiting for QUIT to quit the loop
void installLineReader ();

//@}

#ifdef MAC_OSX
extern "C" {
    // Initialize NSApplication
    // This is necessary to use
    // Some calls to the window manager (e.g. Current Cursor)
    void initalizeNSApplication ();
}
#endif

