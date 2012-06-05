file (GLOB_RECURSE src_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} grabber/*.cpp)
file (GLOB_RECURSE header_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} grabber/*.h)
if (MAC_OSX)
	file (GLOB_RECURSE objcpp_src_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} grabber/*.mm)
else()
	set (objcpp_src_files "")
endif()

WIN32_RELATIVE_FOLDER (${src_files} ${header_files} ${objcpp_src_files})
add_library (grabber ${src_files} ${header_files} ${objcpp_src_files})
if (LINUX)
    # otherwise we get strange linking errors
    target_link_libraries (grabber ${SCREENGRAB_LIBS})
endif()

