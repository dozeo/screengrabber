cmake_minimum_required(VERSION 2.8)

project(libgrabbertest CXX)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/Modules/")
include (Common)
include (FindAllExternals)

file (GLOB_RECURSE src_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.cpp)
file (GLOB_RECURSE header_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.h)

add_gtest()
add_boost()
add_dzlib()
add_directx()
add_libcommon()

include_directories (../..)
include_directories (${HDRS} ${CMAKE_CURRENT_BINARY_DIR})  # Place of UIC Files

if (NOT TARGET libgrabber)
	add_subdirectory(../../libgrabber ${CMAKE_CURRENT_BINARY_DIR}/libgrabber)
endif()

add_executable (libgrabbertest ${src_files} ${header_files})
target_link_libraries (libgrabbertest libgrabber ${LIBS} ${APPLE_FRAMEWORK_LIBS})

enable_testing()

add_test(libgrabbertest libgrabbertest)
