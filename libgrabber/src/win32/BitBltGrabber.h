#pragma once

#ifdef WIN32

#include "Win32Grabber.h"
#include "ScreenEnumerator.h"
#include "../Dimension.h"
#include <vector>
#include <Windows.h>
#include <GdiPlus.h>

namespace dz
{
	class BitBltGrabber : public Win32Grabber
	{
		public:
			static void fillBitmapInfo(int width, int height, BITMAPINFO& info);

			BitBltGrabber();
			virtual ~BitBltGrabber();

			virtual void grab(const Rect& rect, Buffer* destination);

		private:
			void initBitmap(const Dimension2& captureSize);
			void shutdownBitmap();

			void initBitmapBuffer(const Dimension2& captureSize);
			void shutdownBitmapBuffer();

			void resizeBitmapIfNecessary(int width, int height);
			void copyBitmapToBuffer(uint8_t* src, int srcWidth, int lines, Buffer* buffer);

			Dimension2 _captureSize;

			HDC _hdcDesktop;
			HDC _hdcCapture;

			HBITMAP _hBitmap;
			BITMAPINFO _bmpInfo;
			uint8_t* _bmpBuffer;
	};
}

#endif
