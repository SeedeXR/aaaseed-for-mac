#include "deinterlace_smartbob.h"
#include "spy.h"
#include "infrastructure/param/param_st.h"

FACTORY_CREATE_V1( c_deint_smartbob, deinterlace_smartbob, Deinterlace Smart Bob, deinterlace_smartbob );

namespace n_deint_smartbob
{
	static	CONST	INT32	BASE_PARAM_NB			= 3;
	static	CONST	INT32	GROUP_NB				= 0;

	static	CONST	INT32	PARAM_NB_MAX =
			BASE_PARAM_NB
		+	GROUP_NB;

	static	ST_PARAM	param[PARAM_NB_MAX] =
	{
		ST_PARAM_REAL_POS( denoise_diameter, 1., 5. )
		ST_PARAM_REAL_POS( denois_threshold, 1., 7. )
		ST_PARAM_BOOL_OFF( show_deinterlace )
	};
}

void	c_deint_smartbob::param_init_pt()
{
INT32	h = 0;

	param_set_pt( h, _denoise_diameter );
	param_set_pt( h, _denoise_threshold );
	param_set_pt( h, _b_show_deinterlace );

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_deint_smartbob)
{
	_min_image_nb = 2;
	param_init_with( n_deint_smartbob::param, n_deint_smartbob::PARAM_NB_MAX);
}

void c_deint_smartbob::init(INT32 size_x, INT32 size_y)
{
	INT32	nb	= size_x * size_y;
	_prevFrame = new UINT32[nb];	//was * height just here ?
	MEMSET( _prevFrame, 0, nb * sizeof(UINT32) );
	_g_moving	= new UINT8[nb];
	MEMSET( _g_moving, 0, nb * sizeof(UINT8) );
	_g_fmoving = new UINT8[nb];
	MEMSET( _g_fmoving, 0, nb * sizeof(UINT8) );
}

BOOL	c_deint_smartbob::is_field_needed( BOOL b_double_framerate )
{
	if( b_double_framerate )
	{
		return TRUE;
	}
	else
		return FALSE;
}

void c_deint_smartbob::smartbob(UINT8* p_src, UINT8* p_dst, INT32 n)
{
	BOOL	bShiftEven = FALSE;
	BOOL	bDenoise = TRUE;
	INT32 	smart_threshold = 12;


	INT32	iOddEven = bShiftEven ? 0 : 1;
	UINT32	*src, *dst, *srcn, *srcnn, *srcp, *prev;
	UINT8	*moving, *fmoving;
	INT32	x, y;
	long	currValue, prevValue, nextValue, nextnextValue, luma, lumap, luman;
	INT32	r, g, b, rp, gp, bp, rn, gn, bn, rnn, gnn, bnn, R, G, B, T = smart_threshold * smart_threshold;
	
	INT32	pitch = _size_x ;
	
	INT32	hminus = _size_y - 1;
	INT32	hminus2 = _size_y - 2;
	INT32	wminus = _size_x - 1;

	/* Calculate the motion map. */
	moving = _g_moving;
	/* Threshold 0 means treat all areas as moving, i.e., dumb bob. */
	if( smart_threshold == 0 )
		{
		MEMSET( moving, 1, _size_y * _size_x );
		}
	else
		{
		MEMSET( moving, 0, _size_y * _size_x );
		src = (UINT32 *)p_src;
		srcn = (UINT32 *)((UINT8 *)src + 4*pitch);
		prev = _prevFrame;
		if(n % 2 == iOddEven)
			prev += _size_x;
		for (y = 0; y < hminus; ++y )
			{
			for (x = 0; x < _size_x; ++x )
				{
				currValue = prev[x];
				nextValue = srcn[x];
				prevValue = src[x];
				r = (currValue >> 16) & 0xff;
				rp = (prevValue >> 16) & 0xff;
				rn = (nextValue >> 16) & 0xff;
				g = (currValue >> 8) & 0xff;
				gp = (prevValue >> 8) & 0xff;
				gn = (nextValue >> 8) & 0xff;
				b = currValue & 0xff;
				bp = prevValue & 0xff;
				bn = nextValue & 0xff;
				luma = (55 * r + 182 * g + 19 * b) >> 8;
				lumap = (55 * rp + 182 * gp + 19 * bp) >> 8;
				luman = (55 * rn + 182 * gn + 19 * bn) >> 8;
				if ((lumap - luma) * (luman - luma) >= T)
					moving[x] = 1;
				}
			src = (UINT32 *)((UINT8 *)src + 4*pitch);
			srcn = (UINT32 *)((UINT8 *)srcn + 4*pitch);
			moving += _size_x;
			prev += _size_x;
			}
		/* Can't diff the last line. */
		MEMSET(moving, 0, _size_x);

		/* Motion map denoising. */
		if( bDenoise )
			{
			INT32 xlo, xhi, ylo, yhi, xsize;
			INT32 u, v;
			INT32 N = _denoise_diameter;
			INT32 Nover2 = N/2;
			INT32 sum;
			UINT8 *m;


			// Erode.
			moving = _g_moving;
			fmoving = _g_fmoving;
			for (y = 0; y < _size_y; ++y )
				{
				for (x = 0; x < _size_x; ++x )
					{
					if (moving[x] == 0)
						{
						fmoving[x] = 0;	
						continue;
						}
					xlo = x - Nover2; if( xlo < 0 )		xlo = 0;
					xhi = x + Nover2; if( xhi >= _size_x ) xhi = wminus;
					ylo = y - Nover2; if( ylo < 0 )		ylo = 0;
					yhi = y + Nover2; if( yhi >= _size_y )	yhi = hminus;
					for(u = ylo, sum = 0, m = _g_moving + ylo * _size_x; u <= yhi; ++u )
						{
						for(v = xlo; v <= xhi; ++v )
							{
							sum += m[v];
							}
						m += _size_x;
						}
					if( sum > _denoise_threshold )
						fmoving[x] = 1;
					else
						fmoving[x] = 0;
					}
				moving += _size_x;
				fmoving += _size_x;
				}

			// Dilate.
			moving = _g_moving;
			fmoving = _g_fmoving;
			for (y = 0; y < _size_y; ++y )
				{
				for (x = 0; x < _size_x; ++x )
					{
					if (fmoving[x] == 0)
						{
						moving[x] = 0;	
						continue;
						}
					xlo = x - Nover2;
					if (xlo < 0) xlo = 0;
					xhi = x + Nover2;
					/* Use _size_x here instead of wminus so we don't have to add 1 in the
					   the assignment of xsize. */
					if (xhi >= _size_x) xhi = _size_x;
					xsize = xhi - xlo;
					ylo = y - Nover2;
					if (ylo < 0) ylo = 0;
					yhi = y + Nover2;
					if (yhi >= _size_y) yhi = hminus;
					m = _g_moving + ylo * _size_x;
					for (u = ylo; u <= yhi; ++u )
						{
						MEMSET(&m[xlo], 1, xsize); 
						m += _size_x;
						}
					}
				moving += _size_x;
				fmoving += _size_x;
				}
			}
		}

	/* Output the destination frame. */
		/* Output the destination frame. */
		src = (UINT32 *)p_src;
		srcn = (UINT32 *)(p_src + pitch);
		srcnn = (UINT32 *)(p_src + 2 * pitch);
		srcp = (UINT32 *)(p_src - pitch);
		dst = (UINT32 *)p_dst;
		if(n % 2 == iOddEven)
			{
			/* Shift this frame's output up by one line. */
			MEMCPY( dst, src, _size_x * sizeof(UINT32));
//			dst = (UINT32 *)((UINT8 *)dst + pitch);
			dst = dst + pitch;
			prev = _prevFrame + _size_x;
			}
		else
			{
			prev = _prevFrame;
			}
		moving = _g_moving;
		for (y = 0; y < hminus; ++y )
			{
			/* Even output line. Pass it through. */
			MEMCPY( dst, src, _size_x * sizeof(UINT32));
//			dst = (UINT32 *)((UINT8 *)dst + pitch);
			dst = dst + pitch;
			/* Odd output line. Synthesize it. */
			for (x = 0; x < _size_x; ++x )
				{
				if (moving[x] == 1)
					{
					/* Make up a new line. Use cubic interpolation where there
					   are enough samples and linear where there are not enough. */
					nextValue = srcn[x];
					r = (src[x] >> 16) & 0xff;
					rn = (nextValue >> 16) & 0xff;
					g = (src[x] >> 8) & 0xff;
					gn = (nextValue >>8) & 0xff;
					b = src[x] & 0xff;
					bn = nextValue & 0xff;
					if (y == 0 || y == hminus2)
						{	/* Not enough samples; use linear. */
						R = (r + rn) >> 1;
						G = (g + gn) >> 1;
						B = (b + bn) >> 1;
						}
					else
						{
						/* Enough samples; use cubic. */
						prevValue = srcp[x];
						nextnextValue = srcnn[x];
						rp = (prevValue >> 16) & 0xff;
						rnn = (nextnextValue >>16) & 0xff;
						gp = (prevValue >> 8) & 0xff;
						gnn = (nextnextValue >> 8) & 0xff;
						bp = prevValue & 0xff;
						bnn = nextnextValue & 0xff;
						R = (5 * (r + rn) - (rp + rnn)) >> 3;
						if (R > 255) R = 255;
						else if (R < 0) R = 0;
						G = (5 * (g + gn) - (gp + gnn)) >> 3;
						if (G > 255) G = 255;
						else if (G < 0) G = 0;
						B = (5 * (b + bn) - (bp + bnn)) >> 3;
						if (B > 255) B = 255;
						else if (B < 0) B = 0;
						}
					dst[x] = (0x000000ff << 24) | (R << 16) | (G << 8) | B;  
					}
				else
					{
					/* Use line from previous field. */
					dst[x] = prev[x];
					}
				}
			src = (UINT32 *)((UINT8 *)src + 4*pitch);
			srcn = (UINT32 *)((UINT8 *)srcn + 4*pitch);
			srcnn = (UINT32 *)((UINT8 *)srcnn + 4*pitch);
			srcp = (UINT32 *)((UINT8 *)srcp + 4*pitch);
			dst = (UINT32 *)((UINT8 *)dst + pitch);
			moving += _size_x;
			prev += _size_x;
			}
		/* Copy through the last source line. */

		MEMCPY( dst, src, _size_x * sizeof(UINT32));
		if(n % 2 != iOddEven)
			{
			dst = (UINT32 *)((UINT8 *)dst + pitch);
			MEMCPY( dst, src, _size_x * sizeof(UINT32));
			}
	

	/* Buffer the input frame (aka field). */
	src = (UINT32 *)p_src;
	prev = _prevFrame;
	for (y = 0; y < _size_y; ++y )
		{
		MEMCPY( prev, src, _size_x * sizeof(UINT32));
		src = (UINT32 *)((UINT8 *)src + pitch);
		prev += _size_x;
		}
}

void	c_deint_smartbob::close()
{

}

void	c_deint_smartbob::run_frame( UINT8* src, UINT8* dst )
{
}

void	c_deint_smartbob::run_full(  c_image_flux* flux_in, UINT8* dst, BOOL second_pass )
{

}

c_deint_smartbob::~c_deint_smartbob()
{
	close();
}