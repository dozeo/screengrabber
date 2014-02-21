cmake_minimum_required(VERSION 2.8)

project(libgrabbertest CXX)

file (GLOB_RECURSE src_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.cpp)
file (GLOB_RECURSE header_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.h)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/Modules/")
include (Common)

add_subdirectory (../../cmake/external/gtest ${CMAKE_CURRENT_SOURCE_DIR}/build/gtest/)
#set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/Modules/")
#include(

include_directories (../../dependencies_source/gtest-1.7.0/include)
include_directories (../..)
include_directories (${CMAKE_CURRENT_BINARY_DIR})  # Place of UIC Files

if (NOT TARGET libgrabber)
	add_subdirectory(../../libgrabber ${CMAKE_CURRENT_SOURCE_DIR}/build/libgrabber/)
endif()

if (WIN32)
	set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/Modules/")
	find_package (DirectX REQUIRED)

	set (PLATFORMLIBS "psapi")
	set (PLATFORMLIBS ${PLATFORMLIBS} ${DIRECTX_LIBRARIES})
endif()

add_executable (libgrabbertest ${src_files} ${header_files})
target_link_libraries (libgrabbertest libgrabber gtest ${PLATFORMLIBS} ${APPLE_FRAMEWORK_LIBS})

enable_testing()

add_test(libgrabbertest libgrabbertest)