# Stores the output of gitdescribe into a file called "gitdescribe.h"
# Who's path is added to the include search path

find_package (Git REQUIRED)

# Get current version
execute_process(
         COMMAND ${GIT_EXECUTABLE} describe --always
         OUTPUT_VARIABLE GIT_DESCRIBE OUTPUT_STRIP_TRAILING_WHITESPACE
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)


message (STATUS "Git Describe Version: ${GIT_DESCRIBE}")

# Generate gitdescribe/gitdescribe.h file
file (MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/gitdescribe")
file (WRITE "${CMAKE_CURRENT_BINARY_DIR}/gitdescribe/gitdescribe.h" "#define GIT_DESCRIBE \"${GIT_DESCRIBE}\"\n")
include_directories (${CMAKE_CURRENT_BINARY_DIR}/gitdescribe)
