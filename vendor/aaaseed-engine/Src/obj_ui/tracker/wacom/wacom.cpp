#ifndef AAA_WACOM_UTIL_H
#	include "wacom_util.h"
#endif
#include "math/v.h"


// main doc is https://developer-docs.wacom.com/display/DevDocs/Windows+Wintab+Documentation

/*
	PK_CONTEXT				0x0001	// reporting context
	PK_STATUS				0x0002	// status bits
	PK_TIME					0x0004	// time stamp
	PK_CHANGED				0x0008	// change bit vector
	PK_SERIAL_NUMBER		0x0010	// packet serial number
	PK_CURSOR				0x0020	// reporting cursor
	PK_BUTTONS				0x0040	// button information
	PK_X					0x0080	// x axis
	PK_Y					0x0100	// y axis
	PK_Z					0x0200	// z axis
	PK_NORMAL_PRESSURE		0x0400	// normal or tip pressure
	PK_TANGENT_PRESSURE		0x0800	// tangential or barrel pressure (use with air brush
	PK_ORIENTATION			0x1000	// orientation info: tilts
	PK_ROTATION				0x2000	// rotation info; 1.1
*/

#define PACKETDATA	( PK_X | PK_Y | PK_Z | PK_BUTTONS | PK_NORMAL_PRESSURE | PK_ORIENTATION | PK_SERIAL_NUMBER | PK_TIME | PK_CONTEXT )
#define PACKETMODE      0
//#define PACKETMODE	PK_BUTTONS	// if we want pkButtons
#include "tracker/wacom/PKTDEF.h"

#include "wacom.h"
#include "infrastructure/param/param_declare.h"
#include "infrastructure/aaa_window.h"

#define CONVERT_TO_DOUBLE(x)   ((double)(INT(x))+((double)FRAC(x)/65536))
#define pi 3.14159265359


FACTORY_CREATE_V1( c_wacom, wacom, wacom, wacom );

c_wacom*		g_wacom					= nullptr;

bool			c_wacom::b_start_with	= false;
//bool			c_wacom::b_verbose		= false;

namespace {
bool			b_lib_open		= false;
INT32			devices_nb		= 0;
}

namespace	n_wacom
{
	CONSTEXPR INT32 BASE_PARAM_NB	= 13;
	CONSTEXPR INT32 OUT_PARAM_NB	= 12;
	CONSTEXPR INT32 TEK_PARAM_NB	= 2;
	CONSTEXPR INT32 GROUP_PARAM_NB	= 2;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	OUT_PARAM_NB
									+	TEK_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_OFF(		active				)
		PARAM_DEF_BOOL_LOCKED(	lib_open			)
		PARAM_DEF_INT32_LOCKED(	devices_nb_at_start )
		PARAM_DEF_BOOL_OFF(		system_use_event	)
		PARAM_DEF_BOOL_OFF(		open				)
		PARAM_DEF_BOOL_OFF(		verbose				)
		PARAM_DEF_BOOL_OFF(		verbose_message		)

		PARAM_DEF_BOOL_LOCKED(	pressure_normal_can )
		PARAM_DEF_BOOL_LOCKED(	tilt_can			)
		PARAM_DEF_BOOL_ON(		flip_uv				)
		PARAM_DEF_XYZ_LOCKED(	size				)

		PARAM_DEF_GROUP( Out, OUT_PARAM_NB )
			PARAM_DEF_INT32_LOCKED(		raw_time			)
			PARAM_DEF_INT32_LOCKED_XYZ(	raw					)
			PARAM_DEF_REAL_LOCKED(		x					)
			PARAM_DEF_REAL_LOCKED(		y					)
			PARAM_DEF_REAL_LOCKED(		z					)
			PARAM_DEF_INT32_LOCKED(		raw_pressure_norma	)
			PARAM_DEF_REAL_LOCKED(		pressure_normal		)
			PARAM_DEF_BOOL_LOCKED(		eraser				)
			PARAM_DEF_XY_LOCKED(		rot					)


		PARAM_DEF_GROUP( TEK, TEK_PARAM_NB )
			PARAM_DEF_INT32_LOCKED(	packet_nb_this_frame	)
			PARAM_DEF_INT32_LOCKED(	packet_nb_processed	)
	};
}

void	c_wacom::param_init_pt()
{
	INT32	h=0;
	param_set_pt(	h, get_pt_active() );
	param_set_pt(	h, b_lib_open				);
	param_set_pt(	h, devices_nb				);
	param_set_pt(	h, _b_system_ui				);
	param_set_pt(	h, _b_open_ui				);
	param_set_pt(	h, _b_verbose_ui			);
	param_set_pt(	h, _b_verbose_message_ui	);
	param_set_pt(	h, _b_pressure_normal_can	);
	param_set_pt(	h, _b_tilt_can				);
	param_set_pt(	h, _b_flip_uv				);
	param_set_pt_3(h, _size					);

	++h;
		param_set_pt(	h, _time_raw );
		param_set_pt_3(h, _pos_raw );
		param_set_pt_3(h, _pos );
		param_set_pt(	h, _pressure_normal_raw );
		param_set_pt(	h, _pressure_normal );
		param_set_pt(	h, _b_eraser );
		param_set_pt(	h, _rot[0] );
		param_set_pt(	h, _rot[1] );

	++h;
		param_set_pt(	h, _packet_nb );
		param_set_pt(	h, _packet_nb_processed );

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_wacom)// Azimuth factor
	,_altitude_factor		{1}
	,_altitude_adjust		{1}
	,_azimuth_factor		{1}
	,_b_tilt_can			{false}
	,_b_pressure_normal_can	{false}
	,_b_eraser				{false}
{
	_stroke_in	= &_stroke[0];
	_stroke_out = &_stroke[1];
	param_init_with( n_wacom::param, n_wacom::PARAM_NB_MAX );
}

c_wacom::~c_wacom()
{
	close();
}


namespace {

	CONSTEXPR C_PCHAR_C	 WACOM_HEADER = "# WACOM ";
	void	WACOM_PRINT_STRING( C_PCHAR_C fmt
		, ... )
	{
		va_list args;
		va_start( args, fmt );
		HEADER_PRINT_STRING_VA( WACOM_HEADER, fmt, args );
		va_end( args );
	}
	void	WACOM_ERR_PRINT_STRING( C_PCHAR_C fmt, ... )
	{
		va_list args;
		va_start( args, fmt );
		ERR_HEADER_PRINT_STRING_VA( WACOM_HEADER, fmt, args );
		va_end( args );
	}
}

void c_wacom::get_axis( AXIS& axis, INT32 axe_def, C_PCHAR_C axe_str, REAL& size )
{
	UINT val_ret = gpWTInfoA( WTI_DEVICES, axe_def, &axis );
	if( val_ret != sizeof( AXIS ) )
	{
		debug_break( "Tablet context don't return %s info", axe_str );
	}
	else
	{
		if( axis.axResolution )
		{
			DOUBLE d_tmp = CONVERT_TO_DOUBLE( axis.axResolution );
			size = REAL( ( axis.axMax - axis.axMin ) / d_tmp );
		}
		else
			size = 0.;
	}
}

static	LOGCONTEXTA	log_ctx;
//PLOGCONTEXT	g_tablet_ctx = {0};

//LONG CONST AAA_RANGE = 0xfffffff;

HCTX c_wacom::init_tablet( HWND hWnd, bool b_system )
{
	// Open default system context so that we can get tablet data
	// in screen coordinates (not tablet coordinates).
	UINT val_ret = gpWTInfoA( WTI_DEFSYSCTX, 0, &log_ctx );
	if( val_ret != sizeof( LOGCONTEXTA ) )
	{
		debug_break( "Tablet context return with wrong size" );
		return nullptr;
	}
	
	if( (log_ctx.lcOptions & CXO_SYSTEM) == 0 )
	{
		debug_break( "Tablet context default don't have CXO_SYSTEM" );
	}

	UINT rate;
	gpWTInfoA( WTI_DEVICES + 0, DVC_PKTRATE, &rate );
	log_ctx.lcPktRate = rate * 2;

	if( b_system )
		// Set option to move system cursor before getting default system context.
		log_ctx.lcOptions |= CXO_SYSTEM;
	else
		log_ctx.lcOptions &= ~CXO_SYSTEM;

	// modify the digitizing region
	strcpy( log_ctx.lcName, "AAASeed" );

	// Here we ask for WT_PACKET messages.
	log_ctx.lcOptions |= CXO_MESSAGES;

	// What data items we want to be included in the tablet packets
	log_ctx.lcPktData = PACKETDATA;

	// Which packet items should show change in value since the last
	// packet (referred to as 'relative' data) and which items
	// should be 'absolute'.
	log_ctx.lcPktMode = PACKETMODE;

	// This bitfield determines whether or not this context will receive
	// a packet when a value for each packet field changes.  This is not
	// supported by the Intuos Wintab.  Your context will always receive
	// packets, even if there has been no change in the data.
	log_ctx.lcMoveMask = PACKETDATA;

	// Which buttons events will be handled by this context.  lcBtnMask
	// is a bitfield with one bit per button.
	log_ctx.lcBtnUpMask = log_ctx.lcBtnDnMask;

	// Set the entire tablet as active
	AXIS axis;
	REAL size;
	get_axis( axis, DVC_X, "DVC_X", _size[0] );
	log_ctx.lcInOrgX = 0;
	log_ctx.lcInExtX = axis.axMax;
	log_ctx.lcOutOrgX = 0;
	log_ctx.lcOutExtX = axis.axMax;
	_pos_factor[0] = 1. / DOUBLE(axis.axMax-1);

	get_axis( axis, DVC_Y, "DVC_Y", _size[1] );
	log_ctx.lcInOrgY = 0;
	log_ctx.lcInExtY = axis.axMax;
	log_ctx.lcOutOrgY = 0;
	log_ctx.lcOutExtY = axis.axMax;
	_pos_factor[1] = 1. / DOUBLE(axis.axMax-1);

	get_axis( axis, DVC_Z, "DVC_Z", _size[2] );
	// maa don't really get it but works with Wacom intuos medium July 2018
	//INT32 range = 1024;
	//maa was working then not 
	log_ctx.lcInOrgZ = 0;
	log_ctx.lcInExtZ = axis.axMax;
	log_ctx.lcOutOrgZ = 0;
	log_ctx.lcOutExtZ = axis.axMax;
	_pos_factor[2] = 1. / DOUBLE(533);	//Hack Maa July 2018 nothing seems to work on the scaling here except this 

	get_axis( axis, DVC_NPRESSURE, "DVC_NPRESSURE", size );

/*
	if( true )
	{
		// Guarantee the output coordinate space to be in screen coordinates.  
//		log_ctx.lcOutOrgX = 0;
//		log_ctx.lcOutOrgY = 0;


//		log_ctx.lcOutExtX = AAA_RANGE;
//		log_ctx.lcOutExtY = AAA_RANGE;
	}
	else
	{
		// Guarantee the output coordinate space to be in screen coordinates.  
		log_ctx.lcOutOrgX = GetSystemMetrics( SM_XVIRTUALSCREEN );
		log_ctx.lcOutOrgY = GetSystemMetrics( SM_YVIRTUALSCREEN );

		log_ctx.lcOutExtX = GetSystemMetrics( SM_CXVIRTUALSCREEN ); //SM_CXSCREEN );
		// In Wintab, the tablet origin is lower left.  Move origin to upper left
		// so that it coincides with screen origin.
		log_ctx.lcOutExtY = -GetSystemMetrics( SM_CYVIRTUALSCREEN );	//SM_CYSCREEN );
	}
*/

	// Leave the system origin and extents as received:
	// lcSysOrgX, lcSysOrgY, lcSysExtX, lcSysExtY

	// open the region
	// The Wintab spec says we must open the context disabled if we are 
	// using cursor masks.  
	HCTX hctx = gpWTOpenA( hWnd, &log_ctx, FALSE );
	if( hctx )	DBG_PRINT_STRING( "Tablet context is %i", hctx );
	else		ERR_PRINT_STRING( "No Tablet context" );

	return hctx;
}


void c_wacom::c_init()
{
	if( !b_start_with )
		return;
	if( b_lib_open )
		return;

	if( !load_wintab( ) )
	{
		ERR_PRINT_STRING( "Wintab not available." );
		return;
	}
	
	// check if WinTab available
	if( !gpWTInfoA( 0, 0, nullptr ) ) 
	{
		ERR_PRINT_STRING( "WinTab Services Not Available." );
		return;
	}	
	b_lib_open = true;

	UINT l_devices_nb;
	gpWTInfoA( WTI_INTERFACE, IFC_NDEVICES, &l_devices_nb );
	devices_nb = l_devices_nb;
}

void c_wacom::c_deinit()
{
	unload_wintab();
}

AAA_ERR c_wacom::open()
{
	if( !b_lib_open )
		return ERR_ANY;

	UINT l_devices_nb;
	gpWTInfoA( WTI_INTERFACE, IFC_NDEVICES, &l_devices_nb );
	if( l_devices_nb <= 0 )
	{
		ERR_PRINT_STRING( "No Tablet, so no Context to open." );
		return ERR_OBJ_NULL;
	}
		
	// check if WACOM available.
	char            WName[256];        // String to hold window name
	gpWTInfoA( WTI_DEVICES, DVC_NAME, WName );
	if( strncmp( WName,"WACOM",5 ) )
		ERR_PRINT_STRING( "Wacom Tablet Not Installed." );
	else
		GOOD_PRINT_STRING( "Wacom Tablet is %s", WName );

		// get info about tilt
	AXIS axis; // The capabilities of pressure
	
	_b_pressure_normal_can = gpWTInfoA( WTI_DEVICES, DVC_NPRESSURE, &axis ) == sizeof(AXIS);
	if( _b_pressure_normal_can )
	{
		_pressure_factor = 1. / axis.axMax;  
	}

	// get info about tilt
	AXIS TpOri[3]; // The capabilities of tilt
	
	_b_tilt_can = gpWTInfoA( WTI_DEVICES, DVC_ORIENTATION, &TpOri ) == sizeof(AXIS)*3;
	if( _b_tilt_can )
	{
		// does the tablet support azimuth and altitude
		if( TpOri[0].axResolution && TpOri[1].axResolution )
		{
			double	d_tmp;	// A temp for converting fix to double

			// convert azimuth resolution to double
			d_tmp = CONVERT_TO_DOUBLE( TpOri[0].axResolution );
			// convert from resolution to turn
			_azimuth_factor = 1. / d_tmp;  
			
			// convert altitude resolution to double
			d_tmp = CONVERT_TO_DOUBLE( TpOri[1].axResolution );
			// scale to arbitrary value to get decent line length
			_altitude_factor = 1. / d_tmp; 
			// adjust for maximum value at vertical
			_altitude_adjust = TpOri[1].axMax * _altitude_factor;

		}
		else	// no tilt
			_b_tilt_can = false;
	}			

	_h_ctx = init_tablet( get_window_main_handle(), _b_system_ui );
	if( _h_ctx )
	{
		gpWTEnable(	_h_ctx, TRUE );
		gpWTOverlap( _h_ctx, TRUE );
	}
	else
	{
		ERR_PRINT_STRING( "Could Not Open Tablet Context." );
		return ERR_ANY;
	}
	return AAA_OK;
}

void c_wacom::close()
{
	if( !b_lib_open )
		return;
	if( _h_ctx )
	{
		gpWTClose( _h_ctx );
		_h_ctx = NULL;
	}
}
	
void c_wacom::update_low() 
{
	if( _b_flip_uv )
		_orientation_factor[0] = _size[1] / _size[0];
	else
		_orientation_factor[0] = _size[0] / _size[1];
	_orientation_factor[1] = 1.;
		
	_stroke_out->clear();
	if( _stroke_in->is_data() )
	{
		{
			std::lock_guard<aaa::MUTEX> guard(_stroke_lock);
			SWAP( _stroke_in, _stroke_out );
		}
		_packet_nb = _stroke_out->size();
		_packet_nb_processed += _packet_nb;
	}
	else
		_packet_nb = 0;
}

#ifdef WIN32
#define GET_WM_ACTIVATE_STATE(wp)           LOWORD(wp)
#else
#define	GET_WM_ACTIVATE_STATE(wp)               (wp)
#endif

#define CASE_EVENT( e )		case e:	if( _b_verbose_message_ui ) VERBOSE_PRINT_STRING( "wacom event " ## #e );	break;

BOOL c_wacom::handle_event( UINT message, WPARAM wParam, LPARAM lParam )
{
	if( !b_lib_open )
		return FALSE;

	bool b_use = false;
	switch( message )
	{
	case WT_PACKET:
		if( is_active() )
		{
			PACKET pkt;
			if( (HCTX)lParam != _h_ctx )
				DBG_PRINT_STRING( "other tablet context" );
			if( gpWTPacket( (HCTX)lParam, (UINT)wParam, &pkt ) )
			{
				if( (HCTX)lParam != pkt.pkContext )
					DBG_PRINT_STRING( "pb with context %d and %d", (UINT)lParam, pkt.pkContext );
				if( (UINT)wParam != pkt.pkSerialNumber )
					DBG_PRINT_STRING( "pb with serial %d and %d", (UINT)wParam, pkt.pkSerialNumber );

				_time_last = _time_raw;
				_time_raw = pkt.pkTime;
		
				c_stroke::st_stroke_elt elt;
				elt._flags = c_stroke::M_ACTIVE;
				elt._time = _time_raw;

				//if(HIWORD(pkt.pkButtons)==TBN_DOWN) 
				//	MessageBeep(0);
				WORD hi = HIWORD(pkt.pkButtons);
				if(			hi==TBN_DOWN	)		GOOD_PRINT_STRING( "TBN_DOWN" );
				else if(	hi==TBN_UP		)		GOOD_PRINT_STRING( "TBN_UP" );

				if( _b_verbose_ui )
					GOOD_PRINT_STRING( "%d\t-- %d,%d,%d with %d", (UINT)wParam, pkt.pkX, pkt.pkY, pkt.pkZ, pkt.pkNormalPressure );

				_pos_raw[0]	= pkt.pkX;
				_pos_raw[1]	= pkt.pkY;
				_pos_raw[2]	= pkt.pkZ;
				mul_v3( _pos, _pos_raw, _pos_factor );
				if( _b_flip_uv )
				{
					REAL tmp = _pos[0];
					_pos[0] = _pos[1];
					_pos[1] = REAL(1) - tmp;
				}
				_pos[0] -= .5;
				_pos[1] -= .5;
				mul_v2( _pos, _orientation_factor );
				if( _b_verbose_ui )
					GOOD_PRINT_STRING( "        %g,%g,%g", _pos[0], _pos[1], _pos[2] );
				cpy_v3( elt._pos, _pos );

				if( _b_pressure_normal_can )
				{
					_pressure_normal_raw	= pkt.pkNormalPressure;
					_pressure_normal		= REAL( _pressure_normal_raw * _pressure_factor );
					elt._att[0] = _pressure_normal;
				}

				if( _b_tilt_can )
				{
					ORIENTATION     ort = pkt.pkOrientation;
					//  wintab.h defines .orAltitude as a UINT but documents .orAlttude 
					//  as positive for upward angles and negative for downward.
					//  WACOM uses negative altitude values to show that the pen is inverted;
					//  therefore we cast .orAltitude as an (int) and then use the absolute value.

					INT32 z_angle  = ort.orAltitude;	//Raw Altitude
					double d_z_angle;	//Adjusted Altitude
					_b_eraser = z_angle < 0;
					if( _b_eraser )
						elt._flags |= c_stroke::M_ERASER;
					d_z_angle = _altitude_adjust - abs(z_angle) * _altitude_factor;
					_rot[0] = REAL( d_z_angle );
					elt._att[1] = _rot[0];

					// adjust azimuth
					INT32 theta = ort.orAzimuth;	//Raw Azimuth
					double d_Theta;		//Adjusted Azimuth
					d_Theta = WRAP_01( .25 - ort.orAzimuth * _azimuth_factor );
					_rot[1] = REAL( d_Theta );
					elt._att[2] = _rot[1];

					// get the length of the diagonal to draw  
					//Z1Angle.x = (int)(d_z_angle * sin(d_Theta));
					//Z1Angle.y = (int)(d_z_angle * cos(d_Theta));

					if( _b_verbose_ui )
						GOOD_PRINT_STRING( "\t%d->%g\t%d->%g", z_angle, d_z_angle, theta, d_Theta );
				}
				b_use = true;
				{
					std::lock_guard<aaa::MUTEX> guard(_stroke_lock);
					_stroke_in->add( elt );
				}
			}
		}
		break;
	case WM_ACTIVATE:
		// if switching in the middle, disable the region
		if( _h_ctx ) 
		{
			WORD b = GET_WM_ACTIVATE_STATE( wParam );
			gpWTEnable( _h_ctx, b );
			if( b )
				gpWTOverlap( _h_ctx, TRUE );
			b_use = true;
		}
		break;
	CASE_EVENT( WT_CTXOPEN		)		
	CASE_EVENT( WT_CTXCLOSE		)		
	CASE_EVENT( WT_CTXUPDATE	)	
	CASE_EVENT( WT_CTXOVERLAP	)	
	CASE_EVENT( WT_PROXIMITY	)	
	CASE_EVENT( WT_INFOCHANGE	)		
	CASE_EVENT( WT_CSRCHANGE	)		
	CASE_EVENT( WT_PACKETEXT	)		
	CASE_EVENT( WT_MAX			)
	}

	return b_use; 
}
