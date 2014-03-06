cmake_minimum_required(VERSION 2.8)

project(libcommon CXX)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/Modules/")
include (Common)
include (FindAllExternals)

add_dzlib()
add_boost()

set(src_files
	"src/videoframe.cpp"
	"src/videosubframe.cpp"
	"src/main_test.cpp"
	"src/videoframepool.cpp"
	"src/dzrect.cpp"
	"src/grabber_type.cpp")

set(hdr_folder "src/include")
set(header_files
	"${hdr_folder}/libcommon/videoframe.h"
	"${hdr_folder}/libcommon/videosubframe.h"
	"${hdr_folder}/libcommon/videoframepool.h"
	"${hdr_folder}/libcommon/math_helpers.h"
	"${hdr_folder}/libcommon/dzrect.h"
	"${hdr_folder}/libcommon/grabber_type.h"
	"${hdr_folder}/libcommon/grabber_options.h")

include_directories(${HDRS} "${hdr_folder}")

add_library (libcommon ${src_files} ${header_files})
#add_executable (libcommon ${src_files} ${header_files})

target_link_libraries(libcommon ${LIBS})
