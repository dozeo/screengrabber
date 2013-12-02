set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/Modules")

set (SCREENGRABBER_ROOT "${CMAKE_CURRENT_LIST_DIR}/../..")
set (SCREENGRABBER_DEPENDS "${SCREENGRABBER_ROOT}/dependencies")

set (GTEST_CMAKE_PATH "${SCREENGRABBER_ROOT}/cmake/external/gtest")
set (GTEST_DIRECTORY "${SCREENGRABBER_ROOT}/dependencies_source/gtest-1.6.0")
set (FFMPEG_ROOT_DIR "${SCREENGRABBER_DEPENDS}")
set (ZLIB_ROOT "${SCREENGRABBER_DEPENDS}")
set (ZLIB_DLL "${SCREENGRABBER_DEPENDS}/bin/libz-1.dll")

set (PLATFORMLIBS "")
set (DIRECTX_LIBRARIES "")
set (DIRECTX_INCLUDE_DIRS "")
set (LIBS "")

message(STATUS "Binary Directory ${CMAKE_BINARY_DIR}")

#=============================================================
# Set install to some nicer location instead of the Default
# on windows in particular it is really ugly
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install CACHE PATH "Default Installation Path" FORCE)
	message("-- Installing in ${CMAKE_INSTALL_PREFIX}")
endif()
#=============================================================


#=============================================================
# Some platform specific actions, defines and stuff
if (WIN32)
	if (MSVC)
		add_definitions (-D_CRT_SECURE_NO_WARNINGS)
	endif()

	set(LIBS ${LIBS} "psapi")
endif()


#=============================================================
# Macros for adding subprojects and external to your project (adds libs and include directories too)
macro(add_gtest)
	if (NOT TARGET gtest)
		add_subdirectory (${GTEST_CMAKE_PATH} ${CMAKE_BINARY_DIR}/gtest)
	endif()
	include_directories (${GTEST_DIRECTORY}/include)
	set(LIBS ${LIBS} gtest)
endmacro()

macro(add_libgrabber)
	if (NOT TARGET libgrabber)
		add_subdirectory (${SCREENGRABBER_ROOT}/libgrabber ${CMAKE_BINARY_DIR}/libgrabber)
	endif()
	include_directories (${SCREENGRABBER_ROOT})
	set(LIBS ${LIBS} libgrabber ${FFMPEG_LIBRARIES})
endmacro()

macro(add_libvideosend)
	if (NOT TARGET libvideosend)
		message(STATUS "From '${CMAKE_CURRENT_LIST_FILE}'")
		message(STATUS "Adding libvideosend from '${SCREENGRABBER_ROOT}/libvideosend' to '${CMAKE_BINARY_DIR}/libvideosend'")
		add_subdirectory (${SCREENGRABBER_ROOT}/libvideosend ${CMAKE_BINARY_DIR}/libvideosend)
	endif()
	include_directories (${SCREENGRABBER_ROOT})
	if (MSVC)
		include_directories (${SCREENGRABBER_ROOT}/libvideosend/src/win32include)
	endif()
	set(LIBS ${LIBS} libvideosend)
endmacro()

macro(add_grabsend)
	add_subdirectory(${SCREENGRABBER_ROOT}/grabsend ${CMAKE_BINARY_DIR}/grabsend)
	if (WIN32)
		if (CMAKE_BUILD_TYPE STREQUAL "Debug")
			set(GRABSEND_EXE "${CMAKE_BINARY_DIR}/grabsend/Debug/grabsend.exe")
		else()
			set(GRABSEND_EXE "${CMAKE_BINARY_DIR}/grabsend/Release/grabsend.exe")
		endif()
	endif()
endmacro()

macro(add_boost)
	set(Boost_USE_STATIC_LIBS ON)
	find_package (Boost 1.47 COMPONENTS program_options thread date_time REQUIRED)
	# ${Boost_INCLUDE_DIRS}
	# ${Boost_LIBRARIES}
	include_directories (${Boost_INCLUDE_DIRS})
	set(LIBS ${LIBS} ${Boost_LIBRARIES})
endmacro()

macro(add_qt)
	find_package (Qt4 COMPONENTS QtMain QtCore QtGui QtNetwork REQUIRED)
	include (${QT_USE_FILE})
	if (WIN32)
		set (QT_DLLS
			${QT_BINARY_DIR}/QtGui4.dll
			${QT_BINARY_DIR}/QtCore4.dll
			${QT_BINARY_DIR}/QtNetwork4.dll)
		set (QT_DLLS_DEBUG
			${QT_BINARY_DIR}/QtGuid4.dll
			${QT_BINARY_DIR}/QtCored4.dll
			${QT_BINARY_DIR}/QtNetworkd4.dll)
	endif()
	include_directories (${QT_INCLUDE_DIR})
	set(LIBS ${LIBS} ${QT_LIBRARIES})
	file(COPY
		${QT_DLLS} ${QT_DLLS_DEBUG}
		DESTINATION ${CMAKE_BINARY_DIR})
endmacro()

macro(add_directx)
	if (WIN32)
		find_package (DIRECTX REQUIRED)
		# ${DIRECTX_LIBRARIES}
		# ${DIRECTX_INCLUDE_DIRS}
		set (LIBS ${LIBS} "${DIRECTX_LIBRARIES}")
		include_directories (${DIRECTX_LIBRARIES})
	endif()
endmacro()

macro(add_ffmpeg)
	find_package (FFMPEG REQUIRED)
	if (WIN32)
		set (FFMPEG_DLLS
			${FFMPEG_ROOT_DIR}/bin/avformat-55.dll
			${FFMPEG_ROOT_DIR}/bin/avcodec-55.dll
			${FFMPEG_ROOT_DIR}/bin/avutil-52.dll
			${FFMPEG_ROOT_DIR}/bin/swscale-2.dll
			${FFMPEG_ROOT_DIR}/bin/librtmp-0.dll
			${FFMPEG_ROOT_DIR}/bin/libx264-140.dll
			${FFMPEG_ROOT_DIR}/bin/pthreadGC2.dll
			${FFMPEG_ROOT_DIR}/bin/libgcc_s_dw2-1.dll)
		file(COPY
			${FFMPEG_DLLS}
			DESTINATION "${CMAKE_BINARY_DIR}")
		file(COPY
			${ZLIB_DLL}
			DESTINATION "${CMAKE_BINARY_DIR}")
		# we expect the DLL to be named zlib1.dll so rename it
		file(RENAME
			"${CMAKE_BINARY_DIR}/libz-1.dll"
			"${CMAKE_BINARY_DIR}/zlib1.dll")
	endif()
	set (LIBS ${LIBS} ${FFMPEG_LIBRARIES})
	include_directories (${FFMPEG_INCLUDE_DIRS})

endmacro()