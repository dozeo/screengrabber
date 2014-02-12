#pragma once

#include "VideoSender.h"
#include "VideoStream.h"

#include <libgrabber/src/Dimension.h>

#include <memory>

namespace dz
{
	class VideoSenderFFmpeg : public VideoSender
	{
		public:
			VideoSenderFFmpeg();
			virtual ~VideoSenderFFmpeg();

			virtual void setVideoSettings(int w, int h, float fps, int bitRate, int keyframe, enum VideoQualityLevel quality);
			virtual void setTargetFile(const std::string & filename);
			virtual void setTargetUrl(const std::string & url);
			virtual void OpenVideoStream();
			virtual void putFrame(const uint8_t * data, int width, int height, int bytesPerRow, double durationInSec);
			virtual void close();
			// override
			virtual void setLoggingCallback (const LogCallback& callback, void * user);
			virtual const Statistic * statistic () const { return m_videoStream ? m_videoStream->statistic() : 0; }

		private:
			void initLog ();
			void uninitLog ();

			enum OpenMode {
				OM_URL = 1,
				OM_FILE,
			};

			std::unique_ptr<VideoStream> m_videoStream;

			std::string _url;
			OpenMode    _mode;

			Dimension2   _frameSize;
			float        _fps;
			int          _bitRate;
			int          _keyframe;
			VideoQualityLevel _quality;
			int          _defaultLogLevel;
	};
}
