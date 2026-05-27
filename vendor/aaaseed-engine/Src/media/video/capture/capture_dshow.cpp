#include "capture_dshow.h"
#if AAA_USE_DS_TRANSFORM_FILTER()
#	include "../DirectShow/ds_transform_filter.h"
#else
#	include "../DirectShow/ds_renderer_filter.h"
#endif

#include "../DirectShow/ds_util.h"
#include "../DirectShow/qedit_maa.h"
#include "capture_dshow_ui.h"

#include "infrastructure/aaa_window.h"
#include "ui/keyboard.h"	 
#include "system/shared/SystemUtils.h"	// for safe_release
 


#ifndef _UEYE_CAPTURE_INTERFACE_
#	include "uEye/uEyeCaptureInterface.h"
#endif


//
//	DEVICE NAME
//
static INT32						device_count = 0;
static o_str						o_device_name[c_capture::DEVICE_MAX_NB];
static INT32						device_mode_supported_nb[c_capture::DEVICE_MAX_NB];
static ds_helper::st_media_info		device_mode_arrays   [c_capture::DEVICE_MAX_NB] [c_ds_cap_ui::DEVICE_MODE_MAX_NB];
static INT32						device_picked        [c_capture::DEVICE_MAX_NB] [c_capture_dshow::PICK_NB];

INT32	c_capture_dshow::get_device_enumed()
{
	return device_count;
}
o_str CONST * CONST c_capture_dshow::get_device_name( INT32 CONST index )
{
	if( 0 <= index && index < c_capture::DEVICE_MAX_NB )
	{
		//o_str CONST & o = o_device_name[index];
		//if( !o.is_empty() )
		return &o_device_name[index];
	}
	return &o_str::o_empty;
}
o_str CONST * CONST c_capture_dshow::get_mode_supported_text( INT32 CONST device_index, INT32 CONST index )
{
	if( 0 <= device_index && device_index < c_capture::DEVICE_MAX_NB )
	{
		if( 0 <= index && index < c_ds_cap_ui::DEVICE_MODE_MAX_NB )
			return &device_mode_arrays[device_index][index].o_text;
	}
	return &o_str::o_empty;
}

INT32 CONST * c_capture_dshow::get_device_mode_supported_nb_pt( INT32 CONST device_index )
{
	if( 0 <= device_index && device_index < c_capture::DEVICE_MAX_NB )
		return &device_mode_supported_nb[device_index];
	return nullptr;
}


#define DS_CHECK( call_expr, msg )		\
{										\
	if( FAILED( hr = (call_expr) ) )	\
	{									\
		CAPTURE_PRINT_STRING( "DShow : %s, error : %s", msg, ds_helper::get_error_string( hr ) );	\
		return ERR_ANY;					\
	}									\
}

static	bool	find_name_and_path( CComPtr<IPropertyBag> p_property_bag, CHAR* p_buf_name, CHAR* p_buf_devicepath )
{
	p_buf_devicepath[0] = 0;
	p_buf_name[0] = 0;

	VARIANT		var_devicepath;
	VARIANT		var_name;

	var_devicepath.vt = VT_BSTR;
	var_name.vt = VT_BSTR;

	if( SUCCEEDED( p_property_bag->Read( L"FriendlyName", &var_name, 0 ) ) )
	{
		bool	b_device_path_valid = SUCCEEDED( p_property_bag->Read( L"DevicePath", &var_devicepath, 0 ) );
		if( b_device_path_valid )
			WideCharToMultiByte( CP_ACP, 0, var_devicepath.bstrVal, -1, p_buf_devicepath, 512, nullptr, nullptr );

		WideCharToMultiByte( CP_ACP, 0, var_name.bstrVal, -1, p_buf_name, 512, nullptr, nullptr );

		if( b_device_path_valid )
			SysFreeString( var_devicepath.bstrVal );
		SysFreeString( var_name.bstrVal );
		return true;
	}
	return false;
}

static	CComPtr<IEnumMoniker>	create_enum_moniker()
{
	CComPtr<ICreateDevEnum>	p_dev_enum;
	CComPtr<IEnumMoniker>	local_p_enum_moniker = nullptr;

#if DS_MAA
	if( SUCCEEDED( p_dev_enum.CoCreateInstance( CLSID_SystemDeviceEnum ) ) )
#else
	if( SUCCEEDED( CoCreateInstance( CLSID_SystemDeviceEnum, nullptr, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, reinterpret_cast<void**>(&p_dev_enum) ) ) )
#endif
	{
		p_dev_enum->CreateClassEnumerator( CLSID_VideoInputDeviceCategory, &local_p_enum_moniker, 0 );
		p_dev_enum = nullptr;
	}
	return local_p_enum_moniker;
}



INT32	c_capture_dshow::do_enum( bool const b_verbose )
{
	INT32					index = 0;
	CComPtr<IEnumMoniker>	pt_enum_moniker = create_enum_moniker();

	//todo franz	this function is almost equal to c_capture_dshow::find_device(), merge the 2, but this one is static...
	if( pt_enum_moniker )
	{
		CComPtr<IMoniker>	p_moniker;
		ULONG				fetched;
		while( SUCCEEDED( pt_enum_moniker->Next( 1, &p_moniker, &fetched ) ) && fetched==1 )
		{
			CComPtr<IPropertyBag>	p_prop_bag;
			if( SUCCEEDED( p_moniker->BindToStorage( 0, 0, IID_IPropertyBag, reinterpret_cast<void**>(&p_prop_bag) ) ) )
			{
				CHAR	buf_devicepath[512];
				CHAR	buf_name[512];
				if( find_name_and_path( p_prop_bag, buf_name, buf_devicepath ) )
				// FH08	other property available : Description, CLSID
				// DevicePath is guaranteed to be unique per device
				{
					if( b_verbose )
					{
						bool		b_is_VFW_driver = false;
						LPOLESTR	display_name;

						if( SUCCEEDED( p_moniker->GetDisplayName( nullptr, nullptr, &display_name ) ) )
						{
							// Detect a VFW driver by the compression manager tag.
							if( !wcsncmp( display_name, L"@device:cm:", 11 ) )
								b_is_VFW_driver = true;
							CoTaskMemFree( display_name );
						}
						CAPTURE_PRINT_STRING( "DirectShow %sDevice %d : %s (%s)",
													b_is_VFW_driver ? "VFW " : "",
													index, buf_name, buf_devicepath );
					}
					if( index < c_capture::DEVICE_MAX_NB ) 
					{
						o_device_name[index].set( buf_name );
						//c_tex_video::enum_cur->set_capture_device_name( index_enum, buf_name );
						//o_device_name
						enumerate_device_mode( index, p_moniker );
						CAPTURE_PRINT_STRING( "" );
					}
					else
					{
						ERR_PRINT_STRING( "Direct Show Capture in AAASeed only support %d deviced, this device can be used.", c_capture::DEVICE_MAX_NB );
						break;
					}
					++index;
				}
				p_prop_bag = nullptr;
			}
			p_moniker = nullptr;
		}
		pt_enum_moniker = nullptr;
	}

	device_count = index;
	for( ; index < c_capture::DEVICE_MAX_NB; ++index )
		o_device_name[index].erase();
	inc_device_enum_count();

	return device_count;
}

void	c_capture_dshow::c_init()
{
	//c_COM::init();

	do_enum( true );
}

void	c_capture_dshow::c_deinit()
{
	//c_COM::close();
}

c_capture_dshow::c_capture_dshow()
	:_p_aaa_filter					( nullptr	)
	,_b_graph_built					( false		)
	,_b_graph_running				( false		)
	,_b_graph_has_preview			( false		)
	,_p_video_device_moniker		( nullptr	)
	,_p_graph_builder				( nullptr	)
	,_p_cap_graph_builder			( nullptr	)
	,_p_graph_control				( nullptr	)
//	,_p_media_event_ex				( nullptr	)
	,_p_cap_filter					( nullptr	)
	,_p_real_capture_pin			( nullptr	)	// the one on the cap filt
	,_p_real_preview_pin			( nullptr	)	// the one on the cap filt
	,_p_cap_filter_video_port_pin	( nullptr	)	// on cap filt
	,_p_crossbar					( nullptr	)
	,_p_crossbar2					( nullptr	)
	,_p_tuner						( nullptr	)
	,_p_tv_tuner					( nullptr	)
	,_p_video_crossbar				( nullptr	)	// This aliases either p_crossbar or p_crossbar2.
	,_p_vfw_dialogs					( nullptr	)
	,_p_video_config_cap			( nullptr	)
	,_p_video_config_prv			( nullptr	)
	,_video_crossbar_output			( 0			)
	,_p_video_proc_amp				( nullptr	)//
	,_p_dropped_frames				( nullptr	)
	,_cap_size_x					( 0			)
	,_cap_size_y					( 0			)
	,_cap_fps						( 0			)
	,_b_dv_supported				( false		)
	,_b_use_format_index			( false		)
	,_format_index					( -1		)
	,_b_ueye						( false		)
	,_ds_ui							( nullptr	)
	,_eye_bandwidth					( 0			)
	,_eye_exposure_time				( 0			)
	,_eye_exposure_range_min		( 0			)
	,_eye_exposure_range_max		( 0			)
	,_eye_exposure_range_interval	( 0			)
	,_eye_rgb_model_mode			( 0			)
	,_p_eye_capture_ex				( nullptr	)
	,_p_eye_color_temp				( nullptr	)
	,_p_cap_ueye_pin				( nullptr	)
	,_p_cap_ueye					( nullptr	)
	,_b_eye_capture_ex				( false		)
	,_brightness					( REAL_BIG_VALUE	)	//	these init are here to make sure that there will be always an update the first time
	,_contrast						( REAL_BIG_VALUE	)
	,_hue							( REAL_BIG_VALUE	)
	,_saturation					( REAL_BIG_VALUE	)
	,_sharpness						( REAL_BIG_VALUE	)
	,_gamma							( REAL_BIG_VALUE	)
	,_color_enable					( REAL_BIG_VALUE	)
	,_white_balance					( REAL_BIG_VALUE	)
	,_backlight_compensation		( REAL_BIG_VALUE	)
	,_gain							( REAL_BIG_VALUE	)

{
	_b_stream = true;
	_o_crossbar_name.set( "Unknown" );

#if	AAA_DEBUG()
	_register_graph = 0;
#endif

	// Initialize COM
//	if(FAILED( CoInitializeEx( nullptr, COINIT_APARTMENTTHREADED ) ) )
//	if( FAILED( CoInitializeEx( nullptr, COINIT_MULTITHREADED ) ) )
//	{
//		CAPTURE_PRINT_STRING( "DirectShow c_capture_dshow::c_capture_dshow() CoInitialize Failed!" );
//	}
}

c_capture_dshow::~c_capture_dshow()
{
	close();
	sysutils::safe_release( _p_aaa_filter );
	//// Release COM
 //	CoUninitialize();
}

void	c_capture_dshow::update()
{
	if( _b_running )
	{		
		if( _ds_ui )
		{
			_b_use_format_index_asked = _ds_ui->is_use_format_index();
			_format_index_asked       = _ds_ui->get_format_index_asked();
			//todo move in tex_video
			if( _b_use_format_index_asked && _format_index_asked != _format_index )
				close();
		}
		if( _p_dropped_frames )
		{
			LONG frame_nb;
			if( FAILED( _p_dropped_frames->GetNumDropped( &frame_nb ) ) )
				_frames_dropped = -1;
			else
				_frames_dropped = frame_nb;
			if( FAILED( _p_dropped_frames->GetNumNotDropped( &frame_nb ) ) )
				_frames_delivered = -1;
			else
				_frames_delivered = frame_nb;
		}
		else
		{
			_frames_delivered = -1;
			_frames_dropped = -1;
		}
	}
	if( _b_ueye )
		update_ueye_settings();
}

void	c_capture_dshow::dlg_source()
{
	if( _p_crossbar )
		display_property_pages( _p_crossbar, get_window_main_handle() );
	if( _p_crossbar2 )
		display_property_pages( _p_crossbar2, get_window_main_handle() );
}

void	c_capture_dshow::dlg_format()
{
	if( _p_real_capture_pin )
	{
		if( _b_graph_running )
		{
			stop_graph();
			display_property_pages( _p_real_capture_pin, get_window_main_handle() );
			run();
		}
		else
			display_property_pages( _p_real_capture_pin, get_window_main_handle() );
	}
}

void	c_capture_dshow::dlg_display()
{
	if( _p_cap_filter )
		display_property_pages( _p_cap_filter, get_window_main_handle() );
}

bool	c_capture_dshow::run( bool b_stream_in )
{
	if( !_b_opened )
		return false;
	if( _b_running )
		return true;

	if( start_graph() )
	{
		_b_running = true;
		_b_streaming = true;
		return true;
	}
	return false;
}

void	c_capture_dshow::stop()
{
	stop_graph();
	_b_running = false;
}

//#include <d3d11.h>
//#include <dxgi1_2.h>
////dxgi.lib; d3d11.lib
//	HRESULT hr;
//
//	// Store window handle
//	//m_WindowHandle = Window;
//
//	// Driver types supported
//	D3D_DRIVER_TYPE DriverTypes[] =
//	{
//		D3D_DRIVER_TYPE_HARDWARE,
//		D3D_DRIVER_TYPE_WARP,
//		D3D_DRIVER_TYPE_REFERENCE,
//	};
//	UINT NumDriverTypes = ARRAYSIZE(DriverTypes);
//
//	// Feature levels supported
//	D3D_FEATURE_LEVEL FeatureLevels[] =
//	{
//		D3D_FEATURE_LEVEL_11_0,
//		D3D_FEATURE_LEVEL_10_1,
//		D3D_FEATURE_LEVEL_10_0,
//		D3D_FEATURE_LEVEL_9_1
//	};
//	UINT NumFeatureLevels = ARRAYSIZE(FeatureLevels);
//	D3D_FEATURE_LEVEL FeatureLevel;
//
//	// Create device
//	ID3D11Device* m_Device;
//	ID3D11DeviceContext* m_DeviceContext;
//	for (UINT DriverTypeIndex = 0; DriverTypeIndex < NumDriverTypes; ++DriverTypeIndex)
//	{
//		hr = D3D11CreateDevice(nullptr, DriverTypes[DriverTypeIndex], nullptr, 0, FeatureLevels, NumFeatureLevels,
//			D3D11_SDK_VERSION, &m_Device, &FeatureLevel, &m_DeviceContext);
//		if (SUCCEEDED(hr))
//		{
//			// Device creation succeeded, no need to loop anymore
//			break;
//		}
//	}
//
//

void	c_capture_dshow::save_graph()
{
	save_graph( _p_graph_builder, L"aaaseedgraph.grf" );
}

AAA_ERR	c_capture_dshow::open_specific( INT32 CONST index_asked, REAL CONST framerate, o_str CONST * CONST device_clsid )
{
	if( _ds_ui )
	{
		_b_use_format_index_asked = _ds_ui->is_use_format_index();
		_format_index_asked       = _ds_ui->get_format_index_asked();
	}
	if( ( !device_clsid && _device_index_used >= 0 && index_asked != _device_index_used )
			|| _b_use_format_index_asked != _b_use_format_index
			|| (_format_index_asked != _format_index && _b_use_format_index_asked )
		)
		close();
	if( _b_opened || _b_graph_running )
		return AAA_OK;

	INT32 index_found = find_device( index_asked, device_clsid );
	if( index_found < 0 )
	{
		if( device_clsid )
			ERR_PRINT_STRING( "DirectShow, device %s not present", device_clsid );
		else
			ERR_PRINT_STRING( "DirectShow, device index %d not present", index_asked );
		return ERR_ANY;
	}

	_device_index_being_built = index_found;

	if( !_b_graph_built && ERR(init_graph_device( framerate, _b_use_format_index_asked, _format_index_asked )) )
	{
		// problem during initialization
		goto exit_on_error;
	}
	_b_use_format_index = _b_use_format_index_asked;
	_format_index = _format_index_asked;

	modifier::update();
	if( modifier::is_shift_on() )
		dlg();

	if( ERR(init_graph_capture()) )
	{
		CAPTURE_PRINT_STRING( "DirectShow couldn't build graph" );
		goto exit_on_error;
	}

	//if( _p_capture_pin )
	//{
	//	AM_MEDIA_TYPE media_type;
	//	_p_capture_pin->ConnectionMediaType( &media_type );
	//		ds_helper::st_media_info info;
	//		ds_helper::get_media_info( &media_type, info );
	//	FreeMediaType( media_type );
	//
	//	_cap_size_x = info.size_x;
	//	_cap_size_y = info.size_y;

	//	//set_pixel_type( PIXEL_TYPE::RGBA );
	////	set_src_pixel_format( info.pixel_format );
	////	set_bgr( c_pixel_format::is_bgr( info.pixel_format ) );
	//	if( !aaa::c_pixel_format::is_supported( info.pixel_format ) )
	//	{
	//		// media type is not supported by aaaseed, close capture
	//		CAPTURE_PRINT_STRING( "%s() format %128s is not supported", __FUNCTION__, info.str_format );
	//		goto exit_on_error;
	//	}
	//	
	//	set_flux_size_format( _cap_size_x, _cap_size_y, info.pixel_format );
	//}

//	init_with_size( _cap_size_x, _cap_size_y, get_src_bit_per_pixel()==8 ? 1 : c_pixel_format::get_channel_nb_from_force( _s_force_in_format ), __FUNCTION__ );
//	set_pixel_type_default( PIXEL_TYPE::RGBA );

	if( !set_callback( true ) )
	{
		// could not set callback, close capture
		goto exit_on_error;
	}

#if	AAA_DEBUG()
	save_graph();
#endif

	_device_index_used = _device_index_being_built;

	_b_opened = true;
	return AAA_OK;
exit_on_error:
	close_graph();
	//close();
	return ERR_ANY;
}

void	c_capture_dshow::close_specific()
{
	if( _b_opened )
	{
		stop();
		//maa	we should have an open/close generic mechanism with a b_open flag
		//			and have a close_low/open_low fns for each type
		close_graph();
		_b_opened = false;
		_b_ueye = false;
		//clear_format();
	}
	if( _hd_wind )
		_hd_wind = nullptr;
}

//todods
bool	c_capture_dshow::set_callback( bool b_set )
{
//	return ds_helper::set_callback( this, _p_video_grabber, b_set );
	return true;
}

void	print_directshow_capture_format( CHAR* str, INT32 w, INT32 h, CHAR* fourcc )
{
	if( *fourcc == 0 )
		c_capture::CAPTURE_PRINT_STRING( "DirectShow : capture format is %dx%d in %s RGB - no FOURCC", w, h, str );
	else
		c_capture::CAPTURE_PRINT_STRING( "DirectShow : capture format is %dx%d in %s, with FOURCC %s", w, h, str, fourcc );
}

AAA_ERR	c_capture_dshow::enumerate_device_mode( INT32 CONST device_index, CComPtr<IMoniker> p_video_device_moniker )
{
//	device_picked [c_capture::DEVICE_MAX_NB] [c_capture_dshow::PICK_NB];

	HRESULT					hr;
	CComPtr<IFilterGraph>	p_graph = nullptr;
	// Create a filter graph manager.
#if DS_MAA
	DS_CHECK( p_graph.CoCreateInstance( CLSID_FilterGraph ), "DirectShow could not create graph" );
#else
	DS_CHECK( CoCreateInstance( CLSID_FilterGraph, nullptr, CLSCTX_INPROC_SERVER, IID_IFilterGraph, reinterpret_cast<void**>(&p_graph) ), "DirectShow could not create graph" );
#endif
	//DS_CHECK( p_graph->QueryInterface( IID_IGraphBuilder, reinterpret_cast<void**>(&_p_graph_builder) ), "DirectShow could not query graph builder interface" );
	//p_graph = nullptr;

	CComPtr<IBaseFilter> p_filter;
	//DS_CHECK( p_graph->AddFilter( p_filter, L"Capture device for enum"), "DirectShow could not add capture filter to graph" );

	// Attempt to instantiate the capture filter.
	DS_CHECK( p_video_device_moniker->BindToObject( nullptr, nullptr, IID_IBaseFilter,  reinterpret_cast<void**>(&p_filter) ), "DirectShow could not create cap filter" );

	DS_CHECK( p_graph->AddFilter( p_filter, L"Capture device"), "DirectShow could not add capture filter to graph" );

//	clear_format();

	auto & device_mode = device_mode_arrays[device_index];
	INT32 mode_index_supported = 0;
	// locate the video capture pin and QI for stream control
	CComPtr<IAMStreamConfig>	pISC = nullptr;
	hr = ds_helper::find_pin_interface_for_stream( p_filter, PINDIR_OUTPUT, reinterpret_cast<void**>(&pISC) );
	if( SUCCEEDED(hr) )
	{
		// loop through all the capabilities (video formats)
		//   and store only the supported mode in the device_mode arrau
		INT32	count, size;
		hr = pISC->GetNumberOfCapabilities( &count, &size );
		if( SUCCEEDED(hr) && sizeof(VIDEO_STREAM_CONFIG_CAPS)==size )
		{			
			o_str & o = o_str::push_name();

				for( INT32 i = 0; i < count; ++i )
				{
					VIDEO_STREAM_CONFIG_CAPS	vscc;
					AM_MEDIA_TYPE*				pmt = nullptr;
					hr = pISC->GetStreamCaps( i, &pmt, reinterpret_cast<BYTE*>( &vscc ) );
					if( SUCCEEDED(hr) )
					{
						ds_helper::st_media_info& info = device_mode[mode_index_supported];
						if( ds_helper::get_video_info( pmt, info ) )
						{
							o.erase();
							ds_helper::add_media_info( o, &info );
							if( info.pixel_format != aaa::PIXEL_FORMAT::UNKNOWN )
							{
								CAPTURE_PRINT_STRING( "\tVideo Format %d : %s", mode_index_supported, o.get() );	
								info.o_text.erase();
								ds_helper::add_media_info_base( info.o_text, &info );
								++mode_index_supported;
							}
							else
								CAPTURE_PRINT_STRING( "\tVideo Format unimplement : %s", o.get() );	
						}
						else
						{
							ds_helper::get_media_info( pmt, info );
							DBG_PRINT_STRING( "\tIn %s() Not a Video Format : %s", __FUNCTION__, info.str_format );
						}
					}
					DeleteMediaType( pmt );
					if( mode_index_supported == c_ds_cap_ui::DEVICE_MODE_MAX_NB )
					{
						ERR_PRINT_STRING( "Direct Show Capture in AAASeed only support %d modes, skipping next modes.", c_ds_cap_ui::DEVICE_MODE_MAX_NB );
						break;
					}
				}

			o_str::pop_name();
		}
		else
			ERR_PRINT_STRING( "ERROR: Unable to retrieve video formats" );
	}

	device_mode_supported_nb[device_index] = mode_index_supported;
	for( INT32 i = mode_index_supported; i < c_ds_cap_ui::DEVICE_MODE_MAX_NB; ++i )
		device_mode[i].o_text.erase();

	CAPTURE_PRINT_STRING( "\tDevice %s AAASeed support %d Video formats.", o_device_name[device_index].get(), mode_index_supported );

	p_graph = nullptr;
	p_filter = nullptr;

	return AAA_OK;
}

bool is_obs_virtual_cam( IMoniker* moniker )
{
	CComPtr<IPropertyBag> bag;
	if( FAILED(moniker->BindToStorage( nullptr, nullptr, IID_IPropertyBag, (void**)&bag )) )
		return false;

	VARIANT v {};
	if( FAILED(bag->Read( L"FriendlyName", &v, nullptr )) )
		return false;

	bool b_obs = wcsstr(v.bstrVal, L"OBS Virtual Camera") != nullptr;
	VariantClear(&v);
	return b_obs;
}

bool is_hardware_camera(IPin* pPin)
{
    PIN_INFO pi;
    if( FAILED(pPin->QueryPinInfo(&pi)) )
		return false;

    bool b_hardware = false;
    if( pi.pFilter )
    {
        CComQIPtr<IKsPropertySet> pKs(pi.pFilter);
        b_hardware = (pKs != nullptr); // vrai si filtre expose WDM properties
        pi.pFilter->Release(); // libérer COM
    }

    return b_hardware;
}

bool device_requires_terminal_samplegrabber( IPin* pPin )
{
    // 1. On essaye de récupérer le IAMStreamConfig
    CComQIPtr<IAMStreamConfig> pConfig(pPin);
    if( !pConfig )
        return true; // pas de config => peut être virtual push

    // Vérifie si c'est un vrai périphérique
    return !is_hardware_camera(pPin);
}
bool device_upstream_is_single_buffer( IPin* pPin )
{
    if (!pPin)
		return false;

    CComPtr<IMemInputPin> pMemIn;
    if( FAILED(pPin->QueryInterface(IID_PPV_ARGS(&pMemIn))) )
        return false;

    CComPtr<IMemAllocator> pAllocator;
    if( FAILED(pMemIn->GetAllocator(&pAllocator)) )
        return false;

    ALLOCATOR_PROPERTIES props;
    if( FAILED(pAllocator->GetProperties(&props)) )
        return false;

    return props.cBuffers == 1; // OBS-style single-buffer allocator
}

AAA_ERR	c_capture_dshow::init_graph_device( REAL framerate, bool b_use_format_index, INT32 format_index )
{
	HRESULT					hr;
	CComPtr<IFilterGraph>	p_graph = nullptr;
	// Create a filter graph manager.

#if DS_MAA
	DS_CHECK( p_graph.CoCreateInstance( CLSID_FilterGraph ), "DirectShow could not create graph" );
#else
	DS_CHECK( CoCreateInstance( CLSID_FilterGraph, nullptr, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, reinterpret_cast<void**>(&p_graph) ), "DirectShow could not create graph" );
#endif
	DS_CHECK( p_graph->QueryInterface( IID_IGraphBuilder, reinterpret_cast<void**>(&_p_graph_builder) ), "DirectShow could not query graph builder interface" );
	p_graph = nullptr;

	// Create a capture filter graph builder (we're lazy).
#if DS_MAA
	DS_CHECK( _p_cap_graph_builder.CoCreateInstance( CLSID_CaptureGraphBuilder2 ), "DirectShow could not create graph builder" );
#else
	DS_CHECK( CoCreateInstance( CLSID_CaptureGraphBuilder2, nullptr, CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2, reinterpret_cast<void**>(&_p_cap_graph_builder) ), "DirectShow could not create graph builder" );
#endif

	_p_cap_graph_builder->SetFiltergraph( _p_graph_builder );

#if	AAA_DEBUG()
	ds_helper::add_to_running_object_table( _p_graph_builder, &_register_graph );
#endif

	// todo franz change the way we built the graph, device caps have to be set before being add to the graph. Ex TerraTec doest not support current implementation.

	// Attempt to instantiate the capture filter.
	DS_CHECK( _p_video_device_moniker->BindToObject( nullptr, nullptr, IID_IBaseFilter,  reinterpret_cast<void**>(&_p_cap_filter) ), "DirectShow could not create cap filter" );

	DS_CHECK( _p_graph_builder->AddFilter( _p_cap_filter, L"Capture device"), "DirectShow could not add capture filter to graph" );

	// Find the capture pin first. If we don't have one of these, we might as well give up.
	DS_CHECK( _p_cap_graph_builder->FindPin( _p_cap_filter, PINDIR_OUTPUT, &PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, TRUE, 0, &_p_real_capture_pin ), "DirectShow could not find the capture pin" );

	// NOW call device_requires_terminal_samplegrabber() with the actual pin
	//_b_device_requires_terminal_samplegrabber = device_requires_terminal_samplegrabber(_p_real_capture_pin);
	_b_device_requires_terminal_samplegrabber = device_requires_terminal_samplegrabber(_p_real_capture_pin);
	CAPTURE_PRINT_STRING( "Device requires terminal sample grabber: %s", _b_device_requires_terminal_samplegrabber ? "Yes" : "No");

	// Look for a preview pin.  It's actually likely that we won't get one if someone has a USB webcam, so we have to be prepared for it.
	hr = _p_cap_graph_builder->FindPin( _p_cap_filter, PINDIR_OUTPUT, &PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, TRUE, 0, &_p_real_preview_pin );
	_b_graph_has_preview = SUCCEEDED(hr);

	// Enumerate video formats from the capture pin.
	AM_MEDIA_TYPE*	pmt_selected = nullptr;
	if( b_use_format_index && !_b_device_requires_terminal_samplegrabber )
	{
		DBG_PRINT_STRING( "Use format index %d", format_index );

		//clear_format();

		// locate the video capture pin and QI for stream control
		CComPtr<IAMStreamConfig>	pISC = nullptr;
		hr = ds_helper::find_pin_interface_for_stream( _p_cap_filter, PINDIR_OUTPUT, reinterpret_cast<void**>(&pISC) );
		if( SUCCEEDED(hr) )
		{
			// loop through all the capabilities (video formats)
			INT32	count, size;
			hr = pISC->GetNumberOfCapabilities( &count, &size );
			if( SUCCEEDED(hr) && sizeof(VIDEO_STREAM_CONFIG_CAPS)==size )
			{
				o_str & o = o_str::push_name();
				ds_helper::st_media_info& info_wanted = device_mode_arrays[_device_index_being_built][format_index];
				ds_helper::st_media_info info;

					for( INT32 i = 0; i < count; ++i )
					{
						AM_MEDIA_TYPE*				pmt = nullptr;
						VIDEO_STREAM_CONFIG_CAPS	vscc;					
						hr = pISC->GetStreamCaps( i, &pmt, reinterpret_cast<BYTE*>( &vscc ) );
						if( SUCCEEDED(hr) )
						{
							if( ds_helper::get_video_info( pmt, info ) )
							{
								if( is_same_video_info( info, info_wanted ) )
								{
									pmt_selected = pmt;
									o.erase();
									ds_helper::add_media_info( o, &info );
									CAPTURE_PRINT_STRING( "Video Format found : %s", o.get() );	
									break;
								}
							}
						}
						DeleteMediaType( pmt );	//except the selected one that we delete later after using it
					}

				o_str::pop_name();
			}
			else
				ERR_PRINT_STRING( "ERROR: Unable to retrieve video formats" );
		}
	}
	else
	{
		CComPtr<IEnumMediaTypes>	pEnum;
		if( SUCCEEDED( _p_real_capture_pin->EnumMediaTypes( &pEnum ) ) )
		{
			AM_MEDIA_TYPE*	pMediaType;
			INT32 index = 0;
			o_str & o = o_str::push_name();

			for(;;)
			{
				HRESULT	hr2 = pEnum->Next( 1, &pMediaType, nullptr );
				if( hr2 == VFW_E_ENUM_OUT_OF_SYNC )
				{
					if( FAILED( pEnum->Reset() ) )
						break;
					//todo check format size
					//VIDEOINFOHEADER CONST * CONST pvi = (VIDEOINFOHEADER *) mediaType.pbFormat;
					//if( pvi->bmiHeader.biWidth == cap_width_ || pvi->bmiHeader.biHeight == cap_height_ )
					//	continue;

					continue;
				}
				if( hr2 != S_OK )
					break;

				// print format
				if( pMediaType->majortype == MEDIATYPE_Video && pMediaType->formattype == FORMAT_VideoInfo && pMediaType->cbFormat >= sizeof(VIDEOINFOHEADER) )
				{
					//VIDEOINFOHEADER CONST * CONST pvih = (VIDEOINFOHEADER *) pMediaType->pbFormat;
					//CAPTURE_PRINT_STRING( "%dx%d size ", pvi->bmiHeader.biWidth, pvi->bmiHeader.biHeight );
					ds_helper::st_media_info info;
					ds_helper::get_media_info( pMediaType, info );

					_cap_size_x = info.size_x;
					_cap_size_y = info.size_y;
//					_b_src_y_inverted	= c_pixel_format::is_y_inverted( info.pixel_format );
					//_b_luma_flip		= c_pixel_format::is_luma_inverted( info.pixel_format );
					//_b_src_grey		= c_pixel_format::is_grey( info.pixel_format );
					// we don't check if the format is supported here ? maa guess it is done before

					o.erase();
					ds_helper::add_media_info( o, &info );
					CAPTURE_PRINT_STRING( "\tVideo Format %d : %s", index, o.get() );
					++index;
				}

				//if (pMediaType->majortype == MEDIATYPE_Video && pMediaType->formattype == FORMAT_VideoInfo
				//	&& pMediaType->cbFormat >= sizeof(VIDEOINFOHEADER))
				//	{
				//	}

				sysutils::safe_release( pMediaType->pUnk );
				if( pMediaType->pbFormat )
					CoTaskMemFree( pMediaType->pbFormat );
				CoTaskMemFree( pMediaType );
			}

			o_str::pop_name();
		}
	}

	if( b_use_format_index && pmt_selected )
	{
		DBG_PRINT_STRING( "Pass 2 : Use format index %d", format_index );
		hr = ds_helper::find_pin_interface_for_stream( _p_cap_filter, PINDIR_OUTPUT, reinterpret_cast<void**>(&_p_video_config_cap ) );
	
		if( SUCCEEDED(hr) )
		{
			DBG_PRINT_STRING( "Pass 3 : Use format index %d", format_index );
			ds_helper::print_guid( pmt_selected->subtype );
			hr = _p_video_config_cap->SetFormat(pmt_selected);
			if( FAILED(hr) )
				ERR_PRINT_STRING( "failed to impose format from index %d", format_index );
			else
				GOOD_PRINT_STRING( "format just set from index %d", format_index );
		}
		DeleteMediaType( pmt_selected );
	}
	else
	{
		hr = _p_cap_graph_builder->FindInterface( &PIN_CATEGORY_CAPTURE, &MEDIATYPE_Interleaved, _p_cap_filter, IID_IAMStreamConfig, reinterpret_cast<void**>(&_p_video_config_cap) );
		if( FAILED( hr ) )
			DS_CHECK( _p_cap_graph_builder->FindInterface( &PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, _p_cap_filter, IID_IAMStreamConfig, reinterpret_cast<void**>(&_p_video_config_cap) ), "DirectShow could not create find video capture interface" );
	}

	// Get video format configurator
	if( _b_capture_force_default && !_b_device_requires_terminal_samplegrabber )
	{
		DBG_PRINT_STRING( "start capture force default" );
		AM_MEDIA_TYPE	media_type{};	// inited so even padding is initilaised
		MEMCLEAR( &media_type, sizeof( AM_MEDIA_TYPE ) );
		media_type.majortype			= MEDIATYPE_Video;
		media_type.bFixedSizeSamples	= true;
		media_type.formattype			= FORMAT_VideoInfo;
		media_type.cbFormat				= sizeof( VIDEOINFOHEADER );

		VIDEOINFOHEADER * CONST pvih = (VIDEOINFOHEADER *)CoTaskMemAlloc( sizeof( VIDEOINFOHEADER ) );	// media_type.pbFormat;
		MEMCLEAR( pvih, sizeof( pvih ) );
		media_type.pbFormat			= (BYTE*)pvih;
		pvih->bmiHeader.biSize		= sizeof( BITMAPINFOHEADER );
		pvih->bmiHeader.biHeight	= _capture_size_y;
		pvih->bmiHeader.biWidth		= _capture_size_x;
		pvih->rcSource.bottom		= 0;
		pvih->rcSource.left			= 0;
		pvih->rcSource.top			= 0;
		pvih->rcSource.right		= 0;
		pvih->rcTarget.bottom		= 0;
		pvih->rcTarget.left			= 0;
		pvih->rcTarget.right		= 0;
		pvih->rcTarget.top			= 0;
		pvih->AvgTimePerFrame		= REFERENCE_TIME( 10000000. / framerate );

		switch( _s_force_src_pixel_format )
		{
		case aaa::PIXEL_FORMAT_SRC_FORCE::RGB:
			media_type.subtype			= MEDIASUBTYPE_RGB24;
			pvih->bmiHeader.biBitCount	= 24;
			pvih->bmiHeader.biSizeImage	= _capture_size_x * _capture_size_y * (24/8);
			break;
		case aaa::PIXEL_FORMAT_SRC_FORCE::DEFAULT:
		case aaa::PIXEL_FORMAT_SRC_FORCE::RGBA:
			media_type.subtype			= MEDIASUBTYPE_RGB32;
			pvih->bmiHeader.biBitCount	= 32;
			pvih->bmiHeader.biSizeImage	= _capture_size_x * _capture_size_y * (32/8);
			break;
		case aaa::PIXEL_FORMAT_SRC_FORCE::YUY2:
			media_type.subtype				= MEDIASUBTYPE_YUY2;
			pvih->bmiHeader.biBitCount		= 16;
			pvih->bmiHeader.biPlanes		= 1;
			pvih->bmiHeader.biCompression	= 0x32595559;
			pvih->bmiHeader.biSizeImage		= _capture_size_x * _capture_size_y * (16/8);
			pvih->bmiHeader.biClrImportant	= 0;
			pvih->bmiHeader.biClrUsed		= 0;
			pvih->bmiHeader.biXPelsPerMeter	= 0;
			pvih->bmiHeader.biYPelsPerMeter	= 0;
			media_type.lSampleSize			= pvih->bmiHeader.biSizeImage;
			break;
		case aaa::PIXEL_FORMAT_SRC_FORCE::UYVY:
			media_type.subtype				= MEDIASUBTYPE_UYVY;
			pvih->bmiHeader.biBitCount		= 16;
			pvih->bmiHeader.biPlanes			= 1;
//			pvih->bmiHeader.biCompression	= 'YVYU';	// was this
			pvih->bmiHeader.biCompression	= 'UYVY';	// should be this
			pvih->bmiHeader.biSizeImage		= _capture_size_x * _capture_size_y * (16/8) ;

		//pvi->bmiHeader.biClrImportant = 0;
			//pvi->bmiHeader.biClrUsed = 0;
			//pvi->bmiHeader.biXPelsPerMeter = 0;
			//pvi->bmiHeader.biYPelsPerMeter = 0;
			//media_type.lSampleSize = pvi->bmiHeader.biSizeImage;
			break;
		case aaa::PIXEL_FORMAT_SRC_FORCE::I420:
			media_type.subtype			= MEDIASUBTYPE_I420;
			pvih->bmiHeader.biBitCount	= 12;
			pvih->bmiHeader.biSizeImage	= ( _capture_size_x * _capture_size_y * 12 ) / 8;
			break;
		case aaa::PIXEL_FORMAT_SRC_FORCE::R_8: //SC08 +case tester avec camera B&W
			media_type.subtype			= MEDIASUBTYPE_RGB8;
			pvih->bmiHeader.biBitCount	= 8;
			pvih->bmiHeader.biSizeImage	= _capture_size_x * _capture_size_y;
			break;
		default:
			ERR_PRINT_STRING( "Unknown capture format" );
			break;
		}

		hr = _p_video_config_cap->SetFormat( &media_type );
		if( FAILED( hr ) )
			CAPTURE_PRINT_STRING( "DirectShow failed to set capture to desired format" );
		else
			CAPTURE_PRINT_STRING( "DirectShow capture set to desired format" );
		// free media type
		if( IS_NOT_NULL( media_type.pbFormat ) && media_type.cbFormat > 0 )
		{
			CoTaskMemFree( media_type.pbFormat );
			media_type.pbFormat = nullptr;
			media_type.cbFormat = 0;
		}
	}

	if( _b_capture_force_fps && !_b_device_requires_terminal_samplegrabber )
	{
		DBG_PRINT_STRING( "start capture force fps" );
		AM_MEDIA_TYPE*	media_type;

		hr = _p_video_config_cap->GetFormat( &media_type );

	///	MEMCLEAR( &media_type, sizeof( AM_MEDIA_TYPE ) );
	//	media_type.majortype = MEDIATYPE_Video;
	//	media_type.bFixedSizeSamples = TRUE;
		if( media_type->formattype == FORMAT_VideoInfo )
		{
			//media_type.formattype = FORMAT_VideoInfo;
			//media_type.cbFormat = sizeof( VIDEOINFOHEADER );
			VIDEOINFOHEADER * CONST pvih = (VIDEOINFOHEADER *) media_type->pbFormat;
		//	VIDEOINFOHEADER * CONST pvih;	// = (VIDEOINFOHEADER *)CoTaskMemAlloc( sizeof( VIDEOINFOHEADER ) );	// media_type.pbFormat;
		//	MEMCLEAR( pvih, sizeof( pvih ) );
		//	pvih = (VIDEOINFOHEADER*)media_type->pbFormat;
			//pvih->bmiHeader.biSize = sizeof( BITMAPINFOHEADER );
			//pvih->bmiHeader.biHeight = _capture_size_y;
			//pvih->bmiHeader.biWidth = _capture_size_x;
			//pvih->rcSource.bottom = 0;
			//pvih->rcSource.left = 0;
			//pvih->rcSource.top = 0;
			//pvih->rcSource.right = 0;
			//pvih->rcTarget.bottom = 0;
			//pvih->rcTarget.left = 0;
			//pvih->rcTarget.right = 0;
			//pvih->rcTarget.top = 0;
			pvih->AvgTimePerFrame = REFERENCE_TIME( 10000000. / framerate );
			//media_type.subtype = MEDIASUBTYPE_RGB24;
			//pvih->bmiHeader.biBitCount = 24;
			//pvih->bmiHeader.biSizeImage = _capture_size_x * _capture_size_y * (24/8);

			hr = _p_video_config_cap->SetFormat( media_type );
			if( FAILED( hr ) )
				CAPTURE_PRINT_STRING( "DirectShow failed to set capture fps" );
			else
				CAPTURE_PRINT_STRING( "DirectShow capture set to desired fps" );
		}
		// free media type
		sysutils::safe_release( media_type->pUnk );
		if( IS_NOT_NULL( media_type->pbFormat ) )
			CoTaskMemFree( media_type->pbFormat );
		CoTaskMemFree( media_type );
	}

	hr = _p_cap_graph_builder->FindInterface( &PIN_CATEGORY_PREVIEW, &MEDIATYPE_Interleaved, _p_cap_filter, IID_IAMStreamConfig, reinterpret_cast<void**>(&_p_video_config_prv) );
	if( FAILED(hr) )
		hr = _p_cap_graph_builder->FindInterface( &PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, _p_cap_filter, IID_IAMStreamConfig, reinterpret_cast<void**>(&_p_video_config_prv) );

	// Look for a video port pin. We _HAVE_ to render this if it exists; otherwise, the ATI All-in-Wonder driver can lock on a wait in kernel mode and zombie
	// our process. And no, a Null Renderer doesn't work. It's OK for this to fail.
	hr = _p_cap_graph_builder->FindPin( _p_cap_filter, PINDIR_OUTPUT, &PIN_CATEGORY_VIDEOPORT, nullptr, FALSE, 0, &_p_cap_filter_video_port_pin ); 

	// Check for VFW capture dialogs, TV tuner, and crossbar
	_p_cap_graph_builder->FindInterface( nullptr, nullptr, _p_cap_filter, IID_IAMVfwCaptureDialogs,	reinterpret_cast<void**>(&_p_vfw_dialogs) );
	_p_cap_graph_builder->FindInterface( nullptr, nullptr, _p_cap_filter, IID_IAMCrossbar,			reinterpret_cast<void**>(&_p_crossbar) );
	if( _p_crossbar )
	{
		CComPtr<IBaseFilter>	pXFilt;
		if( SUCCEEDED( _p_crossbar->QueryInterface(IID_IBaseFilter, reinterpret_cast<void**>(&pXFilt) ) ) )
			_p_cap_graph_builder->FindInterface( &LOOK_UPSTREAM_ONLY, nullptr, pXFilt, IID_IAMCrossbar, reinterpret_cast<void**>(&_p_crossbar2) );
	}

	// Search for tuner interfaces. Note that IAMTVTuner inherits from IAMTuner.
	if( SUCCEEDED( _p_cap_graph_builder->FindInterface( nullptr, nullptr, _p_cap_filter, IID_IAMTVTuner, reinterpret_cast<void**>(&_p_tv_tuner) ) ) )
		_p_tuner = _p_tv_tuner;
	else
		_p_cap_graph_builder->FindInterface( nullptr, nullptr, _p_cap_filter, IID_IAMTuner, reinterpret_cast<void**>(&_p_tuner) );

	// If there is at least one crossbar, examine the crossbars to see if we can spot an audio input switch.
	_p_video_crossbar = nullptr;
	for( size_t i = 0; i < 2; ++i )
	{
		CComPtr<IAMCrossbar>	pCrossbar;
		if( i == 0 )
			pCrossbar = _p_crossbar;
		else
			pCrossbar = _p_crossbar2;

		if (!pCrossbar)
			continue;

		LONG	nb_outputs, nb_inputs;
		if( FAILED( pCrossbar->get_PinCounts( &nb_outputs, &nb_inputs ) ) )
			continue;

		display_crossbar_info( pCrossbar );

		//for (i = 0; i < nb_inputs; ++i )
		//	{
		//	LONG lRelated = -1, lType = -1;
		//	hr = pCrossbar->get_CrossbarPinInfo( TRUE, i, &lRelated, &lType );
		//	CAPTURE_PRINT_STRING( "Input pin %d - %s\n\tRelated in: %d\n", i, GetPhysicalPinName(lType), lRelated );
		//	}

		for( long pin = 0; pin < nb_outputs; ++pin )
		{
			long	related;
			long	phystype;

			if( FAILED( pCrossbar->get_CrossbarPinInfo( FALSE, pin, &related, &phystype ) ) )
				continue;
			if( !_p_video_crossbar )
			{
				if( phystype == PhysConn_Video_VideoDecoder )
				{
					_p_video_crossbar = pCrossbar;
					_video_crossbar_output = pin;
					// todoqq, not necessary anymore, crossbar index is not in forced
					// Maa 2021 July deal again with _crossbar_index
					//if( _b_capture_force_default )
					//	_p_video_crossbar->Route( pin, _crossbar_index );
				}
			}
		}
	}

	hr = _p_cap_filter->QueryInterface( IID_IuEyeCapturePin, reinterpret_cast<void**>(&_p_cap_ueye_pin) );
	if( SUCCEEDED(hr) )
	{
		_b_ueye = true;
		get_ueye_settings();
	}

	DS_CHECK( _p_graph_builder->QueryInterface( IID_IMediaControl, reinterpret_cast<void**>(&_p_graph_control) ), "DirectShow could not find graph control interface" );

	// get video proc amp
	if( FAILED( _p_cap_filter->QueryInterface( IID_IAMVideoProcAmp, reinterpret_cast<void**>(&_p_video_proc_amp) ) ) )
		_p_video_proc_amp = nullptr; // will be nullptr anyway (replace with something intelligent)

	CAPTURE_PRINT_STRING( "DirectShow capture graph initialized" );

	return AAA_OK;
}

void c_capture_dshow::close_graph_device()
{
	set_callback( false );
	
	//maa	perhaps the order have an importance
	//maa	added the next two
	_p_video_device_moniker = nullptr;
	_p_eye_color_temp		= nullptr;
	//_p_eye_saturation		= nullptr;
	_p_eye_capture_ex		= nullptr;

	_p_cap_ueye_pin			= nullptr;
	_p_cap_ueye				= nullptr;
	_p_capture_pin			= nullptr;

	_p_cap_filter			= nullptr;
	_p_real_capture_pin		= nullptr;
	_p_real_preview_pin		= nullptr;
	_p_cap_filter_video_port_pin = nullptr;

	_p_crossbar				= nullptr;
	_p_crossbar2			= nullptr;
	_p_tuner				= nullptr;
	_p_tv_tuner				= nullptr;
	_p_vfw_dialogs			= nullptr;
	_p_video_config_cap		= nullptr;
	_p_video_config_prv		= nullptr;
//	_p_media_event_ex		= nullptr;
	_p_graph_control		= nullptr;
	_p_video_crossbar		= nullptr;
	_p_video_proc_amp		= nullptr;
	_p_cap_graph_builder	= nullptr;
	destroy_graph();
	_p_graph_builder		= nullptr;

#if	AAA_DEBUG()
	ds_helper::remove_from_object_table( &_register_graph );
#endif

	CAPTURE_PRINT_STRING( "DirectShow capture shutdown" );
}

void	c_capture_dshow::close_graph()
{
	close_graph_device();
}

void c_capture_dshow::destroy_graph()
{
	stop_graph();

	// drop pointers to graph components
	_p_dropped_frames	= nullptr;

	if( _p_graph_builder )
	{
		// reset capture clock
		CComPtr<IMediaFilter>	pGraphMF;
		if( SUCCEEDED( _p_graph_builder->QueryInterface( IID_IMediaFilter, reinterpret_cast<void**>(&pGraphMF) ) ) )
		{
			pGraphMF->SetSyncSource(nullptr);
		}
		pGraphMF = nullptr;
		// destroy downstreams
		ds_helper::destroy_graph( *_p_graph_builder );
		_p_graph_builder = nullptr;
	}
	_b_graph_built = false;
}

bool	c_capture_dshow::stop_graph()
{
	if( !_b_graph_running )
		return true;

	_b_graph_running = false;

	if( _p_graph_control )
	{
		HRESULT	hr = _p_graph_control->Stop();
		CAPTURE_PRINT_STRING( "DirectShow graph stopped" );
		return SUCCEEDED( hr );
	}

	CAPTURE_PRINT_STRING( "DirectShow can't stop graph" );
	return false;
}

bool	c_capture_dshow::start_graph()
{
	if( _b_graph_running )
		return true;

	if( _p_graph_control )
	{
		HRESULT	hr = _p_graph_control->Run();
		if( SUCCEEDED( hr ) )
		{
			CAPTURE_PRINT_STRING( "DirectShow graph started" );
			_b_graph_running = true;
			return true;
		}
		else
			CAPTURE_PRINT_STRING( " DShow : run Graph FAILED : %s", ds_helper::get_error_string( hr ) );
	}
	return false;
}

AAA_ERR c_capture_dshow::init_graph_capture()
{
    HRESULT hr;

    CComPtr<IPin> p_capture_pin = _p_real_capture_pin;
    if( !p_capture_pin )
        return ERR_ANY;

    bool b_mjpg = ds_helper::pin_can_deliver_mjpg( p_capture_pin );
    bool b_dv   = ds_helper::pin_can_deliver_dv  ( p_capture_pin );

    CComPtr<IBaseFilter> p_decoder;
    if( b_dv )
    {
        DS_CHECK( CoCreateInstance( CLSID_DVVideoCodec, nullptr, CLSCTX_INPROC_SERVER, IID_IBaseFilter, reinterpret_cast<void**>( &p_decoder ) ), "Create DV decoder failed" );
        DS_CHECK( _p_graph_builder->AddFilter( p_decoder, L"DV Decoder" ), "Add DV decoder failed" );
    }
    else if( b_mjpg )
    {
        DS_CHECK( CoCreateInstance( CLSID_MjpegDec, nullptr, CLSCTX_INPROC_SERVER, IID_IBaseFilter, reinterpret_cast<void**>( &p_decoder ) ), "Create MJPEG decoder failed" );
        DS_CHECK( _p_graph_builder->AddFilter( p_decoder, L"MJPEG Decoder" ), "Add MJPEG decoder failed" );
    }

    if (!_p_aaa_filter)
    {
#if AAA_USE_DS_TRANSFORM_FILTER()
		_p_aaa_filter = new c_aaa_ds_transform_filter(nullptr, &hr, this);
#else
		_p_aaa_filter = new c_aaa_ds_renderer_filter(nullptr, &hr, this);	
#endif
        if (!_p_aaa_filter || FAILED(hr))
        {
            ERR_PRINT_STRING("Create AAASeed filter failed");
            delete _p_aaa_filter;
            return ERR_ANY;
        }
        _p_aaa_filter->AddRef();
    }

	// Add to graph
    IBaseFilter* p_aaa_filter = static_cast<IBaseFilter*>(_p_aaa_filter);
    DS_CHECK( _p_graph_builder->AddFilter(p_aaa_filter, TEXT("AAA Filter")), "Add AAASeed filter failed" );

#if AAA_DEBUG()
    {
        FILTER_INFO info;
        if (SUCCEEDED(p_aaa_filter->QueryFilterInfo(&info)))
        {
            GOOD_PRINT_STRING("Added Filter: %ls", info.achName);
        }
    }
#endif


    CComPtr<IPin> p_tf_in;
    DS_CHECK(_p_cap_graph_builder->FindPin(p_aaa_filter, PINDIR_INPUT, nullptr, nullptr, TRUE, 0, &p_tf_in), "Find AAA filter input pin failed");

#if AAA_USE_DS_TRANSFORM_FILTER()
	CComPtr<IPin> p_tf_out;
    DS_CHECK(_p_cap_graph_builder->FindPin(p_aaa_filter, PINDIR_OUTPUT, nullptr, nullptr, TRUE, 0, &p_tf_out), "Find AAA filter output pin failed");
#endif

    if( p_decoder )
    {
        CComPtr<IPin> p_dec_in;
        CComPtr<IPin> p_dec_out;

        DS_CHECK( _p_cap_graph_builder->FindPin( p_decoder, PINDIR_INPUT, nullptr, nullptr, TRUE, 0, &p_dec_in ), "Find decoder input pin failed" );
        DS_CHECK( _p_cap_graph_builder->FindPin( p_decoder, PINDIR_OUTPUT, nullptr, nullptr, TRUE, 0, &p_dec_out ), "Find decoder output pin failed" );

        DS_CHECK( _p_graph_builder->Connect( p_capture_pin, p_dec_in ), "Connect capture -> decoder failed" );
        DS_CHECK( _p_graph_builder->Connect( p_dec_out, p_tf_in ), "Connect decoder -> AAA filter failed" );
    }
    else
    {
        DS_CHECK( _p_graph_builder->Connect( p_capture_pin, p_tf_in ), "Connect capture -> AAA filter failed" );
    }

#if AAA_USE_DS_TRANSFORM_FILTER()
    CComPtr<IBaseFilter> p_null;
    CComPtr<IPin> p_null_in;

    DS_CHECK( CoCreateInstance( CLSID_NullRenderer, nullptr, CLSCTX_INPROC_SERVER, IID_IBaseFilter, reinterpret_cast<void**>( &p_null ) ), "Create null renderer failed" );
    DS_CHECK( _p_graph_builder->AddFilter( p_null, L"Null Renderer" ), "Add null renderer failed" );
    DS_CHECK( _p_cap_graph_builder->FindPin( p_null, PINDIR_INPUT, nullptr, nullptr, TRUE, 0, &p_null_in ), "Find null renderer input failed" );
    DS_CHECK( _p_graph_builder->Connect( p_tf_out, p_null_in ), "Connect transform -> null renderer failed" );
#endif
	// Get the negotiated media type
    AM_MEDIA_TYPE connected_mt{};
    if( SUCCEEDED( p_tf_in->ConnectionMediaType( &connected_mt ) ) )
    {
        ds_helper::st_media_info info;
        ds_helper::get_media_info( &connected_mt, info );

        if( info.size_x > 0 && info.size_y > 0 )
        {
            _cap_size_x = info.size_x;
            _cap_size_y = info.size_y;
            set_flux_size_format( _cap_size_x, _cap_size_y, info.pixel_format );
            CAPTURE_PRINT_STRING( "Negotiated format: %s", info.str_format );
        }

        FreeMediaType( connected_mt );
    }

	_p_dropped_frames = nullptr;
	_p_cap_graph_builder->FindInterface( nullptr, nullptr, _p_cap_filter, IID_IAMDroppedFrames, reinterpret_cast<void**>( &_p_dropped_frames ) );

	CComPtr<IMediaFilter> p_graph_mf;
	if( SUCCEEDED( _p_graph_builder->QueryInterface( IID_IMediaFilter, reinterpret_cast<void**>( &p_graph_mf ) ) ) )
	{
		CComPtr<IReferenceClock> p_graph_clock;
		if( SUCCEEDED( CoCreateInstance( CLSID_SystemClock, nullptr, CLSCTX_INPROC_SERVER, IID_IReferenceClock, reinterpret_cast<void**>( &p_graph_clock ) ) ) )
			p_graph_mf->SetSyncSource( p_graph_clock );
	}

	_b_graph_built = true;
	CAPTURE_PRINT_STRING( "DirectShow capture graph built" );

	return AAA_OK;
}



#if 0 // not a ISampleGrabberCB anymore
HRESULT WINAPI	c_capture_dshow::SampleCB( double sample_time, IMediaSample *p_sample )
{
	//DBG_PRINT_STRING("c_capture_dshow::SampleCB");
	if( p_sample )
	{
//#if	AAA_DEBUG()
//todolv
//		if( _b_verbose )
//			VERBOSE_PRINT_STRING( "Sample received Length=%ld\r", p_sample->GetActualDataLength() );
//#endif
		//todo detect change in media type
		//hr = pSample->GetMediaType( &p_media_type );
		//if( p_media_type )
		//	{
		//	if( p_media_type->formattype == FORMAT_DvInfo )
		//			DBG_PRINT_STRING("Format DvInfo");
		//	if( p_media_type->formattype == FORMAT_MPEGVideo )
		//			DBG_PRINT_STRING("Format MPEG1VIDEOINFO");
		//	if( p_media_type->formattype == FORMAT_MPEG2Video )
		//			DBG_PRINT_STRING("Format MPEG2VIDEOINFO");
		//	if( p_media_type->formattype == FORMAT_VideoInfo )
		//			DBG_PRINT_STRING("Format VIDEOINFOHEADER");
		//	if( p_media_type->formattype == FORMAT_VideoInfo2 )
		//			DBG_PRINT_STRING("Format VIDEOINFOHEADER2");
		//	if( p_media_type->formattype == FORMAT_WaveFormatEx )
		//			DBG_PRINT_STRING("Format WAVEFORMATEX");
		//	if( p_media_type->formattype == FORMAT_None )
		//			DBG_PRINT_STRING("Format None");
		//	if( p_media_type->formattype == GUID_NULL )
		//			DBG_PRINT_STRING("Format None");
		//	}
		BYTE*	p_data;
		HRESULT	hr = p_sample->GetPointer( &p_data );
		if( FAILED( hr ) )
			return hr;
		
		//AM_MEDIA_TYPE*	p_media_type = nullptr;
		//hr = p_sample->GetMediaType( &p_media_type );
		//ds_helper::st_frame_info info;
		//ds_helper::get_media_info( p_media_type, info );
		////init_with_size( info.size_x, info.size_y,  );
		//set_src_pitch( info.pitch );
		got_frame( (UINT8*)p_data, "Capture Directshow", 0, true );
		// get stats for dropped frames
	}
	else
		ERR_PRINT_STRING( "Invalid IMediaSample passed to SampleCB!" );

	return( S_OK );
}

HRESULT	WINAPI	c_capture_dshow::BufferCB( double sampleTimeSec, BYTE* bufferPtr, long bufferLength )
{
	return E_NOTIMPL;
	//got_frame( (UINT8*)bufferPtr, "Capture Directshow", 0, true );
	//return S_OK;
}
#endif

//	return the index if it find the device
//		otherwise return -1;
INT32	c_capture_dshow::find_device( INT32 CONST index, o_str CONST * CONST device_id )
{
	INT32	s_found = -1;

	CComPtr<IEnumMoniker>	pt_enum_moniker = create_enum_moniker();
	if( pt_enum_moniker )
	{
		CComPtr<IMoniker>	p_moniker;
		ULONG				c_fetched;
		INT32				index_enum = 0;
		CHAR				buf_devicepath[512];
		CHAR				buf_name[512];
		while( SUCCEEDED( pt_enum_moniker->Next( 1, &p_moniker, &c_fetched ) ) && ( c_fetched == 1 ) )
		{
			CComPtr<IPropertyBag>	p_property_bag;
			if( SUCCEEDED( p_moniker->BindToStorage( 0, 0, IID_IPropertyBag, reinterpret_cast<void**>(&p_property_bag) ) ) )
			{
				if( find_name_and_path( p_property_bag, buf_name, buf_devicepath ) )
				{
					if( device_id || index_enum == index )
					{
						if( device_id && *buf_devicepath )
						{	// use device clsid not index to open capture
							if( strcmp( buf_devicepath, device_id->get() ) == 0 )
								s_found = index_enum;
						}
						else
							s_found = index_enum;

						if( s_found >= 0 )
						{
							// device found
							set_flux_name( buf_name );
							_p_video_device_moniker = p_moniker;
							_o_device_id.set( buf_devicepath );
							CAPTURE_PRINT_STRING( "DirectShow device driver %d found : %s (%s)", s_found, buf_name, buf_devicepath );
						}
					}
					++index_enum;
				}
				p_property_bag = nullptr;
			}
			p_moniker = nullptr;
			if( s_found >= 0 )
				break;
		}
		pt_enum_moniker = nullptr;
	}
	return s_found;
}

bool	c_capture_dshow::display_property_pages( IUnknown *ptr, HWND hwndParent )
{
	if( !ptr )
		return false;

	ISpecifyPropertyPages*	pPages;

	if( FAILED( ptr->QueryInterface( IID_ISpecifyPropertyPages, reinterpret_cast<void**>(&pPages) ) ) )
		return false;

	CAUUID	cauuid;
	bool	b_success = false;

	if( SUCCEEDED( pPages->GetPages( &cauuid ) ) )
	{
		if( cauuid.cElems )
		{
			if( hwndParent )
			{
	//			HRESULT hr = OleCreatePropertyFrame( hwndParent, 0, 0, nullptr, 1, (IUnknown **)&pPages, cauuid.cElems, (GUID *)cauuid.pElems, 0, 0, nullptr );
				HRESULT hr = OleCreatePropertyFrame( hwndParent, 0, 0, nullptr, 1, reinterpret_cast<IUnknown**>(&pPages), cauuid.cElems, (GUID *)cauuid.pElems, 0, 0, nullptr );
				b_success = SUCCEEDED( hr );
			}
			else
				b_success = false;
		}
		CoTaskMemFree( cauuid.pElems );
	}
	sysutils::safe_release( pPages );
	return b_success;
}

HRESULT	c_capture_dshow::save_graph( CComPtr<IGraphBuilder> pGraph, CONST WCHAR * CONST wszPath ) 
{
	CONST	WCHAR		wszStreamName[] = L"AAASeedMovieGraph"; 
	HRESULT				hr;
	CComPtr<IStorage>	pStorage = nullptr;

	hr = StgCreateDocfile( wszPath, STGM_CREATE | STGM_TRANSACTED | STGM_READWRITE | STGM_SHARE_EXCLUSIVE, 0, &pStorage );
	if( FAILED(hr) ) 
		return hr;

	CComPtr<IStream>	pStream;
	hr = pStorage->CreateStream( wszStreamName, STGM_WRITE | STGM_CREATE | STGM_SHARE_EXCLUSIVE, 0, 0, &pStream );
	if( FAILED(hr) ) 
	{
		pStorage = nullptr;
		return hr;
	}

	CComPtr<IPersistStream>	pPersist = nullptr;
	pGraph->QueryInterface( IID_IPersistStream,reinterpret_cast<void**>(&pPersist) );
	hr = pPersist->Save( pStream, TRUE );
	pStream = nullptr;
	pPersist = nullptr;

	if( SUCCEEDED(hr) ) 
	{
		hr = pStorage->Commit( STGC_DEFAULT );
	}
	pStorage = nullptr;
	return hr;
}

// Helper function to associate a name with the type.
C_PCHAR	c_capture_dshow::get_physical_pin_name( LONG type )
{
	switch( type ) 
	{
	case PhysConn_Video_Tuner:				return "Video Tuner";
	case PhysConn_Video_Composite:			return "Video Composite";
	case PhysConn_Video_SVideo:				return "S-Video";
	case PhysConn_Video_RGB:				return "Video RGB";
	case PhysConn_Video_YRYBY:				return "Video YRYBY";
	case PhysConn_Video_SerialDigital:		return "Video Serial Digital";
	case PhysConn_Video_ParallelDigital:	return "Video Parallel Digital"; 
	case PhysConn_Video_SCSI:				return "Video SCSI";
	case PhysConn_Video_AUX:				return "Video AUX";
	case PhysConn_Video_1394:				return "Video 1394";
	case PhysConn_Video_USB:				return "Video USB";
	case PhysConn_Video_VideoDecoder:		return "Video Decoder";
	case PhysConn_Video_VideoEncoder:		return "Video Encoder";
	case PhysConn_Video_SCART:				return "Video SCART";

	case PhysConn_Audio_Tuner:				return "Audio Tuner";
	case PhysConn_Audio_Line:				return "Audio Line";
	case PhysConn_Audio_Mic:				return "Audio Microphone";
	case PhysConn_Audio_AESDigital:			return "Audio AES/EBU Digital";
	case PhysConn_Audio_SPDIFDigital:		return "Audio S/PDIF";
	case PhysConn_Audio_SCSI:				return "Audio SCSI";
	case PhysConn_Audio_AUX:				return "Audio AUX";
	case PhysConn_Audio_1394:				return "Audio 1394";
	case PhysConn_Audio_USB:				return "Audio USB";
	case PhysConn_Audio_AudioDecoder:		return "Audio Decoder";

	default:								return "Unknown Type";
	}
}

void	c_capture_dshow::display_crossbar_info( CComPtr<IAMCrossbar> p_x_bar )
{
	HRESULT	hr;
	LONG	cOutput = -1;
	LONG	cInput = -1;

	if ( p_x_bar == nullptr )
		return;

	if( FAILED( p_x_bar->get_PinCounts( &cOutput, &cInput ) ) )
	{
		CAPTURE_PRINT_STRING( "Couldn't get XBar pin count" );
		return;
	}
	for( LONG i = 0; i < cOutput; ++i )
	{
		LONG	lRelated = -1, lType = -1, lRouted = -1;
		hr = p_x_bar->get_CrossbarPinInfo( FALSE, i, &lRelated, &lType );
		hr = p_x_bar->get_IsRoutedTo( i, &lRouted );
		CAPTURE_PRINT_STRING( "Crossbar Output %d : %s", i, get_physical_pin_name( lType ) );
		//printf("Output pin %d: %s\n", i, GetPhysicalPinName(lType));
		//printf("\tRelated out: %d, Routed in: %d\n", lRelated, lRouted );
		//printf("\tSwitching Matrix: ");

		//for( long j = 0; j < cInput; ++j )
		//{
		//    hr = pXBar->CanRoute( i, j );
		//    printf("%d-%s", j, (S_OK == hr ? "Yes" : "No"));
		//}
		//printf("\n\n");
	}

	for( LONG i = 0; i < cInput; ++i )
	{
		LONG	lRelated = -1, lType = -1;
		hr = p_x_bar->get_CrossbarPinInfo( TRUE, i, &lRelated, &lType );
		CAPTURE_PRINT_STRING( "Crossbar Input %d : %s", i, get_physical_pin_name( lType ) );
	}
}

bool	c_capture_dshow::set_crossbar( INT32 index )
{
	// check video crossbar first
	if( _p_video_crossbar )
	{
		if( _crossbar_index != index )
		{
			HRESULT	hr;
			hr = _p_video_crossbar->Route( _video_crossbar_output, index );
			if( FAILED( hr ) )
			{
				CAPTURE_PRINT_STRING( "Could not route the two pins" );
				return false;
			}
			LONG	lRelated = -1, lType = -1;
			hr = _p_video_crossbar->get_CrossbarPinInfo( TRUE, index, &lRelated, &lType );
			_o_crossbar_name.set( get_physical_pin_name( lType ) );
			_crossbar_index = index;
		}
	}
	else
		_crossbar_index = -1;
	return true;
}

//------------------------------------------------------------------------------------
//
//	VPA
//
//todo we should test which paramater can do auto and act accordingly and set the ui in regard
namespace {
	FINLINE	LONG	vap_property_from_bool( bool b_auto )
	{
		return b_auto ? VideoProcAmp_Flags_Auto : VideoProcAmp_Flags_Manual ;
	}
	static	FINLINE	bool	vap_property_to_bool( LONG flag )
	{
		return flag==VideoProcAmp_Flags_Auto;
	}
}

HRESULT	c_capture_dshow::vap_get_parameter_range( LONG property, LONG* pMin, LONG* pMax, LONG* pSteppingDelta, LONG* pDefault, LONG* pCapsFlags )
{	
	if( !_p_video_proc_amp )
		return E_POINTER;
	return _p_video_proc_amp->GetRange( property, pMin, pMax, pSteppingDelta, pDefault, pCapsFlags );
}
//	we using bool for auto so we need these 2 wrapper functions
HRESULT	c_capture_dshow::vap_get_parameter( LONG property, LONG* value, bool* b_auto )
{
	if( !_p_video_proc_amp )
		return E_POINTER;
	LONG	flags = 0;
	HRESULT	hr = _p_video_proc_amp->Get( property, value, &flags );
	*b_auto = vap_property_to_bool( flags );
	return hr;
}

HRESULT	c_capture_dshow::vap_set_parameter( LONG property, LONG value, bool b_auto )
{
	if( !_p_video_proc_amp )
		return E_POINTER;
	return _p_video_proc_amp->Set( property, value, vap_property_from_bool( b_auto )  );
}
//	we also use normalized value from 0. to 1.
HRESULT	c_capture_dshow::vap_set_parameters_norm( LONG property, DOUBLE value, bool b_auto )
{
	LONG	value_cur;
	bool	b_auto_cur;
	HRESULT	hr;
	if( FAILED( hr = vap_get_parameter( property, &value_cur, &b_auto_cur ) ) )
		return( hr );

	LONG	min;
	LONG	max;
	LONG	stepping_delta;
	LONG	default_value;
	LONG	caps_flags;
	if( FAILED( hr = vap_get_parameter_range( property, &min, &max, &stepping_delta, &default_value, &caps_flags ) ) )
		return( hr );

	LONG value_new = min + LONG( ( value * DOUBLE(max - min) ) / DOUBLE(stepping_delta) ) * stepping_delta;
	if( value_new == value_cur && b_auto == b_auto_cur )
		return S_OK;
	return vap_set_parameter( property, value_new, b_auto );
}
// ----------------------------------------------------------------------------------------------------------
HRESULT	c_capture_dshow::vap_set_parameters_to_default( LONG property, bool b_auto )
{
	LONG	min;
	LONG	max;
	LONG	stepping_delta;
	LONG	default_value;
	LONG	caps_flags;

	HRESULT	hr;
	if( FAILED( hr = vap_get_parameter_range( property, &min, &max, &stepping_delta, &default_value, &caps_flags ) ) )
		return( hr );
	if( FAILED( hr = vap_set_parameter( property, default_value, b_auto ) ) )
		return( hr );

	return(S_OK);
}

/*
void	c_capture_dshow::set_vpa_use_default( bool b_use_default, bool b_use_default_auto )
{
	// reset capture card video proc amp
	if( _p_video_proc_amp )
		if( _b_vpa_first || ( _b_vpa_use_default != b_use_default ) || ( _b_vpa_use_default_auto != b_use_default_auto ) )
			if( SUCCEEDED( vap_reset_parameters( b_use_default_auto ) ) )
			{
				_b_vpa_first = false;
				_b_vpa_use_default = b_use_default;
				_b_vpa_use_default_auto = b_use_default_auto;
			}
}
*/

//void	set_vpa_fn( REAL p_in, REAL& v_last, bool b_auto_in, bool& b_last, tagVideoProcAmpProperty tag )
//{
//	if( ( p_in != v_last ) || ( b_auto_in != v_last ) )
//	{
//		if( SUCCEEDED( vap_set_parameters_norm( tag, p_in, b_auto_in ) ) )
//		{
//			v_last = p_in;
//			b_last = b_auto_in;
//		}
//	}
//}

#define	DEFINE_SET_VPA_FN( what, what_str ) \
	void	c_capture_dshow::set_vpa_##what( REAL r_in, bool b_auto_in )\
{\
	if( _p_video_proc_amp )\
	{\
		if( ( r_in != _##what ) || ( b_auto_in != _b_##what##_auto ) )\
			if( SUCCEEDED( vap_set_parameters_norm( VideoProcAmp_##what_str, r_in, b_auto_in ) ) )\
			{\
				_##what = r_in;\
				_b_##what##_auto = b_auto_in;\
			}\
	}\
}

#if 0
//before macro here as reference only and debug
void	c_capture_dshow::set_vpa_brightness( REAL p_in, bool b_auto_in )
{
	if( _p_video_proc_amp )
		if( ( p_in != _brightness ) || ( b_auto_in != _b_brightness_auto ) )
			if( SUCCEEDED( vap_set_parameters_norm( VideoProcAmp_Brightness, p_in, b_auto_in ) ) )
			{
				_brightness = p_in;
				_b_brightness_auto = b_auto_in;
			}
}

#else
	DEFINE_SET_VPA_FN( brightness,				Brightness )
#endif
DEFINE_SET_VPA_FN( contrast,				Contrast )
DEFINE_SET_VPA_FN( hue,						Hue )
DEFINE_SET_VPA_FN( saturation,				Saturation )
DEFINE_SET_VPA_FN( sharpness,				Sharpness )
DEFINE_SET_VPA_FN( gamma,					Gamma )
DEFINE_SET_VPA_FN( color_enable,			ColorEnable )
DEFINE_SET_VPA_FN( white_balance,			WhiteBalance )
DEFINE_SET_VPA_FN( backlight_compensation,	BacklightCompensation )
DEFINE_SET_VPA_FN( gain,					Gain )

