#include "aaa_util.h"
#include "memory.h"
#include "spy.h"

//#include <xmmintrin.h>
#include <emmintrin.h>

#ifndef	AAA_DEINTERLACE_H
#	include "deinterlace.h"
#endif

#define DENOISE_DIAMETER 5
//#define DENOISE_THRESH 7

#define FRAME_ONLY 0
#define FIELD_ONLY 1
#define FRAME_AND_FIELD 2

//#define DUFFS_DEVICE(x,size) \
//{ \
//	if(size > 0) \
//		{ \
//		int __DUFFS_DEVICE_count, __DUFFS_DEVICE_n; \
//		__DUFFS_DEVICE_count = size; \
//		__DUFFS_DEVICE_n = (__DUFFS_DEVICE_count+7) >> 3; \
//		switch (__DUFFS_DEVICE_count & 7) \
//			{ \
//			case 0: do { x; \
//			case 7: x; \
//			case 6: x; \
//			case 5: x; \
//			case 4: x; \
//			case 3: x; \
//			case 2: x; \
//			case 1: x; \
//			} while (--__DUFFS_DEVICE_n > 0); \
//			} \
//	} \
//}


//#define DUFFS_DEVICE2(x,size) \
//{ \
//	int __DUFFS_DEVICE_n; \
//	__DUFFS_DEVICE_n = (size >> 3); \
//	switch (size & 7) \
//		{ \
//		case 0: while (__DUFFS_DEVICE_n-- > 0) \
//				{ \
//					x; \
//					case 7: x; \
//					case 6: x; \
//					case 5: x; \
//					case 4: x; \
//					case 3: x; \
//					case 2: x; \
//					case 1: x; \
//				} \
//		} \
//}

	extern UINT8	*g_moving;
	extern UINT8	*g_fmoving;
	extern UINT32 *prevFrame, *saveFrame;

	extern UINT8 *s_prevFrame, *s_saveFrame, *s_moving, *s_fmoving;

	extern UINT8*	merge_src;

	// motion map, full resolution
	extern INT32*	s_map_full;


	// smooth deinterlacing
	extern UINT32*		pframe;
	extern UINT32*		pframeprev;
	extern INT32		*piFrameDiffs;
	extern INT32		iFrameNo;

	extern UINT32 *prevFrameLuma;
	extern UINT8 *pixelbackUp;
	extern UINT8 *pixelChanges;

	void deinterlace_area_24( UINT8* p_src, UINT8* p_dst, INT32 w, INT32 h, INT32 d_threshold, BOOL b_show_deinterlace)
{
	// Deinterlacing Area based, RGB 24 bit input
	UINT32	*src = (UINT32 *)p_src;
	UINT32	*dst = (UINT32 *)p_dst;
	// pitch for RGB24
	INT32	src_pitch = w * 3;
	// pitch for dest RGB32
	INT32	dst_pitch = w * 4;

	INT32	hminus = h - 1;

	INT32	iR0, iG0, iB0, iR1, iG1, iB1, iR2, iG2, iB2, iR3, iG3, iB3;
	UINT32	*psrc1, *psrc2, *psrc3, *pdst1;
	INT32	iInterlaceValue0, iInterlaceValue1, iInterlaceValue2;

	BOOL	bBlend = TRUE;
	INT32	iThreshold = d_threshold * d_threshold * 4;
	INT32	iEdgeDetect = 25;
	if(iEdgeDetect > 180)
		iEdgeDetect = 180;	// We don't want an integer overflow in the interlace calculation.
	iEdgeDetect = iEdgeDetect * iEdgeDetect;


	iR1 = iG1 = iB1 = 0;	// Avoid compiler warning. The value is not used.
	for(INT32 x = w; x > 0; x--)
		{
		// for every point on the first line
		psrc3 = (UINT32 *)((UINT8 *)src + 3 * (x -1));
		iR3 = (*psrc3 >> 16) & 0xff;
		iG3 = (*psrc3 >> 8) & 0xff;
		iB3 = *psrc3 & 0xff;
		psrc2 = (UINT32 *)((UINT8 *)psrc3 + src_pitch);
		iR2 = (*psrc2 >> 16) & 0xff;
		iG2 = (*psrc2 >> 8) & 0xff;
		iB2 = *psrc2 & 0xff;
		// dest is RGB32
		pdst1 = dst + x - 1;
		iInterlaceValue1 = iInterlaceValue2 = 0;

		psrc1 = psrc2;
		psrc2 = psrc3;
		psrc3 = (UINT32 *)((UINT8 *)psrc3 + src_pitch);
		iR0 = iR1; iG0 = iG1; iB0 = iB1;
		iR1 = iR2; iG1 = iG2; iB1 = iB2;
		iR2 = iR3; iG2 = iG3; iB2 = iB3;
		iR3 = (*psrc3 >> 16) & 0xff;
		iG3 = (*psrc3 >> 8) & 0xff;
		iB3 = *psrc3 & 0xff;

		// Calculate the interlace value by checking if the pixel color on previous row differs much
		// from this row and next row differs much (with same sign) from this row.
		// Detect edges by checking so that pixel color on previous row doesn't differ too much from next row.
		// If it does, it's probably just an ordinary edge.
		// 3, 6 and 1 are approximate values for converting R, G, B to intensity.
		iInterlaceValue2 = (3 * ((iR1 - iR2) * (iR3 - iR2) - ((iEdgeDetect * (iR1 - iR3) * (iR1 - iR3)) >> 12)) +
							6 * ((iG1 - iG2) * (iG3 - iG2) - ((iEdgeDetect * (iG1 - iG3) * (iG1 - iG3)) >> 12)) +
							(iB1 - iB2) * (iB3 - iB2) - ((iEdgeDetect * (iB1 - iB3) * (iB1 - iB3)) >> 12));

		for(INT32 y = 1; y <= h; y++)
			{
			psrc1 = psrc2;
			psrc2 = psrc3;
			psrc3 = (UINT32 *)((UINT8 *)psrc3 + src_pitch);
			iR0 = iR1; iG0 = iG1; iB0 = iB1;
			iR1 = iR2; iG1 = iG2; iB1 = iB2;
			iR2 = iR3; iG2 = iG3; iB2 = iB3;
			if(y < hminus)
				{
				iR3 = (*psrc3 >> 16) & 0xff;
				iG3 = (*psrc3 >> 8) & 0xff;
				iB3 = *psrc3 & 0xff;
				}
			else
				{
				iR3 = iR1; iG3 = iG1; iB3 = iB1;
				}
			iInterlaceValue0 = iInterlaceValue1;
			iInterlaceValue1 = iInterlaceValue2;
			if (y < h)
				// Calculate the interlace value by checking if the pixel color on previous row differs much
				// from this row and next row differs much (with same sign) from this row.
				// Detect edges by checking so that pixel color on previous row doesn't differ too much from next row.
				// If it does, it's probably just an ordinary edge.
				// 3, 6 and 1 are approximate values for converting R, G, B to intensity.
				iInterlaceValue2 = (3 * ((iR1 - iR2) * (iR3 - iR2) - ((iEdgeDetect * (iR1 - iR3) * (iR1 - iR3)) >> 12)) +
									6 * ((iG1 - iG2) * (iG3 - iG2) - ((iEdgeDetect * (iG1 - iG3) * (iG1 - iG3)) >> 12)) +
									(iB1 - iB2) * (iB3 - iB2) - ((iEdgeDetect * (iB1 - iB3) * (iB1 - iB3)) >> 12));
			else
				iInterlaceValue2 = 0;

			// New in version 1.1: Get mean interlace value of 3 rows (i.e. 5 pixels are examined).
			// Middle row has twice the weight.

			if(iInterlaceValue0 + 2 * iInterlaceValue1 + iInterlaceValue2 > iThreshold)
				{
				// Blend: Get mean value of previous and next row (weight 0.25) and this row (weight 0.5).
				// Interpolate: Odd lines: Copy from source. Even lines: Get mean value of previous and next row
				if (bBlend)
					{
					*pdst1 = (0xff000000) | (((iR0 + 2 * iR1 + iR2) >> 2) << 16) + (((iG0 + 2 * iG1 + iG2) >> 2) << 8) + ((iB0 + 2 * iB1 + iB2) >> 2);
					}
				else
					{
					if (y % 2)
						{
						*pdst1 = (*psrc1 | 0xff000000);
						}
					else
						*pdst1 = (0xff000000) | ((((iR0 + iR2) >> 1) << 16) + (((iG0 + iG2) >> 1) << 8) + ((iB0 + iB2) >> 1));
					}
				}
			else
				{
				if (b_show_deinterlace)
					*pdst1 = (0xff787878 + (iR1 >> 4 << 16) + (iG1 >> 4 << 8) + (iB1 >> 4));
				else
					{
					*pdst1 = (*psrc1 | 0xff000000);
					}
				}
			pdst1 = (UINT32 *)((UINT8 *)pdst1 + dst_pitch);
		}
	}
}



template <class kernel>
static FINLINE	void loop_24( kernel, UINT8* &srcp_saved, UINT8* &dstp_saved, INT32 src_pitch, INT32 dst_pitch, INT32 order, INT32 w, INT32 h, INT32 s_threshold)
{
	INT32	x, y;

    UINT8 *srcp, *prvp, *prvpp, *prvpn, *prvppp, *prvpnn, *prvp4p, *prvp4n;
	UINT8 *srcpp, *srcppp, *srcpn, *srcpnn, *srcp3p, *srcp3n, *srcp4p, *srcp4n;
	UINT8*	dstp;

	INT32 src_pitch_2 = 2 * src_pitch;
	INT32 diff, diff_prev, diff_next;

	/* For the other field choose adaptively between using the previous field
		or the interpolant from the current field. */
	prvp =   s_prevFrame + 5 * src_pitch - (1 - order) * src_pitch;
	prvpp =  prvp - src_pitch;
	prvppp = prvp - src_pitch_2;
	prvp4p = prvp - 4 * src_pitch;
	prvpn =  prvp + src_pitch;
	prvpnn = prvp + src_pitch_2;
	prvp4n = prvp + 4 * src_pitch;

	srcp =   srcp_saved + 5 * src_pitch - (1 - order) * src_pitch;
	srcpp =  srcp - src_pitch;
	srcppp = srcp - src_pitch_2;
	srcp3p = srcp - 3 * src_pitch;
	srcp4p = srcp - 4 * src_pitch;
	srcpn =  srcp + src_pitch;
	srcpnn = srcp + src_pitch_2;
	srcp3n = srcp + 3 * src_pitch;
	srcp4n = srcp + 4 * src_pitch;

	dstp =   dstp_saved  + 5 * dst_pitch - (1 - order) * dst_pitch;

	for (y = 5 - (1-order); y <= (h - 5 - (1 - order)); y+=2)
		{
		for (x = 0; x < src_pitch; x++)
			{
			//if ((s_threshold == 0) || (ABS((INT32)prvp[x] - (INT32)srcp[x]) > s_threshold ) ||
			//	(ABS((INT32)prvpp[x] - (INT32)srcpp[x]) > s_threshold ) || (ABS((INT32)prvpn[x] - (INT32)srcpn[x]) > s_threshold ))
			//if ( (s_threshold == 0) || (OUTSIDE_ABS((INT32)prvp[x] - (INT32)srcp[x], s_threshold)) ||
			//	(OUTSIDE_ABS((INT32)prvpp[x] - (INT32)srcpp[x], s_threshold)) || (OUTSIDE_ABS((INT32)prvpn[x] - (INT32)srcpn[x], s_threshold ))) 
			diff = (INT32)prvp[x] - (INT32)srcp[x];
			diff_prev = (INT32)prvpp[x] - (INT32)srcpp[x];
			diff_next = (INT32)prvpn[x] - (INT32)srcpn[x];
			if ( (s_threshold == 0) || (OUTSIDE_ABS(diff, s_threshold)) ||
				(OUTSIDE_ABS(diff_prev, s_threshold)) || (OUTSIDE_ABS(diff_next, s_threshold ))) 
				{
				kernel::apply(x, dstp, prvp, prvpp, prvppp, prvpn, prvpnn, prvp4p, prvp4n,
					     srcp, srcpp, srcppp, srcp3p, srcp4p, srcpn, srcpnn, srcp3n, srcp4n);
				dstp++;
				}
			else
				{
				*dstp = srcp[x];
				dstp++;
				}
			// every 3 bytes, add alpha channel to destination
			if (!((x+1) % 3))
				{
				*dstp = 0xff;
				dstp++;
				}
			}
		prvp   += src_pitch_2;
		prvpp  += src_pitch_2;
		prvppp += src_pitch_2;
		prvpn  += src_pitch_2;
		prvpnn += src_pitch_2;
		prvp4p += src_pitch_2;
		prvp4n += src_pitch_2;
		srcp   += src_pitch_2;
		srcpp  += src_pitch_2;
		srcppp += src_pitch_2;
		srcp3p += src_pitch_2;
		srcp4p += src_pitch_2;
		srcpn  += src_pitch_2;
		srcpnn += src_pitch_2;
		srcp3n += src_pitch_2;
		srcp4n += src_pitch_2;
		dstp   += dst_pitch;
		}
}


struct kernel_2_float
{	static FINLINE void	apply( INT32 x, UINT8* &dstp, UINT8* &prvp, UINT8* &prvpp, UINT8* &prvppp, UINT8* &prvpn, UINT8* &prvpnn, UINT8* &prvp4p, UINT8* &prvp4n,
					     UINT8* &srcp, UINT8* &srcpp, UINT8* &srcppp, UINT8* &srcp3p, UINT8* &srcp4p, UINT8* &srcpn, UINT8* &srcpnn, UINT8* &srcp3n, UINT8* &srcp4n )
		{
		double valf = 0.526 * ((INT32)srcpp[x] + (INT32)srcpn[x])
					+ 0.170 * ((INT32)srcp[x] + (INT32)prvp[x])
					- 0.116 * ((INT32)srcppp[x] + (INT32)srcpnn[x] + (INT32)prvppp[x] + (INT32)prvpnn[x])
					- 0.026 * ((INT32)srcp3p[x] + (INT32)srcp3n[x])
					+ 0.031 * ((INT32)srcp4p[x] + (INT32)srcp4n[x] + (INT32)prvp4p[x] + (INT32)prvp4n[x]);
		dstp[x] = CLAMP((INT32) valf, 0, 255);
		}
};

struct kernel_1_float
{	static FINLINE void	apply( INT32 x, UINT8* &dstp, UINT8* &prvp, UINT8* &prvpp, UINT8* &prvppp, UINT8* &prvpn, UINT8* &prvpnn, UINT8* &prvp4p, UINT8* &prvp4n,
					     UINT8* &srcp, UINT8* &srcpp, UINT8* &srcppp, UINT8* &srcp3p, UINT8* &srcp4p, UINT8* &srcpn, UINT8* &srcpnn, UINT8* &srcp3n, UINT8* &srcp4n )
		{
		double valf = 0.526 * ((INT32)srcpp[x] + (INT32)srcpn[x])
					+ 0.170 * ((INT32)prvp[x])
					- 0.116 * ((INT32)prvppp[x] + (INT32)prvpnn[x])
					- 0.026 * ((INT32)srcp3p[x] + (INT32)srcp3n[x])
					+ 0.031 * ((INT32)prvp4p[x] + (INT32)prvp4p[x]);
		dstp[x] = CLAMP((INT32) valf, 0, 255);
		}
};

struct kernel_2_int
{	static FINLINE void	apply( INT32 x, UINT8* &dstp, UINT8* &prvp, UINT8* &prvpp, UINT8* &prvppp, UINT8* &prvpn, UINT8* &prvpnn, UINT8* &prvp4p, UINT8* &prvp4n,
					     UINT8* &srcp, UINT8* &srcpp, UINT8* &srcppp, UINT8* &srcp3p, UINT8* &srcp4p, UINT8* &srcpn, UINT8* &srcpnn, UINT8* &srcp3n, UINT8* &srcp4n )
		{
		INT32 val = ((((INT32)srcpp[x] + (INT32)srcpn[x]) << 3) + (((INT32)srcp[x] + (INT32)prvp[x]) << 1) -
					(INT32)(srcppp[x]) - (INT32)(srcpnn[x]) - (INT32)(prvppp[x]) - (INT32)(prvpnn[x])) >> 4;
		dstp[x] = CLAMP(val, 0, 255);
		}
};

struct kernel_1_int
{	static FINLINE void	apply( INT32 x, UINT8* &dstp, UINT8* &prvp, UINT8* &prvpp, UINT8* &prvppp, UINT8* &prvpn, UINT8* &prvpnn, UINT8* &prvp4p, UINT8* &prvp4n,
					     UINT8* &srcp, UINT8* &srcpp, UINT8* &srcppp, UINT8* &srcp3p, UINT8* &srcp4p, UINT8* &srcpn, UINT8* &srcpnn, UINT8* &srcp3n, UINT8* &srcp4n )
		{
		INT32 val = ( (((INT32)srcpp[x] + (INT32)srcpn[x]) << 3) + (((INT32)prvp[x]) << 1) - (INT32)(prvppp[x]) - (INT32)(prvpnn[x])) >> 4;
		dstp[x] = CLAMP(val, 0, 255);
		}
};

void deinterlace_kernel_24( UINT8* p_src, UINT8* p_dst, INT32 w, INT32 h, INT32 s_threshold, INT32 s_field_order, BOOL b_kernel_sharp, BOOL b_kernel_two_way)
{
    UINT8*	srcp;
	UINT8*	srcp_saved;
    UINT8*	dstp;
	UINT8*	dstp_saved;
 	UINT8*	tmp_dst;
	UINT8*	tmp_src;

	INT32	src_pitch = w * 3;
	INT32	dst_pitch = w * 4;
	INT32	y, z;
	INT32	order;

	order = (s_field_order + 1) & 1;

	srcp = srcp_saved = p_src;
	dstp = dstp_saved = p_dst;

	tbuf_master.add( TBUF_CHANNEL_VIDEO_PROCESS, 2., "video_process");

	// clean dst bitmap, init alpha channel
	memset( (UINT32 *)dstp , 0xffff , w * h * sizeof (UINT32));

	tbuf_master.add( TBUF_CHANNEL_VIDEO_PROCESS, -2., "video_process");
	// copy lines that don't need to be deinterlaced
	srcp = srcp_saved  + (1-order) * src_pitch;
	dstp = dstp_saved  + (1-order) * dst_pitch;
	for (y = 0; y < h; y+=2)
		{
		for (z = w; z > 0; --z)
			{
			memcpy(dstp, srcp, 3);
			srcp += 3;
			dstp += 4;
			}
		srcp += src_pitch;
		dstp += dst_pitch;
		}

	// Copy through the lines that will be missed below.
	tmp_dst = dstp_saved + order * dst_pitch;
	tmp_src = srcp_saved + (1-order) * src_pitch;
	for (z = w; z > 0; --z)
		{
		memcpy(tmp_dst, tmp_src, 3);
		tmp_src += 3;
		tmp_dst += 4;
		}

	tmp_dst = dstp_saved + (2+order) * dst_pitch;
	tmp_src = srcp_saved + (3-order) * src_pitch;
	for (z = w; z > 0; --z)
		{
		memcpy(tmp_dst, tmp_src, 3);
		tmp_src += 3;
		tmp_dst += 4;
		}

	tmp_dst = dstp_saved + (h-2+order) * dst_pitch;
	tmp_src = srcp_saved + (h-1-order) * src_pitch;
	for (z = w; z > 0; --z)
		{
		memcpy(tmp_dst ,tmp_src, 3);
		tmp_src += 3;
		tmp_dst += 4;
		}

	tmp_dst = dstp_saved + (h-4+order) * dst_pitch;
	tmp_src = srcp_saved + (h-3-order) * src_pitch;
	for (z = w; z > 0; --z)
		{
		memcpy(tmp_dst ,tmp_src, 3);
		tmp_src += 3;
		tmp_dst += 4;
		}

	tbuf_master.add( TBUF_CHANNEL_VIDEO_PROCESS, 4., "video_process");

	if ( b_kernel_sharp )
		if ( b_kernel_two_way )
			loop_24<kernel_2_float>( kernel_2_float(), srcp_saved, dstp_saved, src_pitch, dst_pitch, order, w, h, s_threshold );
		else
			loop_24<kernel_1_float>( kernel_1_float(), srcp_saved, dstp_saved, src_pitch, dst_pitch, order, w, h, s_threshold );
	else
		if ( b_kernel_two_way )
			loop_24<kernel_2_int>( kernel_2_int(), srcp_saved, dstp_saved, src_pitch, dst_pitch, order, w, h, s_threshold );
		else
			loop_24<kernel_1_int>( kernel_1_int(), srcp_saved, dstp_saved, src_pitch, dst_pitch, order, w, h, s_threshold );

	tbuf_master.add( TBUF_CHANNEL_VIDEO_PROCESS, -4., "video_process");

	memcpy( (UINT32 *)s_prevFrame, (UINT32 *)p_src, w * h * 3);

}

void deinterlace_smart_24_init(INT32 w, INT32 h)
{
	s_prevFrame = (UINT8*) _aligned_malloc(w * h * 3 * sizeof(UINT8), 16);
	memset(s_prevFrame, 0x0, 3 * w * h * sizeof(UINT8));
	s_saveFrame = new UINT8[w * h * 3];
	memset(s_saveFrame, 0x0, 3 * w * h * sizeof(UINT8));
	s_moving = new UINT8[w * h];
	memset(s_moving, 0x0, w * h * sizeof(UINT8));
	s_fmoving = new UINT8[w * h];
	memset(s_fmoving, 0x0, w * h * sizeof(UINT8));

	// motion map, full resolution
	s_map_full = new INT32[w * h];
	memset(s_map_full, 0x0000, w * h * sizeof(UINT32));

}

void deinterlace_smart_24_deinit()
{
	_aligned_free(s_prevFrame);
	delete s_saveFrame;
	delete s_moving;
	delete s_fmoving;
	delete s_map_full;
}

INT32 deinterlace_smart_24_motion_map_field_colordiff (UINT8* p_src, INT32 s_srcpitch, INT32 w, INT32 h, INT32 s_threshold)
{
UINT8*		src;
UINT8*		srcminus;
UINT8*		srcplus;
UINT8*		moving;
INT32		count = 0;
INT32		x, y;
INT32		p;
INT32		threshold = s_threshold * s_threshold;

	// Clear motion map
	memset(s_moving, 0, w * h * sizeof(UINT8));

	src = p_src + s_srcpitch - 1;
//	src = src + s_srcpitch;
	srcminus = src - s_srcpitch;
	srcplus = src + s_srcpitch;
	moving = s_moving + w;

	for (y = h - 2; y > 0; --y)
		{
		if (y & 2)
//		if (y & 1)
			{
			for (x = w; x > 0; --x)
				{
			// Set the moving flag if the diff exceeds the configured
			// threshold.

				p = *++src; //src[x] & 0xff;
//				pp = ; //*prevValue & 0xff;
//				pn = ; //nextValue & 0xff;
				if ((*++srcminus - p) * (*++srcplus - p) > threshold)
					{
					*moving = 1;
					src += 2;
					srcplus += 2;
					srcminus += 2;
					count++;
					}
				else
					{
					p = *++src; //(src[x] >> 8) & 0xff;
//					pp = *++srcminus; //(prevValue >> 8) & 0xff;
//					pn = *++srcplus; //(nextValue >> 8) & 0xff;
					if ((*++srcminus - p) * (*++srcplus - p) > threshold)
						{
							*moving = 1;
							src++;
							srcplus++;
							srcminus++;
							count++;
						}
					else
						{
						p = *++src;  //(src[x] >> 16) & 0xff;
//						pp = *++srcminus; //(prevValue >> 16) & 0xff;
//						pn = *++srcplus; //(nextValue >> 16) & 0xff;
						if ((*++srcminus - p) * (*++srcplus - p) > threshold)
							{
								*moving = 1;
								count++;
							}
						}
					}
				}
				moving++;
			}
		else
			{
			src += s_srcpitch;
			srcplus += s_srcpitch;
			srcminus += s_srcpitch;
			moving += w;
			}
	}

	return count;
}

INT32 deinterlace_smart_24_motion_map_field_luma (UINT8* p_src, INT32 s_srcpitch, INT32 w, INT32 h, INT32 s_threshold)
{
	UINT8*	src;
	UINT8*	srcminus;
	UINT8*	srcplus;
	UINT8*	moving;
	INT32	count = 0;
	INT32	x, y;
	INT32	r, g, b;
	INT32	threshold, lumap, luman;

	// Clear motion map
	memset(s_moving, 0, w * h * sizeof(UINT8));

	threshold = s_threshold * s_threshold * 256 * 256;
	src = p_src + s_srcpitch - 1;
	srcminus = src - s_srcpitch;
	srcplus = src + s_srcpitch;
	moving = s_moving + w;

	for (y = h - 2; y > 0; --y)
		{
		if (y & 2)
//		if (y & 1)
			{
			for (x = w; x > 0; --x)
				{
				// Set the moving flag if the diff exceeds the configured threshold.
				// Now check field motion.
				b = *++src; //src[x] & 0xff;
				g = *++src; //(src[x] >> 8) & 0xff;
				r = *++src;  //(src[x] >> 16) & 0xff;
				lumap = 30 * (*++srcminus - b) + 150 * (*++srcminus - g) + 76 * (*++srcminus - r); //   (76 * rp + 30 * bp + 150 * gp); // >> 8;
				luman = 30 * (*++srcplus - b) + 150 * (*++srcplus - g) + 76 * (*++srcplus - r); //   (76 * rp + 30 * bp + 150 * gp); // >> 8;
				if ((lumap * luman) > threshold)
					{
					*moving = 1;
					count++;
					}
				moving++;
				}
			}
		else
			{
			src += s_srcpitch;
			srcplus += s_srcpitch;
			srcminus += s_srcpitch;
			moving += w;
			}
		}

	return count;
}

INT32 deinterlace_smart_24_motion_map_framefield_colordiff (UINT8* p_src, INT32 s_srcpitch, INT32 w, INT32 h, INT32 s_threshold)
{
UINT8*		src;
UINT8*		prev;
UINT8*		moving;
INT32		count = 0;
INT32		x, y;
INT32		r, g, b;

	// Clear motion map
	memset(s_moving, 0, w * h * sizeof(UINT8));

	/* Skip first and last lines, they'll get a free ride. */
	src = p_src +s_srcpitch - 1;
	prev = s_prevFrame + s_srcpitch;
	moving = s_moving + w;

	for (y = h - 2; y > 0; --y)
		{
		for (x = w; x > 0; --x)
			{
			// Check frame motion.
			// Set the moving flag if the diff exceeds the configured threshold.
			r = *++src - *++prev;
			g = *++src - *++prev;
			b = *++src - *++prev;
			if ( OUTSIDE_ABS( r, s_threshold) || OUTSIDE_ABS( g, s_threshold) || OUTSIDE_ABS( b, s_threshold) )
				{
				*moving = 1;
				++count;
				}
			++moving;
			}
		}
	return count;
}

INT32 deinterlace_smart_24_motion_map_framefield_luma (UINT8* p_src, INT32 s_srcpitch, INT32 w, INT32 h, INT32 s_threshold)
{
UINT8*		src;
UINT8*		srcminus;
UINT8*		prev;
UINT8*		prevplus;
UINT8*		moving;
INT32		count = 0;
INT32 		threshold = 0;
INT32		x, y;
UINT8		frMotion, fiMotion;

	// Clear motion map
	memset(s_moving, 0 ,w * h * sizeof(UINT8));

	//Ignore first line
	src = p_src + s_srcpitch - 1;
	srcminus = p_src - 1;
	prev = s_prevFrame + s_srcpitch - 1;
	prevplus = prev + w;
	moving = s_moving + w;

	threshold = s_threshold * 256 / 30;

	for (y = h - 2; y > 0; --y)
		{
		for ( x = w; x > 0; --x)
			{
			// First check frame motion.
			// Set the moving flag if the diff exceeds the configured threshold.
			UINT8 s_b, s_r, s_g, p_b, p_g, p_r;
			s_b = *++src;
			s_g = *++src;
			s_r = *++src;

			p_b = *++prev;
			p_g = *++prev;
			p_r = *++prev;
			frMotion = 0;
//			if (OUTSIDE_ABS( (*++src - *++prev) + 5 * (*++src - *++prev) + 2 * (*++src - *++prev), threshold))
			if (OUTSIDE_ABS( (s_b - p_b) + 5 * (s_g - p_g) + 2 * (s_r - p_r), threshold))
				{
				frMotion = 1;
//				*moving = 1;
//				count++;
				}
//			++moving;
//			}

			// Now check field motion

			fiMotion = 0;
			if (y & 1)
				{
				// previous ligne
				p_b = *++srcminus;
				p_g = *++srcminus;
				p_r = *++srcminus;
				prevplus += 3;
				}
			else
				{
				p_b = *++prevplus;
				p_g = *++prevplus;
				p_r = *++prevplus;
				srcminus += 3;
				}

			if (OUTSIDE_ABS( (s_b - p_b) + 5 * (s_g - p_g) + 2 * (s_r - p_r), threshold))
				{
				fiMotion = 1;
				}

			*moving = (fiMotion && frMotion);
			//	Keep a count of the number of moving pixels for the scene change detection.
			if (*moving)
				count++;
			moving++;
			}
		}
	return count;
}


INT32 deinterlace_smart_24_motion_map_frame_colordiff(UINT8* p_src, INT32 s_srcpitch, INT32 w, INT32 h, INT32 s_threshold)
{
UINT8*		src;
UINT8*		prev;
UINT8*		moving;
INT32		count = 0;
INT32		x, y;
INT32		r, g, b;

	// Clear motion map
	memset((UINT32 *)s_moving, 0, (w * h * sizeof(UINT8)));

	/* Skip first and last lines, they'll get a free ride. */
	src = p_src +s_srcpitch - 1;
	prev = s_prevFrame + s_srcpitch; //w;
	moving = s_moving + w;

	for (y = h - 2; y > 0; --y)
		{
		for (x = w; x > 0; --x)
			{
			// First check frame motion.
			// Set the moving flag if the diff exceeds the configured threshold.
			r = *++src - *++prev;
			g = *++src - *++prev;
			b = *++src - *++prev;
			if ( OUTSIDE_ABS( r, s_threshold) || OUTSIDE_ABS( g, s_threshold) || OUTSIDE_ABS( b, s_threshold) )
				{
				*moving = 1;
				++count;
				}
			// increment to next pixel
			++moving;
			}
		}
	return count;
}

INT32 deinterlace_smart_24_motion_map_frame_colordiff_full(UINT8* p_src, INT32 s_srcpitch, INT32 w, INT32 h, INT32 s_threshold)
{
UINT8*		src;
UINT8*		prev;
UINT8*		moving;
INT32*		moving_full;
INT32		count = 0;
INT32		x, y;
INT32		r, g, b;

	// Clear motion map
	memset(s_map_full, 0, w * h * sizeof(UINT32));
	memset(s_moving, 0, w * h * sizeof(UINT8));

	/* Skip first and last lines, they'll get a free ride. */
	src = p_src +s_srcpitch - 1;
	prev = s_prevFrame + s_srcpitch; //w;
	moving = s_moving + w;
	moving_full = s_map_full + w;

	for (y = h - 2; y > 0; --y)
		{
		for (x = w; x > 0; --x)
			{
			// Check frame motion.
			// Set the moving flag if the diff exceeds the configured threshold.
			r = *++src - *++prev;
			g = *++src - *++prev;
			b = *++src - *++prev;
			// save the difference
			*moving_full = (r & 0xff) << 16 | (g & 0xff) << 8 | (b & 0xff);
			if ( OUTSIDE_ABS( r, s_threshold) || OUTSIDE_ABS( g, s_threshold) || OUTSIDE_ABS( b, s_threshold) )
				{
				// use last byte of moving to have a simple test
				*moving = 1;
				++count;
				}
			++moving;
			}
		}
	return count;
}

INT32 deinterlace_smart_24_motion_map_frame_colordiff_sse2(UINT8* p_src, INT32 s_srcpitch, INT32 w, INT32 h, INT32 s_threshold)
{
	UINT8*	moving;
	INT32	count = 0;
    INT32	nLoop = w * h / 16;	// loopcounter

	// union to speed access to SSE2 register
	union __m128__m128
		{
        __m128i m;
        UINT32 p[4];
		} ml;
	union __m128__m128_1
		{
        __m128i m;
        UINT32 p[4];
		} mm;
	union __m128__m128_2
		{
        __m128i m;
        UINT32 p[4];
		} mn;

	// Clear motion map
//	memset((UINT32 *)s_moving, 0, (w * h * sizeof(UINT8) / 4));
	memset((UINT32 *)s_moving, 0, (w * h * sizeof(UINT8)));

	moving = s_moving ;
  
	__m128i pSrc1;	// tmp variable for source
    __m128i* pSrc = (__m128i*) p_src;          // input pointer
    __m128i* pPrev = (__m128i*) s_prevFrame;           // output pointer
	
    __m128i tmp;                              // work variable
    __m128i tmp1;                              // work variable
    __m128i tmp2;                              // work variable

	__m128i m128i_threshold;

    _mm_empty();                            // emms

	// init threshold
	m128i_threshold = _mm_set_epi8(s_threshold, s_threshold, s_threshold, s_threshold, 
									s_threshold, s_threshold, s_threshold, s_threshold,
									s_threshold, s_threshold, s_threshold, s_threshold,
									s_threshold, s_threshold, s_threshold, s_threshold);

    for ( INT32 i = 0; i < nLoop; i++ )
		{
		// copy 16 bytes from image source to temp variable, need to aligned on 16 bytes
		memcpy(&pSrc1, pSrc, 16);
//		tmp = _mm_subs_epi8 (pSrc1 , *pPrev);     // Unsigned subtraction with saturation.
		tmp = _mm_sub_epi8(pSrc1 , *pPrev);     // Unsigned subtraction.
		
		// tmp = *pSrc - *pPrev  for each byte
        tmp = _mm_cmpgt_epi8 (tmp , m128i_threshold);     // Greater than threshold.

		pSrc++;		// move to next 16 bytes
		pPrev++;

		memcpy(&pSrc1, pSrc, 16);

//		tmp1 = _mm_subs_epi8 (pSrc1 , *pPrev);     // Unsigned subtraction with saturation.
		tmp1 = _mm_sub_epi8 (pSrc1 , *pPrev);     // Unsigned subtraction.
        tmp1 = _mm_cmpgt_epi8 (tmp1 , m128i_threshold);     // Greater than threshold.

		pSrc++;		// move to next 16 bytes
		pPrev++;
		memcpy(&pSrc1, pSrc, 16);

//		tmp2 = _mm_subs_epi8 (pSrc1 , *pPrev);     // Unsigned subtraction with saturation.
		tmp2 = _mm_sub_epi8 (pSrc1 , *pPrev);     // Unsigned subtraction.
        tmp2 = _mm_cmpgt_epi8  (tmp2 , m128i_threshold);     // Greater than threshold.

		pSrc++;		// move to next 16 bytes, for next loop
		pPrev++;

		// now analyse tmp, tmp1 and tmp2, 16 pixels
		//define a union to access the individual byte

		ml.m = tmp;
		mm.m = tmp1;
		mn.m = tmp2;

		// pixel 1
		if (ml.p[0] << 8)
			{
			*moving = 1;
			++count;
			}
		moving++;
		// pixel 2
		if ( (ml.p[0] >> 24) || (ml.p[1] << 16) )
			{
			*moving = 1;
			++count;
			}
		moving++;
		// pixel 3
		if ( (ml.p[1] >> 16) || (ml.p[2] << 24) )
			{
			*moving = 1;
			++count;
			}
		moving++;
		// pixel 4
		if ( (ml.p[2] >> 8) )
			{
			*moving = 1;
			++count;
			}
		moving++;
		// pixel 5
		if (ml.p[3] << 8)
			{
			*moving = 1;
			++count;
			}
		moving++;
		// pixel 6
		if ( (ml.p[3] >> 24) || (mm.p[0] << 16) )
			{
			*moving = 1;
			++count;
			}
		moving++;
		// pixel 7
		if ( (mm.p[0] >> 16) || (mm.p[1] << 24) )
			{
			*moving = 1;
			++count;
			}
		moving++;
		// pixel 8
		if ( (mm.p[1] >> 8) )
			{
			*moving = 1;
			++count;
			}
		moving++;
		// pixel 9
		if (mm.p[2] << 8)
			{
			*moving = 1;
			++count;
			}
		moving++;
		// pixel 10
		if ( (mm.p[2] >> 24 ) || (mm.p[3] << 16) )
			{
			*moving = 1;
			++count;
			}
		moving++;
		// pixel 11
		if ( (mm.p[3] >> 16) || (mn.p[0] << 24) )
			{
			*moving = 1;
			++count;
			}
		moving++;
		// pixel 12
		if ( (mn.p[0] >> 8) )
			{
			*moving = 1;
			++count;
			}
		moving++;
		// pixel 13
		if (mn.p[1] << 8)
			{
			*moving = 1;
			++count;
			}
		moving++;
		// pixel 14
		if ( (mn.p[1] >> 24) || (mn.p[2] << 16) )
			{
			*moving = 1;
			++count;
			}
		moving++;
		// pixel 15
		if ( (mn.p[2] >> 16) || (mn.p[3] << 24) )
			{
			*moving = 1;
			++count;
			}
		moving++;
		// pixel 16
		if ( (mn.p[3] >> 8) )
			{
			*moving = 1;
			++count;
			}
		moving++;

		//if ( (ml.p[0] & 0x00ffffff) == 0x00ffffff )
		//	{
		//	tmp4 = ml.p[0];
		//	tmp4 = (tmp4 << 8);
		//	tmp4 = (tmp4 >> 8);
		//	if (tmp4)
		//		tmp4 = 8;
		//	*moving = 1;
		//	++count;
		//	}
		//moving++;
		//if ( ((ml.p[0] & 0xff000000) == 0xff000000) || ((ml.p[1] & 0x0000ffff) == 0x0000ffff ) )
		//	{
		//	*moving = 1;
		//	++count;
		//	}
		//moving++;
		//if ( ((ml.p[1] & 0xffff0000) == 0xffff0000) || ((ml.p[2] & 0x000000ff) == 0x000000ff ) )
		//	{
		//	*moving = 1;
		//	++count;
		//	}
		//moving++;
		//if ( ((ml.p[2] & 0xffffff00) == 0xffffff00) )
		//	{
		//	*moving = 1;
		//	++count;
		//	}
		//moving++;
		//if ( (ml.p[3] & 0x00ffffff) == 0x00ffffff )
		//	{
		//	*moving = 1;
		//	++count;
		//	}
		//moving++;

		//if ( ((ml.p[3] & 0xff000000) == 0xff000000) || ((mm.p[0] & 0x0000ffff) == 0x0000ffff ) )
		//	{
		//	*moving = 1;
		//	++count;
		//	}
		//moving++;

		//if ( ((mm.p[0] & 0xffff0000) == 0xffff0000) || ((mm.p[1] & 0x000000ff) == 0x000000ff ) )
		//	{
		//	*moving = 1;
		//	++count;
		//	}
		//moving++;
		//if ( ((mm.p[1] & 0xffffff00) == 0xffffff00) )
		//	{
		//	*moving = 1;
		//	++count;
		//	}
		//moving++;
		//if ( (mm.p[2] & 0x00ffffff) == 0x00ffffff )
		//	{
		//	*moving = 1;
		//	++count;
		//	}
		//moving++;
		//if ( ((mm.p[2] & 0xff000000) == 0xff000000) || ((mm.p[3] & 0x0000ffff) == 0x0000ffff ) )
		//	{
		//	*moving = 1;
		//	++count;
		//	}
		//moving++;
		//if ( ((mm.p[3] & 0xffff0000) == 0xffff0000) || ((mn.p[0] & 0x000000ff) == 0x000000ff ) )
		//	{
		//	*moving = 1;
		//	++count;
		//	}
		//moving++;
		//if ( ((mn.p[0] & 0xffffff00) == 0xffffff00) )
		//	{
		//	*moving = 1;
		//	++count;
		//	}
		//moving++;
		//if ( (mn.p[1] & 0x00ffffff) == 0x00ffffff )
		//	{
		//	*moving = 1;
		//	++count;
		//	}
		//moving++;
		//if ( ((mn.p[1] & 0xff000000) == 0xff000000) || ((mn.p[2] & 0x0000ffff) == 0x0000ffff ) )
		//	{
		//	*moving = 1;
		//	++count;
		//	}
		//moving++;
		//if ( ((mn.p[2] & 0xffff0000) == 0xffff0000) || ((mn.p[3] & 0x000000ff) == 0x000000ff ) )
		//	{
		//	*moving = 1;
		//	++count;
		//	}
		//moving++;
		//if ( ((mn.p[3] & 0xffffff00) == 0xffffff00) )
		//	{
		//	*moving = 1;
		//	++count;
		//	}
		//moving++;
		}
    _mm_empty();                            // exit emms, so it cqn be use by floating point

	// return the number of pixel that loved between 2 frames
	return count;
}


INT32 deinterlace_smart_24_motion_map_frame_luma(UINT8* p_src, INT32 s_srcpitch, INT32 w, INT32 h, INT32 s_threshold)
{
	UINT8*	src;
	UINT8*	prev;
	INT32 	threshold = 0;
	UINT8*	moving;
	INT32	count = 0;
	INT32	x, y;
	
	// Clear motion map
	memset(s_moving, 0 , w * h * sizeof(UINT8));

	//Ignore first line
	src = p_src + s_srcpitch - 1;
	prev = s_prevFrame + s_srcpitch - 1; //w;
	moving = s_moving + w;

	threshold = s_threshold * 256 / 30;

	for (y = h - 2; y > 0; --y)
		{
		for ( x = w; x > 0; --x)
			{
			// First check frame motion.
			// Set the moving flag if the diff exceeds the configured
			// threshold.

//			if (OUTSIDE_ABS( 30 * (*++src - *++prev) + 150 * (*++src - *++prev) + 76 * (*++src - *++prev), threshold))
			if (OUTSIDE_ABS( (*++src - *++prev) + 5 * (*++src - *++prev) + 2 * (*++src - *++prev), threshold))
				{
				*moving = 1;
				count++;
				}
			++moving;
			}
		}
	return count;
}

INT32 deinterlace_smart_24_motion_map_frame_luma_full(UINT8* p_src, INT32 s_srcpitch, INT32 w, INT32 h, INT32 s_threshold)
{
	UINT8*	src;
	UINT8*	prev;
	INT32 	threshold = 0;
	UINT8*	moving;
	INT32*	moving_full;
	INT32	count = 0;
	INT32	x, y;
	
	// Clear motion map
	memset(s_map_full, 0x0000 , w * h * sizeof(UINT32));
	memset(s_moving, 0x0000 , w * h * sizeof(UINT8));

	//Ignore first line
	src = p_src + s_srcpitch - 1;
	prev = s_prevFrame + s_srcpitch - 1; //w;
	moving = s_moving + w;
	moving_full = s_map_full + w;

	threshold = s_threshold * 256 / 30;

	for (y = h - 2; y > 0; --y)
		{
		for ( x = w; x > 0; --x)
			{
			// First check frame motion.
			// Set the moving flag if the diff exceeds the configured threshold.

			// approximate value for luma
			*moving_full = (*++src - *++prev) + 5 * (*++src - *++prev) + 2 * (*++src - *++prev);
//			if (OUTSIDE_ABS( 30 * (*++src - *++prev) + 150 * (*++src - *++prev) + 76 * (*++src - *++prev), threshold))
			if (OUTSIDE_ABS(*moving_full,threshold))
				{
				// use last byte to save the test
				*moving = 1;
				count++;
				}
			++moving;
			++moving_full;
			}
		}
	return count;
}

void deinterlace_smart_24_motion_map_denoise(INT32 w, INT32 h)
{
	INT32	x, y;
	UINT8*	fmoving;
	UINT8*	fmoving2;
	INT32	u, v;

	INT32	wminus1 = w - 1;
	INT32	hminus1 = h - 1;

	INT32	xlo, xhi, ylo, yhi;
	INT32	N = 5;
	INT32	Nover2 = N/2;
	INT32	sum;
	UINT8	*m;

	// Erode.
	fmoving = s_fmoving;
	for (y = h; y > 0; --y)
		{
		fmoving2 = s_moving + y * w;
		for (x = w; x > 0; --x)
			{
			if (!(fmoving2[x]))
				{
				fmoving[x] = 0;	
				continue;
				}
			xlo = x - Nover2; if (xlo < 0) xlo = 0;
			xhi = x + Nover2; if (xhi >= w) xhi = wminus1;
			ylo = y - Nover2; if (ylo < 0) ylo = 0;
			yhi = y + Nover2; if (yhi >= h) yhi = hminus1;
			m = s_moving + ylo * w;
			sum = 0;
			for (u = ylo; u <= yhi; u++)
				{
				for (v = xlo; v <= xhi; v++)
					{
					sum += m[v];
					}
				m += w;
				}
			if (sum > 9)
				fmoving[x] = 1;
			else
				fmoving[x] = 0;
			}
		fmoving += w;
		}
	// Dilate.
	N = 5;
	Nover2 = N/2;
	fmoving = s_moving;
	for (y = 0; y < h; y++)
		{
		fmoving2 = s_fmoving + y * w;

		for (x = 0; x < w; x++)
			{
			if (!fmoving2[x])
				{
				fmoving[x] = 0;	
				continue;
				}
			xlo = x - Nover2; if (xlo < 0) xlo = 0;
			xhi = x + Nover2; if (xhi >= w) xhi = wminus1;
			ylo = y - Nover2; if (ylo < 0) ylo = 0;
			yhi = y + Nover2; if (yhi >= h) yhi = hminus1;
			m = s_moving + ylo * w;
			for (u = ylo; u <= yhi; u++)
				{
				for (v = xlo; v <= xhi; v++)
					{
					m[v] = 1;
					}
				m += w;
				}
			}
		fmoving += w;
		}
}


void deinterlace_smart_24_cubic(UINT8* p_src, UINT8* p_dst, INT32 s_srcpitch, INT32 w, INT32 h, INT32 s_scenechange, BOOL b_show_deinterlace)
{
	UINT8*	src;
	UINT8*	dst;
	UINT8*	srcminus;
	UINT8*	srcminusminus;
	UINT8*	srcplus;
	UINT8*	srcplusplus;
	UINT8*	moving;
	UINT8*	movingminus;
	UINT8*	movingplus;
	INT32	x, y;

	INT32	hminus3 = h - 3;

	src = p_src;
	dst = p_dst;

	// The first line gets a free ride.
	// First line copy
	for(x = w; x > 0; --x)
		{
		memcpy(dst, src, 3);
		src += 3;
		dst += 4;
		}

	srcminus = src - s_srcpitch;
	srcplus = src + s_srcpitch;
	srcminusminus = src - 3 * s_srcpitch;
	srcplusplus = src + 3 * s_srcpitch;

	moving = s_moving + w;
	movingminus = moving - w;
	movingplus = moving + w;

	for (y = h - 2; y > 0; --y)
		{
		if (y&1)
			{
			for (x  = w; x > 0; --x)
				{
				if (!(*movingminus | *moving | *movingplus) && !s_scenechange)
					{
					memcpy( dst, src, 3);
					src += 3;
					dst += 4;
					srcminusminus += 3;
					srcminus += 3;
					srcplus += 3;
					srcplusplus += 3;
					}
				else
					{
					// to do check bounds
					if ((y > 2) && (y < hminus3))
						{
						// blue channel
						*dst++ = CLAMP((5 * (*srcminus++ + *srcplus++) - (*srcminusminus++ + *srcplusplus++)) >> 3, 0, 255)  ;
						// green channel
						*dst++ = CLAMP((5 * (*srcminus++ + *srcplus++) - (*srcminusminus++ + *srcplusplus++)) >> 3, 0, 255);
						// red channel
						*dst++ = CLAMP((5 * (*srcminus++ + *srcplus++) - (*srcminusminus++ + *srcplusplus++)) >> 3, 0, 255);
						dst++; // = 0xff;
						src += 3;
						}
					else
						{
							*dst++ = ((*srcminus++ & 0xfe) + (*srcplus++ & 0xfe)) >> 1;  
							*dst++ = ((*srcminus++ & 0xfe) + (*srcplus++ & 0xfe)) >> 1;  
							*dst++ = ((*srcminus++ & 0xfe) + (*srcplus++ & 0xfe)) >> 1; 
							*dst++ = 0; // = 0xff;
							srcminusminus += 3;
							srcplusplus += 3;
							src += 3;
						}
					}
				moving++;
				movingminus++;
				movingplus++;
				}
			}
		else
			{
			// Even line; pass it through.
			for(x = w; x > 0; --x)
				{
				memcpy(dst, src, 3);
				dst += 4;
				src += 3;
				}
			srcminusminus += s_srcpitch;
			srcminus += s_srcpitch;
			srcplus += s_srcpitch;
			srcplusplus += s_srcpitch;
			
			moving += w;
			movingminus += w;
			movingplus += w;
			}
		}
	// The last line gets a free ride.
	for(x = w; x > 0; --x)
	{
		memcpy(dst, src, 3);
		src += 3;
		dst += 4;
	}
}

void deinterlace_smart_24_cubic32(UINT8* p_src, UINT8* p_dst, INT32 s_srcpitch, INT32 w, INT32 h, INT32 s_scenechange, BOOL b_show_deinterlace)
{
	UINT32*	src;
	UINT32*	dst;
	UINT32*	srcminus;
	UINT32*	srcminusminus;
	UINT32*	srcplus;
	UINT32*	srcplusplus;
	UINT8*	moving;
	UINT8*	movingminus;
	UINT8*	movingplus;
	INT32	x;
	INT32	y;

	INT32	blue, green, red;
	INT32	hminus3 = h - 3;

	src = (UINT32 *)p_src;
	dst = (UINT32 *)p_dst;

	srcminus = src;

	// The first line gets a free ride.
	// First line copy
	for(x = w; x > 0; --x)
		{
		*dst = *src; 
		*dst |= 0xff000000; 
		dst++;
		src = (UINT32 *)((UINT8 *)src + 3);
		}

	srcplus = (UINT32 *)((UINT8 *)src + s_srcpitch);
	srcminusminus = (UINT32 *)((UINT8 *)src - 3 * s_srcpitch);
	srcplusplus = (UINT32 *)((UINT8 *)src + 3 * s_srcpitch);

	moving = s_moving + w;
	movingminus = moving - w;
	movingplus = moving + w;

	for (y = h - 2; y > 0; --y)
		{
		if (y & 1)
			{
			for (x = w; x > 0; --x)
				{
				if (!(*movingminus | *moving | *movingplus) && !s_scenechange)
					{
					*dst++ = *src | 0xff000000; 
					}
				else
					{
					// to do check bounds
					if ((y > 2) && (y < hminus3))
						{
						// blue channel
						blue = (5 * ((*srcminus & 0xff) + (*srcplus & 0xff)) - ((*srcminusminus & 0xff) + (*srcplusplus & 0xff))) >> 3;
						// green channel
						green = (5 * (((*srcminus >> 8) & 0xff) + ((*srcplus >> 8) & 0xff)) - (((*srcminusminus >> 8) & 0xff) + ((*srcplusplus >> 8) & 0xff))) >> 3;
						// red channel
						red = (5 * (((*srcminus >> 16) & 0xff) + ((*srcplus >> 16) & 0xff)) - (((*srcminusminus >> 16) & 0xff) + ((*srcplusplus >> 16) & 0xff))) >> 3;

						green = CLAMP(green, 0, 255);
						blue = CLAMP(blue, 0, 255);
						red = CLAMP(red, 0, 255);

						*dst = ((red << 16) | (green << 8) | blue );
						*dst |= 0xff000000;
						dst++;
						}
					else
						{
						blue = ((*srcminus & 0xff) + (*srcplus & 0xff));
						// green channel
						green = ((*srcminus >> 8) & 0xff) + ((*srcplus >> 8) & 0xff);
						// red channel
						red = ((*srcminus >> 16) & 0xff) + ((*srcplus >> 16) & 0xff);

						blue = CLAMP(blue >> 1, 0, 255);
						green = CLAMP(green >> 1, 0, 255);
						red = CLAMP(red >> 1, 0, 255);

						*dst = ((red << 16) | (green << 8) | blue );
						*dst |= 0xff000000;
						dst++;
						}
					}
				src = (UINT32 *)((UINT8 *)src + 3);
				srcminus = (UINT32 *)((UINT8 *)srcminus + 3);
				srcplus = (UINT32 *)((UINT8 *)srcplus + 3);
				srcminusminus = (UINT32 *)((UINT8 *)srcminusminus + 3);
				srcplusplus = (UINT32 *)((UINT8 *)srcplusplus + 3);

				moving++;
				movingminus++;
				movingplus++;
				}
			}
		else
			{
			// Even line; pass it through.
			for(x = w; x > 0; --x)
				{
				*dst = *src; 
				*dst |= 0xff000000; 
				dst++;
				src = (UINT32 *)((UINT8 *)src + 3);
				}
			srcminus = (UINT32 *)((UINT8 *)srcminus + s_srcpitch);
			srcplus = (UINT32 *)((UINT8 *)srcplus + s_srcpitch);
			srcminusminus = (UINT32 *)((UINT8 *)srcminusminus + s_srcpitch);
			srcplusplus = (UINT32 *)((UINT8 *)srcplusplus + s_srcpitch);
			
			moving += w;
			movingminus += w;
			movingplus += w;
			}
		}

	// The last line gets a free ride.
	for(x=w; x > 0; --x)
		{
		*dst++ = *src | 0xff000000; 
		src = (UINT32 *)((UINT8 *)src + 3);
		}
}
void deinterlace_smart_24_ela_32(UINT8* p_src, UINT8* p_dst, INT32 s_srcpitch, INT32 w, INT32 h, INT32 s_scenechange, BOOL b_show_deinterlace)
{
	UINT32*	src;
	UINT32*	dstp;
	UINT32*	srcminus;
	UINT32*	srcplus;
	UINT8*	moving;
	UINT8*	movingminus;
	UINT8*	movingplus;
	INT32	x, y;
	 
	INT32	val1, val2, val3, val4, val5;

	src = (UINT32 *)p_src;
	dstp = (UINT32 *)p_dst;


	// The first line gets a free ride.
	// First line copy
	for(x = w; x > 0; --x)
		{
		memcpy(dstp, src, 4);
		src++;
		dstp++;
		}

	srcminus = src - w;
	srcplus = src + w;

	moving = s_moving + w;
	movingminus = moving - w;
	movingplus = moving + w;

	for (y = h - 2; y > 0; --y)
		{
		// first pixel, can't do ELA
		// average pixel from previous and next line
		if (!(*moving) && !s_scenechange)
			{
			dstp[0] = src[0];
			}
		else
			dstp[0] = ((srcminus[0] & 0x00ffffff) + (srcplus[0] & 0x00ffffff)) >> 1;
		//alpha channel
		dstp[0] |= 0xff000000;

		moving++;
		movingminus++;
		movingplus++;

		// second pixel, can't do ELA
		if (!(*moving) && !s_scenechange)
			{
			dstp[1] = src[1];
			}
		else
			{
			/* Edge Line Average */
			val1 = ( srcminus[1] & 0x00ffffff + srcplus[1] & 0x00ffffff );
			val2 = ( srcminus[0] & 0x00ffffff + srcplus[2] & 0x00ffffff );
			val3 = ( srcminus[2] & 0x00ffffff + srcplus[0] & 0x00ffffff );
			dstp[1] = ( MIN(val1,val2,val3) ) >> 1;
			}
		//alpha channel
		dstp[1] |= 0xff000000;

		moving++;
		movingminus++;
		movingplus++;

		for (x = 2; x < w - 2; x++)
			{
			if (!(*moving) && !s_scenechange)
				{
				dstp[x] = src[x];
				}
			else
				{
				/* Edge Line Average */
				val1 = ( srcminus[x] & 0x00ffffff   + srcplus[x] & 0x00ffffff );// >> 1;
				val2 = ( srcminus[x-1] & 0x00ffffff + srcplus[x+1] & 0x00ffffff );// >> 1;
				val3 = ( srcminus[x+1] & 0x00ffffff + srcplus[x-1] & 0x00ffffff );// >> 1;
				val4 = ( srcminus[x-2] & 0x00ffffff + srcplus[x+2] & 0x00ffffff );// >> 1;
				val5 = ( srcminus[x+2] & 0x00ffffff + srcplus[x-2] & 0x00ffffff );// >> 1;

				dstp[x] = ( MIN(MIN(val1,val2,val3),val4, val5) ) >> 1;
				}
			//alpha channel
			dstp[x] |= 0xff000000;
			moving++;
			movingminus++;
			movingplus++;
			} 

		// 2 last pixel, can't do ELA
		if (!(*moving) && !s_scenechange)
			{
			dstp[w-2] = src[w-2];
			}
		else
			{
			/* Edge Line Average */
			val1 = ( srcminus[w-2] & 0x00ffffff + srcplus[w-2] & 0x00ffffff );
			val2 = ( srcminus[w-3] & 0x00ffffff + srcplus[w] & 0x00ffffff );
			val3 = ( srcminus[w] & 0x00ffffff + srcplus[w-3] & 0x00ffffff );
			dstp[w-2] = ( MIN(val1,val2,val3) ) >> 1;
			}
		//alpha channel
		dstp[w-2] |= 0xff000000;

		moving++;
		movingminus++;
		movingplus++;

		// last pixel, can't do ELA
		if (!(*moving) && !s_scenechange)
			{
			dstp[w-1] = src[0];
			}
		else
			dstp[w-1] = ((srcminus[w-1] & 0x00ffffff) + (srcplus[w-1] & 0x00ffffff)) >> 1;
		//alpha channel
		dstp[w-1] |= 0xff000000;

		moving++;
		movingminus++;
		movingplus++;

		src += w;
		srcminus += w;
		srcplus += w;
		}

	// The last line gets a free ride.
	for(x=w; x > 0; --x)
		{
		memcpy(dstp, src, 4);
		src++;
		dstp++;
		}
}


void deinterlace_smart_24_blend(UINT8* p_src, UINT8* p_dst, INT32 s_srcpitch, INT32 w, INT32 h, INT32 s_scenechange, BOOL b_show_deinterlace)
{
	UINT8*	src;
	UINT8*	dst;
	UINT8*	srcminus;
	UINT8*	srcplus;
	UINT8*	moving;
	UINT8*	movingminus;
	UINT8*	movingplus;
	INT32	x, y;
	 
	src = p_src;
	dst = p_dst;

	// The first line gets a free ride.
	// First line copy
	for(x = w; x > 0; --x)
		{
		memcpy(dst, src, 3);
		src += 3;
		dst += 4;
		}

	srcminus = src - s_srcpitch;
	srcplus = src + s_srcpitch;

	moving = s_moving + w;
	movingminus = moving - w;
	movingplus = moving + w;

	for (y = h - 2; y > 0; --y)
		{
		for (x = w; x > 0; --x)
			{
			if (!(*movingminus | *moving | *movingplus) && !s_scenechange)
				{
				memcpy( dst, src, 3);
				dst += 4;
				src += 3;
				srcminus += 3;
				srcplus += 3;
				}
			else
				{
				/* Blend fields. */
				*dst++ = (((*src++ & 0xfe) * 2) + (*srcminus++ & 0xfc) + (*srcplus++ & 0xfc)) >> 2;
				*dst++ = (((*src++ & 0xfe) * 2) + (*srcminus++ & 0xfc) + (*srcplus++ & 0xfc)) >> 2;
				*dst++ = (((*src++ & 0xfe) * 2) + (*srcminus++ & 0xfc) + (*srcplus++ & 0xfc)) >> 2;
				dst++;
				}
			moving++;
			movingminus++;
			movingplus++;
			} 
		}
	// The last line gets a free ride.
	for(x=w; x > 0; --x)
		{
		memcpy(dst, src, 3);
		src += 3;
		dst += 4;
		}
}

void deinterlace_smart_24_copy32(UINT8* p_src, UINT8* p_dst, INT32 s_srcpitch, INT32 w, INT32 h, INT32 s_scenechange, BOOL b_show_deinterlace)
{
	UINT32*	src;
	UINT32*	dst;
	UINT32*	srcminus;
	UINT8*	moving;
	UINT8*	movingminus;
	UINT8*	movingplus;
	INT32	x, y;
	 
	src = (UINT32 *)p_src;
	dst = (UINT32 *)p_dst;

	srcminus = src;

	// The first line gets a free ride.
	// First line copy
	if (b_show_deinterlace)
		for(x = w; x > 0; --x)
			{
			*dst++ = *src & 0x00ffffff; 
			src = (UINT32 *)((UINT8 *)src + 3);
			}
	else
		for(x = w; x > 0; --x)
			{
			*dst++ = *src | 0xff000000;
			src = (UINT32 *)((UINT8 *)src + 3);
			}

	moving = s_moving + w;
	movingminus = moving - w;
	movingplus = moving + w;

	for (y = h - 2; y > 0; --y)
		{
		if (y&1)
			{
			for (x = w; x > 0; --x)
				{
				if (!(*movingminus | *moving | *movingplus) && !s_scenechange)
					{
					*dst = *src;
					if (b_show_deinterlace)
						*dst &= 0x00ffffff;
					else
						*dst |= 0xff000000;
					dst++;
					}
				else
					{
					/* Copy fields. */
					*dst = *srcminus; 
					*dst++ |= 0xff000000; 
					}
				src = (UINT32 *)((UINT8 *)src + 3);
				srcminus = (UINT32 *)((UINT8 *)srcminus + 3);
				moving++;
				movingplus++;
				movingminus++;
				} 
			}
		else
			{
			// Even line; pass it through.
			if (b_show_deinterlace)
				for(x = w; x > 0; --x)
					{
					*dst++ = *src & 0x00ffffff; 
					src = (UINT32 *)((UINT8 *)src + 3);
					}
			else
				for(x = w; x > 0; --x)
					{
					*dst = *src | 0xff000000;
					src = (UINT32 *)((UINT8 *)src + 3);
					dst++;
					}

			srcminus = (UINT32 *)((UINT8 *)srcminus + s_srcpitch);			
			moving += w;
			movingminus += w;
			movingplus += w;
			}
		}
	// The last line gets a free ride.
	if (b_show_deinterlace)
		for(x = w; x > 0; --x)
			{
			*dst++ = *src & 0x00ffffff;
			src = (UINT32 *)((UINT8 *)src + 3);
			}
	else
		for(x = w; x > 0; --x)
			{
			*dst++ = *src | 0xff000000;
			src = (UINT32 *)((UINT8 *)src + 3);
			}
}


void deinterlace_smart_24_copy(UINT8* p_src, UINT8* p_dst, INT32 s_srcpitch, INT32 w, INT32 h, INT32 s_scenechange, BOOL b_show_deinterlace)
{
	UINT8*	src;
	UINT8*	dst;
	UINT8*	srcminus;
	UINT8*	moving;
	UINT8*	movingminus;
	UINT8*	movingplus;
	INT32	x, y;
	 
	src = p_src;
	dst = p_dst;

	// The first line gets a free ride.
	// First line copy

	for(x = w; x > 0; --x)
		{
		memcpy(dst, src, 3);
		src += 3;
		dst += 4;
		}

	srcminus = src - s_srcpitch;

	moving = s_moving + w;
	movingminus = moving - w;
	movingplus = moving + w;

	for (y = h - 2; y > 0; --y)
		{
		if (y&1)
			{
			for (x = w; x > 0; --x)
				{
//				if (!(*movingminus | *moving | *movingplus) && !s_scenechange)
				if (!(*moving) && !s_scenechange)
					{
					memcpy( dst, src, 3);
					dst += 4;
//					dst += 3;
//					*dst++ = 0; //0xff;
					src += 3;
					srcminus += 3;
					}
				else
					{
					/* Copy fields. */
					memcpy ( dst, srcminus, 3);
					dst += 3;
					*dst = 0xff;
					dst++;
					src += 3;
					srcminus += 3;

					}
				moving++;
				movingplus++;
				movingminus++;
				} 
			}
		else
			{
			// Even line; pass it through.
			for(x = w; x > 0; --x)
				{
				memcpy(dst, src, 3);
				dst += 4;
				src += 3;
				}
			srcminus += s_srcpitch;
			
			moving += w;
			movingminus += w;
			movingplus += w;
			}
		}
	// The last line gets a free ride.
	for(x=w; x > 0; --x)
	{
		memcpy(dst, src, 3);
		src += 3;
		dst += 4;
	}
}

void deinterlace_smart_24_copy_full(UINT8* p_src, UINT8* p_dst, INT32 s_srcpitch, INT32 w, INT32 h, INT32 s_scenechange, BOOL b_show_deinterlace)
{
	UINT8*	src;
	UINT8*	dst;
	UINT8*	srcminus;
	UINT8*	moving;
	UINT8*	movingminus;
	UINT8*	movingplus;
	INT32	x, y;
	 
	src = p_src;
	dst = p_dst;

	// The first line gets a free ride.
	// First line copy

	for(x = w; x > 0; --x)
		{
		memcpy(dst, src, 3);
		src += 3;
		dst += 4;
		}

	srcminus = src - s_srcpitch;

	moving = s_moving + w;
	movingminus = moving - w;
	movingplus = moving + w;

	for (y = h - 2; y > 0; --y)
		{
		if (y&1)
			{
			for (x = w; x > 0; --x)
				{
				if (!(*movingminus | *moving | *movingplus) && !s_scenechange)
//				if (!(*moving & 0xff000000) && !s_scenechange)
					{
					memcpy( dst, src, 3);
					dst += 4;
					src += 3;
					srcminus += 3;
					}
				else
					{
					/* Copy fields. */
					memcpy ( dst, srcminus, 3);
					dst += 3;
					*dst = 0xff;
					dst++;
					src += 3;
					srcminus += 3;

					}
				moving++;
				movingplus++;
				movingminus++;
				} 
			}
		else
			{
			// Even line; pass it through.
			for(x = w; x > 0; --x)
				{
				memcpy(dst, src, 3);
				dst += 4;
				src += 3;
				}
			srcminus += s_srcpitch;			
			moving += w;
			movingminus += w;
			movingplus += w;
			}
		}
	// The last line gets a free ride.
	for(x=w; x > 0; --x)
		{
		memcpy(dst, src, 3);
		src += 3;
		dst += 4;
		}
}


UINT32 deinterlace_smart_24(UINT8* p_src, UINT8* p_dst, INT32 w, INT32 h, UINT32 n, INT32 b_threshold, INT32 b_scene_threshold, BOOL b_highquality, INT32 s_deinterlace_blend, BOOL b_colordiff, INT32 s_diff_mode, BOOL b_use_32, INT32 s_simd, BOOL b_show_deinterlace, BOOL d_motionmap_full)
{
	UINT8*		src;
	UINT8*		dst;
	INT32 		threshold = 0;
	INT32		count;
	INT32		srcpitch = w * 3;
	INT32		scenechange;

	src = p_src;
	dst = p_dst;


	tbuf_master.add( TBUF_CHANNEL_VIDEO_PROCESS, 1., "video_process");

	// set alpha channel for deinterlaced bitmap
	if (b_show_deinterlace)
		if (b_use_32)
			memset( (UINT32 *)dst , 0x00000000 , w * h * sizeof (UINT32)); 
		else
			memset( dst , 0x00000000 , w * h * sizeof (UINT32));
	else
		if (b_use_32)
			memset( (UINT32 *)dst , 0xffffffff , w * h * sizeof (UINT32)); 
		else
			memset( dst , 0xffffffff , w * h * sizeof (UINT32));

	if (h < 2) return 0;

	tbuf_master.add( TBUF_CHANNEL_VIDEO_PROCESS, 2., "video_process");
	count = 0;
	if (s_diff_mode == FRAME_ONLY) // || s_diff_mode == FRAME_AND_FIELD)
		{
		if (b_colordiff)
			{
			switch (s_simd)
				{
				case SIMD_NONE:
				case SIMD_MMX:
					if (d_motionmap_full)
						count = deinterlace_smart_24_motion_map_frame_colordiff_full(src, srcpitch, w, h, b_threshold );
					else
						count = deinterlace_smart_24_motion_map_frame_colordiff(src, srcpitch, w, h, b_threshold );
					break;
				case SIMD_SSE2:
					count = deinterlace_smart_24_motion_map_frame_colordiff_sse2(src, srcpitch, w, h, b_threshold );
				}
			}
		else
			if (d_motionmap_full)
				count = deinterlace_smart_24_motion_map_frame_luma_full(src, srcpitch, w, h, b_threshold );
			else
				count = deinterlace_smart_24_motion_map_frame_luma(src, srcpitch, w, h, b_threshold );
		}
	else if (s_diff_mode == FIELD_ONLY)
		{
		/* Field differencing only mode. */
		if (b_colordiff)
			count = deinterlace_smart_24_motion_map_field_colordiff(src, srcpitch, w, h, b_threshold );
		else
			count = deinterlace_smart_24_motion_map_field_luma(src, srcpitch, w, h, b_threshold );
		}
	else
		{
		// Frame and Field differencing
		if (b_colordiff)
			count = deinterlace_smart_24_motion_map_framefield_colordiff(src, srcpitch, w, h, b_threshold );
		else
			count = deinterlace_smart_24_motion_map_framefield_luma(src, srcpitch, w, h, b_threshold );
		}

	/* Determine whether a scene change has occurred. */
	if ((100L * count) / (h * w) >= b_scene_threshold)
		scenechange = 1;
	else scenechange = 0;

	tbuf_master.add( TBUF_CHANNEL_VIDEO_PROCESS, 4., NULL);

	/* Perform a denoising of the motion map if enabled. */
	if (!scenechange && b_highquality)
	{
		deinterlace_smart_24_motion_map_denoise(w, h);
	}

	// Render.
	src = p_src;
	dst = p_dst;
	
	tbuf_master.add( TBUF_CHANNEL_VIDEO_PROCESS, -2., "video_process");

	if (b_use_32)
		{
		switch( s_deinterlace_blend )
			{
			case COPY:
				deinterlace_smart_24_copy32(src, dst, srcpitch, w, h, scenechange, b_show_deinterlace);
				break;
			case BLEND:
				deinterlace_smart_24_blend(src, dst, srcpitch, w, h, scenechange, b_show_deinterlace);
				break;
			case CUBIC:
				deinterlace_smart_24_cubic32(src , dst, srcpitch, w, h, scenechange, b_show_deinterlace);
				break;
			case ELA:
//				deinterlace_smart_24_ela_32(src , dst, srcpitch, w, h, scenechange, b_show_deinterlace);
				deinterlace_smart_24_blend(src, dst, srcpitch, w, h, scenechange, b_show_deinterlace);
				break;
			default:
				break;
			}
		}
	else
		{
		switch( s_deinterlace_blend )
			{
			case COPY:
				if (d_motionmap_full)
					deinterlace_smart_24_copy_full(src, dst, srcpitch, w, h, scenechange, b_show_deinterlace);
				else
					deinterlace_smart_24_copy(src, dst, srcpitch, w, h, scenechange, b_show_deinterlace);
				break;
			case BLEND:
				deinterlace_smart_24_blend(src, dst, srcpitch, w, h, scenechange, b_show_deinterlace);
				break;
			case CUBIC:
				deinterlace_smart_24_cubic(src , dst, srcpitch, w, h, scenechange, b_show_deinterlace);
				break;
			case ELA:
				deinterlace_smart_24_blend(src, dst, srcpitch, w, h, scenechange, b_show_deinterlace);
//				deinterlace_smart_24_ela(src , dst, srcpitch, w, h, scenechange, b_show_deinterlace);
				break;
			default:
				break;
			}
		}

	tbuf_master.add( TBUF_CHANNEL_VIDEO_PROCESS, 4., "video_process");
	if (s_diff_mode != FIELD_ONLY)
		if (b_use_32)
			memcpy( (UINT32 *)s_prevFrame, (UINT32 *)p_src, w * h * 3);
		else
			memcpy( s_prevFrame, p_src, w * h * 3);
	tbuf_master.add( TBUF_CHANNEL_VIDEO_PROCESS, -7., "video_process");
	return 0;
}


