
#include "win_avi.h"
#include "media/video/tex_video.h"


#ifndef AAA_SYSTEMUTILS_H
#	include "system/shared/SystemUtils.h"
#endif


void	avi_error( INT32 error_code )
{
	C_PCHAR	str;

	switch( error_code )
	{
	case AVIERR_BADFORMAT:		str = "AVIERR_BADFORMAT : The file couldn't be read, corrupt file or unrecognized format.";			break;
	case AVIERR_MEMORY:			str = "AVIERR_MEMORY : The file could not be opened because of insufficient memory.";				break;
	case AVIERR_FILEREAD:		str = "AVIERR_FILEREAD : A disk error reading the file.";											break;
	case AVIERR_FILEOPEN:		str = "AVIERR_FILEOPEN : A disk error opening the file.";											break;
	case REGDB_E_CLASSNOTREG:	str = "REGDB_E_CLASSNOTREG : According to the registry, the type of file specified in AVIFileOpen does not have a handler to process it.";
								break;
	default:					str = "AAASeed don't know about this AVI error yet.";												break;
	}
	ERR_PRINT_STRING( str );
}

INT32	c_movie_avi::lib_open_count = 0;

void	c_movie_avi::lib_open()		{	if( lib_open_count++ == 0 )	AVIFileInit();	}
void	c_movie_avi::lib_close()	{	if( --lib_open_count == 0 )	AVIFileExit();	}

c_movie_avi::c_movie_avi( c_image_flux_buffer* buf ) : c_movie_player( buf, false )
	,_b_delta_t_update		(	false	)
	,_pavi_file				(	nullptr	)
	,_stream				(	nullptr	)
	,_pgf					(	nullptr	)
	,_stream_length			(	0)
	,_video_frame_last		(	-1		)
{
	//_src_pixel_format = aaa::PIXEL_FORMAT::RGBA;
	lib_open();
}

c_movie_avi::~c_movie_avi()
{
	close();
	lib_close();
}

// Close the stream_ and file.
void	c_movie_avi::close_specific()
{
	//HEAP_IS_CORRUPT();
	if( _pgf )
	{
	//hack	this trigger a bug ????
		try
		{
			if( AVIStreamGetFrameClose( _pgf ) != 0)
				BOX_ERR("AVI: Error in AVIStreamGetFrameClose()");
		}
		catch(...)
		{
			BOX_ERR( "Error in AVIStreamGetFrameClose");
		}
		_pgf = nullptr;
	}

	//HEAP_IS_CORRUPT();
	if( _stream )
	{
		AVIStreamRelease( _stream );
		_stream = nullptr;
	}
	//HEAP_IS_CORRUPT();
	if( _pavi_file )
	{
		AVIFileRelease( _pavi_file );
		_pavi_file = nullptr;
	}
	//hack is it good to leave it allocated ?	data_dealloc();
}

aaa::MOVIE_LIB	c_movie_avi::get_type()
{
	return aaa::MOVIE_LIB::MS_AVI;
}

AAA_ERR	c_movie_avi::open_specific() 
{
	AAA_ERR retcode = ERR_ANY;

	LONG				size;
	HRESULT				hr; 
	BITMAPINFOHEADER*	p_bitmap = nullptr;

	C_PCHAR_C filename = get_filename();
//	hr = AVIFileOpen( &pavi_file_, filename, OF_READ, nullptr);
	hr = AVIFileOpenA( &_pavi_file, filename, OF_SHARE_DENY_WRITE, nullptr );
	if( hr != 0 )
	{
		ERR_PRINT_STRING( "AVI: Can't open file %s", filename );
		avi_error( hr );
		goto exit;
	}

	hr = AVIFileGetStream( _pavi_file, &_stream, streamtypeVIDEO, 0 );
	if( hr != 0 )
	{
		ERR_PRINT_STRING( "AVI: Can't open stream" );
		goto exit;
	}
	/*
	// Get the source format - currently can only deal with 24bpp 8-8-8 
	// video streams. However, should be possible to construct images at 
	// different bitdepths/pixel formats if we need to... 
		LONG lFmtLength; 
	AVIStreamFormatSize( m_pavi, 0, &lFmtLength ); 

	m_lpSrcFmt = (LPBITMAPINFOHEADER)MMMalloc( lFmtLength ); 
	memset ( m_lpSrcFmt, 0, lFmtLength); 
  
	AVIStreamReadFormat( m_pavi, 0, m_lpSrcFmt, &lFmtLength ); 
	*/
	// Determine the size of the format data using 
	// AVIStreamFormatSize.
	hr = AVIStreamFormatSize( _stream, 0, &size );
	if( hr != 0 )
	{
		ERR_PRINT_STRING( "AVI: Can't AVIStreamFormatSize()" );
		goto exit;
	}

	//	if ( size > sizeof(bitmap_ ) ) // Format too large? 
	//		goto exit;
	p_bitmap = (BITMAPINFOHEADER*) MALLOC( size );
	//	size = sizeof(bitmap_ ); 
	hr = AVIStreamReadFormat( _stream, 0, p_bitmap, &size );
	//	if ( bitmap_->biCompression != BI_RGB )
	//		GOOD_PRINT_STRING( "AVI this one is compressed" );

	/*
	{
		AVISTREAMINFO   strhdr; 
		hr = AVIStreamInfo( stream_, &strhdr, sizeof(strhdr) ); 
		if( hr )
		{
			BOX_ERR( "Can't AVIStreamInfo" );
			goto exit;
		}

		// Set parameters for the new stream_.
		biNew = bi;
		biNew.biWidth /= 2;
		biNew.biHeight /= 2;
		biNew.biSizeImage = ((((UINT)biNew.biBitCount * biNew.biWidth + 31) & ~31 ) / 8) * biNew.biHeight;
		SetRect(&strhdr.rcFrame, 0, 0, (int) biNew.biWidth, (int) biNew.biHeight);
	}
	*/
	{
		aaa::PIXEL_FORMAT	format = aaa::PIXEL_FORMAT::UNKNOWN;
		if ( p_bitmap->biBitCount == 24 )
			format = aaa::PIXEL_FORMAT::RGB_8;
		else if ( p_bitmap->biBitCount == 32 )
			format = aaa::PIXEL_FORMAT::RGBA_8;

		// Allocate rgba memory for the bitmaps.
		
		//SC81 if( ERR( init_with_size(  bitmap_->biWidth, bitmap_->biHeight, 4, __FUNCTION__ ) ) )
		set_flux_size_format( p_bitmap->biWidth, p_bitmap->biHeight, format );
		//if( ERR( set_flux_size_format(  p_bitmap->biWidth, p_bitmap->biHeight, format ) ) )
		//{
		//	goto exit;
		//}
	}


	//	init_from_mem( bitmap_->biWidth, bitmap_->biHeight, 4, IMG_TYPE_RGB, nullptr, 0 );
	/*	if ( bitmap_.biCompression == BI_RGB )
			data_in = MALLOC( bitmap_.biSizeImage ); 
		else
	*/
	{
	/*	bitmap_->biBitCount = 32;
		bitmap_->biCompression = BI_RGB;
		pgf_ = AVIStreamGetFrameOpen(stream_, bitmap_ ); 
		if( !pgf_ )
		{
			ERR_PRINT_STRING( "AVI: Can't find decompressor to 32Bit RGB for %s", filename );
			//    lpNew = GlobalAlloc( GMEM_MOVEABLE, biNew.biSizeImage ); 
			bitmap_->biBitCount = 24;
			pgf_	= AVIStreamGetFrameOpen( stream_, bitmap_ ); 
			if( !pgf_ )
			{
				ERR_PRINT_STRING( "AVI: Can't find decompressor to 24Bit RGB for %s", filename );
	*/
		//HEAP_CHECK();
		_pgf = AVIStreamGetFrameOpen( _stream, nullptr ); 
		if( !_pgf )
		{
			ERR_PRINT_STRING( "AVI: Can't find decompressor for %s", filename );
			goto exit;
		}
	/*
			}
		}
	*/
	}

	_stream_start = AVIStreamStart( _stream );
	_stream_end = AVIStreamEnd( _stream );

	_stream_length = AVIStreamSampleToTime( _stream, _stream_end );
	_duration = REAL(_stream_length ) / REAL(1000.);

	_stream_index = _stream_start;

	retcode = AAA_OK;
exit:
	if( ERR(retcode) )
		_stream_length = 0;
	IF_FREE_AND_NULL( p_bitmap );

	return retcode;
}

//todoqqq make sure we don't get the same frame several time
void	c_movie_avi::get_frame_data( INT32 index )
{
	if( _stream )
	{
		/*
		if ( bitmap_.biCompression == BI_RGB )
		{
			hr = AVIStreamRead( stream_, index, 1, data_in, bitmap_.biSizeImage, nullptr, nullptr );
			if( hr )
			{
				ERR_PRINT_STRING("Can't AVIStreamRead");
				return;
			}
			pbitmap_ = &bitmap_;
			psrc = data_in;
		}
		else
		*/
		{
	//		tbuf_add( c_tbuf_master::CH_VIDEO_GET_FRAME, 1., "avi_get_frame" );
			CLAMP_REF( index, _stream_start, _stream_end-1 );
			_pbitmap	= (LPBITMAPINFOHEADER)AVIStreamGetFrame( _pgf, index );
	//		tbuf_add( c_tbuf_master::CH_VIDEO_GET_FRAME, 0., nullptr );

			if( !_pbitmap )
			{
				ERR_PRINT_STRING( "Can't AVIStreamGetFrame");
				return;
			}
			aaa::PIXEL_FORMAT	format = aaa::PIXEL_FORMAT::UNKNOWN;
			if ( _pbitmap->biBitCount == 24 )
				format = aaa::PIXEL_FORMAT::BGR_8;
			else if ( _pbitmap->biBitCount == 32 )
			{
				format = aaa::PIXEL_FORMAT::BGRA_8;
			}
			if( format != aaa::PIXEL_FORMAT::UNKNOWN )
			{
				set_flux_size_format( _pbitmap->biWidth, _pbitmap->biHeight, format );
				got_frame( (UINT8*)_pbitmap + sizeof(BITMAPINFOHEADER), "WinAvi Flux" );
			}
		}

		/*
		// Compress the data. 
		CompressDIB( &bi, lpOld, &biNew, lpNew); 
 
		// Save the compressed data using AVIStreamWrite. 
		hr = AVIStreamWrite(psSmall, lStreamSize, 1, lpNew,
				biNew.biSizeImage, AVIIF_KEYFRAME, nullptr, nullptr ); 
		*/
	}
}

/*
void c_avi::get_data_all()
{
	INT32	i;
	for ( i = stream_start_; i < stream_end_; ++i )
	{
		get_data( i);
	} 
}
*/

INT32	c_movie_avi::get_frame_index_from_time( REAL in )
{
	return AVIStreamTimeToSample( _stream, INT32(in*1000) );
}

void	c_movie_avi::get_frame_data_from_time( REAL in )
{
	get_frame_data( get_frame_index_from_time( in ) );
}

void	c_movie_avi::get_frame_data_next()
{
	if( ++_stream_index >= _stream_end )
	{
		_stream_index = _stream_start;
	}
//	if( --stream_index_ < stream_start_ )
//		stream_index_ = stream_end_-1;
	get_frame_data( _stream_index );
}

INT32	c_movie_avi::get_frame_index_cur()
{
	return get_frame_index_from_time( _time_video );
}

void	c_movie_avi::set_position( REAL pos )
{
	if ( _time_video != pos )
	{
		_time_video = pos;
		_b_delta_t_update =false;
	}
}

void	c_movie_avi::update()
{
	INT32	frame_new = get_frame_index_cur();
	_delta_t.update();
	if( _b_delta_t_update )
		_time_video = REAL( _time_video + _delta_t.get_dt() * _rate);
	_b_delta_t_update = true;

	if( is_loop() )
		_time_video = FWRAP( _time_video, _duration );

	if( _video_frame_last != frame_new )
	{
		get_frame_data( frame_new );
		_video_frame_last = frame_new;
	}
}

