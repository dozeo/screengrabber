cmake_minimum_required(VERSION 2.8)

project(libgrabbertest CXX)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/Modules/")
include (Common)
include (FindAllExternals)

file (GLOB_RECURSE src_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.cpp)
file (GLOB_RECURSE header_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.h)

use_lib(libgrabber ..)

add_gtest()
add_boost()
add_directx()

include_directories (../..)
include_directories (${HDRS} ${CMAKE_CURRENT_BINARY_DIR})  # Place of UIC Files

add_executable(libgrabbertest ${src_files} ${header_files})
target_link_libraries(libgrabbertest libgrabber gtest_main ${LIBS} ${APPLE_FRAMEWORK_LIBS})

add_test(libgrabbertest libgrabbertest)

set_target_properties(libgrabbertest PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}
	RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})
