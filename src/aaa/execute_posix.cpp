// src/aaa/execute_posix.cpp
//
// Mac (POSIX) port of vendor/aaaseed-engine/Src/aaa/execute.cpp.
//
// The Windows source uses _popen + CreateProcessA + CREATE_NEW_CONSOLE.
// Both are Win32-specific. The Mac side substitutes the matching POSIX
// primitives (per the project's "never stub when a real native port
// exists" policy) :
//   - execute_shell   -> system(3)        ; runs via /bin/sh -c "..."
//   - execute_process -> posix_spawn(3)   ; spawns a child without going
//                                           through the shell, mirroring
//                                           the Win32 CreateProcessA path.
//
// The Windows source compiles in vendor/.../execute.cpp under
// #ifdef WIN32 / Win32 headers ; this file provides the same
// aaa::execute_shell + aaa::execute_process symbols for the Mac build.
// Wired via src/aaa/CMakeLists.txt ; the vendor .cpp is NOT added to
// the Mac target so there's no duplicate-symbol clash.
//
// Cross-platform contract :
//   - Returns AAA_OK iff the child exited with status 0.
//   - Returns ERR_ANY on any spawn / wait / non-zero-exit error.
//
// On Mac the command + arg pair is parsed POSIX-style :
//   - execute_shell concatenates `"<command>" <arg>` and hands it to sh ;
//     shell-quoting of the command path lets paths with spaces survive.
//   - execute_process splits arg on whitespace into argv[]. This is the
//     simplest faithful behavior of the Win32 CreateProcessA path :
//     CreateProcessA takes a single command-line string and lets the
//     child do CRT argv parsing ; on POSIX we do that parsing here.
//
// Implementation uses std::string instead of o_str to keep this TU's
// link footprint hermetic : o_str pulls aaa_str.cpp -> aaa_mem.cpp ->
// mem::b_cpy_use_* + c_cpu::one + spy::sleep, all unported on Mac.
// The vendor build's use of o_str is for nothing more than command-line
// string assembly ; std::string is a faithful substitute for that role
// and isolates this Mac port from the wider memcpy/mem/c_cpu cascade.
//
// Both functions log via DBG_PRINT_STRING which is provided by the
// project's err.cpp (Mac-clean since c50) or by test stubs when used
// inside a test executable.

#include "aaa/execute.h"
//	err.h pulled in transitively by execute.h ; the project's err.h has a
//	single-include `#error` guard, so do NOT include it directly here.

#include <cstdlib>          //	system()
#include <cstring>
#include <string>
#include <vector>
#include <spawn.h>          //	posix_spawn
#include <sys/wait.h>       //	waitpid, WIFEXITED, WEXITSTATUS

extern char** environ;      //	POSIX-mandated, see environ(7)

namespace aaa
{
    namespace
    {
        //	Build a shell command line : `"<command>" <arg>`.
        //	Matches the Windows build_command() shape so debug output is
        //	consistent across platforms.
        void build_shell_command_( std::string& out, char const* command, char const* arg )
        {
            DBG_PRINT_STRING( "  with command : %s", command ? command : "" );
            DBG_PRINT_STRING( "  and arg : %s",     arg     ? arg     : "" );
            out.clear();
            out.push_back( '"' );
            if( command )
                out.append( command );
            out.append( "\" " );
            if( arg )
                out.append( arg );
        }

        //	Whitespace split of `arg` into argv tokens.
        void split_whitespace_( char const* s, std::vector< std::string >& out )
        {
            if( !s )
                return;
            std::string token;
            for( char const* p = s; *p; ++p )
            {
                if( *p == ' ' || *p == '\t' )
                {
                    if( !token.empty() )
                    {
                        out.push_back( token );
                        token.clear();
                    }
                }
                else
                {
                    token.push_back( *p );
                }
            }
            if( !token.empty() )
                out.push_back( token );
        }
    }

    AAA_ERR execute_shell( C_PCHAR_C command, C_PCHAR_C arg )
    {
        DBG_PRINT_STRING( "Will call system (POSIX)" );

        std::string str_execute;
        build_shell_command_( str_execute, command, arg );

        DBG_PRINT_STRING( "Will call system with %s", str_execute.c_str() );

        INT32 const ret = ::system( str_execute.c_str() );
        //	POSIX system() returns -1 on fork/wait error, or the wait
        //	status of the child shell. We map "child exited 0" to OK.
        if( ret == -1 )
            return ERR_ANY;
        if( WIFEXITED( ret ) && WEXITSTATUS( ret ) == 0 )
            return AAA_OK;
        return ERR_ANY;
    }

    AAA_ERR execute_process( C_PCHAR_C command, C_PCHAR_C arg )
    {
        DBG_PRINT_STRING( "Will call posix_spawn (POSIX)" );

        if( !command || !*command )
            return ERR_ANY;

        //	argv[0] = command, then whitespace-split tokens of `arg`.
        std::vector< std::string > argv_storage;
        argv_storage.push_back( command );
        split_whitespace_( arg, argv_storage );

        std::vector< char* > argv;
        argv.reserve( argv_storage.size() + 1 );
        for( auto& s : argv_storage )
            argv.push_back( s.data() );
        argv.push_back( nullptr );

        DBG_PRINT_STRING( "Will call posix_spawn with %s (argc=%d)",
            command, static_cast< int >( argv_storage.size() ) );

        pid_t pid = 0;
        int const spawn_ret = ::posix_spawn(
                                  &pid,
                                  command,
                                  nullptr,      //	file_actions
                                  nullptr,      //	attrp
                                  argv.data(),
                                  environ );
        if( spawn_ret != 0 )
            return ERR_ANY;

        //	Mirror the Win32 path : the original closes its handles right
        //	after CreateProcessA returns success. POSIX equivalent : avoid
        //	zombies by waitpid()ing here. If a future caller needs
        //	fire-and-forget, that's a new API.
        int status = 0;
        if( ::waitpid( pid, &status, 0 ) == -1 )
            return ERR_ANY;
        if( WIFEXITED( status ) && WEXITSTATUS( status ) == 0 )
            return AAA_OK;
        return ERR_ANY;
    }
}
