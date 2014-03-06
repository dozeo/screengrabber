#include "QtVideoSender.h"

#include <QPainter>
#include <assert.h>
#include <QImage>

#include <dzlib/dzexception.h>

namespace dz
{
	void DrawingArea::paintEvent(QPaintEvent *) {
		QPainter painter (this);
		painter.fillRect (QRect (QPoint (0,0), this->size()), Qt::black);
		painter.drawImage (QPoint (0,0), mImage);
	}

	QtVideoSender::QtVideoSender () {
		mDrawingArea = 0;
		mWidth    = 0;
		mHeight   = 0;
		mFps      = 0;
		mBitRate  = 0;
		mKeyframe = 0;
	}

	QtVideoSender::~QtVideoSender () {
	}

	void QtVideoSender::setVideoSettings(int w, int h, float fps, int bitRate, int keyframe, enum VideoQualityLevel quality)
	{
		assert (!mDrawingArea && "already started?");

		if (w < 1 || h < 0 || fps < 0.1 || bitRate < 1)
			throw exception(strstream() << "Invalid resolution, ftp or bitrate (w: " << w << ", h:" << h << ", fps: " << fps << ", bitrate: " << bitRate);

		mWidth    = w;
		mHeight   = h;
		mFps      = fps;
		mKeyframe = keyframe;
		mBitRate  = bitRate;
	}

	void  QtVideoSender::setTargetFile(const std::string & filename)
	{
		assert (!mDrawingArea && "already started?");
		mTargetFile = filename;
		mTargetUrl.clear();
	}

	void QtVideoSender::setTargetUrl(const std::string & url)
	{
		assert (!mDrawingArea && "already started?");
	
		if (!mTargetFile.empty())
			throw exception("target url is not empty... and thus invalid?!");
	
		mTargetFile = url;
	}

	void QtVideoSender::OpenVideoStream()
	{
		mDrawingArea = new DrawingArea ();
		mDrawingArea->show();
		mDrawingArea->setFixedSize (mWidth, mHeight);
		mDrawingArea->setWindowTitle ("Width: " + QString::number (mWidth) + " Height: " + QString::number (mHeight) + " FPS: " + QString::number(mFps));
	}

	void  QtVideoSender::putFrame(const uint8_t *data, int width, int height, int bytesPerRow, double durationInSec)
	{
		assert (mDrawingArea && "bad order");
		QImage image (data, width, height, bytesPerRow, QImage::Format_RGB32);
		QImage scaled = image.scaled(mWidth, mHeight);
		mDrawingArea->setImage (scaled);
		mDrawingArea->repaint();
	}

	void QtVideoSender::close(){
		if (mDrawingArea) {
			mDrawingArea->close();
			delete mDrawingArea;
			mDrawingArea = 0;
		}
	}
}
