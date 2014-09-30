PROJECT("screengrabber" C CXX)

cmake_minimum_required(VERSION 2.8)

OPTION(OPTION_BUILD_TESTS "Build Test Projects" ON)

set(SLOG_BUILD_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(vendor/slog)

message(STATUS "adding application: grabsend")
add_subdirectory(grabsend)

if (APPLE)
    add_definitions(-DMAC_OSX -DUNIX)
    set (MAC_OSX TRUE)
endif()

if (OPTION_BUILD_TESTS)
	enable_testing()
	message (STATUS "adding tests...")
	add_subdirectory(tests/libgrabbertest)
endif ()
