#include "deinterlace_smooth.h"
#include "spy.h"
#include "infrastructure/param/param_st.h"

FACTORY_CREATE_V1( c_deint_smooth, deinterlace_smooth, Deinterlace Smooth, deinterlace_smooth );

namespace n_deint_smooth
{
	static	CONST	INT32	BASE_PARAM_NB			= 7;
	static	CONST	INT32	GROUP_NB				= 0;

	static	CONST	INT32	PARAM_NB_MAX =
			BASE_PARAM_NB
		+	GROUP_NB;

	static	ST_PARAM	param[PARAM_NB_MAX] =
	{
		ST_PARAM_BOOL_OFF( show_deinterlace )
		ST_PARAM_INT32_POS( difference_threshold, 15, 15 )
		ST_PARAM_BOOL_OFF( blend )
		ST_PARAM_BOOL_OFF( alt_order )
		ST_PARAM_INT32_POS( edge_threshold,		20, 20 )
		ST_PARAM_INT32_POS( static_threshold,	15, 35 )
		ST_PARAM_INT32_POS( static_averaging,	15, 80 )
	};
}

void	c_deint_smooth::param_init_pt()
{
INT32	h = 0;

	param_set_pt( h, _b_show_deinterlace );
	param_set_pt( h, _smooth_threshold );
	param_set_pt( h, _b_smooth_blend );
	param_set_pt( h, _b_smooth_alt_order );
	param_set_pt( h, _smooth_edge_threshold );
	param_set_pt( h, _smooth_interframe_threshold );
	param_set_pt( h, _smooth_interframe_average );

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_deint_smooth)
{

	_min_image_nb = 4;
	_pframe = nullptr;
	_pframeprev = nullptr;
	_piFrameDiffs = nullptr;

//	set_name("deint_smooth");
	param_init_with( n_deint_smooth::param, n_deint_smooth::PARAM_NB_MAX);
}

BOOL	c_deint_smooth::is_field_needed( BOOL b_double_framerate )
{
	if( b_double_framerate )
	{
		return TRUE;
	}
	else
		return FALSE;
}


void c_deint_smooth::init(INT32 size_x, INT32 size_y, INT32 smooth_interframe_threshold)
{
	INT32 nb = size_x * size_y;
	_pframe = new UINT32[nb];
	if( _pframe )
		MEMSET( _pframe, 0, nb * 4 );
	else
		ERR_PRINT_STRING( "Deinterlace could not allocate pframe" );

	_pframeprev = new UINT32[nb];
	if( _pframeprev )
		MEMSET( _pframeprev, 0, nb * 4 );
	else
		ERR_PRINT_STRING( "Deinterlace could not allocate pframeprev" );

//	INT32 iInitDiff = smooth_interframe_threshold * smooth_interframe_threshold + 1;
	_piFrameDiffs = new INT32[nb];
	if( _piFrameDiffs )
		MEMSET( _piFrameDiffs, smooth_interframe_threshold * smooth_interframe_threshold + 1, nb * 4 );
	else
		ERR_PRINT_STRING( "Deinterlace could not allocate piFrameDiffs" );
	
	//INT32 iInitDiff = smooth_interframe_threshold * smooth_interframe_threshold + 1;
	//
	//for( INT32 i = p_width * p_height - 1; i >= 0; --i )
	//	piFrameDiffs[i] = iInitDiff;
	
	_iFrameNo = 0;
}

void c_deint_smooth::close()
{
	SAFE_DELETE_ARRAY( _pframe );
	SAFE_DELETE_ARRAY( _pframeprev );
	SAFE_DELETE_ARRAY( _piFrameDiffs );
}


struct smooth_show_blend
{	static FINLINE void	apply( UINT32* &pdst, UINT32* &psrc, INT32 y, INT32 iOddEven, INT32 iR0, INT32 iR1, INT32 iR2, INT32 iG0, INT32 iG1, INT32 iG2,INT32 iB0, INT32 iB1, INT32 iB2)
		{
		*pdst = 0xff903030 + ( ( (iR0 + 2 * iR1 + iR2) >> 4) << 16) + ( ( (iG0 + 2 * iG1 + iG2) >> 4) << 8) + ( (iB0 + 2 * iB1 + iB2) >> 4);
		}
};

struct smooth_show_inter
{	static FINLINE void	apply( UINT32* &pdst, UINT32* &psrc, INT32 y, INT32 iOddEven, INT32 iR0, INT32 iR1, INT32 iR2, INT32 iG0, INT32 iG1, INT32 iG2,INT32 iB0, INT32 iB1, INT32 iB2)
		{
		if( y % 2 == iOddEven )
			*pdst = 0xff903030 + ((iR1 >> 2) << 16) + ((iG1 >> 2) << 8) + (iB1 >> 2);
		else
			*pdst = 0xff903030 + (((iR0 + iR2) >> 3) << 16) + (((iG0 + iG2) >> 3) << 8) + ((iB0 + iB2) >> 3);
		}
};

struct smooth_blend
{	static FINLINE void	apply( UINT32* &pdst, UINT32* &psrc, INT32 y, INT32 iOddEven, INT32 iR0, INT32 iR1, INT32 iR2, INT32 iG0, INT32 iG1, INT32 iG2,INT32 iB0, INT32 iB1, INT32 iB2)
		{
		*pdst = 0xff903030 + ( ( (iR0 + 2 * iR1 + iR2) >> 4) << 16) + ( ( (iG0 + 2 * iG1 + iG2) >> 4) << 8) + ( (iB0 + 2 * iB1 + iB2) >> 4);
		}
};
struct smooth_inter
{	static FINLINE void	apply( UINT32* &pdst, UINT32* &psrc, INT32 y, INT32 iOddEven, INT32 iR0, INT32 iR1, INT32 iR2, INT32 iG0, INT32 iG1, INT32 iG2,INT32 iB0, INT32 iB1, INT32 iB2)
		{
		if( y % 2 == iOddEven )
			*pdst = ( 0xff000000 | *psrc );
		else
			*pdst = 0xff000000 + (((iR0 + iR2) >> 1) << 16) + (((iG0 + iG2) >> 1) << 8) + ((iB0 + iB2) >> 1);
		}
};




template <class smooth>
static FINLINE	void loop_smooth_32( smooth, UINT32* &pframe, UINT32* &pframeprev, INT32* &piFrameDiffs, UINT32* &psrc, UINT32* &pdst, INT32 size_x, INT32 size_y, INT32 iOddEven, INT32 threshold, INT32 bEdgeDetect, INT32 bInterFrameAverage, INT32 bInterFrameLeaveThreshold, BOOL b_show_deinterlace, INT32 iFrameNo)
{
	INT32		iThreshold = threshold * threshold * 4;
	INT32		iEdgeDetect = bEdgeDetect;
	INT32		iInterFrameLeaveThreshold;
	INT32		iInterFrameAverage;
	INT32		iInterFrameAverageRest;

	UINT32*		p_pframe;

	INT32		iR0, iG0, iB0, iR1, iG1, iB1, iR2, iG2, iB2, iR3, iG3, iB3;
	INT32		iR2p, iG2p, iB2p;
	UINT32		*pframe1, *pframe2, *pframe3, *pdst1, *pframe2p;

	INT32		*piFrameDiffs2p;
	INT32		iInterlaceValue0, iInterlaceValue1, iInterlaceValue2;
	INT32		iFrameDiffValue0, iFrameDiffValue1, iFrameDiffValue2;
	INT32		dst_pitch = size_x * 4;
	INT32		src_pitch = size_x * 4;
	INT32		h_reel = size_y*2;
	INT32		hminus1 = size_y - 1;

	INT32		y;

	if(iEdgeDetect > 180)
		iEdgeDetect = 180;	// We don't want an integer overflow in the interlace calculation.

	iEdgeDetect = iEdgeDetect * iEdgeDetect;

	iInterFrameLeaveThreshold = bInterFrameLeaveThreshold * bInterFrameLeaveThreshold;

	iInterFrameAverage = CLAMP( bInterFrameAverage * 1024 / 100, 0, 1023 );
	iInterFrameAverageRest = 1024 - iInterFrameAverage;


	iR1 = iG1 = iB1 = 0;	// Avoid compiler warning. The value is not used.

	pframeprev = pframe;
	p_pframe = psrc;

	// Do some area based deinterlacing on the combined frame.
	for( INT32 x = 0; x < size_x; ++x )
		{
		pframe3 = p_pframe + x;
		iR3 = (*pframe3 >> 16) & 0xff;
		iG3 = (*pframe3 >> 8) & 0xff;
		iB3 = *pframe3 & 0xff;
		pframe2 = (UINT32 *)((UINT8 *)pframe3 + dst_pitch);
		iR2 = (*pframe2 >> 16) & 0xff;
		iG2 = (*pframe2 >> 8) & 0xff;
		iB2 = *pframe2 & 0xff;
		
		pframe2p = pframeprev + x;
		piFrameDiffs2p = piFrameDiffs + x;

		pdst1 = pdst + x;
		iInterlaceValue1 = iInterlaceValue2 = 0;
		iFrameDiffValue1 = iFrameDiffValue2 = 0;

		pframe1 = pframe2;
		pframe2 = pframe3;
		pframe3 = (UINT32 *)((UINT8 *)pframe3 + dst_pitch);
		iR0 = iR1; iG0 = iG1; iB0 = iB1; 
		iR1 = iR2; iG1 = iG2; iB1 = iB2;
		iR2 = iR3; iG2 = iG3; iB2 = iB3;
		iR3 = (*pframe3 >> 16) & 0xff;
		iG3 = (*pframe3 >> 8) & 0xff;
		iB3 = *pframe3 & 0xff;

		iR2p = (*pframe2p >> 16) & 0xff;
		iG2p = (*pframe2p >> 8) & 0xff;
		iB2p = *pframe2p & 0xff;

		pframe2p = (UINT32 *)((UINT8 *)pframe2p + dst_pitch);

		iInterlaceValue0 = iInterlaceValue1;
		iInterlaceValue1 = iInterlaceValue2;
		iFrameDiffValue0 = iFrameDiffValue1;
		iFrameDiffValue1 = iFrameDiffValue2;
		// Calculate the interlace value by checking if the pixel color on previous row differs much
		// from this row and next row differs much (with same sign) from this row.
		// Detect edges by checking so that pixel color on previous row doesn't differ too much from next row.
		// If it does, it's probably just an ordinary edge.
		// 3, 6 and 1 are approximate values for converting R, G, B to intensity.
		iInterlaceValue2 = (3 * ((iR1 - iR2) * (iR3 - iR2) - ((iEdgeDetect * (iR1 - iR3) * (iR1 - iR3)) >> 12)) +
							6 * ((iG1 - iG2) * (iG3 - iG2) - ((iEdgeDetect * (iG1 - iG3) * (iG1 - iG3)) >> 12)) +
							(iB1 - iB2) * (iB3 - iB2) - ((iEdgeDetect * (iB1 - iB3) * (iB1 - iB3)) >> 12));
		if (iFrameNo > 1)
			iFrameDiffValue2 = (iInterFrameAverage * *piFrameDiffs2p +
					iInterFrameAverageRest * (3 * (iR2 - iR2p) * (iR2 - iR2p) + 6 * (iG2 - iG2p) * (iG2 - iG2p) + (iB2 - iB2p) * (iB2 - iB2p))) >> 10;
		else
			iFrameDiffValue2 = ( (iInterFrameAverage + iInterFrameAverageRest) * (*piFrameDiffs2p) ) >> 10;
		*piFrameDiffs2p = iFrameDiffValue2;
		piFrameDiffs2p += size_x;

		// In this loop, output is delayed one loop (or row) from input. Current output row has variables with digit 1 and current input row has digit 2.
		for( y = 1; y < size_y; ++y )
			{
			pframe1 = pframe2;
			pframe2 = pframe3;
			pframe3 = (UINT32 *)((UINT8 *)pframe3 + dst_pitch);
			iR0 = iR1; iG0 = iG1; iB0 = iB1;
			iR1 = iR2; iG1 = iG2; iB1 = iB2;
			iR2 = iR3; iG2 = iG3; iB2 = iB3;
			if (y < hminus1)
				{
				iR3 = (*pframe3 >> 16) & 0xff;
				iG3 = (*pframe3 >> 8) & 0xff;
				iB3 = *pframe3 & 0xff;

				iR2p = (*pframe2p >> 16) & 0xff;
				iG2p = (*pframe2p >> 8) & 0xff;
				iB2p = *pframe2p & 0xff;
				}
			else
				{
				iR3 = iR1; iG3 = iG1; iB3 = iB1;
				}
			pframe2p = (UINT32 *)((UINT8 *)pframe2p + dst_pitch);

			iInterlaceValue0 = iInterlaceValue1;
			iInterlaceValue1 = iInterlaceValue2;
			iFrameDiffValue0 = iFrameDiffValue1;
			iFrameDiffValue1 = iFrameDiffValue2;
			// Calculate the interlace value by checking if the pixel color on previous row differs much
			// from this row and next row differs much (with same sign) from this row.
			// Detect edges by checking so that pixel color on previous row doesn't differ too much from next row.
			// If it does, it's probably just an ordinary edge.
			// 3, 6 and 1 are approximate values for converting R, G, B to intensity.
			iInterlaceValue2 = (3 * ((iR1 - iR2) * (iR3 - iR2) - ((iEdgeDetect * (iR1 - iR3) * (iR1 - iR3)) >> 12)) +
								6 * ((iG1 - iG2) * (iG3 - iG2) - ((iEdgeDetect * (iG1 - iG3) * (iG1 - iG3)) >> 12)) +
								(iB1 - iB2) * (iB3 - iB2) - ((iEdgeDetect * (iB1 - iB3) * (iB1 - iB3)) >> 12));
			if (iFrameNo > 1)
				iFrameDiffValue2 = (iInterFrameAverage * *piFrameDiffs2p + iInterFrameAverageRest * (3 * (iR2 - iR2p) * (iR2 - iR2p) + 6 * (iG2 - iG2p) * (iG2 - iG2p) + (iB2 - iB2p) * (iB2 - iB2p))) >> 10;
			else
				iFrameDiffValue2 = ( (iInterFrameAverage + iInterFrameAverageRest) * (*piFrameDiffs2p) ) >> 10;
			*piFrameDiffs2p = iFrameDiffValue2;
			piFrameDiffs2p += size_x;

			// Get mean interlace value of 3 rows (i.e. 5 pixels are examined).
			// Middle row has twice the weight.
			if((iInterlaceValue0 + 2 * iInterlaceValue1 + iInterlaceValue2 > iThreshold) && (iFrameDiffValue0 + 2 * iFrameDiffValue1 + iFrameDiffValue2 >= iInterFrameLeaveThreshold))
				{
				smooth::apply(pdst1, pframe1, y, iOddEven, iR0, iR1, iR2, iG0, iG1, iG2, iB0, iB1, iB2 );
				}
			else
				{
				if(b_show_deinterlace)
					{
					*pdst1 = (iR1 >> 2 << 16) + (iG1 >> 2 << 8) + (iB1 >> 2);
					if(iInterlaceValue0 + 2 * iInterlaceValue1 + iInterlaceValue2 > iThreshold)
						*pdst1 += 0xff309030;
					else if(iFrameDiffValue0 + 2 * iFrameDiffValue1 + iFrameDiffValue2 >= iInterFrameLeaveThreshold)
						*pdst1 += 0xff303090;
					else
						*pdst1 += 0xff303030;
					}
				else
					*pdst1 = (0xff000000 | *pframe1);
				}
			pdst1 = (UINT32 *)((UINT8 *)pdst1 + dst_pitch);
			
			}
		

//y = _size_y
		// In this loop, output is delayed one loop (or row) from input. Current output row has variables with digit 1 and current input row has digit 2.
		pframe1 = pframe2;
		pframe2 = pframe3;
		pframe3 = (UINT32 *)((UINT8 *)pframe3 + dst_pitch);
		iR0 = iR1; iG0 = iG1; iB0 = iB1;
		iR1 = iR2; iG1 = iG2; iB1 = iB2;
		iR2 = iR3; iG2 = iG3; iB2 = iB3;
		iR3 = iR1; iG3 = iG1; iB3 = iB1;
		pframe2p = (UINT32 *)((UINT8 *)pframe2p + dst_pitch);

		iInterlaceValue0 = iInterlaceValue1;
		iInterlaceValue1 = iInterlaceValue2;
		iFrameDiffValue0 = iFrameDiffValue1;
		iFrameDiffValue1 = iFrameDiffValue2;
		iInterlaceValue2 = 0;
		iFrameDiffValue2 = 0;

	// Get mean interlace value of 3 rows (i.e. 5 pixels are examined).
	// Middle row has twice the weight.
		if ((iInterlaceValue0 + 2 * iInterlaceValue1 > iThreshold) && (iFrameDiffValue0 + 2 * iFrameDiffValue1 >= iInterFrameLeaveThreshold))
			{
			smooth::apply(pdst1, pframe1, y, iOddEven, iR0, iR1, iR2, iG0, iG1, iG2, iB0, iB1, iB2 );
			}
		else
			{
			if(b_show_deinterlace)
				{
				*pdst1 = (iR1 >> 2 << 16) + (iG1 >> 2 << 8) + (iB1 >> 2);
				if (iInterlaceValue0 + 2 * iInterlaceValue1 > iThreshold)
					*pdst1 += 0xff309030;
				else if (iFrameDiffValue0 + 2 * iFrameDiffValue1 >= iInterFrameLeaveThreshold)
					*pdst1 += 0xff303090;
				else
					*pdst1 += 0xff303030;
				}
			else
				*pdst1 = (0xff000000 | *pframe1);
			}
		pdst1 = (UINT32 *)((UINT8 *)pdst1 + dst_pitch);
		}

//	MEMCPY( pframeprev, psrc, size_x * _size_y * 4);

}



void c_deint_smooth::smooth_32(UINT8* p_src, UINT8* p_dst, INT32 lCurrentSourceFrame )
{
//	deinterlace - smooth v1.1
//	Generates high resolution, non-interlaced 50/60fps video from interlaced material that has been splitted into separate fields.
//	based on filter coded by Gunnar Thalin

//* Show_deinterlace: Areas are colored differently to help you find suitable parameter values.
//          o Red - Deinterlaced areas
//          o Blue - Non-static areas that would be deinterlaced if interlace patterns were found
//          o Green - Static areas that do contain interlace patterns but are still left untouched
//          o Grey - Static areas without interlace patterns
//* Blend: Blends this and previous field in interlaced areas. (You should probably avoid using this as it just blurs the video.)
//* Interpolate: Interpolate the field to full frame size in interlaced areas.
//* Alternate field order: Put even or odd fields on first line of output video. The correct setting depends on your video clip. If static video
//  jumps up and down slightly when you step through it, change this setting.
//* Interlace threshold: Controls the detection of interlace patterns. Lower values deinterlaces more.
//* Edge detect: It's difficult to distinguish between interlace lines and real edges (which should not be deinterlaced). This value controls this
//  decision. Higher value leaves more edges intact.
//* Static threshold: The filter tries to detect static areas to avoid deinterlacing fine details which could result in flickering.
//  This value controls how much a pixel can variate and still be called static. Use as low value as possible to avoid leaving interlace patterns.
//  Values above 50 (or so) are not recommended. Good quality video can use lower values. If you don't have any text or logos that may flicker I
//  suggest using very low values. 0 makes it work like version 1.0.
//* Static averaging: Controls how long history to look at when determining if areas are static or not. Low values (= short history) find static
//  fast (but maybe incorrectly, leaving interlace patterns). High values means static details may flicker for a longer time before "converging".
//  Also, it can be slower to react when areas go from static to non-static. Valid range is 0-100. A good rule is to set
//  &ltstatic averaging> = 2 * &ltstatic threshold>, or higher (but don't get too close to 100).


//	INT32		smooth_interframe_threshold = smooth_interframe_threshold;
	INT32		iInterFrameAverage = _smooth_interframe_average;

	UINT32*		psrc;
	UINT32*		pdst;
	INT32		iOddEven;

	//b_smooth_blend = FALSE;
	//b_smooth_alt_order = FALSE;
	//smooth_edge_threshold = 20;
	//smooth_interframe_threshold = 35;
	//iInterFrameAverage = 80;


	TBUF_ADD( tbuf::CH_VIDEO_PROCESS, 1., "video_process", this );

	psrc = (UINT32 *)p_src;
	pdst = (UINT32 *)p_dst;

	// clean dst bitmap, init alpha channel
	MEMSET(pdst , 0xffff , _size_x * _size_y * sizeof (UINT32));

	if(_b_smooth_alt_order && ( lCurrentSourceFrame % 2 ) == 1 || !_b_smooth_alt_order && ( lCurrentSourceFrame % 2 ) == 0 )
		iOddEven =  0;
	else
		iOddEven =  1;

	TBUF_ADD( tbuf::CH_VIDEO_PROCESS, 4., "video_process", this );

	// template based approach
	if ( _b_show_deinterlace )
		if ( _b_smooth_blend )
			loop_smooth_32<smooth_show_blend>( smooth_show_blend(), _pframe, _pframeprev, _piFrameDiffs, psrc, pdst, _size_x, _size_y, iOddEven, _smooth_threshold, _smooth_edge_threshold, iInterFrameAverage, _smooth_interframe_threshold, _b_show_deinterlace, _iFrameNo );
		else
			loop_smooth_32<smooth_show_inter>( smooth_show_inter(), _pframe, _pframeprev, _piFrameDiffs, psrc, pdst, _size_x, _size_y, iOddEven, _smooth_threshold, _smooth_edge_threshold, iInterFrameAverage, _smooth_interframe_threshold, _b_show_deinterlace, _iFrameNo );
	else
		if ( _b_smooth_blend )
			loop_smooth_32<smooth_blend>( smooth_blend(), _pframe, _pframeprev, _piFrameDiffs, psrc, pdst, _size_x, _size_y, iOddEven, _smooth_threshold, _smooth_edge_threshold, iInterFrameAverage, _smooth_interframe_threshold, _b_show_deinterlace, _iFrameNo );
		else
			loop_smooth_32<smooth_inter>( smooth_inter(), _pframe, _pframeprev, _piFrameDiffs, psrc, pdst, _size_x, _size_y, iOddEven, _smooth_threshold, _smooth_edge_threshold, iInterFrameAverage, _smooth_interframe_threshold, _b_show_deinterlace, _iFrameNo );
 

	TBUF_ADD( tbuf::CH_VIDEO_PROCESS, -4., "video_process", this );

	MEMCPY( _pframe, psrc, _size_x * _size_y * 4);

	_iFrameNo++;

}

void	c_deint_smooth::run_frame( UINT8* src, UINT8* dst )
{
}

void	c_deint_smooth::run_full(  c_image_flux* flux_in, UINT8* dst, BOOL second_pass )
{

}

c_deint_smooth::~c_deint_smooth()
{
	close();
}