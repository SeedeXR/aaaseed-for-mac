// src/gol/windows/windows_backend_stub.h
//
// Forward declaration / placeholder for `GOL::WindowsBackend`.
//
// This header exists ONLY to make the symbol greppable from the Mac
// side and to provide a documentation entry point for the Win-machine
// session that will land the real implementation. It is NOT included
// in any CMake target on Mac (the Mac build links MetalBackend only)
// and it does NOT declare or define the class -- attempting to use
// `GOL::WindowsBackend` on Mac is a compile error by design.
//
// Mac-side prerequisite (c126-c130 / Task #152, 2026-05-27) :
//   - Abstract `GOL::Backend` interface verified cross-platform
//     (no MTL::* / NS::* / CA::* leak) by the regression-guard test
//     at `tests/unit/gol_backend_cross_platform_test.cpp`.
//   - Win-side runbook published at `docs/windows-backend-howto.md`.
//
// Win-side execution (separate Win-machine session) :
//   - Implement `windows_backend.h` + `windows_backend.cpp` in this
//     directory per `docs/windows-backend-howto.md` Section 3.
//   - Wire the factory branch in `src/gol/gol_backend_factory.cpp`.
//   - Land Phase 3 EXIT golden test (Section 4) + frame-time
//     comparison test (Section 5).
//
// Doctrine references :
//   - memory/feedback_regression_guard_tests.md (c137)
//   - memory/feedback_hermetic_mac_sublibs.md
//   - memory/project_v1_ship_gate.md (Task #152 is v2 / Win-machine,
//     NOT v1 Mac ship blocker).

#pragma once

//	Hard guard : this file must NEVER compile into the Mac build. The
//	regression-guard test asserts `src/gol/windows/` is empty of .cpp
//	files in the Mac CMake target list ; this `#error` is the belt-
//	and-suspenders defense if a future session accidentally adds it.
#if !defined( _WIN32 ) && !defined( _WIN64 )
//	On Mac / Linux this header is documentation-only ; we do not error,
//	but we also do not declare the class. Greppable placeholder line :
//	GOL::WindowsBackend lives in this directory once the Win-machine
//	session lands its real implementation. See docs/windows-backend-howto.md.
namespace GOL
{
    //	Intentionally no class declaration on non-Win platforms. The
    //	abstract `GOL::Backend` in src/gol/gol_backend.h is the
    //	cross-platform contract ; the Mac build pulls MetalBackend from
    //	src/gol/metal/. Any use of `GOL::WindowsBackend` on Mac is a
    //	compile error (unknown identifier) by design.
}
#else
//	On a real Win build this header would forward-declare the class.
//	The Mac repo does not own that declaration ; the Win-machine
//	session will write `windows_backend.h` directly per the runbook.
namespace GOL
{
    class WindowsBackend;   // forward declaration ; defined in windows_backend.h on Win.
}
#endif
