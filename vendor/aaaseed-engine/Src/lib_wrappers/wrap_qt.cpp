//#include "aaa_def.h"
//#include "err.h"
//
//#if !AAA_WIN64()
//#	include <Movies.h>
//#	include <QTML.h>
//#	include <MediaHandlers.h>
//#endif
//
//#include "wrap_qt.h"
//#define		WRAP_SECTION_NAME	"QT"
//#include "platform/win32/wrap_dll.h"
//
//#if AAA_WIN64()
//#	define		USE_LINKED_QT_LIB
//#endif
//
//#ifdef	USE_LINKED_QT_LIB		// do wrapped DLL calls
//
//#	if !AAA_WIN64()
//#	include <lib_use.h>
//	AAA_LIB_USE32( "qtmlClient" )
//#	endif
//#define		WRAPPER_NAME	QUICKTIME
//
////-----------------------------------------------------------------------------
//// Ritual dance - fake the wrap_XXX_Init/Term implementation for linked form
////-----------------------------------------------------------------------------
//UINT32	wrap_qt_Init ( void )
//{
//	return( WRAP_DLL_LINKED( WRAPPER_NAME ) );		// just give a data structure to work
//}
////-----------------------------------------------------------------------------
//UINT32	wrap_qt_Term ( void )
//{
//	return( UNWRAP_DLL_LINKED( WRAPPER_NAME ) );	// just give a data structure to work
//}
////-----------------------------------------------------------------------------
//
//#else	//	USE_LINKED_QT_LIB
////=============================================================================
//// Wrapper NAKED ( ritual dance part 1 )
//// NOTE For the wrapper implementation part we use "__declspec( naked )"
//// NOTE: The __cdecl has to be there, since the library interface is defined so.
//
//#	undef	QT_LIB_API
//#	define	QT_LIB_API	__declspec( naked )
//
////=============================================================================
//// Wrapper static data ( ritual dance part 2 )
////
//DECL_WRAPPER_DLL( QTMLClient, "QTMLClient.dll" )
////	#include <QTML.h>
//	QT_LIB_API	OSErr	MAAAPIENTRY	InitializeQTML( long flag )
//	WRAP_CALL2JUMP( InitializeQTML )
//
//	QT_LIB_API	void	MAAAPIENTRY	TerminateQTML( void )
//	WRAP_CALL2JUMP( TerminateQTML )
//
////	#include <MediaHandlers.h>
//	QT_LIB_API	ComponentResult	MAAAPIENTRY	MediaHasCharacteristic( MediaHandler mh, OSType characteristic, Boolean* hasIt )
//	WRAP_CALL2JUMP( MediaHasCharacteristic )
//
//	QT_LIB_API	ComponentResult	MAAAPIENTRY	MediaGetPublicInfo( MediaHandler mh, OSType infoSelector, void * infoDataPtr, Size* ioDataSize )
//	WRAP_CALL2JUMP( MediaGetPublicInfo )
//
//	QT_LIB_API	OSErr	MAAAPIENTRY	NativePathNameToFSSpec( char * inName, FSSpec * outFile, long flags )
//	WRAP_CALL2JUMP( NativePathNameToFSSpec )
//
////	#include <Movies.h>
//	QT_LIB_API	OSErr	MAAAPIENTRY	EnterMovies( void )
//	WRAP_CALL2JUMP( EnterMovies )
//
//	QT_LIB_API	void	MAAAPIENTRY	ExitMovies( void )
//	WRAP_CALL2JUMP( ExitMovies )
//
//	QT_LIB_API	double	MAAAPIENTRY	Fix2X( Fixed x )
//	WRAP_CALL2JUMP( Fix2X )
//
//	QT_LIB_API	Track	MAAAPIENTRY	GetMovieIndTrackType( Movie theMovie, long index, OSType trackType, long flags )
//	WRAP_CALL2JUMP( GetMovieIndTrackType )
//
//	QT_LIB_API	Media	MAAAPIENTRY	GetTrackMedia( Track theTrack )
//	WRAP_CALL2JUMP( GetTrackMedia )
//
//	QT_LIB_API	MediaHandler	MAAAPIENTRY	GetMediaHandler( Media theMedia )
//	WRAP_CALL2JUMP( GetMediaHandler )
//
//	QT_LIB_API	long	MAAAPIENTRY	GetMediaSampleCount( Media theMedia )
//	WRAP_CALL2JUMP( GetMediaSampleCount )
//
//	QT_LIB_API	OSErr	MAAAPIENTRY	GetMoviesError( void )
//	WRAP_CALL2JUMP( GetMoviesError )
//
//	QT_LIB_API	TimeValue	MAAAPIENTRY	GetMediaDuration( Media theMedia )
//	WRAP_CALL2JUMP( GetMediaDuration )
//
//	QT_LIB_API	TimeScale	MAAAPIENTRY	GetMediaTimeScale( Media theMedia )
//	WRAP_CALL2JUMP( GetMediaTimeScale )
//
//	QT_LIB_API	OSErr	MAAAPIENTRY	OpenMovieFile( const FSSpec * fileSpec, short * resRefNum, SInt8 permission )
//	WRAP_CALL2JUMP( OpenMovieFile )
//
//	QT_LIB_API	OSErr	MAAAPIENTRY	NewMovieFromFile( Movie* theMovie, short resRefNum, short* resId, StringPtr resName,
//											short newMovieFlags, Boolean* dataRefWasChanged )
//	WRAP_CALL2JUMP( NewMovieFromFile )
//
//	QT_LIB_API	OSErr	MAAAPIENTRY	CloseMovieFile( short resRefNum )
//	WRAP_CALL2JUMP( CloseMovieFile )
//
//	QT_LIB_API	Handle	MAAAPIENTRY	NewHandle( Size byteCount )
//	WRAP_CALL2JUMP( NewHandle )
//
//	QT_LIB_API	OSErr	MAAAPIENTRY	MemError( void )
//	WRAP_CALL2JUMP( MemError )
//
//	QT_LIB_API	void	MAAAPIENTRY	BlockMoveData( const void* srcPtr, void * destPtr, Size byteCount )
//	WRAP_CALL2JUMP( BlockMoveData )
//
//	QT_LIB_API	OSErr	MAAAPIENTRY	NewMovieFromDataRef( Movie* m, short flags, short* id, Handle dataRef, OSType dataRefType )
//	WRAP_CALL2JUMP( NewMovieFromDataRef )
//
//	QT_LIB_API	void	MAAAPIENTRY	DisposeHandle( Handle h )
//	WRAP_CALL2JUMP( DisposeHandle )
//
//	QT_LIB_API	void	MAAAPIENTRY	GetMovieBox( Movie theMovie, Rect* boxRect )
//	WRAP_CALL2JUMP( GetMovieBox )
//
//	QT_LIB_API	void	MAAAPIENTRY	SetMovieGWorld( Movie theMovie, CGrafPtr port, GDHandle gdh )
//	WRAP_CALL2JUMP( SetMovieGWorld )
//
//	QT_LIB_API	void	MAAAPIENTRY	DisposeGWorld( GWorldPtr offscreenGWorld )
//	WRAP_CALL2JUMP( DisposeGWorld )
//
//	QT_LIB_API	void	MAAAPIENTRY	SetMovieDrawingCompleteProc( Movie theMovie, long flags, MovieDrawingCompleteUPP proc, long refCon )
//	WRAP_CALL2JUMP( SetMovieDrawingCompleteProc )
//
//	QT_LIB_API	void	MAAAPIENTRY	SetMovieActive( Movie theMovie, Boolean active )
//	WRAP_CALL2JUMP( SetMovieActive )
//
//	QT_LIB_API	void	MAAAPIENTRY	MoviesTask( Movie theMovie, long maxMilliSecToUse )
//	WRAP_CALL2JUMP( MoviesTask )
//
//	QT_LIB_API	void	MAAAPIENTRY	GoToBeginningOfMovie( Movie theMovie )
//	WRAP_CALL2JUMP( GoToBeginningOfMovie )
//
//	QT_LIB_API	void	MAAAPIENTRY	SetMovieActiveSegment( Movie theMovie, TimeValue startTime, TimeValue duration )
//	WRAP_CALL2JUMP( SetMovieActiveSegment )
//
//	QT_LIB_API	TimeValue	MAAAPIENTRY	GetMovieTime( Movie theMovie, TimeRecord * currentTime )
//	WRAP_CALL2JUMP( GetMovieTime )
//
//	QT_LIB_API	Fixed	MAAAPIENTRY	GetMoviePreferredRate( Movie theMovie )
//	WRAP_CALL2JUMP( GetMoviePreferredRate )
//
//	QT_LIB_API	OSErr	MAAAPIENTRY	UpdateMovie( Movie theMovie )
//	WRAP_CALL2JUMP( UpdateMovie )
//
//	QT_LIB_API	OSErr	MAAAPIENTRY	QTNewGWorldFromPtr( GWorldPtr * gw, OSType pixelFormat, const Rect * boundsRect, CTabHandle cTable,
//											GDHandle aGDevice, GWorldFlags flags, void * baseAddr, long rowBytes )
//	WRAP_CALL2JUMP( QTNewGWorldFromPtr )
//
//	QT_LIB_API	Boolean	MAAAPIENTRY	LockPixels( PixMapHandle pm )
//	WRAP_CALL2JUMP( LockPixels )
//
//	QT_LIB_API	PixMapHandle	MAAAPIENTRY	GetGWorldPixMap( GWorldPtr offscreenGWorld )
//	WRAP_CALL2JUMP( GetGWorldPixMap )
//
//	QT_LIB_API	void	MAAAPIENTRY	SetGWorld( CGrafPtr port, GDHandle gdh )
//	WRAP_CALL2JUMP( SetGWorld )
//
//	QT_LIB_API	void	MAAAPIENTRY	DisposeMovie( Movie theMovie )
//	WRAP_CALL2JUMP( DisposeMovie )
//
//	QT_LIB_API	OSStatus	MAAAPIENTRY	SetMovieAudioMute( Movie m, Boolean muted, UInt32 flags )
//	WRAP_CALL2JUMP( SetMovieAudioMute )
//
//	QT_LIB_API	void	MAAAPIENTRY	StartMovie( Movie theMovie )
//	WRAP_CALL2JUMP( StartMovie )
//
//	QT_LIB_API	void	MAAAPIENTRY	StopMovie( Movie theMovie )
//	WRAP_CALL2JUMP( StopMovie )
//
//	QT_LIB_API	TimeBase	MAAAPIENTRY	GetMovieTimeBase( Movie theMovie )
//	WRAP_CALL2JUMP( GetMovieTimeBase )
//
//	QT_LIB_API	long	MAAAPIENTRY	GetTimeBaseFlags( TimeBase tb )
//	WRAP_CALL2JUMP( GetTimeBaseFlags )
//
//	QT_LIB_API	void	MAAAPIENTRY	SetTimeBaseFlags( TimeBase tb, long timeBaseFlags )
//	WRAP_CALL2JUMP( SetTimeBaseFlags )
//
//	QT_LIB_API	void	MAAAPIENTRY	SetMoviePlayHints( Movie theMovie, long flags, long flagsMask )
//	WRAP_CALL2JUMP( SetMoviePlayHints )
//
//	QT_LIB_API	Boolean	MAAAPIENTRY	IsMovieDone( Movie theMovie )
//	WRAP_CALL2JUMP( IsMovieDone )
//
//	QT_LIB_API	Fixed	MAAAPIENTRY	X2Fix( double x )
//	WRAP_CALL2JUMP( X2Fix )
//
//	QT_LIB_API	Fixed	MAAAPIENTRY	GetMovieRate( Movie theMovie )
//	WRAP_CALL2JUMP( GetMovieRate )
//
//	QT_LIB_API	void	MAAAPIENTRY	SetMovieRate( Movie theMovie, Fixed rate )
//	WRAP_CALL2JUMP( SetMovieRate )
//
//	QT_LIB_API	OSStatus	MAAAPIENTRY	SetMovieAudioGain( Movie m, Float32 gain, UInt32 flags )
//	WRAP_CALL2JUMP( SetMovieAudioGain )
//
//	QT_LIB_API	OSStatus	MAAAPIENTRY	SetMovieAudioBalance( Movie m, Float32 leftRight, UInt32 flags )
//	WRAP_CALL2JUMP( SetMovieAudioBalance )
//
//	QT_LIB_API	OSStatus	MAAAPIENTRY	GetMovieAudioBalance( Movie m, Float32* leftRight, UInt32 flags )
//	WRAP_CALL2JUMP( GetMovieAudioBalance )
//
//	QT_LIB_API	OSErr	MAAAPIENTRY	PrerollMovie( Movie theMovie, TimeValue time, Fixed Rate )
//	WRAP_CALL2JUMP( PrerollMovie )
//
//	QT_LIB_API	TimeValue	MAAAPIENTRY	GetMovieDuration( Movie theMovie )
//	WRAP_CALL2JUMP( GetMovieDuration )
//
//	QT_LIB_API	void	MAAAPIENTRY	SetMovieTimeValue( Movie theMovie, TimeValue newtime )
//	WRAP_CALL2JUMP( SetMovieTimeValue )
//
//	QT_LIB_API	TimeScale	MAAAPIENTRY	GetMovieTimeScale( Movie theMovie )
//	WRAP_CALL2JUMP( GetMovieTimeScale )
//DECL_WRAPPER_END( QTMLClient );
//
//UINT32	wrap_qt_Init ( void )
//{
//	return( WRAP_DLL( QTMLClient ) );		// just give a data structure to work
//}
////-----------------------------------------------------------------------------
//UINT32	wrap_qt_Term ( void )
//{
//	return( UNWRAP_DLL( QTMLClient ) );	// just give a data structure to work
//}
////-----------------------------------------------------------------------------
//int		_unresolved_call ( void )
//{
//	WRAP_UNRESOLVED( QTMLClient );		// just to get a 'fancy' error printout
//	return( FALSE );
//}//_unresolved_call
//
//#endif//USE_LINKED_QT_LIB
