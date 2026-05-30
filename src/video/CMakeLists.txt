# src/video/CMakeLists.txt
#
# second_todo.md S2 (c153) : native macOS video sub-lib. The headline feature
# is the zero-copy CVPixelBuffer -> MTLTexture bridge (CVMetalTextureCache) for
# Apple-Silicon unified memory ; plus AVPlayer movie playback and
# AVCaptureSession camera capture, both vending IOSurface-backed BGRA8
# CVPixelBuffers straight into the bridge. Hermetic : pure ObjC++, links
# AVFoundation + CoreVideo + CoreMedia + Metal + Foundation only. No engine
# link / no o_str / no aaa_mem. -fno-objc-arc.
#
# Symbol shape for callers : aaa::video::* (metal_texture_bridge.h,
# movie_mac.h, capture_mac.h).

find_library(AVFOUNDATION_FRAMEWORK_VIDEO AVFoundation REQUIRED)
find_library(COREVIDEO_FRAMEWORK_VIDEO    CoreVideo    REQUIRED)
find_library(COREMEDIA_FRAMEWORK_VIDEO    CoreMedia    REQUIRED)
find_library(METAL_FRAMEWORK_VIDEO        Metal        REQUIRED)
find_library(FOUNDATION_FRAMEWORK_VIDEO   Foundation   REQUIRED)

add_library(aaaseed_video STATIC
    metal_texture_bridge.mm
    movie_mac.mm
    capture_mac.mm
)

set_source_files_properties(metal_texture_bridge.mm movie_mac.mm capture_mac.mm
    PROPERTIES
    LANGUAGE OBJCXX
    COMPILE_FLAGS "-fno-objc-arc"
)

target_include_directories(aaaseed_video PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}"
)

target_link_libraries(aaaseed_video PUBLIC
    "${AVFOUNDATION_FRAMEWORK_VIDEO}"
    "${COREVIDEO_FRAMEWORK_VIDEO}"
    "${COREMEDIA_FRAMEWORK_VIDEO}"
    "${METAL_FRAMEWORK_VIDEO}"
    "${FOUNDATION_FRAMEWORK_VIDEO}"
)

target_compile_features(aaaseed_video PUBLIC cxx_std_20)

set_target_properties(aaaseed_video PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)
