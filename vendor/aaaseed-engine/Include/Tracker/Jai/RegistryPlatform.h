#ifndef GC_REGISTRY_PLATFORM_H_
#define GC_REGISTRY_PLATFORM_H_

#define WIN32_LEAN_AND_MEAN
#pragma warning( disable: 4251 )

#include <Base/GCBase.h>

#ifdef WIN32
  #include <windows.h>
#endif

#ifdef WIN32
  #ifdef REGISTRY_EXPORTS 
    #define  GCRC_IMPORT_EXPORT __declspec(dllexport)
    #pragma message("using Export lib")   
  #else
    #define GCRC_IMPORT_EXPORT __declspec(dllimport)
    #pragma message("GC_REGISTRY_PLATFORM: using Import lib")
    #ifdef _DEBUG
      #pragma message("GC_REGISTRY_PLATFORM: using debug library Jai_Registry_d.lib ")
      #pragma comment (lib, "Jai_Registry_d.lib")
    #else
      #pragma message("GC_REGISTRY_PLATFORM: using release library Jai_Registry.lib ")
      #pragma comment (lib, "Jai_Registry.lib")
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
  #define GCRC_IMPORT_EXPORT
#endif

#endif //GC_REGISTRY_PLATFORM_H_
