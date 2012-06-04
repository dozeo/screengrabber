# used to declare a library project
# 
MACRO(PROJECT_LIBRARY name target)
	MESSAGE(STATUS "Found library project \"${name}\" ")
	SET(${target} ${name} CACHE STRING "library")
	FIND_PROJECT(name target)
ENDMACRO(PROJECT_LIBRARY)


# used to declare a dll library project
#
MACRO(PROJECT_DLL name target)
	MESSAGE(STATUS "Found dll project \"${name}\" ")
	SET(${target} ${name} CACHE STRING "dll project")
	FIND_PROJECT(name target)
	IF (WIN32)
		ADD_DEFINITIONS(-DDLL_EXPORT)
	ENDIF (WIN32)
ENDMACRO(PROJECT_DLL)


# used to declare an executable project
#
MACRO(PROJECT_APPLICATION name target)
	MESSAGE(STATUS "Found application project \"${name}\" ")
	SET(${target} ${name} CACHE STRING "application")
	FIND_PROJECT(name target)
ENDMACRO(PROJECT_APPLICATION)


# sets project default settings
#
MACRO(FIND_PROJECT name target)
	INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
	FindCodeFiles(GLOB_RECURSE internalFiles "")
	SetFilters("${internalFiles}")
	ProcessQtFiles(GLOB_RECURSE qtGeneratedFiles)
	SetWarningLevel(${target})
ENDMACRO(FIND_PROJECT)


# search directories recursively
#
MACRO(SearchDirectories)
	FILE(GLOB CMakeFiles FOLLOW_SYMLINKS "${CMAKE_CURRENT_SOURCE_DIR}/*/CMakeLists.txt" NO_DEFAULT_PATH)

	FOREACH(CMakeFile ${CMakeFiles})
		STRING(REGEX REPLACE "/CMakeLists\\.txt$" "" ProjectPath ${CMakeFile})
		IF (IS_DIRECTORY ${ProjectPath})
			ADD_SUBDIRECTORY(${ProjectPath})
		ENDIF(IS_DIRECTORY ${ProjectPath})
	ENDFOREACH(CMakeFile ${CMakeFiles})
ENDMACRO(SearchDirectories)


# creates filer (folders etc.) for all found code files
#
MACRO(SetFilters files)
	STRING(LENGTH ${CMAKE_CURRENT_SOURCE_DIR} srcDirLength)
	FOREACH(file ${files})
		# truncate the source dir from the filename to have a relative name
		STRING(LENGTH ${file} fileLength)
		MATH(EXPR length ${fileLength}-${srcDirLength})
		STRING(SUBSTRING ${file} ${srcDirLength} ${length} truncatedFile)
		STRING(REGEX MATCH "^.*/" dirName "${truncatedFile}")
		STRING(REPLACE "/" "\\" filterName "${dirName}")
		SOURCE_GROUP(${filterName} FILES ${file})
	ENDFOREACH(file ${files})
ENDMACRO(SetFilters files)


#
# a useful macro to compile and process ui / moc files
#
MACRO (ProcessQtFiles traversalMode generatedFiles)
	IF (QT_FOUND)
		SET(uiFiles "")
		SET(qrcFiles "")
		SET(qtFiles "")
		SET(hFiles "")

		SET(mocGeneratedByHeader "")
		SET(uicGenerated "")
		SET(mocGeneratedByRessources "")

		FILE(${traversalMode}  uiFiles FOLLOW_SYMLINKS *.ui)
		FILE(${traversalMode} qrcFiles FOLLOW_SYMLINKS *.qrc)
		FILE(${traversalMode}  qtFiles FOLLOW_SYMLINKS *.qt)

		SET (hFiles "")
		FOREACH (fileName ${uiFiles} ${qtFiles})
			STRING (LENGTH ${fileName} absLength)
			MATH (EXPR newLengh "${absLength}-2")
			STRING (SUBSTRING ${fileName} 0 ${newLengh} truncatedFileName)
			LIST (APPEND hFiles "${truncatedFileName}h")
		ENDFOREACH (fileName ${uiFiles} ${qtFiles})

		QT4_WRAP_CPP (mocGeneratedByHeader ${hFiles})
		QT4_WRAP_UI (uicGenerated ${uiFiles})
		QT4_ADD_RESOURCES (mocGeneratedByRessources ${qrcFiles})
		SOURCE_GROUP (
			"qt generated files" FILES
			${uicGenerated}
			${mocGeneratedByHeader}
			${mocGeneratedByRessources})
		SET (
			${generatedFiles}
			${uicGenerated}
			${mocGeneratedByHeader}
			${mocGeneratedByRessources})
	ENDIF (QT_FOUND)
ENDMACRO(ProcessQtFiles traversalMode generatedFiles)


# find all matching code files and add them
#
MACRO(FindCodeFiles traversalMode internalFiles relativePath)
	FILE(${traversalMode} ${internalFiles}
		FOLLOW_SYMLINKS
		"${relativePath}*.c"
		"${relativePath}*.cc"
		"${relativePath}*.cpp"
		"${relativePath}*.h"
		"${relativePath}*.hh"
		"${relativePath}*.hpp"
		"${relativePath}*.qrc"
		"${relativePath}*.rc"
		"${relativePath}*.tli"
		"${relativePath}*.txt"
		"${relativePath}*.ui"
	)
ENDMACRO(FindCodeFiles traversalMode internalFiles relativePath)


# sets the warning level of target
#
MACRO(SetWarningLevel target)
	IF (MSVC)
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4100 /wd4201 /wd4505")
	ENDIF (MSVC)
ENDMACRO(SetWarningLevel)


# sets source folder
#
MACRO(SetSourceFolder target sourceFolder)
	IF (NOT target)
		SET_PROPERTY(TARGET ${target} PROPERTY FOLDER ${sourceFolder})
	ELSE (NOT target)
		MESSAGE(FATAL_ERROR "Target ${target} not defined!")
	ENDIF (NOT target)
ENDMACRO(SetSourceFolder)
