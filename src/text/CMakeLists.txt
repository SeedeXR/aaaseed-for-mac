# src/text/CMakeLists.txt
#
# Mac text subsystem (continuation 58). Builds on the stb_truetype
# beachhead (continuation 57). For now : one helper, build a printable-
# ASCII glyph atlas from a TTF buffer. Future additions : MTLTexture
# uploader + HUD text-drawing helper.
#
# Static lib `aaaseed_text` -- depends on `aaaseed_stb` for the
# stb_truetype implementation. PUBLIC includes the engine source dir
# so consumers can `#include "src/text/glyph_atlas.h"` directly.

add_library(aaaseed_text STATIC
    "${CMAKE_SOURCE_DIR}/src/text/glyph_atlas.cpp"
)

target_link_libraries(aaaseed_text PUBLIC
    aaaseed_stb
)

target_include_directories(aaaseed_text PUBLIC
    "${CMAKE_SOURCE_DIR}"
)

target_compile_features(aaaseed_text PUBLIC cxx_std_20)

set_target_properties(aaaseed_text PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)
