PROJECT("screengrabber" C CXX)
cmake_minimum_required(VERSION 2.8)

# Additional tools
set     (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
include (FindDirectX)
include (Win32RelativeFolder)


OPTION(OPTION_DISABLE_TESTS "Disable Test Projects" Off)


# Variables
if (WIN32)
    # WIN32 will be set automatically
elseif (APPLE)
    add_definitions (-DMAC_OSX -DUNIX)
    set (MAC_OSX TRUE)

    # set (CMAKE_XCODE_ATTRIBUTE_GCC_VERSION "com.apple.compilers.llvm.clang.1_0")
    # set (CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
    set (CMAKE_CXX_FLAGS_DEBUG "-g -Wall")
    set (CMAKE_CXX_FLAGS_RELEASE "-O3")
    set (CMAKE_C_FLAGS_DEBUG "-g -Wall")
    set (CMAKE_C_FLAGS_RELEASE "-O3")
 
else ()
    add_definitions (-DLINUX -DUNIX)
    set (LINUX TRUE)
	set (CMAKE_CXX_FLAGS_DEBUG      "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG -Wall")	
	set (CMAKE_C_FLAGS_DEBUG        "${CMAKE_C_FLAGS_DEBUG} -D_DEBUG -Wall")
	set (CMAKE_CXX_FLAGS_RELEASE    "${CMAKE_CXX_FLAGS_RELEASE} -Wall")
	set (CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} -Wall")
endif()

if (NOT DEFINED ENABLE_DEPLOY)
	# Strips some files from installation
	set (ENABLE_DEPLOY FALSE)
endif()

message (STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message (STATUS "ENABLE_DEPLOY:    ${ENABLE_DEPLOY}")


message (STATUS "Dependencies")
message (STATUS "    Qt4")
find_package (Qt4 COMPONENTS QtMain QtCore QtGui QtXml)
message (STATUS "    QT4_FOUND:      ${QT4_FOUND}")
message (STATUS "    QT_INCLUDE_DIR: ${QT_INCLUDE_DIR}")
if (QT4_FOUND)
	include (${QT_USE_FILE})
	message (STATUS "    QT_LIBRARIES: ${QT_LIBRARIES}")
	set (LIBS ${LIBS} ${QT_LIBRARIES})
	include_directories (${QT_INCLUDE_DIR})
endif()

if (LINUX)
	set (LIBS ${LIBS} X11 dl Xext Xrandr pthread)
    SET(CMAKE_INSTALL_RPATH "\$ORIGIN/../lib")
endif()

if (MAC_OSX)
	find_library (APP_SERVICES ApplicationServices REQUIRED )
	find_library (COCOA Cocoa REQUIRED)
	set (LIBS ${LIBS} ${COCOA} ${APP_SERVICES})
endif()

if (WIN32)
	set (LIBS ${LIBS} psapi) # Process information in libgrabber
endif()

find_package (FFMPEG REQUIRED)
message (STATUS "    FFMPEG_FOUND: ${FFMPEG_FOUND}")
if (FFMPEG_FOUND)
	message (STATUS "        FFMPEG_INCLUDE_DIRS=${FFMPEG_INCLUDE_DIRS}")
	message (STATUS "        FFMPEG_LIBRARIES=${FFMPEG_LIBRARIES}")
	include_directories (${FFMPEG_INCLUDE_DIRS})
	set (GPL_LIBS ${GPL_LIBS} ${FFMPEG_LIBRARIES})
endif ()

find_package (Boost COMPONENTS program_options REQUIRED)
if (Boost_FOUND)
		message (STATUS "        Boost_INCLUDE_DIRS=${Boost_INCLUDE_DIRS}")
		message (STATUS "        Boost_LIBRARY_DIRS=${Boost_LIBRARY_DIRS}")
		message (STATUS "        Boost_LIBRARIES   =${Boost_LIBRARIES}")
		include_directories (${Boost_INCLUDE_DIRS})
		link_directories (${Boost_LIBRARY_DIRS})
		if (NOT WIN32)
			# Boost for windows finds its libraries by itself.
			set (LIBS ${LIBS} ${Boost_LIBRARIES})
		endif()
endif()

if (WIN32)
	find_package (DIRECTX REQUIRED)
	if (DIRECTX_FOUND)
		include_directories (${DIRECTX_INCLUDE_DIRS})
		set (LIBS ${LIBS} ${DIRECTX_LIBRARIES})
	endif ()
endif ()

# PusherCon Libraries
# We need additional boost libraries
find_package (Boost COMPONENTS system random regex thread date_time REQUIRED)

message (STATUS "${Boost_LIBRARIES}")
if (NOT WIN32)
	# Windows finds it by itself
	set (LIBS ${LIBS} ${Boost_LIBRARIES})
endif()
	
# And additional OpenSSL
if (NOT WIN32)
	find_package (OpenSSL REQUIRED)
else ()
	# Use our own OpenSSL here
	set (OPENSSL_LIBRARIES ${CMAKE_SOURCE_DIR}/dependencies/lib/ssleay32.lib ${CMAKE_SOURCE_DIR}/dependencies/lib/libeay32.lib)
endif()
message (STATUS "OpenSSL Libraries: ${OPENSSL_LIBRARIES}")
set (LIBS ${LIBS} ${OPENSSL_LIBRARIES})


include (gitdescribe)

message (STATUS "Libraries")
message (STATUS "    libgrabber")
include_directories (libgrabber)
add_subdirectory (libgrabber)
set (LIBS ${LIBS} grabber)

include_directories (libvideosend)
add_subdirectory (libvideosend)
set (GPL_LIBS ${GPL_LIBS} videosend)


message (STATUS "Applications")
if (QT4_FOUND)
	message (STATUS "    grabsend")
	add_subdirectory (grabsend)
endif()


# include google test projects when option is set
if (NOT OPTION_DISABLE_TESTS)
	# We compile our own gtest
	set (GTEST_DIRECTORY ${CMAKE_SOURCE_DIR}/dependencies_source/gtest-1.6.0)
	add_subdirectory (cmake/external/gtest)
	include_directories(${GTEST_DIRECTORY}/include)
	set (GTEST_LIBRARY gtest)
	set (GTEST_LIBRARY_MAIN gtest_main)

	message (STATUS "Tests")
	message (STATUS "    libgrabbertest")
	add_subdirectory (tests/libgrabbertest)
endif ()


# include (Deploy)
