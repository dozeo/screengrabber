#pragma once

#include <libgrabber/src/Grabber.h>
#include <libgrabber/src/IWindowGrabber.h>

#include "GrabberOptions.h"

#include <libgrabber/src/IDesktopTools.h>

#include <libgrabber/src/WindowInfo.h>
#include <libgrabber/src/ProcessInfo.h>
#include <iostream>

#include <dzlib/dzexception.h>

#include <boost/scoped_ptr.hpp>

#define BITS_PER_PIXEL 32

/// Manages the whole grabbing pipeline + configuration
///
/// In order to make it includeable in other applications this
/// class is completely inlined.
class GrabbingPipeline
{
	public:
		GrabbingPipeline(const GrabberOptions* options, bool correctAspectToVideo = false, int videoW = 0, int videoH = 0) :
			mGrabberOptions(options), mCorrectAspectToVideo(correctAspectToVideo), mVideoWidth(videoW), mVideoHeight(videoH)
		{
			dz::GrabberType::Enum grabberType = mGrabberOptions->grabberType;

			int64_t windowId = options->grabWid;
			if (windowId != -1)
			{
				m_grabber.reset(dz::IWindowGrabber::CreateWindowGrabber(windowId));
				grabberType = dz::GrabberType::GrabWindow;
			}
			else
				m_grabber.reset(dz::IGrabber::create(mGrabberOptions->grabberType));

			mCurrentGrabberType = grabberType;

			mGrabRect = calcGrabRect();
			if (mGrabRect.empty())
				throw dz::exception("Grab rectangle was calculated to be empty (0 width and height)");

			mDestinationBuffer.init(mGrabRect.w, mGrabRect.h, mGrabRect.w * (BITS_PER_PIXEL/8));
			mDestinationBuffer.clear();
			m_grabber->setEnableGrabCursor(mGrabberOptions->grabCursor);
		}

		~GrabbingPipeline ()
		{
			m_grabber.reset(NULL);
		}

		/// Sets option, must be done before reinit or grab
		void setOptions(const GrabberOptions* options, bool correctAspectToVideo = false, int videoW = 0, int videoH = 0)
		{
			mGrabberOptions = options;
			mCorrectAspectToVideo = correctAspectToVideo;
			mVideoWidth  = videoW;
			mVideoHeight = videoH;
		}

		// (Re-) initializes grabber
		// You need this only if you want early error codes
		//int reinit()
		//{
		//	if (!mGrabber || mGrabberOptions->grabberType != mCurrentGrabberType)
		//	{
		//		mGrabber = dz::Grabber::create(mGrabberOptions->grabberType);
		//		mGrabber->init();

		//		mCurrentGrabberType = mGrabberOptions->grabberType;

		//		mGrabRect = calcGrabRect();
		//		if (mGrabRect.empty())
		//		{
		//			std::cerr << "Error: Invalid grabbing rect" << std::endl;
		//			return 1;
		//		}

		//		mDestinationBuffer.init (mGrabRect.w, mGrabRect.h);
		//		mDestinationBuffer.clear();
		//		mGrabber->setEnableGrabCursor(mGrabberOptions->grabCursor);
		//	}

		//	// nothing to do
		//	return 0;
		//}

		// Grab one image; you can get the image using the buffer
		void grab()
		{
			//int result = reinit();
			//if (result)
			//	return result;

			if (mGrabberOptions->grabFollow)
			{
				dz::Rect r = calcGrabRect();
				if (!(mGrabRect == r))
				{
					// otherwise we could get artefacts.
					mDestinationBuffer.clear();
				}

				mGrabRect = r;
				if (mGrabRect.empty())
					throw dz::exception("Grab rectangle is empty");
			}

			int bufferWidth(mGrabRect.w);
			int bufferHeight(mGrabRect.h);
			int letterX = 0; ///< Pillarbox (black left and right)
			int letterY = 0; ///< Letterbox (black up and down)

			if (mCorrectAspectToVideo)
			{
				float aspect = (float) mVideoWidth / (float) mVideoHeight;
				int bestWidth = (int)(aspect * mGrabRect.h);
				if (bestWidth > mGrabRect.w)
				{
					// increase width
					bufferWidth = bestWidth;
					letterX = (bestWidth - mGrabRect.w) / 2;
				}
				else
				{
					// increase height
					int bestHeight = (int)(mGrabRect.w / aspect);
					if (bestHeight > mGrabRect.h)
					{
						bufferHeight = bestHeight;
						letterY = (bestHeight - mGrabRect.h) / 2;
					}
					else
					{
						// huh? rounding errors?
						// we are probably already right
					}
				}
			}

			if (mDestinationBuffer.width != bufferWidth || mDestinationBuffer.height != bufferHeight)
				mDestinationBuffer.init(bufferWidth, bufferHeight);

			mDestinationBufferBox.initAsSubBufferFrom(&mDestinationBuffer, letterX, letterY, mGrabRect.w, mGrabRect.h);

			m_grabber->grab(mGrabRect, &mDestinationBufferBox);
		}

		// Returns the image of the last grab
		// Undefined if there wasnt a last grab
		const dz::Buffer * buffer() const {
			return &mDestinationBuffer;
		}

		// Access to current grabrect
		const dz::Rect& grabRect () const {
			return mGrabRect;
		}

		// Access to current grabber
		const dz::IGrabber* grabber() const { return m_grabber.get(); }

		const dz::IDesktopTools& GetDesktopTools() const { return *m_desktopTools.get(); }

	private:

		/// Calculates grabbing rect
		dz::Rect calcGrabRect()
		{
			if (!mGrabberOptions->grabRect.empty())
			{
				// grab rect given
				return mGrabberOptions->grabRect;
			}
			else if (mGrabberOptions->grabScreen >= 0)
			{
				// screen given
				return GetDesktopTools().GetScreenResolution(mGrabberOptions->grabScreen);
			}
			else if (mGrabberOptions->grabPid > 0)
			{
				// pid given
				dz::Rect grabRect;

				std::vector<dz::WindowInfo> windows;
				dz::WindowInfo::populate (&windows, mGrabberOptions->grabPid);
				for (size_t i = 0; i != windows.size(); i++) {
					grabRect.addToBoundingRect (windows[i].area);
				}

				return grabRect;
			}
			else if (mGrabberOptions->grabWid > 0)
			{
				// wid given
				dz::WindowInfo window = dz::WindowInfo::about(mGrabberOptions->grabWid);

				dz::Rect grabRect;
				grabRect.addToBoundingRect(window.area);

				return grabRect;
			}
			
			// else grab everything
			return m_desktopTools->GetCombinedScreenResolution();
		}

		const GrabberOptions * mGrabberOptions; ///< Current set video options, must stay valid as long as grab() and init () rns
		dz::Buffer mDestinationBuffer;			///< Buffer in which grabbing is done, can be recreated during grab calls
		dz::Buffer mDestinationBufferBox;		///< The box isnide the destination buffer in which is grabbed actually
		dz::Rect mGrabRect;						///< Current grab rect
		dz::GrabberType::Enum mCurrentGrabberType;	///< Current initialized grabber type
		boost::scoped_ptr<dz::IGrabber> m_grabber;					///< Current Grabber
		boost::scoped_ptr<dz::IDesktopTools> m_desktopTools;
		bool mCorrectAspectToVideo;				///< Rescale the grabbed images, so that they have the same aspect like the destination video
		// Destination video size, only valid if correctAspectToVideo is set to true
		int  mVideoWidth;
		int mVideoHeight;
};
