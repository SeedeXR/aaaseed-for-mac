
###################################################################################################
# NOTE:
#	AAASeed is defined in AAASeed\AAASeed\src\CMakeLists.txt
#	LibAAASeed is defined in AAASeed\AAASeed\src\CMakeLists.txt
# 	OPTION_BUILD_PATH_SOURCE_EXTERNAL is defined in AAASeed\CMakeLists.txt
#	OPTION_BUILD_PATH_RESSOURCES is defined in AAASeed\CMakeLists.txt
#	BASE_BINARY_PATH is defined in AAASeed\CMakeLists.txt
###################################################################################################

ADD_CUSTOM_TARGET(copyEnv ALL DEPENDS LibAAASeed)


###################################################################################################
# Grab target files
###################################################################################################
file( GLOB_RECURSE ENVIRONMENT_FILES ${OPTION_BUILD_PATH_RESSOURCES}/environment/* )

foreach(file ${ENVIRONMENT_FILES})
	# Filter name to avoid full path recopy
	STRING(REPLACE ${OPTION_BUILD_PATH_RESSOURCES}/environment/ "" NEW_ENV_FILE_PATH ${file})
	
	# Copy files (this need to be done post build cause we need the "bin" folder to be generated)
	ADD_CUSTOM_COMMAND( TARGET copyEnv
						POST_BUILD
						COMMAND ${CMAKE_COMMAND}
						ARGS -E copy "${OPTION_BUILD_PATH_RESSOURCES}/environment/${NEW_ENV_FILE_PATH}" 
						"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/${NEW_ENV_FILE_PATH}"
						DEPENDS "${OPTION_BUILD_PATH_RESSOURCES}/environment/${NEW_ENV_FILE_PATH}")
endforeach()
	

#################################################
# !!! Special case -> licence file !!!
#################################################
ADD_CUSTOM_COMMAND( TARGET copyEnv
					POST_BUILD
					COMMAND ${CMAKE_COMMAND}
					ARGS -E copy "${OPTION_BUILD_PATH_RESSOURCES}/environment/license.txt" 
					"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../AAASeed/src/license.txt"
					DEPENDS "${OPTION_BUILD_PATH_RESSOURCES}/environment/license.txt")		   

 
 
 
 
 
 
 
