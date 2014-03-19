#pragma once

/**
 * @file
 *
 * Encapsulates scaling and encoding of a video _AND_ sending to some target (file, url..)
 *
 */

#include <cstdint>
#include <string>

#include <libcommon/videotypes.h>
#include <libcommon/sender_options.h>
#include <libcommon/videoframe.h>

namespace dz
{
	class VideoSender 
	{
		public:
			virtual ~VideoSender() {}

			/// Creates a VideoSender instance
			static VideoSender* CreateVideoSender(const VideoSenderOptions& senderOptions);

			/// Puts in a new frame
			/// @param durationInSec exact time point not duration since last frame!
			virtual void putFrame(VideoFrameHandle videoFrame, double durationInSec) = 0;

			virtual float GetFPS() const = 0;

			typedef void (*LogCallback)(const std::string &logLine, void * user);

			/// If possible set a custom logging callback
			virtual void setLoggingCallback (const LogCallback& callback, void * user) {}

			/// Some statistic about video sending process
			struct Statistic
			{
				Statistic();

				/// Add one frame with its data to the statistic
				/// (lastTime... must be updated already)
				void frameWritten(int64_t bytes);

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
			virtual const Statistic* statistic () const { return 0; }
	};
}
