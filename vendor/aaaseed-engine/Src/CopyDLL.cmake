
###################################################################################################
# NOTE:
#	AAASeed is defined in AAASeed\AAASeed\src\CMakeLists.txt
#	LibAAASeed is defined in AAASeed\AAASeed\src\CMakeLists.txt
# 	OPTION_BUILD_PATH_SOURCE_EXTERNAL is defined in AAASeed\CMakeLists.txt
###################################################################################################

ADD_CUSTOM_TARGET(copydll ALL DEPENDS LibAAASeed)

	
	# Future use -> Clang
	# IF(APPLE)
	# #=== Copy the dll in the bin folder===    
		  # ADD_CUSTOM_COMMAND (
							# TARGET copydll
							# POST_BUILD
							# COMMAND ${CMAKE_COMMAND}
							# ARGS -E copy "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/LibAAASeed.dylib" 
							# "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/AAASeed.app/Contents/MacOS/LibAAASeed.dylib"
							# DEPENDS "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/LibAAASeed.dylib")
	# ENDIF()


###################################################################################################
# External DLLs
###################################################################################################

#################################################
# lua51.dll
#################################################
ADD_CUSTOM_COMMAND(TARGET copydll
				   POST_BUILD
				   COMMAND ${CMAKE_COMMAND}
				   ARGS -E copy "${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/dll/lua51.dll" 
				   "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/lua51.dll"
				   DEPENDS "${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/dll/lua51.dll" 
				   )

#################################################
# ftgl.dll
#################################################
ADD_CUSTOM_COMMAND(TARGET copydll
				   POST_BUILD
				   COMMAND ${CMAKE_COMMAND}
				   ARGS -E copy "${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/dll/ftgl.dll" 
				   "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/ftgl.dll"
				   DEPENDS "${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/dll/ftgl.dll" 
				   )

#################################################
# ftd2xx.dll
#################################################
ADD_CUSTOM_COMMAND(TARGET copydll
				   POST_BUILD
				   COMMAND ${CMAKE_COMMAND}
				   ARGS -E copy "${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/dll/ftd2xx.dll" 
				   "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/ftd2xx.dll"
				   DEPENDS "${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/dll/ftd2xx.dll" 
				   )

#################################################
# touchco.dll
#################################################
ADD_CUSTOM_COMMAND(TARGET copydll
				   POST_BUILD
				   COMMAND ${CMAKE_COMMAND}
				   ARGS -E copy "${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/dll/touchco.dll" 
				   "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/touchco.dll"
				   DEPENDS "${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/dll/touchco.dll" 
				   )

#################################################
# FlyCapture2_v100.dll
#################################################					   
IF( OPTION_BUILD_TYPE_DEBUG )
	ADD_CUSTOM_COMMAND(TARGET copydll
					   POST_BUILD
					   COMMAND ${CMAKE_COMMAND}
					   ARGS -E copy "${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/dll/FlyCapture2d_v100.dll" 
					   "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/FlyCapture2d_v100.dll"
					   DEPENDS "${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/dll/FlyCapture2d_v100.dll" 
					   )
					   
ELSE( OPTION_BUILD_TYPE_DEBUG )
	ADD_CUSTOM_COMMAND(TARGET copydll
					   POST_BUILD
					   COMMAND ${CMAKE_COMMAND}
					   ARGS -E copy "${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/dll/FlyCapture2_v100.dll" 
					   "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/FlyCapture2_v100.dll"
					   DEPENDS "${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/dll/FlyCapture2_v100.dll" 
					   )

ENDIF( OPTION_BUILD_TYPE_DEBUG )

#################################################
# FlyCapture2GUI.dll
#################################################
ADD_CUSTOM_COMMAND(TARGET copydll
				   POST_BUILD
				   COMMAND ${CMAKE_COMMAND}
				   ARGS -E copy "${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/dll/FlyCapture2GUI.dll" 
				   "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/FlyCapture2GUI.dll"
				   DEPENDS "${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/dll/FlyCapture2GUI.dll" 
				   )

#################################################
# Gypsy.dll
#################################################
ADD_CUSTOM_COMMAND(TARGET copydll
				   POST_BUILD
				   COMMAND ${CMAKE_COMMAND}
				   ARGS -E copy "${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/dll/Gypsy.dll" 
				   "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/Gypsy.dll"
				   DEPENDS "${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/dll/Gypsy.dll" 
				   )

#################################################
# libiomp5md.dll
#################################################
ADD_CUSTOM_COMMAND(TARGET copydll
				   POST_BUILD
				   COMMAND ${CMAKE_COMMAND}
				   ARGS -E copy "${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/dll/libiomp5md.dll" 
				   "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/libiomp5md.dll"
				   DEPENDS "${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/dll/libiomp5md.dll" 
				   )

#################################################
# PS3EyeLib.dll
#################################################
ADD_CUSTOM_COMMAND(TARGET copydll
				   POST_BUILD
				   COMMAND ${CMAKE_COMMAND}
				   ARGS -E copy "${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/dll/PS3EyeLib.dll" 
				   "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/PS3EyeLib.dll"
				   DEPENDS "${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/dll/PS3EyeLib.dll" 
				   )

#################################################
# PQMTClient.dll
#################################################
ADD_CUSTOM_COMMAND(TARGET copydll
				   POST_BUILD
				   COMMAND ${CMAKE_COMMAND}
				   ARGS -E copy "${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/dll/PQMTClient.dll" 
				   "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/PQMTClient.dll"
				   DEPENDS "${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/dll/PQMTClient.dll" 
				   )

#################################################
# Awesomium.dll
#################################################
ADD_CUSTOM_COMMAND(TARGET copydll
				   POST_BUILD
				   COMMAND ${CMAKE_COMMAND}
				   ARGS -E copy "${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/dll/Awesomium.dll" 
				   "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/Awesomium.dll"
				   DEPENDS "${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/dll/Awesomium.dll" 
				   )

#################################################
# wke.dll
#################################################
ADD_CUSTOM_COMMAND(TARGET copydll
				   POST_BUILD
				   COMMAND ${CMAKE_COMMAND}
				   ARGS -E copy "${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/dll/wke.dll" 
				   "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/wke.dll"
				   DEPENDS "${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/dll/wke.dll" 
				   )

#################################################
# sixense.dll
#################################################
ADD_CUSTOM_COMMAND(TARGET copydll
				   POST_BUILD
				   COMMAND ${CMAKE_COMMAND}
				   ARGS -E copy "${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/dll/sixense.dll" 
				   "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/sixense.dll"
				   DEPENDS "${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/dll/sixense.dll" 
				   )

#################################################
# sixense_utils.dll
#################################################
ADD_CUSTOM_COMMAND(TARGET copydll
				   POST_BUILD
				   COMMAND ${CMAKE_COMMAND}
				   ARGS -E copy "${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/dll/sixense_utils.dll" 
				   "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/sixense_utils.dll"
				   DEPENDS "${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/dll/sixense_utils.dll" 
				   )

#################################################
# PDI.dll
#################################################
ADD_CUSTOM_COMMAND(TARGET copydll
				   POST_BUILD
				   COMMAND ${CMAKE_COMMAND}
				   ARGS -E copy "${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/dll/PDI.dll" 
				   "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/PDI.dll"
				   DEPENDS "${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/dll/PDI.dll" 
				   )

#################################################
# PiCmdIF.dll
#################################################
ADD_CUSTOM_COMMAND(TARGET copydll
				   POST_BUILD
				   COMMAND ${CMAKE_COMMAND}
				   ARGS -E copy "${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/dll/PiCmdIF.dll" 
				   "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/PiCmdIF.dll"
				   DEPENDS "${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/dll/PiCmdIF.dll" 
				   )

#################################################
# libgfl340.dll
#################################################
ADD_CUSTOM_COMMAND(TARGET copydll
				   POST_BUILD
				   COMMAND ${CMAKE_COMMAND}
				   ARGS -E copy "${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/dll/libgfl340.dll" 
				   "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/libgfl340.dll"
				   DEPENDS "${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/dll/libgfl340.dll" 
				   )

#################################################
# cg.dll
#################################################
ADD_CUSTOM_COMMAND(TARGET copydll
				   POST_BUILD
				   COMMAND ${CMAKE_COMMAND}
				   ARGS -E copy "${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/dll/cg.dll" 
				   "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/cg.dll"
				   DEPENDS "${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/dll/cg.dll" 
				   )

#################################################
# cgGL.dll
#################################################
ADD_CUSTOM_COMMAND(TARGET copydll
				   POST_BUILD
				   COMMAND ${CMAKE_COMMAND}
				   ARGS -E copy "${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/dll/cgGL.dll" 
				   "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/cgGL.dll"
				   DEPENDS "${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/dll/cgGL.dll" 
				   )

#################################################
# msvcp100.dll
#################################################				   
IF( OPTION_BUILD_TYPE_DEBUG OR OPTION_BUILD_TYPE_WATCHDOGDEBUG )
	ADD_CUSTOM_COMMAND(TARGET copydll
					   POST_BUILD
					   COMMAND ${CMAKE_COMMAND}
					   ARGS -E copy "${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/dll/msvcp100d.dll" 
					   "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/msvcp100d.dll"
					   DEPENDS "${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/dll/msvcp100d.dll" 
					   )
					   
ELSE( OPTION_BUILD_TYPE_DEBUG OR OPTION_BUILD_TYPE_WATCHDOGDEBUG )		
	ADD_CUSTOM_COMMAND(TARGET copydll
				   POST_BUILD
				   COMMAND ${CMAKE_COMMAND}
				   ARGS -E copy "${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/dll/msvcp100.dll" 
				   "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/msvcp100.dll"
				   DEPENDS "${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/dll/msvcp100.dll" 
				   )			   
					   
ENDIF( OPTION_BUILD_TYPE_DEBUG OR OPTION_BUILD_TYPE_WATCHDOGDEBUG )

#################################################
# msvcp100.dll
#################################################				   
IF( OPTION_BUILD_TYPE_DEBUG OR OPTION_BUILD_TYPE_WATCHDOGDEBUG )
	ADD_CUSTOM_COMMAND(TARGET copydll
					   POST_BUILD
					   COMMAND ${CMAKE_COMMAND}
					   ARGS -E copy "${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/dll/msvcr100d.dll" 
					   "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/msvcr100d.dll"
					   DEPENDS "${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/dll/msvcr100d.dll" 
					   )
					   
ELSE( OPTION_BUILD_TYPE_DEBUG OR OPTION_BUILD_TYPE_WATCHDOGDEBUG )	
	ADD_CUSTOM_COMMAND(TARGET copydll
				   POST_BUILD
				   COMMAND ${CMAKE_COMMAND}
				   ARGS -E copy "${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/dll/msvcr100.dll" 
				   "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/msvcr100.dll"
				   DEPENDS "${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/dll/msvcr100.dll" 
				   )				   
					   
ENDIF( OPTION_BUILD_TYPE_DEBUG OR OPTION_BUILD_TYPE_WATCHDOGDEBUG )
				   

 
 
 
 
 
 
