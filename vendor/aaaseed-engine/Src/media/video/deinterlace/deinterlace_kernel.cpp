#include "deinterlace_kernel.h"
#include "spy.h"
#include "infrastructure/param/param_st.h"

FACTORY_CREATE_V1( c_deint_kernel, deinterlace_kernel, Deinterlace Kernel, deinterlace_kernel );

template <class kernel>
static FINLINE	void loop_32( kernel, UINT8* &s_prevFrame2, UINT8* &srcp_saved, UINT8* &dstp_saved, INT32 src_pitch, INT32 dst_pitch, INT32 order, INT32 size_x, INT32 size_y, INT32 threshold, BOOL b_show_deinterlace )
{
	INT32	x, y;

	UINT8	*srcp, *prvp, *prvpp, *prvpn, *prvppp, *prvpnn, *prvp4p, *prvp4n;
	UINT8	*srcpp, *srcppp, *srcpn, *srcpnn, *srcp3p, *srcp3n, *srcp4p, *srcp4n;
	UINT8*	dstp;

	INT32	src_pitch_2 = 2 * src_pitch;
	INT32	diff, diff_prev, diff_next;

	/* For the other field choose adaptively between using the previous field or the interpolant from the current field. */
	prvp =   s_prevFrame2 + 5 * src_pitch - ( 1 - order ) * src_pitch;
	prvpp =  prvp - src_pitch;
	prvppp = prvp - src_pitch_2;
	prvp4p = prvp - 4 * src_pitch;
	prvpn =  prvp + src_pitch;
	prvpnn = prvp + src_pitch_2;
	prvp4n = prvp + 4 * src_pitch;

	srcp =   srcp_saved + 5 * src_pitch - ( 1 - order ) * src_pitch;
	srcpp =  srcp - src_pitch;
	srcppp = srcp - src_pitch_2;
	srcp3p = srcp - 3 * src_pitch;
	srcp4p = srcp - 4 * src_pitch;
	srcpn =  srcp + src_pitch;
	srcpnn = srcp + src_pitch_2;
	srcp3n = srcp + 3 * src_pitch;
	srcp4n = srcp + 4 * src_pitch;

	dstp =   dstp_saved  + 5 * dst_pitch - ( 1 - order ) * dst_pitch;

	for( y = 5 - ( 1 - order ); y <= ( size_y - 5 - ( 1 - order ) ); y += 2 )
		{
		for( x = 0; x < src_pitch; ++x )
			{
			diff = (INT32)prvp[x] - (INT32)srcp[x];
			diff_prev = (INT32)prvpp[x] - (INT32)srcpp[x];
			diff_next = (INT32)prvpn[x] - (INT32)srcpn[x];
			if ( ( threshold == 0 ) || ( OUTSIDE_ABS( diff, threshold ) ) ||
				( OUTSIDE_ABS( diff_prev, threshold ) ) || ( OUTSIDE_ABS( diff_next, threshold ) ) ) 
				{
				if( b_show_deinterlace )
					{
					INT32 g = x & ~3;
					dstp[g++] = 255; dstp[g++] = 255; dstp[g++] = 255;
					dstp[g] = 255;
					x = g;
					}
				else
					kernel::apply(x, dstp, prvp, prvpp, prvppp, prvpn, prvpnn, prvp4p, prvp4n,
							 srcp, srcpp, srcppp, srcp3p, srcp4p, srcpn, srcpnn, srcp3n, srcp4n);
				}
			else
				{
				dstp[x] = srcp[x];
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
		dstp   += dst_pitch * 2;
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


namespace n_deint_kernel
{
	static	CONST	INT32	BASE_PARAM_NB	= 5;
	static	CONST	INT32	GROUP_NB		= 0;

	static	CONST	INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
											+	GROUP_NB;

	static	ST_PARAM	param[PARAM_NB_MAX] =
	{
		ST_PARAM_BOOL_OFF( show_deinterlace )
		ST_PARAM_INT32( difference_threshold, 15, 15, 0, 255 )
	//	{	NULL,	PARAM_INT32,	"difference_threshold",	15,	15,		0, 255,	NULL, NULL },
		ST_PARAM_BOOL_OFF( sharp )
		ST_PARAM_BOOL_OFF( two_way )
		ST_PARAM_BOOL_OFF( field_order )
	};
}

void	c_deint_kernel::param_init_pt()
{
INT32	h;

	h = 0;
	param_set_pt( h, _b_show_deinterlace );
	param_set_pt( h, _threshold );
	param_set_pt( h, _b_sharp );
	param_set_pt( h, _b_two_way );
	param_set_pt( h, _field_order );

	err_param_init_pt( h );
}

CONSTRUCTOR_CREATE( c_deint_kernel )
{
	_min_image_nb = 6;
//	set_name( "deint_kernel" );
	param_init_with( n_deint_kernel::param, n_deint_kernel::PARAM_NB_MAX );

	_s_prevFrame2 = nullptr;
}

BOOL	c_deint_kernel::is_field_needed( BOOL b_double_framerate )
{
	if( b_double_framerate )
	{
		return TRUE;
	}
	else
		return FALSE;
}

void c_deint_kernel::run_frame( UINT8* p_src, UINT8* p_dst ) //, INT32 s_field_order )
{
	UINT8*	srcp;
	UINT8*	srcp_saved;
	UINT8*	dstp;
	UINT8*	dstp_saved;

	INT32	src_pitch = _size_x * 4;
	INT32	dst_pitch = _size_x * 4;
	INT32	y;
	INT32	order;

	order = ( _field_order + 1 ) & 1;

	srcp = srcp_saved = p_src;
	dstp = dstp_saved = p_dst;

	TBUF_ADD( tbuf::CH_VIDEO_PROCESS, -2., "video_process", this );
	// copy lines that don't need to be deinterlaced
	srcp = srcp_saved + ( 1 - order ) * src_pitch;
	dstp = dstp_saved + ( 1 - order ) * dst_pitch;

	for ( y = 0; y < _size_y; y+=2 )
		{
		MEMCPY( (UINT32 *)dstp, (UINT32 *)srcp, dst_pitch );
		srcp += 2 * src_pitch;
		dstp += 2 * dst_pitch;
		}

	// Copy through the lines that will be missed below.
	MEMCPY((UINT32 *)( dstp_saved + order * dst_pitch )					, (UINT32 *)( srcp_saved + ( 1 - order ) * src_pitch )			, dst_pitch);
	MEMCPY((UINT32 *)( dstp_saved + ( 2 + order ) * dst_pitch )			, (UINT32 *)( srcp_saved + ( 3 - order ) * src_pitch )			, dst_pitch);
	MEMCPY((UINT32 *)( dstp_saved + ( _size_y - 2 + order ) * dst_pitch ), (UINT32 *)( srcp_saved + ( _size_y - 1 - order ) * src_pitch )	, dst_pitch);
	MEMCPY((UINT32 *)( dstp_saved + ( _size_y - 4 + order ) * dst_pitch ), (UINT32 *)( srcp_saved + ( _size_y - 3 - order ) * src_pitch )	, dst_pitch);

	TBUF_ADD( tbuf::CH_VIDEO_PROCESS, 4., "video_process", this );

	if ( _b_sharp )
		if ( _b_two_way )
			loop_32<kernel_2_float>( kernel_2_float(), _s_prevFrame2, srcp_saved, dstp_saved, src_pitch, dst_pitch, order, _size_x, _size_y, _threshold, _b_show_deinterlace );
		else
			loop_32<kernel_1_float>( kernel_1_float(), _s_prevFrame2, srcp_saved, dstp_saved, src_pitch, dst_pitch, order, _size_x, _size_y, _threshold, _b_show_deinterlace );
	else
		if ( _b_two_way )
			loop_32<kernel_2_int>( kernel_2_int(), _s_prevFrame2, srcp_saved, dstp_saved, src_pitch, dst_pitch, order, _size_x, _size_y, _threshold, _b_show_deinterlace );
		else
			loop_32<kernel_1_int>( kernel_1_int(), _s_prevFrame2, srcp_saved, dstp_saved, src_pitch, dst_pitch, order, _size_x, _size_y, _threshold, _b_show_deinterlace );

	TBUF_ADD( tbuf::CH_VIDEO_PROCESS, -4., "video_process", this );

	// save last frame in memory
	MEMCPY( (UINT32 *)_s_prevFrame2, (UINT32 *)p_src, _size_x * _size_y * 4 );

	TBUF_ADD( tbuf::CH_VIDEO_PROCESS, 2., "video_process", this );
}

void c_deint_kernel::kernel_32_field( UINT8* p_src, UINT8* p_src_next, UINT8* p_dst )//, INT32 s_field_order )
{
	UINT8*	srcp;
	UINT8*	srcp_saved;
	UINT8*	dstp;
	UINT8*	dstp_saved;

	INT32	h2 = _size_y * 2;
	INT32	src_pitch = _size_x * 4;
	INT32	dst_pitch = _size_x * 4;
//	INT32	y;
	INT32	order;

	order = ( _field_order + 1 ) & 1;

	TBUF_ADD( tbuf::CH_VIDEO_PROCESS, -2., nullptr, this );

	// mege field for now
	//todo kernel deinterlace with field
	UINT32*	src = (UINT32 *)_merge_src;
	UINT32*	src1 = (UINT32 *)p_src_next;
	UINT32*	src2 = (UINT32 *)p_src;

	for( INT32 i = _size_y; i > 0; --i )
	{
		MEMCPY( src, src1, src_pitch );
		src += _size_x;
		src1 += _size_x;
		MEMCPY( src, src2, src_pitch );
		src += _size_x;
		src2 += _size_x;
	}	

	srcp = srcp_saved = _merge_src;
	dstp = dstp_saved = p_dst;

	// copy lines that don't need to be deinterlaced
	srcp = srcp_saved  + ( 1 - order ) * src_pitch;
	dstp = dstp_saved  + ( 1 - order ) * dst_pitch;

	for( INT32 y = 0; y < h2; y += 2 )
	{
		MEMCPY( (UINT32 *)dstp, (UINT32 *)srcp, dst_pitch );
		srcp += 2 * src_pitch;
		dstp += 2 * dst_pitch;
	}

	// Copy through the lines that will be missed below.
	MEMCPY( (UINT32 *)(dstp_saved + order * dst_pitch )					, (UINT32 *)( srcp_saved + (1 - order) * src_pitch )		, dst_pitch );
	MEMCPY( (UINT32 *)(dstp_saved + ( 2 + order ) * dst_pitch )			, (UINT32 *)( srcp_saved + (3 - order) * src_pitch )		, dst_pitch );
	MEMCPY( (UINT32 *)(dstp_saved + ( _size_y - 2 + order ) * dst_pitch ), (UINT32 *)( srcp_saved + (h2 - 1 - order) * src_pitch )	, dst_pitch );
	MEMCPY( (UINT32 *)(dstp_saved + ( _size_y - 4 + order ) * dst_pitch ), (UINT32 *)( srcp_saved + (h2 - 3 - order) * src_pitch )	, dst_pitch );

	TBUF_ADD( tbuf::CH_VIDEO_PROCESS, 4., "video_process", this );

	if ( _b_sharp )
		if ( _b_two_way )
			loop_32<kernel_2_float>( kernel_2_float(), _s_prevFrame2, srcp_saved, dstp_saved, src_pitch, dst_pitch, order, _size_x, h2, _threshold, _b_show_deinterlace );
		else
			loop_32<kernel_1_float>( kernel_1_float(), _s_prevFrame2, srcp_saved, dstp_saved, src_pitch, dst_pitch, order, _size_x, h2, _threshold, _b_show_deinterlace );
	else
		if ( _b_two_way )
			loop_32<kernel_2_int>( kernel_2_int(), _s_prevFrame2, srcp_saved, dstp_saved, src_pitch, dst_pitch, order, _size_x, h2, _threshold, _b_show_deinterlace );
		else
			loop_32<kernel_1_int>( kernel_1_int(), _s_prevFrame2, srcp_saved, dstp_saved, src_pitch, dst_pitch, order, _size_x, h2, _threshold, _b_show_deinterlace );

	TBUF_ADD( tbuf::CH_VIDEO_PROCESS, -4., "video_process", this );

	// save last frame in memory
	MEMCPY(  (UINT32 *)_s_prevFrame2, (UINT32 *)_merge_src, _size_x * h2 * 4 );

	TBUF_ADD( tbuf::CH_VIDEO_PROCESS, 2., "video_process", this );
}

void	c_deint_kernel::close()
{
	SAFE_DELETE_ARRAY( _s_prevFrame2 );
}

void	c_deint_kernel::init( INT32 size_x, INT32 size_y )
{
	_s_prevFrame2 = new UINT8[size_x * size_y * 4];
	if( _s_prevFrame2 )
		MEMSET( _s_prevFrame2, 0, size_x * size_y * sizeof(UINT32) );
	else
		ERR_PRINT_STRING( "Kernel Deinterlace : could not allocate s_prevFrame2" );
}

//void	c_deint_kernel::run_frame( UINT8* src, UINT8* dst )
//{
//}

void	c_deint_kernel::run_full(  c_image_flux* flux_in, UINT8* dst, BOOL second_pass )
{

}

c_deint_kernel::~c_deint_kernel()
{
	close();
}