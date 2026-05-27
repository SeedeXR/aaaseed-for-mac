#include "hid.h"

#if	AAA_TRACKER_HID()
#include "infrastructure/param/param_declare.h"

#include "hidapi.h"
#include "spy.h"
//#include "lib_wrappers/wrap_ftdi.h"

bool	c_hid::b_dll_loaded = true;

c_hid*	c_hid::array[ c_hid::OBJ_MAX_NB];


#include <lib_use.h>
AAA_LIB_USE( "hidapi" )


/*
static	CONST	INT32		FTDI_INOUT_MAX_NB = 2;	
static	C_PCHAR_C	ftdi_inout_str[ FTDI_INOUT_MAX_NB ] =
{
	"Out",
	"In",
};
*/
namespace {
C_PCHAR_C HID_HEADER = "# HID ";
void	HID_PRINT_STRING( C_PCHAR_C fmt, ... )
{
	va_list args;
	va_start( args, fmt );
	trackers::PRINT_STRING_VA( HID_HEADER, fmt, args );
	va_end( args );
}
void	HID_ERR_PRINT_STRING( C_PCHAR_C fmt, ... )
{
	va_list args;
	va_start( args, fmt );
	ERR_HEADER_PRINT_STRING_VA( HID_HEADER, fmt, args );
	va_end( args );
}
}
FACTORY_CREATE_V1( c_hid, hid, hid emotiv, hid );

namespace
{
	CONSTEXPR INT32 BASE_PARAM_NB	= 1;
	CONSTEXPR INT32 GROUP_PARAM_NB	= 0;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_OFF( active )
/*		PARAM_DEF_STR( serial_id_to_open )
//		{	nullptr,	PARAM_INT32,	"device_id",	2, 1,	1, PARAM_MAX_INT32,	nullptr, nullptr },
		PARAM_DEF_BOOL_LOCKED( open )
		PARAM_DEF_SYMBO_PSTR_ONE( pin_01, ftdi_inout_str )
		PARAM_DEF_SYMBO_PSTR_ONE( pin_02, ftdi_inout_str )
		PARAM_DEF_SYMBO_PSTR_ONE( pin_03, ftdi_inout_str )
		PARAM_DEF_SYMBO_PSTR_ONE( pin_04, ftdi_inout_str )
		PARAM_DEF_SYMBO_PSTR_ONE( pin_05, ftdi_inout_str )
		PARAM_DEF_SYMBO_PSTR_ONE( pin_06, ftdi_inout_str )
		PARAM_DEF_SYMBO_PSTR_ONE( pin_07, ftdi_inout_str )
		PARAM_DEF_SYMBO_PSTR_ONE( pin_08, ftdi_inout_str )
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
*/
	};
}

void	c_hid::param_init_pt()
{
	INT32	h = 0;

	param_set_pt( h, get_pt_active() );
/*	param_set_pt( h, _serial_to_open );
	param_set_pt( h, _b_open );
	param_set_pt_n( h, _pin, 8 );
	param_set_pt_n( h, _b_val_out, 8 );
	param_set_pt_n( h, _b_val_in, 8 );
*/
	err_param_init_pt( h );
}

//hack
//FT_HANDLE ft_handle;

CONSTRUCTOR_CREATE( c_hid )
{
	if( is_obj_first() ) 
		enumerate();

	CHAR	str[] = "hid_a emotiv";
	str[4] = 'a' + get_factory()->get_obj_count() - 1;
	set_name( str );

	param_init_with( param, PARAM_NB_MAX );
	_b_open = false;
//	_ft_handle = nullptr;
}


c_hid::~c_hid()
{
	close();
}

void	c_hid::update()
{
	if( is_active()!=_b_open )
	{
		if( _b_open )
			close();
		else
			open();
	}
	if( _b_open )
	{
/*		FT_STATUS	ft_status;
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
*/
	}
}


void c_hid::enumerate()
{
	struct hid_device_info *devs, *cur_dev;
	
	devs = hid_enumerate(0x0, 0x0);

	cur_dev = devs;
	while (cur_dev) {
		HID_PRINT_STRING( "Device Found"			);
		HID_PRINT_STRING( "\tvendor: 0x%04hx",		cur_dev->vendor_id				);
		HID_PRINT_STRING( "\tproduct_id: 0x%04hx",	cur_dev->product_id				);
		HID_PRINT_STRING( "\tpath: %s",				cur_dev->path					);	//	Platform-specific device path
		HID_PRINT_STRING( "\tserial_number: %ls",	cur_dev->serial_number			);
		HID_PRINT_STRING( "\tManufacturer: %ls",	cur_dev->manufacturer_string	);
		HID_PRINT_STRING( "\tProduct:      %ls",	cur_dev->product_string			);
		HID_PRINT_STRING( "\tRelease:      0x%hx",	cur_dev->release_number			);
		HID_PRINT_STRING( "\tInterface:    0x%d",	cur_dev->interface_number		);
		HID_PRINT_STRING( "\tUsage page:   0x%d",	cur_dev->usage_page				);
		HID_PRINT_STRING( "\tUsage:        0x%d",	cur_dev->usage					);
		HID_PRINT_STRING( "\tInterface nb: %d",		cur_dev->interface_number		);
		cur_dev = cur_dev->next;
	}
	hid_free_enumeration(devs);


/*	FT_STATUS ftStatus;
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
		ftStatus = dll_ftdi.FT_ListDevices( (PVOID)index, Buffer, FT_LIST_BY_INDEX|FT_OPEN_BY_SERIAL_NUMBER );
		if(ftStatus == FT_OK)
			trackers::PRINT_STRING( FTDI_HEADER, "device %d serial is %s", index, Buffer );
		else
			ERR_PRINT_STRING( "%s() can't read serial of %d device", __FUNCTION__, index );

		ftStatus = dll_ftdi.FT_ListDevices( (PVOID)index, Buffer, FT_LIST_BY_INDEX|FT_OPEN_BY_DESCRIPTION );
		if(ftStatus == FT_OK)
			trackers::PRINT_STRING( FTDI_HEADER, "          description is %s", Buffer );
		else
			ERR_PRINT_STRING( "%s() can't read description of %d device", __FUNCTION__, index );
	}
*/
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

/*
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
*/

void	c_hid::open()
{

//	FT_STATUS ft_status;

	_b_open = false;

	unsigned char buf[256];

		// Set up the command buffer.
	memset(buf,0x00,sizeof(buf));
	buf[0] = 0x01;
	buf[1] = 0x81;
	

	// Open the device using the VID, PID,
	// and optionally the Serial number.
	////handle = hid_open(0x4d8, 0x3f, L"12345");
	//handle = hid_open( 0x4d8, 0x3f, nullptr );
	hid_device*	handle = hid_open( 0x21a1, 0x1, nullptr );
	if( !handle )
	{
		HID_ERR_PRINT_STRING( "unable to open device" );
		return;
	}

	#define MAX_STR 255
	wchar_t wstr[MAX_STR];

	// Read the Manufacturer String
	wstr[0] = 0x0000;
	int res = hid_get_manufacturer_string(handle, wstr, MAX_STR);
	if( res < 0 )
		HID_ERR_PRINT_STRING( "Unable to read manufacturer string" );
	else
		HID_PRINT_STRING( "Manufacturer String: %ls", wstr);

	// Read the Product String$
	wstr[0] = 0x0000;
	res = hid_get_product_string(handle, wstr, MAX_STR);
	if( res < 0 )
		HID_ERR_PRINT_STRING( "Unable to read product string");
	else
		HID_PRINT_STRING( "Product String: %ls", wstr);

	// Read the Serial Number String
	wstr[0] = 0x0000;
	res = hid_get_serial_number_string(handle, wstr, MAX_STR);
	if( res < 0 )
		HID_ERR_PRINT_STRING(  "Unable to read serial number string" );
	else
		HID_PRINT_STRING( "Serial Number String: (%d) %ls", wstr[0], wstr);

	// Read Indexed String 1
	wstr[0] = 0x0000;
	res = hid_get_indexed_string(handle, 1, wstr, MAX_STR);
	if( res < 0 )
		HID_ERR_PRINT_STRING( "Unable to read indexed string 1");
	else
		HID_PRINT_STRING( "Indexed String 1: %ls", wstr);

	// Set the hid_read() function to be non-blocking.
	hid_set_nonblocking(handle, 1);
	
	// Try to read from the device. There shoud be no
	// data here, but execution should not block.
	res = hid_read(handle, buf, 17);

	// Send a Feature Report to the device
	buf[0] = 0x2;
	buf[1] = 0xa0;
	buf[2] = 0x0a;
	buf[3] = 0x00;
	buf[4] = 0x00;
	res = hid_send_feature_report(handle, buf, 17);
	if(res < 0)
	{
		HID_ERR_PRINT_STRING( "Unable to send a feature report." );
	}

	memset(buf,0,sizeof(buf));

	// Read a Feature Report from the device
	buf[0] = 0x2;
	res = hid_get_feature_report(handle, buf, sizeof(buf));
	if( res < 0 )
	{
		HID_ERR_PRINT_STRING( "Unable to get a feature report.");
		HID_ERR_PRINT_STRING( "%ls", hid_error(handle));
	}
	else
	{
		// Print out the returned buffer.
		HID_PRINT_STRING( "Feature Report");
		for( INT32 i = 0; i < res; i++)
			HID_PRINT_STRING( "%02hhx ", buf[i]);
	}

	memset(buf,0,sizeof(buf));

	// Toggle LED (cmd 0x80). The first byte is the report number (0x1).
	buf[0] = 0x1;
	buf[1] = 0x80;
	res = hid_write(handle, buf, 17);
	if( res < 0 )
	{
		HID_ERR_PRINT_STRING( "Unable to write()");
		HID_ERR_PRINT_STRING( "\tError: %ls", hid_error(handle));
	}
	

	// Request state (cmd 0x81). The first byte is the report number (0x1).
	buf[0] = 0x1;
	buf[1] = 0x81;
	hid_write(handle, buf, 17);
	if( res < 0 )
		HID_ERR_PRINT_STRING( "Unable to write() (2)");

	// Read requested state. hid_read() has been set to be
	// non-blocking by the call to hid_set_nonblocking() above.
	// This loop demonstrates the non-blocking nature of hid_read().
	res = 0;
	while( res == 0 )
	{
		res = hid_read(handle, buf, sizeof(buf));
		if( res == 0 )
			HID_ERR_PRINT_STRING( "waiting...");
		if( res < 0 )
			HID_ERR_PRINT_STRING( "Unable to read()");
		spy::sleep( 5, "sleep c_hid::open()" );
	}

	HID_PRINT_STRING(  "Data read:");
	// Print out the returned buffer.
	for (INT32 i = 0; i < res; i++)
		HID_PRINT_STRING( "\t%02hhx ", buf[i]);
	PRINT_CR();

/*
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
*/
	_b_open = true;
}

void	c_hid::close()
{
	if( _b_open )
	{
//		dll_ftdi.FT_Close( _ft_handle );
		_b_open = false;
	}
}

void	c_hid::c_init()
{
//	c_ftdi::b_dll_loaded = dll_ftdi.init();	//wrap_ftdi_Init() == 0;
}
void	c_hid::c_deinit()
{
	hid_exit();
//	if( c_ftdi::b_dll_loaded )
//		dll_ftdi.deinit();		//wrap_ftdi_Term();
}

#endif	//AAA_TRACKER_HID

