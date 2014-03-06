#pragma once

#ifdef WIN32

#include "grabber_win32.h"
#include "win32/ScreenEnumerator.h"
#include "Dimension.h"
#include <vector>
#include <Windows.h>
#include <GdiPlus.h>

class VideoFrame;

namespace dz
{
	class BitBltGrabber_Win32 : public Grabber_Win32
	{
		public:
			BitBltGrabber_Win32(Rect capture);
			virtual ~BitBltGrabber_Win32();

			virtual void SetCaptureRect(Rect capture);

			virtual VideoFrameHandle GrabVideoFrame();

			static void fillBitmapInfo(int width, int height, BITMAPINFO& info);

		private:
			void initBitmap(const Dimension2& captureSize);
			void shutdownBitmap();

			void initBitmapBuffer(const Dimension2& captureSize);
			void shutdownBitmapBuffer();

			void resizeBitmapIfNecessary(int width, int height);
			void copyBitmapToBuffer(uint8_t* src, int srcWidth, int lines, Buffer* buffer);
			void CopyToFrame(VideoFrameHandle& frame);

			Rect m_captureRect;
			Dimension2 _captureSize;

			HDC _hdcDesktop;
			HDC _hdcCapture;

			HBITMAP _hBitmap;
			BITMAPINFO _bmpInfo;
			uint8_t* _bmpBuffer;
	};
}

#endif
