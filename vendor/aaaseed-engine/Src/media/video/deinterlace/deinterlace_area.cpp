#include "deinterlace_area.h"
#include "spy.h"
#include "infrastructure/param/param_st.h"

FACTORY_CREATE_V1( c_deint_area, deinterlace_area, Deinterlace Area, deinterlace_area );

namespace n_deint_area
{
	static	CONST	INT32	BASE_PARAM_NB	= 4;
	static	CONST	INT32	GROUP_NB		= 0;

	static	CONST	INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
											+	GROUP_NB;

	static	ST_PARAM	param[PARAM_NB_MAX] =
	{
		ST_PARAM_BOOL_OFF( show_deinterlace )
		ST_PARAM_INT32_POS( difference_threshold, 15., 15. )
		ST_PARAM_BOOL_OFF( blend )
		ST_PARAM_INT32_POS( edge_threshold, 25., 25. )
	};
}

void	c_deint_area::param_init_pt()
{
	INT32	h=0;

	param_set_pt( h, _b_show_deinterlace );
	param_set_pt( h, _threshold );
	param_set_pt( h, _b_blend );
	param_set_pt( h, _edge_threshold );

	err_param_init_pt( h );
}

CONSTRUCTOR_CREATE( c_deint_area )
{
	_min_image_nb = 4;
//	set_name( "deint_area" );
	param_init_with( n_deint_area::param, n_deint_area::PARAM_NB_MAX );
}

BOOL	c_deint_area::is_field_needed( BOOL b_double_framerate )
{
	if( b_double_framerate )
		return TRUE;
	else
		return FALSE;
}

void	c_deint_area::run_frame( UINT8* p_src, UINT8* p_dst )
{
	UINT32	*psrc = (UINT32 *)p_src;
	UINT32	*pdst = (UINT32 *)p_dst;
	//assume RGB32
	INT32	pitch = _size_x * 4;
	INT32	hminus = _size_y - 1;

	INT32	iR0, iG0, iB0, iR1, iG1, iB1, iR2, iG2, iB2, iR3, iG3, iB3;
	UINT32	*psrc1, *psrc2, *psrc3, *pdst1;
	UINT8	*pdst1_8;
	INT32	interface_value_0, interface_value_1, interface_value_2;

//	BOOL bBlend = b_blend;
	INT32 iThreshold = _threshold * _threshold * 4;
	INT32 iEdgeDetect = _edge_threshold;

	if(iEdgeDetect > 180)
		iEdgeDetect = 180;	// We don't want an integer overflow in the interlace calculation.
	iEdgeDetect = iEdgeDetect * iEdgeDetect;

	iR1 = iG1 = iB1 = 0;	// Avoid compiler warning. The value is not used.
	for( INT32 x = 0; x < _size_x; ++x )
	{
		psrc3 = psrc + x;
		iR3 = (*psrc3 >> 16) & 0xff;
		iG3 = (*psrc3 >> 8) & 0xff;
		iB3 = *psrc3 & 0xff;
		psrc2 = (UINT32 *)((CHAR *)psrc3 + pitch);
		iR2 = (*psrc2 >> 16) & 0xff;
		iG2 = (*psrc2 >> 8) & 0xff;
		iB2 = *psrc2 & 0xff;
		pdst1 = pdst + x;
		interface_value_1 = interface_value_2 = 0;
		for( INT32 y = 0; y <= _size_y; ++y )
		{
			psrc1 = psrc2;
			psrc2 = psrc3;
			psrc3 = (UINT32 *)( (CHAR *)psrc3 + pitch );
			iR0 = iR1; iG0 = iG1; iB0 = iB1;
			iR1 = iR2; iG1 = iG2; iB1 = iB2;
			iR2 = iR3; iG2 = iG3; iB2 = iB3;
			if( y < hminus )
			{
				iR3 = (*psrc3 >> 16) & 0xff;
				iG3 = (*psrc3 >> 8) & 0xff;
				iB3 = *psrc3 & 0xff;
			}
			else
			{
				iR3 = iR1; iG3 = iG1; iB3 = iB1;
			}

			interface_value_0 = interface_value_1;
			interface_value_1 = interface_value_2;
			if( y < _size_y )
				// Calculate the interlace value by checking if the pixel color on previous row differs much
				// from this row and next row differs much (with same sign) from this row.
				// Detect edges by checking so that pixel color on previous row doesn't differ too much from next row.
				// If it does, it's probably just an ordinary edge.
				// 3, 6 and 1 are approximate values for converting R, G, B to intensity.
				interface_value_2 = (3 * ((iR1 - iR2) * (iR3 - iR2) - ((iEdgeDetect * (iR1 - iR3) * (iR1 - iR3)) >> 12)) +
									6 * ((iG1 - iG2) * (iG3 - iG2) - ((iEdgeDetect * (iG1 - iG3) * (iG1 - iG3)) >> 12)) +
									(iB1 - iB2) * (iB3 - iB2) - ((iEdgeDetect * (iB1 - iB3) * (iB1 - iB3)) >> 12));
			else
				interface_value_2 = 0;

			if( y > 0 )
			{
				// New in version 1.1: Get mean interlace value of 3 rows (i.e. 5 pixels are examined).
				// Middle row has twice the weight.
				if(interface_value_0 + 2 * interface_value_1 + interface_value_2 > iThreshold)
				{
					// Blend: Get mean value of previous and next row (weight 0.25) and this row (weight 0.5).
					// Interpolate: Odd lines: Copy from source. Even lines: Get mean value of previous and next row
					if( _b_blend )
						*pdst1 = (((iR0 + 2 * iR1 + iR2) >> 2) << 16) + (((iG0 + 2 * iG1 + iG2) >> 2) << 8) + ((iB0 + 2 * iB1 + iB2) >> 2);
					else if( y % 2 == 1 )
						*pdst1 = *psrc1;
					else
						*pdst1 = ((((iR0 + iR2) >> 1) << 16) + (((iG0 + iG2) >> 1) << 8) + ((iB0 + iB2) >> 1));
				}
				else if( _b_show_deinterlace )
					*pdst1 = 0x00787878 + (iR1 >> 4 << 16) + (iG1 >> 4 << 8) + (iB1 >> 4);
				else
					*pdst1 = *psrc1;

				// clean alpha channel
				pdst1_8 = (UINT8 *)pdst1+3;
				*pdst1_8 = 0x000000ff;

				pdst1 = (UINT32 *)( (CHAR *)pdst1 + pitch );
			}
		}
	}
}

void	c_deint_area::area_32_field( UINT8* p_src, UINT8* p_src_next, UINT8* p_dst )
{
	UINT32	*psrc = (UINT32 *)p_src;
	UINT32	*pdst = (UINT32 *)p_dst;
	INT32	pitch = _size_x * 4;	//assume RGB32
	INT32	h2 = _size_y * 2;
	INT32	hminus = h2 - 1;

	INT32	iR0, iG0, iB0, iR1, iG1, iB1, iR2, iG2, iB2, iR3, iG3, iB3;
	UINT32	*psrc1, *psrc2, *psrc3, *pdst1;
	UINT8	*pdst1_8;
	INT32	interface_value_0, interface_value_1, interface_value_2;

//	BOOL bBlend = b_blend;
	INT32 iThreshold = _threshold * _threshold * 4;
	INT32 iEdgeDetect = _edge_threshold;
	if( iEdgeDetect > 180 )
		iEdgeDetect = 180;	// We don't want an integer overflow in the interlace calculation.
	iEdgeDetect = iEdgeDetect * iEdgeDetect;

	TBUF_ADD( tbuf::CH_VIDEO_PROCESS, -2., nullptr, this );

	// mege field for now
	//todo smart deinterlace with field
	UINT32* src = (UINT32 *)_merge_src;
	UINT32*	src1 = (UINT32 *)p_src_next;
	UINT32*	src2 = (UINT32 *)p_src;

	for( INT32 i = _size_y; i > 0; --i )
	{
		MEMCPY( src, src1, pitch );
		src += _size_x;
		src1 += _size_x;
		MEMCPY( src, src2, pitch );
		src += _size_x;
		src2 += _size_x;
	}	

	psrc = (UINT32 *)_merge_src;

	iR1 = iG1 = iB1 = 0;	// Avoid compiler warning. The value is not used.
	for( INT32 x = 0; x < _size_x; ++x )
	{
		psrc3 = psrc + x;
		iR3 = (*psrc3 >> 16) & 0xff;
		iG3 = (*psrc3 >> 8) & 0xff;
		iB3 = *psrc3 & 0xff;
		psrc2 = (UINT32 *)((CHAR *)psrc3 + pitch);
		iR2 = (*psrc2 >> 16) & 0xff;
		iG2 = (*psrc2 >> 8) & 0xff;
		iB2 = *psrc2 & 0xff;
		pdst1 = pdst + x;
		interface_value_1 = interface_value_2 = 0;
		for( INT32 y = 0; y <= h2; ++y )
		{
			psrc1 = psrc2;
			psrc2 = psrc3;
			psrc3 = (UINT32 *)((CHAR *)psrc3 + pitch);
			iR0 = iR1; iG0 = iG1; iB0 = iB1;
			iR1 = iR2; iG1 = iG2; iB1 = iB2;
			iR2 = iR3; iG2 = iG3; iB2 = iB3;
			if( y < hminus )
			{
				iR3 = (*psrc3 >> 16) & 0xff;
				iG3 = (*psrc3 >> 8) & 0xff;
				iB3 = *psrc3 & 0xff;
			}
			else
			{
				iR3 = iR1; iG3 = iG1; iB3 = iB1;
			}
			interface_value_0 = interface_value_1;
			interface_value_1 = interface_value_2;
			if( y < h2 )
				// Calculate the interlace value by checking if the pixel color on previous row differs much
				// from this row and next row differs much (with same sign) from this row.
				// Detect edges by checking so that pixel color on previous row doesn't differ too much from next row.
				// If it does, it's probably just an ordinary edge.
				// 3, 6 and 1 are approximate values for converting R, G, B to intensity.
				interface_value_2 = (3 * ((iR1 - iR2) * (iR3 - iR2) - ((iEdgeDetect * (iR1 - iR3) * (iR1 - iR3)) >> 12)) +
									6 * ((iG1 - iG2) * (iG3 - iG2) - ((iEdgeDetect * (iG1 - iG3) * (iG1 - iG3)) >> 12)) +
									(iB1 - iB2) * (iB3 - iB2) - ((iEdgeDetect * (iB1 - iB3) * (iB1 - iB3)) >> 12));
			else
				interface_value_2 = 0;

			if( y > 0 )
			{
				// New in version 1.1: Get mean interlace value of 3 rows (i.e. 5 pixels are examined).
				// Middle row has twice the weight.
				if( ( interface_value_0 + 2 * interface_value_1 + interface_value_2 ) > iThreshold)
				{
					// Blend: Get mean value of previous and next row (weight 0.25) and this row (weight 0.5).
					// Interpolate: Odd lines: Copy from source. Even lines: Get mean value of previous and next row
					if( _b_blend )
						*pdst1 = (((iR0 + 2 * iR1 + iR2) >> 2) << 16) + (((iG0 + 2 * iG1 + iG2) >> 2) << 8) + ((iB0 + 2 * iB1 + iB2) >> 2);
					else if( y % 2 == 1 )
						*pdst1 = *psrc1;
					else
						*pdst1 = ((((iR0 + iR2) >> 1) << 16) + (((iG0 + iG2) >> 1) << 8) + ((iB0 + iB2) >> 1));
				}
				else if( _b_show_deinterlace )
					*pdst1 = 0x00787878 + (iR1 >> 4 << 16) + (iG1 >> 4 << 8) + (iB1 >> 4);
				else
					*pdst1 = *psrc1;

				// clean alpha channel
				pdst1_8 = (UINT8 *)pdst1+3;
				*pdst1_8 = 0x000000ff;
				pdst1 = (UINT32 *)((CHAR *)pdst1 + pitch);
			}
		}
	}
}

void	c_deint_area::close()
{
}

void	c_deint_area::init( INT32 size_x, INT32 size_y )
{
}

void	c_deint_area::run_full(  c_image_flux* flux_in, UINT8* dst, BOOL second_pass )
{

}

c_deint_area::~c_deint_area()
{
	close();
}
