if(COMMAND cmake_policy)
      cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

	#############################################
	# Build getopt library 			
	#############################################

	SET(GETOPT_SOURCES
		${BASE_SOURCE_PATH}/AAASeed/src/executable/getopt.c
		${BASE_SOURCE_PATH}/AAASeed/src/executable/getopt1.c
		${BASE_SOURCE_PATH}/AAASeed/src/executable/getopt.h 
		)
	ADD_LIBRARY(getopt STATIC ${GETOPT_SOURCES})
	SET_TARGET_PROPERTIES(getopt PROPERTIES
		COMPILE_FLAGS "${EXECUTABLE_C_COMPILE_FLAGS}"
		PREFIX ""
		OUTPUT_NAME "libgetopt" )
		
	
	#############################################
	# Add include directories			
	#############################################
	INCLUDE_DIRECTORIES(	
		${AAASeed_SOURCE_DIR}
		${AAASeedProject_SOURCE_DIR}     
		${AAASeedProject_BINARY_DIR} 		
		)

	
	##########################################################################################
	# Build main executables			
	##########################################################################################
SET(executables_name AAASeed)

FOREACH(target AAASeed_exe)
	
	SET(${target}_EXE_SOURCES
		${UTILS_SOURCE_DIR}/seed.h
		${UTILS_SOURCE_DIR}/seed.cpp
		)
		
	ADD_EXECUTABLE( ${target} MACOSX_BUNDLE ${${target}_EXE_SOURCES} )
	
	SET_TARGET_PROPERTIES(${target} PROPERTIES COMPILE_FLAGS "${EXECUTABLE_CXX_COMPILE_FLAGS}")
	SET_TARGET_PROPERTIES(${target} PROPERTIES OUTPUT_NAME "${${target}_name}")  
		
	TARGET_LINK_LIBRARIES(${target} LibAAASeed getopt)
	
ENDFOREACH(target)

	
	INSTALL( TARGETS AAASeed_exe BUNDLE DESTINATION . COMPONENT AAASeed RUNTIME DESTINATION . COMPONENT AAASeed)

# IF( USING_WINDOWS_MSVC)
	# CONFIGURE_FILE(${OPTION_BUILD_PATH_RESSOURCES}/icon/logo_mwm_exe.ico ${CMAKE_CURRENT_BINARY_DIR}/logo_mwm_exe.ico COPYONLY)	
	# INSTALL( FILES ${OPTION_BUILD_PATH_RESSOURCES}/icon/logo_mwm_exe.ico DESTINATION . COMPONENT AAASeed)
# ENDIF( USING_WINDOWS_MSVC)

IF( USING_WINDOWS_MSVC)

	IF(OPTION_BUILD_TYPE_DEBUG)  
		INSTALL(FILES ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/${executables_name}.pdb DESTINATION . COMPONENT AAASeed)
	ENDIF(OPTION_BUILD_TYPE_DEBUG) 
	
ENDIF( USING_WINDOWS_MSVC)
