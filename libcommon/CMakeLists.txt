cmake_minimum_required(VERSION 2.8)

project(libcommon CXX)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/Modules/")
include (Common)
include (FindAllExternals)

add_dzlib()
add_boost()

set(src_files
	"src/videoframe.cpp"
	"src/main_test.cpp"
	"src/videoframepool.cpp"
	"src/dzrect.cpp")

set(header_files
	"src/include/libcommon/videoframe.h"
	"src/include/libcommon/videoframepool.h"
	"src/include/libcommon/math_helpers.h"
	"src/include/libcommon/dzrect.h")

include_directories(${HDRS} "src/include")

add_library (libcommon ${src_files} ${header_files})
#add_executable (libcommon ${src_files} ${header_files})

target_link_libraries(libcommon ${LIBS})
