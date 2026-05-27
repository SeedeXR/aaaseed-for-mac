# Link search paths and link options for the AAASeed target.
# Replicates the vcxproj <LibraryPath> + <Link><AdditionalOptions> for parity.


# Library search paths.
# The codebase uses AAA_LIB_USE_*("name") macros (lib_use.h) which expand to relative paths
# like "./lib_x64/<name>.lib". Those resolve relative to the linker's working directory --
# which is the .vcxproj location. The CMake-generated .vcxproj lives in out/cmake-build/,
# so we explicitly add the project root and the boost lib dir to the linker search path so
# every relative path the project uses still resolves.
target_link_directories(AAASeed PRIVATE
    ${CMAKE_SOURCE_DIR}                         # for "./lib_x64/..." paths from AAA_LIB_USE_MESSAGE
    ${CMAKE_SOURCE_DIR}/Lib_x64/boost           # boost libs (when targeting x64)
)
if(DEFINED ENV{AAASeed_Path_CUDA})
    target_link_directories(AAASeed PRIVATE $ENV{AAASeed_Path_CUDA}/lib)
endif()


# Link options replicate the vcxproj's <Link><AdditionalOptions>.
# /NODEFAULTLIB:libc/libcmt/libcd/libcmtd = avoid CRT name clashes from third-party static libs
# (Boost, OpenCV, Spout) that were built against the static CRT. The runtime DLL is still pulled
# in via msvcrt.lib (or msvcrtd.lib for Debug) which is the stub that points to ucrt + vcruntime
# on modern MSVC. We deliberately do NOT exclude msvcrt.lib here, even though the vcxproj does:
# on VS2015+ that exclusion would also block its transitive /defaultlib references to ucrt.lib
# and vcruntime.lib, leaving CRT symbols (atan2f, _Init_thread_*, __CxxFrameHandler*, ...)
# unresolved. TODO Phase F : revisit if name clashes appear from the static libs we pull in.
# /LARGEADDRESSAWARE = needed for >2GB virtual memory on x64.
target_link_options(AAASeed PRIVATE
    /NODEFAULTLIB:libc.lib
    /NODEFAULTLIB:libcmt.lib
    /NODEFAULTLIB:libcd.lib
    /NODEFAULTLIB:libcmtd.lib
    /LARGEADDRESSAWARE
)
