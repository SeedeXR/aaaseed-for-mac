

###################################################################################################
# Limit Global configurations to Debug and Release -> must go before the project command
###################################################################################################
set( CMAKE_CONFIGURATION_TYPES "Debug;Release;MetalTuned;WatchDogDebug;WatchDogMetalTuned" CACHE STRING "Configs" FORCE )
if(DEFINED CMAKE_BUILD_TYPE AND CMAKE_VERSION VERSION_GREATER "2.8")
  set_property( CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${CMAKE_CONFIGURATION_TYPES} )
endif()


##############################################################################
# AAASeed Project
##############################################################################
PROJECT(AAASeedProject)


##############################################################################
# CMake required version
##############################################################################
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

set( BASE_SOURCE_PATH "${AAASeedProject_SOURCE_DIR}" )

list(APPEND CMAKE_MODULE_PATH ${BASE_SOURCE_PATH}/CMake)
list(APPEND CMAKE_PREFIX_PATH ${SystemDrive}:/cygwin/bin)


###################################################################################################
# Debug Link Flags
###################################################################################################
if( WIN32 )
	set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /SAFESEH:NO /NODEFAULTLIB:LIBCMTD.lib /NODEFAULTLIB:msvcrt.lib /NODEFAULTLIB:atlsd.lib" )
	list( REMOVE_DUPLICATES CMAKE_EXE_LINKER_FLAGS_DEBUG )
	set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} /SAFESEH:NO /NODEFAULTLIB:LIBCMTD.lib /NODEFAULTLIB:msvcrt.lib /NODEFAULTLIB:atlsd.lib" )
	list( REMOVE_DUPLICATES CMAKE_SHARED_LINKER_FLAGS_DEBUG )
	set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS_DEBUG} /SAFESEH:NO /NODEFAULTLIB:LIBCMTD.lib /NODEFAULTLIB:msvcrt.lib /NODEFAULTLIB:atlsd.lib" )
	list( REMOVE_DUPLICATES CMAKE_MODULE_LINKER_FLAGS_DEBUG )
	
	set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /DDEBUG /D_DEBUG /ZI" )
	list( REMOVE_DUPLICATES CMAKE_CXX_FLAGS_DEBUG )
	set( CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /DDEBUG /D_DEBUG /ZI" )
	list( REMOVE_DUPLICATES CMAKE_C_FLAGS_DEBUG )
	
	mark_as_advanced( CMAKE_EXE_LINKER_FLAGS_DEBUG 
					  CMAKE_SHARED_LINKER_FLAGS_DEBUG 
					  CMAKE_MODULE_LINKER_FLAGS_DEBUG 
					  CMAKE_CXX_FLAGS_DEBUG 
					  CMAKE_C_FLAGS_DEBUG 
					  )
endif()
	

###################################################################################################
# Release Link Flags
###################################################################################################
if( WIN32 )			
	set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /SAFESEH:NO /NODEFAULTLIB:LIBCMTD.lib")
	list( REMOVE_DUPLICATES CMAKE_EXE_LINKER_FLAGS_RELEASE )
	
	set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /SAFESEH:NO /NODEFAULTLIB:LIBCMTD.lib")
	list( REMOVE_DUPLICATES CMAKE_SHARED_LINKER_FLAGS_RELEASE )
	
	set(CMAKE_MODULE_LINKER_FLAGS_RELEASE "${CMAKE_MODULE_LINKER_FLAGS_RELEASE} /SAFESEH:NO /NODEFAULTLIB:LIBCMTD.lib")
	list( REMOVE_DUPLICATES CMAKE_MODULE_LINKER_FLAGS_RELEASE )
	
	set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /DNDEBUG /MP"  ) # /Gr" ) #  /Gr = fastCall
	list( REMOVE_DUPLICATES CMAKE_CXX_FLAGS_RELEASE )
	set( CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /DNDEBUG /MP"  ) # /Gr" ) #  /Gr = fastCall
	list( REMOVE_DUPLICATES CMAKE_C_FLAGS_RELEASE )
	
	MARK_AS_ADVANCED( CMAKE_EXE_LINKER_FLAGS_RELEASE 
					  CMAKE_SHARED_LINKER_FLAGS_RELEASE 
					  CMAKE_MODULE_LINKER_FLAGS_RELEASE 
					  CMAKE_CXX_FLAGS_RELEASE 
					  CMAKE_C_FLAGS_RELEASE 
					  )
endif()


###################################################################################################
# Custom compilation METAL TUNED
###################################################################################################
if( WIN32 )
	list( APPEND CMAKE_CONFIGURATION_TYPES 
		  METALTUNED 
		  )
	list( REMOVE_DUPLICATES CMAKE_CONFIGURATION_TYPES )
	
	
	set( CMAKE_CXX_FLAGS_METALTUNED 
		"${CMAKE_CXX_FLAGS_RELEASE} /Zi /Ox /Ob2 /Oi /Ot /Oy /Gd /arch:SSE2 /Gy- /Oi /Gm- /GF /fp:fast /GS- /Zc:forScope /O2 /MD /Zp16 /WX- /Zc:wchar_t /W3 /GL /LTCG /DDYNAMICBASE:NO /OPT:REF /VERBOSE:Lib /DAAA_METAL_TUNED=1 /D_SECURE_SCL=0 /D_HAS_ITERATOR_DEBUGGING=0" 
		CACHE STRING "Flags used by the C++ compiler during Metal Tuned builds." )
	
	STRING( REPLACE "/DDEBUG" "" CMAKE_CXX_FLAGS_METALTUNED ${CMAKE_CXX_FLAGS_METALTUNED})
	STRING( REPLACE "-DDEBUG" "" CMAKE_CXX_FLAGS_METALTUNED ${CMAKE_CXX_FLAGS_METALTUNED}) 
	STRING( REPLACE "/D_DEBUG" "" CMAKE_CXX_FLAGS_METALTUNED ${CMAKE_CXX_FLAGS_METALTUNED})
	list( REMOVE_DUPLICATES CMAKE_CXX_FLAGS_METALTUNED )
	
	
	set( CMAKE_C_FLAGS_METALTUNED 
		"${CMAKE_C_FLAGS_RELEASE} /Zi /Ox /Ob2 /Oi /Ot /Oy /Gd /arch:SSE2 /Gy- /Oi /Gm- /GF /fp:fast /GS- /Zc:forScope /O2 /MD /Zp16 /WX- /Zc:wchar_t /W3 /GL /LTCG /DDYNAMICBASE:NO /OPT:REF /VERBOSE:Lib /DAAA_METAL_TUNED=1 /D_SECURE_SCL=0 /D_HAS_ITERATOR_DEBUGGING=0" 
		CACHE STRING "Flags used by the C++ compiler during Metal Tuned builds." )
	
	STRING( REPLACE "/DDEBUG" "" CMAKE_C_FLAGS_METALTUNED ${CMAKE_C_FLAGS_METALTUNED})
	STRING( REPLACE "-DDEBUG" "" CMAKE_C_FLAGS_METALTUNED ${CMAKE_C_FLAGS_METALTUNED}) 
	STRING( REPLACE "/D_DEBUG" "" CMAKE_C_FLAGS_METALTUNED ${CMAKE_C_FLAGS_METALTUNED})
	list( REMOVE_DUPLICATES CMAKE_C_FLAGS_METALTUNED )
	

	set( CMAKE_EXE_LINKER_FLAGS_METALTUNED 
		"${CMAKE_EXE_LINKER_FLAGS_RELEASE} /SAFESEH:NO /NODEFAULTLIB:LIBCMT.lib" 
		CACHE STRING "Flags used for linking binaries during Metal Tuned builds." )
		
	set( CMAKE_SHARED_LINKER_FLAGS_METALTUNED 
		"${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /SAFESEH:NO /NODEFAULTLIB:LIBCMT.lib" 
		CACHE STRING "Flags used by the shared libraries linker during Metal Tuned builds." )
		
	set( CMAKE_MODULE_LINKER_FLAGS_METALTUNED 
		"${CMAKE_MODULE_LINKER_FLAGS_RELEASE} /SAFESEH:NO /NODEFAULTLIB:LIBCMT.lib" 
		CACHE STRING "Flags used by the module linker during Metal Tuned builds." )
		
	MARK_AS_ADVANCED( CMAKE_CXX_FLAGS_METALTUNED 
					  CMAKE_C_FLAGS_METALTUNED 
					  CMAKE_EXE_LINKER_FLAGS_METALTUNED 
					  CMAKE_SHARED_LINKER_FLAGS_METALTUNED 
					  CMAKE_MODULE_LINKER_FLAGS_METALTUNED 
					  )
endif()


###################################################################################################
# Custom compilation Debug WATCHDOG
###################################################################################################
if( WIN32 )
	list( APPEND CMAKE_CONFIGURATION_TYPES 
		  WATCHDOGDEBUG 
		  )
	list( REMOVE_DUPLICATES CMAKE_CONFIGURATION_TYPES )
	
	
	set( CMAKE_CXX_FLAGS_WATCHDOGDEBUG 
		"${CMAKE_CXX_FLAGS_DEBUG} /DEBUG /DAAA_WATCHDOG /ZI" 
		CACHE STRING "Flags used by the C++ compiler during WatchDog Debug builds." )
	
	STRING( REPLACE "/DNDEBUG" "" CMAKE_CXX_FLAGS_WATCHDOGDEBUG ${CMAKE_CXX_FLAGS_WATCHDOGDEBUG})
	STRING( REPLACE "-DNDEBUG" "" CMAKE_CXX_FLAGS_WATCHDOGDEBUG ${CMAKE_CXX_FLAGS_WATCHDOGDEBUG}) 
	STRING( REPLACE "/D_NDEBUG" "" CMAKE_CXX_FLAGS_WATCHDOGDEBUG ${CMAKE_CXX_FLAGS_WATCHDOGDEBUG})
	list( REMOVE_DUPLICATES CMAKE_CXX_FLAGS_WATCHDOGDEBUG )
	
	
	set( CMAKE_C_FLAGS_WATCHDOGDEBUG 
		"${CMAKE_C_FLAGS_DEBUG} /DEBUG /DAAA_WATCHDOG" 
		CACHE STRING "Flags used by the C++ compiler during WatchDog Debug builds." )
	
	STRING( REPLACE "/DNDEBUG" "" CMAKE_C_FLAGS_WATCHDOGDEBUG ${CMAKE_C_FLAGS_WATCHDOGDEBUG})
	STRING( REPLACE "-DNDEBUG" "" CMAKE_C_FLAGS_WATCHDOGDEBUG ${CMAKE_C_FLAGS_WATCHDOGDEBUG}) 
	STRING( REPLACE "/D_NDEBUG" "" CMAKE_C_FLAGS_WATCHDOGDEBUG ${CMAKE_C_FLAGS_WATCHDOGDEBUG})
	list( REMOVE_DUPLICATES CMAKE_C_FLAGS_WATCHDOGDEBUG )
	

	set( CMAKE_EXE_LINKER_FLAGS_WATCHDOGDEBUG 
		"${CMAKE_EXE_LINKER_FLAGS_DEBUG} /SAFESEH:NO /NODEFAULTLIB:LIBCMTD.lib /NODEFAULTLIB:msvcrt.lib" 
		CACHE STRING "Flags used for linking binaries during WatchDog Debug builds." )
		
	set( CMAKE_SHARED_LINKER_FLAGS_WATCHDOGDEBUG 
		"${CMAKE_SHARED_LINKER_FLAGS_DEBUG} /SAFESEH:NO /NODEFAULTLIB:LIBCMT.lib /NODEFAULTLIB:msvcrt.lib" 
		CACHE STRING "Flags used by the shared libraries linker during WatchDog Debug builds." )
		
	set( CMAKE_MODULE_LINKER_FLAGS_WATCHDOGDEBUG 
		"${CMAKE_MODULE_LINKER_FLAGS_DEBUG} /SAFESEH:NO /NODEFAULTLIB:LIBCMT.lib /NODEFAULTLIB:msvcrt.lib" 
		CACHE STRING "Flags used by the module linker during WatchDog Debug builds." )
		
	MARK_AS_ADVANCED( CMAKE_CXX_FLAGS_WATCHDOGDEBUG 
					  CMAKE_C_FLAGS_WATCHDOGDEBUG 
					  CMAKE_EXE_LINKER_FLAGS_WATCHDOGDEBUG 
					  CMAKE_SHARED_LINKER_FLAGS_WATCHDOGDEBUG 
					  CMAKE_MODULE_LINKER_FLAGS_WATCHDOGDEBUG 
					  )
endif()


###################################################################################################
# Custom compilation MetalTuned WATCHDOG
###################################################################################################
if( WIN32 )
	list( APPEND CMAKE_CONFIGURATION_TYPES 
		  WATCHDOGMETALTUNED 
		  )
	list( REMOVE_DUPLICATES CMAKE_CONFIGURATION_TYPES )
	
	
	set( CMAKE_CXX_FLAGS_WATCHDOGMETALTUNED 
		"${CMAKE_CXX_FLAGS_RELEASE} /Zi /Ox /Ob2 /Oi /Ot /Oy /Gd /arch:SSE2 /Gy- /Oi /Gm- /GF /fp:fast /GS- /Zc:forScope /O2 /MD /Zp16 /WX- /Zc:wchar_t /W3 /GL /LTCG /DDYNAMICBASE:NO /OPT:REF /VERBOSE:Lib /DAAA_METAL_TUNED=1 /D_SECURE_SCL=0 /D_HAS_ITERATOR_DEBUGGING=0 /DAAA_WATCHDOG" 
		CACHE STRING "Flags used by the C++ compiler during WatchDog Metal Tuned builds." )
	
	STRING( REPLACE "/DDEBUG" "" CMAKE_CXX_FLAGS_WATCHDOGMETALTUNED ${CMAKE_CXX_FLAGS_WATCHDOGMETALTUNED})
	STRING( REPLACE "-DDEBUG" "" CMAKE_CXX_FLAGS_WATCHDOGMETALTUNED ${CMAKE_CXX_FLAGS_WATCHDOGMETALTUNED}) 
	STRING( REPLACE "/D_DEBUG" "" CMAKE_CXX_FLAGS_WATCHDOGMETALTUNED ${CMAKE_CXX_FLAGS_WATCHDOGMETALTUNED})
	list( REMOVE_DUPLICATES CMAKE_CXX_FLAGS_WATCHDOGMETALTUNED )
	
	
	set( CMAKE_C_FLAGS_WATCHDOGMETALTUNED 
		"${CMAKE_C_FLAGS_RELEASE} /Zi /Ox /Ob2 /Oi /Ot /Oy /Gd /arch:SSE2 /Gy- /Oi /Gm- /GF /fp:fast /GS- /Zc:forScope /O2 /MD /Zp16 /WX- /Zc:wchar_t /W3 /GL /LTCG /DDYNAMICBASE:NO /OPT:REF /VERBOSE:Lib /DAAA_METAL_TUNED=1 /D_SECURE_SCL=0 /D_HAS_ITERATOR_DEBUGGING=0 /DAAA_WATCHDOG" 
		CACHE STRING "Flags used by the C++ compiler during WatchDog Metal Tuned builds." )
	
	STRING( REPLACE "/DDEBUG" "" CMAKE_C_FLAGS_WATCHDOGMETALTUNED ${CMAKE_C_FLAGS_WATCHDOGMETALTUNED})
	STRING( REPLACE "-DDEBUG" "" CMAKE_C_FLAGS_WATCHDOGMETALTUNED ${CMAKE_C_FLAGS_WATCHDOGMETALTUNED}) 
	STRING( REPLACE "/D_DEBUG" "" CMAKE_C_FLAGS_WATCHDOGMETALTUNED ${CMAKE_C_FLAGS_WATCHDOGMETALTUNED})
	list( REMOVE_DUPLICATES CMAKE_C_FLAGS_WATCHDOGMETALTUNED )
	

	set( CMAKE_EXE_LINKER_FLAGS_WATCHDOGMETALTUNED 
		"${CMAKE_EXE_LINKER_FLAGS_RELEASE} /SAFESEH:NO /NODEFAULTLIB:LIBCMT.lib" 
		CACHE STRING "Flags used for linking binaries during WatchDog Metal Tuned builds." )
		
	set( CMAKE_SHARED_LINKER_FLAGS_WATCHDOGMETALTUNED 
		"${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /SAFESEH:NO /NODEFAULTLIB:LIBCMT.lib" 
		CACHE STRING "Flags used by the shared libraries linker during WatchDog Metal Tuned builds." )
		
	set( CMAKE_MODULE_LINKER_FLAGS_WATCHDOGMETALTUNED 
		"${CMAKE_MODULE_LINKER_FLAGS_RELEASE} /SAFESEH:NO /NODEFAULTLIB:LIBCMT.lib" 
		CACHE STRING "Flags used by the module linker during WatchDog Metal Tuned builds." )
		
	MARK_AS_ADVANCED( CMAKE_CXX_FLAGS_WATCHDOGMETALTUNED 
					  CMAKE_C_FLAGS_WATCHDOGMETALTUNED 
					  CMAKE_EXE_LINKER_FLAGS_WATCHDOGMETALTUNED 
					  CMAKE_SHARED_LINKER_FLAGS_WATCHDOGMETALTUNED 
					  CMAKE_MODULE_LINKER_FLAGS_WATCHDOGMETALTUNED 
					  )
endif()


###################################################################################################
# Set Default Folders
###################################################################################################
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib )
set( CMAKE_INCLUDE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/include )


###################################################################################################
# Set default Build Type
###################################################################################################
if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE Debug
       CACHE STRING "Choose the type of build : None Debug Release MetalTuned."
       FORCE)
endif()


###################################################################################################
# Grab active configuration
###################################################################################################
set( OPTION_BUILD_TYPE_DEBUG OFF )
set( OPTION_BUILD_TYPE_RELEASE OFF )

# Debug
IF( CMAKE_BUILD_TYPE MATCHES "Debug" OR CMAKE_BUILD_TYPE MATCHES "WatchDogDebug" )
    set( OPTION_BUILD_TYPE_DEBUG ON )
else()
	set( OPTION_BUILD_TYPE_RELEASE ON )
endif()


###################################################################################################
# External path
###################################################################################################
IF( NOT OPTION_BUILD_PATH_SOURCE_EXTERNAL OR (OPTION_BUILD_PATH_SOURCE_EXTERNAL EQUAL ${AAASeedProject_SOURCE_DIR}/external) )
	MESSAGE(INFO " OPTION_BUILD_PATH_SOURCE_EXTERNAL set to ${AAASeedProject_SOURCE_DIR}/../AAASeed_External")
ENDIF( NOT OPTION_BUILD_PATH_SOURCE_EXTERNAL OR (OPTION_BUILD_PATH_SOURCE_EXTERNAL EQUAL ${AAASeedProject_SOURCE_DIR}/external) )

set(OPTION_BUILD_PATH_SOURCE_EXTERNAL "${AAASeedProject_SOURCE_DIR}/../AAASeed_External" CACHE PATH "OPTION_BUILD_PATH_SOURCE_EXTERNAL")


###################################################################################################
# Resources path
###################################################################################################
IF( NOT OPTION_BUILD_PATH_RESSOURCES OR (OPTION_BUILD_PATH_RESSOURCES EQUAL ${AAASeedProject_SOURCE_DIR}/Ressource) )
	MESSAGE(INFO " OPTION_BUILD_PATH_RESSOURCES set to ${AAASeedProject_SOURCE_DIR}/../AAASeed_Resources")
ENDIF( NOT OPTION_BUILD_PATH_RESSOURCES OR (OPTION_BUILD_PATH_RESSOURCES EQUAL ${AAASeedProject_SOURCE_DIR}/Ressource) )

set(OPTION_BUILD_PATH_RESSOURCES "${AAASeedProject_SOURCE_DIR}/../AAASeed_Resources" CACHE PATH "OPTION_BUILD_PATH_RESSOURCES")


###################################################################################################
# Setting out path
###################################################################################################
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_IN ${PROJECT_BINARY_DIR}/bin )
set( CMAKE_INCLUDE_OUTPUT_DIRECTORY_IN ${PROJECT_BINARY_DIR}/include )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_IN ${PROJECT_BINARY_DIR}/lib )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_IN ${PROJECT_BINARY_DIR}/lib )


###################################################################################################
# CUDA link
###################################################################################################
option( OPTION_LINK_CUDA "Link CUDA library." OFF )


##############################################################################
# Setting compilation option
##############################################################################
# Allows verbose:
option(OPTION_BUILD_VERBOSE "Enable verbose option (-v)." OFF)
# Warnings
option(OPTION_DISABLE_WARNINGS "Deactivate visual studio deprecated and other warnings." ON)
# STDC MACROS 
if(WIN32)	
	option(OPTION_BUILD_STDC_MACROS "Activate __STDC_LIMIT_MACROS and __STDC_CONSTANT_MACROS" ON)
endif()

# AAA_NEW_DESIGN definition
option(OPTION_AAA_NEW_DESIGN "Define AAA_NEW_DESIGN" ON)
if(OPTION_AAA_NEW_DESIGN)
	add_definitions( -DAAA_NEW_DESIGN )
endif()

##############################################################################
# Configure Visual Leak Detector 
##############################################################################
if( WIN32 AND OPTION_BUILD_TYPE_DEBUG)  
	option( OPTION_BUILD_USE_VLD "Use Visual Leak Detector for Visual Studio (requires Debugging Tools for Windows)." ON)
endif() 


##############################################################################
# MSA
##############################################################################
option(OPTION_BUILD_MSA_HOST_AAASEED "Activate AAASeed MSA (default behavior)" ON)
option(OPTION_BUILD_MSA_HOST_CINDER "Activate cinder MSA" OFF)
option(OPTION_BUILD_MSA_HOST_OPENFRAMEWORKS "Activate openFrameworks MSA" OFF)


##############################################################################
# Setting package versions informations according to text files in /version
##############################################################################
FILE(READ ${CMAKE_SOURCE_DIR}/version/AUTHORS.txt AUTHORS)
set(BUILDVERSION_STRING \"${BUILDVERSION_MAJOR}.${BUILDVERSION_MINOR}.${BUILDVERSION_PATCH}\")
set(BUILDVERSION "${BUILDVERSION_MAJOR}.${BUILDVERSION_MINOR}.${BUILDVERSION_PATCH}")


##############################################################################
# Printing and directories
##############################################################################
set(ENCRYPTOR_PROJECT_NAME ${CMAKE_PROJECT_NAME})
##############################################################################
# Printing
##############################################################################
set(NATIVE_LIB_STR_PRINT_PREFIX \"AAASeed\")
IF(WIN32)
set(NATIVE_LOCAL_PATH_DEDICATED \"\\\\AAASeed\\\\\")
ELSE(WIN32)
IF(APPLE)
set(NATIVE_LOCAL_PATH_DEDICATED \"/AAASeed/\")
ENDIF(APPLE)
ENDIF(WIN32)

set(NATIVE_BUILD_SQLITE_WRAPPER 1)

set(NATIVE_LOCAL_DIR_DEDICATED \"AAASeed.dir\")

set(NATIVE_LOCAL_PRODUCT_TYPE 10) # 10 for AAASeed


##############################################################################
##Platform configuration tests.
##############################################################################
include( ${CMAKE_ROOT}/Modules/CMakeBackwardCompatibilityCXX.cmake)
include( ${BASE_SOURCE_PATH}/CMake/CheckAtomicOps.cmake )


##############################################################################
# Setting coverage options :
##############################################################################
IF(WIN32)
	IF( CMAKE_SYSTEM_PROCESSOR STREQUAL "amd64" )
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
		
	ELSE( CMAKE_SYSTEM_PROCESSOR STREQUAL "amd64" )
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
		
	ENDIF( CMAKE_SYSTEM_PROCESSOR STREQUAL "amd64" )
	
	
ELSE(WIN32)
	IF(APPLE)
		set(CMAKE_MACOSX_RPATH TRUE)
		IF(CMAKE_SYSTEM_PROCESSOR STREQUAL "ia64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -single_module -headerpad_max_install_names ${COVERAGE_LIB}") 
			set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
		ELSE(CMAKE_SYSTEM_PROCESSOR STREQUAL "ia64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -single_module -headerpad_max_install_names ${COVERAGE_LIB}") 
		ENDIF(CMAKE_SYSTEM_PROCESSOR STREQUAL "ia64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL  "x86_64")
		
	ELSE(APPLE)
		IF(CMAKE_SYSTEM_PROCESSOR STREQUAL "ia64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wl,--export-dynamic")
			set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
		ELSE(CMAKE_SYSTEM_PROCESSOR STREQUAL "ia64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--export-dynamic")
		ENDIF(CMAKE_SYSTEM_PROCESSOR STREQUAL "ia64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
	ENDIF(APPLE)
	
ENDIF(WIN32)


##############################################################################
# Setting compilation options :
##############################################################################
# GCC specifics
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wall -std=c++0x")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -ggdb")
endif()
# CLANG specifics
if(APPLE)
  set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++0x")
  set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
  set(CMAKE_CXX_FLAGS "-std=c++0x -stdlib=libc++ ${CMAKE_CXX_FLAGS}")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g -Wall")
endif()
# Visual Studio specifics
IF(WIN32)
	IF(OPTION_BUILD_VERBOSE)
		# Warning Level 4 for Debug builds
		list(APPEND CMAKE_CXX_FLAGS_DEBUG " /W4")
	ENDIF(OPTION_BUILD_VERBOSE)
  list(REMOVE_DUPLICATES CMAKE_CXX_FLAGS_DEBUG)
ENDIF(WIN32)


###################################################################################################
#	Add definitions according to selected options
###################################################################################################
if( WIN32)
	if( OPTION_DISABLE_WARNINGS )
		add_definitions(-D_CRT_SECURE_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_DEPRECATE -D_SCL_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_DEPRECATE)
	endif()
		
	if( OPTION_BUILD_STDC_MACROS )
		add_definitions(-D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS)
	endif()
	
	add_definitions( -D_UNICODE -DUNICODE ) 
	
	#############################################
	# Maa OPTIONS
	#############################################
	add_definitions(-DAAASEED 
					-DAAA_VSTOOL=120 
					-D_WIN32_WINNT=0x0601
					-DWINVER=0x0601
					-DDPSAPI_VERSION=1 
					-DTRACKER 
					)

	add_definitions(-DOPTION_BUILD_EXPORT)
	
	
	add_definitions(-DGLEW_STATIC=1 
					# -DFT2_BUILD_LIBRARY
					# -DFTGL_LIBRARY=1 # -DFTGL_LIBRARY_STATIC=1
					-DWIN32
					#-DGLUT_STATIC=1
					#-DGLUT_BUILDING_LIB
					#-DGLUT32_EXPORTS
					-DLUAJIT_ENABLE_LUA52COMPAT
					-DLUA_LIB=1
					#-DCVAPI_EXPORTS
					-DAAA_OPENCV2
					# -DPGRCAMERAGUI_EXPORTS
					)

endif()
	
# POCO
#################################################
if(CMAKE_SYSTEM MATCHES "Windows")
	add_definitions( -DPOCO_OS_FAMILY_WINDOWS )
elseif (CMAKE_SYSTEM MATCHES "Linux" )
	add_definitions( -DPOCO_OS_FAMILY_UNIX )
endif()	
add_definitions(-DPOCO_STATIC -DPOCO_NO_AUTOMATIC_LIBS)
add_definitions(-DXML_STATIC -DXML_NS -DXML_DTD -DHAVE_EXPAT_CONFIG_H -DXML_EXPORTS)
add_definitions(-DUtil_EXPORTS)


# Freetype
add_definitions(-DFT2_BUILD_LIBRARY)


###################################################################################################
# MSAA Options (favor AAASeed host)
###################################################################################################
IF( OPTION_BUILD_MSA_HOST_AAASEED )
	ADD_DEFINITIONS(-DMSA_HOST_AAASEED)
	
ELSE( OPTION_BUILD_MSA_HOST_AAASEED )
	IF( OPTION_BUILD_MSA_HOST_OPENFRAMEWORKS )
		ADD_DEFINITIONS(-DMSA_HOST_OPENFRAMEWORKS)
	ENDIF( OPTION_BUILD_MSA_HOST_OPENFRAMEWORKS )

	IF( OPTION_BUILD_MSA_HOST_CINDER )
		ADD_DEFINITIONS(-DMSA_HOST_CINDER)
	ENDIF( OPTION_BUILD_MSA_HOST_CINDER )

ENDIF( OPTION_BUILD_MSA_HOST_AAASEED )


###################################################################################################
# Include Directories
###################################################################################################
include_directories(BEFORE ${CMAKE_SOURCE_DIR} ${CMAKE_INCLUDE_OUTPUT_DIRECTORY})


###################################################################################################
# Link Directories
###################################################################################################
LINK_DIRECTORIES( ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} )
# LINK_DIRECTORIES( "${BASE_BINARY_PATH}/OpenCV/lib/${CMAKE_CFG_INTDIR}" )
# LINK_DIRECTORIES( ${BASE_SOURCE_PATH}/../AAASeed_External/lib )


##############################################################################
# FINDING EXTERNAL LIBRARIES
##############################################################################
# Setting path to libraries and modules for apps
# set(UTILS_SOURCE_DIR ${AAASeedProject_SOURCE_DIR}/src/executable )
include( ${BASE_SOURCE_PATH}/CMake/FindExternalLibs.cmake )

#Set directories for linker 
IF(OPTION_BUILD_USE_VLD)
	LINK_DIRECTORIES(${VLD_LIBRARIES_DIR}) 
ENDIF(OPTION_BUILD_USE_VLD)


##############################################################################
# Set the include directories paths to build
##############################################################################
INCLUDE_DIRECTORIES( 
		${AAASeed_BINARY_DIR} # To include config.h
		
		#########################################
		# Resources
		#########################################
		${OPTION_BUILD_PATH_RESSOURCES}/dialog
		
		
		#########################################
		# Externals
		#########################################
		
		# Assimp
		${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/assimp/include
		
		# Freetype
		# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/freetype/include/freetype/config
		# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/freetype/include/freetype/internal
		# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/freetype/include/freetype
		${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/freetype/include
		
		# FTGL
		${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/FTGL/src
		# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/FTGL/src/FTFont
		# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/FTGL/src/FTGL
		# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/FTGL/src/FTGlyph
		# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/FTGL/src/FTLayout
		
		# Glew
		${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/glew/include
		${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/glew/include/GL
		
		# # Glut Maa
		# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/glutMaa
		# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/glutMaa/GL
		# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/glutMaa/include
		# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/glutMaa/include/mui
		# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/glutMaa/Src
		
		# # LuaJIT
		# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/LuaJIT/dynasm
		# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/LuaJIT/etc
		# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/LuaJIT/src/jit
		# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/LuaJIT/src/host
		# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/LuaJIT/src
		
		# OpenCV
		# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/OpenCV/include
		# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/OpenCV/modules/calib3d/include
		# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/OpenCV/modules/core/include
		# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/OpenCV/modules/features2d/include
		# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/OpenCV/modules/flann/include
		# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/OpenCV/modules/highgui/include
		# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/OpenCV/modules/imgproc/include
		# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/OpenCV/modules/legacy/include
		# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/OpenCV/modules/contrib/include
		# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/OpenCV/modules/ml/include
		# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/OpenCV/modules/objdetect/include
		# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/OpenCV/modules/photo/include
		# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/OpenCV/modules/video/include
		# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/OpenCV/modules/traincascade/include
		# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/OpenCV/modules/haartraining/include
		# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/OpenCV/modules/gpu/include
		# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/OpenCV/modules/stitching/include
		# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/OpenCV/include/opencv
		# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/OpenCV/include/opencv2
		# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/OpenCV/modules/ts/include/opencv2/ts
		# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/OpenCV/modules/ts/src
		# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/OpenCV/modules/video/include/opencv2/video
		# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/OpenCV/modules/video/src
		${AAASeedProject_SOURCE_DIR}/externalLookalike/opencv
		${AAASeedProject_SOURCE_DIR}/externalLookalike/opencv2
		
		# POCO
		${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/poco/CppParser/include 
		${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/poco/CppUnit/include 
		${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/poco/Crypto/include 
		${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/poco/Data/include 
		${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/poco/Foundation/include 
		${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/poco/JSON/include 
		${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/poco/MongoDB/include 
		${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/poco/Net/include 
		${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/poco/NetSSL_OpenSSL/include 
		${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/poco/PageCompiler/include 
		${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/poco/PDF/include 
		${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/poco/SevenZip/include 
		${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/poco/Util/include 
		${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/poco/XML/include 
		${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/poco/Zip/include 
		
		# OpenSSl
		${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/openssl/include
		
		
		
		# External look alike
		${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/Headers_ToDispose
		
		${AAASeedProject_SOURCE_DIR}/externalLookalike/
		${AAASeedProject_SOURCE_DIR}/externalLookalike/Awesomium
		${AAASeedProject_SOURCE_DIR}/externalLookalike/Cg
		${AAASeedProject_SOURCE_DIR}/externalLookalike/DXBaseClasses
		${AAASeedProject_SOURCE_DIR}/externalLookalike/FFGL
		${AAASeedProject_SOURCE_DIR}/externalLookalike/ffmpeg
		${AAASeedProject_SOURCE_DIR}/externalLookalike/ffmpeg/libavcodec
		${AAASeedProject_SOURCE_DIR}/externalLookalike/ffmpeg/libavdevice
		${AAASeedProject_SOURCE_DIR}/externalLookalike/ffmpeg/libavfilter
		${AAASeedProject_SOURCE_DIR}/externalLookalike/ffmpeg/libavformat
		# ${AAASeedProject_SOURCE_DIR}/externalLookalike/ffmpeg/libavutil
		${AAASeedProject_SOURCE_DIR}/externalLookalike/ffmpeg/libpostproc
		${AAASeedProject_SOURCE_DIR}/externalLookalike/ffmpeg/libswscale
		# ${AAASeedProject_SOURCE_DIR}/externalLookalike/FlyCap2
		# ${AAASeedProject_SOURCE_DIR}/externalLookalike/FlyCap2/C
		# ${AAASeedProject_SOURCE_DIR}/externalLookalike/FlyCap2/FC1
		# ${AAASeedProject_SOURCE_DIR}/externalLookalike/FlyCap2/freetype1 
		# ${AAASeedProject_SOURCE_DIR}/externalLookalike/FlyCap2/freetype2
		# ${AAASeedProject_SOURCE_DIR}/externalLookalike/freetype1
		# ${AAASeedProject_SOURCE_DIR}/externalLookalike/freetype2
		# ${AAASeedProject_SOURCE_DIR}/externalLookalike/FT
		# ${AAASeedProject_SOURCE_DIR}/externalLookalike/FT/Automates
		# ${AAASeedProject_SOURCE_DIR}/externalLookalike/FT/PLSuite
		# ${AAASeedProject_SOURCE_DIR}/externalLookalike/FT/SpeechEngine
		${AAASeedProject_SOURCE_DIR}/externalLookalike/FTPreDec2002
		${AAASeedProject_SOURCE_DIR}/externalLookalike/GflSdk
		${AAASeedProject_SOURCE_DIR}/externalLookalike/lua
		${AAASeedProject_SOURCE_DIR}/externalLookalike/NVidia
		# ${AAASeedProject_SOURCE_DIR}/externalLookalike/opencv2/calib3d
		# ${AAASeedProject_SOURCE_DIR}/externalLookalike/opencv2/contrib
		# ${AAASeedProject_SOURCE_DIR}/externalLookalike/opencv2/core
		# ${AAASeedProject_SOURCE_DIR}/externalLookalike/opencv2/features2d
		# ${AAASeedProject_SOURCE_DIR}/externalLookalike/opencv2/flann
		# ${AAASeedProject_SOURCE_DIR}/externalLookalike/opencv2/gpu
		# ${AAASeedProject_SOURCE_DIR}/externalLookalike/opencv2/highgui
		# ${AAASeedProject_SOURCE_DIR}/externalLookalike/opencv2/imgproc
		# ${AAASeedProject_SOURCE_DIR}/externalLookalike/opencv2/legacy
		# ${AAASeedProject_SOURCE_DIR}/externalLookalike/opencv2/ml
		# ${AAASeedProject_SOURCE_DIR}/externalLookalike/opencv2/objdetect
		# ${AAASeedProject_SOURCE_DIR}/externalLookalike/opencv2/ts
		# ${AAASeedProject_SOURCE_DIR}/externalLookalike/opencv2/video
		${AAASeedProject_SOURCE_DIR}/externalLookalike/openframeworks
		${AAASeedProject_SOURCE_DIR}/externalLookalike/openframeworks/utils
		${AAASeedProject_SOURCE_DIR}/externalLookalike/Quancom
		${AAASeedProject_SOURCE_DIR}/externalLookalike/Quicktime
		${AAASeedProject_SOURCE_DIR}/externalLookalike/Quicktime/CoreFoundation
		${AAASeedProject_SOURCE_DIR}/externalLookalike/Quicktime/GNUCompatibility
		${AAASeedProject_SOURCE_DIR}/externalLookalike/Tracker/ascension/PcBird
		${AAASeedProject_SOURCE_DIR}/externalLookalike/Tracker/KinectSDK
		${AAASeedProject_SOURCE_DIR}/externalLookalike/Tracker/PointGreyResearch
		${AAASeedProject_SOURCE_DIR}/externalLookalike/Tracker/PointGreyResearch/Digiclops
		${AAASeedProject_SOURCE_DIR}/externalLookalike/Tracker/PointGreyResearch/Triclops
		${AAASeedProject_SOURCE_DIR}/externalLookalike/Tracker/polhemus/pdi
		${AAASeedProject_SOURCE_DIR}/externalLookalike/Tracker/PQLabs
		${AAASeedProject_SOURCE_DIR}/externalLookalike/Tracker/SixSense
		${AAASeedProject_SOURCE_DIR}/externalLookalike/Tracker/SixSense/sixense_utils
		${AAASeedProject_SOURCE_DIR}/externalLookalike/Tracker/SixSense/sixense_utils/controller_manager
		${AAASeedProject_SOURCE_DIR}/externalLookalike/Tracker/TouchCo
		${AAASeedProject_SOURCE_DIR}/externalLookalike/Tracker
		
		#########################################
		# Internal
		#########################################
		${AAASeedProject_SOURCE_DIR}/src
		${AAASeedProject_SOURCE_DIR}/src/boids
		${AAASeedProject_SOURCE_DIR}/src/code_utils
		${AAASeedProject_SOURCE_DIR}/src/code_utils/buffer
		${AAASeedProject_SOURCE_DIR}/src/code_utils/file
		${AAASeedProject_SOURCE_DIR}/src/code_utils/thread
		${AAASeedProject_SOURCE_DIR}/src/code_utils/time
		${AAASeedProject_SOURCE_DIR}/src/core 
		${AAASeedProject_SOURCE_DIR}/src/draw
		${AAASeedProject_SOURCE_DIR}/src/FBO
		${AAASeedProject_SOURCE_DIR}/src/FFGL
		${AAASeedProject_SOURCE_DIR}/src/ftgl
		${AAASeedProject_SOURCE_DIR}/src/Gypsy
		${AAASeedProject_SOURCE_DIR}/src/image
		${AAASeedProject_SOURCE_DIR}/src/infrastructure
		${AAASeedProject_SOURCE_DIR}/src/infrastructure/bind
		${AAASeedProject_SOURCE_DIR}/src/infrastructure/data
		${AAASeedProject_SOURCE_DIR}/src/infrastructure/factory
		${AAASeedProject_SOURCE_DIR}/src/infrastructure/layer
		${AAASeedProject_SOURCE_DIR}/src/infrastructure/obj
		${AAASeedProject_SOURCE_DIR}/src/infrastructure/param
		${AAASeedProject_SOURCE_DIR}/src/language/lua
		${AAASeedProject_SOURCE_DIR}/src/lib_wrappers
		${AAASeedProject_SOURCE_DIR}/src/license
		${AAASeedProject_SOURCE_DIR}/src/math
		${AAASeedProject_SOURCE_DIR}/src/math/fn1d
		${AAASeedProject_SOURCE_DIR}/src/media/sound
		${AAASeedProject_SOURCE_DIR}/src/media/video
		${AAASeedProject_SOURCE_DIR}/src/media/video/capture
		${AAASeedProject_SOURCE_DIR}/src/media/video/capture/Kinect
		${AAASeedProject_SOURCE_DIR}/src/media/video/capture/Kinect/libusb/src
		${AAASeedProject_SOURCE_DIR}/src/media/video/deinterlace
		${AAASeedProject_SOURCE_DIR}/src/mocap
		${AAASeedProject_SOURCE_DIR}/src/MSA/MSACore
		${AAASeedProject_SOURCE_DIR}/src/MSA/MSACore/cinder-lite
		${AAASeedProject_SOURCE_DIR}/src/MSA/MSAFluid
		${AAASeedProject_SOURCE_DIR}/src/obj_ui
		${AAASeedProject_SOURCE_DIR}/src/obj_ui/bdd
		${AAASeedProject_SOURCE_DIR}/src/obj_ui/bdd/bdd_edit
		${AAASeedProject_SOURCE_DIR}/src/obj_ui/bdd/bdd_geo
		${AAASeedProject_SOURCE_DIR}/src/obj_ui/bdd/bdd_hardware
		${AAASeedProject_SOURCE_DIR}/src/obj_ui/bdd/bdd_image
		${AAASeedProject_SOURCE_DIR}/src/obj_ui/bdd/bdd_multitouch
		${AAASeedProject_SOURCE_DIR}/src/obj_ui/bdd/bdd_old
		${AAASeedProject_SOURCE_DIR}/src/obj_ui/bdd/bdd_point
		${AAASeedProject_SOURCE_DIR}/src/obj_ui/bdd/bdd_sound
		${AAASeedProject_SOURCE_DIR}/src/obj_ui/bdd/bdd_spe
		${AAASeedProject_SOURCE_DIR}/src/obj_ui/bdd/util
		${AAASeedProject_SOURCE_DIR}/src/obj_ui/bdd/com
		${AAASeedProject_SOURCE_DIR}/src/obj_ui/com
		${AAASeedProject_SOURCE_DIR}/src/obj_ui/com/osc
		${AAASeedProject_SOURCE_DIR}/src/obj_ui/deformer
		${AAASeedProject_SOURCE_DIR}/src/obj_ui/nvidia_cpl
		${AAASeedProject_SOURCE_DIR}/src/obj_ui/tessellation
		${AAASeedProject_SOURCE_DIR}/src/obj_ui/tracker
		${AAASeedProject_SOURCE_DIR}/src/obj_ui/tracker/Glove5DT
		${AAASeedProject_SOURCE_DIR}/src/obj_ui/transfo
		${AAASeedProject_SOURCE_DIR}/src/OpenCL
		${AAASeedProject_SOURCE_DIR}/src/OpenFramework/MsaFluids
		${AAASeedProject_SOURCE_DIR}/src/platform
		${AAASeedProject_SOURCE_DIR}/src/platform/win32
		${AAASeedProject_SOURCE_DIR}/src/shaders
		${AAASeedProject_SOURCE_DIR}/src/system
		${AAASeedProject_SOURCE_DIR}/src/system/shared/
		${AAASeedProject_SOURCE_DIR}/src/thread/shared
		${AAASeedProject_SOURCE_DIR}/src/tinyxml
		${AAASeedProject_SOURCE_DIR}/src/tracking
		${AAASeedProject_SOURCE_DIR}/src/TrueType
		${AAASeedProject_SOURCE_DIR}/src/ui
		${AAASeedProject_SOURCE_DIR}/src/ui/event
		${AAASeedProject_SOURCE_DIR}/src/unknown
		${AAASeedProject_SOURCE_DIR}/src/wrapper
		
		${AAASeedProject_SOURCE_DIR}/src/executable

		#########################################
		# Ressource
		#########################################
		${AAASeedProject_SOURCE_DIR}/Ressource/
		)
		
		# Thread
		if( WIN32 )
			include_directories( ${AAASeedProject_SOURCE_DIR}/src/thread/win32 )
		elseif( APPLE )
			include_directories( ${AAASeedProject_SOURCE_DIR}/src/thread/osx )
			include_directories( ${AAASeedProject_SOURCE_DIR}/src/thread/pthread )
		endif()
		
		# System
		if( WIN32 )
			include_directories( ${AAASeedProject_SOURCE_DIR}/src/system/win32/ )
		elseif( APPLE )
		endif()

IF(OPTION_BUILD_USE_VLD)
	INCLUDE_DIRECTORIES( ${VLD_INCLUDE_DIR} )
ENDIF(OPTION_BUILD_USE_VLD)


# Include src folder to compiling proccess
# Go to compile AAASeed libraries
ADD_SUBDIRECTORY(src)
#------------------------------------------------  





