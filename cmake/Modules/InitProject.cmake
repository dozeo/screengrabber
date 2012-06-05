
# ! NOTE ! SCREENGRABBER_DIRECTORY needs to be set

# first set module path
set     (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${SCREENGRABBER_DIRECTORY}/cmake/Modules/")
include (Win32RelativeFolder)

# find and list all dependencies!
message (STATUS "Screengrabber Dependencies")

# platform specific libraries
if (LINUX)
	set (SCREENGRAB_LIBS ${SCREENGRAB_LIBS} X11 dl Xext Xrandr pthread)
    SET(CMAKE_INSTALL_RPATH "\$ORIGIN/../lib")
endif()
if (MAC_OSX)
	find_library (APP_SERVICES ApplicationServices REQUIRED )
	find_library (COCOA Cocoa REQUIRED)
	set (SCREENGRAB_LIBS ${SCREENGRAB_LIBS} ${COCOA} ${APP_SERVICES})
endif()
if (WIN32)
	set (SCREENGRAB_LIBS ${SCREENGRAB_LIBS} psapi) # Process information in libgrabber
endif()

# ffmpeg
message (STATUS "FFMPEG Searching ...")
find_package (FFMPEG REQUIRED)
message (STATUS "    FFMPEG_FOUND: ${FFMPEG_FOUND}")
if (FFMPEG_FOUND)
 	message (STATUS "        FFMPEG_INCLUDE_DIRS=${FFMPEG_INCLUDE_DIRS}")
 	message (STATUS "        FFMPEG_LIBRARIES=${FFMPEG_LIBRARIES}")
 	include_directories (${FFMPEG_INCLUDE_DIRS})
	set (LIBS ${LIBS} ${SCREENGRAB_LIBS})
endif ()

# DirectX
if (WIN32)
	include (FindDirectX)
	find_package (DIRECTX REQUIRED)
	message (STATUS "    DIRECTX_FOUND: ${DIRECTX_FOUND}")
	if (DIRECTX_FOUND)
		include_directories (${DIRECTX_INCLUDE_DIRS})
		set (SCREENGRAB_LIBS ${SCREENGRAB_LIBS} ${DIRECTX_LIBRARIES})
		message (STATUS "         DIRECTX_INCLUDE_DIRS=${DIRECTX_INCLUDE_DIRS}")
		message (STATUS "         DIRECTX_LIBRARIES=${DIRECTX_LIBRARIES}")
		set (SCREENGRAB_LIBS ${SCREENGRAB_LIBS} ${DIRECTX_LIBRARIES})
	endif ()
endif ()
