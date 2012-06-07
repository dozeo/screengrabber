PROJECT("screengrabber" C CXX)
cmake_minimum_required(VERSION 2.8)


OPTION(OPTION_DISABLE_TESTS "Disable Test Projects" Off)

# Additional tools
set (SCREENGRABBER_DIRECTORY ${CMAKE_SOURCE_DIR})
set     (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${SCREENGRABBER_DIRECTORY}/cmake/Modules/")
include (Win32RelativeFolder)
include (InitProject)
include (gitdescribe)

message (STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

#
# List all necessary dependencies!
#
LIST_SCREENGRAB_DEPENDENCIES()

include_directories (${SCREENGRAB_INCLUDES})
if (Boost_FOUND)
	link_directories (${Boost_LIBRARY_DIRS})
endif ()


message (STATUS "Libraries")
message (STATUS "    libgrabber")
include_directories (libgrabber)
add_subdirectory (libgrabber)
set (SCREENGRAB_LIBS ${SCREENGRAB_LIBS} grabber)

include_directories (libvideosend)
add_subdirectory (libvideosend)
set (SCREENGRAB_OPEN_LIBS ${SCREENGRAB_OPEN_LIBS} videosend)


message (STATUS "Applications")
if (QT4_FOUND)
	message (STATUS "    grabsend")
	add_subdirectory (grabsend)
endif()


# include google test projects when option is set
if (NOT OPTION_DISABLE_TESTS)
	# We compile our own gtest
	set (GTEST_DIRECTORY ${CMAKE_SOURCE_DIR}/dependencies_source/gtest-1.6.0)
	add_subdirectory (cmake/external/gtest)
	include_directories(${GTEST_DIRECTORY}/include)
	set (GTEST_LIBRARY gtest)
	set (GTEST_LIBRARY_MAIN gtest_main)

	message (STATUS "Tests")
	message (STATUS "    libgrabbertest")
	add_subdirectory (tests/libgrabbertest)
endif ()


# include (Deploy)
