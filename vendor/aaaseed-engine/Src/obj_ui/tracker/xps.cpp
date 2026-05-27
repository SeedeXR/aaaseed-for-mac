#include "xps.h"
#include "infrastructure/param/param_declare.h"
#include "system/shared/SystemUtils.h"

FACTORY_CREATE_V1( c_xps, xps, Xps LED, xps );

namespace	n_xps
{
	CONSTEXPR INT32 BASE_PARAM_NB	= 5;
	CONSTEXPR INT32 GROUP_PARAM_NB	= 0;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_OFF( active )
		PARAM_DEF_INT32( color_back,	1,0,	0, 16 )
		PARAM_DEF_INT32( color_side,	1,0,	0, 16 )
		PARAM_DEF_INT32( color_front,	1,0,	0, 16 )
		PARAM_DEF_INT32( intensity,		1,0,	0, 7 )
	};
}

void	c_xps::param_init_pt_static()
{
	INT32	h=0;
	param_set_pt( h, get_pt_active() );
	param_set_pt( h, _color_back );
	param_set_pt( h, _color_side );
	param_set_pt( h, _color_front );
	param_set_pt( h, _luma );

	err_param_init_pt(h);
}


CONSTRUCTOR_CREATE(c_xps)
{
	param_init_with( n_xps::param, n_xps::PARAM_NB_MAX ); // xps_param, XPS_PARAM_NB_MAX);
}

#define BUFF_SIZE 0x2c
static	CHAR buffer[BUFF_SIZE];
static	CHAR buffer_last[4];
static	HANDLE hHandle = nullptr;

c_xps::~c_xps()
{
	if( hHandle )
	{
		CloseHandle( hHandle );
		hHandle = nullptr;
	}
}

bool	b_can_xps = true;

bool	set_xps_colors( CHAR top_color, CHAR side_color, CHAR front_color, CHAR brightness )
{
	bool	result = false;
	if( b_can_xps )
	{
		DWORD	bytesReturned;
		if( !hHandle )
		{
			hHandle = CreateFileA(	"\\\\.\\APPDRV", 
								GENERIC_READ|GENERIC_WRITE, 0, nullptr,
								OPEN_EXISTING, 0x80, nullptr );
			memset(buffer, 0, BUFF_SIZE);
			if( hHandle == INVALID_HANDLE_VALUE)
			{
				ERR_PRINT_STRING( "XPS CreateFileA(\\\\.\\APPDRV) failed: %s", aaa::system::get_err_message().c_str() );
				goto exit;
			}
			buffer[0] = 0x04;
			buffer[1] = 0x00;
			buffer[2] = 0x06;
			buffer[3] = 0x00;

			buffer[8] = 0x1;
		}

		buffer[4] = side_color;
		buffer[5] = front_color;
		buffer[6] = top_color;
		buffer[7] = brightness;

		if( *(INT32*)buffer_last != *(INT32*)(buffer+4) )
		{
			*(INT32*)buffer_last = *(INT32*)(buffer+4);
			result = DeviceIoControl( hHandle, 0x22209C, buffer, BUFF_SIZE, buffer, BUFF_SIZE, &bytesReturned, nullptr );
			if( !result )
			{
				ERR_PRINT_STRING( "%s() DeviceIoControl failed: %s", __FUNCTION__, aaa::system::get_err_message().c_str() );
				ERR_PRINT_STRING( "Switching Off XPS lights command until next run." );
				b_can_xps = false;
			}
		}
	}
exit:
	return result ? true : false;
}

void	c_xps::update()
{
	if( is_active() )
		set_xps_colors( (CHAR)_color_back, (CHAR)_color_side, (CHAR)_color_front, (CHAR)_luma );
}

c_xps*	c_xps::def = nullptr;
c_xps*	c_xps::out = nullptr;
