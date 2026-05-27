#include "ftdi.h"

#if	AAA_TRACKER_FTDI()
#include "infrastructure/param/param_declare.h"

#include "lib_wrappers/wrap_ftdi.h"

bool	c_ftdi::b_dll_loaded = false;
c_ftdi*	c_ftdi::array[ c_ftdi::OBJ_MAX_NB ];

namespace {
	CONSTEXPR INT32	INOUT_MAX_NB = 2;	
	C_PCHAR_C		ftdi_inout_str[ INOUT_MAX_NB ] =
	{
		"Out",
		"In",
	};
}


FACTORY_CREATE_V1( c_ftdi, ftdi, Ftdi By fred, ftdi );

namespace	n_ftdi
{
	CONSTEXPR INT32 BASE_PARAM_NB	= 27;
	CONSTEXPR INT32 GROUP_PARAM_NB	= 0;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_OFF( active )
		PARAM_DEF_STR( serial_id_to_open )
//		{	nullptr,	PARAM_INT32,	"device_id",	2, 1,	1, PARAM_MAX_INT32,	nullptr, nullptr },
		PARAM_DEF_BOOL_LOCKED( open )
		PARAM_DEF_SYMBO_PSTR_ONE( pin_01,	ftdi_inout_str )
		PARAM_DEF_SYMBO_PSTR_ONE( pin_02,	ftdi_inout_str )
		PARAM_DEF_SYMBO_PSTR_ONE( pin_03,	ftdi_inout_str )
		PARAM_DEF_SYMBO_PSTR_ONE( pin_04,	ftdi_inout_str )
		PARAM_DEF_SYMBO_PSTR_ONE( pin_05,	ftdi_inout_str )
		PARAM_DEF_SYMBO_PSTR_ONE( pin_06,	ftdi_inout_str )
		PARAM_DEF_SYMBO_PSTR_ONE( pin_07,	ftdi_inout_str )
		PARAM_DEF_SYMBO_PSTR_ONE( pin_08,	ftdi_inout_str )
		PARAM_DEF_BOOL_OFF( out_01 )
		PARAM_DEF_BOOL_OFF( out_02 )
		PARAM_DEF_BOOL_OFF( out_03 )
		PARAM_DEF_BOOL_OFF( out_04 )
		PARAM_DEF_BOOL_OFF( out_05 )
		PARAM_DEF_BOOL_OFF( out_06 )
		PARAM_DEF_BOOL_OFF( out_07 )
		PARAM_DEF_BOOL_OFF( out_08 )
		PARAM_DEF_BOOL_LOCKED( in_01 )
		PARAM_DEF_BOOL_LOCKED( in_02 )
		PARAM_DEF_BOOL_LOCKED( in_03 )
		PARAM_DEF_BOOL_LOCKED( in_04 )
		PARAM_DEF_BOOL_LOCKED( in_05 )
		PARAM_DEF_BOOL_LOCKED( in_06 )
		PARAM_DEF_BOOL_LOCKED( in_07 )
		PARAM_DEF_BOOL_LOCKED( in_08 )
	};
}

void	c_ftdi::param_init_pt()
{
	INT32	h = 0;

	param_set_pt( h, get_pt_active() );
	param_set_pt( h, _serial_to_open );
	param_set_pt( h, _b_open );
	param_set_pt_n( h, _pin, 8 );
	param_set_pt_n( h, _b_val_out, 8 );
	param_set_pt_n( h, _b_val_in, 8 );

	err_param_init_pt( h );
}

//hack
//FT_HANDLE ft_handle;

CONSTRUCTOR_CREATE( c_ftdi )
{

	if( is_obj_first() ) 
		enumerate();

	CHAR	str[] = "Ftdi_a by fred";
	str[5] = 'a' + get_factory()->get_obj_count() - 1;
	set_name( str );

	param_init_with( n_ftdi::param, n_ftdi::PARAM_NB_MAX );
	_b_open = false;
	_ft_handle = nullptr;
//	enumerate();

}


c_ftdi::~c_ftdi()
{
}

void	c_ftdi::update()
{
	if( is_active() != _b_open )
	{
		if( _b_open )
			close();
		else
			open();
	}
	if( _b_open )
	{
		FT_STATUS	ft_status;
		// check if mask have changed
		UINT8 mask = create_bitmask();
		if( mask != _mask_last )
		{
			ft_status = dll_ftdi.FT_SetBitMode( _ft_handle, mask, 0x1 );
			if( ft_status != FT_OK )
			{
				ERR_PRINT_STRING( "ftdi can't set gangbang Mode" );
				_mask_last = 0;
				return;
			}
			_mask_last = mask;
		}

		UINT8		buf;
		DWORD		len;

		ft_status = dll_ftdi.FT_GetBitMode( _ft_handle, &buf );
		if( ft_status == FT_OK )
		{
			for( INT32 i = 0; i < 8; ++i )
			{
				if( _pin[ i ] == 1 )
					_b_val_in[ i ] = buf & ( 1 << i );
			}			

		}
		else
		{
			ERR_PRINT_STRING( "ftdi can't read" );
		}

		buf = 0;
		for( INT32 i = 0; i < 8; ++i )
		{
			if( ( _pin[ i ] == 0 ) && _b_val_out[ i ] )
				buf |= 1 << i;
		}

		ft_status = dll_ftdi.FT_Write( _ft_handle, &buf, 1, &len );
		if( ft_status != FT_OK )
		{
			ERR_PRINT_STRING( "ftdi can't write" );
	//		return;
		}		
	}
}

C_PCHAR_C FTDI_HEADER = "# FTDI ";
void c_ftdi::enumerate()
{
	FT_STATUS ftStatus;
	DWORD nb;

	//Get the number of devices currently connected
	ftStatus = dll_ftdi.FT_ListDevices( &nb, nullptr, FT_LIST_NUMBER_ONLY );
	if( ftStatus == FT_OK ) 
	{
		trackers::PRINT_STRING( FTDI_HEADER, "%d devices", nb );
	}
	else
	{
		ERR_PRINT_STRING( "%s() can't enumerate", __FUNCTION__ );
		return;
	}

	//Get serial number of first device
	for( DWORD index = 0; index<nb; ++index )
	{
		char Buffer[64]; // more than enough room!
		ftStatus = dll_ftdi.FT_ListDevices( reinterpret_cast<P_VOID>(static_cast<intptr_t>(index)), Buffer, FT_LIST_BY_INDEX|FT_OPEN_BY_SERIAL_NUMBER );
		if(ftStatus == FT_OK)
			trackers::PRINT_STRING( FTDI_HEADER, "device %d serial is %s", index, Buffer );
		else
			ERR_PRINT_STRING( "%s() can't read serial of %d device", __FUNCTION__, index );

		ftStatus = dll_ftdi.FT_ListDevices( reinterpret_cast<P_VOID>(static_cast<intptr_t>(index)), Buffer, FT_LIST_BY_INDEX|FT_OPEN_BY_DESCRIPTION );
		if(ftStatus == FT_OK)
			trackers::PRINT_STRING( FTDI_HEADER, "          description is %s", Buffer );
		else
			ERR_PRINT_STRING( "%s() can't read description of %d device", __FUNCTION__, index );
	}
/*
//Note that indexes are zero-based. If more than one device is connected, incrementing devIndex
	//	will get the serial number of each connected device in turn.
	//	Get device descriptions of all devices currently connected
	char *BufPtrs[3]; // pointer to array of 3 pointers
	char Buffer1[64]; // buffer for description of first device
	char Buffer2[64]; // buffer for description of second device
	// initialize the array of pointers
	BufPtrs[0] = Buffer1;
	BufPtrs[1] = Buffer2;
	BufPtrs[2] = nullptr; // last entry should be NULL
	ftStatus = FT_ListDevices(BufPtrs,&numDevs,FT_LIST_ALL|FT_OPEN_BY_DESCRIPTION);
	if(ftStatus == FT_OK) {
		// FT_ListDevices OK, product descriptions are in Buffer1 and Buffer2, and
		// numDevs contains the number of devices connected
	}
	else {
		// FT_ListDevices failed
	}
*/
}

UINT8	c_ftdi::create_bitmask()
{
	UINT8	mask = 0;

	for ( INT32 i = 0; i < 8; ++i )
	{
		if( _pin[ i ] == 0 )
			mask += 1 << i ;
	}
	return mask;
}

void	c_ftdi::open()
{
	FT_STATUS ft_status;

	_b_open = false;

//	ft_status = FT_Open( _device_id, &_ft_handle );
	ft_status = dll_ftdi.FT_OpenEx( (VOID*) _serial_to_open.get(), FT_OPEN_BY_SERIAL_NUMBER, &_ft_handle );
	if( ft_status != FT_OK )
	{
		ERR_PRINT_STRING( "%s() dans le cul la balayette dit fred.", __FUNCTION__ );
		return;
	}

	trackers::PRINT_STRING( FTDI_HEADER, "%s() ok", __FUNCTION__ );
	ft_status = dll_ftdi.FT_ResetDevice( _ft_handle );
	if( ft_status != FT_OK ) 
		ERR_PRINT_STRING( "ftdi can't reset" );
	// set the bigbang async mode on FT245b
//	UINT8	mask = 0x00; //create_bitmask();
	// when opening set bitmask to In to protect the module
	_mask_last = 0x00;
	ft_status = dll_ftdi.FT_SetBitMode( _ft_handle, _mask_last, 0x1 );
	if( ft_status != FT_OK )
	{
		ERR_PRINT_STRING( "ftdi can't set gangbang Mode" );
		_mask_last = 0;
		return;
	}
	
	ft_status = dll_ftdi.FT_SetBaudRate( _ft_handle, FT_BAUD_9600 );
	if( ft_status != FT_OK )
	{
		ERR_PRINT_STRING( "ftdi can't set baudrate" );
		return;
	}
	_b_open = true;
}

void	c_ftdi::close()
{
	if( _b_open )
	{
		dll_ftdi.FT_Close( _ft_handle );
		_b_open = false;
	}
}

void	ftdi_open()
{
	c_ftdi::b_dll_loaded = dll_ftdi.init();	//wrap_ftdi_Init() == 0;
}
void	ftdi_close()
{
	if( c_ftdi::b_dll_loaded )
		dll_ftdi.deinit();		//wrap_ftdi_Term();
}

#endif