
# platform specific settings
set (CPACK_PACKAGE_VENDOR "dozeo") # update with correct info!
set (CPACK_PACKAGE_VERSION ${GIT_DESCRIBE}) # from gitdescribe.cmake
if (LINUX)
	SET(CPACK_SOURCE_GENERATOR "TGZ" "DEB")
	SET(CPACK_GENERATOR "TGZ")
    SET(CPACK_INCLUDE_TOPLEVEL_DIRECTORY 0)
endif()

if (WIN32 AND NOT UNIX)
	set (CPACK_GENERATOR "ZIP")
	set (CPACK_NSIS_MODIFY_PATH ON)
endif ()
if (APPLE)
	set (CPACK_SOURCE_GENERATOR "DragNDrop")
	set (CPACK_GENERATOR "DragNDrop") 
endif()

# actual packaging script
include (CPack)
if (NOT MSVC10)
	include (InstallRequiredSystemLibraries)
endif()

message (STATUS "Generators: ${CPACK_GENERATOR}")
