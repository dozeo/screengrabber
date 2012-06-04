#include "QtVideoSender.h"
#include <QPainter>
#include <assert.h>
#include <QImage>

namespace dz {

void DrawingArea::paintEvent(QPaintEvent *) {
	QPainter painter (this);
	painter.fillRect (QRect (QPoint (0,0), this->size()), Qt::black);
	painter.drawImage (QPoint (0,0), mImage);
}

QtVideoSender::QtVideoSender () {
	mDrawingArea = 0;
	mWidth   = 0;
	mHeight  = 0;
	mFps     = 0;
	mBitRate = 0;
}

QtVideoSender::~QtVideoSender () {

}


int QtVideoSender::setVideoSettings(int w, int h, float fps, int bitRate, enum VideoQualityLevel quality) {
	assert (!mDrawingArea && "already started?");
	if (w < 1 || h < 0 || fps < 0.1 || bitRate < 1) return VE_INVALID_RESOLUTION;
	mWidth   = w;
	mHeight  = h;
	mFps     = fps;
	mBitRate = bitRate;
	return 0;
}

int QtVideoSender::setTargetFile(const std::string & filename){
	assert (!mDrawingArea && "already started?");
	mTargetFile = filename;
	mTargetUrl.clear();
	return 0;
}

int QtVideoSender::setTargetUrl(const std::string & url){
	assert (!mDrawingArea && "already started?");
	if (!mTargetFile.empty()) {
		return VE_INVALID_TARGET;
	}
	mTargetFile = url;
	return 0;
}

int QtVideoSender::open(){
	mDrawingArea = new DrawingArea ();
	mDrawingArea->show();
	mDrawingArea->setFixedSize (mWidth, mHeight);
	mDrawingArea->setWindowTitle ("Width: " + QString::number (mWidth) + " Height: " + QString::number (mHeight) + " FPS: " + QString::number(mFps));
	return 0;
}

int QtVideoSender::putFrame(const uint8_t *data, int width, int height, int bytesPerRow, double durationInSec){
	assert (mDrawingArea && "bad order");
	QImage image (data, width, height, bytesPerRow, QImage::Format_RGB32);
	QImage scaled = image.scaled(mWidth, mHeight);
	mDrawingArea->setImage (scaled);
	mDrawingArea->repaint();
	return 0;
}

void QtVideoSender::close(){
	if (mDrawingArea) {
		mDrawingArea->close();
		delete mDrawingArea;
		mDrawingArea = 0;
	}
}

}











