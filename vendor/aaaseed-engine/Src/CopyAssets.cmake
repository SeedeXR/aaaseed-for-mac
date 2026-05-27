
###################################################################################################
# NOTE:
#	AAASeed is defined in AAASeed\AAASeed\src\CMakeLists.txt
#	LibAAASeed is defined in AAASeed\AAASeed\src\CMakeLists.txt
# 	OPTION_BUILD_PATH_SOURCE_EXTERNAL is defined in AAASeed\CMakeLists.txt
#	OPTION_BUILD_PATH_RESSOURCES is defined in AAASeed\CMakeLists.txt
#	BASE_BINARY_PATH is defined in AAASeed\CMakeLists.txt
###################################################################################################

ADD_CUSTOM_TARGET(copyAssets ALL DEPENDS LibAAASeed)


###################################################################################################
# Grab target files
###################################################################################################
file( GLOB_RECURSE ASSETS_FILES ${OPTION_BUILD_PATH_RESSOURCES}/dialog/* )

foreach(file ${ASSETS_FILES})
	# Filter name to avoid full path recopy
	STRING(REPLACE ${OPTION_BUILD_PATH_RESSOURCES}/dialog/ "" NEW_ASSETS_FILE_PATH ${file})
	
	# Copy files (this need to be done post build cause we need the "bin" folder to be generated)
	ADD_CUSTOM_COMMAND( TARGET copyAssets
						POST_BUILD
						COMMAND ${CMAKE_COMMAND}
						ARGS -E copy "${OPTION_BUILD_PATH_RESSOURCES}/dialog/${NEW_ASSETS_FILE_PATH}" 
						"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/${NEW_ASSETS_FILE_PATH}"
						DEPENDS "${OPTION_BUILD_PATH_RESSOURCES}/dialog/${NEW_ASSETS_FILE_PATH}")
endforeach()	   

 
 
 
 
 
 
 
