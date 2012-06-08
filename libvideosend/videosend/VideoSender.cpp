
#include "VideoSender.h"
#include "null/NullVideoSender.h"
#include "qt/QtVideoSender.h"
#include "ffmpeg/VideoSenderFFmpeg.h"
#include <sstream>

namespace dz {

VideoSender* VideoSender::create(VideoSenderType type)
{
	if (type == VT_NULL)
	{
		return new NullVideoSender();
	}
	if (type == VT_QT)
	{
		return new QtVideoSender();
	}
	else if (type == VT_DEFAULT)
	{		
		return new VideoSenderFFmpeg();
	}
	return 0;
}

VideoSender::Statistic::Statistic () {
	framesWritten  = 0;
	bytesSent      = 0;
	lastScaleTime  = 0;
	lastEncodeTime = 0;
	lastSendTime   = 0;
	sumEncodeTime  = 0;
	sumSendTime    = 0;
	sumScaleTime   = 0;
}
/// Add one frame with its data to the statistic
/// (lastTime... must be updated already)
void VideoSender::Statistic::frameWritten (int64_t bytes) {
	bytesSent+=bytes;
	framesWritten++;
	sumScaleTime   += lastScaleTime;
	sumEncodeTime  += lastEncodeTime;
	sumSendTime    += lastSendTime;
}


std::ostream & operator << (std::ostream & o, const VideoSender::Statistic & s) {
	o
		<< s.framesWritten << "(" << (s.bytesSent / 1024) << "kb)"
		<< " sc:" << (s.sumScaleTime / 1000) << "ms e:" << (s.sumEncodeTime / 1000) << "ms tx: " << (s.lastSendTime / 1000) << "ms";
	return o;
}

}




