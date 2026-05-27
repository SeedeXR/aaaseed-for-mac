#include "power_master.h"
#include "infrastructure/param/param_declare.h"

/*
__pragma(   message( " NTDDI_VERSION : "		)	);
__pragma(   message( AAA_STRING(NTDDI_VERSION)	)	);
__pragma(   message( " NTDDI_VISTA : "			)	);
__pragma(   message( AAA_STRING(NTDDI_VISTA)	)	);
*/
//	maa 2018 using definition of fn did not work so we force it
#define AAA_LOCAL_DO_POWER_PROF()	(NTDDI_VERSION >= NTDDI_VISTA)

#if AAA_LOCAL_DO_POWER_PROF()
#	include "aaa_os.h"	//needed before #include <PowrProf.h>
#	include <PowrProf.h>
#	include <initguid.h> // for DEFINE_GUID
#	include <lib_use.h>
	AAA_LIB_USE_MESSAGE( "PowrProf" )		// lib for power manager function Vista and Up.

	// GUID definition; thank you Microsoft
	// {893dee8e-2bef-41e0-89c6-b55d0929964c}	Minimum Processor State
	DEFINE_GUID ( GUID_PROC_STATE_MIN,		0x893dee8e, 0x2bef, 0x41e0, 0x89, 0xc6, 0xb5, 0x5d, 0x09, 0x29, 0x96, 0x4c );
	// {bc5038f7-23e0-496096da33abaf5935ec}		Maximum Processor State
	DEFINE_GUID ( GUID_PROC_STATE_MAX,		0xbc5038f7, 0x23e0, 0x4960, 0x96, 0xda, 0x33, 0xab, 0xaf, 0x59, 0x35, 0xec );
	// {aded5e82-b909-4619-9949-f5d71dac0bcb}	Display Brightness
	DEFINE_GUID ( GUID_DISPLAY_BRIGTHNESS,	0xaded5e82, 0xb909, 0x4619, 0x99, 0x49, 0xf5, 0xd7, 0x1d, 0xac, 0x0b, 0xcb );
	DEFINE_GUID ( GUID_PROC_IDLE_DISABLE,	0x5d76a2ca, 0xe8c0, 0x402f, 0xa1, 0x33, 0x21, 0x58, 0x49, 0x2d, 0x58, 0xad );

	namespace
	{
		GUID*	g_scheme = nullptr;
	}
#else
namespace{
	void err_not_implemented()
	{
		ERR_PRINT_STRING( "cpu state change not implemented in AAASeed version");
	}
}
#endif
// we should forbid before Windows 7


FACTORY_CREATE_V1( c_power_master, power_master, Power Master, power_master );

namespace	n_power_master
{

	//PO_THROTTLE_NONE		0
	//PO_THROTTLE_CONSTANT	1
	//PO_THROTTLE_DEGRADE	2
	//PO_THROTTLE_ADAPTIVE	3
	C_PCHAR_C	str_throttle[]	=	{	"None", "CONSTANT"	};	//	the two here don't pass on debug Win10  , "DEGRADE", "ADAPTATIVE"		};

	CONSTEXPR INT32 BASE_PARAM_NB	= 28;
	CONSTEXPR INT32 GROUP_NB		= 0;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	GROUP_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_LOCKED(		can									)
		PARAM_DEF_BOOL_OFF(			active								)
		PARAM_DEF_BOOL_LOCKED(		plugged								)
		PARAM_DEF_BOOL_LOCKED(		charging							)
		PARAM_DEF_REAL_LOCKED(		battery_level						)
		PARAM_DEF_REAL_LOCKED(		time_left							)
		PARAM_DEF_REAL_LOCKED(		time_max							)

		PARAM_DEF_BOOL_OFF(			cpu_state_read_trig					)

		PARAM_DEF_SYMBO_PSTR(			cpu_throttle_ac_asked,				2, 0, str_throttle )	
		PARAM_DEF_SYMBO_PSTR(			cpu_throttle_dc_asked,				2, 0, str_throttle )	
		PARAM_DEF_BOOL_OFF(				cpu_throttle_trig					)
		PARAM_DEF_SYMBO_LOCKED_PSTR(	cpu_throttle_ac,					str_throttle )
		PARAM_DEF_SYMBO_LOCKED_PSTR(	cpu_throttle_dc,					str_throttle )

		PARAM_DEF_FP32_ONE_ZERO(	cpu_state_plugged_min_asked			)
		PARAM_DEF_FP32_ZERO_ONE(	cpu_state_plugged_max_asked			)
		PARAM_DEF_BOOL_OFF(			cpu_state_plugged_trig				)
		PARAM_DEF_FP32_LOCKED(		cpu_state_plugged_min				)
		PARAM_DEF_FP32_LOCKED(		cpu_state_plugged_max				)

		PARAM_DEF_FP32_ONE_ZERO(	cpu_state_battery_min_asked			)
		PARAM_DEF_FP32_ZERO_ONE(	cpu_state_battery_max_asked			)
		PARAM_DEF_BOOL_OFF(			cpu_state_battery_trig				)
		PARAM_DEF_FP32_LOCKED(		cpu_state_battery_min				)
		PARAM_DEF_FP32_LOCKED(		cpu_state_battery_max				)

		PARAM_DEF_FP32_ZERO_ONE(	display_brightness_plugged_asked	)
		PARAM_DEF_FP32_ZERO_ONE(	display_brightness_battery_asked	)
		PARAM_DEF_BOOL_OFF(			display_brightness_trig				)
		PARAM_DEF_FP32_LOCKED(		display_brightness_plugged			)
		PARAM_DEF_FP32_LOCKED(		display_brightness_battery			)
	};
}

void	c_power_master::param_init_pt()
{
	INT32	h = 0;

	param_set_pt( h,	_b_can							);
	param_set_pt( h,	get_pt_active()					);
	param_set_pt( h,	_b_plugged						);
	param_set_pt( h,	_b_charging						);
	param_set_pt( h,	_battery_level					);
	param_set_pt( h,	_time_left						);
	param_set_pt( h,	_time_max						);

	param_set_pt( h,	_b_cpu_state_read_trig_ui		);

	param_set_pt( h,	_cpu_throttle_ac_asked			);
	param_set_pt( h,	_cpu_throttle_dc_asked			);
	param_set_pt( h,	_b_cpu_throttle_trig_ui			);
	param_set_pt( h,	_cpu_throttle_ac				);
	param_set_pt( h,	_cpu_throttle_dc				);

	param_set_pt( h,	_cpu_state_ac_min_asked			);
	param_set_pt( h,	_cpu_state_ac_max_asked			);
	param_set_pt( h,	_b_cpu_state_ac_write_trig_ui	);
	param_set_pt( h,	_cpu_state_ac_min				);
	param_set_pt( h,	_cpu_state_ac_max				);

	param_set_pt( h,	_cpu_state_dc_min_asked			);
	param_set_pt( h,	_cpu_state_dc_max_asked			);
	param_set_pt( h,	_b_cpu_state_dc_write_trig_ui	);
	param_set_pt( h,	_cpu_state_dc_min				);
	param_set_pt( h,	_cpu_state_dc_max				);

	param_set_pt( h,	_display_brightness_ac_asked	);
	param_set_pt( h,	_display_brightness_dc_asked	);
	param_set_pt( h,	_b_display_bright_trig_ui		);
	param_set_pt( h,	_display_brightness_ac			);
	param_set_pt( h,	_display_brightness_dc			);

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_power_master)
#if AAA_LOCAL_DO_POWER_PROF()
	,_b_can					(true)
#else
	,_b_can					(false)
#endif
	,_b_plugged				(false)
	,_b_charging			(false)
	,_battery_level			(0)
	,_time_left				(0)
	,_time_max				(0)
	,_cpu_throttle_ac		(0)
	,_cpu_throttle_dc		(0)
	,_cpu_state_ac_min		(-1.)
	,_cpu_state_ac_max		(-1.)
	,_cpu_state_dc_min		(-1.)
	,_cpu_state_dc_max		(-1.)
	,_display_brightness_ac	(-1.)
	,_display_brightness_dc	(-1.)
{
	param_init_with( n_power_master::param, n_power_master::PARAM_NB_MAX );
	if( is_obj_first() )
	{
		_b_cpu_state_read_trig_ui = true;
		_b_cpu_throttle_trig_ui = true;
	}
}


c_power_master::~c_power_master()
{
}

void	c_power_master::update()
{
	if( !is_active() )
		return;

	SYSTEM_POWER_STATUS status; // note not LPSYSTEM_POWER_STATUS
	::GetSystemPowerStatus( &status );
	BYTE state = status.ACLineStatus;
		_b_plugged = state & AC_LINE_ONLINE;
	state = status.BatteryFlag;
		_b_charging = state & BATTERY_FLAG_CHARGING;
	_battery_level = ((REAL) status.BatteryLifePercent ) * REAL(.01);
	DWORD life = status.BatteryLifeTime;
		_time_left = life == BATTERY_LIFE_UNKNOWN ? 0 : (REAL) life;
	life = status.BatteryFullLifeTime;
		_time_max = life == BATTERY_LIFE_UNKNOWN ? 0 : (REAL) life;

#if AAA_LOCAL_DO_POWER_PROF()
	GUID*	scheme = nullptr;
#endif

	// CPU Throttling
	if( _b_cpu_throttle_trig_ui )
	{
#if AAA_LOCAL_DO_POWER_PROF()
		// get active power scheme
		if( scheme || ::PowerGetActiveScheme( nullptr, &scheme ) == ERROR_SUCCESS )
		{
			if( ::PowerWriteACValueIndex(	nullptr, scheme, &GUID_PROCESSOR_SETTINGS_SUBGROUP, &GUID_PROC_IDLE_DISABLE, _cpu_throttle_ac_asked ) != ERROR_SUCCESS )
				ERR_PRINT_STRING( "Couldn't set Minimum Processor State Plugged In" );
			if( ::PowerWriteDCValueIndex(	nullptr, scheme, &GUID_PROCESSOR_SETTINGS_SUBGROUP, &GUID_PROC_IDLE_DISABLE, _cpu_throttle_dc_asked ) != ERROR_SUCCESS )
				ERR_PRINT_STRING( "Couldn't set Maximum Processor State Plugged In" );
		}
		else
			ERR_PRINT_STRING( "PowerGetActiveScheme() failed." );
#else
		err_not_implemented();
#endif
		_b_cpu_state_read_trig_ui = true;
		_b_cpu_throttle_trig_ui = false;
	}

	if( _b_cpu_state_ac_write_trig_ui )
	{
#if AAA_LOCAL_DO_POWER_PROF()
		// get active power scheme
		if( scheme || ::PowerGetActiveScheme( nullptr, &scheme ) == ERROR_SUCCESS )
		{
			if( ::PowerWriteACValueIndex(	nullptr, scheme, &GUID_PROCESSOR_SETTINGS_SUBGROUP, &GUID_PROC_STATE_MIN, (INT32)(_cpu_state_ac_min_asked*100.) ) != ERROR_SUCCESS )
				ERR_PRINT_STRING( "Couldn't set Minimum Processor State Plugged In" );
			if( ::PowerWriteACValueIndex(	nullptr, scheme, &GUID_PROCESSOR_SETTINGS_SUBGROUP, &GUID_PROC_STATE_MAX, (INT32)(_cpu_state_ac_max_asked*100.) ) != ERROR_SUCCESS )
				ERR_PRINT_STRING( "Couldn't set Maximum Processor State Plugged In" );
		}
		else
			ERR_PRINT_STRING( "PowerGetActiveScheme() failed." );
#else
		err_not_implemented();
#endif
		_b_cpu_state_read_trig_ui = true;
		_b_cpu_state_ac_write_trig_ui = false;
	}
	if( _b_cpu_state_dc_write_trig_ui )
	{
#if AAA_LOCAL_DO_POWER_PROF()
		if( scheme || ::PowerGetActiveScheme( nullptr, &scheme ) == ERROR_SUCCESS )
		{
			if( ::PowerWriteDCValueIndex(	nullptr, scheme, &GUID_PROCESSOR_SETTINGS_SUBGROUP, &GUID_PROC_STATE_MIN, (INT32)(_cpu_state_dc_min_asked*100.) ) != ERROR_SUCCESS )
				ERR_PRINT_STRING( "Couldn't set Minimum Processor State on Battery" );
			if( ::PowerWriteDCValueIndex(	nullptr, scheme, &GUID_PROCESSOR_SETTINGS_SUBGROUP, &GUID_PROC_STATE_MAX, (INT32)(_cpu_state_dc_max_asked*100.) ) != ERROR_SUCCESS )
				ERR_PRINT_STRING( "Couldn't set Maximum Processor State on Battery" );
		}
		else
			ERR_PRINT_STRING( "PowerGetActiveScheme() failed." );
#else
		err_not_implemented();
#endif
		_b_cpu_state_read_trig_ui = true;
		_b_cpu_state_dc_write_trig_ui = false;
	}
	if( _b_display_bright_trig_ui )
	{
#if AAA_LOCAL_DO_POWER_PROF()
		if( scheme || ::PowerGetActiveScheme( nullptr, &scheme ) == ERROR_SUCCESS )
		{
			if( ::PowerWriteACValueIndex(	nullptr, scheme, &GUID_VIDEO_SUBGROUP, &GUID_DISPLAY_BRIGTHNESS, (INT32)(_display_brightness_ac*100.) ) != ERROR_SUCCESS )
				ERR_PRINT_STRING( "Couldn't set Display Brightness when Plugged" );
			if( ::PowerWriteDCValueIndex(	nullptr, scheme, &GUID_VIDEO_SUBGROUP, &GUID_DISPLAY_BRIGTHNESS, (INT32)(_display_brightness_dc*100.) ) != ERROR_SUCCESS )
				ERR_PRINT_STRING( "Couldn't set Display Brightness on Battery" );
		}
		else
			ERR_PRINT_STRING( "PowerGetActiveScheme() failed." );
#else
		err_not_implemented();
#endif
		_b_cpu_state_read_trig_ui = true;
		_b_display_bright_trig_ui = false;
	}	
	

	if( _b_cpu_state_read_trig_ui )
	{
#if AAA_LOCAL_DO_POWER_PROF()
		if( scheme || ::PowerGetActiveScheme( nullptr, &scheme ) == ERROR_SUCCESS )
		{
			DWORD val = 0;
			if( ::PowerReadACValueIndex(	nullptr, scheme, &GUID_PROCESSOR_SETTINGS_SUBGROUP, &GUID_PROC_STATE_MIN, &val ) == ERROR_SUCCESS )
				_cpu_state_ac_min		=	REAL( val / 100. );
			if( ::PowerReadACValueIndex(	nullptr, scheme, &GUID_PROCESSOR_SETTINGS_SUBGROUP, &GUID_PROC_STATE_MAX, &val ) == ERROR_SUCCESS )
				_cpu_state_ac_max		=	REAL( val / 100. );			
			if( ::PowerReadDCValueIndex(	nullptr, scheme, &GUID_PROCESSOR_SETTINGS_SUBGROUP, &GUID_PROC_STATE_MIN, &val ) == ERROR_SUCCESS )
				_cpu_state_dc_min		=	REAL( val / 100. );
			if( ::PowerReadDCValueIndex(	nullptr, scheme, &GUID_PROCESSOR_SETTINGS_SUBGROUP, &GUID_PROC_STATE_MAX, &val ) == ERROR_SUCCESS )
				_cpu_state_dc_max		=	REAL(val / 100. );		
			if( ::PowerReadACValueIndex(	nullptr, scheme, &GUID_VIDEO_SUBGROUP, &GUID_DISPLAY_BRIGTHNESS, &val ) == ERROR_SUCCESS )
				_display_brightness_ac	=	REAL(val / 100. );
			if( ::PowerReadDCValueIndex(	nullptr, scheme, &GUID_VIDEO_SUBGROUP, &GUID_DISPLAY_BRIGTHNESS, &val ) == ERROR_SUCCESS )
				_display_brightness_dc	=	REAL(val / 100. );
			if( ::PowerReadACValueIndex(	nullptr, scheme, &GUID_PROCESSOR_SETTINGS_SUBGROUP, &GUID_PROC_IDLE_DISABLE, &val ) == ERROR_SUCCESS )
				_cpu_throttle_ac		=	val;
			if( ::PowerReadDCValueIndex(	nullptr, scheme, &GUID_PROCESSOR_SETTINGS_SUBGROUP, &GUID_PROC_IDLE_DISABLE, &val ) == ERROR_SUCCESS )
				_cpu_throttle_dc		=	val;
		}
		else
			ERR_PRINT_STRING( "PowerGetActiveScheme() failed." );
#else
		err_not_implemented();
#endif
		_b_cpu_state_read_trig_ui = false;
	}

#if AAA_LOCAL_DO_POWER_PROF()
	if( scheme )
	{
		::PowerSetActiveScheme( nullptr, scheme );
		//::LocalFree( scheme );
		scheme = nullptr;
	}
#endif
}

//	const int MONITOR_ON = -1;
//	const int MONITOR_OFF = 2;
//	const int MONITOR_STANBY = 1;
//	SendMessage(	HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, (LPARAM) MONITOR_STANBY );
//	SendMessage(	HWND, WM_SYSCOMMAND, SC_MONITORPOWER, (LPARAM) 2 );
/*
void	c_power_master::c_init()
{
}
*/

c_power_master*	g_power_master = nullptr;