// tests/unit/aaa_execute_test.cpp
//
// c104 (2026-05-26) -- POSIX port of aaa::execute_shell + execute_process.
// Vendor source at vendor/aaaseed-engine/Src/aaa/execute.cpp is Win32-only
// (uses CreateProcessA, _popen). The Mac port lives at src/aaa/execute_posix.cpp
// and provides identical symbols via system(3) + posix_spawn(3) + waitpid(3).
//
// Test approach :
//   1. execute_shell( "/usr/bin/true", nullptr )  -> AAA_OK (shell exits 0).
//   2. execute_shell( "/usr/bin/false", nullptr ) -> ERR_ANY (shell exits 1).
//   3. execute_process( "/usr/bin/true", nullptr ) -> AAA_OK.
//   4. execute_process( "/usr/bin/false", nullptr ) -> ERR_ANY.
//   5. execute_process( "/nonexistent/path", nullptr ) -> ERR_ANY (spawn fails).
//   6. execute_process( "/bin/echo", "hello world" ) -> AAA_OK (whitespace-
//      split argv works ; child writes stdout but parent doesn't care).
//
// /usr/bin/true and /usr/bin/false are POSIX-mandated and present on every macOS
// install ; no test fixtures needed.

#include <gtest/gtest.h>

#include "aaa_type.h"
#include "err.h"
#include "aaa/execute.h"

TEST( AaaExecute, ShellTrueReturnsOk )
{
    AAA_ERR const ret = aaa::execute_shell( "/usr/bin/true", nullptr );
    EXPECT_EQ( ret, AAA_OK );
}

TEST( AaaExecute, ShellFalseReturnsErr )
{
    AAA_ERR const ret = aaa::execute_shell( "/usr/bin/false", nullptr );
    EXPECT_NE( ret, AAA_OK );
}

TEST( AaaExecute, ProcessTrueReturnsOk )
{
    AAA_ERR const ret = aaa::execute_process( "/usr/bin/true", nullptr );
    EXPECT_EQ( ret, AAA_OK );
}

TEST( AaaExecute, ProcessFalseReturnsErr )
{
    AAA_ERR const ret = aaa::execute_process( "/usr/bin/false", nullptr );
    EXPECT_NE( ret, AAA_OK );
}

TEST( AaaExecute, ProcessNonexistentPathReturnsErr )
{
    AAA_ERR const ret = aaa::execute_process(
        "/this/path/does/not/exist/AAASeed_c104_test", nullptr );
    EXPECT_NE( ret, AAA_OK );
}

TEST( AaaExecute, ProcessEchoWithArgSplitsArgv )
{
    //	/bin/echo always exits 0 ; this exercises the whitespace-split
    //	codepath inside execute_process. The shell never sees the args.
    AAA_ERR const ret = aaa::execute_process( "/bin/echo", "hello world" );
    EXPECT_EQ( ret, AAA_OK );
}

TEST( AaaExecute, ProcessNullCommandReturnsErr )
{
    AAA_ERR const ret = aaa::execute_process( nullptr, "ignored" );
    EXPECT_NE( ret, AAA_OK );
}
