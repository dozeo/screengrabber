
#include "VideoSender.h"
#include "null/NullVideoSender.h"
#include "ffmpeg/VideoSenderFFmpeg.h"

#include <dzlib/dzexception.h>

#include <sstream>
#include <algorithm>

namespace dz
{
	//static
	VideoSender* VideoSender::CreateVideoSender(const VideoSenderOptions& senderOptions)
	{
		switch (senderOptions.senderType)
		{
			case VideoSenderType::Null: return new NullVideoSender();
			case VideoSenderType::Default: return new VideoSenderFFmpeg(senderOptions);
		}
	
		throw exception(strstream() << "Could not create video sender of type " << senderOptions.senderType);
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
