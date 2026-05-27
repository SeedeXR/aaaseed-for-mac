// Stage 1 of the infrastructure/obj/obj_ui port : prove the header chain
// rooted at obj_ui.h parses cleanly under AppleClang on arm64. We do NOT
// compile obj_ui.cpp here — that's Stage 3, after the factory and param
// subsystems have been ported.
//
// This test does nothing useful at runtime ; the only assertion is that the
// types declared in obj_ui.h are visible and have nonzero size. If the file
// compiles, the include chain is healthy. If it doesn't, we get a precise
// error pointing at the next blocker.

#include <gtest/gtest.h>

#include "infrastructure/obj/obj_ui.h"

TEST( ObjUiHeader, ParsesAndDeclaresCObjUi )
{
    EXPECT_GT( sizeof( c_obj_ui ), 0u );
}
