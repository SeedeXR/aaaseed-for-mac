#ifndef GC_FACTORY_PLATFORM_H_
#define GC_FACTORY_PLATFORM_H_

#ifdef WIN32
  #define WIN32_LEAN_AND_MEAN
  #pragma warning( disable: 4251 )
  #include <windows.h>
#endif

#include <Base/GCBase.h>

#ifdef WIN32
  #ifdef FACTORY_EXPORTS
    #define  GCFC_IMPORT_EXPORT __declspec(dllexport)
    #pragma message("GC_FACTORY_PLATFORM: using Export lib")   // <== add this line
  #else
    #define GCFC_IMPORT_EXPORT __declspec(dllimport)
    #pragma message("GC_FACTORY_PLATFORM: using Import lib")
    #ifdef _DEBUG
      #pragma message("GC_FACTORY_PLATFORM: using debug library Jai_Factory_d.lib ")
      #pragma comment (lib, "Jai_Factory_d.lib")
    #else
      #pragma message("using release library Jai_Factory.lib ")
      #pragma comment (lib, "Jai_Factory.lib")
    #endif
  #endif
  
  #ifdef __cplusplus
    #ifndef EXTERN_C
      #define EXTERN_C extern "C"
    #endif
  #else
        #define EXTERN_C
  #endif

  #define GCFC_CALLTYPE __stdcall
#elif __linux__
  #define GCFC_IMPORT_EXPORT
#endif

#endif //GC_FACTORY_PLATFORM_H_
