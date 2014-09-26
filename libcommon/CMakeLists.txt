cmake_minimum_required(VERSION 2.8)

project(libcommon)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/Modules/")
include (Common)
include (FindAllExternals)

use_lib(dzlib ../vendor)
add_boost()

set(src_files
	"src/videoframe.cpp"
	"src/videosubframe.cpp"
	"src/main_test.cpp"
	"src/videoframepool.cpp"
	"src/dzrect.cpp"
	"src/videotypes.cpp"
	"src/sender_options.cpp")

list(APPEND hdr_folder
	"${CMAKE_CURRENT_SOURCE_DIR}/src/include"
	"${Boost_INCLUDE_DIRS}"
	)

set(header_files
	"src/include/libcommon/videoframe.h"
	"src/include/libcommon/videosubframe.h"
	"src/include/libcommon/videoframepool.h"
	"src/include/libcommon/math_helpers.h"
	"src/include/libcommon/dzrect.h"
	"src/include/libcommon/grabber_options.h"
	"src/include/libcommon/videotypes.h"
	"src/include/libcommon/sender_options.h")

if (NOT TARGET libcommon)
	add_library (libcommon ${src_files} ${header_files})
	set_target_properties(libcommon PROPERTIES
		PUBLIC_HEADER "${header_files}"
		INCLUDE_DIRECTORIES "${hdr_folder}"
		INTERFACE_INCLUDE_DIRECTORIES "${hdr_folder}"
		PREFIX ""
		)

	target_link_libraries(libcommon dzlib ${Boost_LIBRARIES})
endif()