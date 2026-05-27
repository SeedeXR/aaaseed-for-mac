#include "media/video/vbl.h"
#include "ddraw.h"
#include "spy.h"
#include "Thread/aaa_thread.h"


#if AAA_OS_WINDOWS()
#	include <lib_use.h>
	AAA_LIB_USE( "ddraw" )
	AAA_LIB_USE( "DXGuid" )
#endif


static	LPDIRECTDRAW7 p_dd = nullptr;

bool	vbl::b_process = false;
bool	vbl::b_count_active = false;
INT32	vbl::count;

INT32	get_scanline()
{
	HRESULT	hr;
	DWORD	scanline = 0;
	if( p_dd && FAILED( hr = p_dd->GetScanLine(&scanline) ) )
	{
		if( hr == DDERR_VERTICALBLANKINPROGRESS )
			return 0;
		ERR_PRINT_STRING( "Can't GetScanLine" );
		return 0;
	}
	else
		return scanline;
}

void	vbl::wait_one()
{
	HRESULT hr;
//	if( FAILED( hr = IDirectDraw7_WaitForVerticalBlank( p_dd, DDWAITVB_BLOCKBEGIN, nullptr) ) )
	if( p_dd && FAILED( hr = p_dd->WaitForVerticalBlank( DDWAITVB_BLOCKBEGIN, nullptr ) ) )
		ERR_PRINT_STRING( "Can't WaitForVerticalBlank" );
	else
	{
		++count;
//		DBG_PRINT_STRING( "vb : %d", vbl_get_count() );
		spy::sleep( 0, "sleep vbl::wait_one()" );
	}
}

INT32	vbl::get_count()
{
	return count;
}

void
#ifdef	WIN32
__cdecl
#endif
th_vbl_count( void* dummy )
{
//c_net	*pt_obj;
//	pt_obj = (c_net *)dummy;
//	set_process_priority_class( PRIO_CLASS_HIGH);
//	THREAD_PRIORITY_TIME_CRITICAL
//	THREAD_PRIORITY_BELOW_NORMAL
//	THREAD_PRIORITY_NORMAL
//	SetThreadPriority( get_thread_cur(), THREAD_PRIORITY_NORMAL);
	vbl::count = 0;
	vbl::b_process = true;
	while( vbl::b_count_active )
		vbl::wait_one();
	vbl::b_process = false;
	c_thread::end();
}

void	vbl_start_count()
{
	c_thread::begin( "vbl_count", th_vbl_count );
}

static	INT32	scanline_last;
void	vbl::update_private()
{
//	vbl_start_count();
//	DBG_PRINT_STRING( "scanline %d", get_scanline());
	INT32 scanline = get_scanline();
	TBUF_ADD( tbuf::CH_VBL, REAL(scanline)/1200., nullptr );	//hack we need to find the nb of scan line
	if( scanline < scanline_last )
		++count;
	scanline_last = scanline;
}

AAA_ERR	vbl::init()
{
	HRESULT hr;
	// Create a DirectDraw object
	//todo	we should use WIN32

	if( FAILED( hr = DirectDrawCreateEx( nullptr, reinterpret_cast<VOID**>(&p_dd), IID_IDirectDraw7, nullptr ) ) )
	{
		p_dd = nullptr;
		BOX_ERR( "No DirectDraw 7 -> no vertical Blanking measure");
		return ERR_ANY;
	}
	update();
	return AAA_OK;
	// Get the device information and save it
//    pDD->GetDeviceIdentifier( &g_DeviceIdent[g_iMaxDevices].DeviceInfo, 0 );
//    pDD->GetDeviceIdentifier( &g_DeviceIdent[g_iMaxDevices].DeviceInfoHost, 
//                              DDGDI_GETHOSTIDENTIFIER );
}

void	vbl::deinit()
{
	b_count_active = false;
}
