#include "mediapipe.h"

//#include <lib_use.h>
//AAA_LIB_USE( "OpenVR/openvr_api" )

#include "infrastructure/param/param_declare.h"
#include "file/aaa_dir.h"

//#include <opencv2/core/core.hpp> 
//#include <opencv2/highgui/highgui.hpp> 
//#include <opencv2/opencv.hpp>

namespace {

}

FACTORY_CREATE_V1( c_mediapipe, mediapipe, mediapipe, mediapipe );

c_mediapipe*		g_mediapipe = nullptr;

bool	c_mediapipe::b_start_with				= false;
bool	c_mediapipe::b_dll_loaded				= false;
bool	c_mediapipe::b_verbose					= false;
bool	c_mediapipe::b_holistic_model_loaded	= false;


MediapipeHolisticTrackingDll	c_mediapipe::dll;

namespace	n_mediapipe
{
	CONSTEXPR INT32 CTL_PARAM_NB	= 0;
	CONSTEXPR INT32 BASE_PARAM_NB	= 4 + CTL_PARAM_NB;
	CONSTEXPR INT32 GROUP_PARAM_NB	= 0;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_OFF(		active					)
		PARAM_DEF_BOOL_LOCKED(	dll_loaded				)
		PARAM_DEF_BOOL_LOCKED(	holistic_model_loaded	)
		PARAM_DEF_BOOL_OFF(		verbose					)
	};
}

void	c_mediapipe::param_init_pt()
{
	INT32	h=0;
	param_set_pt( h, get_pt_active()			);
	param_set_pt( h, b_dll_loaded				);
	param_set_pt( h, b_holistic_model_loaded	);
	param_set_pt( h, b_verbose					);


	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_mediapipe)
{
	param_init_with( n_mediapipe::param, n_mediapipe::PARAM_NB_MAX );
}

c_mediapipe::~c_mediapipe()
{
}

AAA_ERR	c_mediapipe::open()
{
	return AAA_OK;
}
void	c_mediapipe::close()
{
}
void	c_mediapipe::update_low()
{
	if( !is_active() )
		return;

	if( b_start_with && !b_dll_loaded )
		c_init();
}

void c_mediapipe::c_init()
{
	if( b_dll_loaded )
		return;

#if AAA_DEBUG()
	C_PCHAR_C path = "C:/MediaPipe2022/GoogleMediapipePackageDll-main/dll_use_example/MediapipePackageDllTest/bin/MediapipeTest/x64/Debug";
#else
	C_PCHAR_C path = "C:/MediaPipe2022/GoogleMediapipePackageDll-main/dll_use_example/MediapipePackageDllTest/bin/MediapipeTest/x64/Release";
#endif

	c_dir::push_def( path );

//		std::string dll_path = ".././bin/MediapipeTest/x64/Debug/MediapipeHolisticTracking.dll";
		//"M:/AAA/AAAGaBuZo_Dll/Main_x64/MediapipeHolisticTracking.dll"
//		C_PCHAR_C dll_path = "C:/MediaPipe2022/GoogleMediapipePackageDll-main/dll_use_example/MediapipePackageDllTest/bin/MediapipeTest/x64/Debug/MediapipeHolisticTracking.dll";
		C_PCHAR_C dll_path = "MediapipeHolisticTracking";
		
		if( !dll.LoadMediapipeHolisticTrackingDll( dll_path ) )
			goto pop_and_return;
		if( !dll.GetAllFunctions() )
			goto pop_and_return;
		b_dll_loaded = true;

//#ifdef _DEBUG
//	std::string mediapipe_holistic_tracking_model_path = ".././bin/MediapipeTest/x64/Debug/holistic_tracking_cpu.pbtxt";
//#else
//	std::string mediapipe_holistic_tracking_model_path = ".././bin/MediapipeTest/x64/Release/holistic_tracking_cpu.pbtxt";
//#endif // _DEBUG

//		c_dir::push_def( "../../AAAGaBuZo_Dll/Main_x64" );
//		c_dir::push_def( "M:/AAA/AAAGaBuZo_Dll/Main_x64/" );
		


		//todo have a more definitive strategy on these files
//		C_PCHAR_C model_path = "M:/AAA/AAAGaBuZo_Dll/Main_x64/holistic_tracking_cpu.pbtxt";
	
		{
			C_PCHAR_C model_path = "holistic_tracking_cpu.pbtxt";
			if( dll.HolisticTrackingInit( model_path, false, true, true, true ) )
			{
				GOOD_PRINT_STRING( "Mediapipe : Holistic tracking model loaded %s", model_path );
				b_holistic_model_loaded = true;
			}
			else
			{
				ERR_PRINT_STRING( "Mediapipe : Failed to load Holistic tracking model %s", model_path );
			}
		}

pop_and_return:
	c_dir::pop_def();
}

void c_mediapipe::c_deinit()
{
	if( b_holistic_model_loaded )
	{
		//todo unload
		b_holistic_model_loaded = false;
	}
	if( b_dll_loaded )
	{
		//todo release lib
		b_dll_loaded = false;
	}
}
