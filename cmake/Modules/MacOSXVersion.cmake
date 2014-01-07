#
# MAC_OSX_VERSION defines the Mac OSX Version, e.g.
#   MAC_OSX_VERSION = 106
# specifies OSX Snow Leopard (10.6)
#
if (APPLE)
	# NOTE MacOSX provides different system versions than CMake is parsing.
	#      The following table lists the most recent OSX versions
	#     9.x.x = Mac OSX Leopard (10.5)
	#    10.x.x = Mac OSX Snow Leopard (10.6)
	#    11.x.x = Mac OSX Lion (10.7)
	#    12.x.x = Mac OSX Mountain Lion (10.8)
	if (${CMAKE_SYSTEM_VERSION} LESS 10)
		set (MAC_OSX_VERSION 105)
	elseif (${CMAKE_SYSTEM_VERSION} GREATER 10 AND ${CMAKE_SYSTEM_VERSION} LESS 11)
		set (MAC_OSX_VERSION 106)
	elseif (${CMAKE_SYSTEM_VERSION} GREATER 11 AND ${CMAKE_SYSTEM_VERSION} LESS 12)
		set (MAC_OSX_VERSION 107)
	elseif (${CMAKE_SYSTEM_VERSION} GREATER 12 OR ${CMAKE_SYSTEM_VERSION} EQUAL 12)
		set (MAC_OSX_VERSION 108)
	else ()
		set (MAC_OSX_VERSION 0)
	endif ()
endif ()

message(STATUS "OSX Version ${MAC_OSX_VERSION}")
