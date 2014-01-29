if (APPLE)
    add_definitions (-DMAC_OSX -DUNIX)
    set (MAC_OSX TRUE)

	add_definitions("-std=c++11")
    set (CMAKE_CXX_FLAGS_DEBUG "-g -Wall")
    set (CMAKE_CXX_FLAGS_RELEASE "-O3")
    set (CMAKE_C_FLAGS_DEBUG "-g -Wall")
    set (CMAKE_C_FLAGS_RELEASE "-O3")
    list (APPEND CMAKE_C_FLAGS "-std=c99")
    # the following flag does not compile on OSX 10.6 build
#    list (APPEND CMAKE_CXX_FLAGS "-std=c++0x")

	include (MacOSXVersion)

	set(APPLE_FRAMEWORK_LIBS "")
	find_library(COCOA_LIBRARY Cocoa REQUIRED)
	find_library(FOUNDATION_LIBRARY Foundation REQUIRED)
	find_library(APP_SERVICES ApplicationServices REQUIRED)
	
	list(APPEND APPLE_FRAMEWORK_LIBS ${COCOA_LIBRARY})
	list(APPEND APPLE_FRAMEWORK_LIBS ${FOUNDATION_LIBRARY})
	list(APPEND APPLE_FRAMEWORK_LIBS ${APP_SERVICES})	

#	add_definitions("--stdlib=libc++")
elseif (LINUX)
    add_definitions (-DLINUX -DUNIX)
    set (LINUX TRUE)
    set (CMAKE_CXX_FLAGS_DEBUG      "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG -Wall")	
    set (CMAKE_C_FLAGS_DEBUG        "${CMAKE_C_FLAGS_DEBUG} -D_DEBUG -Wall")
    set (CMAKE_CXX_FLAGS_RELEASE    "${CMAKE_CXX_FLAGS_RELEASE} -Wall")
    set (CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} -Wall")
endif ()

if (MAC_OSX)
        if (MAC_OSX_VERSION GREATER 106)
                add_definitions ("-DUSE_COCOA_GRAB")
        endif ()
endif()

message (STATUS "C Compiler   ${CMAKE_C_COMPILER}")
message (STATUS "C++ Compiler ${CMAKE_CXX_COMPILER}")
