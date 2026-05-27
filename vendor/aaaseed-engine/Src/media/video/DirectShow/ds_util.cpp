#include "ds_util.h"
#include "aaa_util.h"
#include "DXBaseClasses/Mtype.h"
#include "system/shared/SystemUtils.h"
#include "DXBaseClasses/qedit_maa.h"


#include <lib_use.h>
#if AAA_DEBUG()
#	define LIB_NAME "strmbasd"
#else
#	define LIB_NAME "strmbase"
#endif

AAA_LIB_USE( "DXBaseClasses/"AAA_LIB_VSTOOL_STR()"/"LIB_NAME )

FOURCCMap	MEDIASUBTYPE_I420	( MAKEFOURCC ('I','4','2','0') );
FOURCCMap	MEDIASUBTYPE_IYU2	( MAKEFOURCC ('I','Y','U','2') );
FOURCCMap	MEDIASUBTYPE_HDYC	( MAKEFOURCC ('H','D','Y','C') );
FOURCCMap	MEDIASUBTYPE_v210	( MAKEFOURCC ('v','2','1','0') );

namespace ds_helper
{

	DOUBLE	round_double( DOUBLE val, INT32 precision )
	{
		static	const	DOUBLE	do_base = 10.0f;
		DOUBLE	do_complete_5;
		DOUBLE	do_complete_5i;

		do_complete_5 = val * pow( do_base, (DOUBLE) ( precision + 1 ) );

		if( val < 0.0f )
			do_complete_5 -= 5.0f;
		else
			do_complete_5 += 5.0f;

		do_complete_5 /= do_base;
		modf( do_complete_5, &do_complete_5i );

		return do_complete_5i / pow( do_base, (DOUBLE) precision );
	}

	DOUBLE	avg_2_fps( REFERENCE_TIME avg_time_per_frame, INT32 precision )
	{
		return( round_double( 10000000.0f / (DOUBLE)avg_time_per_frame, precision ) ); 
	}

	// ds_add_graph_to_running_object_table, needed to connect to graph in real time with graphedit
	HRESULT	add_to_running_object_table( CComPtr<IGraphBuilder> p_graph, DWORD* p_register ) 
	{
		CComPtr<IMoniker>				p_moniker;
		CComPtr<IRunningObjectTable>	p_rot;
		WCHAR							wsz[256];

		if( FAILED( GetRunningObjectTable( 0, &p_rot ) ) )
			return E_FAIL;

		wsprintfW( wsz, L"FilterGraph %08p pid %08x", (DWORD_PTR)((IGraphBuilder*)p_graph), GetCurrentProcessId() );
		HRESULT	hr = CreateItemMoniker( L"!", wsz, &p_moniker );
		if( SUCCEEDED( hr ) )
		{
			hr = p_rot->Register( 0, p_graph, p_moniker, p_register );
			p_moniker = nullptr;
		}
		return hr;
	}

	// Remove from RunningObjectTable
	void	remove_from_object_table( DWORD* p_register )
	{
		if( p_register )
		{
			CComPtr<IRunningObjectTable>	p_rot;
			if( SUCCEEDED( GetRunningObjectTable( 0, &p_rot ) ) )
			{
				p_rot->Revoke( *p_register );
				p_rot = nullptr;
			}
			*p_register = 0; 
		}
	}
#if 0
	void destroy_graph( CComPtr<IGraphBuilder>& graph )
	{
		if (!graph)
			return;

		CComPtr<IMediaControl> mc;
		if (SUCCEEDED(graph->QueryInterface(&mc)))
		{
			mc->Stop();
			Sleep(50); // allow streaming threads to exit
		}

		graph.Release(); // graph releases all filters safely
	}
#else
	void destroy_graph( IGraphBuilder& p_graph )
	{
		// Abort the graph immediately
		p_graph.Abort();

		// Enumerate the filters in the graph
		CComPtr<IEnumFilters> p_enum;
		HRESULT hr = p_graph.EnumFilters(&p_enum);
		if (FAILED(hr) || !p_enum)
			return;

		CComPtr<IBaseFilter> p_filter;
		ULONG fetched = 0;

		while (S_OK == p_enum->Next(1, &p_filter, &fetched) && fetched == 1)
		{
	#if AAA_DEBUG()
			{
				FILTER_INFO info;
				if (SUCCEEDED(p_filter->QueryFilterInfo(&info)))
				{
					GOOD_PRINT_STRING("Remove Filter : %ls", info.achName);
					//if (info.pGraph) info.pGraph->Release();
				}
			}
	#endif

			// Stop the filter if it's running
			FILTER_STATE state;
			if (SUCCEEDED(p_filter->GetState(1, &state)) && state != State_Stopped)
			{
				p_filter->Stop();
			}
	#if AAA_DEBUG()
			else
			{
				DBG_PRINT_STRING("  filter not running so we don't stop");
			}
	#endif

			// Remove the filter from the graph
			p_graph.RemoveFilter(p_filter);
			p_filter = nullptr;
			// Reset the enumerator to continue safely
			p_enum->Reset();

			// Reset fetched for next iteration
			fetched = 0;
		}
	}
#endif
	bool	set_callback( ISampleGrabberCB* grabber_cb, ISampleGrabber* sample_grabber, bool b_set )
	{
		if( sample_grabber )
		{
			HRESULT hr;
			bool b_ret = false;
			if( b_set )
			{
				hr = sample_grabber->SetBufferSamples( FALSE );
				if( FAILED( hr ) )
					ERR_PRINT_STRING( "DirectShow could not set capture to sample callback !!"	);
				else
					{
					//	DIRECTSHOW_CHECK( sampleGrabber_->SetOneShot( TRUE ),			"DirectShow could not set sample callback to OneShot!!"	);				
					CComPtr<ISampleGrabberCB>	pCB = nullptr;
					// query interface
					hr = grabber_cb->QueryInterface( IID_ISampleGrabberCB, reinterpret_cast<void**>(&pCB) );
					//hr = pCB.QueryInterface( IID_ISampleGrabberCB );
					if( FAILED( hr ) )
						ERR_PRINT_STRING( "%s, %s in %s()", "DirectShow, could not query callback !!", get_error_string( hr ), __FUNCTION__ );
					else
					{
						hr = sample_grabber->SetCallback( pCB, 0 );
						b_ret = SUCCEEDED(hr);
						if( b_ret )
							GOOD_PRINT_STRING( "DirectShow callback set" );
						else
							ERR_PRINT_STRING( "%s, %s in %s()", "DirectShow could not set callback !!", get_error_string( hr ), __FUNCTION__ );
					}
					pCB = nullptr;
				}
			}
			else
			{
				hr = sample_grabber->SetCallback( nullptr, 0 );
				b_ret = SUCCEEDED(hr);
				if( b_ret )
					GOOD_PRINT_STRING( "DirectShow callback cleared" );
				else
					ERR_PRINT_STRING( "%s, %s in %s()", "DirectShow could not clear callback !!", get_error_string( hr ), __FUNCTION__ );
				return true;
			}
			return b_ret;
		}
		if( b_set )
		{
			ERR_PRINT_STRING( "%s in %s()", "we shouldn't be here anyway : no sample_grabber !!", __FUNCTION__ );
			return false;
		}
		return true;
	}
	
	// checks if a pin can deliver majortype
	bool	pin_can_deliver_major_type( CComPtr<IPin> pin, CONST GUID& majortype )
	{
		bool	b_ret = false;
		if( pin )
		{
			CComPtr<IEnumMediaTypes>	enum_mt;
			HRESULT hr = pin->EnumMediaTypes( &enum_mt );
			if( SUCCEEDED(hr) )
			{
				enum_mt->Reset();
				AM_MEDIA_TYPE*	p_mt = nullptr;
				while( S_OK == ( hr = enum_mt->Next( 1, &p_mt, nullptr ) ) )
				{
					bool b_found =	(majortype == p_mt->majortype);
					DeleteMediaType( p_mt );
					if( b_found )
					{
						b_ret = true;
						break;
					}
				}
			}
			enum_mt = nullptr;
		}
		return b_ret;
	}

	bool	pin_can_deliver_sub_type( CComPtr<IPin> pin, CONST std::vector<GUID>& subtypes ) 
	{
		bool	b_ret = false;
		if( pin && !subtypes.empty() )
		{
			CComPtr<IEnumMediaTypes> enum_mt;
			HRESULT hr = pin->EnumMediaTypes( &enum_mt );
			if( SUCCEEDED(hr) )
			{
				enum_mt->Reset();
				AM_MEDIA_TYPE*	p_mt = nullptr;
				while( S_OK == ( hr = enum_mt->Next( 1, &p_mt, nullptr ) ) )
				{
					bool b_found =  false;
					GUID subtype = p_mt->subtype;
					for( auto const & sub : subtypes )
					{
						if( sub==subtype )
						{
							b_found = true;
							break;
						}
					}
					DeleteMediaType( p_mt ); 
					if( b_found )
					{
						b_ret = true;
						break;
					}
				}
			}
			enum_mt = nullptr;
		}
		return b_ret;
	}

	/*
	FOURCC	GUID				Data Rate	Description 
	'dvsl'	MEDIASUBTYPE_dvsl	12.5 Mbps	SD-DVCR 525-60 or SD-DVCR 625-50 
	'dvsd'	MEDIASUBTYPE_dvsd	25 Mbps		SDL-DVCR 525-60 or SDL-DVCR 625-50 
	'dvhd'	MEDIASUBTYPE_dvhd	50 Mbps		HD-DVCR 1125-60 or HD-DVCR 1250-50 
	*/

	bool pin_can_deliver_mjpg( CComPtr<IPin> pin )
	{
#if 1
		std::vector<GUID> subtypes;
		subtypes.push_back( MEDIASUBTYPE_MJPG );
		return pin_can_deliver_sub_type( pin, subtypes );
#else
		CComPtr<IEnumMediaTypes> enumMT;
		if(	FAILED(pin->EnumMediaTypes(&enumMT)) )
			return false;
		
		AM_MEDIA_TYPE* mt = nullptr;
		bool b_found;
		while( enumMT->Next(1, &mt, nullptr) == S_OK)
		{
			bool b_found = mt->majortype == MEDIATYPE_Video && mt->subtype == MEDIASUBTYPE_MJPG;
			DeleteMediaType(mt);
			if( b_found )
				break;
		}
		return b_found;
#endif
	}

	// checks if a pin can deliver compressed DV formats
	// (refer to Microsoft DirectX Documentation, "DV Video Subtypes" for more information)
	bool pin_can_deliver_dv( CComPtr<IPin> pin ) 
	{
		std::vector<GUID> subtypes;
		subtypes.push_back( MEDIASUBTYPE_dvsl );
		subtypes.push_back( MEDIASUBTYPE_dvsd );
		subtypes.push_back( MEDIASUBTYPE_dvhd );
		return pin_can_deliver_sub_type( pin, subtypes );
	}

	// checks if a pin can deliver MEDIAFORMAT_Video
	bool	pin_can_deliver_video( CComPtr<IPin> pin )
	{
		return pin_can_deliver_major_type( pin, MEDIATYPE_Video );
	}

	//-----------------------------------------------------------------------------
	// Name: DXUtil_ConvertAnsiStringToWide
	// Desc: This is a UNICODE conversion utility to convert a CHAR string into a
	//       WCHAR string. 
	//       cchDestChar is the size in TCHARs of wstrDestination.  Be careful not to 
	//       pass in sizeof(strDest) 
	//-----------------------------------------------------------------------------
	HRESULT	DXUtil_ConvertAnsiStringToWide( WCHAR* wstrDestination, const CHAR* strSource, int cchDestChar )
	{
		if( IS_NULL( wstrDestination ) || IS_NULL( strSource ) || cchDestChar < 1 )
			return E_INVALIDARG;

		int	nResult = MultiByteToWideChar( CP_ACP, 0, strSource, -1, wstrDestination, cchDestChar );
		wstrDestination[ cchDestChar - 1 ] = 0;

		if( nResult == 0 )
			return E_FAIL;
		return S_OK;
	}

	aaa::PIXEL_FORMAT	get_aaa_pixel_format( AM_MEDIA_TYPE CONST * media_type )
	{
		aaa::PIXEL_FORMAT pixel_format;
		if( media_type->formattype == FORMAT_VideoInfo )
		{
			GUID CONST & subtype = media_type->subtype;
			
			if(			subtype==MEDIASUBTYPE_MJPG )
			{	
				pixel_format = aaa::PIXEL_FORMAT::UNKNOWN;
				pixel_format = aaa::PIXEL_FORMAT::BGR_8;
				//ERR_PRINT_STRING( "video format is MJPG but unsupported in AAASEED for now" );
			}
			else if	(	subtype==MEDIASUBTYPE_UYVY )	{	pixel_format = aaa::PIXEL_FORMAT::UYVY;		}
			else if	(	subtype==MEDIASUBTYPE_YUY2 )	{	pixel_format = aaa::PIXEL_FORMAT::YUY2;		}
			else if	(	subtype==MEDIASUBTYPE_YVYU )	{	pixel_format = aaa::PIXEL_FORMAT::YVYU;		}
			else if	(	subtype==MEDIASUBTYPE_v210 )	{	pixel_format = aaa::PIXEL_FORMAT::V210;		}	//todo check
			else if	(	subtype==MEDIASUBTYPE_I420 )	{	pixel_format = aaa::PIXEL_FORMAT::I420;		}
			else if	(	subtype==MEDIASUBTYPE_NV12 )	{	pixel_format = aaa::PIXEL_FORMAT::NV12;		}
			else if	(	subtype==MEDIASUBTYPE_RGB24 )	{	pixel_format = aaa::PIXEL_FORMAT::BGR_8;	}
			else if	(	subtype==MEDIASUBTYPE_RGB32 )	{	pixel_format = aaa::PIXEL_FORMAT::BGRA_8;	}
			else if	(	subtype==MEDIASUBTYPE_RGB8 )	{	pixel_format = aaa::PIXEL_FORMAT::R_8;		}	// with a palette
		
			else if	(	subtype==MEDIASUBTYPE_Y211 )	{	pixel_format = aaa::PIXEL_FORMAT::Y211;		}
			else if	(	subtype==MEDIASUBTYPE_RGB565 )	{	pixel_format = aaa::PIXEL_FORMAT::RGB_565;	}
			else if	(	subtype==MEDIASUBTYPE_RGB555 )	{	pixel_format = aaa::PIXEL_FORMAT::RGB_555;	}

			else if (	subtype==MEDIASUBTYPE_HDYC )	{	pixel_format = aaa::PIXEL_FORMAT::HDYC;		}
			else if	(	subtype==MEDIASUBTYPE_IYUV )	{	pixel_format = aaa::PIXEL_FORMAT::IYUV;		}
			else if	(	subtype==MEDIASUBTYPE_YVU9 )	{	pixel_format = aaa::PIXEL_FORMAT::YVU9;		}
			else if	(	subtype==MEDIASUBTYPE_Y411 )	{	pixel_format = aaa::PIXEL_FORMAT::YUV_411;	}
			else if	(	subtype==MEDIASUBTYPE_Y41P )	{	pixel_format = aaa::PIXEL_FORMAT::Y41P;		}
			else
			{
				CHAR name[5];
				ds_helper::fourcc_as_string( subtype, name );
				pixel_format = aaa::PIXEL_FORMAT::UNKNOWN;
				if(	subtype==MEDIASUBTYPE_Y800 || subtype==MEDIASUBTYPE_Y8 )
					ERR_PRINT_STRING( "%s() %s Video format subtype %s knowned but unsupported by AAASEED for now.", __FUNCTION__, name );
				else
					ERR_PRINT_STRING( "%s() Video format subtype %s is Unknowned and so unsupported by AAASEED for now.", __FUNCTION__, name );
				print_guid( subtype );
			}
		}
		else
		{
			pixel_format = aaa::PIXEL_FORMAT::UNKNOWN;
			ERR_PRINT_STRING( "%s() formattype is not FORMAT_VideoInfo", __FUNCTION__ );
		}
		return pixel_format;
	}

	
	void add_media_info_base( o_str & o, st_media_info CONST * info )
	{
		o.add( info->fourcc );
		o.add_space();
		o.add( info->size_x );
		o.add_char( 'x' );
		o.add( info->size_y );
		o.add( " @ " );
		o.add_fp32_2decimals( info->fps );
		o.add( "fps" );
	}

	void add_media_info( o_str & o, st_media_info CONST * info )
	{
		add_media_info_base( o, info );
		o.add( " -> " );
		o.add( info->str_format );
		o.add_space();
		o.add( info->bits_by_pixel );
		o.add( " bits by pixel" );
		if( info->pixel_format == aaa::PIXEL_FORMAT::UNKNOWN )
			o.add( " : UNIMPLEMENTED" );
	}

	bool	is_same_video_info( st_media_info CONST & a, st_media_info CONST & b )
	{
		return		a.size_x == b.size_x
				&&	a.size_y == b.size_y
				&&	a.fps == b.fps
				&&	a.pixel_format == b.pixel_format
				&&	a.pitch == b.pitch
				&&	strcmp( a.fourcc, b.fourcc ) == 0;
	}

	bool	get_video_info( AM_MEDIA_TYPE CONST * media_type, st_media_info& info )
	{
		//info.pixel_format = aaa::PIXEL_FORMAT::UNKNOWN;
		//info.format_str = nullptr;
		if( media_type->formattype == FORMAT_VideoInfo )
		{
			VIDEOINFOHEADER CONST * CONST pvih = ( VIDEOINFOHEADER * ) media_type->pbFormat;
			info.size_x			= pvih->bmiHeader.biWidth;
			info.size_y			= pvih->bmiHeader.biHeight;
			info.pitch			= ((((pvih->bmiHeader.biWidth * pvih->bmiHeader.biBitCount) + 31) & ~31) >> 3);
			info.fps			= FP32( avg_2_fps( pvih->AvgTimePerFrame, 3 ) );
			info.bits_by_pixel	= pvih->bmiHeader.biBitCount;
			info.pixel_format	= get_aaa_pixel_format( media_type );
			info.str_format		= aaa::c_pixel_format::get_name( info.pixel_format );
			fourcc_as_string( media_type->subtype, info.fourcc );

			//DBG_PRINT_STRING( "%s() Identified a media with format %.128s", __FUNCTION__, info.str_format );

			//this is a check
			if( info.pixel_format != aaa::PIXEL_FORMAT::UNKNOWN )
			{
				INT32 src_bit_per_pixel	= aaa::c_pixel_format::get_bits_per_pixel( info.pixel_format );
				if( info.bits_by_pixel != src_bit_per_pixel )
					debug_break( "windows say %d bits by pixel and AAASeed say %d", info.bits_by_pixel, src_bit_per_pixel );
			}
			return true;
		}
		return false;
	}

	void	get_media_info( AM_MEDIA_TYPE CONST * media_type, st_media_info& info )
	{
		if( !get_video_info( media_type, info ) )
		{
			//todo, what do we do with the other format, these are defined mostly for dvd playback
			info.size_x = 0;
			info.size_y = 0;
			info.pixel_format = aaa::PIXEL_FORMAT::UNKNOWN;
			//info.fps = 1;
			if		( media_type->formattype==FORMAT_DvInfo )		{	info.str_format = "DvInfo";				}
			else if	( media_type->formattype==FORMAT_MPEGVideo )	{	info.str_format = "MPEG1VIDEOINFO";		}
			else if	( media_type->formattype==FORMAT_MPEG2Video )	{	info.str_format = "MPEG2VIDEOINFO";		}
			else if	( media_type->formattype==FORMAT_VideoInfo2 )	{	info.str_format = "VIDEOINFOHEADER2";	}
			else if	( media_type->formattype==FORMAT_WaveFormatEx )	{	info.str_format = "WAVEFORMATEX";		}
			else if	( media_type->formattype==FORMAT_None
					|| media_type->formattype==GUID_NULL )			{	info.str_format = "None";				}
			else													{	info.str_format = "Unknown";			}
		}
		/*
		if( !format_str && (pixel_format != c_pixel_format::NOT_SUPPORTED) )
		{
			format_str			=	c_pixel_format::get_name( pixel_format );
			src_bit_per_pixel	=	c_pixel_format::get_bit_per_pixel( pixel_format );
			b_supported			=	c_pixel_format::get_supported( pixel_format );
			b_y_inverted		=	c_pixel_format::get_y_inverted( pixel_format );
			b_luma_flip			=	c_pixel_format::get_luma_inverted( pixel_format );
		}
		*/
	}

	CComPtr<IPin>	get_pin( CComPtr<IBaseFilter> p_filter, PIN_DIRECTION pin_dir_asked )
	{
		bool				b_found = false;
		CComPtr<IEnumPins>	p_enum;
		CComPtr<IPin>		pin;

		HRESULT	hr = p_filter->EnumPins( &p_enum );
		if( FAILED(hr) )
		{
			return nullptr;
		}
		int	i = 0;
		while( p_enum->Next( 1, &pin, 0 ) == S_OK )
		{
			PIN_DIRECTION pin_dir;
			pin->QueryDirection( &pin_dir );
			b_found = ( pin_dir_asked == pin_dir );
			if( b_found )
				break;
			pin = nullptr;
		}
		p_enum = nullptr;
		return ( b_found ? pin : nullptr );
	}

	HRESULT	get_pin( CComPtr<IBaseFilter> flt, PIN_DIRECTION pin_dir_asked, INT32 number, CComPtr<IPin>& p_pin_ret )
	{
		INT32				n = 0;
		CComPtr<IPin>		pin;
		CComPtr<IEnumPins>	enum_pins;
		ULONG				fetched;
		PIN_INFO			p_info;

		flt->EnumPins( &enum_pins );
		enum_pins->Reset();
		enum_pins->Next( 1, &pin, &fetched );
		pin->QueryPinInfo( &p_info );

		do
		{
			// the pFilter member has an outstanding ref count -> release it, we do not use it anyways!
			sysutils::safe_release( p_info.pFilter );
			if( p_info.dir == pin_dir_asked )
			{
				++n;
				if( n == number ) 
				{
					enum_pins = nullptr;
					p_pin_ret = pin;
					return S_OK;
				}
				else
				{
					pin = nullptr;
					enum_pins->Next( 1, &pin, &fetched );
					if( fetched == 0 ) // no more pins
					{
						enum_pins = nullptr;
						p_pin_ret = nullptr;
						return E_FAIL;
					}
					pin->QueryPinInfo( &p_info );
				}
			}
			else //if ( pinfo.dir != dir )
			{
				pin = nullptr;
				enum_pins->Next( 1, &pin, &fetched );
				if( fetched == 0 ) // no more pins
				{
					enum_pins = nullptr;
					p_pin_ret = nullptr;
					return E_FAIL;
				}
				pin->QueryPinInfo( &p_info );
				//pinfo.pFilter->Release();
			}
		}
		while( IS_NOT_NULL( pin ) );

		enum_pins = nullptr;
		return E_FAIL;
	}

	HRESULT	get_pins(	CComPtr<IBaseFilter>& filter_out,	INT32 out_pin_nr,	CComPtr<IPin>& pin_out,
						CComPtr<IBaseFilter>& in_filter,	INT32 in_pin_nr,	CComPtr<IPin>& pin_in
	)
	{
		HRESULT			hr;
		pin_out = nullptr;
		pin_in = nullptr;
		if( FAILED( hr = get_pin( filter_out, PINDIR_OUTPUT, out_pin_nr, pin_out ) ) )
			return hr;
		if( FAILED( hr = get_pin( in_filter, PINDIR_INPUT, in_pin_nr, pin_in ) ) )
			return hr;
		if( IS_NULL( pin_out ) || IS_NULL( pin_in ) )
			return E_FAIL;
		return S_OK;
	}

	HRESULT	auto_connect_filters( CComPtr<IBaseFilter> filter_out, INT32 out_pin_nr, CComPtr<IBaseFilter> in_filter, INT32 in_pin_nr, CComPtr<IGraphBuilder> p_graph_builder )
	{
		CComPtr<IPin>	pin_out;
		CComPtr<IPin>	pin_in;
		HRESULT hr = get_pins(	filter_out,	out_pin_nr,	pin_out, in_filter, in_pin_nr,pin_in );
		if( SUCCEEDED( hr ) )
			hr = p_graph_builder->Connect( pin_out, pin_in );
		return hr;
	}

	HRESULT	connect_filters( CComPtr<IBaseFilter> filter_out, INT32 out_pin_nr, CComPtr<IBaseFilter> in_filter, INT32 in_pin_nr )
	{
		CComPtr<IPin>	pin_out;
		CComPtr<IPin>	pin_in;
		HRESULT hr = get_pins(	filter_out,	out_pin_nr,	pin_out, in_filter, in_pin_nr,pin_in );
		if( SUCCEEDED( hr ) )
			hr = pin_out->Connect( pin_in, nullptr );
		return hr;
	}

	//-----------------------------------------------------------------------------
	// GetPin
	// Find the pin of the specified format type on the given filter
	// This method leaves an outstanding reference on the pin if successful
	HRESULT	get_pin( CComPtr<IBaseFilter> p_filter, const GUID* p_format, PIN_DIRECTION pin_dir_asked, CComPtr<IPin> &p_pin_ret )
	{
		HRESULT	hr = S_OK;

//		if (pFilter && pFormat && pRetPin)
		if ( p_filter && p_format )
		{
			CComPtr<IEnumPins>	pIEnumPins = nullptr;
			hr = p_filter->EnumPins( &pIEnumPins );
			if( SUCCEEDED(hr) )
			{
				// find the pin with the specified format
				CComPtr<IPin>	pin = nullptr;
				while( S_OK == pIEnumPins->Next( 1, &pin, nullptr ) )
				{
					// match the pin direction
					PIN_DIRECTION	pin_dir;
					pin->QueryDirection( &pin_dir );
					if( pin_dir == pin_dir_asked  )
					{
						if( pin_can_deliver_major_type( pin, *p_format ) )
						{
							// found the pin with the specified format
							p_pin_ret = pin;
							break;
						}
					}
					pin = nullptr;
				}

				if( nullptr == p_pin_ret )
					// failed to find the named pin
					hr = E_FAIL;
			}
		}
		else
			hr = E_INVALIDARG;
		return hr;
	}

	//-----------------------------------------------------------------------------
	// FindPinInterface
	// Attempt to locate the interface on the pin with the specified format or on the first pin if no format is provided.
	HRESULT	find_pin_interface( CComPtr<IBaseFilter> p_filter, CONST GUID* p_format, PIN_DIRECTION pin_dir_asked, CONST IID& riid, void** ppvInterface )
	{
		HRESULT	hr = S_OK;
		if( p_filter && ppvInterface )
		{
			CComPtr<IPin>	pin = nullptr;
			if( p_format )
				hr = get_pin( p_filter, p_format, pin_dir_asked, pin );
			else
			{
				CComPtr<IEnumPins>	enum_pins = nullptr;
				hr = p_filter->EnumPins( &enum_pins );
				if( SUCCEEDED(hr) )
					hr = enum_pins->Next( 1, &pin, nullptr );
			}
			if( SUCCEEDED(hr) )
				hr = pin->QueryInterface( riid, ppvInterface );
		}
		else
			hr = E_INVALIDARG;
		return hr;
	}

	HRESULT	find_pin_interface_for_stream( CComPtr<IBaseFilter> p_filter, PIN_DIRECTION pin_dir_asked, void** ppvInterface )
	{
		HRESULT hr = find_pin_interface( p_filter, &MEDIATYPE_Interleaved,	pin_dir_asked, IID_IAMStreamConfig, ppvInterface );
		if( FAILED(hr) )
			hr = find_pin_interface( p_filter, &MEDIATYPE_Video, pin_dir_asked,  IID_IAMStreamConfig,  ppvInterface );
		return hr;
	}

	static	FINLINE	DWORD	fourcc_get_unsafe( const GUID& subtype )
	{
		return reinterpret_cast<FOURCCMap&>(const_cast <GUID&>(subtype)).GetFOURCC();
	}

	static	FINLINE	bool	is_fourcc ( const GUID& subtype )
	{
		return FOURCCMap (fourcc_get_unsafe(subtype)) == subtype;
	}

	static	FINLINE	DWORD	fourcc_get( const GUID& subtype )
	{
		if( !is_fourcc( subtype ) )
			return 0;
		return fourcc_get_unsafe( subtype );
	}

	static	FINLINE	void	fourcc_as_string( DWORD fourcc, CHAR str[5] )
	{
		str[0] = (CHAR)	( fourcc & 0xff );
		str[1] = (CHAR)	( fourcc >> 8 ) & 0xff;
		str[2] = (CHAR)	( fourcc >> 16 ) & 0xff;
		str[3] = (CHAR)	( fourcc >> 24 ) &  0xff;
		str[4] = 0;
	}

	FINLINE	void	fourcc_as_string ( const GUID& subtype, CHAR str[5] )
	{
		fourcc_as_string( fourcc_get( subtype ), str );
	}


	void print_guid( CONST GUID& guid )
	{
		//DBG_PRINT_STRING( "Guid is %08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX", 
		//			guid.Data1,
		//			guid.Data2,
		//			guid.Data3, 
		//			guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]
		//		);

		OLECHAR*	bstrGuid;
		//StringFromCLSID(guid, &bstrGuid);
		StringFromIID( guid, &bstrGuid );
		CHAR	buf_name[512];
		WideCharToMultiByte( CP_ACP, 0, bstrGuid, -1, buf_name, 512, nullptr, nullptr );

		GOOD_PRINT_STRING( "GUID : %s", buf_name );

		CHAR	name[5];
		fourcc_as_string ( fourcc_get( guid ), name );
		GOOD_PRINT_STRING( "FOURCC : %s", name );

		// ensure memory is freed
		::CoTaskMemFree( bstrGuid );
	}

	static	CHAR	err_buf[256];
	C_PCHAR	get_error_string( CONST HRESULT hr )
	{
#define DS_X( err, msg ) case err: return msg;
		switch( hr )
		{
			DS_X( VFW_E_INVALIDMEDIATYPE,		"An invalid media type was specified." )
			DS_X( VFW_E_INVALIDSUBTYPE,			"An invalid media subtype was specified." )
			DS_X( VFW_E_NEED_OWNER,				"This object can only be created as an aggregated object." )
			DS_X( VFW_E_ENUM_OUT_OF_SYNC,		"The enumerator has become invalid." )
			DS_X( VFW_E_ALREADY_CONNECTED,		"At least one of the pins involved in the operation is already connected." )
			DS_X( VFW_E_FILTER_ACTIVE,			"This operation cannot be performed because the filter is active." )
			DS_X( VFW_E_NO_TYPES,				"One of the specified pins supports no media types." )
			DS_X( VFW_E_NO_ACCEPTABLE_TYPES,	"There is no common media type between these pins." )
			DS_X( VFW_E_INVALID_DIRECTION,		"Two pins of the same direction cannot be connected together." )
			DS_X( VFW_E_NOT_CONNECTED,			"The operation cannot be performed because the pins are not connected." )
			DS_X( VFW_E_NO_ALLOCATOR,			"No sample buffer allocator is available." )
			DS_X( VFW_E_RUNTIME_ERROR,			"A run-time error occurred." )
			DS_X( VFW_E_BUFFER_NOTSET,			"No buffer space has been set." )
			DS_X( VFW_E_BUFFER_OVERFLOW,		"The buffer is not big enough." )
			DS_X( VFW_E_BADALIGN,				"An invalid alignment was specified." )
			DS_X( VFW_E_ALREADY_COMMITTED,		"Cannot change allocated memory while the filter is active." )
			DS_X( VFW_E_BUFFERS_OUTSTANDING,	"One or more buffers are still active." )
			DS_X( VFW_E_NOT_COMMITTED,			"Cannot allocate a sample when the allocator is not active." )
			DS_X( VFW_E_SIZENOTSET,				"Cannot allocate memory because no size has been set." )
			DS_X( VFW_E_NO_CLOCK,				"Cannot lock for synchronization because no clock has been defined." )
			DS_X( VFW_E_NO_SINK,				"Quality messages could not be sent because no quality sink has been defined." )
			DS_X( VFW_E_NO_INTERFACE,			"A required interface has not been implemented." )
			DS_X( VFW_E_NOT_FOUND,				"An object or name was not found." )
			DS_X( VFW_E_CANNOT_CONNECT,			"No combination of intermediate filters could be found to make the connection." )
			DS_X( VFW_E_CANNOT_RENDER,			"No combination of filters could be found to render the stream." )
			DS_X( VFW_E_CHANGING_FORMAT,		"Could not change formats dynamically." )
			DS_X( VFW_E_NO_COLOR_KEY_SET,		"No color key has been set." )
			DS_X( VFW_E_NOT_OVERLAY_CONNECTION,	"Current pin connection is not using the IOverlay transport." )
			DS_X( VFW_E_NOT_SAMPLE_CONNECTION,	"Current pin connection is not using the IMemInputPin transport." )
			DS_X( VFW_E_PALETTE_SET,			"Setting a color key would conflict with the palette already set." )
			DS_X( VFW_E_COLOR_KEY_SET,			"Setting a palette would conflict with the color key already set." )
			DS_X( VFW_E_NO_COLOR_KEY_FOUND,		"No matching color key is available." )
			DS_X( VFW_E_NO_PALETTE_AVAILABLE,	"No palette is available." )
			DS_X( VFW_E_NO_DISPLAY_PALETTE,		"Display does not use a palette." )
			DS_X( VFW_E_TOO_MANY_COLORS,		"Too many colors for the current display settings." )
			DS_X( VFW_E_STATE_CHANGED,			"The state changed while waiting to process the sample." )
			DS_X( VFW_E_NOT_STOPPED,			"The operation could not be performed because the filter is not stopped." )
			DS_X( VFW_E_NOT_PAUSED,				"The operation could not be performed because the filter is not paused." )
			DS_X( VFW_E_NOT_RUNNING,			"The operation could not be performed because the filter is not running." )
			DS_X( VFW_E_WRONG_STATE,			"The operation could not be performed because the filter is in the wrong state." )
			DS_X( VFW_E_START_TIME_AFTER_END,	"The sample start time is after the sample end time." )
			DS_X( VFW_E_INVALID_RECT,			"The supplied rectangle is invalid." )
			DS_X( VFW_E_TYPE_NOT_ACCEPTED,		"This pin cannot use the supplied media type." )
			DS_X( VFW_E_SAMPLE_REJECTED,		"This sample cannot be rendered." )
			DS_X( VFW_E_SAMPLE_REJECTED_EOS,	"This sample cannot be rendered because the end of the stream has been reached." )
			DS_X( VFW_E_DUPLICATE_NAME,			"An attempt to add a filter with a duplicate name failed." )
			DS_X( VFW_S_DUPLICATE_NAME,			"An attempt to add a filter with a duplicate name succeeded with a modified name." )
			DS_X( VFW_E_TIMEOUT,				"A time-out has expired." )
			DS_X( VFW_E_INVALID_FILE_FORMAT,	"The file format is invalid." )
			DS_X( VFW_E_ENUM_OUT_OF_RANGE,		"The list has already been exhausted." )
			DS_X( VFW_E_CIRCULAR_GRAPH,			"The filter graph is circular." )
			DS_X( VFW_E_NOT_ALLOWED_TO_SAVE,	"Updates are not allowed in this state." )
			DS_X( VFW_E_TIME_ALREADY_PASSED,	"An attempt was made to queue a command for a time in the past." )
			DS_X( VFW_E_ALREADY_CANCELLED,		"The queued command has already been canceled." )
			DS_X( VFW_E_CORRUPT_GRAPH_FILE,		"Cannot render the file because it is corrupt." )
			DS_X( VFW_E_ADVISE_ALREADY_SET,		"An overlay advise link already exists." )
			DS_X( VFW_S_STATE_INTERMEDIATE,		"The state transition has not completed." )
			DS_X( VFW_E_NO_MODEX_AVAILABLE,		"No full-screen modes are available." )
			DS_X( VFW_E_NO_ADVISE_SET,			"This Advise cannot be canceled because it was not successfully set." )
			DS_X( VFW_E_NO_FULLSCREEN,			"A full-screen mode is not available." )
			DS_X( VFW_E_IN_FULLSCREEN_MODE,		"Cannot call IVideoWindow methods while in full-screen mode." )
			DS_X( VFW_E_UNKNOWN_FILE_TYPE,		"The media type of this file is not recognized." )
			DS_X( VFW_E_CANNOT_LOAD_SOURCE_FILTER, "The source filter for this file could not be loaded." )
			DS_X( VFW_S_PARTIAL_RENDER,			"Some of the streams in this movie are in an unsupported format." )
			DS_X( VFW_E_FILE_TOO_SHORT,			"A file appeared to be incomplete." )
			DS_X( VFW_E_INVALID_FILE_VERSION,	"The version number of the file is invalid." )
			DS_X( VFW_S_SOME_DATA_IGNORED,		"The file contained some property settings that were not used." )
			DS_X( VFW_S_CONNECTIONS_DEFERRED,	"Some connections have failed and have been deferred." )
			DS_X( VFW_E_INVALID_CLSID,			"This file is corrupt: it contains an invalid class identifier." )
			DS_X( VFW_E_INVALID_MEDIA_TYPE,		"This file is corrupt: it contains an invalid media type." )
			DS_X( VFW_E_BAD_KEY,				"A registry entry is corrupt." )
			DS_X( VFW_S_NO_MORE_ITEMS,			"The end of the list has been reached." )
			DS_X( VFW_E_SAMPLE_TIME_NOT_SET,	"No time stamp has been set for this sample." )
			DS_X( VFW_S_RESOURCE_NOT_NEEDED,	"The resource specified is no longer needed." )
			DS_X( VFW_E_MEDIA_TIME_NOT_SET,		"No media time stamp has been set for this sample." )
			DS_X( VFW_E_NO_TIME_FORMAT_SET,		"No media time format has been selected." )
			DS_X( VFW_E_MONO_AUDIO_HW,			"Cannot change balance because audio device is mono only." )
			DS_X( VFW_S_MEDIA_TYPE_IGNORED,		"A connection could not be made with the media type in the persistent graph, but has been made with a negotiated media type." )
			DS_X( VFW_E_NO_DECOMPRESSOR,		"Cannot play back the video stream: no suitable decompressor could be found." )
			DS_X( VFW_E_NO_AUDIO_HARDWARE,		"Cannot play back the audio stream: no audio hardware is available, or the hardware is not responding." )
			DS_X( VFW_S_VIDEO_NOT_RENDERED,		"Cannot play back the video stream: no suitable decompressor could be found." )
			DS_X( VFW_S_AUDIO_NOT_RENDERED,		"Cannot play back the audio stream: no audio hardware is available." )
			DS_X( VFW_E_RPZA,					"Cannot play back the video stream: format 'RPZA' is not supported." )
			DS_X( VFW_S_RPZA,					"Cannot play back the video stream: format 'RPZA' is not supported." )
			DS_X( VFW_E_PROCESSOR_NOT_SUITABLE,	"ActiveMovie cannot play MPEG movies on this processor." )
			DS_X( VFW_E_UNSUPPORTED_AUDIO,		"Cannot play back the audio stream: the audio format is not supported." )
			DS_X( VFW_E_UNSUPPORTED_VIDEO,		"Cannot play back the video stream: the video format is not supported." )
			DS_X( VFW_E_MPEG_NOT_CONSTRAINED,	"ActiveMovie cannot play this video stream because it falls outside the constrained standard." )
			DS_X( VFW_E_NOT_IN_GRAPH,			"Cannot perform the requested function on an object that is not in the filter graph." )
			DS_X( VFW_S_ESTIMATED,				"The value returned had to be estimated.  It's accuracy can not be guaranteed." )
			DS_X( VFW_E_NO_TIME_FORMAT,			"Cannot get or set time related information on an object that is using a time format of TIME_FORMAT_NONE." )
			DS_X( VFW_E_READ_ONLY,				"The connection cannot be made because the stream is read only and the filter alters the data." )
			DS_X( VFW_S_RESERVED,				"This success code is reserved for internal purposes within ActiveMovie." )
			DS_X( VFW_E_BUFFER_UNDERFLOW,		"The buffer is not full enough." )
			DS_X( VFW_E_UNSUPPORTED_STREAM,		"Cannot play back the file. The format is not supported." )
			DS_X( VFW_E_NO_TRANSPORT,			"Pins cannot connect due to not supporting the same transport." )
			DS_X( VFW_S_STREAM_OFF,				"The stream has been turned off." )
			DS_X( VFW_S_CANT_CUE,				"The graph can't be cued because of lack of or corrupt data." )
			DS_X( VFW_E_BAD_VIDEOCD,			"The Video CD can't be read correctly by the device or is the data is corrupt." )
			DS_X( VFW_S_NO_STOP_TIME,			"The stop time for the sample was not set." )
			DS_X( VFW_E_OUT_OF_VIDEO_MEMORY,	"There is not enough Video Memory at this display resolution and number of colors. Reducing resolution might help." )
			DS_X( VFW_E_VP_NEGOTIATION_FAILED,	"The VideoPort connection negotiation process has failed." )
			DS_X( VFW_E_DDRAW_CAPS_NOT_SUITABLE,	"Either DirectDraw has not been installed or the Video Card capabilities are not suitable. Make sure the display is not in 16 color mode or try changing the graphics mode." )
			DS_X( VFW_E_NO_VP_HARDWARE,			"No VideoPort hardware is available, or the hardware is not responding." )
			DS_X( VFW_E_NO_CAPTURE_HARDWARE,	"No Capture hardware is available, or the hardware is not responding." )
			DS_X( VFW_E_DDRAW_VERSION_NOT_SUITABLE,	"Version number of DirectDraw not suitable. Make sure to install dx5 or higher version." )
			DS_X( VFW_E_COPYPROT_FAILED,		"Copy protection cannot be enabled. Please make sure any other copy protected content is not being shown now." )
			DS_X( VFW_S_NOPREVIEWPIN,			"There was no preview pin available, so the capture pin output is being split to provide both capture and preview." )
			DS_X( VFW_E_TIME_EXPIRED,			"This object cannot be used anymore as its time has expired." )
			DS_X( VFW_E_FRAME_STEP_UNSUPPORTED,	"Frame step is not supported on this configuration." )
			DS_X( VFW_E_PIN_ALREADY_BLOCKED_ON_THIS_THREAD,	"This thread has already blocked this output pin.  There is no need to call IPinFlowControl::Block() again." )
			DS_X( VFW_E_PIN_ALREADY_BLOCKED,	"IPinFlowControl::Block() has been called on another thread.  The current thread cannot make any assumptions about this pin's block state." )
			DS_X( VFW_E_CERTIFICATION_FAILURE,	"An operation failed due to a certification failure." )
			DS_X( VFW_E_VMR_NOT_IN_MIXER_MODE,	"The VMR has not yet created a mixing component.  That is, IVMRFilterConfig::SetNumberofStreams has not yet been called." )
			DS_X( VFW_E_VMR_NO_AP_SUPPLIED,		"The application has not yet provided the VMR filter with a valid allocator-presenter object." )
			DS_X( VFW_E_VMR_NO_DEINTERLACE_HW,	"The VMR could not find any de-interlacing hardware on the current display device." )
			DS_X( VFW_E_VMR_NO_PROCAMP_HW,		"The VMR could not find any ProcAmp hardware on the current display device." )
			DS_X( VFW_E_NO_COPP_HW,				"The current display device does not support Content Output Protection Protocol (COPP) H/W." )
			DS_X( E_PROP_SET_UNSUPPORTED,		"The Specified property set is not supported." )
			DS_X( E_PROP_ID_UNSUPPORTED,		"The specified property ID is not supported for the specified property set." )
			DS_X( VFW_E_CODECAPI_LINEAR_RANGE,	"Parameter has linear range." )
			DS_X( VFW_E_CODECAPI_ENUMERATED,	"Parameter is enumerated. It has no range." )
			DS_X( VFW_E_CODECAPI_NO_DEFAULT,	"No default value." )
			DS_X( VFW_E_CODECAPI_NO_CURRENT_VALUE,	"No current value." )

		case 0x8007001f:
			//todo use FormatMessage() to get Windows error string for non DS error code...
			return "generic window error : \"A device attached to the system is not functioning.\"";
		case E_FAIL:
			return "operation failed";
		default:
			snprintf( err_buf, sizeof(err_buf)-1, "error unknown or implemented 0x%08X", hr );
			return err_buf;
		}
	}

}
