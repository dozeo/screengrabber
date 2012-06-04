#pragma once
#include <string>
#include <grabber/platform.h>

#include <string>

namespace dz {

/**
 * @file
 *
 * Encapsulates scaling and encoding of a video _AND_ sending to some target (file, url..)
 *
 */

enum VideoSenderType { VT_NULL, VT_DEFAULT, VT_QT };
enum VideoQualityLevel { VQ_LOW = 0, VQ_MEDIUM, VQ_HIGH };

class VideoSender {
public:

	enum VideoSenderError {
		VE_OK = 0,
		VE_INVALID_RESOLUTION = 1,
		VE_INVALID_TARGET = 2,
		VE_CODEC_NOT_FOUND = 3,
		VE_INVALID_CONVERSION = 4,
		VE_FAILED_OPEN_STREAM = 5,
	};

	virtual ~VideoSender () {}

	/// Creates a VideoSender instance
	static VideoSender* create (VideoSenderType type = VT_DEFAULT);

	/// Usage: at first call setters and then init
	virtual int setVideoSettings (int w, int h, float fps, int bitRate, enum VideoQualityLevel quality = VQ_MEDIUM) = 0;

	/// Sets a target file
	virtual int setTargetFile (const std::string & filename) = 0;

	/// Sets target url (do not use together with target file)
	virtual int setTargetUrl (const std::string & url) = 0;

	/// Opens video stream
	virtual int open () = 0;

	/// Puts in a new frame
	/// @param durationInSec exact time point not duration since last frame!
	virtual int putFrame (const uint8_t * data, int width, int height, int bytesPerRow, double durationInSec) = 0;

	/// Closes video stream
	virtual void close () = 0;

	typedef void (*LogCallback )(const std::string &logLine, void * user);
	/// If possible set a custom logging callback
	virtual void setLoggingCallback (const LogCallback& callback, void * user) {}

};

}
