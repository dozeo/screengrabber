cmake_minimum_required(VERSION 2.8)

project(libgrabber CXX)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/Modules/")
include (Common)
include (FindAllExternals)

add_dzlib()

file (GLOB_RECURSE src_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.cpp)
file (GLOB_RECURSE header_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.h)

if (MAC_OSX)
	if (MAC_OSX_VERSION GREATER 106)
		add_definitions ("-DUSE_COCOA_GRAB")
	endif ()
	file (GLOB_RECURSE objcpp_src_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.mm)
else()
	set (objcpp_src_files "")
endif()

include_directories (${HDRS})
add_library (libgrabber ${src_files} ${header_files} ${objcpp_src_files})
target_link_libraries(libgrabber ${LIBS})

# if (LINUX)
# 	# otherwise we get strange linking errors
# 	target_link_libraries (grabber ${SCREENGRAB_LIBS})
# endif()
