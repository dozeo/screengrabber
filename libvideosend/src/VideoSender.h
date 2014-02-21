#pragma once

/**
 * @file
 *
 * Encapsulates scaling and encoding of a video _AND_ sending to some target (file, url..)
 *
 */

#include <cstdint>
#include <string>

namespace dz
{
	enum VideoSenderType { VT_NULL, VT_DEFAULT, VT_QT };
	enum VideoQualityLevel { VQ_LOW = 0, VQ_MEDIUM, VQ_HIGH };

	class VideoSender 
	{
		public:
			//enum VideoSenderError
			//{
			//	VE_OK = 0,
			//	VE_INVALID_RESOLUTION = 1,
			//	VE_INVALID_TARGET = 2,
			//	VE_CODEC_NOT_FOUND = 3,
			//	VE_INVALID_CONVERSION = 4,
			//	VE_FAILED_OPEN_STREAM = 5,
			//};

			virtual ~VideoSender () {}

			/// Creates a VideoSender instance
			static VideoSender* create (VideoSenderType type = VT_DEFAULT);

			/// Usage: at first call setters and then init
			virtual void setVideoSettings (int w, int h, float fps, int bitRate, int keyframe, enum VideoQualityLevel quality = VQ_MEDIUM) = 0;

			/// Sets a target file
			virtual void setTargetFile (const std::string & filename) = 0;

			/// Sets target url (do not use together with target file)
			virtual void setTargetUrl (const std::string & url) = 0;

			/// Opens video stream
			virtual void OpenVideoStream() = 0;

			/// Puts in a new frame
			/// @param durationInSec exact time point not duration since last frame!
			virtual void putFrame (const uint8_t * data, int width, int height, int bytesPerRow, double durationInSec) = 0;

			/// Closes video stream
			virtual void close () = 0;

			typedef void (*LogCallback )(const std::string &logLine, void * user);
			/// If possible set a custom logging callback
			virtual void setLoggingCallback (const LogCallback& callback, void * user) {}

			///@name Statistic
			///@{

			/// Some statistic about video sending process
			struct Statistic {
				Statistic ();

				/// Add one frame with its data to the statistic
				/// (lastTime... must be updated already)
				void frameWritten (int64_t bytes);

				int32_t framesWritten;	///< Frames written so far
				int64_t bytesSent;		///< Bytes sent so far

				// All timing information in µs

				// last putFrameOperation
				int64_t lastScaleTime;	///< last time used for scaling image
				int64_t lastEncodeTime;	///< last time used for encoding image
				int64_t lastSendTime;	///< last time used for sending image

				// summarized frame operation
				int64_t sumScaleTime;	///< summarized scaling time
				int64_t sumEncodeTime;	///< summarized encoding time
				int64_t sumSendTime;	///< summarized sending time
			};

			/// Gets statistic about current sending process
			/// Not available in all implementations
			virtual const Statistic * statistic () const { return 0; }

			///@}
	};
}
