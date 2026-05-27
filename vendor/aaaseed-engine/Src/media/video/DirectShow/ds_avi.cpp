#include "ds_avi.h"
#include "ds_util.h"
#include "system/shared/SystemUtils.h"
#include "media/video/tex_video.h"
#include "DXBaseClasses/Mtype.h"
		 
#include "obj_ui/tracker/trackers.h"

// {04FE9017-F873-410E-871E-AB91661A4EF7} 
CONST	GUID	CLSID_FFDShowVideoDecoder = { 0x04FE9017, 0xF873, 0x410E, { 0x87, 0x1E, 0xAB, 0x91, 0x66, 0x1A, 0x4E, 0xF7 } };

// {0F40E1E5-4F79-4988-B1A9-CC98794E6B55}
CONST	GUID	CLSID_FFDShowAudioDecoder = { 0x0F40E1E5, 0x4F79, 0x4988, { 0xB1, 0xA9, 0xCC, 0x98, 0x79, 0x4E, 0x6B, 0x55 } };

////{0B390488-D80F-4A68-8408-48DC199F0E97}
//CONST	GUID	CLSID_FFDshow_raw_video = { 0x0B390488, 0xD80F, 0x8408, { 0x84, 0x08, 0x48, 0xDC, 0x19, 0x9F, 0x0E, 0x97 } };

// {62D767FE-4F1B-478B-B350-8ACE9E4DB00E}
CONST	GUID	CLSID_Lav_Cuvid = { 0x62D767FE, 0x4F1B, 0x478B, { 0xB3, 0x50, 0x8A, 0xCE, 0x9E, 0x4D, 0xB0, 0x0E } };

// {B98D13E7-55DB-4385-A33D-09FD1BA26338}
CONST	GUID	CLSID_Lav_Split_Src = { 0xB98D13E7, 0x55DB, 0x4385, { 0xA3, 0x3D, 0x09, 0xFD, 0x1B, 0xA2, 0x63, 0x38 } };

// {E8E73B6B-4CB3-44A4-BE99-4F7BCB96E491}
CONST	GUID	CLSID_Lav_Audio = { 0xE8E73B6B, 0x4CB3, 0x44A4, { 0xBE, 0x99, 0x4F, 0x7B, 0xCB, 0x96, 0xE4, 0x91 } };

// {EE30215D-164F-4A92-A4EB-9D4C13390F9F}
CONST	GUID	CLSID_Lav_Video_Decoder =  { 0xEE30215D, 0x164F, 0x4A92, { 0xA4, 0xEB, 0x9D, 0x4C, 0x13, 0x39, 0x0F, 0x9F } };

//// {79376820-07D0-11CF-A24D-0020AFD79767}
//CONST	GUID	CLSID_DS_AUDIO = { 0x79376820, 0x07D0, 0x11CF, { 0xA2, 0x4D, 0x00, 0x20, 0xAF, 0xD7, 0x97, 0x67 } };

#define	MOVIE_DS_HEADER	"# MOVIE DS "
void	MOVIE_DS_PRINT_STRING( C_PCHAR_C fmt, ... )
{
	va_list	args;
	va_start( args, fmt );
	trackers::PRINT_STRING_VA( MOVIE_DS_HEADER, fmt, args );
	va_end( args );
}

#define DS_CHECK( exp, msg )			\
{										\
	HRESULT hr;							\
	if( FAILED( hr = ( exp ) ) )		\
	{									\
		MOVIE_DS_PRINT_STRING( "%s, %s in %s()", msg, ds_helper::get_error_string( hr ), __FUNCTION__ ); \
		goto exit;						\
	}									\
}

#define DS_AUDIO_CHECK( exp, msg )			\
{											\
	HRESULT hr;								\
	if( FAILED( hr = ( exp ) ) )			\
	{										\
		MOVIE_DS_PRINT_STRING( "%s, %s in %s()", msg, ds_helper::get_error_string( hr ), __FUNCTION__ ); \
		goto exit_audio;					\
	}										\
}


#if AAA_USE_GRABBER()
HRESULT	WINAPI	c_movie_ds::SampleCB( double sample_time, IMediaSample* sample )
{
	if( !_b_graph_running )
		return S_OK;

	//AM_MEDIA_TYPE*	media_type = nullptr;

	//DBG_PRINT_STRING( "c_capture_dshow::SampleCB" );
	if( sample )
	{
		if( _sample_time_last != sample_time )
		{
			_sample_time_last = sample_time;
#if	AAA_DEBUG()
			//  printf( "Sample received = %05ld  Length=%ld\r\n", framecounter_++, pSample->GetActualDataLength() );
#endif
			//todo detect change in media type
			//hr = sample->GetMediaType( &media_type );
			//if( p_media_type )
			//	{
			//	if( p_media_type->formattype == FORMAT_DvInfo )
			//		²DBG_PRINT_STRING( "Format DvInfo" );
			//	if( p_media_type->formattype == FORMAT_MPEGVideo )
			//			DBG_PRINT_STRING( "Format MPEG1VIDEOINFO" );
			//	if( p_media_type->formattype == FORMAT_MPEG2Video )
			//			DBG_PRINT_STRING( "Format MPEG2VIDEOINFO" );
			//	if( p_media_type->formattype == FORMAT_VideoInfo )
			//			DBG_PRINT_STRING( "Format VIDEOINFOHEADER" );
			//	if( p_media_type->formattype == FORMAT_VideoInfo2 )
			//			DBG_PRINT_STRING( "Format VIDEOINFOHEADER2" );
			//	if( p_media_type->formattype == FORMAT_WaveFormatEx )
			//			DBG_PRINT_STRING( "Format WAVEFORMATEX" );
			//	if( p_media_type->formattype == FORMAT_None )
			//			DBG_PRINT_STRING( "Format None" );
			//	if( p_media_type->formattype == GUID_NULL )
			//			DBG_PRINT_STRING( "Format None" );
			//	}

			BYTE*	data;
			HRESULT	hr = sample->GetPointer( &data );
			if( FAILED(hr) )
				return hr;

			//AM_MEDIA_TYPE*	p_media_type = nullptr;
			//hr = sample->GetMediaType( &p_media_type );
			//ds_helper::st_frame_info info;
			//ds_helper::get_media_info( p_media_type, info );
			////init_with_size( info.size_x, info.size_y,  );
			//set_src_pitch( info.pitch );
			got_frame( (UINT8*)data, " Movie DirectShow", _pitch, true );
		}
		else
		{
#if AAA_DEBUG()
			DBG_PRINT_STRING( "same time sample" );
#endif
		}
	}
	else
		MOVIE_DS_PRINT_STRING( "Invalid IMediaSample passed to SampleCB!" );

	return S_OK;
}


HRESULT	WINAPI	c_movie_ds::BufferCB( double sampleTimeSec, BYTE* bufferPtr, long bufferLength )
{
	return E_NOTIMPL;
}

HRESULT	WINAPI	c_movie_ds::QueryInterface( REFIID iid, void** ppvObject )
{
	// Return requested interface
	if( IID_IUnknown == iid )
	{
		*ppvObject = dynamic_cast<IUnknown*>( this );
	}
	else if( IID_ISampleGrabberCB == iid )
	{
		// Sample grabber callback object
		*ppvObject = dynamic_cast<ISampleGrabberCB*>( this );
	}
	else
	{
		// No interface for requested iid - return error.
		*ppvObject = nullptr;
		return E_NOINTERFACE;
	}

	// inc reference count
	AddRef();
	return S_OK;
}

ULONG	WINAPI	c_movie_ds::AddRef()
{
	return( _ref_count++ );
}

ULONG	WINAPI	c_movie_ds::Release()
{
	if( _ref_count > 0 )
		--_ref_count;
	return _ref_count;
}

bool	c_movie_ds::set_callback( bool b_set )
{
	_sample_time_last = -42.;
	return ds_helper::set_callback( this, _sample_grabber, b_set );
}
#endif

c_movie_ds::c_movie_ds( c_image_flux_buffer* buf )
	:c_movie_player			(	buf, true	)
	,_ref_count				(	0			)
	,_pitch					(	0			)
	,_b_graph_running		(	false		)
	,_b_audio_filter		(	true		)
	,_sample_grabber		(	nullptr		)
	,_p_sample_grabber		(	nullptr		)
	,_p_graph_builder		(	nullptr		)	// GraphBuilder
	,_p_ds_device			(	nullptr		)	// DirectSound Device
	,_p_graph_builder_2		(	nullptr		)
	,_p_media_control		(	nullptr		)	// Media Control
	,_p_media_seeking		(	nullptr		)	// Media Seeking
	,_p_media_event			(	nullptr		)	// Media Event
	,_p_media_position		(	nullptr		)	// Media Position
	,_p_wma_reader			(	nullptr		)	// interface to the WM ASF Reader
	,_p_video_source		(	nullptr		)
	,_p_source_filter		(	nullptr		)	// Source Filter
	,_p_source_pin_out_0	(	nullptr		)	// #0 Source Filter Output Pin ( force to be audio ) 
	,_p_source_pin_out_1	(	nullptr		)	// #1 Source Filter Output Pin ( force to be video ) 
	,_p_decoder_filter		(	nullptr		)	// Decoder Filter
	,_p_null_renderer		(	nullptr		)	// Video Renderer
	,_p_dec_pin_out			(	nullptr		)	// Decoder Filter Output Pin
	,_p_basic_audio			(	nullptr		)	// Audio control
#if	AAA_DEBUG()
	,_register_graph		(	0			)
#endif
{
	_b_is_callback = true;		// DirectShow use Callbacks

	// Initialize COM
	//c_COM::init();
}

c_movie_ds::~c_movie_ds()
{
	// cleanup DirectShow
	close();
	//lib_close();
	// Release COM
	//c_COM::close();
}

void	c_movie_ds::play()
{
	HRESULT hr = S_OK;

	if( !_p_graph_builder )
		return;

	// Get the graph's media control, event & position interfaces
	check_media_control();
	check_media_seeking();
	check_media_event();

	// check to see if callback is set
	if( !_b_callback_set )
	{
		_b_callback_set = set_callback( true );
		if( !_b_callback_set )
		{
			// could not set callback, close capture
			MOVIE_DS_PRINT_STRING( "Could not set DirectShow Callback" );
			//	close();
			//	return ERR_ANY;
			return;
		}
	}

	// Start the graph running;
	if( _p_media_control )
	{
		if( FAILED( hr = _p_media_control->Run() ) )
		{
			MOVIE_DS_PRINT_STRING( "Unable to RUN the DirectShow graph!  ERR=0x%x", hr );
		}
		else
		{
			_b_graph_running = true;
			set_playing( true );
			set_paused( false );
		}
	}
}

void	c_movie_ds::check_media_control()
{
	if( !_p_media_control && _p_graph_builder )
	{
		_p_graph_builder->QueryInterface( &_p_media_control );
	}
}

void	c_movie_ds::check_media_seeking()
{
	if( _p_graph_builder )
	{
		if( !_p_media_seeking )
		{
			_p_graph_builder->QueryInterface( &_p_media_seeking );
		}
		if( !_p_media_position )
		{
			_p_graph_builder->QueryInterface( &_p_media_position );
		}
	}
}

void	c_movie_ds::check_media_event()
{
	if( !_p_media_event && _p_graph_builder )
	{
		_p_graph_builder->QueryInterface( &_p_media_event );
	}
}

void	c_movie_ds::pause()
{
	if( is_paused() )
		return;
	// Get the graph's media control, event & position interfaces
	check_media_control();

	// if p_media_control_ exist
	if( _p_media_control )
	{
		HRESULT	hr = S_FALSE;
		// Pause graph;
		if( FAILED( hr = _p_media_control->Pause() ) )
		{
			MOVIE_DS_PRINT_STRING( "Unable to PAUSE the DirectShow graph!  ERR=0x%x", hr );
		}
		else
		{
			set_playing( false );
			set_paused( true );
		}
	}
}

void	c_movie_ds::stop()
{
	if( !_b_graph_running )
		return;
	// Get the graph's media control, event & position interfaces
	check_media_control();

	if( _p_media_control )
	{
		//	this way the callback will do nothing
		_b_graph_running = false;
		HRESULT	hr = S_FALSE;
		// Start the graph running;
		if( FAILED( hr = _p_media_control->Stop() ) )
		{
			MOVIE_DS_PRINT_STRING( "Unable to STOP the DirectShow graph!  ERR=0x%x", hr );
		}
		//	this way the callback will do nothing
		_b_graph_running = false;
		set_playing( false );
		set_paused( false );
	}
}

REAL	c_movie_ds::get_position()
{
	if( _p_media_seeking )
	{
		LONGLONG	p_pos;
		_p_media_seeking->GetCurrentPosition( &p_pos );
		REAL		time = p_pos / 10000000.0f;
		//	time /= _rate;
		//DBG_PRINT_STRING( "time %f", time );
		_time_video = time;
		return time;
	}
	return 0.0f;
}

void	c_movie_ds::set_position_low( REAL time )
{
	_time_video = time;
	// convert to reference time, 100 nanoseconds units
	if( _p_media_position )
	{
		const auto hr = _p_media_position->put_CurrentPosition( time );
	}
	//if( _p_media_seeking )
	//{
		//LONGLONG	p_pos;
		//p_pos = time * 10000000;
		//_p_media_seeking->SetPositions( &p_pos, AM_SEEKING_AbsolutePositioning, 0, AM_SEEKING_NoPositioning );
		// make sure the graph compute current position
		//play();
		//OAFilterState	pfs;
		//if( _p_media_control )
		//{
		//	_p_media_control->GetState( 10, &pfs );
		//}
		//stop();
	//}
}

void	c_movie_ds::set_position( REAL time )
{
	if( ABS( _time_video - time ) > .0f )
	{
		set_position_low( time );
	}
}

DOUBLE	c_movie_ds::set_rate( DOUBLE rate )
{
	DOUBLE t = aaa::time::get_real_time();
	if( ABS(_rate_change_time - t) > .3 )	//we force a delay between rate change
	{										//because in 2024 August Alain Longuet had troubles changing speed on his machine with a filtered midi input
		if( rate < .00000001 )
			rate = .00000001;
		if( rate != _rate_last )
		{
			if( _p_media_seeking )	//&& rate >= 0.0f )
			{
				_p_media_seeking->SetRate( rate );
				_rate_change_time = t;
				//_p_media_seeking->GetRate(&_rate_last);
				_rate_last = rate;
				_rate = rate;
				//_p_media_seeking->GetRate( &rate );
				//_rate = rate;			
			}
		}
	}
	return _rate;
}

#define	VOLUME_FULL		0L
#define	VOLUME_SILENCE	-10000L
bool	c_movie_ds::set_volume( REAL volume )
{
	_volume = volume;
	if( _b_audio_filter && _p_basic_audio )
	{
		LONG lVolume = LONG( ( REAL(1) - volume ) * VOLUME_SILENCE );
		if( FAILED( _p_basic_audio->put_Volume( lVolume ) ) )
		{
//			MOVIE_DS_PRINT_STRING( "Could not set volume" );
			return false;
		}
	}
	return true;
}

REAL	c_movie_ds::get_volume()
{
	if( _b_audio_filter && _p_basic_audio )
	{
		LONG	lVolume;
		HRESULT	hr;

		// Read current volume
		hr = _p_basic_audio->get_Volume( &lVolume );
		if( hr == E_NOTIMPL || FAILED( hr ) )
		{
			return -1.0;	// return -1.0, to tell aaaseed that there's no volume
		}
		REAL volume = ( REAL )( lVolume ) / ( REAL ) VOLUME_SILENCE + REAL(1);
		_volume = volume;
		return volume;
	}
	else
		return -1.0;	// return -1.0, to tell aaaseed that there's no volume
}

#define BALANCE_RANGE	10000L
REAL	c_movie_ds::get_pan()
{
	if( _b_audio_filter && _p_basic_audio )
	{
		LONG	lBalance;
		HRESULT hr;

		// Read current balance
		hr = _p_basic_audio->get_Balance( &lBalance );
		if( hr == E_NOTIMPL || FAILED( hr ) )
		{
			return -1.0;	// return -1.0, to tell aaaseed that there's no panning
		}
		REAL pan =  ( REAL )( lBalance ) / ( REAL ) BALANCE_RANGE;
		_pan = pan;
		return pan;
	}
	else
		return -10.0;	// return -1.0, to tell aaaseed that there's no panning
}

bool	c_movie_ds::set_pan( REAL balance )
{
//	LONG	lVolume;
	// convert to reference time, 100 nanoseconds units
	_pan = balance;
	if( _b_audio_filter && _p_basic_audio )
	{
		LONG	lBalance;
		lBalance = LONG( balance * BALANCE_RANGE );
		HRESULT	hr = _p_basic_audio->put_Balance( lBalance );
		if( FAILED( hr ) )
		{
//			MOVIE_DS_PRINT_STRING( "Could not set balance" );
			return false;
		}
	}
	return true;
}

INT32	c_movie_ds::get_frame_index_cur()
{
	if( _time_video > .000000001f )
		return 1;
	return 0;
}

//HRESULT c_ds_avi::connect_WMV_file( CHAR* filename )
//{
//    HRESULT hr = S_OK;
//
//    // This requires DirectX Media Objects, meaning WM ASF Reader is used instead of the standard File Source Filter
//
//    hr = AddFilterByCLSID( p_graph_builder_, CLSID_WMAsfReader, filename, &p_source_filter_ );
//
//    if( FAILED( hr ) ) {
//        sprintf( "Unable to add [WM ASF Reader]!  ERR=0x%x" ), hr );
//        return hr;
//    }
//
//    // Get the Interface to IFileSource so we can load the file
//    hr = p_source_filter_->QueryInterface( IID_IFileSourceFilter, reinterpret_cast<void**>&p_wma_reader_ );
//    if( !FAILED( hr ) && ( p_wma_reader_ != nullptr ) )
//		{
//        hr = p_wma_reader_->Load( filename, nullptr );
//
//        if( FAILED( hr ) )
//			{
//            Msg( TEXT( "Unable to load file %s!  hr=0x%x" ), wFileName, hr );
//            return hr;
//			}
//		}
//
//    // Find the source's Raw Video #1 output pin and the video renderer's input pin
//	  if( FAILED( hr = p_source_filter_->FindPin( L"Raw Video 0", &p_source_pin_out_1_ ) ) )
//		{
//		if( FAILED( hr = p_source_filter_->FindPin( L"Raw Video 1", &p_source_pin_out_1_ ) ) )
//			{
//			Msg( TEXT( "ConnectWMVFile() unable to find Raw Video Pin #1!  ERR=0x%x" ), hr );
//			return hr;
//			}
//		}
//
//    // Find the source's Raw Audio #0 output pin and the audio renderer's input pin
//    if( FAILED( hr = p_source_filter_->FindPin( L"Raw Audio 0", &p_source_pin_out_0_ ) ) ) {
//    if( FAILED( hr = p_source_filter_->FindPin( L"Raw Audio 1", &p_source_pin_out_0_ ) ) ) {
//        p_source_pin_out_0_ = nullptr;
//        hr = S_OK;
//    }
//    }
//
//    return hr;
//}


//HRESULT c_ds_avi::connect_MPG_file( CHAR* filename )
//{
//    HRESULT hr = S_OK;
//
//    // Add the source filter to the graph.
//    hr = p_graph_builder_->AddSourceFilter ( filename, L"FileSource", &p_source_filter_ );
//
//    // If the media file was not found, inform the user.
//    if( hr == VFW_E_NOT_FOUND )
//    {
//        Msg( TEXT( "Could not add source filter to graph!  ( hr==VFW_E_NOT_FOUND )\r\n\r\n" )
//            TEXT( "Media file: %s could not be found\r\n" ), filename );
//        return hr;
//    }
//    else if( FAILED( hr ) )
//    {
//        Msg( TEXT( "Could not add source filter to graph!  ERR=0x%x" ), hr );
//        return hr;
//    }
//
//    if( FAILED( hr = p_source_filter_->FindPin( L"Output", &p_source_pin_out_0_ ) ) )
//    {
//        Msg( TEXT( "Could not find SourcePin Output Pin!  ERR=0x%x" ), hr );
//        return hr;
//    }
//
//    return hr;
//}

//HRESULT c_ds_avi::connect_AVI_file( CHAR* filename )
//{
//    HRESULT hr = S_OK;
//
//    // Add the source filter to the graph.
//    hr = p_graph_builder_->AddSourceFilter ( filename, L"FileSource", &p_source_filter_ );
//
//    // If the media file was not found, inform the user.
//    if( hr == VFW_E_NOT_FOUND )
//    {
//        Msg( TEXT( "Could not add source filter to graph!  ( ERR==VFW_E_NOT_FOUND )\r\n\r\n" )
//            TEXT( "Media file: %s could not be found\r\n" ), filename );
//        return hr;
//    }
//    else if( FAILED( hr ) )
//    {
//        Msg( TEXT( "Could not add source filter to graph!  hr=0x%x" ), hr );
//        return hr;
//    }
//
//    if( FAILED( hr = p_source_filter_->FindPin( L"Output", &p_source_pin_out_0_ ) ) )
//    {
//        Msg( TEXT( "Could not find Output Pin!  ERR=0x%x" ), hr );
//        return hr;
//    }
//    return hr;
//}


////-----------------------------------------------------------------------------
//// LoadVideoFile: Depending on what type of media files we want to load, we 
////                will initialize the appropriate graph.  WMV ( Requires a 
////                different source filter to operate properly )
////-----------------------------------------------------------------------------
//HRESULT c_ds_avi::load_video_file( CHAR* filename )
//{
//    // Determine the file to load based on DirectX Media path ( from SDK )
//	// Use a helper function included in DXUtils.cpp
//    CHAR *wmv_ext[] = { ".wmv", ".wma", ".asf", nullptr };
//    CHAR *mpg_ext[] = { ".mpg", "mpeg", ".m2v", nullptr };
//    
//	LONG length = lstrlen( filename )-4;
//
//    HRESULT hr = S_OK;
//
//    if( !lstrcmpi( &filename[length], wmv_ext[0] ) || !lstrcmpi( &filename[length], wmv_ext[1] ) || !lstrcmpi( filename[length], wmv_ext[2] ) ) 
//		{
//        hr = connect_WMV( wFileName );
//        if( FAILED( hr ) )
//			{
//            return hr;
//			}
//		}
//	else if( !lstrcmpi( &filename[length], mpg_ext[0] ) || !lstrcmpi( &filename[length], mpg_ext[1] ) || !lstrcmpi( &filename[length], mpg_ext[2] ) )
//		{
//        hr = connect_MPG( wFileName );
//        if( FAILED( hr ) )
//			{
//            return hr;
//			}
//	    }
//	else
//		{
//        // This is all other standard media types that do not use the Microsoft DirectX Media Objects
//        hr = connect_AVI( wFileName );
//        if( FAILED( hr ) )
//			{
//            return hr;
//			}
//    }
//
//    IPin*			p_filter_pin_in;    // Texture Renderer Input Pin
////    IFilterGraph2	p_filter_graph_2; // Filter Graph2 Interface
//
//    // If no audio component is desired, directly connect the two video pins instead of allowing the Filter Graph Manager to render all pins.
//
//    // Find the source's output pin and the renderer's input pin
//    if( FAILED( hr = p_graph_builder_->FindPin( L"In", &p_filter_pin_in ) ) )
//    {
//        sprintf( "Could not find input pin!  ERR=0x%x", hr );
//        return hr;
//    }
//
//    // Connect these two filters
//    if( FAILED( hr = p_graph_builder_->Connect( p_source_pin_out_0_, p_filter_pin_in ) ) )
//    {
//        sprintf( "Could not connect pins!  ERR=0x%x", hr );
//        return hr;
//    }
//
//    if( p_source_pin_out_0_ )
//		{
//        if( FAILED( hr = p_graph_builder_->Render( p_source_pin_out_0_ ) ) )
//			{
//            sprintf( "Connecting RenderPin output failed! ERR=0x%x\nCheck to see if Video Decoder filters are installed.", hr );
//            return hr;
//			}
//		}
//
//    return S_OK;
//}
//
//
//

//-----------------------------------------------------------------------------
// CleanupDShow
//-----------------------------------------------------------------------------
//void	c_movie_ds::lib_close()
//{
////#if	AAA_DEBUG()
////	// Pull graph from Running Object Table ( Debug )
////	ds_helper::remove_from_object_table( &_register_graph );
////#endif
//
//	//// Shut down the graph
//	//destroy_graph();
//}

//void	c_movie_ds::destroy_sub_graph( CComPtr<IGraphBuilder> pGraph, CComPtr<IBaseFilter> pFilt )
//{
//	CComPtr<IEnumPins>	pEnum;
//
//	if( !pFilt )
//		return;
//
//	if( SUCCEEDED( pFilt->EnumPins( &pEnum ) ) )
//	{
//		CComPtr<IPin> pPin;
//
//		pEnum->Reset();
//
//		for(;;)
//		{
//			HRESULT hr = pEnum->Next( 1, &pPin, 0 );
//			if( hr == VFW_E_ENUM_OUT_OF_SYNC )
//			{
//				hr = pEnum->Reset();
//				if( SUCCEEDED(hr) )
//					continue;
//			}
//			if( hr != S_OK )
//				break;
//
//			PIN_DIRECTION dir;
//
//			SUCCEEDED( pPin->QueryDirection( &dir ) );
//
//			if( dir == PINDIR_OUTPUT)
//			{
//				CComPtr<IPin>	pPin2;
//
//				if( SUCCEEDED( pPin->ConnectedTo( &pPin2 ) ) )
//				{
//					PIN_INFO pi;
//
//					if( SUCCEEDED( pPin2->QueryPinInfo( &pi ) ) )
//					{
//						destroy_sub_graph( pGraph, pi.pFilter );
//						SUCCEEDED( pGraph->RemoveFilter( pi.pFilter ) );
//						SAFE_RELEASE( pi.pFilter );
//						//						pi.pFilter->Release();
//					}
//					pPin2 = nullptr;
//					//					pPin2->Release();
//				}
//			}
//			pPin = nullptr;
//			//			pPin->Release();
//		}
//		pEnum = nullptr;
//		//		pEnum->Release();
//	}
//}

void	c_movie_ds::destroy_graph()
{
	// Shut down the graph
	// stop graph if running
	stop();
#if	AAA_DEBUG()
	// Pull graph from Running Object Table ( Debug )
	ds_helper::remove_from_object_table( &_register_graph );
	_register_graph		= 0;
#endif
	if( _p_graph_builder )
	{
		// destroy downstreams
//		destroy_sub_graph( _p_graph_builder, _p_video_source );
		ds_helper::destroy_graph( *_p_graph_builder );
		_p_graph_builder	= nullptr;
	}
	_p_wma_reader		= nullptr;
	_p_source_pin_out_1 = nullptr;
	_p_source_pin_out_0 = nullptr;
	_p_source_filter	= nullptr;
	_p_decoder_filter	= nullptr;
	_p_null_renderer	= nullptr;
	_p_media_control	= nullptr;
	_p_media_event		= nullptr;
	_p_media_seeking	= nullptr;
	_p_media_position	= nullptr;
	_p_basic_audio		= nullptr;
	_p_dec_pin_out		= nullptr;
	_p_ds_device		= nullptr;
	_sample_grabber		= nullptr;
	_p_sample_grabber	= nullptr;
	_p_video_source		= nullptr;
	_p_graph_builder_2	= nullptr;
}

void	c_movie_ds::close_specific()
{
	set_callback( false );
	destroy_graph();
	_b_callback_set = false;
}

void	c_movie_ds::get_info()
{
	check_media_seeking();
	if( _p_media_seeking )
	{
//		LONGLONG	duration;
		LONGLONG	duration_seconds;

		_p_media_seeking->GetDuration( &duration_seconds );
/*
		_p_media_seeking->ConvertTimeFormat(
			&duration,
			&TIME_FORMAT_MEDIA_TIME,
			duration_seconds,
			&TIME_FORMAT_BYTE
			);
*/
		_duration = duration_seconds / 10000000.0f;
	}
}

AAA_ERR	c_movie_ds::open_specific()
{
	set_audio_use( get_tex_video()->is_audio() );

	AAA_ERR ret = open_low();
	if( ERR(ret) )
	{
		// problem during initialization
		ERR_PRINT_STRING( "DS VIDEO : Error opening file %s", get_filename() );
		return ret;
	}

	if( !get_media_type() )
	{
		//	sound file have an unsupported video format this should be done in a cleaner way 
		C_PCHAR_C filename = get_filename();
		if( !is_sound_file_from_name(filename) )	
		{
			// media type is not supported by aaaseed, close capture
			ERR_PRINT_STRING( "DS VIDEO : DirectShow format is not supported %s", filename );
			return ERR_UNIMPLEMENTED_YET;
		}
	}

	enum_filters();

	check_media_control();
	check_media_seeking();
	check_media_event();
	get_info();

	// Set Volume, Panning, Rate and Position to last known values
	set_volume( _volume );
	set_pan(	_pan );
	set_rate(	_rate );

	set_position_low( MIN( _time_video, _duration ) );
		
	//	play();
	//	OAFilterState	pfs;

	//	if( _p_media_control )
	//	{
	//		_p_media_control->GetState( INFINITE, &pfs );
	//	}

	//	set_position_low( MIN( _time_video, _duration ) );
	//	stop();
	//	set_position_low( MIN( _time_video, _duration ) );
	return AAA_OK;
}

bool	c_movie_ds::open_wmv( C_PCHAR_C filename )
{
	//	IBaseFilter*	pVideoSource = nullptr;
	// This requires DirectX Media Objects, meaning WM ASF Reader is used instead of the standard File Source Filter
#if DS_MAA
	DS_CHECK( _p_source_filter.CoCreateInstance( CLSID_WMAsfReader ), "Couldn't create WMV reader." );
#else
	DS_CHECK( CoCreateInstance( CLSID_WMAsfReader, nullptr, CLSCTX_INPROC_SERVER, IID_IBaseFilter, reinterpret_cast<void**>(&_p_source_filter) ), "Couldn't create WMV reader." );
#endif

	if( _p_graph_builder )
	{
//		DIRECTSHOW_CHECK( p_graph_builder_->AddFilter( p_source_filter_, L"ASF Reader" ), "Couldn't add WMV File Reader filter to graph." );
		DS_CHECK( _p_graph_builder->AddFilter( _p_source_filter, L"WMR Reader" ), "Couldn't add WMV File Reader filter to graph." );
	}
	else
	{
		MOVIE_DS_PRINT_STRING( "Graph Builder pointer is not valid" );
		return false;
	}

	// Get the Interface to IFileSource so we can load the file
	if( _p_source_filter )
	{
		DS_CHECK( _p_source_filter->QueryInterface( IID_IFileSourceFilter, reinterpret_cast<void**>(&_p_wma_reader) ), "Couldn't get interface to file source." );
	}
	else
	{
		MOVIE_DS_PRINT_STRING( "Source Filter pointer is not valid." );
		return false;
	}

	if( IS_NOT_NULL( _p_wma_reader ) )
	{
		WCHAR	sourceFilterName[MAX_PATH];
		ds_helper::DXUtil_ConvertAnsiStringToWide( sourceFilterName, filename, MAX_PATH );

		DS_CHECK( _p_wma_reader->Load( sourceFilterName, nullptr ), "Couldn't load WMV file." );
	}
	else
	{
		MOVIE_DS_PRINT_STRING( "WMA Reader pointer is not valid." );
		return false;
	}

	// Find the source's Raw Video #1 output pin and the video renderer's input pin
	if( FAILED( _p_source_filter->FindPin( L"Raw Video 0", &_p_source_pin_out_1 ) ) )
	{
		DS_CHECK( _p_source_filter->FindPin( L"Raw Video 1", &_p_source_pin_out_1 ) , "Source Filter unable to find Raw Video Pin #1!" )
	}

	if( _p_sample_grabber )
	{
//		DIRECTSHOW_CHECK( ds_auto_connect_filters( p_source_filter_, 2, p_sample_grabber_, 1, p_graph_builder_ ), "Couldn't find a matching decoder filter for stream 0x00. Check if the required AVI codec is installed." );
		if( FAILED( ds_helper::auto_connect_filters( _p_source_filter, 2, _p_sample_grabber, 1, _p_graph_builder ) ) )
			DS_CHECK( ds_helper::auto_connect_filters( _p_source_filter, 1, _p_sample_grabber, 1, _p_graph_builder ), "Couldn't find a matching decoder filter for stream 0x00. Check if the required AVI codec is installed." );
		// get output pin of the sample grabber, so we can know the format of the sample in the Sample Grabber callback
		DS_CHECK( ds_helper::get_pin( _p_sample_grabber, PINDIR_OUTPUT, 1, _p_dec_pin_out ), " Could not Output pin of the Sample Grabber." );
	}
	else
	{
		MOVIE_DS_PRINT_STRING( "Sample Grabber pointer is not valid." );
		return false;
	}

	if( _p_null_renderer )
	{
		DS_CHECK( ds_helper::auto_connect_filters( _p_sample_grabber, 1, _p_null_renderer, 1, _p_graph_builder ), "Couldn't connect Sample Grabber to the Video Renderer." );
	}
	else
	{
		MOVIE_DS_PRINT_STRING( "Video Renderer pointer is not valid." );
		return false;
	}

//	pVideoSource->SetSyncSource( nullptr );
//	pStreamSplitter->SetSyncSource( nullptr );
//	pVideoRenderer_->SetSyncSource( nullptr );
//	pSampleGrabber_->SetSyncSource( nullptr );

	return true;

exit:
	//todoqqq release interfaces, because could not build graph
	return false;
}

bool		c_movie_ds::open_mpg( C_PCHAR_C filename )
{
	//	LPWSTR sourceFilterName;
	//	sourceFilterName = ( LPWSTR ) CoTaskMemAlloc( sizeof( wchar_t ) * MAX_PATH );
	//	swprintf( sourceFilterName, sizeof( wchar_t ) * MAX_PATH,L"" );
	//	swprintf( sourceFilterName, sizeof( wchar_t ) * MAX_PATH,_bstr_t( filename ) );

	//	// Add the source filter to the graph.
	//	hr = p_graph_builder_->AddSourceFilter( sourceFilterName, L"FileSource", &p_source_filter_ );
	//	// If the media file was not found, inform the user.
	//	if( hr == VFW_E_NOT_FOUND )
	//		{
	//		ERR_PRINT_STRING( "Media file: %s could not be found\r", filename );
	//		return ERR_ANY;
	//		}
	//	else if( FAILED( hr ) )
	//	{
	//		printf( "Could not add source filter to graph!  hr=0x%x", hr );
	//		return ERR_ANY;
	//	}

	//	if( FAILED( hr = p_source_filter_->FindPin( L"Output", &p_source_pin_out_0_ ) ) )
	//		{
	//		printf( "Could not find SourcePin Output Pin!  hr=0x%x", hr );
	//		return ERR_ANY;
	//		}
	WCHAR						sourceFilterName[MAX_PATH];
	//IBaseFilter*				pVideoSource = nullptr;
	//IBaseFilter*				p_mpeg_decoder = nullptr;
	//IFileSourceFilter*		pFileSource = nullptr;
	//IBaseFilter*				pStreamSplitter = nullptr; // used in conjunction with the Async File Source filter
	//IPin*						pStreamPin00 = nullptr;
	CComPtr<IBaseFilter>		pVideoSource = nullptr;
	CComPtr<IBaseFilter>		p_mpeg_decoder = nullptr;
	CComPtr<IFileSourceFilter>	pFileSource = nullptr;
	CComPtr<IBaseFilter>		pStreamSplitter = nullptr; // used in conjunction with the Async File Source filter
	CComPtr<IPin>				pStreamPin00 = nullptr;

	ds_helper::DXUtil_ConvertAnsiStringToWide( sourceFilterName, filename, MAX_PATH );

#if DS_MAA
	DS_CHECK( pVideoSource.CoCreateInstance( CLSID_AsyncReader ),		"Couldn't create Video Source." );
	DS_CHECK( pStreamSplitter.CoCreateInstance( CLSID_MPEG1Splitter ),	"Couldn't create MPEG Splitter." );
	DS_CHECK( p_mpeg_decoder.CoCreateInstance( CLSID_CMpegVideoCodec ),	"Couldn't create MPEG Decoder." );
#else
	DS_CHECK( CoCreateInstance( CLSID_AsyncReader, nullptr, CLSCTX_INPROC_SERVER, IID_IBaseFilter, reinterpret_cast<void**>(&pVideoSource) ), "Couldn't create Video Source." );
	DS_CHECK( CoCreateInstance( CLSID_MPEG1Splitter, nullptr, CLSCTX_INPROC_SERVER, IID_IBaseFilter, reinterpret_cast<void**>(&pStreamSplitter) ), "Couldn't create MPEG Splitter." );
	DS_CHECK( CoCreateInstance( CLSID_CMpegVideoCodec, nullptr, CLSCTX_INPROC_SERVER, IID_IBaseFilter, reinterpret_cast<void**>(&p_mpeg_decoder) ), "Couldn't create MPEG Decoder." );
#endif

	// Add the source filter to the graph.
	DS_CHECK( _p_graph_builder->AddSourceFilter( sourceFilterName, L"FileSource", &_p_source_filter ), "Couldn't add Source Filter to graph." );

	if( pVideoSource )
	{
		DS_CHECK( pVideoSource->QueryInterface( IID_IFileSourceFilter, reinterpret_cast<void**>(&pFileSource) ), "Couldn't get File Source filter." );
	}
	else
	{
		MOVIE_DS_PRINT_STRING( "Video Source pointer is not valid." );
		return false;
	}

	if( pFileSource )
	{
		DS_CHECK( pFileSource->Load( sourceFilterName, nullptr ), "Video Input file not found." );
	}
	else
	{
		MOVIE_DS_PRINT_STRING( "File Source pointer is not valid" );
		return false;
	}

	DS_CHECK( _p_graph_builder->AddFilter( pVideoSource, L"File Reader" ), "Couldn't add File Reader filter to graph." );
	DS_CHECK( _p_graph_builder->AddFilter( pStreamSplitter, L"MPEG Splitter" ), "Couldn't add MPEG Splitter to graph." );
	DS_CHECK( _p_graph_builder->AddFilter( p_mpeg_decoder, L"MPEG Decoder" ), "Couldn't add MPEG decoder." );

	if( pStreamSplitter )
	{
		DS_CHECK( ds_helper::connect_filters( pVideoSource, 1, pStreamSplitter, 1 ), "Couldn't connect Async File Source to MPEG Splitter!" );
	}
	else
	{
		MOVIE_DS_PRINT_STRING( "Stream Filter pointer is not valid." );
		return false;
	}

	if( p_mpeg_decoder )
	{
		DS_CHECK( ds_helper::connect_filters( pStreamSplitter, 2, p_mpeg_decoder, 1 ), "Couldn't connect MPEG Splitter to MPEG Decoder!" );
	}
	else
	{
		MOVIE_DS_PRINT_STRING( "Stream Filter pointer is not valid." );
		return false;
	}

	DS_CHECK( ds_helper::connect_filters( p_mpeg_decoder, 1, _p_sample_grabber, 1 ), "Couldn't find a matching decoder filter for stream 0x00. Check if the required MPEG codec is installed." );

	// get output pin of the sample grabber, so we can know the format of the sample in the Sample Grabber callback
	DS_CHECK( ds_helper::get_pin( _p_sample_grabber, PINDIR_OUTPUT, 1, _p_dec_pin_out ), " Could not Output pin of the Sample Grabber." );

	DS_CHECK( ds_helper::auto_connect_filters( _p_sample_grabber, 1, _p_null_renderer, 1, _p_graph_builder ), "Couldn't connect Sample Grabber to the Video Renderer." );

//maa	pVideoSource->SetSyncSource( nullptr );
//maa	pStreamSplitter->SetSyncSource( nullptr );
//maa	_p_null_renderer->SetSyncSource( nullptr );
//maa	_p_sample_grabber->SetSyncSource( nullptr );

	return true;
//	return false;
exit:
	//todoqqq release interfaces, because could not build graph

	return false;
}

bool		c_movie_ds::open_avi( C_PCHAR_C filename )
{
	WCHAR						sourceFilterName[MAX_PATH];
	CComPtr<IFileSourceFilter>	pFileSource = nullptr;
	CComPtr<IBaseFilter>		pStreamSplitter = nullptr; // used in conjunction with the Async File Source filter
	CComPtr<IPin>				pStreamPin00 = nullptr;

	_p_video_source = nullptr;
	// todoqqq, if filename is more than 256, crash!! but should not happen since max_path is 256
	ds_helper::DXUtil_ConvertAnsiStringToWide( sourceFilterName, filename, MAX_PATH );

#if DS_MAA
	DS_CHECK( _p_video_source.CoCreateInstance( CLSID_AsyncReader ), "Couldn't create Video Source." );
	DS_CHECK( pStreamSplitter.CoCreateInstance( CLSID_AviSplitter ), "Couldn't create AVI Splitter." );
#else
	DS_CHECK( CoCreateInstance( CLSID_AsyncReader, nullptr, CLSCTX_INPROC_SERVER, IID_IBaseFilter, reinterpret_cast<void**>(&_p_video_source) ), "Couldn't create Video Source." );
	DS_CHECK( CoCreateInstance( CLSID_AviSplitter, nullptr, CLSCTX_INPROC_SERVER, IID_IBaseFilter, reinterpret_cast<void**>(&pStreamSplitter) ), "Couldn't create AVI Splitter." );
#endif
//	// Add the source filter to the graph.
//	DIRECTSHOW_CHECK( p_graph_builder_->AddSourceFilter( sourceFilterName, L"FileSource", &p_source_filter_ ), "Couldn't add Source Filter to graph." );

	if( _p_video_source )
	{
		DS_CHECK( _p_video_source->QueryInterface( IID_IFileSourceFilter, reinterpret_cast<void**>(&pFileSource) ), "Couldn't get File Source filter." );
	}
	else
	{
		MOVIE_DS_PRINT_STRING( "Video Source pointer is not valid." );
		return false;
	}

	if( pFileSource )
	{
		DS_CHECK( pFileSource->Load( sourceFilterName, nullptr ), "Video Input file not found." );
	}
	else
	{
		MOVIE_DS_PRINT_STRING( "File Source pointer is not valid" );
		return false;
	}

	DS_CHECK( _p_graph_builder->AddFilter( _p_video_source, L"File Reader" ), "Couldn't add File Reader filter to graph." );
	DS_CHECK( _p_graph_builder->AddFilter( pStreamSplitter, L"AVI Splitter" ), "Couldn't add AVI Splitter to graph." );

	if( pStreamSplitter )
	{
		DS_CHECK( ds_helper::connect_filters( _p_video_source, 1, pStreamSplitter, 1 ), "Couldn't connect Async File Source to AVI Splitter!" );
	}
	else
	{
		MOVIE_DS_PRINT_STRING( "Stream Filter pointer is not valid." );
		return false;
	}

	DS_CHECK( ds_helper::get_pin( pStreamSplitter, PINDIR_OUTPUT, 1, pStreamPin00 ), "Couldn't get output pin of AVI Splitter!" );

	if( IS_NULL( pStreamPin00 ) || !ds_helper::pin_can_deliver_video( pStreamPin00 ) )
	{
		MOVIE_DS_PRINT_STRING( "AVI file format error. Substream 0x00 does not deliver MEDIATYPE_Video." );
		return false;
	}

//	determine_compressor( pStreamPin00 );
	DS_CHECK( ds_helper::auto_connect_filters( pStreamSplitter, 1, _p_sample_grabber, 1, _p_graph_builder ), "Couldn't find a matching decoder filter for stream 0x00. Check if the required AVI codec is installed." );

//	// get output pin of the sample grabber, so we can know the format of the sample in the Sample Grabber callback
	DS_CHECK( ds_helper::get_pin( _p_sample_grabber, PINDIR_OUTPUT, 1, _p_dec_pin_out ), " Could not Output pin of the Sample Grabber." );

	DS_CHECK( ds_helper::auto_connect_filters( _p_sample_grabber, 1, _p_null_renderer, 1, _p_graph_builder ), "Couldn't connect Sample Grabber to the Video Renderer." );

	if( is_audio_use() )
	{
#if DS_MAA
	DS_CHECK( _p_ds_device.CoCreateInstance( CLSID_DSoundRender ), "Couldn't create DirectSound Renderer." );
#else
	DS_CHECK( CoCreateInstance( CLSID_DSoundRender, nullptr, CLSCTX_INPROC_SERVER, IID_IBaseFilter, reinterpret_cast<void**>(&_p_ds_device) ), "Couldn't create DirectSound Renderer." );
#endif
		if( !_p_ds_device )
		{
			MOVIE_DS_PRINT_STRING( "DirectSound Render pointer is not valid." );
			return false;
		}
	//	DIRECTSHOW_CHECK( CoCreateInstance( CLSID_AviSplitter, nullptr, CLSCTX_INPROC_SERVER, IID_IBaseFilter, ( void** )&( pStreamSplitter ) ), "Couldn't create AVI Splitter." );
		DS_CHECK( _p_graph_builder->AddFilter( _p_ds_device, L"Default DirectSound Device" ), "Couldn't add DirectSound Device." );
	//	DIRECTSHOW_CHECK( getPin( pStreamSplitter, PINDIR_OUTPUT, 2, pStreamPin01 ), "Couldn't get output pin of AVI Splitter!" );

		//if( ( pStreamPin01 == nullptr ) )
		//	{
		//	MOVIE_DS_PRINT_STRING( "AVI file format error. Substream 0x01 does not deliver MEDIATYPE_Video." );
		//	return false;
		//	}
		HRESULT	hr1 = ds_helper::auto_connect_filters( pStreamSplitter, 2, _p_ds_device, 1, _p_graph_builder );
		if( hr1 == S_OK )
		{
		}
		else
			MOVIE_DS_PRINT_STRING( "No Audio stream in file or Audio codec not installed" );
//		else
//			DIRECTSHOW_CHECK( _p_graph_builder->QueryInterface( IID_IBasicAudio, reinterpret_cast<void**>(&_p_basic_audio) ), "Could find Basic Audio " );
	}

//maa	_p_video_source->SetSyncSource( nullptr );
//maa	pStreamSplitter->SetSyncSource( nullptr );
//	p_video_renderer_->SetSyncSource( nullptr );
//maa	_p_sample_grabber->SetSyncSource( nullptr );

	return true;

exit:
	//todoqqq release interfaces, because could not build graph
	pFileSource = nullptr;
	return false;
}

bool		c_movie_ds::open_renderfile( C_PCHAR_C filename )
{
	// use RenderFile of Directshow, but then we have to modify the graph to suits our needs

	bool					b_ret = false;
	WCHAR					filename_unicode[ MAX_PATH ];
	//locate default video decoder
	CComPtr<IBaseFilter>	vid_dec = nullptr;
	// FFdshow decoder
	CComPtr<IBaseFilter>	ffdshow_decoder = nullptr;

	//locate default video renderer
	CComPtr<IBaseFilter>	vid_renderer = nullptr;

	_p_video_source = nullptr;
	// todoqqq, if filename is more than 256, crash!! but should not happen since max_path is 256
	ds_helper::DXUtil_ConvertAnsiStringToWide( filename_unicode, filename, MAX_PATH );

	_p_graph_builder->RemoveFilter( _p_sample_grabber );
	_p_graph_builder->RemoveFilter( _p_null_renderer );

	DS_CHECK( _p_graph_builder->RenderFile( filename_unicode, L"" ), "Couldn't RenderFile" );

	enum_filters();

	DS_CHECK( _p_graph_builder->AddFilter( _p_sample_grabber, L"Sample Grabber" ), "Couldn't add Sample Grabber." );
	DS_CHECK( _p_graph_builder->AddFilter( _p_null_renderer, L"Null Video Renderer" ), "Couldn't add Null Video Renderer." );

	// check for codec that creates overlays, we can't get image using this codecs
	// so :	Microsoft DTV-DVD Video Decoder
	//		WMVideo Decoder DMO
	//		Mpeg4s Decoder DMO
	// and we replace it with FFDShow codec which decompress mostly anything
	_p_graph_builder->FindFilterByName( L"Microsoft DTV-DVD Video Decoder", &vid_dec );

	if( IS_NULL( vid_dec ) )
	{
		_p_graph_builder->FindFilterByName( L"WMVideo Decoder DMO", &vid_dec );
		if( IS_NULL( vid_dec ) )
			_p_graph_builder->FindFilterByName( L"Mpeg4s Decoder DMO", &vid_dec );
	}
	// get video renderer so we can put a sampler grabber
	_p_graph_builder->FindFilterByName( L"Video Renderer", &vid_renderer );

	if( IS_NOT_NULL( vid_dec ) )
	{
		// add FFDShow codec
#if DS_MAA
		DS_CHECK( ffdshow_decoder.CoCreateInstance( CLSID_FFDShowVideoDecoder ), "Could not create FFDshow Instance." );
#else
		DS_CHECK( CoCreateInstance( CLSID_FFDShowVideoDecoder, nullptr, CLSCTX_INPROC_SERVER, IID_IBaseFilter, reinterpret_cast<void**>(&ffdshow_decoder) ), "Could not create FFDshow Instance" ); 
#endif
		DS_CHECK( _p_graph_builder->AddFilter( ffdshow_decoder, L"FFDshow Video Decoder"), "Could not add FFDshow filter to graph" ); 

		// get input pin from video decoder, so we can get the output pin is's connected to
		CComPtr<IPin>	ipin = ds_helper::get_pin( vid_dec, PINDIR_INPUT );
		CComPtr<IPin>	opin = nullptr;
		ipin->ConnectedTo( &opin );
		ipin->Disconnect();
		opin->Disconnect();

		// get output pin from video decoder, so we know what filter we have to connect to
		CComPtr<IPin>	opin2 = ds_helper::get_pin( vid_dec, PINDIR_OUTPUT );
		CComPtr<IPin>	ipin2 = nullptr;
		opin2->ConnectedTo( &ipin2 );
		ipin2->Disconnect();
		opin2->Disconnect();

		ipin2 = nullptr;
		opin2 = nullptr;

		//remove filter we don't want to use
		_p_graph_builder->RemoveFilter( vid_dec );
		_p_graph_builder->RemoveFilter( vid_renderer );
		vid_dec = nullptr;
		vid_renderer = nullptr;

		//get the input pin of the sample grabber
		ipin2 = ds_helper::get_pin(_p_sample_grabber, PINDIR_INPUT );

		//connect the filter that was originally connected to the default renderer to the sample grabber
		_p_graph_builder->Connect( opin, ipin2 );

		ipin2 = nullptr;
		opin = nullptr;

		//get output pin of sample grabber
		opin = ds_helper::get_pin( _p_sample_grabber, PINDIR_OUTPUT );
		//get input pin of null renderer
		ipin = ds_helper::get_pin( _p_null_renderer, PINDIR_INPUT );

		//connect them
		_p_graph_builder->Connect( opin, ipin );
		ipin = nullptr;
		opin = nullptr;
	}
	else
	{
		if( vid_renderer )
		{
			//get input pin of video renderer
			CComPtr<IPin>	ipin = ds_helper::get_pin( vid_renderer, PINDIR_INPUT );
			CComPtr<IPin>	opin = nullptr;

			//find out who the renderer is connected to and disconnect from them
			ipin->ConnectedTo( &opin );
			ipin->Disconnect();
			opin->Disconnect();

			ipin = nullptr;

			//remove the default renderer from the graph
			_p_graph_builder->RemoveFilter( vid_renderer );
			vid_renderer = nullptr;

			//see if the video renderer was originally connected to a color space converter
			CComPtr<IBaseFilter>	color_converter = nullptr;
			_p_graph_builder->FindFilterByName( L"Color Space Converter", &color_converter );
			if( color_converter )
			{
				opin = nullptr;

				//remove the converter from the graph as well
				ipin = ds_helper::get_pin( color_converter, PINDIR_INPUT );

				ipin->ConnectedTo( &opin );
				ipin->Disconnect();
				opin->Disconnect();

				ipin = nullptr;

				_p_graph_builder->RemoveFilter( color_converter );
				color_converter = nullptr;
			}
			//connect the filter that was originally connected to the default renderer to the sample grabber
			_p_graph_builder->Connect( opin, ipin );
			ipin = nullptr;
			opin = nullptr;

			//get output pin of sample grabber
			opin = ds_helper::get_pin( _p_sample_grabber, PINDIR_OUTPUT );
			//get input pin of null renderer
			ipin = ds_helper::get_pin( _p_null_renderer, PINDIR_INPUT );

			//connect them
			_p_graph_builder->Connect( opin, ipin );
			ipin = nullptr;
			opin = nullptr;
		}
	}
	_p_graph_builder->FindFilterByName( L"Default DirectSound Device", &_p_ds_device );

	//if( !_b_use_audio )
	//{
	//	//remove audio filters from graph
	//	CComPtr<IBaseFilter>	ds_audio = nullptr;
	//	_p_graph_builder->FindFilterByName( L"Default DirectSound Device", &ds_audio );
	//	CComPtr<IPin>	ipin = nullptr;
	//	if( ds_audio != nullptr )
	//	{
	//		ipin = ds_helper::get_pin( ds_audio, PINDIR_OUTPUT );
	//		if( ipin != nullptr )
	//		{
	//			ipin->Disconnect();
	//			ipin = nullptr;
	//		}
	//		_p_graph_builder->RemoveFilter( ds_audio );
	//		ds_audio = nullptr;
	//	}
	//}
	//if( pVidDec )
	//{
	//	CComPtr<IPin>	ipin = GetPin( pVidDec, PINDIR_INPUT );
	//	CComPtr<IPin>	opin = nullptr;
	//	ipin->ConnectedTo( &opin );
	//	ipin->Disconnect();
	//	opin->Disconnect();

	//	CComPtr<IPin>	opin2 = GetPin( pVidDec, PINDIR_OUTPUT );
	//	CComPtr<IPin>	ipin2 = nullptr;
	//	opin2->ConnectedTo( &ipin2 );
	//	ipin2->Disconnect();
	//	opin2->Disconnect();

	//	SAFE_RELEASE( ipin2 );
	//	SAFE_RELEASE( opin2 );

	//	_p_graph_builder->RemoveFilter( pVidDec );
	//	_p_graph_builder->RemoveFilter( pVidRenderer );
	//	SAFE_RELEASE( pVidRenderer );
	//	SAFE_RELEASE( pVidDec );

	//	//get the input pin of the sample grabber
	//	ipin2 = GetPin(_p_sample_grabber, PINDIR_INPUT );

	//	//connect the filter that was originally connected to the default renderer
	//	//to the sample grabber
	//	_p_graph_builder->Connect( opin, ipin2 );
	//	SAFE_RELEASE( ipin2 );
	//	SAFE_RELEASE( opin );

	//	//get output pin of sample grabber
	//	opin = GetPin(_p_sample_grabber, PINDIR_OUTPUT);
	//	//get input pin of null renderer
	//	ipin = GetPin(_p_null_renderer, PINDIR_INPUT);

	//	//connect them
	//	_p_graph_builder->Connect(opin, ipin);
	//	SAFE_RELEASE(ipin);
	//	SAFE_RELEASE(opin);
	//}
	//if( pVidRenderer )
	//{
	//	//get input pin of video renderer
	//	IPin* ipin = GetPin(pVidRenderer, PINDIR_INPUT);
	//	IPin* opin = nullptr;
	//	IPin* ipin2 = nullptr;
	//	IPin* opin2 = nullptr;

	//	//find out who the renderer is connected to and disconnect from them
	//	ipin->ConnectedTo(&opin);
	//	ipin->Disconnect();
	//	opin->Disconnect();

	//	SAFE_RELEASE(ipin);

	//	//remove the default renderer from the graph
	//	_p_graph_builder->RemoveFilter(pVidRenderer);
	//	SAFE_RELEASE(pVidRenderer);

	//	//see if the video renderer was originally connected to 
	//	//a color space converter
	//	IBaseFilter* pColorConverter = nullptr;
	//	_p_graph_builder->FindFilterByName(L"Color Space Converter", &pColorConverter);
	//	if(pColorConverter)
	//	{
	//		SAFE_RELEASE(opin);

	//		//remove the converter from the graph as well
	//		ipin = GetPin(pColorConverter, PINDIR_INPUT);

	//		ipin->ConnectedTo(&opin);
	//		ipin->Disconnect();
	//		opin->Disconnect();

	//		SAFE_RELEASE(ipin);

	//		_p_graph_builder->RemoveFilter(pColorConverter);
	//		SAFE_RELEASE(pColorConverter);
	//	}

	//	if( pVidDec)
	//	{
	//		ipin2 = GetPin(pVidDec, PINDIR_INPUT);
	//		//find out who the renderer is connected to and disconnect from them
	//		ipin2->ConnectedTo(&opin2);
	//	//	ipin->Disconnect();
	//		opin2->Disconnect();
	//	//	SAFE_RELEASE(opin);

	//		ipin2 = GetPin( pFFDshowDecoder, PINDIR_INPUT );
	//		_p_graph_builder->Connect(opin2, ipin2);

	//		opin2 = GetPin( pFFDshowDecoder, PINDIR_OUTPUT );

	//		_p_graph_builder->RemoveFilter(pVidDec);
	//		SAFE_RELEASE( pVidDec );
	//	}

	//	//get the input pin of the sample grabber
	//	ipin = GetPin(_p_sample_grabber, PINDIR_INPUT);

	//	//connect the filter that was originally connected to the default renderer
	//	//to the sample grabber
	//	_p_graph_builder->Connect(opin2, ipin);
	//	SAFE_RELEASE(ipin);
	//	SAFE_RELEASE(opin);

	//	//get output pin of sample grabber
	//	opin = GetPin(_p_sample_grabber, PINDIR_OUTPUT);
	//	//get input pin of null renderer
	//	ipin = GetPin(_p_null_renderer, PINDIR_INPUT);

	//	//connect them
	//	_p_graph_builder->Connect(opin, ipin);
	//	SAFE_RELEASE(ipin);
	//	SAFE_RELEASE(opin);
	//}


	//	// get output pin of the sample grabber, so we can know the format of the sample in the Sample Grabber callback
	DS_CHECK( ds_helper::get_pin( _p_sample_grabber, PINDIR_OUTPUT, 1, _p_dec_pin_out ), "Could not get Output pin of the Sample Grabber." );

//maa	_p_sample_grabber->SetSyncSource( nullptr );

	b_ret  = true;

exit:
	vid_dec			= nullptr;
	ffdshow_decoder = nullptr;
	vid_renderer	= nullptr;
	return b_ret;
}

bool c_movie_ds::open_ffdshow_lav( C_PCHAR_C filename, bool b_lav )
{
	// Directshow Graph for FFDShow :
	//	LavSplitter Source	-> FFDShow Video Decoder -> Sample Grabber -> Null Renderer
	//						-> FFDShow Audio Decoder -> DirectSound Default Device
	// Directshow Graph for Lav Cuvid :
	//	LavSplitter Source	-> Lav Cuvid -> FFDShow Video Decoder -> Sample Grabber -> Null Renderer
	//						-> Lav Audio -> DirectSound Default Device

	bool						b_ret		= false;
	WCHAR						filename_unicode[ MAX_PATH ];
	
	CComPtr<IBaseFilter>		video_dec	= nullptr;		// Lav Video Decoder or FFdshow decoder
//	CComPtr<IBaseFilter>		ffdshow_video_dec	= nullptr;		// FFdshow decoder
	
	CComPtr<IBaseFilter>		audio_dec	= nullptr;		// Audio Decoder (Lav or FFDShow)
	
	CComPtr<IBaseFilter>		lav_src		= nullptr;		// Lav Splitter Source
	
	CComPtr<IFileSourceFilter>	pFileSource	= nullptr;		// File Source
	CComPtr<IPin>				ipin		= nullptr;
	CComPtr<IPin>				opin		= nullptr;

	// todoqqq, if filename is more than 256, crash!! but should not happen since max_path is 256
	ds_helper::DXUtil_ConvertAnsiStringToWide( filename_unicode, filename, MAX_PATH );

	// Create and Add Filters to the graph

	// add Lav Splitter Source
#if DS_MAA
	DS_CHECK( lav_src.CoCreateInstance( CLSID_Lav_Split_Src ), "Could not create Lav Splitter Source" );
#else
	DS_CHECK( CoCreateInstance( CLSID_Lav_Split_Src, nullptr, CLSCTX_INPROC_SERVER, IID_IBaseFilter, reinterpret_cast<void**>(&lav_src) ), "Could not create Lav Splitter Source" ); 
#endif
	DS_CHECK( _p_graph_builder->AddFilter( lav_src, L"LavAudio Splitter Source"), "Could not add Lav Splitter Source filter to graph" ); 

	// add Lav Video decoder
	if( !b_lav )
	{
	//	DIRECTSHOW_CHECK( CoCreateInstance( CLSID_Lav_Cuvid, nullptr, CLSCTX_INPROC_SERVER, IID_IBaseFilter, reinterpret_cast<void**>(&cuvid_dec) ), "Could not create Lav Cuvid Decoder" ); 
	//	DIRECTSHOW_CHECK( _p_graph_builder->AddFilter( cuvid_dec, L"LavCuvid Decoder"), "Could not add LavCuvid filter to graph" ); 
#if DS_MAA
		DS_CHECK( video_dec.CoCreateInstance( CLSID_Lav_Video_Decoder ), "Could not create Lav Video Decoder" );
#else
		DS_CHECK( CoCreateInstance( CLSID_Lav_Video_Decoder, nullptr, CLSCTX_INPROC_SERVER, IID_IBaseFilter, reinterpret_cast<void**>(&video_dec) ), "Could not create Lav Video Decoder" );
#endif
		DS_CHECK( _p_graph_builder->AddFilter( video_dec, L"Lav Video Decoder" ), "Could not add Lav Video filter to graph" );
	}
	else
	{
#if DS_MAA
		DS_CHECK( video_dec.CoCreateInstance( CLSID_FFDShowVideoDecoder ), "Could not create FFDshow Video Decoder" );
#else
		DS_CHECK( CoCreateInstance( CLSID_FFDShowVideoDecoder, nullptr, CLSCTX_INPROC_SERVER, IID_IBaseFilter, reinterpret_cast<void**>(&video_dec) ), "Could not create FFDshow Video Decoder" ); 
#endif
		// add FFDShow Video Decoder
		DS_CHECK( _p_graph_builder->AddFilter( video_dec, L"FFDshow Video Decoder"), "Could not add FFDshow filter to graph" ); 
	}

	// Try to find Load Interface on Source Filter and Open File
	if( lav_src )
	{
		DS_CHECK( lav_src->QueryInterface( IID_IFileSourceFilter, reinterpret_cast<void**>(&pFileSource) ), "Couldn't get File Source filter." );
	}
	else
	{
		MOVIE_DS_PRINT_STRING( "Video Source pointer is not valid." );
		goto exit;
		//return false;
	}
	if( pFileSource )
	{
		DS_CHECK( pFileSource->Load( filename_unicode, nullptr ), "Video Input file not found." );
	}
	else
	{
		MOVIE_DS_PRINT_STRING( "File Source pointer is not valid" );
		goto exit;
	}

	//if( b_lav )
	//{
	//	DS_CHECK( ds_helper::connect_filters( lav_src, 1,				video_dec, 1 ),				"Couldn't connect Lav Source Splitter to Lav Video Decoder!"	);
	//	//DIRECTSHOW_CHECK( ds_helper::connect_filters( cuvid_dec, 1,				ffdshow_video_decoder, 1 ),	"Couldn't connect Lav Cuvid Decoder to FFDshow Video Decoder!"	);
	//	DS_CHECK( ds_helper::connect_filters( video_dec, 1,				_p_sample_grabber, 1 ), "Couldn't connect Lav Video Decoder to Sample Grabber!" );
	//} 
	//else
	//{
		DS_CHECK( ds_helper::connect_filters( lav_src, 1,	video_dec, 1 ),			"Couldn't connect Lav Source Splitter to FFDshow Video Decoder!" );
		DS_CHECK( ds_helper::connect_filters( video_dec, 1, _p_sample_grabber, 1 ), "Couldn't connect FFDshow Video Decoder to Sample Grabber!"	);
	//}
	//DIRECTSHOW_CHECK( ds_helper::connect_filters( ffdshow_video_decoder, 1,	_p_sample_grabber, 1 ),		"Couldn't connect FFDshow Video Decoder to Sample Grabber!"		);
	DS_CHECK( ds_helper::connect_filters( _p_sample_grabber, 1,		_p_null_renderer, 1 ),	 "Couldn't connect Sample Grabber to Null Renderer!" );

	{
		HRESULT	hr = S_FALSE;
		//get subtitle pin of source
		hr = ds_helper::get_pin( lav_src, &MEDIATYPE_Text, PINDIR_OUTPUT, opin );
		//get text pin of ffdshow video decoder
		hr = ds_helper::get_pin( video_dec, &MEDIATYPE_Text, PINDIR_INPUT, ipin );
	}

	if( IS_NULL( ipin ) || IS_NULL( opin ) )
	{
#if AAA_DEBUG()
		DBG_PRINT_STRING( "%s() No Subtitle", __FUNCTION__ );
#endif
	}
	else
	{
		//connect them
		_p_graph_builder->Connect( opin, ipin );
		ipin = nullptr;
		opin = nullptr;
	}

	_b_audio_filter = false;
	if( is_audio_use() )
	{
		// but we don't care if there's any error during the construction of the audio graph, no audio continue without it
		if( !b_lav )
		{
			// add Lav Audio decoder
#if DS_MAA
			DS_AUDIO_CHECK( audio_dec.CoCreateInstance( CLSID_Lav_Audio ), "Could not create Lav Audio Decoder" );
#else
			DS_AUDIO_CHECK( CoCreateInstance( CLSID_Lav_Audio, nullptr, CLSCTX_INPROC_SERVER, IID_IBaseFilter, reinterpret_cast<void**>(&audio_dec) ), "Could not create Lav Audio Decoder" );
#endif
			DS_AUDIO_CHECK( _p_graph_builder->AddFilter( audio_dec, L"LavAudio Decoder"), "Could not add LavAudio filter to graph" ); 
		}
		else
		{
			// add FFDShow Audio decoder
#if DS_MAA
			DS_AUDIO_CHECK( audio_dec.CoCreateInstance( CLSID_FFDShowAudioDecoder ), "Could not create FFDShow Audio Decoder" );
#else
			DS_AUDIO_CHECK( CoCreateInstance( CLSID_FFDShowAudioDecoder, nullptr, CLSCTX_INPROC_SERVER, IID_IBaseFilter, reinterpret_cast<void**>(&audio_dec) ), "Could not create FFDShow Audio Decoder" ); 
#endif
			DS_AUDIO_CHECK( _p_graph_builder->AddFilter( audio_dec, L"FFDShow Audio Decoder"), "Could not add LavAudio filter to graph" ); 
		}

#if DS_MAA
		DS_AUDIO_CHECK( _p_ds_device.CoCreateInstance( CLSID_DSoundRender ), "Couldn't create DirectSound Renderer." );
#else
		DS_AUDIO_CHECK( CoCreateInstance( CLSID_DSoundRender, nullptr, CLSCTX_INPROC_SERVER, IID_IBaseFilter, reinterpret_cast<void**>(&_p_ds_device) ), "Couldn't create DirectSound Renderer." );
#endif
		if( !_p_ds_device )
		{
			MOVIE_DS_PRINT_STRING( "DirectSound Render pointer is not valid." );
			goto exit_audio;
		}
		DS_AUDIO_CHECK( _p_graph_builder->AddFilter( _p_ds_device, L"Default DirectSound Device" ),							"Couldn't add DirectSound Device." );

		//HRESULT	hr1 = ds_helper::auto_connect_filters( lav_src, 2, _p_ds_device, 1, _p_graph_builder );
		DS_AUDIO_CHECK( ds_helper::connect_filters( lav_src, 2,		audio_dec, 1 ),		"Couldn't connect Source to Audio Decoder!" );
		DS_AUDIO_CHECK( ds_helper::connect_filters( audio_dec, 1,	_p_ds_device, 1 ),	"Couldn't connect Audio Decoder to Output!" );
	}
	_b_audio_filter = true;
exit_audio:
//maa	lav_src->SetSyncSource( nullptr );

	// get output pin of the sample grabber, so we can know the format of the sample in the Sample Grabber callback
	DS_CHECK( ds_helper::get_pin( _p_sample_grabber, PINDIR_OUTPUT, 1, _p_dec_pin_out ),	"Could not get Output pin of the Sample Grabber." );

//maa	_p_sample_grabber->SetSyncSource( nullptr );

	b_ret = true;

exit:
	// release interface
	audio_dec		= nullptr;

	ipin			= nullptr;
	opin			= nullptr;
	video_dec		= nullptr;
//	ffdshow_video_dec	= nullptr; 

	pFileSource		= nullptr;
	lav_src			= nullptr;

	return b_ret;
}

AAA_ERR		c_movie_ds::open_low()
{
	AAA_ERR	ret_error = ERR_ANY;
#if	AAA_DEBUG()
	bool	b_add_to_rot = false;
#endif
	// Create a filter graph manager.
	CComPtr<IFilterGraph>	p_graph = nullptr;

#if DS_MAA
	DS_CHECK( p_graph.CoCreateInstance( CLSID_FilterGraph ), "DirectShow could not create graph" );
#else
	DS_CHECK( CoCreateInstance( CLSID_FilterGraph, nullptr, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, reinterpret_cast<void**>(&p_graph) ), "DirectShow could not create graph" );
#endif
	if( !p_graph )
	{
		MOVIE_DS_PRINT_STRING( "Graph Builder pointer is not valid." );
		goto exit;
	}

	DS_CHECK( p_graph->QueryInterface( IID_IGraphBuilder, reinterpret_cast<void**>(&_p_graph_builder) ), "DirectShow could not query graph builder interface" );
	p_graph = nullptr;

	// Create a capture filter graph builder ( we're lazy ).
#if DS_MAA
	DS_CHECK( _p_graph_builder_2.CoCreateInstance( CLSID_CaptureGraphBuilder2 ), "DirectShow could not create graph builder2" );
#else
	DS_CHECK( CoCreateInstance( CLSID_CaptureGraphBuilder2, nullptr, CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2, reinterpret_cast<void**>(&_p_graph_builder_2) ), "DirectShow could not create graph builder2" );
#endif
	if( !_p_graph_builder_2 )
	{
		MOVIE_DS_PRINT_STRING( "Graph Builder 2 pointer is not valid." );
		goto exit;
	}

	DS_CHECK( _p_graph_builder_2->SetFiltergraph( _p_graph_builder ), "Couldn't set graph builder 2." );

#if	AAA_DEBUG()
	ds_helper::add_to_running_object_table( _p_graph_builder, &_register_graph );
	b_add_to_rot = true;
#endif

	// Create a capture filter graph builder ( we're lazy ).
#if DS_MAA
	DS_CHECK( _p_null_renderer.CoCreateInstance( CLSID_NullRenderer ), "Couldn't create Null Video Renderer." );
#else
	DS_CHECK( CoCreateInstance( CLSID_NullRenderer, nullptr, CLSCTX_INPROC_SERVER, IID_IBaseFilter, ( void** )&_p_null_renderer ), "Couldn't create Null Video Renderer." );
#endif
	if( !_p_null_renderer )
	{
		MOVIE_DS_PRINT_STRING( "Video Renderer pointer is not valid." );
		goto exit;
	}
	DS_CHECK( _p_graph_builder->AddFilter( _p_null_renderer, L"Null Video Renderer" ), "Couldn't add Null Video Renderer." );

#if DS_MAA
	DS_CHECK( _p_sample_grabber.CoCreateInstance( CLSID_SampleGrabber ), "Couldn't create Sample Grabber." );
#else
	DS_CHECK( CoCreateInstance( CLSID_SampleGrabber, nullptr, CLSCTX_INPROC_SERVER, IID_IBaseFilter, ( void** )&_p_sample_grabber ), "Couldn't create Sample Grabber." );
#endif
	if( !_p_sample_grabber )
	{
		MOVIE_DS_PRINT_STRING( "Sample Grabber pointer is not valid." );
		goto exit;
	}
	DS_CHECK( _p_sample_grabber->QueryInterface( IID_ISampleGrabber, ( void** )&_sample_grabber ), "Couldn't query Sample Grabber interface." );
	if( !_sample_grabber )
	{
		MOVIE_DS_PRINT_STRING( "Real Sample Grabber pointer is not valid." );
		goto exit;
	}
	DS_CHECK( _p_graph_builder->AddFilter( _p_sample_grabber, L"Sample Grabber" ), "Couldn't add Sample Grabber." );

	{
		AM_MEDIA_TYPE	mt {};	// inited so even padding is initilaised
		mt.majortype	= MEDIATYPE_Video;
		mt.formattype	= GUID_NULL;
		C_PCHAR str;
		switch( _s_force_src_pixel_format )
		{
		case aaa::PIXEL_FORMAT_SRC_FORCE::R_8 :		mt.subtype = MEDIASUBTYPE_RGB8;		str = "Red 8";		break;
		case aaa::PIXEL_FORMAT_SRC_FORCE::RGB :		mt.subtype = MEDIASUBTYPE_RGB24;	str = "RGB";		break;
		case aaa::PIXEL_FORMAT_SRC_FORCE::YUY2 :	mt.subtype = MEDIASUBTYPE_YUY2;		str = "YUY2";		break;
		case aaa::PIXEL_FORMAT_SRC_FORCE::UYVY :	mt.subtype = MEDIASUBTYPE_UYVY;		str = "UYVY";		break;
		case aaa::PIXEL_FORMAT_SRC_FORCE::I420 :	mt.subtype = MEDIASUBTYPE_I420;		str = "I420";		break;
		case aaa::PIXEL_FORMAT_SRC_FORCE::RGBA:
		default:									mt.subtype = MEDIASUBTYPE_RGB32;	str = "RGBA";		break;
		}
		{
			o_str format;	// dynamic allocation: we usually avoid but can use o_str::push_name() here because of goto exit 
			format.set( "Couldn't set Sample Grabber to " );
			format.add( str );
			DS_CHECK( _sample_grabber->SetMediaType( &mt ), format.get() );
		}

		//	mt.subtype = MEDIASUBTYPE_YUY2;
		//	DIRECTSHOW_CHECK( sampleGrabber_->SetMediaType( &mt ), "Couldn't set Sample Grabber to YUV2." );
	}

	// Determine video file type
	//	INT32	video_file_type;
	{
		bool b_open = false;
		{
			C_PCHAR_C filename = get_filename();
			if( get_tex_video()->is_ds_lav() )
				b_open = open_ffdshow_lav( filename, true );
			else if( get_tex_video()->is_ds_ffdshow() )
				b_open = open_ffdshow_lav( filename, false );
			else if( get_tex_video()->is_ds_render_file() )
				b_open = open_renderfile( filename );
			else
			{
			//	video_file_type = pick_kind_from_name( filename );
				switch ( pick_kind_from_name( filename ) )
				{
				case aaa::MOVIE_TYPE::MPG :	b_open = open_mpg( filename );	break;
				case aaa::MOVIE_TYPE::WMV :	b_open = open_wmv( filename );	break;
				case aaa::MOVIE_TYPE::AVI :	b_open = open_avi( filename );	break;
				//todo test win7
				default :			b_open = open_renderfile( filename );	break;
				}
			}
		}
		if( b_open )
		{
			// make sure that audio is ok according to aaaseed interface, renderfile method will add Audio to the graph
			if( !is_audio_use() && IS_NOT_NULL( _p_ds_device ) )
			{
				CComPtr<IPin>	ipin = nullptr;
				if( IS_NOT_NULL( _p_ds_device ) )
				{
					ipin = ds_helper::get_pin( _p_ds_device, PINDIR_OUTPUT );
					if( IS_NOT_NULL( ipin ) )
					{
						ipin->Disconnect();
						ipin = nullptr;
					}
					_p_graph_builder->RemoveFilter( _p_ds_device );
					_p_ds_device = nullptr;
				}
			}
			_p_graph_builder->QueryInterface( IID_IBasicAudio, reinterpret_cast<void**>(&_p_basic_audio) );
			if( IS_NULL( _p_basic_audio ) )
				ERR_PRINT_STRING( "Could find Basic Audio" );

			bool	b_use_default_clock = false;
			// Set the graph clock
			if( b_use_default_clock )
			{
				CComPtr<IMediaFilter>	p_graph_mf;
				if( SUCCEEDED( _p_graph_builder->QueryInterface( IID_IMediaFilter, reinterpret_cast<void**>(&p_graph_mf) ) ) )
				{
					CComPtr<IReferenceClock>	p_graph_clock;
					HRESULT hr;
#if DS_MAA
					hr = p_graph_clock.CoCreateInstance( CLSID_SystemClock );
#else
					hr = CoCreateInstance( CLSID_SystemClock, nullptr, CLSCTX_INPROC_SERVER, IID_IBaseFilter, reinterpret_cast<void**>(&p_graph_clock) );
#endif		
					if( SUCCEEDED(hr) )
						p_graph_mf->SetSyncSource( p_graph_clock );
				}
			}
			return AAA_OK;
		}
	}
exit:
	// ERROR, release interfaces
#if	AAA_DEBUG()
	if( b_add_to_rot )
		ds_helper::remove_from_object_table( &_register_graph );
#endif
	p_graph = nullptr;
	// make sure everything is NULLified
	close();
	return ret_error;
}

void	c_movie_ds::enum_filters()
{
	CComPtr<IEnumFilters>	pEnum = nullptr;
	CComPtr<IBaseFilter>	pFilter = nullptr;
	ULONG					cFetched;
	//	std::vector<CString> names;

	_p_graph_builder->EnumFilters( &pEnum );

	while( pEnum->Next( 1, &pFilter, &cFetched ) == S_OK)
	{
		FILTER_INFO FilterInfo;
		char szName[256];
		//		o_str fname;

		pFilter->QueryFilterInfo( &FilterInfo );
		WideCharToMultiByte( CP_ACP, 0, FilterInfo.achName, -1, szName, 256, 0, 0 );
		MOVIE_DS_PRINT_STRING( "DirectShow Filter : %s", szName );
		//		fname= szName;
		//		names.push_back( fname );

		sysutils::safe_release( FilterInfo.pGraph );
		pFilter = nullptr;
	}
	pEnum = nullptr;
	//	return names;
}

bool	c_movie_ds::get_media_type()
{
	bool	b_supported = false;

//	_src_pixel_format = PIXEL_FORMAT::UNKNOWN;
	if( _p_dec_pin_out )
	{
		AM_MEDIA_TYPE	media_type;
		_p_dec_pin_out->ConnectionMediaType( &media_type );
		ds_helper::st_media_info info;
		ds_helper::get_media_info( &media_type, info );
		FreeMediaType( media_type );

		b_supported = aaa::c_pixel_format::is_supported( info.pixel_format );
		//set_src_pitch( info.pitch );
		_pitch					=	info.pitch;
		_fps					=	info.fps; 

		//if( b_supported )
		{
			//	INT32	nb_channel = c_pixel_format::get_channel_nb_from_force( _s_force_in_format );
			//PIXEL_FORMAT	type = c_pixel_format::get_src_pixel_format_from_force( _s_force_in_pixel_format );
			set_flux_size_format( info.size_x, info.size_y, info.pixel_format );
		}

		if( info.str_format )
			MOVIE_DS_PRINT_STRING( "DirectShow, format is %s", info.str_format );
	}
	return b_supported;
}

void	c_movie_ds::update()
{
	if( is_loop() )
	{
		// CheckMovieStatus: If the movie has ended, rewind to beginning
		long		lEventCode;
		LONG_PTR	lParam1, lParam2;
		HRESULT		hr;

		check_media_event();
		if( !_p_media_event )
			return;

		// Check for completion events
		hr = _p_media_event->GetEvent( &lEventCode, &lParam1, &lParam2, 0 );
		if( SUCCEEDED( hr ) )
		{
			// If we have reached the end of the media file, reset to beginning
			if( EC_COMPLETE == lEventCode ) 
			{
				set_position( 0.0f );
			}
			// Free any memory associated with this event
			hr = _p_media_event->FreeEventParams( lEventCode, lParam1, lParam2 );
		}
	}
}

//void	c_movie_ds::start()
//{
//	play();
//}


