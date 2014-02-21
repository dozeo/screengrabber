# Try to find FFMPEG (video codec library)
# 
# FFMPEG_FOUND
# FFMPEG_INCLUDE_DIRS
# FFMPEG_LIBRARIES
# FFMPEG_DEFINITIONS

if (NOT FFMPEG_ROOT_DIR)
	if (IS_DIRECTORY ${SCREENGRABBER_DIRECTORY}/dependencies/ffmpeg)
		set (FFMPEG_ROOT_DIR ${SCREENGRABBER_DIRECTORY}/dependencies/ffmpeg)
	endif ()
	if (IS_DIRECTORY ${SCREENGRABBER_DIRECTORY}/dependencies/include/libavutil)
		# directly compiled into dependencies folder
		set (FFMPEG_ROOT_DIR ${SCREENGRABBER_DIRECTORY}/dependencies)
	endif()
endif()


macro (FFMPEG_FIND varname libname headername)
	find_path (FFMPEG_${varname}_INCLUDE_DIR lib${libname}/${headername}
		PATHS
			${FFMPEG_ROOT_DIR}/include
			$ENV{FFMPEG_HOME}/include
			/usr/local/ffmpeg/include
			/usr/local/include
			/usr/include
		PATH_SUFFIXES ffmpeg
		NO_DEFAULT_PATH
	)

	find_library (FFMPEG_${varname}_LIBRARY
		NAMES ${libname} lib${libname}
		PATHS
			${FFMPEG_ROOT_DIR}/bin # windows searches here
			${FFMPEG_ROOT_DIR}/lib
			$ENV{FFMPEG_HOME}/lib
			/usr/local/ffmpeg/lib
			/usr/local/lib
			/usr/lib
		NO_DEFAULT_PATH
	)

	# message (STATUS "include dir: ${FFMPEG_${varname}_INCLUDE_DIR}")
	# message (STATUS "library: ${FFMPEG_${varname}_LIBRARY}")

	if (FFMPEG_${varname}_LIBRARY AND FFMPEG_${varname}_INCLUDE_DIR)
		set (FFMPEG_${varname}_FOUND TRUE)
	endif ()

endmacro ()

#message("ffmpeg using root ${FFMPEG_ROOT_DIR}")

FFMPEG_FIND (LIBAVFORMAT avformat avformat.h)
FFMPEG_FIND (LIBAVCODEC  avcodec  avcodec.h)
FFMPEG_FIND (LIBAVUTIL   avutil   avutil.h)
FFMPEG_FIND (LIBSWSCALE  swscale  swscale.h)

#message("ffmpeg include dir: ${FFMPEG_LIBAVFORMAT_INCLUDE_DIR}")

if (FFMPEG_LIBAVFORMAT_FOUND AND FFMPEG_LIBAVCODEC_FOUND AND FFMPEG_LIBAVUTIL_FOUND AND FFMPEG_LIBSWSCALE_FOUND)
	set (FFMPEG_INCLUDE_DIRS
		${FFMPEG_LIBAVFORMAT_INCLUDE_DIR}
		${FFMPEG_LIBAVCODEC_INCLUDE_DIR}
		${FFMPEG_LIBAVUTIL_INCLUDE_DIR}
		${FFMPEG_LIBSWSCALE_INCLUDE_DIR}
	)
	set (FFMPEG_LIBRARIES
		${FFMPEG_LIBAVFORMAT_LIBRARY}
		${FFMPEG_LIBAVCODEC_LIBRARY}
		${FFMPEG_LIBAVUTIL_LIBRARY}
		${FFMPEG_LIBSWSCALE_LIBRARY}
	)
	
	if (WIN32)
		set (FFMPEG_DLLS 
			${FFMPEG_ROOT_DIR}/bin/avformat-55.dll
			${FFMPEG_ROOT_DIR}/bin/avcodec-55.dll
			${FFMPEG_ROOT_DIR}/bin/avutil-52.dll
			${FFMPEG_ROOT_DIR}/bin/swscale-2.dll
			${FFMPEG_ROOT_DIR}/bin/librtmp-1.dll
			${FFMPEG_ROOT_DIR}/bin/libz-1.dll
			${FFMPEG_ROOT_DIR}/bin/libx264-140.dll
			${FFMPEG_ROOT_DIR}/bin/pthreadGC2.dll
			${FFMPEG_ROOT_DIR}/bin/libgcc_s_dw2-1.dll
		)
	endif()
	
	list (REMOVE_DUPLICATES FFMPEG_INCLUDE_DIRS)
	set (FFMPEG_FOUND TRUE)
else ()
	set (FFMPEG_FOUND FALSE)
endif ()


include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (FFMPEG DEFAULT_MSG FFMPEG_LIBRARIES FFMPEG_INCLUDE_DIRS)
mark_as_advanced (FFMPEG_INCLUDE_DIRS FFMPEG_LIBRARIES)


#
# A small helper macro to copy all necessary dll libraries to a specific folder
#
function (CopyFFMpegLibrariesToDirectory target destinationFolder)
	if (FFMPEG_FOUND AND FFMPEG_ROOT_DIR AND WIN32)
		foreach (dll ${FFMPEG_DLLS})
			ADD_CUSTOM_COMMAND(
				TARGET ${target}
				POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E copy ${dll} ${destinationFolder} VERBATIM
			)
		endforeach ()
	endif()
endfunction (CopyFFMpegLibrariesToDirectory target destinationFolder)

