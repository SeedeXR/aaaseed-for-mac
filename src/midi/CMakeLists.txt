# src/midi/CMakeLists.txt
#
# second_todo.md S3 (c153) : native macOS CoreMIDI sub-lib. Mirrors the
# src/syphon hermetic pattern : pure ObjC++, links CoreMIDI + CoreFoundation
# only, no engine link / no o_str / no aaa_mem cascade. Manual reference
# counting (-fno-objc-arc).
#
# Symbol shape for callers : aaa::midi::* (header src/midi/midi_mac.h). Backs
# the engine-side c_midi banks on Mac, replacing the Win32 midiIn*/midiOut*
# byte-stream API with a 1:1 short-message / sysex surface.

find_library(COREMIDI_FRAMEWORK_MIDI       CoreMIDI       REQUIRED)
find_library(COREFOUNDATION_FRAMEWORK_MIDI CoreFoundation REQUIRED)

add_library(aaaseed_midi STATIC
    midi_mac.mm
)

set_source_files_properties(midi_mac.mm
    PROPERTIES
    LANGUAGE OBJCXX
    COMPILE_FLAGS "-fno-objc-arc"
)

target_include_directories(aaaseed_midi PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}"
)

target_link_libraries(aaaseed_midi PUBLIC
    "${COREMIDI_FRAMEWORK_MIDI}"
    "${COREFOUNDATION_FRAMEWORK_MIDI}"
)

target_compile_features(aaaseed_midi PUBLIC cxx_std_20)

set_target_properties(aaaseed_midi PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)
