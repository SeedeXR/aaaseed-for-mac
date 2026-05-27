# Replicates the vcxproj <PreprocessorDefinitions> for the AAASeed target, post pre-cleanup.
# AAASEED(), AAA_NEW_DESIGN(), AAA_TRACKER(), MSA_HOST_AAASEED are NOT here -- they live in
# Src/aaa_build_config.h which is force-included for every TU (see root CMakeLists.txt).


# Platform Windows
target_compile_definitions(AAASeed PRIVATE
    WIN32                               # the project assumes WIN32 is defined (gates <windows.h> in aaa_os.h, etc.)
    NOMINMAX
    UNICODE
    _UNICODE
    _CONSOLE
    DPSAPI_VERSION=1
    WGL_WGLEXT_PROTOTYPES
    GL_GLEXT_PROTOTYPES
    # NOTE: WIN32_LEAN_AND_MEAN and VC_EXTRALEAN deliberately NOT defined globally.
    # aaa_os.h (force-included via aaa_build_config.h chain) checks if WIN32_LEAN_AND_MEAN
    # is undefined and only then defines it AND includes <windows.h>. Defining the macro
    # globally would skip that include, leaving HANDLE / THREAD_PRIORITY_* / etc. undefined
    # in every TU that doesn't already pull <windows.h> via another path. The Debug_v143
    # vcxproj config behaved this way and built fine.
)


# Windows API floor : NOT enforced. Maa's actual day-to-day configs are the v145-suffixed
# vcxproj entries (Debug_v145|x64, Metal_v145|x64, Release_v145|x64, Wood_v145|x64) and none
# of them define _WIN32_WINNT explicitly -- they let the Windows SDK default kick in (currently
# Windows 10+), which exposes the Win8.1+ APIs the codebase calls directly without dynamic
# loading (touch_windows.h's ORIENTATION_PREFERENCE, display_info.cpp's GetDpiForMonitor, ...).
#
# The legacy unsuffixed vcxproj configs (Debug|x64, Release|x64) DO define _WIN32_WINNT=0x0601
# (Windows 7 floor) but they are not actually used in practice and never built end-to-end with
# the current code. The CMake build mirrors the v145 configs.


# Third-party library config
target_compile_definitions(AAASeed PRIVATE
    GLEW_STATIC
    GLUT_STATIC
    POCO_STATIC
    POCO_NO_AUTOMATIC_LIBS
    XML_NS
    XML_DTD
    HAVE_EXPAT_CONFIG_H
    XML_EXPORTS
    Util_EXPORTS
    _USE_MATH_DEFINES
)


# Per-config defines.
# Standard CMake configs (Debug, Release) auto-set _DEBUG / NDEBUG, but our custom configs
# Metal and Wood do not. Set them explicitly so the codebase sees the same NDEBUG it does
# in MSBuild Release/Metal/Wood today.
target_compile_definitions(AAASeed PRIVATE
    $<$<CONFIG:Debug>:_DEBUG;DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
    $<$<CONFIG:Metal>:NDEBUG>
    $<$<CONFIG:Wood>:NDEBUG>
)


# AAA_BUILD_REQUIRES_AVX2_VALUE feeds the AAA_BUILD_REQUIRES_AVX2() macro defined in
# aaa_check_cpu.h. 1 on configs compiled with /arch:AVX2 (Debug/Metal/Release), 0 on
# Wood (/arch:SSE2 which runs on any CPU). The macro gates the pre-main AVX2 check
# in aaa_check_cpu.cpp ; on Wood the static init is compiled out so a Wood user on
# a non-AVX2 CPU does not see a misleading "please use Wood" message while already
# running Wood.
target_compile_definitions(AAASeed PRIVATE
    $<$<CONFIG:Debug>:AAA_BUILD_REQUIRES_AVX2_VALUE=1>
    $<$<CONFIG:Metal>:AAA_BUILD_REQUIRES_AVX2_VALUE=1>
    $<$<CONFIG:Release>:AAA_BUILD_REQUIRES_AVX2_VALUE=1>
    $<$<CONFIG:Wood>:AAA_BUILD_REQUIRES_AVX2_VALUE=0>
)


# AAA_BUILD_CONFIG_NAME : the build config name as a C string literal, baked in at
# compile time. Used by aaa_check_cpu.cpp's failure message so the user sees which
# config they ran (independent of any rename of the .exe).
target_compile_definitions(AAASeed PRIVATE
    AAA_BUILD_CONFIG_NAME="$<CONFIG>"
)


# GLM_FORCE_XYZW_ONLY applied to all configs : disables glm's .rgba / .stpq accessors so only
# .xyzw is allowed. The vcxproj had this on Debug only ; Mâa wants it everywhere for consistent
# accessor semantics across the four builds.
target_compile_definitions(AAASeed PRIVATE GLM_FORCE_XYZW_ONLY)
