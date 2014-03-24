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
			virtual void SendFrame(VideoFrameHandle videoFrame, double durationInSec);
			virtual float GetFPS() const;
	};
}
