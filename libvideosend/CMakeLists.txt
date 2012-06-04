file (GLOB_RECURSE src_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} videosend/*.cpp)
file (GLOB_RECURSE header_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} videosend/*.h)

if (WIN32)
    include_directories (${CMAKE_CURRENT_SOURCE_DIR}/videosend/win32include)
endif()

WIN32_RELATIVE_FOLDER (${src_files} ${header_files})
add_library (videosend ${src_files} ${header_files})

if (LINUX)
    # otherwise we get strange linking errors
    target_link_libraries (videosend ${LIBS} ${GPL_LIBS})
endif()
