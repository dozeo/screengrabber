file (GLOB_RECURSE src_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} grabber/*.cpp)
file (GLOB_RECURSE header_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} grabber/*.h)

WIN32_RELATIVE_FOLDER (${src_files} ${header_files})

include_directories (${CMAKE_CURRENT_BINARY_DIR})  # Place of UIC Files
add_executable (libgrabbertest ${src_files} ${header_files})
target_link_libraries (libgrabbertest ${SCREENGRAB_LIBS} grabber ${GTEST_LIBRARY})
