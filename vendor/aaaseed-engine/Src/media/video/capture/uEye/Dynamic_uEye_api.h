//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\///
//###########################################################################//
//#                                                                         #//
//#     IDS - Imaging Development Systems GmbH, All rights reserved         #//
//#                                                                         #//
//###########################################################################//
//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\///

// DynamicuEye.h: interface for the dynamic loading of the ueye_api.dll

#ifndef _DYNAMIC_UEYE_API_H_
#define _DYNAMIC_UEYE_API_H_

#include "uEye.h"

#define ICB_SUCCESS     1
#define ICB_NO_SUCCESS  0

#define UEYE_MACRO_DO(name) typedef INT (__cdecl* IS__##name)
#define DECLARE(pars) pars;
  #include "uEye_api_Macro.h"
#undef DECLARE
#undef UEYE_MACRO_DO


class CuEyeDll
{
public:
  CuEyeDll();
  virtual ~CuEyeDll();

  bool IsLoaded();
  
  // Declare UEYE functions
  #define UEYE_MACRO_DO(name) IS__##name is_##name ;
  #define DECLARE(pars)
   #include "uEye_api_Macro.h"
  #undef DECLARE
  #undef UEYE_MACRO_DO

protected:
  HMODULE m_hMod; // module handle

public:
  long Connect(const TCHAR* dllName);
  long Disconnect();

private:
  bool _SetPointers(bool Load);
};

#endif //_DYNAMIC_UEYE_API_H_


