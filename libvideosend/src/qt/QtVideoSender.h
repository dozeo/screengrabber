#include "../VideoSender.h"
#include <QWidget>

namespace dz {

class DrawingArea : public QWidget {
public:
	DrawingArea (QWidget * parent = 0) : QWidget (parent) {}
	void setImage (const QImage& image) { mImage = image; }
protected:
    virtual void paintEvent(QPaintEvent *);
private:
	QImage mImage;
};

/// A video sender backend which disables the content in a window
class QtVideoSender : public VideoSender {
public:
	QtVideoSender ();
	virtual ~QtVideoSender ();

	// Implementation
	virtual void setVideoSettings (int w, int h, float fps, int bitRate, int keyframe, enum VideoQualityLevel quality);
	virtual void setTargetFile (const std::string & filename);
	virtual void setTargetUrl (const std::string & url);
	virtual void OpenVideoStream();
	virtual void putFrame (const uint8_t * data, int width, int height, int bytesPerRow, double durationInSec);
	virtual void close ();

private:
	DrawingArea * mDrawingArea;
	int  mWidth, mHeight;
	float mFps;
	int  mBitRate;
	int mKeyframe;
	std::string mTargetFile;
	std::string mTargetUrl;

};

}
