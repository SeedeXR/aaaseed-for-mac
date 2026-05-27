# AAASeed_EXE -- CMake build (Windows-only for now ; Mac / Linux will be added later by Louis).
#
# Usage:
#   cmake -G "Visual Studio 17 2022" -B out/cmake-build-v145
#   cmake --build out/cmake-build-v145 --config Release
#
# Toolset switch (must be set BEFORE the project() call -> via cache var on first run):
#   cmake -G "Visual Studio 17 2022" -DAAA_VSTOOL=143 -B out/cmake-build-v143
#   cmake -G "Visual Studio 17 2022" -DAAA_VSTOOL=145 -B out/cmake-build-v145
#
# This file is intentionally a thin shell on top of cmake/*.cmake so the root
# stays scannable. Per-subsystem source lists live in Src/<subsys>/sources.cmake
# and are include()-ed below.


cmake_minimum_required(VERSION 3.21)


#--------------------------------------------------------------------------------------------------
# Toolset selection BEFORE project()
#--------------------------------------------------------------------------------------------------
# AAA_VSTOOL drives which MSVC toolset CMake passes to the Visual Studio generator.
# 145 = Visual Studio 2026 preview, 143 = Visual Studio 2022.
# Default 145 to match what Mâa runs locally.
set(AAA_VSTOOL "145" CACHE STRING "MSVC toolset version (143 or 145)")
set_property(CACHE AAA_VSTOOL PROPERTY STRINGS "143" "145")

if(NOT DEFINED CMAKE_GENERATOR_TOOLSET OR CMAKE_GENERATOR_TOOLSET STREQUAL "")
    if(AAA_VSTOOL STREQUAL "143")
        set(CMAKE_GENERATOR_TOOLSET "v143" CACHE STRING "Toolset selected via AAA_VSTOOL" FORCE)
    elseif(AAA_VSTOOL STREQUAL "145")
        set(CMAKE_GENERATOR_TOOLSET "v145" CACHE STRING "Toolset selected via AAA_VSTOOL" FORCE)
    else()
        message(FATAL_ERROR "AAA_VSTOOL must be 143 or 145, got '${AAA_VSTOOL}'")
    endif()
endif()


#--------------------------------------------------------------------------------------------------
# Multi-config setup BEFORE project()
#--------------------------------------------------------------------------------------------------
# We override the default Debug;Release;MinSizeRel;RelWithDebInfo with the four configs
# actually used in the AAASeed vcxproj : Debug, Metal, Release, Wood.
# Metal = AVX2 release. Release = SSE2 release. Wood = release with GLM_FORCE_XYZW_ONLY.
set(CMAKE_CONFIGURATION_TYPES "Debug;Metal;Release;Wood" CACHE STRING "" FORCE)


#--------------------------------------------------------------------------------------------------
# Project
#--------------------------------------------------------------------------------------------------
project(AAASeed CXX)

# The vcxproj uses /std:c++latest (LanguageStandard = stdcpplatest) on every config so the
# code can rely on recent additions like std::chrono::clock_cast, jthread, ranges, etc.
# Set CMAKE_CXX_STANDARD to 23 as a baseline and override with /std:c++latest below.
set(CMAKE_CXX_STANDARD          23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)


#--------------------------------------------------------------------------------------------------
# Generated header: aaa_sdk_version.generated.h
#--------------------------------------------------------------------------------------------------
# Replicates the MSBuild target GenerateAaaSdkVersionHeader at the end of the vcxproj.
# Substitutes @CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION@ at configure time.
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/license/aaa_sdk_version.generated.h.in
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/license/aaa_sdk_version.generated.h
    @ONLY
)


#--------------------------------------------------------------------------------------------------
# Target declaration (sources attached below via include())
#--------------------------------------------------------------------------------------------------
add_executable(AAASeed)


# Make AAASeed the F5 startup target instead of the default ALL_BUILD. F5 then builds
# only AAASeed (and its incremental deps) in the active config, not the whole solution.
# Without this, VS rebuilds ALL_BUILD on every debug launch which is much slower.
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT AAASeed)


# Match the vcxproj <OutputFile>$(AAA_Out)\AAASeed_$(Configuration).exe</OutputFile>.
# AAA_Out is an environment variable that points at the user's runtime location for AAASeed.
# Falls back to a per-config dir under the build tree when the env var is unset (CI, fresh
# clone) so the build always succeeds without external setup.
if(DEFINED ENV{AAA_Out})
    set_target_properties(AAASeed PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY                "$ENV{AAA_Out}"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG          "$ENV{AAA_Out}"
        RUNTIME_OUTPUT_DIRECTORY_METAL          "$ENV{AAA_Out}"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE        "$ENV{AAA_Out}"
        RUNTIME_OUTPUT_DIRECTORY_WOOD           "$ENV{AAA_Out}"
    )
endif()
set_target_properties(AAASeed PROPERTIES
    OUTPUT_NAME_DEBUG       "AAASeed_Debug"
    OUTPUT_NAME_METAL       "AAASeed_Metal"
    OUTPUT_NAME_RELEASE     "AAASeed_Release"
    OUTPUT_NAME_WOOD        "AAASeed_Wood"
)


# F5 launch from Visual Studio uses %AAASeed_DirStart% as the working directory.
# The env var is captured at CMake configure time and written as a resolved path
# into AAASeed.vcxproj's <LocalDebuggerWorkingDirectory>. The literal $(AAASeed_DirStart)
# was tried first but VS does not expand env-var-style MSBuild macros for the working
# directory ; it shows the unresolved string and refuses to launch. Resolving at
# configure time means changing the env var requires a `cmake --preset ...` reconfigure,
# which is fine -- this path changes rarely.
if(DEFINED ENV{AAASeed_DirStart})
    set_target_properties(AAASeed PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "$ENV{AAASeed_DirStart}"
    )
    message(STATUS "  Debug working dir      : $ENV{AAASeed_DirStart}")
else()
    message(STATUS "  Debug working dir      : (env var AAASeed_DirStart not set, F5 will use VS default)")
endif()


#--------------------------------------------------------------------------------------------------
# Force-include of aaa_build_config.h (defines AAASEED(), AAA_NEW_DESIGN(), AAA_TRACKER(),
# MSA_HOST_AAASEED). Equivalent of the vcxproj <ForcedIncludeFiles> entry.
#--------------------------------------------------------------------------------------------------
if(MSVC)
    target_compile_options(AAASeed PRIVATE /FI"${CMAKE_CURRENT_SOURCE_DIR}/Src/aaa_build_config.h")
else()
    target_compile_options(AAASeed PRIVATE -include "${CMAKE_CURRENT_SOURCE_DIR}/Src/aaa_build_config.h")
endif()


#--------------------------------------------------------------------------------------------------
# Defines, compiler flags, external libs (each in its own file under cmake/)
#--------------------------------------------------------------------------------------------------
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/defines.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/compiler_flags.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/include_dirs.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/link_options.cmake)


#--------------------------------------------------------------------------------------------------
# Source enumeration (one sources.cmake per subsystem, attached to the AAASeed target)
#--------------------------------------------------------------------------------------------------
include(${CMAKE_CURRENT_SOURCE_DIR}/Src/sources_root.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Src/aaa/sources.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Src/boids/sources.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Src/code_utils/sources.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Src/core/sources.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Src/draw/sources.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Src/evt/sources.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Src/FBO/sources.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Src/fbx/sources.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Src/flex/sources.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Src/ftgl/sources.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Src/gl/sources.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Src/gol/sources.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Src/Gypsy/sources.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Src/image/sources.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Src/infrastructure/sources.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Src/language/sources.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Src/lib_wrappers/sources.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Src/license/sources.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Src/math/sources.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Src/media/sources.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Src/mocap/sources.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Src/MSA/sources.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Src/net_requests/sources.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Src/obj_ui/sources.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Src/OpenCL/sources.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Src/OpenFramework/sources.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Src/platform/sources.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Src/SdfText/sources.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Src/shaders/sources.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Src/SpoutSDK/sources.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Src/system/sources.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Src/thread/sources.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Src/tinyxml/sources.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Src/tracking/sources.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Src/TrueType/sources.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Src/ui/sources.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/Src/wrapper/sources.cmake)


# Single non-Src/ source file the project compiles : dlib's link-time check.
# Defines USER_ERROR__inconsistent_build_configuration__see_dlib_faq_1/2 which the dlib headers
# emit references to. Without it, anything that uses dlib (face_tracker_clm.cpp, dlib.cpp, ...)
# fails to link with LNK2001 on those symbols.
target_sources(AAASeed PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Include/dlib/test_for_odr_violations.cpp)


# Resource compile : icon, version info, manifest fragments. Equivalent of the vcxproj
# <ResourceCompile Include="Ressource\Seed.rc"/>. CMake handles .rc files automatically on
# MSVC when added to a target's sources.
target_sources(AAASeed PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Ressource/Seed.rc)




#--------------------------------------------------------------------------------------------------
# Force all .c files in the target to compile as C++.
# Reproduces the vcxproj's <CompileAs>CompileAsCpp</CompileAs> setting (set on every config that
# ships with the v143/v145 toolset). Required because vendor C files like
# Src/language/lua/lua_socket/luasocket.h use bare `extern "C" {` blocks (without #ifdef
# __cplusplus guards), which only parse in C++. Compiling them as C++ is also what makes the
# C++ caller (aaalua_util.cpp) and C definition (luasocket.c) link, since both end up with
# C++ name mangling.
#--------------------------------------------------------------------------------------------------
get_target_property(_aaa_sources AAASeed SOURCES)
foreach(_src IN LISTS _aaa_sources)
    if(_src MATCHES "\\.c$")
        set_source_files_properties(${_src} PROPERTIES LANGUAGE CXX)
    endif()
endforeach()
unset(_aaa_sources)


#--------------------------------------------------------------------------------------------------
# Status echo (for sanity at configure time)
#--------------------------------------------------------------------------------------------------
message(STATUS "AAASeed CMake configuration:")
message(STATUS "  Toolset                : ${CMAKE_GENERATOR_TOOLSET} (AAA_VSTOOL=${AAA_VSTOOL})")
message(STATUS "  Configurations         : ${CMAKE_CONFIGURATION_TYPES}")
message(STATUS "  Windows SDK target ver : ${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}")
message(STATUS "  Source dir             : ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "  Binary dir             : ${CMAKE_BINARY_DIR}")
