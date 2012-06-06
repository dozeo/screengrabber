file (GLOB_RECURSE src_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.cpp)
file (GLOB_RECURSE header_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.h)
if (MAC_OSX)
	file (GLOB_RECURSE objcpp_src_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.mm)
else()
	set (objcpp_src_files "")
endif()

WIN32_RELATIVE_FOLDER (${src_files} ${header_files})

add_executable (grabsend ${src_files} ${header_files})
target_link_libraries (grabsend ${SCREENGRAB_LIBS} grabber videosend ${FFMPEG_LIBRARIES})
if (NOT APPLE OR NOT ENABLE_DEPLOY) # gets already integrated into bundle
	install (TARGETS grabsend RUNTIME DESTINATION bin)
endif()
