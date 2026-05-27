# cmake/aaa_tinyexr.cmake
#
# Static lib wrapping the vendored tinyexr 1.0.x single-header.
# Phase 5 fifth beachhead (continuation 67), alongside :
#   stb_image       cmake/aaa_stb.cmake
#   Lua 5.1.5       cmake/aaa_lua.cmake
#   stb_truetype    (in aaaseed_stb)
#   LuaSocket 3.1.0 cmake/aaa_luasocket.cmake
#
# Closes the EXR asset-format gap noted in todo.md.
#
# Convention :
#   target_link_libraries(<consumer> PRIVATE aaaseed_tinyexr)
#   then `#include "tinyexr.h"` directly.
#
# License : BSD 3-clause (Syoyo Fujita and contributors).

set(AAA_TINYEXR_DIR "${CMAKE_SOURCE_DIR}/third_party/tinyexr")

add_library(aaaseed_tinyexr STATIC
    "${AAA_TINYEXR_DIR}/tinyexr_impl.cpp"
)

#  PUBLIC include : consumers `#include "tinyexr.h"` resolves here.
#  Also expose stb headers since tinyexr inline-references stb_image's
#  zlib helpers via TINYEXR_USE_STB_ZLIB=1.
target_include_directories(aaaseed_tinyexr PUBLIC
    "${AAA_TINYEXR_DIR}"
    "${CMAKE_SOURCE_DIR}/third_party/stb"
)

#  Need aaaseed_stb to provide the stbi_zlib_* / stbiw__zlib_compress
#  symbols at link time.
target_link_libraries(aaaseed_tinyexr PUBLIC
    aaaseed_stb
)

#  tinyexr targets older C++ conventions ; silence noisy warnings.
target_compile_options(aaaseed_tinyexr PRIVATE
    -Wno-deprecated-declarations
    -Wno-unused-parameter
    -Wno-unused-variable
    -Wno-sign-compare
    -Wno-implicit-fallthrough
    -Wno-shorten-64-to-32
    -Wno-format
    -Wno-unused-function
)

set_target_properties(aaaseed_tinyexr PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)
