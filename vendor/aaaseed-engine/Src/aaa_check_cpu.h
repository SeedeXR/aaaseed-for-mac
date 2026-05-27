#ifdef AAA_AAA_CHECK_CPU_H
#error "AAA_CHECK_CPU_H included more than once."
#endif
#define AAA_AAA_CHECK_CPU_H 1


// AAA_BUILD_REQUIRES_AVX2() expands to 1 on configs that need AVX2 (Debug/Metal/
// Release) and 0 on Wood. The actual 0/1 value is passed by CMake via
// AAA_BUILD_REQUIRES_AVX2_VALUE (see cmake/defines.cmake). Match the project's
// AAASEED() / AAA_DEBUG() / AAA_TRACKER() function-style feature-flag convention.
#define AAA_BUILD_REQUIRES_AVX2() AAA_BUILD_REQUIRES_AVX2_VALUE


// AVX2 runtime check. Call as the first statement in main() on builds compiled
// with /arch:AVX2 (Debug, Metal, Release) ; if the CPU lacks AVX2, the function
// shows a MessageBox pointing the user to AAASeed_Wood.exe and ExitProcess(1).
//
// The implementation in aaa_check_cpu.cpp is force-compiled with /arch:SSE2 so
// the check itself never emits an AVX2 instruction -- otherwise the binary
// would crash before the message could appear.
extern void aaa_check_avx2_or_exit();
