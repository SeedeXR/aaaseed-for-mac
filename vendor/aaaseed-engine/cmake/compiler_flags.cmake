# Per-config compiler flags. Mirrors the vcxproj's Metal_v145|x64, Wood_v145|x64 and
# Release_v145|x64 ItemDefinitionGroups (plus the per-config PropertyGroup that enables
# WholeProgramOptimization for those three).


if(MSVC)
    # /std:c++latest = matches the vcxproj LanguageStandard "stdcpplatest". Includes preview
    # C++ features that the codebase uses (std::chrono::clock_cast in aaa_file.cpp, ...).
    target_compile_options(AAASeed PRIVATE /std:c++latest)


    # /MP   = parallel compilation, one cl.exe per logical core.
    # /Zm300 = heap allocation factor at 300% of default. Big enough for the heaviest
    #         template TUs (dlib, GLM) without the extreme cost of /Zm1000. Applied to all
    #         configs including Debug so iteration builds are also parallel.
    # /bigobj = lift the 65,536 section limit on .obj files. Required by heavy template TUs
    #          like Src/image/convert/img_convert_generic.cpp (cascaded instantiations).
    target_compile_options(AAASeed PRIVATE /MP /Zm300 /bigobj)


    # Debug : Edit & Continue + AVX2 + 16-byte struct alignment.
    #
    #   /ZI         (capital I) emits PDB in the format required for E&C ; without it VS
    #               shows "Hot Reload not supported by project" when you edit on a breakpoint.
    #   /Zp16       struct member alignment, mandatory for SIMD types in the math layer.
    #               Must match Metal/Wood/Release or struct layouts drift between configs
    #               -- catastrophic for any header-shared struct containing __m128 / __m256.
    #   /arch:AVX2  match Metal/Release ISA so intrinsic-using code paths behave the same
    #               under Debug as under release builds.
    #   /Od /Ob0    no optimisation, no inlining (the actual debug semantics).
    #   /RTC1       runtime stack-frame + uninitialized-var checks ; catches stack overruns
    #               like the pre-existing MapWindowPoints cPoints=2 bug in SystemEventReader.
    #   /INCREMENTAL on the linker side keeps incremental link active so the patch-in-place
    #               workflow is fast.
    # Note : no /Gy here. Mâa wants i-cache locality even in Debug (function grouping per .cpp).
    set(CMAKE_CXX_FLAGS_DEBUG "/MDd /ZI /Zp16 /arch:AVX2 /Od /Ob0 /RTC1" CACHE STRING "" FORCE)
    set(CMAKE_C_FLAGS_DEBUG   "/MDd /ZI /Zp16 /arch:AVX2 /Od /Ob0 /RTC1" CACHE STRING "" FORCE)
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "/INCREMENTAL /DEBUG" CACHE STRING "" FORCE)


    # Optim baseline shared by Metal / Wood / Release.
    #
    #   /MD     Multi-threaded DLL runtime (RuntimeLibrary=MultiThreadedDLL)
    #   /O2     MaxSpeed
    #   /Ob3    InlineFunctionExpansion=AnySuitable+. Aggressive inline policy.
    #   /Oi     IntrinsicFunctions=true
    #   /Ot     FavorSizeOrSpeed=Speed
    #   /Oy     OmitFramePointers=true
    #   /GS-    BufferSecurityCheck=false
    #   /Gy-    no function-level linking (preserves i-cache locality, see root CLAUDE.md)
    #   /fp:fast    FloatingPointModel=Fast
    #   /Zp16   StructMemberAlignment=16Bytes
    #   /Qpar   parallel auto-vectorisation of loops
    set(_AAA_OPTIM_BASE "/MD /O2 /Ob3 /Oi /Ot /Oy /GS- /Gy- /fp:fast /Zp16 /Qpar")


    # Metal / Wood : full whole-program optimisation. /GL pairs with /LTCG on the link side
    # for cross-TU inline + devirtualisation. Link is slow (minutes), runtime gain ~10-20%.
    # No /Zi : combining /Zi with /GL on the v145 toolset triggers MSVC C1001 ICE on
    # Src/obj_ui/bdd/bdd_edit/bdd_curve3d.cpp:2018 during the link codegen pass.
    set(CMAKE_CXX_FLAGS_METAL "${_AAA_OPTIM_BASE} /GL /arch:AVX2"
        CACHE STRING "C++ flags for Metal" FORCE)
    set(CMAKE_C_FLAGS_METAL   "${_AAA_OPTIM_BASE} /GL /arch:AVX2"
        CACHE STRING "C flags for Metal"   FORCE)
    set(CMAKE_CXX_FLAGS_WOOD  "${_AAA_OPTIM_BASE} /GL /arch:SSE2"
        CACHE STRING "C++ flags for Wood"  FORCE)
    set(CMAKE_C_FLAGS_WOOD    "${_AAA_OPTIM_BASE} /GL /arch:SSE2"
        CACHE STRING "C flags for Wood"    FORCE)


    # Release : optimised but no /GL / no LTCG -- link in seconds, not minutes. /Zi produces
    # a PDB so VS can attach a debugger with readable stacks at full speed. This is the
    # "fast iterate but optimised" config used to debug bugs that only repro under release
    # speed. AVX2 to match Metal so behaviour matches the actual user-facing build.
    set(CMAKE_CXX_FLAGS_RELEASE "${_AAA_OPTIM_BASE} /Zi /arch:AVX2"
        CACHE STRING "" FORCE)
    set(CMAKE_C_FLAGS_RELEASE   "${_AAA_OPTIM_BASE} /Zi /arch:AVX2"
        CACHE STRING "" FORCE)


    # Linker flags.
    #   Metal / Wood : /LTCG pairs with /GL, /OPT:REF/ICF for size, /INCREMENTAL:NO required
    #                  by /LTCG.
    #   Release      : no /LTCG, /DEBUG to emit a PDB next to the exe so the debugger can
    #                  resolve symbols. /OPT:REF /OPT:ICF still strip dead code.
    set(_AAA_LINK_LTCG  "/INCREMENTAL:NO /LTCG /OPT:REF /OPT:ICF")
    set(_AAA_LINK_RELEASE "/INCREMENTAL:NO /DEBUG /OPT:REF /OPT:ICF")
    set(CMAKE_EXE_LINKER_FLAGS_METAL    "${_AAA_LINK_LTCG}"    CACHE STRING "" FORCE)
    set(CMAKE_EXE_LINKER_FLAGS_WOOD     "${_AAA_LINK_LTCG}"    CACHE STRING "" FORCE)
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE  "${_AAA_LINK_RELEASE}" CACHE STRING "" FORCE)


    unset(_AAA_OPTIM_BASE)
    unset(_AAA_LINK_LTCG)
    unset(_AAA_LINK_RELEASE)
endif()
