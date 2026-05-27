/*
 * third_party/stb/stb_impl.c
 *
 * Single translation unit that instantiates the stb_image / stb_image_write
 * / stb_truetype implementations. Sean Barrett's single-header library
 * convention : include the header in many TUs, but define
 * STB_*_IMPLEMENTATION in exactly ONE of them. This file is that one.
 *
 * Wrapped into a static library `aaaseed_stb` by cmake/aaa_stb.cmake so
 * any future Mac code can link against it without recompiling the impl.
 *
 * Doctrine reminder : Mac-primary, Windows-reciprocal. stb_* are pure
 * portable C ; they build on Windows just the same. The Windows engine
 * uses FreeImage + freetype for the equivalents ; the stb single-headers
 * are the Mac-side beachheads (Phase 5).
 *
 * stb_truetype added continuation 57 (Phase 5 third beachhead, alongside
 * stb_image continuation 44 + Lua 5.1 continuation 45).
 */

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION

#include "stb_image.h"
#include "stb_image_write.h"
#include "stb_truetype.h"
