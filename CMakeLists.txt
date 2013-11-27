PROJECT("screengrabber" C CXX)

cmake_minimum_required(VERSION 2.8)

OPTION(OPTION_DISABLE_TESTS "Disable Test Projects" Off)

message (STATUS "adding application: grabsend")
add_subdirectory (grabsend)

if (NOT OPTION_DISABLE_TESTS)
	message (STATUS "adding tests: libgrabbertest")
	add_subdirectory (tests/libgrabbertest)
endif ()