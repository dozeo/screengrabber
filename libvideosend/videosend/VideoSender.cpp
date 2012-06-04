
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

}
