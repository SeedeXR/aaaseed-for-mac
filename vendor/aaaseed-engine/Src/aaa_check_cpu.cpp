#include "aaa_check_cpu.h"

#include "aaa_os.h"   // pulls <windows.h> with the project's WIN32_LEAN_AND_MEAN policy
#include <intrin.h>


// Detect the full /arch:AVX2 instruction set group via raw CPUID (CPU instruction)
// rather than IsProcessorFeaturePresent (Windows API). The Win32 API queries the OS
// feature table populated from the host CPU at boot, so under Intel SDE emulation
// (`sde64 -snb`) it still reports the host features. Raw CPUID is intercepted by
// SDE and reflects the emulated CPU.
//
// MSVC /arch:AVX2 lets the compiler emit instructions from : AVX, AVX2, FMA, BMI1,
// BMI2, F16C, LZCNT, MOVBE, POPCNT. All real CPUs that have AVX2 (Haswell+, AMD
// Excavator+) also have the rest, but checking each flag explicitly catches every
// SDE profile and any future weird config.
static bool aaa_cpu_has_arch_avx2_set()
{
    int regs[4];
    __cpuid( regs, 0 );
    if( regs[0] < 7 )                       // need leaf 7 for AVX2/BMI flags
        return false;

    __cpuid( regs, 1 );
    int const ecx1 = regs[2];
    if( !(ecx1 & (1 << 28)) )   // AVX
        return false;
    if( !(ecx1 & (1 << 12)) )   // FMA
        return false;
    if( !(ecx1 & (1 << 29)) )   // F16C
        return false;
    if( !(ecx1 & (1 << 22)) )   // MOVBE
        return false;
    if( !(ecx1 & (1 << 23)) )   // POPCNT  
        return false;

    __cpuidex( regs, 7, 0 );
    int const ebx7 = regs[1];
    if( !(ebx7 & (1 << 3)) )    // BMI1
        return false;
    if( !(ebx7 & (1 << 5)) )    // AVX2
        return false;
    if( !(ebx7 & (1 << 8)) )    // BMI2
        return false; 

    __cpuid( regs, 0x80000000 );
    if( (unsigned)regs[0] < 0x80000001 )
        return false;
    __cpuid( regs, 0x80000001 );
    if( !(regs[2] & (1 << 5)) ) // LZCNT (extended leaf)
           return false;

    return true;
}


void aaa_check_avx2_or_exit()
{
    if( aaa_cpu_has_arch_avx2_set() )
        return;

    MessageBoxA( nullptr,
        "This AAASeed build (config " AAA_BUILD_CONFIG_NAME ") requires AVX2.\n"
        "Your CPU does not support AVX2.\n\n"
        "Please use AAASeed_Wood.exe instead.",
        "AAASeed -- No AVX2 n this CPU",
        MB_OK | MB_ICONERROR );

    ExitProcess( 1 );
}


// Pre-main static initializer.
// AAASeed has global instances (e.g. c_math in aaa_math.cpp) whose constructors run as
// static initializers BEFORE main(). On AVX2 builds those constructors emit AVX2
// instructions that crash on a non-AVX2 CPU before our top-of-main() check fires.
//
// We register a function pointer in section .CRT$XIB. The MSVC CRT iterates the C
// initializer table .CRT$XIA -> .CRT$XIZ BEFORE the C++ initializer table
// .CRT$XCA -> .CRT$XCZ, so this pre-init runs ahead of every C++ global ctor
// regardless of which segment they target. /include linker pragma keeps the
// pointer from being stripped by /OPT:REF.
//
// Gated on AAA_BUILD_REQUIRES_AVX2 (set by CMake for Debug / Metal / Release, not Wood)
// so Wood users on a non-AVX2 CPU don't see a "please use Wood" message while already
// running Wood.
#if AAA_BUILD_REQUIRES_AVX2()

extern "C" {
    static int aaa_check_avx2_pre_init();

    #pragma section(".CRT$XIB", long, read)
    __declspec(allocate(".CRT$XIB"))
    int (*aaa_check_avx2_pre_init_ptr)() = aaa_check_avx2_pre_init;

    static int aaa_check_avx2_pre_init()
    {
        aaa_check_avx2_or_exit();
        return 0;
    }
}

#pragma comment(linker, "/include:aaa_check_avx2_pre_init_ptr")

#endif // AAA_BUILD_REQUIRES_AVX2()
