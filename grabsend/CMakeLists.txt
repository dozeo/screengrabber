file (GLOB_RECURSE src_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.cpp)
file (GLOB_RECURSE header_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.h)
if (MAC_OSX)
	file (GLOB_RECURSE objcpp_src_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.mm)
else()
	set (objcpp_src_files "")
endif()

WIN32_RELATIVE_FOLDER (${src_files} ${header_files} ${objcpp_src_files})

include (gitdescribe)

add_executable (grabsend ${src_files} ${header_files} ${objcpp_src_files})
target_link_libraries (grabsend ${SCREENGRAB_LIBS} grabber videosend ${FFMPEG_LIBRARIES})

install (TARGETS grabsend RUNTIME DESTINATION bin)
if (WIN32)
	install (FILES ${FFMPEG_DLLS} DESTINATION bin)
	install (FILES ${QT_DLLS} DESTINATION bin)
	install (FILES ${QT_DLLS_DEBUG} CONFIGURATIONS Debug DESTINATION bin)
endif()
if (LINUX)
	file (GLOB INSTALL_FILES ${FFMPEG_ROOT_DIR}/lib/*.so*)
	install (FILES ${INSTALL_FILES} DESTINATION lib)
	
	# Fix for Ubuntu Buildser; won't work on other platforms probably.
	set (INSTALL_OPENSSL_LIBRARIES /lib/i386-linux-gnu/libssl.so.1.0.0 /lib/i386-linux-gnu/libcrypto.so.1.0.0)
	install (FILES ${INSTALL_OPENSSL_LIBRARIES} DESTINATION lib OPTIONAL)
endif()

