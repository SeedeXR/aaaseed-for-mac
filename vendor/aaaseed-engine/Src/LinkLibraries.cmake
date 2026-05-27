
###################################################################################################
# NOTE:
#	executable_name is defined in AAASeed\AAASeed\src\CMakeLists.txt
#	LibAAASeed is defined in AAASeed\AAASeed\src\CMakeLists.txt
# 	OPTION_BUILD_PATH_SOURCE_EXTERNAL is defined in AAASeed\CMakeLists.txt
#	OPTION_LINK_CUDA is defined in AAASeed\CMakeLists.txt
# 	NVIDIA_OPENCL_BASEDIR is defined in CMake/FindOpenCL.cmake
###################################################################################################


###################################################################################################
# Static dependencies
###################################################################################################
LIST(APPEND LIB_AAASEED_STATIC_DEPENDENCIES		 
		opengl32.lib 
		glu32.lib 
		setupapi.lib 
		winmm 
		version.lib 
		vfw32.lib 
		User32.lib 
		Psapi.lib 
		netapi32.lib 
		wsock32.lib 
		PowrProf.lib 
		DelayImp.lib 
		version.lib 
		comctl32.lib 
		ws2_32.lib 
		iphlpapi.lib 
		)	
		
if( OPTION_LINK_CUDA )
	list( APPEND LIB_AAASEED_STATIC_DEPENDENCIES 
			${NVIDIA_OPENCL_BASEDIR}/lib/Win32/cublas.lib 
			${NVIDIA_OPENCL_BASEDIR}/lib/Win32/cublas_device.lib 
			${NVIDIA_OPENCL_BASEDIR}/lib/Win32/cuda.lib 
			${NVIDIA_OPENCL_BASEDIR}/lib/Win32/cudadevrt.lib 
			# ${NVIDIA_OPENCL_BASEDIR}/lib/Win32/cudart.lib 
			${NVIDIA_OPENCL_BASEDIR}/lib/Win32/cudart_static.lib 
			${NVIDIA_OPENCL_BASEDIR}/lib/Win32/cufft.lib 
			${NVIDIA_OPENCL_BASEDIR}/lib/Win32/cufftw.lib 
			${NVIDIA_OPENCL_BASEDIR}/lib/Win32/curand.lib 
			${NVIDIA_OPENCL_BASEDIR}/lib/Win32/cusparse.lib 
			${NVIDIA_OPENCL_BASEDIR}/lib/Win32/nppc.lib 
			${NVIDIA_OPENCL_BASEDIR}/lib/Win32/nppi.lib 
			${NVIDIA_OPENCL_BASEDIR}/lib/Win32/npps.lib 
			${NVIDIA_OPENCL_BASEDIR}/lib/Win32/nvcuvenc.lib 
			${NVIDIA_OPENCL_BASEDIR}/lib/Win32/nvcuvid.lib 
			)
endif()


###################################################################################################
# Other dependencies
###################################################################################################	 

SET( LIB_AAASEED_OTHER_DEPENDENCIES
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/lib/LuaJIT.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/lib/ftgl.lib 

	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/lib/ftd2xx.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/lib/touchco.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/lib/PS3EyeLib.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/lib/PS3EyeMulticam.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/lib/PQMTClient.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/lib/Awesomium.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/lib/wke.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/lib/Tracker/SixSense/sixense.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/lib/Tracker/SixSense/sixense_utils.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/lib/PDI.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/lib/dsound.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/lib/lua51.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/lib/avcodec.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/lib/avdevice.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/lib/avformat.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/lib/avutil.lib 	
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/lib/swscale.lib
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/lib/ddraw.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/lib/dxguid.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/lib/Gypsy.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/lib/bass.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/lib/bassasio.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/lib/bird.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/lib/cg.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/lib/cgGL.lib 
	# libfftw3-3.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/lib/libgfl.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/lib/Tracker/KinectSDK/MSRKinectNUI.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/lib/censys2.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/lib/Tracker/KinectSDK/Kinect10.lib 
	# PGRFlyCapture.lib 
	# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/lib/pgrflycapturegui.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/lib/triclops.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/lib/hid.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/lib/setupapi.lib 
	# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/lib/QuickTime/QTMLClient.lib
	
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/lib/libfftw3-3.lib 
	)
	
	
###################################################################################################
# Configuration dedicated libraries lists
###################################################################################################
SET( LIBRARIES_WITH_DEBUG_SYMBOLS 
	${LIB_AAASEED_STATIC_DEPENDENCIES} 
	${LIB_AAASEED_OTHER_DEPENDENCIES} 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libDebug/Debug/assimpD.lib
	
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libDebug/Debug/opencv_calib3d248d.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libDebug/Debug/opencv_contrib248d.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libDebug/Debug/opencv_core248d.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libDebug/Debug/opencv_features2d248d.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libDebug/Debug/opencv_flann248d.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libDebug/Debug/opencv_gpu248d.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libDebug/Debug/opencv_haartraining_engined.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libDebug/Debug/opencv_highgui248d.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libDebug/Debug/opencv_imgproc248d.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libDebug/Debug/opencv_legacy248d.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libDebug/Debug/opencv_ml248d.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libDebug/Debug/opencv_nonfree248d.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libDebug/Debug/opencv_objdetect248d.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libDebug/Debug/opencv_ocl248d.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libDebug/Debug/opencv_photo248d.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libDebug/Debug/opencv_stitching248d.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libDebug/Debug/opencv_superres248d.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libDebug/Debug/opencv_ts248d.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libDebug/Debug/opencv_video248d.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libDebug/Debug/opencv_videostab248d.lib 
	
	# POCO
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libDebug/Debug/PocoCryptod.lib 
	# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libDebug/Debug/PocoDatad.lib 
	# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libDebug/Debug/PocoDataODBCd.lib 
	# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libDebug/Debug/PocoDataSQLited.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libDebug/Debug/PocoFoundationd.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libDebug/Debug/PocoJSONd.lib 
	# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libDebug/Debug/PocoMongoDBd.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libDebug/Debug/PocoNetd.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libDebug/Debug/PocoNetSSLd.lib 
	# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libDebug/Debug/PocoPDFd.lib
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libDebug/Debug/PocoUtild.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libDebug/Debug/PocoXMLd.lib 
	# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libDebug/Debug/PocoZipd.lib 
	
	# OpenSSL
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/openssl/x86/Debug/lib/libeay32.lib
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/openssl/x86/Debug/lib/ssleay32.lib
	
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libDebug/Debug/glewd.lib 
	# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libDebug/Debug/glutMaad.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libDebug/Debug/freetyped.lib 
	# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libDebug/Debug/ftgld.lib 
	# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libDebug/Debug/LuaJITd.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libDebug/Debug/zlibstaticd.lib
	     
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/lib/freetypedeb.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/lib/FlyCapture2d_v100.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/lib/fgloved.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/lib/strmbased.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/lib/FlyCapture2GUId.lib
	)
	
	
SET( LIBRARIES_WITHOUT_DEBUG_SYMBOLS 
	${LIB_AAASEED_STATIC_DEPENDENCIES} 
	${LIB_AAASEED_OTHER_DEPENDENCIES} 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libRelease/Release/assimp.lib
	
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libRelease/Release/opencv_calib3d248.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libRelease/Release/opencv_contrib248.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libRelease/Release/opencv_core248.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libRelease/Release/opencv_features2d248.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libRelease/Release/opencv_flann248.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libRelease/Release/opencv_gpu248.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libRelease/Release/opencv_haartraining_engine.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libRelease/Release/opencv_highgui248.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libRelease/Release/opencv_imgproc248.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libRelease/Release/opencv_legacy248.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libRelease/Release/opencv_ml248.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libRelease/Release/opencv_nonfree248.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libRelease/Release/opencv_objdetect248.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libRelease/Release/opencv_ocl248.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libRelease/Release/opencv_photo248.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libRelease/Release/opencv_stitching248.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libRelease/Release/opencv_superres248.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libRelease/Release/opencv_ts248.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libRelease/Release/opencv_video248.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libRelease/Release/opencv_videostab248.lib 
	
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libRelease/Release/glew.lib 
	# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libRelease/Release/glutMaa.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libRelease/Release/freetype.lib 
	# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libRelease/Release/ftgl.lib 
	# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libRelease/Release/LuaJIT.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libRelease/Release/zlibstatic.lib 
	
	# POCO
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libRelease/Release/PocoCrypto.lib 
	# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libRelease/Release/PocoData.lib 
	# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libRelease/Release/PocoDataODBC.lib 
	# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libRelease/Release/PocoDataSQLite.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libRelease/Release/PocoFoundation.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libRelease/Release/PocoJSON.lib 
	# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libRelease/Release/PocoMongoDB.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libRelease/Release/PocoNet.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libRelease/Release/PocoNetSSL.lib 
	# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libRelease/Release/PocoPDF.lib
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libRelease/Release/PocoUtil.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libRelease/Release/PocoXML.lib 
	# ${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/libRelease/Release/PocoZip.lib 
	
	# OpenSSL
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/openssl/x86/Release/lib/libeay32.lib
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/openssl/x86/Release/lib/ssleay32.lib
	
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/lib/freetypedeb.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/lib/FlyCapture2_v100.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/lib/fglove.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/lib/strmbase.lib 
	${OPTION_BUILD_PATH_SOURCE_EXTERNAL}/lib/FlyCapture2GUI.lib 
	)
					
					
###################################################################################################
# Link libraries depending on target configuration
###################################################################################################	
	
# Create dummy 'quasi' empty lib
FILE(WRITE ${CMAKE_BINARY_DIR}/Src/dummy.c "")
ADD_LIBRARY(dummy STATIC dummy.c)

SET_TARGET_PROPERTIES(dummy PROPERTIES LINKER_LANGUAGE C)

EXPORT(TARGETS dummy NAMESPACE imported FILE importeddummy.cmake)
INCLUDE(${CMAKE_BINARY_DIR}/Src/importeddummy.cmake)


# Set dummy lib properties
SET_TARGET_PROPERTIES(importeddummy PROPERTIES
						IMPORTED_LINK_INTERFACE_LIBRARIES_DEBUG "${LIBRARIES_WITH_DEBUG_SYMBOLS}" 
						IMPORTED_LINK_INTERFACE_LIBRARIES_WATCHDOGDEBUG "${LIBRARIES_WITH_DEBUG_SYMBOLS}" 
						IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "${LIBRARIES_WITHOUT_DEBUG_SYMBOLS}" 
						IMPORTED_LINK_INTERFACE_LIBRARIES_METALTUNED "${LIBRARIES_WITHOUT_DEBUG_SYMBOLS}" 
						IMPORTED_LINK_INTERFACE_LIBRARIES_WATCHDOGMETALTUNED "${LIBRARIES_WITHOUT_DEBUG_SYMBOLS}" 
						)

# Link to executable 
TARGET_LINK_LIBRARIES(${executable_name} importeddummy)

SET_TARGET_PROPERTIES(${executable_name} PROPERTIES COMPILE_FLAGS "${LIBAAASEED_CXX_COMPILE_FLAGS}")

 

 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
