
# ! NOTE ! SCREENGRABBER_DIRECTORY needs to be set

# SCREENGRAB_LIBS      - NON-GPL libraries used by screengrabber
# SCREENGRAB_OPEN_LIBS - all open source libraries used by screengrabber
# SCREENGRAB_INCLUDES  - all include directories

# platform specific libraries
if (LINUX)
	set (SCREENGRAB_LIBS ${SCREENGRAB_LIBS} X11 dl Xext Xrandr pthread)
    SET(CMAKE_INSTALL_RPATH "\$ORIGIN/../lib")
endif()
if (MAC_OSX)
	find_library (APP_SERVICES ApplicationServices REQUIRED)
	find_library (COCOA Cocoa REQUIRED)
	set (SCREENGRAB_LIBS ${SCREENGRAB_LIBS} ${COCOA} ${APP_SERVICES})
endif()
if (WIN32)
	set (SCREENGRAB_LIBS ${SCREENGRAB_LIBS} psapi) # Process information in libgrabber
endif()

# Qt
find_package (Qt4 COMPONENTS QtMain QtCore QtGui QtXml)
if (QT4_FOUND)
	include (${QT_USE_FILE})
	set (SCREENGRAB_LIBS ${SCREENGRAB_LIBS} ${QT_LIBRARIES})
	set (SCREENGRAB_INCLUDES ${SCREENGRAB_INCLUDES} ${QT_INCLUDE_DIR})
endif()

# ffmpeg
find_package (FFMPEG REQUIRED)
if (FFMPEG_FOUND)
	set (SCREENGRAB_OPEN_LIBS ${SCREENGRAB_OPEN_LIBS} ${FFMPEG_LIBRARIES})
	set (SCREENGRAB_INCLUDES ${SCREENGRAB_INCLUDES} ${FFMPEG_INCLUDE_DIRS})
endif ()

# DirectX
if (WIN32)
	find_package (DIRECTX REQUIRED)
	if (DIRECTX_FOUND)
		set (SCREENGRAB_LIBS ${SCREENGRAB_LIBS} ${DIRECTX_LIBRARIES})
		set (SCREENGRAB_INCLUDES ${SCREENGRAB_INCLUDES} ${DIRECTX_INCLUDE_DIRS})
	endif ()
endif ()

# Boost
find_package (Boost COMPONENTS program_options REQUIRED)
if (Boost_FOUND)
	set (SCREENGRAB_INCLUDES ${SCREENGRAB_INCLUDES} ${Boost_INCLUDE_DIRS})
	if (NOT WIN32)
		# Boost for windows finds its libraries by itself.
		set (SCREENGRAB_LIBS ${SCREENGRAB_LIBS} ${Boost_LIBRARIES})
	endif()
endif()

# OpenSSL
if (NOT WIN32)
	find_package (OpenSSL REQUIRED)
else ()
	# Use our own OpenSSL here
	set (OpenSSL_FOUND TRUE)
	set (OpenSSL_LIBRARIES ${SCREENGRABBER_DIRECTORY}/dependencies/lib/ssleay32.lib ${SCREENGRABBER_DIRECTORY}/dependencies/lib/libeay32.lib)
endif()
set (SCREENGRAB_OPEN_LIBS ${SCREENGRAB_OPEN_LIBS} ${OpenSSL_LIBRARIES})


#
# lists all dependencies
#
function (LIST_SCREENGRAB_DEPENDENCIES)
	message (STATUS "Screengrabber Dependencies")
	message (STATUS "    QT4_FOUND: ${QT4_FOUND}")
	if (QT4_FOUND)
		message (STATUS "        QT_LIBRARIES: ${QT_LIBRARIES}")
		message (STATUS "        QT_INCLUDE_DIR: ${QT_INCLUDE_DIR}")
	endif()

	message (STATUS "    FFMPEG_FOUND: ${FFMPEG_FOUND}")
	if (FFMPEG_FOUND)
		message (STATUS "        FFMPEG_ROOT_DIR=${FFMPEG_ROOT_DIR}")
	 	message (STATUS "        FFMPEG_INCLUDE_DIRS=${FFMPEG_INCLUDE_DIRS}")
	 	message (STATUS "        FFMPEG_LIBRARIES=${FFMPEG_LIBRARIES}")
	endif ()

	message (STATUS "    DIRECTX_FOUND: ${DIRECTX_FOUND}")
	if (DIRECTX_FOUND)
		message (STATUS "         DIRECTX_INCLUDE_DIRS=${DIRECTX_INCLUDE_DIRS}")
		message (STATUS "         DIRECTX_LIBRARIES=${DIRECTX_LIBRARIES}")
	endif ()

	message (STATUS "    Boost_FOUND: ${Boost_FOUND}")
	if (Boost_FOUND)
		message (STATUS "        Boost_INCLUDE_DIRS=${Boost_INCLUDE_DIRS}")
		message (STATUS "        Boost_LIBRARY_DIRS=${Boost_LIBRARY_DIRS}")
		message (STATUS "        Boost_LIBRARIES   =${Boost_LIBRARIES}")
	endif()

	message (STATUS "    OpenSSL_FOUND: ${OpenSSL_FOUND}")
	if (OpenSSL_FOUND)
		message (STATUS "        OpenSSL_LIBRARIES=${OpenSSL_LIBRARIES}")
	endif()

	message (STATUS "SCREENGRAB_INCLUDES  = ${SCREENGRAB_INCLUDES}")
	message (STATUS "SCREENGRAB_LIBS      = ${SCREENGRAB_LIBS}")
	message (STATUS "SCREENGRAB_OPEN_LIBS = ${SCREENGRAB_OPEN_LIBS}")
endfunction ()
