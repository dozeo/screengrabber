cmake_minimum_required(VERSION 2.8)

project(libgrabber CXX)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/Modules/")
include (Common)
include (FindAllExternals)

use_lib(libcommon ..)

list(APPEND hdr_dir "${CMAKE_CURRENT_SOURCE_DIR}/..")

set(src "")
list(APPEND src
	"src/IDesktopTools.cpp"
	"src/IDesktopTools.h"
	"src/grabber_null.cpp"
	"src/grabber_null.h"
	)

if(WIN32)
	list(APPEND src
		"src/window_info_win32.cpp"
		"src/bitbltgrabber_win32.cpp"
		"src/bitbltgrabber_win32.h"
		"src/DesktopTools_Win32.cpp"
		"src/DesktopTools_Win32.h"
		"src/WindowGrabber_Win32.cpp"
		"src/WindowGrabber_Win32.h"
		"src/grabber_win32.cpp"
		"src/grabber_win32.h"
		"src/screen_enumerator_win32.cpp"
		"src/screen_enumerator_win32.h"
		"src/process_info_win32.cpp"
		)
endif()

if(APPLE)
	list(APPEND src
		"src/osx/GrabberOSX.cpp"
		"src/osx/GrabberOSX.h"
		"src/osx/GrabberOSX_mouse.mm"
		"src/process_info_osx.cpp"
		"src/window_info_osx.cpp"
		"src/DesktopTools_OSX.h"
		"src/DesktopTools_OSX.cpp"
		)
endif()

#file (GLOB_RECURSE src_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.cpp)
#file (GLOB_RECURSE header_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.h)

if (MAC_OSX)
	if (MAC_OSX_VERSION GREATER 106)
		add_definitions ("-DUSE_COCOA_GRAB")
	endif ()
	file (GLOB_RECURSE objcpp_src_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.mm)
else()
	set (objcpp_src_files "")
endif()

add_library(libgrabber ${src} ${objcpp_src_files})
target_link_libraries(libgrabber dzlib libcommon)
set_target_properties(libgrabber PROPERTIES
	INCLUDE_DIRECTORIES "${hdr_dir}"
	INTERFACE_INCLUDE_DIRECTORIES "${hdr_dir}"
	)
