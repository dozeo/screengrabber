#include "NullVideoSender.h"
#include "../qt/QtVideoSender.h"

namespace dz
{
	NullVideoSender::NullVideoSender () {}

	NullVideoSender::~NullVideoSender() {}

	void NullVideoSender::setVideoSettings (int w, int h, float fps, int bitRate, int keyframe, enum VideoQualityLevel quality) {}

	void NullVideoSender::setTargetFile (const std::string & filename) {}

	void NullVideoSender::setTargetUrl (const std::string & url) {}

	void NullVideoSender::OpenVideoStream() {}

	void NullVideoSender::putFrame (const uint8_t * data, int width, int height, int bytesPerRow, double durationInSec) {}

	void NullVideoSender::close () {
	}
}
