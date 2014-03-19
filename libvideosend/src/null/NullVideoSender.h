#pragma once
#include "../VideoSender.h"

namespace dz
{
	class NullVideoSender : public VideoSender
	{
		public:
			NullVideoSender();
			virtual ~NullVideoSender();

			// Implementation
			virtual void putFrame(VideoFrameHandle videoFrame, double durationInSec);
			virtual float GetFPS() const;
	};
}
