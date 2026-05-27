#include "capture_display.h"
		 
#include "os_version.h"
#include "infrastructure/param/param_declare.h"
#include "Thread/aaa_thread.h"
#include "system/shared/SystemUtils.h"

#if defined(WIN32)
#	include <d3dcommon.h>
#	include <d3d11.h>
#	include <dxgi1_2.h>
#	include <atlcomcli.h>
//#	include <DXGIDebug.h>		// to use D3D11 debug functions
#endif // WIN32

//todo deal with screen accross different devices (2024 August we deal only with one device)

class c_thread_grabber : public c_thread
{
public:
	virtual	void	run()	{	run_it< c_capture_display, -1 > ();	}
	c_thread_grabber() : c_thread( "display_grabber_thread_loop" )	{}
};


FACTORY_CREATE_V1( c_cap_display_ui, capture_display_ui, Display Grabber, capture_display_ui );

namespace n_cap_display_ui
{

	CONSTEXPR INT32	BASE_PARAM_NB	= 2;
	CONSTEXPR INT32	CROP_PARAM_NB	= 5;
	CONSTEXPR INT32	GROUP_NB		= 1;
	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
									+	CROP_PARAM_NB
									+	GROUP_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_LOCKED(	can			)
		PARAM_DEF_BOOL_OFF(		thread_use			)
		PARAM_DEF_GROUP(		Crop, CROP_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		crop_active		)
			PARAM_DEF_INT32_POS(	crop_left,		100, 0	)
			PARAM_DEF_INT32_POS(	crop_top,		100, 0	)
			PARAM_DEF_INT32_POS(	crop_right,		100, 0	)
			PARAM_DEF_INT32_POS(	crop_bottom,	100, 0	)
	};
}

void	c_cap_display_ui::param_init_pt()
{
	INT32	h = 0;
	param_set_pt( h, c_capture_display::b_can );
	param_set_pt( h, _b_use_thread_ui	);
	++h;
		param_set_pt( h, _b_crop_ui		);
		param_set_pt( h, _left_ui		);
		param_set_pt( h, _top_ui		);
		param_set_pt( h, _right_ui		);
		param_set_pt( h, _bottom_ui		);

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE( c_cap_display_ui )
{
	param_init_with( n_cap_display_ui::param, n_cap_display_ui::PARAM_NB_MAX );
}
EMPTY_DESTRUCTOR( c_cap_display_ui )


//INT32	c_capture_display::do_enum( bool b_verbose )
//{
//
//}


static INT32	device_count = 0;
static o_str	o_device_name[c_capture::DEVICE_MAX_NB];
bool	c_capture_display::b_can			= false;

INT32	c_capture_display::get_device_enumed()
{
	return device_count;
}
o_str CONST * CONST c_capture_display::get_device_name( INT32 CONST index )
{
	if( 0 <= index && index < c_capture::DEVICE_MAX_NB )
	{
		//o_str CONST & o = o_device_name[index];
		//if( !o.is_empty() )
		return &o_device_name[index];
	}
	return &o_str::o_empty;
}

void	c_capture_display::c_init()
{
#if defined(WIN32)
	// Initialize COM
	//c_COM::init();
	if( n_os_version::b_win8_or_more )
	{
		b_can = true;
		//do_enum( true );
	}
	else
#endif
	{
		b_can = false;
		ERR_PRINT_STRING( "Capture of a display is only supported on Windows 8 and up" );
	}
}

void	c_capture_display::c_deinit()
{
#ifdef WIN32
	//c_COM::close();
#endif
}

c_capture_display::c_capture_display()
	//:_buffer_data			(	nullptr	)
	:_display_cap_ui		(	nullptr	)
	,_cap_size_x			(	0		)
	,_cap_size_y			(	0		)
	,_cap_framerate			(	0		)
	,_b_device_opened		(	false	)
	,_d3d11_device			(	nullptr )
	,_dxgi_factory2			(	nullptr )
	,_device_context		(	nullptr )
	,_out_duplicate			(	nullptr )
	,_dxgi_device			(	nullptr	)
	,_dxgi_adapter			(	nullptr	)
	,_desktop_tex			(	nullptr	)
	,_d3d11_tex2d			(	nullptr	)
	,_d3d11_sx				(	0		)
	,_d3d11_sy				(	0		)
	,_thread_grabber		(	nullptr	)
	,_b_thread_closing		(	false	)
	//,_dxgi_debug			(	nullptr	)
{
	_b_stream = false;
	set_frame_callback( false );
	_o_crossbar_name.set( "None" );
}

c_capture_display::~c_capture_display()
{
	close();
	close_thread();
}


void	c_capture_display::ask_frame()
{
#if defined(WIN32)
	//if( _dxgi_debug )
	//		{
	//			hr = _dxgi_debug->ReportLiveDeviceObjects(D3D11_RLDO_SUMMARY);
	//			if( FAILED( hr ) )
	//				DBG_PRINT_STRING( "Reporting" );
	//		}

	if( !_device_context )
		return;

	// Get new frame
	bool	b_need_frame_release = false;
	HRESULT						hr;
	IDXGIResource*				desktop_resource = nullptr;
	DXGI_OUTDUPL_FRAME_INFO		frame_info;
	hr = _out_duplicate->AcquireNextFrame( _b_use_thread ? 500 : 0, &frame_info, &desktop_resource );

	if( hr == DXGI_ERROR_WAIT_TIMEOUT )
	{
//		ERR_PRINT_STRING( "%s() Time_out", __FUNCTION__ );
		goto exit;
	}
	if( hr == DXGI_ERROR_ACCESS_LOST )	// && (hr != DXGI_ERROR_INVALID_CALL) )
	{
		//we should stop close and reopen
//		ERR_PRINT_STRING( "%s() DXGI_ERROR_ACCESS_LOST The keyed mutex was abandoned.", __FUNCTION__ );
		goto exit;
	}
	if( FAILED(hr) )
	{
		err_print( "%s() Failed to acquire next frame, hr is %08X", __FUNCTION__, hr );
		goto exit;
	}

	b_need_frame_release = true;
	if( !_desktop_tex )
	{
		hr = desktop_resource->QueryInterface( IID_PPV_ARGS(&_desktop_tex) );
		if( FAILED( hr ) )
		{
			err_print( "%s() Failed to QueryInterface, hr is %08X", __FUNCTION__, hr );
			goto exit;
		}
	}
	if( _desktop_tex )
	{
		_b_crop = _display_cap_ui->is_crop();
		_display_cap_ui->get_crop( &_crop_region );

		D3D11_TEXTURE2D_DESC tex_description;
		_desktop_tex->GetDesc( &tex_description );

		INT32 sx = tex_description.Width;
		INT32 sy = tex_description.Height;

		tex_description.BindFlags		= 0;
		tex_description.CPUAccessFlags	= D3D11_CPU_ACCESS_READ;	// | D3D11_CPU_ACCESS_WRITE;
		tex_description.Usage			= D3D11_USAGE_STAGING;
		tex_description.MipLevels		= 1;
		tex_description.ArraySize		= 1;
		tex_description.MiscFlags		= 0;
		//for interop
		if( false )
		{
			tex_description.MiscFlags			= D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;
			tex_description.Format				= DXGI_FORMAT_B8G8R8A8_UNORM; // compatible with OpenGL
			tex_description.SampleDesc.Count	= 1;
			tex_description.Usage				= D3D11_USAGE_DEFAULT;
			tex_description.BindFlags			= D3D11_BIND_RENDER_TARGET;
			tex_description.CPUAccessFlags		= 0;                     // no CPU access needed
		}


		INT32 sx_asked = sx;
		INT32 sy_asked = sy;
		if( _b_crop )
		{
			sx_asked -= _crop_region.right + _crop_region.left;
			sy_asked -= _crop_region.top + _crop_region.bottom;
		}

		//sx_asked &= 0xffffffe0;	// have apparently to be a mnultiple of 32 for CopySubresourceRegion to function
		//sx_asked &= 0xfffffff8;	// have apparently to be a mnultiple of 32 for CopySubresourceRegion to function
		if( sx_asked > 0 && sy_asked > 0 )
		{
			// create/recreate texture whem needed: first time or size changed
			if( _d3d11_sx != sx_asked || _d3d11_sy != sy_asked )
			{
				release_d3d11_tex();
				tex_description.Width = sx_asked;
				tex_description.Height = sy_asked;
				hr = _d3d11_device->CreateTexture2D( &tex_description, nullptr, &_d3d11_tex2d );
				if( SUCCEEDED(hr) )
				{
					_d3d11_sx = sx_asked;
					_d3d11_sy = sy_asked;
				}
				else
					err_print( "%s() CreateTexture2D() failed", __FUNCTION__ );
			}
			if( _d3d11_tex2d )
			{
				if( sx != sx_asked || sy != sy_asked )
				{
					D3D11_BOX	src_box;
					if( _b_crop )
					{
						// Copy rect out of shared surface
						src_box.left	= _crop_region.left;
						src_box.top		= _crop_region.top;
						src_box.right	= _crop_region.left + sx_asked;
						src_box.bottom	= _crop_region.top  + sy_asked;
					}
					else
					{
						src_box.left	= 0;
						src_box.top		= 0;
						src_box.right	= sx_asked;
						src_box.bottom	= sy_asked;
					}
					src_box.front = 0;	// front = 0 and back = 1 for 2D Texture...
					src_box.back  = 1;

					_device_context->CopySubresourceRegion( _d3d11_tex2d, 0,0, 0,0, _desktop_tex, 0, &src_box );
				}
				else
					_device_context->CopyResource( _d3d11_tex2d, _desktop_tex );

				D3D11_MAPPED_SUBRESOURCE	resource;
				UINT32						subresource = D3D11CalcSubresource( 0, 0, 0 );

				HRESULT hr = _device_context->Map( _d3d11_tex2d, subresource, D3D11_MAP_READ, 0, &resource );	//was D3D11_MAP_READ_WRITE
				if( FAILED(hr) )
				{
					err_print( "%s() Map Failed", __FUNCTION__ );
					goto exit;
				}
				if( resource.pData == nullptr )
				{
					err_print( "%s() Texture Data pointer is null", __FUNCTION__ );
					goto exit;
				}

				if( _cap_size_x != sx_asked || _cap_size_y != sy_asked )
				{
					_cap_size_x = sx_asked;
					_cap_size_y = sy_asked;
					set_flux_size_format( _cap_size_x, _cap_size_y, aaa::PIXEL_FORMAT::RGBA_8 );

					//set_src_pixel_format( aaa::PIXEL_FORMAT::BGRA_8 );
		//			SAFE_DELETE( _buffer_data );
				}
				UINT32 pitch = resource.RowPitch; // was sx_asked << 2; but ressource is the truth
				CONST UINT8* source = static_cast<CONST UINT8*>(resource.pData);
				set_src_y_inverted( true );
				set_src_pixel_format( aaa::PIXEL_FORMAT::BGRA_8 );
				//set_swap_red_blue( true );
				//set_src_pitch( pitch );
				got_frame( source, "capture display", pitch, true, 1. );
			
				_device_context->Unmap( _d3d11_tex2d, subresource );
			}
		}
	}
exit:

	if( b_need_frame_release )
	{
		hr = _out_duplicate->ReleaseFrame();
		if( FAILED( hr ) )
			DBG_PRINT_STRING( "Failed to release frame in %s()", __FUNCTION__ );
	}
	sysutils::safe_release( desktop_resource );
	//_device_context->ClearState();
	//_device_context->Flush();
#endif
	return;
}

void	c_capture_display::update()
{
	if( _b_running )
	{
		update_threading();
		if( !_b_use_thread )
		{
			if( _out_duplicate )
				ask_frame();
		}
	}
}

namespace {
	// Driver types supported
	CONSTEXPR D3D_DRIVER_TYPE driver_types[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	CONSTEXPR UINT DRIVER_TYPE_NB = ARRAYSIZE( driver_types );
	CONSTEXPR C_PCHAR_C	driver_type_names[DRIVER_TYPE_NB] =
	{
		"Hardware",
		"Warp",
		"reference"
	};
	D3D_FEATURE_LEVEL feature_levels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_1
	};
}

/*
INT32 enum_d3d11_adapter()
{
	INT32 display_nb = 0;
	// Enum Adapters first : multiple video cards
	IDXGIFactory1*	dxgi_factory1;
	//	if ( FAILED( CreateDXGIFactory1( __uuidof( IDXGIFactory1 ), (void**)&dxgi_device_ptr ) ) )
	if ( FAILED( CreateDXGIFactory1( __uuidof(IDXGIFactory1), (void**)&dxgi_factory1 ) ) )
	{
		err_print( "Could not create CreateDXGIFactory1" );
	}

	DBG_PRINT_STRING( "Enum using DXGI FACTORY 1 for info only");
	UINT32	i = 0;
	IDXGIAdapter*	adapter1_ptr = nullptr;
	for( UINT32 i = 0; dxgi_factory1->EnumAdapters( i, &adapter1_ptr ) != DXGI_ERROR_NOT_FOUND; i++ )
	{
		DBG_PRINT_STRING( "D3D11 Adapter %d found", i );

		DXGI_ADAPTER_DESC	desc;
		adapter1_ptr->GetDesc( &desc );
		DBG_PRINT_STRING( "  Adapter   : %S", desc.Description );
		DBG_PRINT_STRING( "  Vendor Id : %d", desc.VendorId );
		DBG_PRINT_STRING( "  Dedicated System Memory MB : %.0f", (REAL)desc.DedicatedSystemMemory / (1024.f * 1024.f) );
		DBG_PRINT_STRING( "  Dedicated Video Memory  MB : %.0f", (REAL)desc.DedicatedVideoMemory  / (1024.f * 1024.f) );
		DBG_PRINT_STRING( "  Shared System Memory    MB : %.0f", (REAL)desc.SharedSystemMemory    / (1024.f * 1024.f) );
		CComPtr<IDXGIOutput>	p_output;
	//	IDXGIOutput*	p_output;
		for( UINT32 j = 0; adapter1_ptr->EnumOutputs( j, &p_output ) != DXGI_ERROR_NOT_FOUND; j++ )
		{
			DXGI_OUTPUT_DESC	desc_out;
			p_output->GetDesc( &desc_out );
			DBG_PRINT_STRING( "  Output %d -> Name %S", j, desc_out.DeviceName );
			DBG_PRINT_STRING( "    Attached to desktop : %s", desc_out.AttachedToDesktop ? "Yes" : "no" );
			//GOOD_PRINT_STRING( "    Rotation", desc_out.DeviceName );
			DBG_PRINT_STRING( "    Left   : %d", desc_out.DesktopCoordinates.left   );
			DBG_PRINT_STRING( "    Top    : %d", desc_out.DesktopCoordinates.top    );
			DBG_PRINT_STRING( "    Right  : %d", desc_out.DesktopCoordinates.right  );
			DBG_PRINT_STRING( "    Bottom : %d", desc_out.DesktopCoordinates.bottom );
			p_output.Release();
			++display_nb; 
		}
		//if( index == i )
		//{
		//	return adapter1_ptr;
		//}
		adapter1_ptr->Release();
		//	++i;
	}
	//return nullptr;
	return display_nb;
}
*/
INT32	c_capture_display::do_enum( bool CONST	b_verbose )
{
//	return enum_d3d11_adapter();
	return 1;
}

//todo use IDXGIFactory1::EnumAdapters.
AAA_ERR	c_capture_display::open_device()
{
	if( !b_can )
		return ERR_ANY;

#if defined(WIN32)
	HRESULT hr;


	// Feature levels supported
	UINT				num_feature_levels = ARRAYSIZE( feature_levels );
	D3D_FEATURE_LEVEL	feature_level;
	UINT				creationFlags = 0;		//D3D11_CREATE_DEVICE_DEBUG;
	// Create device
	//todo a little crude
#if 0
	for( UINT i = 0; i < DRIVER_TYPE_NB; ++i )
	{
		hr = D3D11CreateDevice( nullptr, driver_types[i], nullptr, creationFlags, feature_levels, num_feature_levels, D3D11_SDK_VERSION, &_d3d11_device, &feature_level, &_device_context );
		if( FAILED( hr ) )
		{
			ERR_PRINT_STRING( "Device creation failed in %s() for device type %s", __FUNCTION__, driver_type_names[i] );
			continue;
		}
		CAPTURE_PRINT_STRING( "D3D11CreateDevice() done for device type %s", driver_type_names[i] );
		// Device creation succeeded, no need to loop anymore
		break;
	}
#else	// only HARDWARE is usable 
		hr = D3D11CreateDevice( nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, creationFlags, feature_levels, num_feature_levels, D3D11_SDK_VERSION, &_d3d11_device, &feature_level, &_device_context );
		if( FAILED(hr) )
		{
			ERR_PRINT_STRING( "Device creation failed in %s() for device type %s", __FUNCTION__, driver_type_names[D3D_DRIVER_TYPE_HARDWARE] );
			return ERR_ANY;
		}
		CAPTURE_PRINT_STRING( "D3D11CreateDevice() done for device type %s", driver_type_names[D3D_DRIVER_TYPE_HARDWARE] );
		// Device creation succeeded, no need to loop anymore
#endif

	//hr = _d3d11_device->QueryInterface(IID_PPV_ARGS(&_dxgi_debug));
	
	// Get DXGI factory
	hr = _d3d11_device->QueryInterface( IID_PPV_ARGS(&_dxgi_device) );
	//hr = _d3d11_device->QueryInterface( __uuidof(IDXGIDevice), reinterpret_cast<void**>(&_dxgi_device) );
	if( FAILED( hr ) )
	{
		ERR_PRINT_STRING( "Failed to QueryInterface for DXGI Device in %s()", __FUNCTION__ );
		return ERR_ANY;
	}

	hr = _dxgi_device->GetParent( IID_PPV_ARGS(&_dxgi_adapter) );
	if( FAILED( hr ) )
	{
		ERR_PRINT_STRING( "Failed to get parent DXGI Adapter in %s()", __FUNCTION__ );
		return ERR_ANY;
	}

	hr = _dxgi_adapter->GetParent( IID_PPV_ARGS(&_dxgi_factory2) );
	if( FAILED( hr ) )
	{
		ERR_PRINT_STRING( "Failed to get parent DXGI Factory in %s()", __FUNCTION__ );
		return ERR_ANY;
	}

	list_display();

	return AAA_OK;
#else
	return ERR_ANY;
#endif

}

void	c_capture_display::release_d3d11_tex()
{
#if defined(WIN32)
	_d3d11_sx = 0;
	_d3d11_sy = 0;
	sysutils::safe_release( _d3d11_tex2d );
#endif
}

AAA_ERR	c_capture_display::close_device()
{
#if defined(WIN32)
	release_d3d11_tex();
	sysutils::safe_release( _desktop_tex	);
	sysutils::safe_release( _dxgi_adapter	);
	sysutils::safe_release( _dxgi_device	);
	sysutils::safe_release( _d3d11_device	);
	sysutils::safe_release( _dxgi_factory2	);
	sysutils::safe_release( _device_context	);
	sysutils::safe_release( _out_duplicate	);
#endif
	_b_device_opened = false;
	_b_init = false;
	return AAA_OK;
}

AAA_ERR	c_capture_display::open_specific( INT32 CONST index, REAL CONST framerate, o_str CONST * CONST device_name )
{
	if( !_b_device_opened )
	{
		_b_device_opened = open_device() == AAA_OK;
		if( !_b_device_opened )
			return ERR_ANY;
	}
		
	if( !_b_opened )
	{
		_b_opened = open_low( index ) == AAA_OK;
		if( _b_opened )
			_device_index_used = index;
	//	_cap_size_x = 10;
	//	_cap_size_y = 10;
	//	init_with_size( 10, 10, 4, __FUNCTION__ );
	//	set_source_grey( false );
	//	set_bgr( false );
	//	set_src_bit_per_pixel( 32 );
	//	set_pixel_format( c_pixel_format::RGB_32 );
	//	set_src_pixel_format( aaa::PIXEL_FORMAT::RGBA );
	}
	return _b_opened ? AAA_OK : ERR_ANY;
}

void	c_capture_display::list_display()
{
#if defined(WIN32)
	HRESULT	hr = S_OK;

	hr = _dxgi_device->GetParent( IID_PPV_ARGS(&_dxgi_adapter) );
	if( FAILED( hr ) )
	{
		ERR_PRINT_STRING( "Failed to get parent DXGI Adapter in %s()", __FUNCTION__ );
		return;
	}

	IDXGIOutput* dxgi_output = nullptr;
	// Get # of outputs available
	INT32 index = 0;
	hr = S_OK;
	for( UINT32 i = 0; SUCCEEDED( hr ); ++i )
	{
		sysutils::safe_release( dxgi_output );
		hr = _dxgi_adapter->EnumOutputs( i, &dxgi_output );
		if( dxgi_output && (hr != DXGI_ERROR_NOT_FOUND) )
		{
			
			DXGI_OUTPUT_DESC out_desc;
			dxgi_output->GetDesc( &out_desc );
			auto & r = out_desc.DesktopCoordinates;
			INT32 sx = r.right - r.left;
			INT32 sy = r.bottom - r.top;
			o_str & o = o_device_name[index]; 
			++index;

			o.set( index );
			o.add(": ");
			o.add( out_desc.DeviceName );
			o.add(" ");
			o.add(sx);
			o.add_char('x');
			o.add(sy);
			o.add(" @ ");
			o.add( INT32(r.left) );
			o.add_char(',');
			o.add( INT32(r.top) );

			CAPTURE_PRINT_STRING( "Display %s", o.get() );
		}
	}
	device_count = index;	// ok we confuse device and display in this case
	sysutils::safe_release( dxgi_output );

	for( ; index < c_capture::DEVICE_MAX_NB; ++index )
		o_device_name[index].erase();
	inc_device_enum_count();
#endif
}

AAA_ERR	c_capture_display::open_low( UINT32 index )
{
#if defined(WIN32)
	if( IS_NULL( _d3d11_device ) )
	{
		ERR_PRINT_STRING( "No Device" );
		return ERR_ANY; 
	}

	HRESULT		hr = S_OK;

	// Get output
	IDXGIOutput* dxgi_output = nullptr;
	hr = _dxgi_adapter->EnumOutputs( index, &dxgi_output );
	if( FAILED( hr ) )
	{
		ERR_PRINT_STRING( "Failed to get specified output in %s()", __FUNCTION__ );
		return ERR_ANY;
	}

	// evetually store and reuse 
	//DXGI_OUTPUT_DESC	m_OutputDesc;
	//dxgi_output->GetDesc( &m_OutputDesc );

	// QI for Output 1
	IDXGIOutput1* dxgi_output1 = nullptr;
	hr = dxgi_output->QueryInterface( __uuidof(dxgi_output1), reinterpret_cast<void**>(&dxgi_output1) );
	sysutils::safe_release( dxgi_output );
	if( FAILED( hr ) )
	{
		ERR_PRINT_STRING( "Failed to QI for DxgiOutput1 in %s()", __FUNCTION__ );
		return ERR_ANY;
	}

	// Create desktop duplication
	hr = dxgi_output1->DuplicateOutput( _d3d11_device, &_out_duplicate );
	sysutils::safe_release( dxgi_output1 );
	if( FAILED( hr ) )
	{
		if( hr == DXGI_ERROR_NOT_CURRENTLY_AVAILABLE )
		{
			ERR_PRINT_STRING( "There is already the maximum number of applications using the Desktop Duplication API running, please close one of those applications and then try again." );
			return ERR_ANY;
		}
		ERR_PRINT_STRING( "Failed to get duplicate output in %s()", __FUNCTION__ );
		return ERR_ANY;
	}

	// Take a reference on the device
	_d3d11_device->AddRef();
	return AAA_OK;
#else
	return ERR_ANY;
#endif
}

void	c_capture_display::close_low()
{
	if( _b_opened )
	{
		CAPTURE_PRINT_STRING( "%s() begin", __FUNCTION__ );
		close_device();
//		FREE_AND_NULL( _buffer_data );
		CAPTURE_PRINT_STRING( "%s() done", __FUNCTION__ );
		_b_opened = false;
	//	_cap_size_x = 0;
	//	_cap_size_y = 0;
	}
}

void	c_capture_display::close_specific()
{
	if( _b_opened ) 
	{
		CAPTURE_PRINT_STRING( "%s() begin", __FUNCTION__ );
		stop();
		close_low();
		CAPTURE_PRINT_STRING( "%s() done", __FUNCTION__ );
	}
}

void	c_capture_display::update_threading()
{
	bool b_use_thread = _display_cap_ui ? _display_cap_ui->is_threaded() : false;

	if( b_use_thread != _b_use_thread)
	{
		set_frame_callback( b_use_thread );
		if( b_use_thread )
			init_thread();
		else
			close_thread();
		_b_use_thread = b_use_thread;
		_b_stream = _b_use_thread;
	}
	
}
bool	c_capture_display::run( bool b_stream_in )
{
	if( _b_opened && !_b_running )
	{
		update_threading();
		_b_init = true;

		ask_frame();	// hack : run once to get image_flux initialized
		_b_running = true;

	//	bool	b_run = false;
	}
	return _b_running;
}

void	c_capture_display::stop()
{
	if( _b_running )
	{
		_b_running = false;
		if( _b_use_thread )
		{
			_b_thread_closing = true;
			close_thread();
		}
	}
}

void	c_capture_display::update_async()
{
	if( _b_thread_closing )
		return;
	ask_frame();
}

void	c_capture_display::init_thread()
{
	if( IS_NULL( _thread_grabber ) )
	{
		_thread_grabber = new c_thread_grabber;
		_thread_grabber->create( this, 0 );
		_thread_grabber->start();
	}
}

void	c_capture_display::close_thread()
{
	if( IS_NOT_NULL( _thread_grabber ) )
	{
		_thread_grabber->shutdown();
		_thread_grabber->join();
		SAFE_DELETE( _thread_grabber );
		_b_use_thread = false;
		_b_thread_closing = false;
	}
}
