#include "mov_quicktime.h"

#if AAA_USE_QUICKTIME()
#	ifndef AAA_SOUND_NEW_H
#		include "media/sound/sound_new.h"
#	endif
	#include "media/video/tex_video.h"
	namespace	mov_quicktime
	{
#		include <QTML.h>
#		include <MediaHandlers.h>
	}
	using	namespace	mov_quicktime;
#	include "lib_wrappers/wrap_qt.h"

bool	c_movie_qt::b_dll_loaded = false;

namespace {
	bool	b_quicktime_init = false;
};
#define		NO_ERR( err )	( err == NO_ERROR )

void			initialize_quicktime();
void			close_quicktime();

OSErr			is_mpeg_media_handler( MediaHandler inMediaHandler, Boolean *outIsMPEG );
ComponentResult	get_frame_rate_mpeg( MediaHandler inMPEGMediaHandler, Fixed *outStaticFrameRate );
OSErr			get_frame_rate( Media inMovieMedia, double *outFPS );
void			get_mov_media_handler( Movie inMovie, Media *outMedia, MediaHandler *outMediaHandler );
void			get_mov_frame_rate( Movie inMovie, double *outStaticFrameRate );
bool			create_mov_from_path( C_PCHAR path, Movie &movie );
bool			create_mov_from_url( C_PCHAR url,  Movie &movie );
OSErr	__cdecl	quicktime_callback( Movie theMovie, long refCon );

#define OSERR_TO_STRING( code )		case code : return #code;	break;

/*QuickTime errors*/
C_PCHAR	get_quicktime_error_string( OSErr code )
{
	switch( code )
	{
	OSERR_TO_STRING( couldNotResolveDataRef			)
	OSERR_TO_STRING( badImageDescription			)
	OSERR_TO_STRING( badPublicMovieAtom				)
	OSERR_TO_STRING( cantFindHandler				)
	OSERR_TO_STRING( cantOpenHandler				)
	OSERR_TO_STRING( badComponentType				)
	OSERR_TO_STRING( noMediaHandler					)
	OSERR_TO_STRING( noDataHandler					)
	OSERR_TO_STRING( invalidMedia					)
	OSERR_TO_STRING( invalidTrack					)
	OSERR_TO_STRING( invalidMovie					)
	OSERR_TO_STRING( invalidSampleTable				)
	OSERR_TO_STRING( invalidDataRef					)
	OSERR_TO_STRING( invalidHandler					)
	OSERR_TO_STRING( invalidDuration				)
	OSERR_TO_STRING( invalidTime					)
	OSERR_TO_STRING( cantPutPublicMovieAtom			)
	OSERR_TO_STRING( badEditList					)
	OSERR_TO_STRING( mediaTypesDontMatch			)
	OSERR_TO_STRING( progressProcAborted			)
	OSERR_TO_STRING( movieToolboxUninitialized		)
	OSERR_TO_STRING( wfFileNotFound					)
	OSERR_TO_STRING( cantCreateSingleForkFile		)
	OSERR_TO_STRING( invalidEditState				)
	OSERR_TO_STRING( nonMatchingEditState			)
	OSERR_TO_STRING( staleEditState					)
	OSERR_TO_STRING( userDataItemNotFound			)
	OSERR_TO_STRING( maxSizeToGrowTooSmall			)
	OSERR_TO_STRING( badTrackIndex					)
	OSERR_TO_STRING( trackIDNotFound				)
	OSERR_TO_STRING( trackNotInMovie				)
	OSERR_TO_STRING( timeNotInTrack					)
	OSERR_TO_STRING( timeNotInMedia					)
	OSERR_TO_STRING( badEditIndex					)
	OSERR_TO_STRING( internalQuickTimeError			)
	OSERR_TO_STRING( cantEnableTrack				)
	OSERR_TO_STRING( invalidRect					)
	OSERR_TO_STRING( invalidSampleNum				)
	OSERR_TO_STRING( invalidChunkNum				)
	OSERR_TO_STRING( invalidSampleDescIndex			)
	OSERR_TO_STRING( invalidChunkCache				)
	OSERR_TO_STRING( invalidSampleDescription		)
	OSERR_TO_STRING( dataNotOpenForRead				)
	OSERR_TO_STRING( dataNotOpenForWrite			)
	OSERR_TO_STRING( dataAlreadyOpenForWrite		)
	OSERR_TO_STRING( dataAlreadyClosed				)
	OSERR_TO_STRING( endOfDataReached				)
	OSERR_TO_STRING( dataNoDataRef					)
	OSERR_TO_STRING( noMovieFound					)
	OSERR_TO_STRING( invalidDataRefContainer		)
	OSERR_TO_STRING( badDataRefIndex				)
	OSERR_TO_STRING( noDefaultDataRef				)
	OSERR_TO_STRING( couldNotUseAnExistingSample	)
	OSERR_TO_STRING( featureUnsupported				)
	OSERR_TO_STRING( noVideoTrackInMovieErr			)
	OSERR_TO_STRING( noSoundTrackInMovieErr			)
	OSERR_TO_STRING( soundSupportNotAvailableErr	)
	OSERR_TO_STRING( unsupportedAuxiliaryImportData	)
	OSERR_TO_STRING( auxiliaryExportDataUnavailable	)
	OSERR_TO_STRING( samplesAlreadyInMediaErr		)
	OSERR_TO_STRING( noSourceTreeFoundErr			)
	OSERR_TO_STRING( sourceNotFoundErr				)
	OSERR_TO_STRING( movieTextNotFoundErr			)
	OSERR_TO_STRING( missingRequiredParameterErr	)
	OSERR_TO_STRING( invalidSpriteWorldPropertyErr	)
	OSERR_TO_STRING( invalidSpritePropertyErr		)
	OSERR_TO_STRING( gWorldsNotSameDepthAndSizeErr	)
	OSERR_TO_STRING( invalidSpriteIndexErr			)
	OSERR_TO_STRING( invalidImageIndexErr			)
	OSERR_TO_STRING( invalidSpriteIDErr				)
	default:	return "Unknown Error Code";	break;
	}
}



void	initialize_quicktime()
{
	if( b_quicktime_init == false )
	{
		OSErr	err = noErr;
		err = dll_qt.InitializeQTML( 0 );
		if( !NO_ERR( err ) )
		{
			err_print( "Quicktime can't start up" );
			return;
		}
		err = dll_qt.EnterMovies();
		if( !NO_ERR( err ) )
		{
			err_print( "Quicktime can't EnterMovies" );
			return;
		}
		b_quicktime_init = true;
	}
}

void	close_quicktime()
{
	if( b_quicktime_init )
	{
		dll_qt.ExitMovies();
		dll_qt.TerminateQTML();
	//	wrap_qt_Term();
		b_quicktime_init = false;
	}
}

//Calculate the static frame rate for a given movie.
void	get_mov_frame_rate( Movie movie, double *frame_rate )
{
	if ( IS_NULL( movie ) )
		return;
	if ( IS_NULL( frame_rate ) )
		return;
	*frame_rate = 0;

	Media			movie_media;
	MediaHandler	movie_media_handler;
	/* get the media identifier for the media that contains the first
	video track's sample data, and also get the media handler for
	this media. */
	get_mov_media_handler( movie, &movie_media, &movie_media_handler );
	if ( movie_media && movie_media_handler )
	{
		Boolean	b_is_MPEG = false;
		/* is this the MPEG-1/MPEG-2 media handler? */
		OSErr err = is_mpeg_media_handler( movie_media_handler, &b_is_MPEG );
		if ( NO_ERR( err ) )
		{
			if ( b_is_MPEG ) /* working with MPEG-1/MPEG-2 media */
			{
				Fixed	static_frame_rate;
				ComponentResult	err1 = get_frame_rate_mpeg( movie_media_handler, &static_frame_rate );
				if ( err1 == noErr )
				{
					/* convert Fixed data result to type double */
					*frame_rate = dll_qt.Fix2X( static_frame_rate );
				}
			}
			else	/* working with non-MPEG-1/MPEG-2 media */
			{
				err = get_frame_rate( movie_media, frame_rate );
			}
		}
	}
}

#define	kCharacteristicHasVideoFrameRate	FOUR_CHAR_CODE('vfrr')
#define	kCharacteristicIsAnMpegTrack		FOUR_CHAR_CODE('mpeg')

/*
Get the media identifier for the media that contains the first
video track's sample data, and also get the media handler for
this media.
*/
void	get_mov_media_handler( Movie mov, Media *media, MediaHandler *media_handler )
{
	if( IS_NULL( mov ) || IS_NULL( media ) || IS_NULL( media_handler) )
		return;

	*media = nullptr;
	*media_handler = nullptr;

	/* get first video track */
	Track	video_track = dll_qt.GetMovieIndTrackType( mov, 1, kCharacteristicHasVideoFrameRate, movieTrackCharacteristic | movieTrackEnabledOnly );
	if ( IS_NOT_NULL( video_track ) )
	{
		/* get media ref. for track's sample data */
		*media = dll_qt.GetTrackMedia( video_track );
		if ( *media )
		{
			/* get a reference to the media handler component */
			*media_handler = dll_qt.GetMediaHandler( *media );
		}
	}
}

/*
Return true if media handler reference is from the MPEG-1/MPEG-2 media handler.
Return false otherwise.
*/
OSErr	is_mpeg_media_handler( MediaHandler media_handler, Boolean *b_is_MPEG )
{
	if( IS_NULL( media_handler ) )
		return 0;
	if( IS_NULL( b_is_MPEG ) )
		return 0;

	// is this the MPEG-1/MPEG-2 media handler ?
	return ( dll_qt.MediaHasCharacteristic( media_handler, kCharacteristicIsAnMpegTrack, b_is_MPEG ) );
}

/*
Given a reference to the media handler used for media in a MPEG-1/MPEG-2
track, return the static frame rate.
*/
ComponentResult	get_frame_rate_mpeg( MediaHandler mpeg_media_handler, Fixed* frame_rate )
{
	if( IS_NULL( mpeg_media_handler ) )
		return 0;
	if( IS_NULL( frame_rate ) )
		return 0;

	*frame_rate = 0;

	MHInfoEncodedFrameRateRecord	encodedFrameRate;
	Size							encodedFrameRateSize = sizeof(encodedFrameRate);

	/* get the static frame rate */
	ComponentResult	err = dll_qt.MediaGetPublicInfo( mpeg_media_handler, kMHInfoEncodedFrameRate, &encodedFrameRate, &encodedFrameRateSize );
	if( NO_ERR( err ) )
	{
		/* return frame rate at which the track was encoded */
		*frame_rate = encodedFrameRate.encodedFrameRate;
	}
	return err;
}

//Given a reference to the media that contains the sample data for a track, calculate the static frame rate.
OSErr	get_frame_rate( Media movie_media, double *frame_rate )
{
	if( IS_NULL( movie_media ) )
		return 0;
	if( IS_NULL( frame_rate ) )
		return 0;

	*frame_rate = 0;

	/* get the number of samples in the media */
	long	sample_count = dll_qt.GetMediaSampleCount( movie_media );
	OSErr	err = dll_qt.GetMoviesError();

	if( sample_count && NO_ERR( err ) )
	{
		/* find the media duration */
		//Quicktime 7.0 code
		//TimeValue64 duration = GetMediaDisplayDuration(inMovieMedia);
		TimeValue64	duration = dll_qt.GetMediaDuration( movie_media );
		err = dll_qt.GetMoviesError();
		if ( NO_ERR( err ) )
		{
			/* get the media time scale */
			TimeValue64	timeScale = dll_qt.GetMediaTimeScale( movie_media );
			err = dll_qt.GetMoviesError();
			if ( NO_ERR( err ) )
			{
				*frame_rate = (double)sample_count * (double)timeScale / (double)duration;
			}
		}
	}
	return err;
}

bool	create_mov_from_path( C_PCHAR path, Movie &movie )
{
	OSErr	err = 0;
	FSSpec	theFSSpec;
	short	actualResId = DoTheRightThing;

	err = dll_qt.NativePathNameToFSSpec( (CHAR*)path, &theFSSpec, 0 );
	if( !NO_ERR( err ) )
	{
		ERR_PRINT_STRING( "Quicktime NativePathNameToFSSpec failed %d", err );
		return false;
	}

	short	movieResFile;
	err = dll_qt.OpenMovieFile( &theFSSpec, &movieResFile, fsRdPerm );
	if( NO_ERR( err ) )
	{
		short	movieResID = 0;
		err = dll_qt.NewMovieFromFile( &movie, movieResFile, &movieResID, (UINT8*)0, newMovieActive, (Boolean *) 0 );
		if( NO_ERR( err ) )
			dll_qt.CloseMovieFile( movieResFile );
		else
		{
			ERR_PRINT_STRING( "Quicktime NewMovieFromFile %s failed : %d", path, err );
			return false;
		}
	}
	else
	{
		ERR_PRINT_STRING( "Quicktime OpenMovieFile %s failed : %d", path, err );
		return false;
	}
	return true;
}

bool	create_mov_from_url( C_PCHAR url,  Movie &movie )
{
//	char * url = (char *)urlIn.c_str();
	Handle	url_data_ref;
	OSErr	err;

	url_data_ref = dll_qt.NewHandle( strlen( url ) + 1 );
	if( ( err = dll_qt.MemError() ) != noErr )
	{
		ERR_PRINT_STRING( "Quicktime error creating URL handle" );
		return false;
	}

	dll_qt.BlockMoveData( url, *url_data_ref, strlen( url ) + 1 );

	err = dll_qt.NewMovieFromDataRef( &movie, newMovieActive, nil, url_data_ref, URLDataHandlerSubType );
	dll_qt.DisposeHandle( url_data_ref );

	if( !NO_ERR(err) )
	{
		ERR_PRINT_STRING( "Quicktime error loading URL" );
		return false;
	}
	else
		return true;
}

OSErr	__cdecl	quicktime_callback( Movie theMovie, long refCon )
{
	c_movie_qt*	p_mov = (c_movie_qt*)refCon;
	if ( p_mov )
		p_mov->update_pixels();
	return noErr;
}

void	c_movie_qt::update_pixels()
{
	got_frame( _offscreenGWorldPixels, "Quicktime Flux" );
}

AAA_ERR	c_movie_qt::open_specific()
{
	//if( name.substr(0, 7) == "http://"){
	//	if(! createMovieFromURL( name, moviePtr) ) return;
	if( !b_quicktime_init )
	{
		ERR_PRINT_STRING( "Quicktime not initialized" );
		return ERR_ANY;
	}
	if( !create_mov_from_path( get_filename(), _movie ) )
		return ERR_ANY;

	_rate = -1;	//	to make sure it is updated
	_b_audio_state = true;	//	works if it is the default at creation

	bool	b_Gworld = get_size_x() != 0 && get_size_y() != 0 );

	Rect	mov_rect;
	dll_qt.GetMovieBox( _movie, &(mov_rect) );
	if( b_Gworld )
	{
		// is the gworld the same size, then lets *not* de-allocate and reallocate:
		if ( get_size_x() == mov_rect.right && get_size_y() == mov_rect.bottom )
			dll_qt.SetMovieGWorld( _movie, _offscreenGWorld, nullptr );
		else
		{
			set_flux_size_format( mov_rect.right, mov_rect.bottom, aaa::PIXEL_FORMAT::BGRA_8 );
			delete( _offscreenGWorldPixels );
			if ( _offscreenGWorld )
				dll_qt.DisposeGWorld( _offscreenGWorld );
			createImgMemAndGWorld();
		}
	}
	else
	{
		set_flux_size_format( mov_rect.right, mov_rect.bottom, aaa::PIXEL_FORMAT::BGRA_8 );
		//_b_src_grey = false;
		//set_src_bit_per_pixel( 32 );
		createImgMemAndGWorld();
	}
	//set_src_pixel_format( PIXEL_FORMAT::BGRA );

	if( IS_NULL( _movie ) )
	{
		return ERR_OBJ_NULL;
	}

	//----------------- callback method
	callback_function = NewMovieDrawingCompleteUPP( quicktime_callback );
	dll_qt.SetMovieDrawingCompleteProc( _movie, movieDrawingCallWhenChanged, callback_function, (long)this );

	dll_qt.SetMovieActive( _movie, true );
	dll_qt.MoviesTask( _movie, 0 );
	set_rate_low( 1.0 );
	dll_qt.MoviesTask( _movie, 0 );
	// TODO franz, transform _qt_movie_duration to FLOAT using GetMovieTimeScale
	_qt_movie_duration = dll_qt.GetMovieDuration( _movie );
	update_duration();
	// ------------- get some pixels in there ------
	dll_qt.GoToBeginningOfMovie( _movie );
	dll_qt.MoviesTask( _movie, 0 );
	dll_qt.SetMovieActiveSegment(_movie, -1, -1 );
	dll_qt.MoviesTask( _movie, 0 );

	//------------------ set the movie rate to default
	//------------------ and preroll, so the first frames come correct
	TimeValue	time_now = dll_qt.GetMovieTime( _movie, 0 );
	Fixed		play_rate = dll_qt.GetMoviePreferredRate( _movie ); 		//Not being used!

	dll_qt.PrerollMovie( _movie, time_now, dll_qt.X2Fix( _rate ) );
	set_rate_low( _rate );
	dll_qt.MoviesTask( _movie, 0 );

	// Set Volume, Panning, Rate and Position to last known values
	set_volume( _volume );
	set_pan( _pan );
	set_position_low( MIN( _time_video, _duration ) );
	stop();
	dll_qt.UpdateMovie( _movie );
	dll_qt.MoviesTask( _movie, 0 );

	double	fps;
	get_mov_frame_rate( _movie, &fps );
	set_fps( fps );

	//	got_frame( _offscreenGWorldPixels );
	return AAA_OK;
}

void	c_movie_qt::createImgMemAndGWorld()
{
	Rect	movieRect;
	movieRect.left 			= 0;
	movieRect.top 			= 0;
	movieRect.right 		= get_size_x();
	movieRect.bottom 		= get_size_y();
	_offscreenGWorldPixels 	= new UINT8[ 4 * get_size_x() * get_size_y() + 32 ];
//	_pixels					= new unsigned char[_size_x*_size_y*3];

	dll_qt.QTNewGWorldFromPtr( &(_offscreenGWorld), k32BGRAPixelFormat, &(movieRect), nullptr, nullptr, 0, (_offscreenGWorldPixels), 4 * get_size_x() );

	dll_qt.LockPixels( dll_qt.GetGWorldPixMap( _offscreenGWorld ) );
	dll_qt.SetGWorld(_offscreenGWorld, nullptr );
	dll_qt.SetMovieGWorld(_movie, _offscreenGWorld, nullptr );
}

void 	c_movie_qt::close_specific()
{
	if( is_valid() )
	{
		dll_qt.SetMovieActive( _movie, false );
		dll_qt.DisposeMovie( _movie );
		DisposeMovieDrawingCompleteUPP( callback_function );
		_movie = nullptr;
	}
}

//void	c_movie_qt::start()
//{
//	if( is_valid() && !is_started() )
//	{
//		set_flux_size_format( _size_x, _size_y, PIXEL_FORMAT::BGRA );
////		_b_src_grey = false;
////		set_src_bit_per_pixel( 32 );
////		set_src_pixel_format( PIXEL_FORMAT::BGRA );
//		set_started( true );
//	}
//}

void	c_movie_qt::pause()
{
	stop();
}

void	c_movie_qt::play()
{
	if( !is_started() )
	{
		if( is_valid() )
			set_started( true );
	//	start();
	}
	if( is_started() )
	{
		set_audio_use( get_tex_video()->is_audio() );
		if( _b_audio_state != is_audio_use() )
		{
			dll_qt.SetMovieAudioMute( _movie, !is_audio_use(), 0 );
			dll_qt.MoviesTask( _movie, 0 );
			_b_audio_state = is_audio_use();
		}
		if ( !is_playing() )
		{
			set_playing( true );
			dll_qt.StartMovie( _movie );
		}
		dll_qt.MoviesTask( _movie, 0 );
	}
}

void	c_movie_qt::stop()
{
	dll_qt.StopMovie( _movie );
	dll_qt.MoviesTask( _movie, 0 );
	if( _b_need_refresh )
	{
		// make sure 1st frame is there
		dll_qt.UpdateMovie( _movie );
		dll_qt.MoviesTask( _movie, 0 );
		_b_need_refresh = false;
	}
	set_playing( false );
}

void	c_movie_qt::setLoopState( INT32 state )
{
	TimeBase	myTimeBase	= dll_qt.GetMovieTimeBase( _movie );
	long		myFlags		= dll_qt.GetTimeBaseFlags( myTimeBase );
	switch (state)
	{
	case 1:		// loop
		myFlags |= loopTimeBase;
		myFlags &= ~palindromeLoopTimeBase;
		dll_qt.SetMoviePlayHints( _movie, hintsLoop, hintsLoop );
		dll_qt.SetMoviePlayHints( _movie, 0L, hintsPalindrome );
		break;
	case 2:		// loop palindrome
		myFlags |= loopTimeBase;
		myFlags |= palindromeLoopTimeBase;
		dll_qt.SetMoviePlayHints( _movie, hintsLoop, hintsLoop );
		dll_qt.SetMoviePlayHints( _movie, hintsPalindrome, hintsPalindrome );
		break;
	case 0:	// no loop
	default:
		myFlags &= ~loopTimeBase;
		myFlags &= ~palindromeLoopTimeBase;
		dll_qt.SetMoviePlayHints( _movie, 0L, hintsLoop | hintsPalindrome );
		break;
	}
	dll_qt.SetTimeBaseFlags( myTimeBase, myFlags );
}

void	c_movie_qt::set_position_low( REAL pos )
{
	_time_video = pos;
	long	scale	= dll_qt.GetMovieTimeScale( _movie );
	long	new_pos	= (long)( (REAL)scale * pos );
	dll_qt.SetMovieTimeValue( _movie, new_pos );
	dll_qt.MoviesTask( _movie, 0 );
}

void	c_movie_qt::set_position( REAL pos )
{
	if( ABS( _time_video - pos ) > .0f )
	{
		set_position_low( pos );
	}
}

REAL	c_movie_qt::get_position()
{
	long	scale	= dll_qt.GetMovieTimeScale( _movie );
	long	current	= dll_qt.GetMovieTime( _movie, nullptr );
	_pos_last = current;
	REAL	pos 	= ( (REAL)current / (REAL)scale );
	_time_video = pos;
	return pos;
}

INT32	c_movie_qt::get_frame_index_cur()
{
	if ( _time_video > .000000001f )
		return 1;
	return 0;
}

void	c_movie_qt::update()
{
	if( is_loop() )
	{
		if( dll_qt.IsMovieDone( _movie ) )
			dll_qt.GoToBeginningOfMovie( _movie );
	}
}

void	c_movie_qt::update_duration()
{
	_duration = _qt_movie_duration / (double) dll_qt.GetMovieTimeScale( _movie );	//todo get the second value only when we do change which can impact it and cache as duration
}

void	c_movie_qt::set_rate_low( DOUBLE rate )
{
	if( _rate != rate )
	{
		//setMovieRate actually plays, so let's call it only when we are playing
		dll_qt.SetMovieRate( _movie, dll_qt.X2Fix( rate ) );
		_rate = dll_qt.Fix2X( dll_qt.GetMovieRate( _movie ) );
		update_duration();
	}
}

DOUBLE	c_movie_qt::set_rate( DOUBLE rate )
{
	if( is_playing() )	//setMovieRate actually plays, so let's call it only when we are playing
		set_rate_low( rate );
	return _rate;
}

bool	c_movie_qt::set_volume( REAL volume )
{
	// gain 0 -> 0, 1.0 normal volume
	dll_qt.SetMovieAudioGain( _movie, volume, nil );
	return true;
}

bool	c_movie_qt::set_pan( REAL pan )
{
	// range is [-1.,1.] in Quicktime
	//REAL	real_pan = pan * 2.0 - 1.0;
	OSErr	err = dll_qt.SetMovieAudioBalance( _movie, pan, 0 );
	if ( !NO_ERR( err ) )
	{
		ERR_PRINT_STRING( "Quicktime error setting panning" );
		return false;
	}
	return true;
}

REAL	c_movie_qt::get_pan()
{
	OSErr	err;
	Float32	qt_pan = 0.0f;
	err = dll_qt.GetMovieAudioBalance( _movie, &qt_pan, 0 );
	if ( !NO_ERR( err ) )
	{
		ERR_PRINT_STRING( "Quicktime error getting panning" );
		return .5f;
	}

	// range is -1.0-1.0 in Quicktime
	//REAL	pan;
	//pan = ( qt_pan + 1.0f ) * .5;
	return qt_pan;
}

INT32	c_movie_qt::lib_open_count = 0;

void	c_movie_qt::lib_open()
{
	if( lib_open_count++ == 0 )
	{
//		b_dll_loaded = wrap_qt_Init() == 0;
		b_dll_loaded = dll_qt.init();
		if( b_dll_loaded )
		{
			initialize_quicktime();
		}
	}
}

void	c_movie_qt::lib_close()
{
	if( --lib_open_count == 0 )
	{
		close_quicktime();
		if( b_dll_loaded )
		{

			dll_qt.deinit();
//			wrap_qt_Term();
		}
	}
}

void	c_movie_qt::init_post_constructor()
{
	lib_open();
}

c_movie_qt::c_movie_qt( c_image_flux_buffer* buf ) : c_movie_player( buf, true )
,_movie(nullptr)

,_b_need_refresh(true)
{
	_b_is_callback = true;		// Quicktime use Callbacks

	//_src_pixel_format = PIXEL_FORMAT::RGBA;
	set_src_y_inverted( true );

	lib_open();
}

c_movie_qt::~c_movie_qt()
{
	close();
	lib_close();
}
#endif
