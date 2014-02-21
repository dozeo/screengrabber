
#include "VideoSender.h"
#include "null/NullVideoSender.h"
#include "qt/QtVideoSender.h"
#include "ffmpeg/VideoSenderFFmpeg.h"
#include <sstream>

#include <dzlib/dzexception.h>

namespace dz
{
	VideoSender* VideoSender::create(VideoSenderType type)
	{
		switch (type)
		{
			case VT_NULL: return new NullVideoSender();
			case VT_QT: return new QtVideoSender();
			case VT_DEFAULT: return new VideoSenderFFmpeg();
		}
	
		throw exception(strstream() << "Could not create video sender of type " << type);
	}

	VideoSender::Statistic::Statistic()
	{
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
	void VideoSender::Statistic::frameWritten(int64_t bytes)
	{
		bytesSent+=bytes;
		framesWritten++;
		sumScaleTime   += lastScaleTime;
		sumEncodeTime  += lastEncodeTime;
		sumSendTime    += lastSendTime;
	}
}
