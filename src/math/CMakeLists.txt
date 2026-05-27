# src/math/CMakeLists.txt
#
# Stage 2 of the engine port: compile the simplest math .cpp from the upstream
# Windows tree under Clang on arm64. This proves the cross-tree compile pipeline
# works for an actual engine translation unit, not just headers.
#
# We do NOT enumerate all 20 .cpp files of Src/math/ at this stage. We start
# with aaa_rand.cpp (30 lines, no SIMD, no XNA) and grow from there.
#
# Static lib target: aaaseed_math.

add_library(aaaseed_math STATIC
    "${AAASEED_ENGINE_SRC}/math/aaa_rand.cpp"
    "${AAASEED_ENGINE_SRC}/math/aaa_quat.cpp"
    "${AAASEED_ENGINE_SRC}/math/v_base.cpp"
    "${AAASEED_ENGINE_SRC}/math/aaa_matrix.cpp"
    "${AAASEED_ENGINE_SRC}/math/TMath.cpp"
    "${AAASEED_ENGINE_SRC}/math/v.cpp"
    # ──────────────────────────────────────────────────────────────────
    # Excluded from the Mac math lib :
    #
    # v_xna.cpp           — DirectX XNA Math; Windows-only. v.cpp's
    #                        unconditional `_xna` calls resolve via the
    #                        v_xna.h `#else` branch added 2026-05-25.
    # billboard.cpp       — transitively includes `Src/draw/aaa_gl.h`
    #                        which `#include <GL/glu.h>`. Lands when
    #                        the GOL Metal backend ports the GL include
    #                        chain (Phase 3).
    # math_lua.cpp        — needs Lua 5.1 headers; lands with Src/language/lua/.
    # noisturb.cpp, curves.cpp, voronoi.cpp, spots.cpp, rand.cpp,
    # noisdist.cpp, fn1d/*.cpp — pull in infrastructure/, image/, gol/
    #                       or file/ which haven't been ported yet.
    # ──────────────────────────────────────────────────────────────────
)

aaa_apply_engine_compile_flags(aaaseed_math)

target_include_directories(aaaseed_math PUBLIC
    "${AAASEED_ENGINE_SRC}"
    "${AAASEED_ENGINE_SRC}/math"
    "${AAASEED_ENGINE_SRC}/code_utils"
    "${AAASEED_ENGINE_SRC}/system"
)

set_target_properties(aaaseed_math PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)
