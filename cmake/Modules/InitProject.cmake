
# ! NOTE ! SCREENGRABBER_DIRECTORY needs to be set

# first set module path
set     (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${SCREENGRABBER_DIRECTORY}/cmake/Modules/")
include (FindDirectX)
include (Win32RelativeFolder)
find_package (FFMPEG REQUIRED)

set (SCREENGRAB_LIBS ${SCREENGRAB_LIBS} ${FFMPEG_LIBRARIES})
