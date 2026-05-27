# Replicates the vcxproj <IncludePath> for the AAASeed target.
# Paths are relative to the project root (CMAKE_SOURCE_DIR).
# CUDA include path is picked up from the AAASeed_Path_CUDA environment variable, mirroring
# the vcxproj's $(AAASeed_Path_CUDA) variable.


target_include_directories(AAASeed PRIVATE
    ${CMAKE_SOURCE_DIR}/Include
    ${CMAKE_SOURCE_DIR}/Src
    ${CMAKE_SOURCE_DIR}/Src/code_utils
    ${CMAKE_SOURCE_DIR}/Include/DXBaseClasses
    ${CMAKE_SOURCE_DIR}/Include/OpenCV/opencv4130
    ${CMAKE_SOURCE_DIR}/Src/ftgl
    ${CMAKE_SOURCE_DIR}/Include/Tracker/KinectSDK
    ${CMAKE_SOURCE_DIR}/Include/Tracker/KinectAzure
    ${CMAKE_SOURCE_DIR}/Include/freetype2
    ${CMAKE_SOURCE_DIR}/Src/MSA/MSACore
    ${CMAKE_SOURCE_DIR}/Src/tracking/CLM
    ${CMAKE_SOURCE_DIR}/Include/Tracker/SixSense
    ${CMAKE_SOURCE_DIR}/Include/Tracker/Jai
    ${CMAKE_SOURCE_DIR}/Include/ffmpeg
    ${CMAKE_SOURCE_DIR}/Include/Tracker/PointGreyResearch
    ${CMAKE_SOURCE_DIR}/Include/Quicktime
    ${CMAKE_SOURCE_DIR}/Ressource
    ${CMAKE_SOURCE_DIR}/Include/LibreOffice
)


# CUDA include (only if the env var is set, just like the vcxproj's $(AAASeed_Path_CUDA))
if(DEFINED ENV{AAASeed_Path_CUDA})
    target_include_directories(AAASeed PRIVATE $ENV{AAASeed_Path_CUDA}/include)
endif()


# OpenCV 4.1.2 only used by the Metal config in the vcxproj. Generator expression keeps it
# config-scoped without polluting the other configs.
target_include_directories(AAASeed PRIVATE
    $<$<CONFIG:Metal>:${CMAKE_SOURCE_DIR}/Include/opencv412>
)
