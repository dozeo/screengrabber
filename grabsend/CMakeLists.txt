cmake_minimum_required(VERSION 2.8)

project(grabsend)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 6)
set(VERSION_PATCH 0)
set(VERSION_STRING "v${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/Modules/")
include (Common)
include (FindAllExternals)

use_lib(libgrabber ..)
use_lib(libvideosend ..)
add_ffmpeg()
add_directx()

list(APPEND src
	"src/GrabSendOptions.cpp"
	"src/Tools.cpp"
	"src/main.cpp"
	)

list(APPEND hdr
	"src/GrabSendOptions.h"
	"src/GrabberOptions.h"
	"src/Tools.h"
	"src/VideoSenderOptions.h"
	"${CMAKE_BINARY_DIR}/grabsend_version.h"
	"src/grabsend_version.h.in"
	)

if(APPLE)
	list(APPEND src
		"src/OSXTools.mm"
		)
endif()

configure_file("src/grabsend_version.h.in"
	"${CMAKE_BINARY_DIR}/grabsend_version.h")

execute_process(COMMAND git describe --long --tags
	OUTPUT_VARIABLE GRABSEND_GIT_DESCRIBE OUTPUT_STRIP_TRAILING_WHITESPACE
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

configure_file("src/grabsend_gitdescribe.h.in"
	"${CMAKE_BINARY_DIR}/grabsend_gitdescribe.h")

list(APPEND hdr
	"src/grabsend_gitdescribe.h.in"
	"${CMAKE_BINARY_DIR}/grabsend_gitdescribe.h")

list(APPEND hdr_dir
	"${CMAKE_BINARY_DIR}"
	${HDRS}
	)

message (STATUS "Version for grabsend: ${VERSION_STRING}")

add_executable(grabsend ${src} ${hdr})
target_link_libraries(grabsend libgrabber libvideosend ${LIBS})
set_target_properties(grabsend PROPERTIES
	INCLUDE_DIRECTORIES "${hdr_dir}"
	FOLDER /app)

if (APPLE)
	install(TARGETS grabsend
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
