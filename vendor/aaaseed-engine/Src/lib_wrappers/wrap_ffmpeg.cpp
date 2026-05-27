#include "aaa_def.h"
#include "wrap_ffmpeg.h"

#if AAA_USE_FFMPEG()

#include "err.h"

#define		WRAP_SECTION_NAME	"AV"
#include "platform/win32/wrap_dll.h"


#if AAA_WIN64()
#	define	USE_LINKED_FFMPEG_LIB
#endif

#ifdef	USE_LINKED_FFMPEG_LIB		// do wrapped DLL calls

#include <lib_use.h>
#define AAA_LIB_USE_FFMPEG( name )	AAA_LIB_USE( "FFmpeg/" #name )
AAA_LIB_USE_FFMPEG( avformat	)
AAA_LIB_USE_FFMPEG( avutil		)
AAA_LIB_USE_FFMPEG( avcodec		)
AAA_LIB_USE_FFMPEG( avdevice	)
AAA_LIB_USE_FFMPEG( swscale		)

#define		WRAPPER_AV_FORMAT	AV_FORMAT
#define		WRAPPER_AV_UTIL		AV_UTIL
#define		WRAPPER_AV_CODEC	AV_CODEC
#define		WRAPPER_AV_DEVICE	AV_DEVICE
#define		WRAPPER_SW_SCALE	SW_SCALE

//-----------------------------------------------------------------------------
// Ritual dance - fake the wrap_XXX_Init/Term implementation for linked form
//-----------------------------------------------------------------------------
UINT32	wrap_ffmpeg_Init ( void )
{
	return		WRAP_DLL_LINKED( WRAPPER_AV_FORMAT ) 
			||	WRAP_DLL_LINKED( WRAPPER_AV_UTIL )
			||	WRAP_DLL_LINKED( WRAPPER_AV_CODEC )
			||	WRAP_DLL_LINKED( WRAPPER_AV_DEVICE )
			||	WRAP_DLL_LINKED( WRAPPER_SW_SCALE )
			;
}//wrap_ffmpeg_Init
//-----------------------------------------------------------------------------
UINT32	wrap_ffmpeg_Term ( void )
{
	return		UNWRAP_DLL_LINKED( WRAPPER_SW_SCALE ) 
			||	UNWRAP_DLL_LINKED( WRAPPER_AV_DEVICE )
			||	UNWRAP_DLL_LINKED( WRAPPER_AV_CODEC )
			||	UNWRAP_DLL_LINKED( WRAPPER_AV_UTIL )
			||	UNWRAP_DLL_LINKED( WRAPPER_AV_FORMAT )
			;
}//wrap_ffmpeg_Term
//-----------------------------------------------------------------------------

#else	//  USE_LINKED_FFMPEG_LIB

extern "C"
{
//#define UINT64_C(val) val##ui64
#ifndef AVCODEC_AVCODEC_H
#	include "ffmpeg/libavcodec/avcodec.h"
#endif
#ifndef AVFORMAT_AVFORMAT_H
#	include "ffmpeg/libavformat/avformat.h"
#endif
#ifndef SWSCALE_SWSCALE_H
#	include "ffmpeg/libswscale/swscale.h"
#endif
}

//=============================================================================
// Wrapper NAKED (ritual dance part 1)
// NOTE  For the wrapper implementation part we  use "__declspec(naked)"
// NOTE: The __cdecl has to be there, since the library interface is defined so.

#	undef	FFMPEG_LIB_API
#if AAA_WIN64()
#	define	FFMPEG_LIB_API	__declspec()
#else
#	define	FFMPEG_LIB_API	__declspec(naked)
#endif

//=============================================================================
// Wrapper static data (ritual dance part 2)
//
DECL_WRAPPER_DLL( AV_FORMAT, "avformat-53.dll" )
//gone in version 54
//	FFMPEG_LIB_API	int		MAAAPIENTRY	av_open_input_file(AVFormatContext **ic_ptr, const char *filename,
//											AVInputFormat *fmt, int buf_size, AVFormatParameters *ap)
//	WRAP_CALL2JUMP( av_open_input_file )
FFMPEG_LIB_API	AVFormatContext * MAAAPIENTRY avformat_alloc_context(void)
	WRAP_CALL2JUMP( avformat_alloc_context )
	
	FFMPEG_LIB_API	int		MAAAPIENTRY avformat_open_input(AVFormatContext **ps, const char *filename, AVInputFormat *fmt, AVDictionary **options)
	WRAP_CALL2JUMP( avformat_open_input )

	//deprectated
	FFMPEG_LIB_API	int		MAAAPIENTRY	av_find_stream_info(AVFormatContext *ic)
	WRAP_CALL2JUMP( av_find_stream_info )

	FFMPEG_LIB_API	int		MAAAPIENTRY avformat_find_stream_info(AVFormatContext *ic, AVDictionary **options)
	WRAP_CALL2JUMP( avformat_find_stream_info )

	FFMPEG_LIB_API	void	MAAAPIENTRY	av_close_input_file(AVFormatContext *s)
	WRAP_CALL2JUMP( av_close_input_file )

	FFMPEG_LIB_API	int64_t	MAAAPIENTRY	av_gettime(void)
	WRAP_CALL2JUMP( av_gettime )

	FFMPEG_LIB_API	int		MAAAPIENTRY	av_seek_frame(AVFormatContext *s, int stream_index, int64_t timestamp, int flags)
	WRAP_CALL2JUMP( av_seek_frame )

	FFMPEG_LIB_API	void	MAAAPIENTRY	av_register_all(void)
	WRAP_CALL2JUMP( av_register_all )

	FFMPEG_LIB_API	int		MAAAPIENTRY	av_read_frame(AVFormatContext *s, AVPacket *pkt)
	WRAP_CALL2JUMP( av_read_frame )
DECL_WRAPPER_END( AV_FORMAT );


DECL_WRAPPER_DLL( AV_CODEC, "avcodec-53.dll" )
	FFMPEG_LIB_API	AVCodec*	MAAAPIENTRY	avcodec_find_decoder(enum CodecID id)
	WRAP_CALL2JUMP( avcodec_find_decoder )

	FFMPEG_LIB_API	int		MAAAPIENTRY	avcodec_open(AVCodecContext *avctx, AVCodec *codec)
	WRAP_CALL2JUMP( avcodec_open )

	FFMPEG_LIB_API	AVFrame*	MAAAPIENTRY	avcodec_alloc_frame(void)
	WRAP_CALL2JUMP( avcodec_alloc_frame )

	FFMPEG_LIB_API	int		MAAAPIENTRY	avpicture_get_size(enum PixelFormat pix_fmt, int width, int height)
	WRAP_CALL2JUMP( avpicture_get_size )

	FFMPEG_LIB_API	int		MAAAPIENTRY	avpicture_fill(AVPicture *picture, uint8_t *ptr, enum PixelFormat pix_fmt, int width, int height)
	WRAP_CALL2JUMP( avpicture_fill )

	FFMPEG_LIB_API	int		MAAAPIENTRY	avcodec_close(AVCodecContext *avctx)
	WRAP_CALL2JUMP( avcodec_close )

	FFMPEG_LIB_API	void	MAAAPIENTRY	av_init_packet(AVPacket *pkt)
	WRAP_CALL2JUMP( av_init_packet )

	FFMPEG_LIB_API	int		MAAAPIENTRY	avcodec_decode_video2(AVCodecContext *avctx, AVFrame *picture, int *got_picture_ptr, const AVPacket *avpkt)
	WRAP_CALL2JUMP( avcodec_decode_video2 )

	FFMPEG_LIB_API	void	MAAAPIENTRY	av_free_packet(AVPacket *pkt)
	WRAP_CALL2JUMP( av_free_packet )
DECL_WRAPPER_END( AV_CODEC );


DECL_WRAPPER_DLL( SW_SCALE, "swscale-2.dll" )
	FFMPEG_LIB_API	struct SwsContext*	MAAAPIENTRY	sws_getContext(int srcW, int srcH, enum PixelFormat srcFormat,
														int dstW, int dstH, enum PixelFormat dstFormat, int flags,
														SwsFilter *srcFilter, SwsFilter *dstFilter, const double *param)
	WRAP_CALL2JUMP( sws_getContext )

	FFMPEG_LIB_API	void	MAAAPIENTRY	sws_freeContext(struct SwsContext *swsContext)
	WRAP_CALL2JUMP( sws_freeContext )

	FFMPEG_LIB_API	int		MAAAPIENTRY	sws_scale(struct SwsContext *context, const uint8_t* const srcSlice[], const int srcStride[],
											int srcSliceY, int srcSliceH, uint8_t* const dst[], const int dstStride[])
	WRAP_CALL2JUMP( sws_scale )
DECL_WRAPPER_END( SW_SCALE );


DECL_WRAPPER_DLL( AV_UTIL, "avutil-51.dll" )
	FFMPEG_LIB_API	void	MAAAPIENTRY	av_free(void *ptr)
	WRAP_CALL2JUMP( av_free )

	FFMPEG_LIB_API	int MAAAPIENTRY av_strerror(int errnum, char *errbuf, size_t errbuf_size)
	WRAP_CALL2JUMP( av_strerror )
DECL_WRAPPER_END( AV_UTIL );

UINT32	wrap_ffmpeg_Init ( void )
{
	UINT32	ret = 0;
	ret &= WRAP_DLL( AV_FORMAT );
	ret &= WRAP_DLL( AV_CODEC );
	ret &= WRAP_DLL( SW_SCALE );
	ret &= WRAP_DLL( AV_UTIL );
	return( ret );		// just give a data structure to work
}
//-----------------------------------------------------------------------------
UINT32	wrap_ffmpeg_Term ( void )
{
	UINT32	ret = 0;
	ret &= UNWRAP_DLL( AV_FORMAT );
	ret &= UNWRAP_DLL( AV_CODEC );
	ret &= UNWRAP_DLL( SW_SCALE );
	ret &= UNWRAP_DLL( AV_UTIL );
	return( ret );	// just give a data structure to work
}
//-----------------------------------------------------------------------------
int		_unresolved_call ( void )
{
	WRAP_UNRESOLVED( AV_FORMAT );	// just to get a 'fancy' error printout
	WRAP_UNRESOLVED( AV_CODEC );	// just to get a 'fancy' error printout
	WRAP_UNRESOLVED( SW_SCALE );	// just to get a 'fancy' error printout
	WRAP_UNRESOLVED( AV_UTIL );	// just to get a 'fancy' error printout
	return( FALSE );
}//_unresolved_call

#endif//USE_LINKED_FFMPEG_LIB

// EOF: wrap_ffmpeg.cpp

#endif	//#if AAA_USE_FFMPEG()