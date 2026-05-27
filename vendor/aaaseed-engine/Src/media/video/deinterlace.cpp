
#ifndef	AAA_DEINTERLACE_H
#	include "deinterlace.h"
#endif
#include "spy.h"
#include "image_flux.h"

//#define DENOISE_DIAMETER 5
//#define DENOISE_THRESH 7
//
//#define FRAME_ONLY 0
//#define FIELD_ONLY 1
//#define FRAME_AND_FIELD 2

FACTORY_ABSTRACT_CREATE_V1( c_deinterlace, deinterlace, Deinterlace );

CONSTRUCTOR_ABSTRACT_CREATE( c_deinterlace )
{
	_size_y = 0;
	_size_x = 0;
	_size_out_y = 0;
	_nb_bit_out = 0;
	_min_image_nb = 2;
}

c_deinterlace::~c_deinterlace()
{
	close();
}

//bool	c_deinterlace::is_field_needed( bool b_double_framerate )
//{
//	return b_double_framerate;
//}

void	c_deinterlace::init_with_size( INT32 size_x, INT32 size_y )
{
	if( _b_create_buffers || ( _size_x != size_x ) || ( _size_y != size_y ) )
	{
		close();
		init( size_x, size_y );
		_b_create_buffers = false;
		_size_x = size_x;
		_size_out_y = _size_y = size_y;
	}
}

void	c_deinterlace::simple_copy( UINT8 *src, UINT8 *dst )
{
// basic deinterlacing based on 1 frame
// Weave is a simple copy
	// assuming RGB32 as input
	MEMCPY( dst, src, _size_y * _size_x * 4, __FUNCTION__ );
}

void	c_deinterlace::run_frame( UINT8 *src, UINT8 *dst )
{
// basic deinterlacing based on 1 frame
	// assuming RGB32 as input
	simple_copy( dst, src );
}

void	c_deinterlace::run_half( c_image_flux* image_flux, UINT8* dst )
{
	_b_is_processed = false;

	bool b_img_locked;
	c_img_2d*	img_a = image_flux->lock_and_get_image_index( _image_index, b_img_locked );
	if( !img_a )
	{
		ERR_PRINT_STRING( "nothing ready in image_flux" );
	}
	else
	{
		UINT8* src_a = img_a->get_data_uint8();

//		dst = image_flux->get_data();
	
//			INT32	bit_nb_out;

		if( src_a )
		{
			//if( texture_flux_master->b_move_to_gpu_force_32bits)
			//	bit_nb_out = 32;	//todoqq refine
			//else
			_nb_bit_out = img_a->get_byte_per_pixel() * 8;

			//REAL	col[3];
			//REAL	col_dia[4];
			//scale_v3r( col, col_ui, col_ui[3] );
			//scale_v3r( col_dia, col_dia_ui, col_dia_ui[3] );
			//col_dia[3] = col_dia_int;

			TBUF_ADD_OBJ( tbuf::CH_VIDEO_PROCESS, 1., "video_process", this );
			
			// frame based deinterlacing
			run_frame( src_a, dst );

			_size_out_y = _size_y;
		}
		if( b_img_locked )
			img_a->unlock();
		//image_flux->release_data();
		//image_flux->release_bind();
		if( src_a )
		{
//			image_flux->clear_empty();	//todo set_empty(0) ?
			TBUF_ADD_OBJ( tbuf::CH_VIDEO_PROCESS, 0., nullptr, this );

			_b_is_processed = true;
		}
	}
}

//void	c_deinterlace::run(UINT8* src_a, UINT8* dst)
//{
//	switch(s_deinterlace_method)
//		{
//		case SIMPLE_BOB :
//			// todo, copy image since methods are not implemented
//			MEMCPY( dst, src_a, width * _size_y * 4);
//			break;
//		case AREA :
//			area_32( src_a, dst );
//			break;
//		case SMART :
//			smart_32( src_a, dst );
//  			break;
//		case SMOOTH :
//			smooth_32( src_a, dst, image_index );
//			break;
//		case KERNEL :
//		case LEAK_KERNEL :
//			kernel_32( src_a, dst, 1 );
//			break;
//		}
//}

//INT32	c_deinterlace::run_field(UINT8* src_a, UINT8* src_b, UINT8* dst )
//{
//	INT32	h;
//
//	switch(s_deinterlace_method)
//		{
//		case SIMPLE_BOB :
//			// bob copy field, we gain 1 field because the field are not combined
//			MEMCPY( dst, src_b, _size_x * _size_y * 4 );
//			h = _size_y;
//			break;
//		case WEAVE :
//			weave_32( src_a, src_b, dst );
//			h = _size_y * 2;
//			break;
//		case AREA :
//			area_32_field( src_a, src_b, dst );
//			h = _size_y * 2;
//			break;
//		case SMART :
//			smart_32_field( src_a, src_b, dst );
//			h = _size_y * 2;
//  			break;
//		case SMOOTH :
//			smooth_32( src_a, dst, image_index );
//			h = _size_y;
//  			break;
//		case KERNEL :
//		case LEAK_KERNEL :
//			kernel_32_field( src_a, src_b, dst, /*s_field_order*/ 1 );
//			h = _size_y * 2;
//			break;
//		}
//	return h;
//}








//void kernel_32_old( UINT8* p_src, UINT8* p_dst, INT32 width, INT32 _size_y, INT32 threshold)
//{
//	INT32 order, threshold;
//	bool sharp, twoway, map;
//
//
//    CONST UINT8 *srcp, *prvp, *prvpp, *prvpn, *prvppp, *prvpnn, *prvp4p, *prvp4n;
//	CONST UINT8 *srcp_saved;
//	CONST UINT8 *srcpp, *srcppp, *srcpn, *srcpnn, *srcp3p, *srcp3n, *srcp4p, *srcp4n;
//    UINT8 *dstp;
//	UINT8 *dstp_saved;
// 
//	INT32 src_pitch;
//    INT32 dst_pitch;
//	INT32 x, y;
//	INT32 val, hi, lo;
//	double valf;
//
//	sharp = true;
//	twoway = false;
//	map = true;
//
//	order = 0;
//	threshold = 10;
//
//	order = (order + 1) & 1;
//
//	srcp = srcp_saved = p_src;
//
//	src_pitch =  width * 4;
////	src_pitch =  width * 3;
//	dstp = dstp_saved = p_dst;
//	dst_pitch =  width * 4;
//
//	// clean dst bitmap
//	MEMSET( (UINT32 *)dstp , 0xffff , width * _size_y * sizeof (UINT32)); // * sizeof (UINT32));
//
//
////	srcp = srcp_saved  + (1-order) * src_pitch;
////	dstp = dstp_saved  + (1-order) * dst_pitch;
//	for (y = 0; y < _size_y; y+=2)
//		{
//		MEMCPY( dstp, srcp, dst_pitch); //width);
//		srcp += 2*src_pitch;
//		dstp += 2*dst_pitch;
//		}
//
//	// Copy through the lines that will be missed below.
//	MEMCPY( dstp_saved + order*dst_pitch, srcp_saved + (1-order)*src_pitch, dst_pitch); //width);
//	MEMCPY( dstp_saved + (2+order)*dst_pitch, srcp_saved + (3-order)*src_pitch, dst_pitch); //width);
//	MEMCPY( dstp_saved + (_size_y-2+order)*dst_pitch, srcp_saved + (_size_y-1-order)*src_pitch, dst_pitch); //width);
//	MEMCPY( dstp_saved + (_size_y-4+order)*dst_pitch, srcp_saved + (_size_y-3-order)*src_pitch, dst_pitch); //width);
//	/* For the other field choose adaptively between using the previous field
//		or the interpolant from the current field. */
//	prvp = s_prevFrame + 5*src_pitch - (1-order)*src_pitch;
//	prvpp = prvp - src_pitch;
//	prvppp = prvp - 2*src_pitch;
//	prvp4p = prvp - 4*src_pitch;
//	prvpn = prvp + src_pitch;
//	prvpnn = prvp + 2*src_pitch;
//	prvp4n = prvp + 4*src_pitch;
//	srcp = srcp_saved + 5*src_pitch - (1-order)*src_pitch;
//	srcpp = srcp - src_pitch;
//	srcppp = srcp - 2*src_pitch;
//	srcp3p = srcp - 3*src_pitch;
//	srcp4p = srcp - 4*src_pitch;
//	srcpn = srcp + src_pitch;
//	srcpnn = srcp + 2*src_pitch;
//	srcp3n = srcp + 3*src_pitch;
//	srcp4n = srcp + 4*src_pitch;
//	dstp =  dstp_saved  + 5*dst_pitch - (1-order)*dst_pitch;
//	for (y = 5 - (1-order); y <= _size_y - 5 - (1-order); y+=2)
//		{
//		for (x = 0; x < width * 4; ++x )
//			{
//			if ((threshold == 0) || /*(n == 0) ||*/
//				(ABS((INT32)prvp[x] - (INT32)srcp[x]) > threshold) ||
//				(ABS((INT32)prvpp[x] - (INT32)srcpp[x]) > threshold) ||
//				(ABS((INT32)prvpn[x] - (INT32)srcpn[x]) > threshold))
//				{
//				if (map)
//					{
//					INT32 g = x & ~3;
//					dstp[g++] = 255;
//					dstp[g++] = 255;
//					dstp[g++] = 255;
//					dstp[g] = 255;
//					x = g;
//					}
//				else
//					{
//					hi = 255;
//					lo = 0;
//					if (sharp)
//						{
//						if (twoway)
//							valf = + 0.526*((INT32)srcpp[x] + (int)srcpn[x])
//								+ 0.170*((INT32)srcp[x] + (INT32)prvp[x])
//								- 0.116*((INT32)srcppp[x] + (INT32)srcpnn[x] + (INT32)prvppp[x] + (INT32)prvpnn[x])
//					 			- 0.026*((INT32)srcp3p[x] + (INT32)srcp3n[x])
//								+ 0.031*((INT32)srcp4p[x] + (INT32)srcp4n[x] + (INT32)prvp4p[x] + (INT32)prvp4n[x]);
//						else
//							valf = + 0.526*((INT32)srcpp[x] + (INT32)srcpn[x])
//								+ 0.170*((INT32)prvp[x])
//								- 0.116*((INT32)prvppp[x] + (INT32)prvpnn[x])
//					 			- 0.026*((INT32)srcp3p[x] + (INT32)srcp3n[x])
//								+ 0.031*((INT32)prvp4p[x] + (INT32)prvp4p[x]);
//						if (valf > hi) valf = hi;
//						else if (valf < lo) valf = lo;
//						dstp[x] = (INT32) valf;
////						dstp[x] &= 0xff000000;
//						}
//					else
//						{
//						if (twoway)
//							val = (8*((INT32)srcpp[x] + (INT32)srcpn[x]) + 2*((INT32)srcp[x] + (INT32)prvp[x]) -
//								(INT32)(srcppp[x]) - (INT32)(srcpnn[x]) -
//								(INT32)(prvppp[x]) - (INT32)(prvpnn[x])) >> 4;
//						else
//							val = (8*((INT32)srcpp[x] + (INT32)srcpn[x]) + 2*((INT32)prvp[x]) -
//								(INT32)(prvppp[x]) - (INT32)(prvpnn[x])) >> 4;
//						if (val > hi) val = hi;
//						else if (val < lo) val = lo;
//						dstp[x] = (INT32) val;
//	//					dstp[x] &= 0xff000000;
//					}
//				}
//			}
//			else
//			{
//				dstp[x] = srcp[x];
////				dstp[x] &= 0xff000000;
//			}
//		}
//		prvp  += 2*src_pitch;
//		prvpp  += 2*src_pitch;
//		prvppp  += 2*src_pitch;
//		prvpn  += 2*src_pitch;
//		prvpnn  += 2*src_pitch;
//		prvp4p  += 2*src_pitch;
//		prvp4n  += 2*src_pitch;
//		srcp  += 2*src_pitch;
//		srcpp += 2*src_pitch;
//		srcppp += 2*src_pitch;
//		srcp3p += 2*src_pitch;
//		srcp4p += 2*src_pitch;
//		srcpn += 2*src_pitch;
//		srcpnn += 2*src_pitch;
//		srcp3n += 2*src_pitch;
//		srcp4n += 2*src_pitch;
//		dstp  += 2*dst_pitch;
//	}
//
//	MEMCPY(  (UINT32 *)s_prevFrame, (UINT32 *)p_src, width * _size_y * 4);
//
//}
//




//INT32 deinterlace_32_smart27(UINT8* p_src, UINT8* p_dst, INT32 width, INT32 _size_y, UINT32 n)
//{
//	INT32			pitch = width * 4;
//	UINT32 CONST	srcpitch = width * 4;
//	UINT32 CONST	srcpitchtimes2 = 2 * srcpitch;
//	UINT32 CONST	dstpitch = width * 4;
//	UINT32 CONST	dstpitchtimes2 = 2 * dstpitch;
//	UINT32 CONST	wminus1 = width - 1;
//	UINT32 CONST	wtimes2 = width * 2;
//	UINT32 CONST	wtimes4 = width * 4;
//	INT32 CONST	hminus1 = _size_y - 1;
//	INT32 CONST	hminus3 = _size_y - 3;
//	INT32 CONST	hover2 = _size_y / 2;
//	UINT32			*src, *dst, *srcminus, *srcplus, *srcminusminus, *srcplusplus;
//	UINT8			*moving, *movingminus, *movingplus;
//	UINT8			*fmoving;
//	UINT32			*saved, *sv;
//	UINT32 			*src1, *src2, *s1, *s2;
//	UINT32 			*dst1, *dst2, *d1, *d2;
//	UINT32			*prev;
//	INT32			scenechange;
//	INT32			count;
//	INT32			x, y;
//	INT32			prevValue, nextValue, luma, lumap, luman;
//	UINT32			p0, p1, p2;
//	INT32			r, g, b, rp, gp, bp, rn, gn, bn, T;
//	INT32			rpp, gpp, bpp, rnn, gnn, bnn, R, G, B;
//	UINT8			frMotion, fiMotion;
//	UINT32			copyback;
//
//	src = (UINT32 *)p_src;
//	dst = (UINT32 *)p_dst;
//
//	INT32			motionOnly = 0;
//	INT32 			Blend = 0;
//	INT32 			threshold = 0;
//	INT32			scenethreshold = 0;
//	INT32			fieldShift = 0;
//	INT32			inswap = 0;
//	INT32			outswap = 0;
//	INT32			highq = 1;
//	INT32			diffmode = 0;
//	INT32			colordiff = 0;
//	INT32			noMotion = 0;
//	INT32			cubic = 1;
//
//	threshold = 15;
//	scenethreshold = 100;
//	highq = 0;
//	diffmode = FRAME_ONLY;
//	colordiff = 1;
//	noMotion = 0;
//	cubic = 1;
//
//	/* If we are performing Advanced Processing... */
//	if (inswap || outswap || fieldShift)
//	{
//		/* Advanced Processing is used typically to clean up PAL video
//		   which has erroneously been digitized with the field phase off by
//		   one field. The result is that the frames look interlaced,
//		   but really if we can phase shift by one field, we'll get back
//		   the original progressive frames. Also supported are field swaps
//		   before and/or after the phase shift to accommodate different
//		   capture cards and telecining methods, as explained in the
//		   help file. Finally, the user can optionally disable full
//		   motion processing after this processing. */
//		copyback = 1;
//		if (!fieldShift)
//		{
//			/* No phase shift enabled, but we have swap(s) enabled. */
//			if (inswap && outswap)
//			{
//				if (noMotion)
//				{
//					/* Swapping twice is a null operation. */
//					src1 = src;
//					dst1 = dst;
//					for (y = 0; y < _size_y; ++y )
//					{
//						MEMCPY( dst1, src1, wtimes4);
//						src1 = (UINT32 *)((char *)src1 + srcpitch);
//						dst1 = (UINT32 *)((char *)dst1 + dstpitch);
//					}
//					return 0;
//				}
//				else
//				{
//					copyback = 0;
//				}
//			}
//			else
//			{
//				/* Swap fields. */
//				src1 = (UINT32 *)((char *)src + pitch);
//				saved = saveFrame + width;
//				for (y = 0; y < hover2; ++y )
//				{					
//					MEMCPY( saved, src1, wtimes4);
//					src1 = (UINT32 *)((UINT8 *)src1 + srcpitchtimes2);
//					saved += wtimes2;
//				}
//				src1 = src;
//				dst1 = (UINT32 *)((UINT8 *)dst + pitch);
//				for (y = 0; y < hover2; ++y )
//				{					
//					MEMCPY( dst1, src1, wtimes4);
//					src1 = (UINT32 *)((UINT8 *)src1 + srcpitchtimes2);
//					dst1 = (UINT32 *)((UINT8 *)dst1 + dstpitchtimes2);
//				}
//				dst1 = dst;
//				saved = saveFrame + width;
//				for (y = 0; y < hover2; ++y )
//				{					
//					MEMCPY( dst1, saved, wtimes4);
//					dst1 = (UINT32 *)((UINT8 *)dst1 + dstpitchtimes2);
//					saved += wtimes2;
//				}
//			}
//		}
//		/* If we reach here, then phase shift has been enabled. */
//		else
//		{
//			switch (inswap | (outswap << 1))
//			{
//			case 0:
//				/* No inswap, no outswap. */
//				src1 = src;
//				src2 = (UINT32 *)((UINT8 *)src + srcpitch);
//				dst1 = (UINT32 *)((UINT8 *)dst + dstpitch);
//				dst2 = dst;
//				saved = saveFrame + width;
//				break;
//			case 1:
//				/* Inswap, no outswap. */
//				src1 = (UINT32 *)((UINT8 *)src + srcpitch);
//				src2 = src;
//				dst1 = (UINT32 *)((UINT8 *)dst + dstpitch);
//				dst2 = dst;
//				saved = saveFrame;
//				break;
//			case 2:
//				/* No inswap, outswap. */
//				src1 = src;
//				src2 = (UINT32 *)((UINT8 *)src + srcpitch);
//				dst1 = dst;
//				dst2 = (UINT32 *)((UINT8 *)dst + dstpitch);
//				saved = saveFrame + width;
//				break;
//			case 3:
//				/* Inswap, outswap. */
//				src1 = (UINT32 *)((UINT8 *)src + srcpitch);
//				src2 = src;
//				dst1 = dst;
//				dst2 = (UINT32 *)((UINT8 *)dst + dstpitch);
//				saved = saveFrame;
//				break;
//			}
//
//			s1 = src1;
//			d1 = dst1;
//			for (y = 0; y < hover2; ++y )
//			{
//				MEMCPY( d1, s1, wtimes4);
//				s1 = (UINT32 *)((UINT8 *)s1 + srcpitchtimes2);
//				d1 = (UINT32 *)((UINT8 *)d1 + dstpitchtimes2);
//			}
//
//			/* If this is not the first frame, copy the buffered field
//			   of the last frame to the output. This creates a correct progressive
//			   output frame. If this is the first frame, a buffered field is not
//			   available, so interpolate the field from the current field. */
//			if (n == 0)
//			{
//				s1 = src1;
//				d2 = dst2;
//				for (y = 0; y < hover2; ++y )
//				{
//					MEMCPY( d2, s1, wtimes4);
//					s1 = (UINT32 *)((UINT8 *)s1 + srcpitchtimes2);
//					d2 = (UINT32 *)((UINT8 *)d2 + dstpitchtimes2);
//				}
//			}
//			else
//			{
//				d2 = dst2;
//				sv = saved;
//				for (y = 0; y < hover2; ++y )
//				{
//					MEMCPY( d2, sv, wtimes4);
//					sv += wtimes2;
//					d2 = (UINT32 *)((UINT8 *)d2 + dstpitchtimes2);
//				}
//			}
//			/* Finally, save the unused field of the current frame in the buffer.
//			   It will be used to create the next frame. */
//			s2 = src2;
//			sv = saved;
//			for (y = 0; y < hover2; ++y )
//			{
//				MEMCPY( sv, s2, wtimes4);
//				sv += wtimes2;
//				s2 = (UINT32 *)((UINT8 *)s2 + srcpitchtimes2);
//			}
//		}
//		if (noMotion) return 0;
//
//		if (copyback)
//		{
//			/* We're going to do motion processing also, so copy
//			   the result back into the src bitmap. */
//			src1 = dst;
//			dst1 = src;
//			for (y = 0; y < _size_y; ++y )
//			{
//				MEMCPY( dst1, src1, wtimes4);
//				src1 = (UINT32 *)((UINT8 *)src1 + srcpitch);
//				dst1 = (UINT32 *)((UINT8 *)dst1 + dstpitch);
//			}
//		}
//	}
//	else if (noMotion)
//	{
//		/* Well, I suppose somebody might select no advanced processing options
//		   but tick disable motion processing. This covers that. */
//		src1 = src;
//		dst1 = dst;
//		for (y = 0; y < _size_y; ++y )
//		{
//			MEMCPY( dst1, src1, srcpitch);
//			src1 = (UINT32 *)((UINT8 *)src1 + srcpitch);
//			dst1 = (UINT32 *)((UINT8 *)dst1 + dstpitch);
//		}
//		return 0;
//	}
//
//	/* End advanced processing mode code. Now do full motion-adaptive deinterlacing. */
//
//	/* Not much deinterlacing to do if there aren't at least 2 lines. */
//	if (_size_y < 2) return 0;
//
//	count = 0;
//
//	if (diffmode == FRAME_ONLY || diffmode == FRAME_AND_FIELD)
//	{
//		/* Skip first and last lines, they'll get a free ride. */
//		src = (UINT32 *)((UINT8 *)src + srcpitch);
//		srcminus = (UINT32 *)((UINT8 *)src - srcpitch);
//		prev = prevFrame + width;
//		moving = g_moving + width;
//		for (y = 1; y < hminus1; ++y )
//		{
//			x = 0;
//			do
//			{
//				// First check frame motion.
//				// Set the moving flag if the diff exceeds the configured
//				// threshold.
//				moving[x] = 0;
//				frMotion = 0;
//				prevValue = *prev;
//				if (!colordiff)
//				{
//					r = (src[x] >> 16) & 0xff;
//					g = (src[x] >> 8) & 0xff;
//					b = src[x] & 0xff;
//					luma = (76 * r + 30 * b + 150 * g) >> 8;
//					if (ABS(luma - prevValue) > threshold) frMotion = 1;
//				}
//				else
//				{
//					b = src[x] & 0xff;
//					bp = prevValue & 0xff;
//					if (ABS(b - bp) > threshold) frMotion = 1;
//					else
//					{
//						r = (src[x] >>16) & 0xff;
//						rp = (prevValue >> 16) & 0xff;
//						if (ABS(r - rp) > threshold) frMotion = 1;
//						else
//						{
//							g = (src[x] >> 8) & 0xff;
//							gp = (prevValue >> 8) & 0xff;
//							if (ABS(g - gp) > threshold) frMotion = 1;
//						}
//					}
//				}
//
//				// Now check field motion if applicable.
//				if (diffmode == FRAME_ONLY) moving[x] = frMotion;
//				else
//				{
//					fiMotion = 0;
//					if (y & 1)
//						prevValue = srcminus[x];
//					else
//						prevValue = *(prev + width);
//					if (!colordiff)
//					{
//						r = (src[x] >> 16) & 0xff;
//						g = (src[x] >> 8) & 0xff;
//						b = src[x] & 0xff;
//						luma = (76 * r + 30 * b + 150 * g) >> 8;
//						if (ABS(luma - prevValue) > threshold) fiMotion = 1;
//					}
//					else
//					{
//						b = src[x] & 0xff;
//						bp = prevValue & 0xff;
//						if (ABS(b - bp) > threshold) fiMotion = 1;
//						else
//						{
//							r = (src[x] >> 16) & 0xff;
//							rp = (prevValue >> 16) & 0xff;
//							if (ABS(r - rp) > threshold) fiMotion = 1;
//							else
//							{
//								g = (src[x] >> 8) & 0xff;
//								gp = (prevValue >> 8) & 0xff;
//								if (ABS(g - gp) > threshold) fiMotion = 1;
//							}
//						}
//					}
//					moving[x] = (fiMotion && frMotion);
//				}
//				if (!colordiff)
//					*prev++ = luma;
//				else
//					*prev++ = src[x];
//				/* Keep a count of the number of moving pixels for the
//				   scene change detection. */
//				if (moving[x]) ++count;
//			} while(++x < width);
//			src = (UINT32 *)((UINT8 *)src + srcpitch);
//			srcminus = (UINT32 *)((UINT8 *)srcminus + srcpitch);
//			moving += width;
//		}
//
//		/* Determine whether a scene change has occurred. */
//		if ((100L * count) / (_size_y * width) >= scenethreshold) scenechange = 1;
//		else scenechange = 0;
//
//		/* Perform a denoising of the motion map if enabled. */
//		if (!scenechange && highq)
//		{
//			int xlo, xhi, ylo, yhi;
//			int u, v;
//			int N = 5;
//			int Nover2 = N/2;
//			int sum;
//			unsigned UINT8 *m;
//
//			// Erode.
//			fmoving = g_fmoving;
//			for (y = 0; y < _size_y; ++y )
//			{
//				for (x = 0; x < width; ++x )
//				{
//					if (!((g_moving + y * width)[x]))
//					{
//						fmoving[x] = 0;	
//						continue;
//					}
//					xlo = x - Nover2; if (xlo < 0) xlo = 0;
//					xhi = x + Nover2; if (xhi >= width) xhi = wminus1;
//					ylo = y - Nover2; if (ylo < 0) ylo = 0;
//					yhi = y + Nover2; if (yhi >= _size_y) yhi = hminus1;
//					m = g_moving + ylo * width;
//					sum = 0;
//					for (u = ylo; u <= yhi; ++u )
//					{
//						for (v = xlo; v <= xhi; ++v )
//						{
//							sum += m[v];
//						}
//						m += width;
//					}
//					if (sum > 9)
//						fmoving[x] = 1;
//					else
//						fmoving[x] = 0;
//				}
//				fmoving += width;
//			}
//			// Dilate.
//			N = 5;
//			Nover2 = N/2;
//			moving = g_moving;
//			for (y = 0; y < _size_y; ++y )
//			{
//				for (x = 0; x < width; ++x )
//				{
//					if (!((g_fmoving + y * width)[x]))
//					{
//						moving[x] = 0;	
//						continue;
//					}
//					xlo = x - Nover2; if (xlo < 0) xlo = 0;
//					xhi = x + Nover2; if (xhi >= width) xhi = wminus1;
//					ylo = y - Nover2; if (ylo < 0) ylo = 0;
//					yhi = y + Nover2; if (yhi >= _size_y) yhi = hminus1;
//					m = g_moving + ylo * width;
//					for (u = ylo; u <= yhi; ++u )
//					{
//						for (v = xlo; v <= xhi; ++v )
//						{
//							m[v] = 1;
//						}
//						m += width;
//					}
//				}
//				moving += width;
//			}
//		}
//	}
//	else
//	{
//		/* Field differencing only mode. */
//		T = threshold * threshold;
//		src = (UINT32 *)((UINT8 *)src + srcpitch);
//		srcminus = (UINT32 *)((UINT8 *)src - srcpitch);
//		srcplus = (UINT32 *)((UINT8 *)src + srcpitch);
//		moving = g_moving + width;
//		for (y = 1; y < hminus1; ++y )
//		{
//			x = 0;
//			do
//			{
//				// Set the moving flag if the diff exceeds the configured
//				// threshold.
//				moving[x] = 0;
//				if (y & 1)
//				{
//					// Now check field motion.
//					fiMotion = 0;
//					nextValue = srcplus[x];
//					prevValue = srcminus[x];
//					if (!colordiff)
//					{
//						r = (src[x] >> 16) & 0xff;
//						rp = (prevValue >> 16) & 0xff;
//						rn = (nextValue >> 16) & 0xff;
//						g = (src[x] >> 8) & 0xff;
//						gp = (prevValue >> 8) & 0xff;
//						gn = (nextValue >> 8) & 0xff;
//						b = src[x] & 0xff;
//						bp = prevValue & 0xff;
//						bn = nextValue & 0xff;
//						luma = (76 * r + 30 * b + 150 * g) >> 8;
//						lumap = (76 * rp + 30 * bp + 150 * gp) >> 8;
//						luman = (76 * rn + 30 * bn + 150 * gn) >> 8;
//						if ((lumap - luma) * (luman - luma) > T)
//							moving[x] = 1;
//					}
//					else
//					{
//						b = src[x] & 0xff;
//						bp = prevValue & 0xff;
//						bn = nextValue & 0xff;
//						if ((bp - b) * (bn - b) > T) moving[x] = 1;
//						else
//						{
//							r = (src[x] >> 16) & 0xff;
//							rp = (prevValue >> 16) & 0xff;
//							rn = (nextValue >> 16) & 0xff;
//							if ((rp - r) * (rn - r) > T) moving[x] = 1;
//							else
//							{
//								g = (src[x] >> 8) & 0xff;
//								gp = (prevValue >> 8) & 0xff;
//								gn = (nextValue >> 8) & 0xff;
//								if ((gp - g) * (gn - g) > T) moving[x] = 1;
//							}
//						}
//					}
//				}
//				/* Keep a count of the number of moving pixels for the
//				   scene change detection. */
//				if (moving[x]) ++count;
//			} while(++x < width);
//			src = (UINT32 *)((UINT8 *)src + srcpitch);
//			srcminus = (UINT32 *)((UINT8 *)srcminus + srcpitch);
//			srcplus = (UINT32 *)((UINT8 *)srcplus + srcpitch);
//			moving += width;
//		}
//
//		/* Determine whether a scene change has occurred. */
//		if ((100L * count) / (_size_y * width) >= scenethreshold) scenechange = 1;
//		else scenechange = 0;
//
//		/* Perform a denoising of the motion map if enabled. */
//		if (!scenechange && highq)
//		{
//			INT32 xlo, xhi, ylo, yhi;
//			INT32 u, v;
//			INT32 N = 5;
//			INT32 Nover2 = N/2;
//			INT32 sum;
//			UINT8 *m;
//
//			// Erode.
//			fmoving = g_fmoving;
//			for (y = 0; y < _size_y; ++y )
//			{
//				for (x = 0; x < width; ++x )
//				{
//					if (!((g_moving + y * width)[x]))
//					{
//						fmoving[x] = 0;	
//						continue;
//					}
//					xlo = x - Nover2; if (xlo < 0) xlo = 0;
//					xhi = x + Nover2; if (xhi >= width) xhi = wminus1;
//					ylo = y - Nover2; if (ylo < 0) ylo = 0;
//					yhi = y + Nover2; if (yhi >= _size_y) yhi = hminus1;
//					m = g_moving + ylo * width;
//					sum = 0;
//					for (u = ylo; u <= yhi; ++u )
//					{
//						for (v = xlo; v <= xhi; ++v )
//						{
//							sum += m[v];
//						}
//						m += width;
//					}
//					if (sum > 9)
//						fmoving[x] = 1;
//					else
//						fmoving[x] = 0;
//				}
//				fmoving += width;
//			}
//
//			// Dilate.
//			N = 5;
//			Nover2 = N/2;
//			moving = g_moving;
//			for (y = 0; y < _size_y; ++y )
//			{
//				for (x = 0; x < width; ++x )
//				{
//					if (!((g_fmoving + y * width)[x]))
//					{
//						moving[x] = 0;	
//						continue;
//					}
//					xlo = x - Nover2; if (xlo < 0) xlo = 0;
//					xhi = x + Nover2; if (xhi >= width) xhi = wminus1;
//					ylo = y - Nover2; if (ylo < 0) ylo = 0;
//					yhi = y + Nover2; if (yhi >= _size_y) yhi = hminus1;
//					m = g_moving + ylo * width;
//					for (u = ylo; u <= yhi; ++u )
//					{
//						for (v = xlo; v <= xhi; ++v )
//						{
//							m[v] = 1;
//						}
//						m += width;
//					}
//				}
//				moving += width;
//			}		
//		}
//	}
//
//	// Render.
//    // The first line gets a free ride.
//	src = (UINT32 *)p_src;
//	dst = (UINT32 *)p_dst;
//
//	MEMCPY( dst, src, wtimes4);
//	src = (UINT32 *)((UINT8 *)src + pitch);
//	srcminus = (UINT32 *)((UINT8 *)src - srcpitch);
//	srcplus = (UINT32 *)((UINT8 *)src + srcpitch);
//	if (cubic)
//	{
//		srcminusminus = (UINT32 *)((UINT8 *)src - 3 * srcpitch);
//		srcplusplus = (UINT32 *)((UINT8 *)src + 3 * srcpitch);
//	}
//	dst = (UINT32 *)((UINT8 *)dst + pitch);
//	moving = g_moving + width;
//	movingminus = moving - width;
//	movingplus = moving + width;
//	for (y = 1; y < hminus1; ++y )
//	{
//		if (motionOnly)
//		{
//			if (Blend)
//			{
//				x = 0;
//				do {
//					if (!(movingminus[x] | moving[x] | movingplus[x]) && !scenechange)
//						dst[x] = 0x7f7f7f;
//					else
//					{	
//						/* Blend fields. */
//						p0 = src[x];
//						p0 &= 0x00fefefe;
//
//						p1 = srcminus[x];
//						p1 &= 0x00fcfcfc;
//
//						p2 = srcplus[x];
//						p2 &= 0x00fcfcfc;
//
//						dst[x] = (p0>>1) + (p1>>2) + (p2>>2);
//					}
//				} while(++x < width);
//			}
//			else
//			{
//				x = 0;
//				do {
//					if (!(movingminus[x] | moving[x] | movingplus[x]) && !scenechange)
//						dst[x] = 0x7f7f7f;
//					else if (y & 1)
//					{
//						if (cubic && (y > 2) && (y < hminus3))
//						{
//							rpp = (srcminusminus[x] >> 16) & 0xff;
//							rp =  (srcminus[x] >> 16) & 0xff;
//							rn =  (srcplus[x] >> 16) & 0xff;
//							rnn = (srcplusplus[x] >>16) & 0xff;
//							gpp = (srcminusminus[x] >> 8) & 0xff;
//							gp =  (srcminus[x] >> 8) & 0xff;
//							gn =  (srcplus[x] >>8) & 0xff;
//							gnn = (srcplusplus[x] >> 8) & 0xff;
//							bpp = (srcminusminus[x]) & 0xff;
//							bp =  (srcminus[x]) & 0xff;
//							bn =  (srcplus[x]) & 0xff;
//							bnn = (srcplusplus[x]) & 0xff;
//							R = (5 * (rp + rn) - (rpp + rnn)) >> 3;
//							if (R > 255) R = 255;
//							else if (R < 0) R = 0;
//							G = (5 * (gp + gn) - (gpp + gnn)) >> 3;
//							if (G > 255) G = 255;
//							else if (G < 0) G = 0;
//							B = (5 * (bp + bn) - (bpp + bnn)) >> 3;
//							if (B > 255) B = 255;
//							else if (B < 0) B = 0;
//							dst[x] = (R << 16) | (G << 8) | B;  
//						}
//						else
//						{
//							p1 = srcminus[x];
//							p1 &= 0x00fefefe;
//
//							p2 = srcplus[x];
//							p2 &= 0x00fefefe;
//							dst[x] = (p1>>1) + (p2>>1);
//						}
//					}
//					else
//						dst[x] = src[x];
//				} while(++x < width);
//			}
//		}
//		else  /* Not motion only */
//		{
//			if (Blend)
//			{
//				x = 0;
//				do {
//					if (!(movingminus[x] | moving[x] | movingplus[x]) && !scenechange)
//						dst[x] = src[x];
//					else
//					{
//						/* Blend fields. */
//						p0 = src[x];
//						p0 &= 0x00fefefe;
//
//						p1 = srcminus[x];
//						p1 &= 0x00fcfcfc;
//
//						p2 = srcplus[x];
//						p2 &= 0x00fcfcfc;
//
//						dst[x] = (p0>>1) + (p1>>2) + (p2>>2);
//					}
//				} while(++x < width);
//			}
//			else
//			{
//				// Doing line interpolate. Thus, even lines are going through
//				// for moving and non-moving mode. Odd line pixels will be subject
//				// to the motion test.
//				if (y&1)
//				{
//					x = 0;
//					do {
//						if (!(movingminus[x] | moving[x] | movingplus[x]) && !scenechange)
//							dst[x] = src[x];
//						else
//						{
//							if (cubic && (y > 2) && (y < hminus3))
//							{
//								rpp = (srcminusminus[x] >> 16) & 0xff;
//								rp =  (srcminus[x] >> 16) & 0xff;
//								rn =  (srcplus[x] >> 16) & 0xff;
//								rnn = (srcplusplus[x] >>16) & 0xff;
//								gpp = (srcminusminus[x] >> 8) & 0xff;
//								gp =  (srcminus[x] >> 8) & 0xff;
//								gn =  (srcplus[x] >>8) & 0xff;
//								gnn = (srcplusplus[x] >> 8) & 0xff;
//								bpp = (srcminusminus[x]) & 0xff;
//								bp =  (srcminus[x]) & 0xff;
//								bn =  (srcplus[x]) & 0xff;
//								bnn = (srcplusplus[x]) & 0xff;
//								R = (5 * (rp + rn) - (rpp + rnn)) >> 3;
//								if (R > 255) R = 255;
//								else if (R < 0) R = 0;
//								G = (5 * (gp + gn) - (gpp + gnn)) >> 3;
//								if (G > 255) G = 255;
//								else if (G < 0) G = 0;
//								B = (5 * (bp + bn) - (bpp + bnn)) >> 3;
//								if (B > 255) B = 255;
//								else if (B < 0) B = 0;
//								dst[x] = (0x000000ff << 24) | (R << 16) | (G << 8) | B;  
//							}
//							else
//							{
//								p1 = srcminus[x];
//								//p1 &= 0x00fefefe;
//								p1 &= 0xfffefefe;
//
//								p2 = srcplus[x];
//								//p2 &= 0x00fefefe;
//								p2 &= 0xfffefefe;
//
//								dst[x] = (p1>>1) + (p2>>1);
//							}
//						}
//					} while(++x < width);
//				}
//				else
//				{
//					// Even line; pass it through.
//					MEMCPY( dst, src, wtimes4);
//				}
//			}
//		}
//		src = (UINT32 *)((UINT8 *)src + pitch);
//		srcminus = (UINT32 *)((UINT8 *)srcminus + pitch);
//		srcplus = (UINT32 *)((UINT8 *)srcplus + pitch);
//		if (cubic)
//		{
//			srcminusminus = (UINT32 *)((UINT8 *)srcminusminus + pitch);
//			srcplusplus = (UINT32 *)((UINT8 *)srcplusplus + pitch);
//		}
//		dst = (UINT32 *)((UINT8 *)dst + pitch);
//		moving += width;
//		movingminus += width;
//		movingplus += width;
//	}
//	
//	// The last line gets a free ride.
//	MEMCPY( dst, src, wtimes4);
//
//	MEMCPY(  prevFrame, p_src, width * _size_y * 4);
//
//	return 0;
//}





//void c_deinterlace::smartbob_32(UINT8* p_field_1, UINT8* p_field_2, UINT8* p_dst)
//{
//
//}


