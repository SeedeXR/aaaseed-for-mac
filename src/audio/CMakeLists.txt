# src/audio/CMakeLists.txt
#
# second_todo.md S1 (c153) : native macOS audio sub-lib. AVAudioEngine player +
# input tap + CoreAudio device enumeration + a real (non-stub) energy-based
# beat detector. Hermetic : pure ObjC++ (the beat detector is pure std:: but
# lives here so it ships in one target), links AVFoundation + CoreAudio +
# Foundation only. No engine link / no o_str / no aaa_mem. -fno-objc-arc.
#
# Symbol shape for callers : aaa::audio::* (audio_mac.h, beat_detector.h).

find_library(AVFOUNDATION_FRAMEWORK_AUDIO AVFoundation REQUIRED)
find_library(COREAUDIO_FRAMEWORK_AUDIO    CoreAudio    REQUIRED)
find_library(FOUNDATION_FRAMEWORK_AUDIO   Foundation   REQUIRED)

add_library(aaaseed_audio STATIC
    audio_mac.mm
    beat_detector.mm
)

set_source_files_properties(audio_mac.mm beat_detector.mm
    PROPERTIES
    LANGUAGE OBJCXX
    COMPILE_FLAGS "-fno-objc-arc"
)

target_include_directories(aaaseed_audio PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}"
)

target_link_libraries(aaaseed_audio PUBLIC
    "${AVFOUNDATION_FRAMEWORK_AUDIO}"
    "${COREAUDIO_FRAMEWORK_AUDIO}"
    "${FOUNDATION_FRAMEWORK_AUDIO}"
)

target_compile_features(aaaseed_audio PUBLIC cxx_std_20)

set_target_properties(aaaseed_audio PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)
