#include "NullVideoSender.h"

namespace dz
{
	NullVideoSender::NullVideoSender () {}

	NullVideoSender::~NullVideoSender() {}

	float NullVideoSender::GetFPS() const { return 24.f; }

	void NullVideoSender::putFrame(VideoFrameHandle videoFrame, double durationInSec)
	{
	}
}
