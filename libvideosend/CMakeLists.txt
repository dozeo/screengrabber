cmake_minimum_required(VERSION 2.8)

project(libvideosend)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/Modules")
include(FindAllExternals)

if (MINGW)
	message ("-- Enabling c++11")
	add_definitions (-std=c++11)
endif()

use_lib(libcommon ..)

add_qt()
add_ffmpeg()

file (GLOB_RECURSE src_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.cpp)
file (GLOB_RECURSE header_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.h)

if (MSVC)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR}/src/win32include)
endif()

include_directories(${SCREENGRABBER_ROOT} ${CMAKE_CURRENT_SOURCE_DIR}/src ${HDRS})
add_library(libvideosend ${src_files} ${header_files})
target_link_libraries(libvideosend dzlib libcommon ${LIBS})
set_target_properties(libvideosend PROPERTIES
	PREFIX ""
	)
