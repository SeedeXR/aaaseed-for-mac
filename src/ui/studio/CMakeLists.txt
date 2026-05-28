# src/ui/studio/CMakeLists.txt
#
# AAASeed ImGui Studio — authoring surface built on Dear ImGui + Metal.
#
# Produces: aaaseed_ui_studio_mac (static lib)
#
# Hermetic Mac sub-lib (feedback_hermetic_mac_sublibs.md):
#   - Pure ObjC++. No o_str / aaa_str / aaa_mem / aaaseed_code_utils.
#   - Links: aaaseed_imgui_metal_backend + aaaseed_gol_metal +
#            aaaseed_meu_runner + Cocoa + Metal + MetalKit.

find_library(COCOA_FW_STUDIO      Cocoa         REQUIRED)
find_library(METAL_FW_STUDIO      Metal         REQUIRED)
find_library(METALKIT_FW_STUDIO   MetalKit      REQUIRED)
find_library(COREAUDIO_FW_STUDIO  CoreAudio     REQUIRED)
find_library(AVFOUNDATION_FW_STUDIO   AVFoundation   REQUIRED)
find_library(COREMEDIA_FW_STUDIO      CoreMedia      REQUIRED)
find_library(COREVIDEO_FW_STUDIO      CoreVideo      REQUIRED)
find_library(IOSURFACE_FW_STUDIO      IOSurface      REQUIRED)

add_library(aaaseed_ui_studio_mac STATIC
    aaa_studio.cpp           # c151-B : platform-neutral data model
    aaa_studio_metal.mm      # Mac glue : ImGui Metal + Cocoa + NSTask + AVFoundation + CoreAudio
)

# c151-B : the .cpp is pure C++ (no ObjC), the .mm is MRC ObjC++.
set_source_files_properties(aaa_studio.cpp
    PROPERTIES
    LANGUAGE CXX
)
set_source_files_properties(aaa_studio_metal.mm
    PROPERTIES
    LANGUAGE OBJCXX
    COMPILE_FLAGS "-fno-objc-arc -Wno-unused-parameter"
)

target_include_directories(aaaseed_ui_studio_mac PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}"
)

# c151-A : same compile define as the path-A perf test ; the Shader
# Catalog panel uses it to enumerate src/shaders/msl/*.metal at runtime.
target_compile_definitions(aaaseed_ui_studio_mac PRIVATE
    "AAA_SHADERS_MSL_DIR=\"${CMAKE_SOURCE_DIR}/src/shaders/msl\""
)

target_link_libraries(aaaseed_ui_studio_mac PUBLIC
    aaaseed_imgui_metal_backend
    aaaseed_gol_metal
    aaaseed_meu_runner
    "${COCOA_FW_STUDIO}"
    "${METAL_FW_STUDIO}"
    "${METALKIT_FW_STUDIO}"
    "${COREAUDIO_FW_STUDIO}"
    # c151-B : AVFoundation camera capture + IOSurface-backed
    # CVPixelBuffer → MTLTexture preview.
    "${AVFOUNDATION_FW_STUDIO}"
    "${COREMEDIA_FW_STUDIO}"
    "${COREVIDEO_FW_STUDIO}"
    "${IOSURFACE_FW_STUDIO}"
)

target_compile_features(aaaseed_ui_studio_mac PUBLIC cxx_std_20)

set_target_properties(aaaseed_ui_studio_mac PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)
