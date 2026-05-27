#include "MediapipeHolisticTrackingDll.h"
#include "file/aaa_file.h"

MediapipeHolisticTrackingDll::MediapipeHolisticTrackingDll()
{
	HolisticTrackingInit = nullptr;
	HolisticTrackingDetectFrameDirect = nullptr;
	HolisticTrackingRelease = nullptr;
}

MediapipeHolisticTrackingDll::~MediapipeHolisticTrackingDll()
{
	if (m_DynamicModuleLoader.GetDynamicModuleState())
	{
		m_DynamicModuleLoader.UnloadDynamicModule();
	}
}

bool MediapipeHolisticTrackingDll::LoadMediapipeHolisticTrackingDll( C_PCHAR_C dll_path )
{
//	if( c_file::is_exist( dll_path ) )
	{
		if( m_DynamicModuleLoader.LoadDynamicModule( dll_path ) )
			return true;
	}

	return false;
}

bool MediapipeHolisticTrackingDll::UnLoadMediapipeHolisticTrackingDll()
{
	if (m_DynamicModuleLoader.UnloadDynamicModule())
	{
		return true;
	}

	return false;
}

bool MediapipeHolisticTrackingDll::GetAllFunctions()
{
	if( m_DynamicModuleLoader.GetDynamicModuleState() )
	{
		void* p_fn;
			
		p_fn = m_DynamicModuleLoader.GetFunction("MediapipeHolisticTrackingInit");
		if( p_fn )
			HolisticTrackingInit = (FuncMediapipeHolisticTrackingInit)(p_fn);
		else
			return false;

		p_fn = m_DynamicModuleLoader.GetFunction("MediapipeHolisticTrackingDetectFrameDirect");
		if( p_fn )
			HolisticTrackingDetectFrameDirect = (FuncMediapipeHolisticTrackingDetectFrameDirect)(p_fn);
		else
			return false;

		p_fn = m_DynamicModuleLoader.GetFunction("MediapipeHolisticTrackingDetectCamera");
		if( p_fn )
			HolisticTrackingDetectCamera = (FuncMediapipeHolisticTrackingDetectCamera)(p_fn);
		else
			return false;

		p_fn = m_DynamicModuleLoader.GetFunction("MediapipeHolisticTrackingRelease");
		if( p_fn )
			HolisticTrackingRelease = (FuncMediapipeHolisticTrackingRelease)(p_fn);
		else
			return false;
	}

	return true;
}
