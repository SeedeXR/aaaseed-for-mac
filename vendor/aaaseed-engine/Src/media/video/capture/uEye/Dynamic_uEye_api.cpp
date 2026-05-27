// ############################################################################
//
//  Project:  CuEyeDll Dynymic loading of uEye DLL
//
//  Module:    Dynamic_uEye_api.cpp  
//
//  Target:    Win32    Win NT
//
// ----------------------------------------------------------------------------
//  History:  Ed   When   Who   What                          in Version
//  -------- ---- ------- ---- ------------------------------ -----------------
//
// ----------------------------------------------------------------------------
//
// ############################################################################

//////////////////////////////////////////////////////////////////////
#include "Dynamic_uEye_api.h"
#include <tchar.h>

CuEyeDll::CuEyeDll()
{
    m_hMod = nullptr;
}


CuEyeDll::~CuEyeDll()
{
    Disconnect();
}


bool CuEyeDll::IsLoaded()
{
    return m_hMod;
}


bool CuEyeDll::_SetPointers(bool Load)
{
    int tried  = 0;
    int made   = 0;

#define UEYE_MACRO_DO(name) tried++;if (!Load) \
    { \
    ++made; \
    is_##name=nullptr; \
    } \
                       else \
    { \
    if ((is_##name=(IS__##name)GetProcAddress(m_hMod,"is_"#name))!=nullptr) \
    ++made; \
    }
#define DECLARE(pars)

#include "uEye_api_Macro.h"

#undef DECLARE
#undef UEYE_MACRO_DO

    return tried==made;
}


long CuEyeDll::Connect(const TCHAR* dllname)
{
    long lRet = ICB_SUCCESS;

    if( dllname )
    {
        if( _tcslen(dllname) != 0 )
        {
            m_hMod = LoadLibrary(dllname);

            if( m_hMod )
            {                       
                if( !_SetPointers(true) )
                {
                    Disconnect();
                    lRet = ICB_NO_SUCCESS;
                }
            }
            else
                lRet = ICB_NO_SUCCESS;
        }
        else
            lRet = ICB_NO_SUCCESS;
    }
    else
        lRet = ICB_NO_SUCCESS;

    return lRet;
}


long CuEyeDll::Disconnect()
{
	_SetPointers(false);
	if( m_hMod )
	{
		FreeLibrary(m_hMod);
		m_hMod = nullptr;
	}
	return ICB_SUCCESS;
}
