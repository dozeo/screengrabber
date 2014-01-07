cmake_minimum_required(VERSION 2.8)

project(libvideosend CXX)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/Modules")
include (FindAllExternals)

if (MINGW)
	message ("-- Enabling c++11")
	add_definitions (-std=c++11)
endif()

file (GLOB_RECURSE src_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.cpp)
file (GLOB_RECURSE header_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.h)

add_qt()
add_ffmpeg()

include_directories (${SCREENGRABBER_ROOT})

if (MSVC)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR}/src/win32include)
endif()

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/src)
add_library (libvideosend ${src_files} ${header_files})

# if (LINUX)
#     # otherwise we get strange linking errors
#     target_link_libraries (libvideosend ${SCREENGRAB_LIBS} ${SCREENGRAB_OPEN_LIBS})
# endif()
