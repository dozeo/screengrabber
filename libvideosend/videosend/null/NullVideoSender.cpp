#include "NullVideoSender.h"
#include "../qt/QtVideoSender.h"

namespace dz {



NullVideoSender::NullVideoSender () {

}

NullVideoSender::~NullVideoSender() {

}

int NullVideoSender::setVideoSettings (int w, int h, float fps, int bitRate, enum VideoQualityLevel quality) {
	return 0;
}

int NullVideoSender::setTargetFile (const std::string & filename) {
	return 0;
}

int NullVideoSender::setTargetUrl (const std::string & url) {
	return 0;
}

int NullVideoSender::open () {
	return 0;
}

int NullVideoSender::putFrame (const uint8_t * data, int width, int height, int bytesPerRow, double durationInSec) {
	return 0;
}

void NullVideoSender::close () {
}

}
