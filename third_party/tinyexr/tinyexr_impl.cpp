/*
 * third_party/tinyexr/tinyexr_impl.cpp
 *
 * Single translation unit that instantiates the tinyexr implementation
 * (continuation 67). Mirrors the third_party/stb/stb_impl.c convention :
 * include the header in many TUs, but define TINYEXR_IMPLEMENTATION in
 * exactly ONE of them.
 *
 * tinyexr offers two deflate backends : miniz (default) and stb_zlib.
 * We use stb_zlib so we don't have to vendor miniz separately --
 * aaaseed_stb already provides stbi_zlib_decode_* + stbiw__zlib_compress.
 *
 * Phase 5 fifth beachhead (continuation 67), alongside :
 *   stb_image      c44
 *   Lua 5.1.5      c45
 *   stb_truetype   c57
 *   LuaSocket 3.1.0 c62
 *
 * BSD-3-Clause license (Syoyo Fujita + contributors).
 *
 * Compiled as C++ : tinyexr.h is C++ throughout.
 */

#define TINYEXR_IMPLEMENTATION
#define TINYEXR_USE_MINIZ     0
#define TINYEXR_USE_STB_ZLIB  1

//	Pull stb_image + stb_image_write FIRST so tinyexr.h sees the
//	stbi_zlib_decode_* / stbiw__zlib_compress symbols already declared.
#include "stb_image.h"
#include "stb_image_write.h"

#include "tinyexr.h"
