
# ! NOTE ! SCREENGRABBER_DIRECTORY needs to be set

# SCREENGRAB_LIBS      - NON-GPL libraries used by screengrabber
# SCREENGRAB_OPEN_LIBS - all open source libraries used by screengrabber
# SCREENGRAB_INCLUDES  - all include directories

# Variables
if (WIN32)
    # WIN32 will be set automatically
elseif (APPLE)
    add_definitions (-DMAC_OSX -DUNIX)
    set (MAC_OSX TRUE)

    set (CMAKE_CXX_FLAGS_DEBUG "-g -Wall")
    set (CMAKE_CXX_FLAGS_RELEASE "-O3")
    set (CMAKE_C_FLAGS_DEBUG "-g -Wall")
    set (CMAKE_C_FLAGS_RELEASE "-O3")

    include (MacOSXVersion)

else ()
    add_definitions (-DLINUX -DUNIX)
    set (LINUX TRUE)
	set (CMAKE_CXX_FLAGS_DEBUG      "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG -Wall")	
	set (CMAKE_C_FLAGS_DEBUG        "${CMAKE_C_FLAGS_DEBUG} -D_DEBUG -Wall")
	set (CMAKE_CXX_FLAGS_RELEASE    "${CMAKE_CXX_FLAGS_RELEASE} -Wall")
	set (CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} -Wall")
endif()


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
find_package (Qt4 COMPONENTS QtMain QtCore QtGui QtNetwork REQUIRED)
if (QT4_FOUND)
	include (${QT_USE_FILE})
	set (SCREENGRAB_LIBS ${SCREENGRAB_LIBS} ${QT_LIBRARIES})
	set (SCREENGRAB_INCLUDES ${SCREENGRAB_INCLUDES} ${QT_INCLUDE_DIR})
	if (WIN32)
		set (QT_DLLS
			${QT_BINARY_DIR}/QtGui4.dll
			${QT_BINARY_DIR}/QtCore4.dll
			${QT_BINARY_DIR}/QtNetwork4.dll
		)
		set (QT_DLLS_DEBUG
			${QT_BINARY_DIR}/QtGuid4.dll
			${QT_BINARY_DIR}/QtCored4.dll
			${QT_BINARY_DIR}/QtNetworkd4.dll
		)
	endif()	
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
find_package (Boost COMPONENTS program_options thread REQUIRED)
if (Boost_FOUND)
	set (SCREENGRAB_INCLUDES ${SCREENGRAB_INCLUDES} ${Boost_INCLUDE_DIRS})
	if (NOT WIN32)
		# Boost for windows finds its libraries by itself.
		set (SCREENGRAB_LIBS ${SCREENGRAB_LIBS} ${Boost_LIBRARIES})
	endif()
	if (Boost_FOUND)
		link_directories (${Boost_LIBRARY_DIRS})
	endif ()
endif()

#
# lists all dependencies
#
function (LIST_SCREENGRAB_DEPENDENCIES)
	message (STATUS "Screengrabber Dependencies")
	message (STATUS "    QT4_FOUND: ${QT4_FOUND}")
	if (QT4_FOUND)
		message (STATUS "        QT_LIBRARIES: ${QT_LIBRARIES}")
		message (STATUS "        QT_INCLUDE_DIR: ${QT_INCLUDE_DIR}")
		if (WIN32)
			message (STATUS "        QT_DLLS: ${QT_DLLS}")
			message (STATUS "        QT_DLLS_DEBUG: ${QT_DLLS_DEBUG}")
		endif()
	endif()

	message (STATUS "    FFMPEG_FOUND: ${FFMPEG_FOUND}")
	if (FFMPEG_FOUND)
		message (STATUS "        FFMPEG_ROOT_DIR=${FFMPEG_ROOT_DIR}")
	 	message (STATUS "        FFMPEG_INCLUDE_DIRS=${FFMPEG_INCLUDE_DIRS}")
	 	message (STATUS "        FFMPEG_LIBRARIES=${FFMPEG_LIBRARIES}")
		message (STATUS "        FFMPEG_DLLS=${FFMPEG_DLLS}")
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

	message (STATUS "SCREENGRAB_INCLUDES  = ${SCREENGRAB_INCLUDES}")
	message (STATUS "SCREENGRAB_LIBS      = ${SCREENGRAB_LIBS}")
	message (STATUS "SCREENGRAB_OPEN_LIBS = ${SCREENGRAB_OPEN_LIBS}")
endfunction ()
