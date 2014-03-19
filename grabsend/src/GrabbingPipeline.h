#pragma once

//#include "GrabberOptions.h"
#include "GrabSendOptions.h"

#include <libcommon/dzrect.h>

#include <libgrabber/src/IDesktopTools.h>
#include <libgrabber/src/igrabber.h>
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
		GrabbingPipeline(const GrabSendOptions& options) :
			m_grabberOptions(options), mVideoWidth(options.videowidth), mVideoHeight(options.videoheight),
			m_grabber(dz::IGrabber::CreateGrabber(options))
		{
			//dz::GrabberType::Enum grabberType = m_grabberOptions.m_grabberType;

			//int64_t windowId = m_grabberOptions.m_grabWindowId;
			//if (windowId != -1)
			//{
			//	m_grabber.reset(dz::IWindowGrabber::CreateWindowGrabber(windowId));
			//	grabberType = dz::GrabberType::GrabWindow;
			//}
			//else
			//	m_grabber.reset(dz::IGrabber::create(grabberType));

			m_grabRect = calcGrabRect();
			if (m_grabRect.empty())
				throw dz::exception("Grab rectangle was calculated to be empty (0 width and height)");

			mDestinationBuffer.init(m_grabRect.width, m_grabRect.height, m_grabRect.width * (BITS_PER_PIXEL/8));
			mDestinationBuffer.clear();
		}

		~GrabbingPipeline()
		{
			m_grabber.reset(NULL);
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

		//		m_grabRect = calcGrabRect();
		//		if (m_grabRect.empty())
		//		{
		//			std::cerr << "Error: Invalid grabbing rect" << std::endl;
		//			return 1;
		//		}

		//		mDestinationBuffer.init (m_grabRect.w, m_grabRect.h);
		//		mDestinationBuffer.clear();
		//		mGrabber->setEnableGrabCursor(mGrabberOptions->grabCursor);
		//	}

		//	// nothing to do
		//	return 0;
		//}

		// Grab one image; you can get the image using the buffer
		dz::VideoFrameHandle grab()
		{
			m_grabber->SetCaptureRect(m_grabRect);

			return m_grabber->GrabVideoFrame();
		}

		// Returns the image of the last grab
		// Undefined if there wasnt a last grab
		const dz::Buffer * buffer() const {
			return &mDestinationBuffer;
		}

		// Access to current grabrect
		const dz::Rect GetGrabRect() const { return m_grabRect; }

		// Access to current grabber
		const dz::IGrabber* grabber() const { return m_grabber.get(); }

		const dz::IDesktopTools& GetDesktopTools() const { return *m_desktopTools.get(); }

	private:

		/// Calculates grabbing rect
		dz::Rect calcGrabRect()
		{
			dz::Rect result = m_grabberOptions.m_grabRect;

			if (!result.empty())
			{
				// grab rect given
				return result;
			}
			else if (m_grabberOptions.m_grabScreen >= 0)
			{
				// screen given
				return GetDesktopTools().GetScreenResolution(m_grabberOptions.m_grabScreen);
			}
			//else if (m_grabberOptions.GetPro ->grabPid > 0)
			//{
			//	// pid given
			//	dz::Rect grabRect;

			//	std::vector<dz::WindowInfo> windows;
			//	dz::WindowInfo::populate (&windows, mGrabberOptions->grabPid);
			//	for (size_t i = 0; i != windows.size(); i++) {
			//		grabRect.addToBoundingRect (windows[i].area);
			//	}

			//	return grabRect;
			//}
			else if (m_grabberOptions.m_grabWindowId > 0)
			{
				// wid given
				dz::WindowInfo window = dz::WindowInfo::about(m_grabberOptions.m_grabWindowId);

				dz::Rect grabRect;
				grabRect.addToBoundingRect(window.area);

				return grabRect;
			}
			
			// else grab everything
			return m_desktopTools->GetCombinedScreenResolution();
		}

		const GrabberOptions& m_grabberOptions; ///< Current set video options, must stay valid as long as grab() and init () rns
		dz::Buffer mDestinationBuffer;			///< Buffer in which grabbing is done, can be recreated during grab calls
		dz::Buffer mDestinationBufferBox;		///< The box isnide the destination buffer in which is grabbed actually
		dz::Rect m_grabRect;						///< Current grab rect
		//dz::GrabberType::Enum mCurrentGrabberType;	///< Current initialized grabber type
		boost::scoped_ptr<dz::IGrabber> m_grabber;					///< Current Grabber
		boost::scoped_ptr<dz::IDesktopTools> m_desktopTools;
		
		// Destination video size, only valid if correctAspectToVideo is set to true
		int  mVideoWidth;
		int mVideoHeight;
};
