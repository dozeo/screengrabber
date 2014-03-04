cmake_minimum_required(VERSION 2.8)

project(grabsend CXX)

file (GLOB_RECURSE src_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.cpp)
file (GLOB_RECURSE header_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.h)

if (APPLE)
	file (GLOB_RECURSE objcpp_src_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.mm)
else()
	set (objcpp_src_files "")
endif()

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/Modules/")
include (Common)
include (FindAllExternals)
include (gitdescribe)

add_libgrabber()
add_libvideosend()
add_boost()
add_ffmpeg()
add_qt()
add_directx()
add_dzlib()
add_libcommon()

if (APPLE)
	file (GLOB_RECURSE ${objcpp_src_files} RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.mm)
	set (src_files ${src_files} ${additional})
endif()

include_directories(${HDRS})
add_executable (grabsend ${src_files} ${header_files} ${objcpp_src_files})
target_link_libraries (grabsend ${LIBS})
set_target_properties (grabsend PROPERTIES
	FOLDER /app)

#set_target_properties (grabsend PROPERTIES
#	RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}
#	RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})

if (APPLE)
	install (TARGETS grabsend
		BUNDLE DESTINATION . COMPONENT Runtime
		RUNTIME DESTINATION . COMPONENT Runtime)
endif()

if (WIN32)
	set_target_properties (grabsend PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}
		RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})
	install (TARGETS grabsend
		BUNDLE DESTINATION . COMPONENT Runtime
		RUNTIME DESTINATION . COMPONENT Runtime)
	install (FILES ${FFMPEG_DLLS}
		DESTINATION . COMPONENT Runtime)
	install (FILES ${QT_DLLS}
		DESTINATION . COMPONENT Runtime)
	install (FILES ${CMAKE_BINARY_DIR}/zlib1.dll
		DESTINATION . COMPONENT Runtime)
endif()

if (LINUX)
	file (GLOB INSTALL_FILES ${FFMPEG_ROOT_DIR}/lib/*.so*)
	install (
		FILES ${INSTALL_FILES}
		DESTINATION lib COMPONENT Runtime)

	# Fix for Ubuntu Buildser; won't work on other platforms probably.
	set (INSTALL_OPENSSL_LIBRARIES /lib/i386-linux-gnu/libssl.so.1.0.0 /lib/i386-linux-gnu/libcrypto.so.1.0.0)
	install (
		FILES ${INSTALL_OPENSSL_LIBRARIES}
		DESTINATION lib OPTIONAL)
endif()
