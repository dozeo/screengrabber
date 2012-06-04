#
# DXSDK_DIR - environment variable must be set in order to find DirectX
# 

if (WIN32)

	find_path (DIRECTX_INCLUDE_DIR d3d9.h dxguid.h
		PATHS
		${DXSDK_HOME}/Include
		$ENV{DXSDK_DIR}/Include
		$ENV{DXSDK_HOME}/Include)

	find_library (DIRECTX_D3D_LIBRARY d3d9
		PATHS
		${DXSDK_HOME}/Lib/x86
		$ENV{DXSDK_DIR}/Lib/x86
		$ENV{DXSDK_HOME}/Lib/x86)

	find_library (DIRECTX_DXGUID_LIBRARY dxguid
		PATHS
		${DXSDK_HOME}/Lib/x86
		$ENV{DXSDK_DIR}/Lib/x86
		$ENV{DXSDK_HOME}/Lib/x86)

	if (DIRECTX_INCLUDE_DIR AND DIRECTX_D3D_LIBRARY AND DIRECTX_DXGUID_LIBRARY)
		set (DIRECTX_INCLUDE_DIRS ${DIRECTX_INCLUDE_DIR})
		set (DIRECTX_LIBRARIES ${DIRECTX_D3D_LIBRARY} ${DIRECTX_DXGUID_LIBRARY})
		set (DIRECTX_FOUND TRUE)
	else ()
		set (DIRECTX_FOUND FALSE)
	endif ()

endif ()
