#include "magellan.h"

#if	AAA_TRACKER_MAGELLAN()

#include "aaa_os.h"
#include "draw/aaa_glut.h"
#include "mgldll.h"
#include "aaaseed.h"
#include "infrastructure/param/param_declare.h"
#include "infrastructure/aaa_window.h"


FACTORY_CREATE_V1( c_magellan, magellan, Magellan 3D Controller, magellan );

c_magellan*	magellan = nullptr;

static	REAL CONST	ScaleRotation		= 1/512.;
static	REAL CONST	ScaleTranslation	= 1/512.;


//---------------- Magellan 3D Controller Handle -------------------
static	HMAGELLAN MagellanHandle = nullptr;
static	INT32	b_first = 100;

void	magellan_connect_to_window()
{
	if( MagellanHandle )
		MagellanSetWindow( MagellanHandle, get_window_main_handle() );
}

LRESULT
WINAPI
magellan_win_proc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	//MagellanTypeConversion MagellanType;
	MagellanIntegerEvent MagellanData;
	MSG MagellanMessage;
	int MagellanRepaint;
	double a,b,c,x,y,z;

	if( b_first < 100)
	{
		magellan_connect_to_window();
		++b_first;
	}
	MagellanRepaint = FALSE;

/*	switch( Msg)
		{
		default:
*/
	MagellanMessage.hwnd = hWnd;
	MagellanMessage.message = Msg;
	MagellanMessage.wParam = wParam;
	MagellanMessage.lParam = lParam;

	switch( MagellanTranslateEvent( MagellanHandle, &MagellanMessage, &MagellanData ) )
		{ 
		case MotionEvent :
			x = MagellanData.MagellanData[0]*ScaleTranslation;
			y = MagellanData.MagellanData[1]*ScaleTranslation;
			z = MagellanData.MagellanData[2]*ScaleTranslation;
			a = MagellanData.MagellanData[3]*ScaleRotation;
			b = MagellanData.MagellanData[4]*ScaleRotation;
			c = MagellanData.MagellanData[5]*ScaleRotation;
			MagellanRepaint = TRUE;
			break;

		case ButtonPressEvent: 
			switch( MagellanData.MagellanButton )
			{
			case 1:
			case 2:
			case 3:
				magellan->_sensor.set_button( MagellanData.MagellanButton - 1, true );
				break;
			case 5:
				//Sensitivity /= 2.0;
					
				//MagellanType.Float = (float) Sensitivity;
				//MagellanInfoWindow( MagellanHandle, MagellanApplicationSensitivity, MagellanType.lParam );
				MagellanInfoWindow( MagellanHandle, MagellanRingBell, 125 );
				break;
			case 6:
				//Sensitivity *= 2.0;

				//MagellanType.Float = (float) Sensitivity;
				//MagellanInfoWindow( MagellanHandle, MagellanApplicationSensitivity, MagellanType.lParam );
				MagellanInfoWindow( MagellanHandle, MagellanRingBell, 125 );
				break;
			case 7:
				//Sensitivity = 1.0;

				//MagellanType.Float = (float) Sensitivity;
				//MagellanInfoWindow( MagellanHandle, MagellanApplicationSensitivity, MagellanType.lParam );
				MagellanInfoWindow( MagellanHandle, MagellanRingBell, 125 );
				break;
			case 4: 
				//a=B=c=x=y=z=0.0; z=-10.0;
//						MathFrameTranslation( &FrameCube, x, y, z );
//						MathFrameRotation( &FrameCube, a, B, c );
				//MagellanRepaint = TRUE;

				MagellanInfoWindow( MagellanHandle, MagellanRingBell, 125 );
				break;
			};
			break;

		case ButtonReleaseEvent: 
			switch( MagellanData.MagellanButton )
				{
				case 1:
				case 2:
				case 3:
					magellan->_sensor.set_data_in( MagellanData.MagellanButton - 1, FALSE );
					break;
				};
			break;

		default:
			//todo
			//printf( "intercepted message %d\n", Msg);
			return( DefWindowProc( hWnd, Msg, wParam, lParam ) );
			break;
		}
/*
			break;
		}
*/
	if( MagellanRepaint )
	{
		magellan->_sensor.set_tra_in( -x, -y, -z );
		magellan->_sensor.set_rot_in( -a, -b, -c );
//		printf( "Magellan A %f, B %f, C %f\n", a, B, c );
//		printf( "Magellan X %f, Y %f, Z %f\n", x, y, z );
	}
	return TRUE;
}

AAA_ERR	magellan_init()
{
#if !AAA_NEW_DESIGN()
	trackers::PRINT_STRING( "# Magellan ", "Opening(Cyberpuck)" );

//	set_window_main();

	HWND hWnd = get_window_main_handle();
	MagellanHandle = MagellanInit( hWnd );
	if( !MagellanHandle )
	{
		ERR_PRINT_STRING( "Can't find a Magellan 3D Driver or Controller" );
//		MessageBox( hWnd, "Can't find a Magellan 3D Controller", 
//                        "ERROR", MB_OK | MB_ICONEXCLAMATION | MB_TOPMOST); 
		return	ERR_ANY;
	};

	MagellanInfoWindow(MagellanHandle, MagellanApplicationStarts, CUBE3D );
	MagellanInfoWindow(MagellanHandle, MagellanRingBell, 250 );

//	MagellanInfoWindow(MagellanHandle, MagellanModeChange, TRUE );
//	MagellanInfoWindow(MagellanHandle, MagellanNullRadiusChange, TRUE );
//	MagellanInfoWindow(MagellanHandle, MagellanControlPanel, TRUE );

	MagellanInfoWindow(MagellanHandle, MagellanRingBell, 500 );

	maaglut_set_win_proc( magellan_win_proc );

	trackers::PRINT_STRING( "# Magellan", "(Cyberpuck) opened" );
	return	AAA_OK;
#else
	debug_break( "%s() need implementation", __FUNCTION__ );
	return	ERR_UNIMPLEMENTED_YET;
#endif
}   

void	magellan_deinit()
{
	if( MagellanHandle )
		MagellanClose( MagellanHandle );
}

namespace	n_magellan
{
	CONSTEXPR INT32	BASE_PARAM_NB	= 3;
	CONSTEXPR INT32	GROUP_PARAM_NB	= 0;
	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_OFF( active )
		PARAM_DEF_BOOL_OFF( open )
		PARAM_DEF_REAL_ZERO( controller_1 )
	};
}

void	c_magellan::param_init_pt()
{
	INT32	h=0;
	param_set_pt( h, get_pt_active() );
	param_set_pt( h, _b_open_ui );
	param_attach_obj( h, (c_obj_ui*) &_sensor );

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_magellan)
{
	param_init_with( n_magellan::param, n_magellan::PARAM_NB_MAX ); // magellan_param, MAGELLAN_PARAM_NB );
	_sensor.set_root( this );
}

c_magellan::~c_magellan()
{
	close();
}

AAA_ERR	c_magellan::open()	
{
	return magellan_init();
}

void	c_magellan::close()
{	
	magellan_deinit();
}

void	c_magellan::update_low()
{
	_sensor.update_sensor_6dof();
}

AAA_ERR	c_magellan::load_do_after( o_str CONST & filename_in )
{
	o_str& filename = o_str::push_name();
		make_name( filename, filename_in, 0 );
		AAA_ERR retcode = _sensor.load_from_file_add_ext( filename );
	o_str::pop_name();
	return retcode;
}

AAA_ERR	c_magellan::save_do_after( o_str CONST & filename_in )
{
	o_str& filename = o_str::push_name();
		make_name( filename, filename_in, 0 );
		AAA_ERR retcode = _sensor.save_to_file_add_ext( filename );
	o_str::pop_name();
	return retcode;
}

#endif //#if AAA_TRACKER_MAGELLAN()
