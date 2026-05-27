# cmake/aaa_stb.cmake
#
# Static lib wrapping the vendored stb_image / stb_image_write headers.
# Phase 5 asset-library beachhead (continuation 44). Pure portable C ;
# builds identically on Mac and Windows. Mac side uses it as the lighter
# alternative to the Windows engine's FreeImage. The Windows path is
# untouched -- this lib is Mac-only by virtue of being unused on Windows.
#
# Public-domain license (stb_image header preamble) ; license-friendly
# for AAASeed's MIT distribution.
#
# Convention :
#   target_link_libraries(<consumer> PRIVATE aaaseed_stb)
#   then `#include "stb_image.h"` (header search path public on the lib).

set(AAA_STB_DIR "${CMAKE_SOURCE_DIR}/third_party/stb")

add_library(aaaseed_stb STATIC
    "${AAA_STB_DIR}/stb_impl.c"
)

target_include_directories(aaaseed_stb PUBLIC
    "${AAA_STB_DIR}"
)

#  stb_image emits a handful of -Wunused-* / -Wsign-compare warnings on
#  AppleClang's higher warning levels. We don't own this code -- silence
#  them rather than promote to errors. Engine code (under src/) keeps
#  full warnings via aaa_apply_engine_compile_flags().
target_compile_options(aaaseed_stb PRIVATE
    -Wno-unused-function
    -Wno-unused-but-set-variable
    -Wno-sign-compare
)

set_target_properties(aaaseed_stb PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)
