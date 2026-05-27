#include "deinterlace_smart.h"
#include "spy.h"
#include "infrastructure/param/param_st.h"

//#define FRAME_ONLY 0
//#define FIELD_ONLY 1
//#define FRAME_AND_FIELD 2

FACTORY_CREATE_V1( c_deint_smart, deinterlace_smart, Deinterlace Smart, deinterlace_smart );


enum DEINTERLACE_DIFF_TYPE
{
	FRAME_ONLY = 0,
	FIELD_ONLY, 
	FRAME_AND_FIELD, 
	DEINTERLACE_DIFF_MAX_NB,
};

char*	deinterlace_diff_str[DEINTERLACE_DIFF_MAX_NB] =
{
	"Frame",
	"Field",
	"Frame & Field",
};

char*	deinterlace_blend_str[c_deinterlace::BLEND_MAX_NB] =
{
	"Copy",
	"Blend",
	"Cubic",
	"Edge Line Average",
};

char*	deinterlace_simd_str[c_deinterlace::SIMD_MAX_NB] =
{
	"None",
	"MMX",
	"SSE2",
};

namespace n_deint_smart
{
	static	CONST	INT32	BASE_PARAM_NB			= 7;
	static	CONST	INT32	GROUP_NB				= 0;

	static	CONST	INT32	PARAM_NB_MAX =
			BASE_PARAM_NB
		+	GROUP_NB;

	static	ST_PARAM	param[PARAM_NB_MAX] =
	{
		ST_PARAM_BOOL_OFF( show_deinterlace )
		ST_PARAM_INT32_POS( difference_threshold, 15., 15. )
		ST_PARAM_SYMBO( motion_map_mode, 1., 0., DEINTERLACE_DIFF_MAX_NB-1, deinterlace_diff_str )
//		ST_PARAM_BOOL_OFF( motion_map_full )
		ST_PARAM_BOOL_OFF( motion_map_rgb )
		ST_PARAM_INT32_POS( motion_map_scene_threshold, 15., 100. )
		ST_PARAM_BOOL_OFF( motion_map_denoising )
		ST_PARAM_SYMBO( blend_mode, 1., 0., c_deinterlace::BLEND_MAX_NB-1, deinterlace_blend_str )
//		ST_PARAM_BOOL_OFF( memory_access_32_bits )
//		ST_PARAM_SYMBO( simd, 1., 0., c_deinterlace::SIMD_MAX_NB-1, deinterlace_simd_str )
	};
}

void	c_deint_smart::param_init_pt()
{
INT32	h=0;

	param_set_pt( h, _b_show_deinterlace );
	param_set_pt( h, _smart_threshold );
	param_set_pt( h, _s_smart_motion_map_mode );
//	param_set_pt( h, _b_smart_motionmap_full );
	param_set_pt( h, _b_smart_colordiff );
	param_set_pt( h, _smart_scene_threshold );
	param_set_pt( h, _b_smart_highquality );
	param_set_pt( h, _smart_blend_mode );
//	param_set_pt( h, b_smart_32_bits_ );
//	param_set_pt( h, s_smart_simd_ );

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_deint_smart)
{
	_merge_src = nullptr;
	_prevFrame = nullptr;
	_saveFrame = nullptr;
	_g_moving = nullptr;
	_g_fmoving = nullptr;
//	s_prevFrame2 = NULL;

	_min_image_nb = 4;
//	set_name("deint_smart");
	param_init_with( n_deint_smart::param, n_deint_smart::PARAM_NB_MAX);
}

c_deint_smart::~c_deint_smart()
{
	close();
}

BOOL	c_deint_smart::is_field_needed( BOOL b_double_framerate )
{
	if( b_double_framerate )
	{
		return TRUE;
	}
	else
		return FALSE;
}


void c_deint_smart::init( INT32 size_x, INT32 size_y )
{
	//width = w;
	//height = h;
	INT32	nb	= size_x * size_y;
	_merge_src = new UINT8[nb * 4];
	if( _merge_src )
		MEMSET( _merge_src, 0, nb * 4 );
	else
		ERR_PRINT_STRING( "Deinterlace : could not allocate merge_src" );

	_prevFrame = new UINT32[nb];
	if( _prevFrame )
		MEMSET( _prevFrame, 0, nb * sizeof(UINT32) );
	else
		ERR_PRINT_STRING( "Deinterlace : could not allocate prevFrame" );

	_saveFrame = new UINT32[nb];
	if( _saveFrame )
		MEMSET( _saveFrame, 0, nb * sizeof(UINT32) );
	else
		ERR_PRINT_STRING( "Deinterlace : could not allocate saveFrame" );

	_g_moving	= new UINT8[nb];
	if( _g_moving )
		MEMSET( _g_moving, 0, nb * sizeof(UINT8) );
	else
		ERR_PRINT_STRING( "Deinterlace : could not allocate g_moving" );

	_g_fmoving = new UINT8[nb];
	if( _g_fmoving )
		MEMSET( _g_fmoving, 0, nb * sizeof(UINT8) );
	else
		ERR_PRINT_STRING( "Deinterlace : could not allocate g_fMoving" );

	//s_prevFrame2 = new UINT8[nb * 4];
	//if( s_prevFrame2 )
	//	MEMSET( s_prevFrame2, 0, nb * sizeof(UINT32) );
	//else
	//	ERR_PRINT_STRING( "Deinterlace : could not allocate s_prevFrame2" );
	
}

void c_deint_smart::close()
{
	SAFE_DELETE_ARRAY( _merge_src );
	SAFE_DELETE_ARRAY( _prevFrame );
	SAFE_DELETE_ARRAY( _saveFrame );
	SAFE_DELETE_ARRAY( _g_moving );
	SAFE_DELETE_ARRAY( _g_fmoving );
}

INT32 c_deint_smart::smart_32_motion_map_frame_rgb(UINT32* p_src )
{
	UINT32*		src;
	UINT32*		srcminus;
	UINT32*		prev;
	UINT8*		moving;
	//INT32		x, y;
	INT32		hminus1 = _size_y - 1;
	INT32		prevValue;
	INT32		r, g, b, rp, gp, bp;
	INT32		count;
	INT32		pitch = _size_x * 4;

	src = p_src;
	/* Skip first and last lines, they'll get a free ride. */
	src = (UINT32 *)( (UINT8 *)src + pitch );
	srcminus = (UINT32 *)( (UINT8 *)src - pitch );
	prev = _prevFrame + _size_x;
	moving = _g_moving + _size_x;
	count = 0;
	for( INT32 y = 1; y < hminus1; ++y )
		{
		for( INT32 x = 0; x < _size_x; ++x )
			{
			// First check frame motion.
			// Set the moving flag if the diff exceeds the configured threshold.
			moving[x] = 0;
			prevValue = *prev;
			b = src[x] & 0xff;
			bp = prevValue & 0xff;
			if( ABS( b - bp ) > _smart_threshold )
				{
					moving[x] = 1;
					/* Keep a count of the number of moving pixels for the scene change detection. */
					count++;
				}
			else
				{
				r = (src[x] >>16) & 0xff;
				rp = (prevValue >> 16) & 0xff;
				if( ABS( r - rp ) > _smart_threshold )
					{
						moving[x] = 1;
						/* Keep a count of the number of moving pixels for the scene change detection. */
						count++;
					}
				else
					{
					g = (src[x] >> 8) & 0xff;
					gp = (prevValue >> 8) & 0xff;
					if( ABS( g - gp ) > _smart_threshold )
						{
							moving[x] = 1;
							/* Keep a count of the number of moving pixels for the scene change detection. */
							count++;
						}
					}
				}
			*prev++ = src[x];
			}
		src = (UINT32 *)((UINT8 *)src + pitch);
		srcminus = (UINT32 *)((UINT8 *)srcminus + pitch);
		moving += _size_x;
	}
	return count;
}

INT32 c_deint_smart::smart_32_motion_map_frame_luma(UINT32* p_src )
{
	UINT32*		src;
	UINT32*		srcminus;
	UINT32*		prev;
	UINT8*		moving;
	INT32		x, y;
	INT32		hminus1 = _size_y - 1;
	INT32		prevValue;
	INT32		r, g, b, luma;
	INT32		count = 0;
	INT32		tmp_threshold = _smart_threshold << 8;
	INT32		pitch = _size_x * 4;

	src = p_src;
	/* Skip first and last lines, they'll get a free ride. */
	src = (UINT32 *)((UINT8 *)src + pitch);
	srcminus = (UINT32 *)((UINT8 *)src - pitch);
	prev = _prevFrame + _size_x;
	moving = _g_moving + _size_x;
	for (y = 1; y < hminus1; ++y )
		{
		for (x = 0; x < _size_x; ++x )
			{
			// First check frame motion.
			// Set the moving flag if the diff exceeds the configured threshold.
			moving[x] = 0;
			prevValue = *prev;
			r = (src[x] >> 16) & 0xff;
			g = (src[x] >> 8) & 0xff;
			b = src[x] & 0xff;
			//if (r > 0 || b>0 || g>>0)
			//	DBG_PRINT_STRING("superior a 0");
			luma = (76 * r + 30 * b + 150 * g); // >> 8;
			if (ABS(luma - prevValue) > tmp_threshold )
				{
				moving[x] = 1;
				/* Keep a count of the number of moving pixels for the scene change detection. */
				count++;
				}
			*prev++ = luma;
			}
		src = (UINT32 *)((UINT8 *)src + pitch);
		srcminus = (UINT32 *)((UINT8 *)srcminus + pitch);
		moving += _size_x;
		}	
	return count;
}



INT32 c_deint_smart::smart_32_motion_map_framefield_rgb( UINT32* p_src )
{
	UINT32*		src;
	UINT32*		srcminus;
	UINT32*		prev;
	UINT8*		moving;
	INT32		x, y;
	INT32		hminus1 = _size_y - 1;
	INT32		prevValue;
	INT32		r, g, b, rp, gp, bp;
	INT32		count = 0;
	UINT8		frMotion, fiMotion;
	INT32		pitch = _size_x * 4;

	src = p_src;

	/* Skip first and last lines, they'll get a free ride. */
	src = (UINT32 *)((UINT8 *)src + pitch);
	srcminus = (UINT32 *)((UINT8 *)src - pitch);
	prev = _prevFrame + _size_x;
	moving = _g_moving + _size_x;
	for (y = 1; y < hminus1; ++y )
		{
		for (x = 0; x < _size_x; ++x )
			{
			// First check frame motion.
			// Set the moving flag if the diff exceeds the configured threshold.
			moving[x] = 0;
			frMotion = 0;
			prevValue = *prev;
			b = src[x] & 0xff;
			bp = prevValue & 0xff;
			if (ABS(b - bp) > _smart_threshold ) frMotion = 1;
			else
				{
				r = (src[x] >>16) & 0xff;
				rp = (prevValue >> 16) & 0xff;
				if (ABS(r - rp) > _smart_threshold ) frMotion = 1;
				else
					{
					g = (src[x] >> 8) & 0xff;
					gp = (prevValue >> 8) & 0xff;
					if (ABS(g - gp) > _smart_threshold ) frMotion = 1;
					}
				}
			fiMotion = 0;
			if (y & 1)
				prevValue = srcminus[x];
			else
				prevValue = *(prev + _size_x);

			// second check field motion
			b = src[x] & 0xff;
			bp = prevValue & 0xff;
			if (ABS(b - bp) > _smart_threshold ) fiMotion = 1;
			else
				{
				r = (src[x] >> 16) & 0xff;
				rp = (prevValue >> 16) & 0xff;
				if (ABS(r - rp) > _smart_threshold ) fiMotion = 1;
				else
					{
					g = (src[x] >> 8) & 0xff;
					gp = (prevValue >> 8) & 0xff;
					if (ABS(g - gp) > _smart_threshold ) fiMotion = 1;
					}
				}
			moving[x] = (fiMotion && frMotion );

			*prev++ = src[x];
			/* Keep a count of the number of moving pixels for the scene change detection. */
			if (moving[x]) count++;
			}
		src = (UINT32 *)((UINT8 *)src + pitch );
		srcminus = (UINT32 *)((UINT8 *)srcminus + pitch );
		moving += _size_x;
	}
	return count;
}

INT32 c_deint_smart::smart_32_motion_map_framefield_luma(UINT32* p_src )
{
	UINT32*		src;
	UINT32*		srcminus;
	UINT32*		prev;
	UINT8*		moving;
	INT32		x, y;
	INT32		hminus1 = _size_y - 1;
	INT32		prevValue;
	INT32		r, g, b, luma;
	INT32		count = 0;
	UINT8		frMotion, fiMotion;
	INT32		tmp_threshold = _smart_threshold << 8;
	INT32		pitch = _size_x * 4;

	src = p_src;

	/* Skip first and last lines, they'll get a free ride. */
	src = (UINT32 *)((UINT8 *)src + pitch);
	srcminus = (UINT32 *)((UINT8 *)src - pitch);
	prev = _prevFrame + _size_x;
	moving = _g_moving + _size_x;
	for (y = 1; y < hminus1; ++y )
		{
		for (x = 0; x < _size_x; ++x )
			{
			// First check frame motion.
			// Set the moving flag if the diff exceeds the configured threshold.
			moving[x] = 0;
			frMotion = 0;
			prevValue = *prev;

			r = (src[x] >> 16) & 0xff;
			g = (src[x] >> 8) & 0xff;
			b = src[x] & 0xff;
			luma = (76 * r + 30 * b + 150 * g); // >> 8;
			if (ABS(luma - prevValue) > tmp_threshold ) frMotion = 1;

			// Now check field motion if applicable.
			fiMotion = 0;
			if (y & 1)
				prevValue = srcminus[x];
			else
				prevValue = *(prev + _size_x);

			r = (src[x] >> 16) & 0xff;
			g = (src[x] >> 8) & 0xff;
			b = src[x] & 0xff;
			luma = (76 * r + 30 * b + 150 * g); // >> 8;
			if (ABS(luma - prevValue) > tmp_threshold ) fiMotion = 1;

			moving[x] = (fiMotion && frMotion);

			*prev++ = luma;

			/* Keep a count of the number of moving pixels for the scene change detection. */
			if (moving[x]) count++;
			}
		src = (UINT32 *)((UINT8 *)src + pitch);
		srcminus = (UINT32 *)((UINT8 *)srcminus + pitch);
		moving += _size_x;
	}
	return count;
}

INT32 c_deint_smart::smart_32_field_motion_map_field_rgb(UINT32* p_src_a, UINT32* p_src_b )
{
	UINT32*		src;
	UINT32*		srcminus;
	UINT32*		srcplus;
	UINT8*		moving;
	INT32		y;
	INT32		hminus1 = _size_y - 1;
	INT32		prevValue, nextValue;
	INT32		r, g, b, rp, gp, bp, bn, gn, rn, T;
	INT32		count = 0;
	INT32		pitch = _size_x * 4;

	src = p_src_a;

	/* Field differencing only mode. */
	T = _smart_threshold * _smart_threshold;
	src = (UINT32 *)((UINT8 *)src + pitch);
	srcminus = p_src_b; //(UINT32 *)((UINT8 *)src - pitch);
	srcplus = p_src_b + pitch; //(UINT32 *)((UINT8 *)src + pitch);
	moving = _g_moving + _size_x;
	for (y = 1; y < hminus1; ++y )
		{
		// ignore odd field
		if (y & 1)
			{
			MEMSET(moving, 0x0, _size_x * sizeof(UINT8));
			}
		else
			{
			for( INT32 x = _size_x - 1; x >= 0; --x)
				{
				// Set the moving flag if the diff exceeds the configured threshold.
				moving[x] = 0;

				// Now check field motion.
				nextValue = srcplus[x];
				prevValue = srcminus[x];

				b = src[x] & 0xff;
				bp = prevValue & 0xff;
				bn = nextValue & 0xff;
				if ((bp - b) * (bn - b) > T)
					{
					moving[x] = 1;
					/* Keep a count of the number of moving pixels for the scene change detection. */
					count++;
					}
				else
					{
					r = (src[x] >> 16) & 0xff;
					rp = (prevValue >> 16) & 0xff;
					rn = (nextValue >> 16) & 0xff;
					if ((rp - r) * (rn - r) > T)
						{
						moving[x] = 1;
						/* Keep a count of the number of moving pixels for the scene change detection. */
						count++;
						}
					else
						{
						g = (src[x] >> 8) & 0xff;
						gp = (prevValue >> 8) & 0xff;
						gn = (nextValue >> 8) & 0xff;
						if ((gp - g) * (gn - g) > T)
							{
							moving[x] = 1;
							/* Keep a count of the number of moving pixels for the scene change detection. */
							count++;
							}
						}
					}
				}
			}
		src = (UINT32 *)((UINT8 *)src + pitch);
		srcminus = (UINT32 *)((UINT8 *)srcminus + pitch);
		srcplus = (UINT32 *)((UINT8 *)srcplus + pitch);
		moving += _size_x;
		}
	return count;
}

INT32 c_deint_smart::smart_32_motion_map_field_rgb(UINT32* p_src )
{
	UINT32*		src;
	UINT32*		srcminus;
	UINT32*		srcplus;
	UINT8*		moving;
	INT32		y;
	INT32		hminus1 = _size_y - 1;
	INT32		prevValue, nextValue;
	INT32		r, g, b, rp, gp, bp, bn, gn, rn, T;
	INT32		count = 0;
	INT32		pitch = _size_x * 4;

	src = p_src;

	/* Field differencing only mode. */
	T = _smart_threshold * _smart_threshold;
	src = (UINT32 *)((UINT8 *)src + pitch);
	srcminus = (UINT32 *)((UINT8 *)src - pitch);
	srcplus = (UINT32 *)((UINT8 *)src + pitch);
	moving = _g_moving + _size_x;
	for (y = 1; y < hminus1; ++y )
		{
		// ignore odd field
		if (y & 1)
			{
			MEMSET(moving, 0x0, _size_x * sizeof(UINT8));
			}
		else
			{
			for( INT32 x = _size_x - 1; x >= 0; --x)
				{
				// Set the moving flag if the diff exceeds the configured threshold.
				moving[x] = 0;
				// Now check field motion.
				nextValue = srcplus[x];
				prevValue = srcminus[x];

				b = src[x] & 0xff;
				bp = prevValue & 0xff;
				bn = nextValue & 0xff;
				if ((bp - b) * (bn - b) > T)
					{
					moving[x] = 1;
					/* Keep a count of the number of moving pixels for the scene change detection. */
					count++;
					}
				else
					{
					r = (src[x] >> 16) & 0xff;
					rp = (prevValue >> 16) & 0xff;
					rn = (nextValue >> 16) & 0xff;
					if ((rp - r) * (rn - r) > T)
						{
						moving[x] = 1;
						/* Keep a count of the number of moving pixels for the scene change detection. */
						count++;
						}
					else
						{
						g = (src[x] >> 8) & 0xff;
						gp = (prevValue >> 8) & 0xff;
						gn = (nextValue >> 8) & 0xff;
						if ((gp - g) * (gn - g) > T)
							{
							moving[x] = 1;
							/* Keep a count of the number of moving pixels for the scene change detection. */
							count++;
							}
						}
					}
				}
			}
		src = (UINT32 *)((UINT8 *)src + pitch);
		srcminus = (UINT32 *)((UINT8 *)srcminus + pitch);
		srcplus = (UINT32 *)((UINT8 *)srcplus + pitch);
		moving += _size_x;
		}
	return count;
}

INT32 c_deint_smart::smart_32_motion_map_field_luma(UINT32* p_src )
{
	UINT32*		src = p_src;
	UINT32*		srcminus;
	UINT32*		srcplus;
	UINT8*		moving;
	INT32		y;
	INT32		hminus1 = _size_y - 1;
	INT32		prevValue, nextValue;
	INT32		r, g, b, rp, gp, bp, rn, bn, gn, T, luma, lumap, luman;
	INT32		count = 0;
	INT32		pitch = _size_x * 4;

	/* Field differencing only mode. */
	T = ( _smart_threshold * _smart_threshold ) << 16;
	src = (UINT32 *)((UINT8 *)src + pitch);
	srcminus = (UINT32 *)((UINT8 *)src - pitch);
	srcplus = (UINT32 *)((UINT8 *)src + pitch);
	moving = _g_moving + _size_x;
	for (y = 1; y < hminus1; ++y )
		{
		// ignore odd field
		if (y & 1)
			{
			MEMSET(moving, 0x00, _size_x * sizeof(UINT8));
			}
		else
			{
			for( INT32 x = _size_x - 1; x >= 0; --x)
				{
				// Set the moving flag if the diff exceeds the configured threshold.
				moving[x] = 0;
				// Now check field motion.
				nextValue = srcplus[x];
				prevValue = srcminus[x];
				r = (src[x] >> 16) & 0xff;
				rp = (prevValue >> 16) & 0xff;
				rn = (nextValue >> 16) & 0xff;
				g = (src[x] >> 8) & 0xff;
				gp = (prevValue >> 8) & 0xff;
				gn = (nextValue >> 8) & 0xff;
				b = src[x] & 0xff;
				bp = prevValue & 0xff;
				bn = nextValue & 0xff;
				luma = (76 * r + 30 * b + 150 * g); // >> 8;
				lumap = (76 * rp + 30 * bp + 150 * gp); // >> 8;
				luman = (76 * rn + 30 * bn + 150 * gn); // >> 8;
				if ((lumap - luma) * (luman - luma) > T)
					{
					moving[x] = 1;
					/* Keep a count of the number of moving pixels for the scene change detection. */
					count++;
					}
				}
			}
		src = (UINT32 *)((UINT8 *)src + pitch);
		srcminus = (UINT32 *)((UINT8 *)srcminus + pitch);
		srcplus = (UINT32 *)((UINT8 *)srcplus + pitch);
		moving += _size_x;
	}
	return count;
}

void c_deint_smart::smart_32_motion_map_denoise()
{
	INT32		x, y;
	INT32		xlo, xhi, ylo, yhi;
	INT32		u, v;
	INT32		n = 5;
	INT32		nover2 = n/2;
	INT32		sum;
	UINT8*		m;
	UINT8		*moving;
	UINT8		*fmoving;
	INT32		hminus1 = _size_y - 1;
	INT32		wminus1 = _size_x - 1;

	/* Perform a denoising of the motion map if enabled. */

	// Erode.
	fmoving = _g_fmoving;
	for (y = 0; y < _size_y; ++y )
		{
		for (x = 0; x < _size_x; ++x )
			{
			if (!((_g_moving + y * _size_x)[x]))
				{
				fmoving[x] = 0;
				continue;
				}
			xlo = x - nover2; if (xlo < 0) xlo = 0;
			xhi = x + nover2; if (xhi >= _size_x) xhi = wminus1;
			ylo = y - nover2; if (ylo < 0) ylo = 0;
			yhi = y + nover2; if (yhi >= _size_y) yhi = hminus1;
			m = _g_moving + ylo * _size_x;
			sum = 0;
			for (u = ylo; u <= yhi; ++u )
				{
				for (v = xlo; v <= xhi; ++v )
					{
					sum += m[v];
					}
				m += _size_x;
				}
			if (sum > 9)
				fmoving[x] = 1;
			else
				fmoving[x] = 0;
			}
		fmoving += _size_x;
		}
	// Dilate.
	moving = _g_moving;
	for (y = 0; y < _size_y; ++y )
		{
		for (x = 0; x < _size_x; ++x )
			{
			if (!((_g_fmoving + y * _size_x)[x]))
				{
				moving[x] = 0;	
				continue;
				}
			xlo = x - nover2; if (xlo < 0) xlo = 0;
			xhi = x + nover2; if (xhi >= _size_x) xhi = wminus1;
			ylo = y - nover2; if (ylo < 0) ylo = 0;
			yhi = y + nover2; if (yhi >= _size_y) yhi = hminus1;
			m = _g_moving + ylo * _size_x;
			for (u = ylo; u <= yhi; ++u )
				{
				for (v = xlo; v <= xhi; ++v )
					{
					m[v] = 1;
					}
				m += _size_x;
				}
			}
		moving += _size_x;
		}
}

void c_deint_smart::smart_32_copy(UINT32* p_src, UINT32* p_dst, INT32 s_scenechange)
{
	UINT32*		src = p_src;
	UINT32*		srcminus;
	UINT32*		srcplus;
	UINT32*		dst = p_dst;
	UINT8*		moving;
	UINT8*		movingminus;
	UINT8*		movingplus;
	
	INT32		x;
	INT32		pitch = _size_x * 4;
	INT32		hminus1 = _size_y - 1;
	UINT32		p0, p1, p2;

	// Render.
	// The first line gets a free ride.
	MEMCPY( dst, src, pitch);

	src = (UINT32 *)((UINT8 *)src + pitch);
	srcminus = (UINT32 *)((UINT8 *)src - pitch);
	srcplus = (UINT32 *)((UINT8 *)src + pitch);
	dst = (UINT32 *)((UINT8 *)dst + pitch);

	moving = _g_moving + _size_x;
	movingminus = moving - _size_x;
	movingplus = moving + _size_x;

	if( _b_show_deinterlace )
		for( INT32 y = 1; y < hminus1; ++y )
			{
			x = 0;
			do {
				if (!(movingminus[x] | moving[x] | movingplus[x]) && !s_scenechange)
					dst[x] = 0xff7f7f7f;
				else
					{	
					/* Blend fields. */
					p0 = src[x];
					p0 &= 0x00fefefe;

					p1 = srcminus[x];
					p1 &= 0x00fcfcfc;

					p2 = srcplus[x];
					p2 &= 0x00fcfcfc;

					dst[x] = (0xff000000) | ((p0>>1) + (p1>>2) + (p2>>2));
					}
//				dst[x] |= 0xff000000;
				}
			while(++x < _size_x);
			
			src = (UINT32 *)((UINT8 *)src + pitch);
			srcminus = (UINT32 *)((UINT8 *)srcminus + pitch);
			srcplus = (UINT32 *)((UINT8 *)srcplus + pitch);

			dst = (UINT32 *)((UINT8 *)dst + pitch);

			moving += _size_x;
			movingminus += _size_x;
			movingplus += _size_x;
		}
	else
		{
		for( INT32 y = 1; y < hminus1; ++y )
			{
			x = 0;
			do
				{
				if( !( moving[x]) && !s_scenechange )
//				if (!(movingminus[x] | moving[x] | movingplus[x]) && !s_scenechange)
					{
					if( _b_show_deinterlace )
						dst[x] = src[x] & 0x00ffffff;
					else
						dst[x] = src[x] | 0xff000000;
					}
				else
					{
					/* Copy fields. */
					dst[x] = 0xff000000 | srcminus[x]; 
					}
				}
			while(++x < _size_x);
				
			src = (UINT32 *)((UINT8 *)src + pitch);
			srcminus = (UINT32 *)((UINT8 *)srcminus + pitch);
			srcplus = (UINT32 *)((UINT8 *)srcplus + pitch);
			dst = (UINT32 *)((UINT8 *)dst + pitch);

			moving += _size_x;
			movingminus += _size_x;
			movingplus += _size_x;
			}
		}
	MEMCPY( dst, src, pitch );
}


void c_deint_smart::smart_32_blend(UINT32* p_src, UINT32* p_dst, INT32 s_scenechange )
{
	UINT32*		src = p_src;
	UINT32*		srcminus;
	UINT32*		srcplus;
	UINT32*		dst = p_dst;
	UINT8*		moving;
	UINT8*		movingminus;
	UINT8*		movingplus;
	
	INT32		x, y;
	INT32		pitch = _size_x * 4;
	INT32		hminus1 = _size_y - 1;
	UINT32		p0, p1, p2;

	// Render.
	// The first line gets a free ride.
	MEMCPY( dst, src, pitch );

	src = (UINT32 *)( (UINT8 *)src + pitch );
	srcminus = (UINT32 *)( (UINT8 *)src - pitch );
	srcplus = (UINT32 *)( (UINT8 *)src + pitch );
	dst = (UINT32 *)( (UINT8 *)dst + pitch );

	moving = _g_moving + _size_x;
	movingminus = moving - _size_x;
	movingplus = moving + _size_x;

	if (_b_show_deinterlace )
		for (y = 1; y < hminus1; ++y )
			{
			x = 0;
			do {
				if (!(movingminus[x] | moving[x] | movingplus[x]) && !s_scenechange )
					dst[x] = 0xff7f7f7f;
				else
					{	
					/* Blend fields. */
					p0 = src[x];
					p0 &= 0x00fefefe;

					p1 = srcminus[x];
					p1 &= 0x00fcfcfc;

					p2 = srcplus[x];
					p2 &= 0x00fcfcfc;

					dst[x] = (0xff000000) | ((p0>>1) + (p1>>2) + (p2>>2));
					}
				}
			while(++x < _size_x);
			
			src = (UINT32 *)((UINT8 *)src + pitch);
			srcminus = (UINT32 *)((UINT8 *)srcminus + pitch);
			srcplus = (UINT32 *)((UINT8 *)srcplus + pitch);

			dst = (UINT32 *)((UINT8 *)dst + pitch);

			moving += _size_x;
			movingminus += _size_x;
			movingplus += _size_x;
		}
	else
		{
		for (y = 1; y < hminus1; ++y )
			{
			x = 0;
			do
				{
				if (!(movingminus[x] | moving[x] | movingplus[x]) && !s_scenechange)
					dst[x] = (0xff000000) | src[x];
				else
					{
					/* Blend fields. */
					p0 = src[x];
					p0 &= 0x00fefefe;

					p1 = srcminus[x];
					p1 &= 0x00fcfcfc;

					p2 = srcplus[x];
					p2 &= 0x00fcfcfc;

					dst[x] = (0xff000000) | ((p0>>1) + (p1>>2) + (p2>>2));
					}
				}
			while(++x < _size_x);
				
			src = (UINT32 *)((UINT8 *)src + pitch);
			srcminus = (UINT32 *)((UINT8 *)srcminus + pitch);
			srcplus = (UINT32 *)((UINT8 *)srcplus + pitch);

			dst = (UINT32 *)((UINT8 *)dst + pitch);

			moving += _size_x;
			movingminus += _size_x;
			movingplus += _size_x;
			}
		}

	// The last line gets a free ride.
	MEMCPY( dst, src, pitch);
}

void c_deint_smart::smart_32_cubic(UINT32* p_src, UINT32* p_dst, INT32 s_scenechange )
{

	UINT32*		src;
	UINT32*		srcminus;
	UINT32*		srcplus;
	UINT32*		srcminusminus;
	UINT32*		srcplusplus;
	UINT32*		dst;
	UINT8*		moving;
	UINT8*		movingminus;
	UINT8*		movingplus;
	
	INT32		x, y;
	INT32		pitch = _size_x * 4;
	INT32		hminus1 = _size_y - 1;
	INT32		hminus3 = _size_y - 3;
	UINT32		p1, p2;
	INT32		rpp, gpp, bpp, rnn, gnn, bnn, R, G, B;
	INT32		rp, gp, bp, rn, bn, gn;

	src = p_src;
	dst = p_dst;

	// Render.
	// The first line gets a free ride.
	MEMCPY( dst, src, pitch);

	src = (UINT32 *)( (UINT8 *)src + pitch );
	srcminus = (UINT32 *)( (UINT8 *)src - pitch );
	srcplus = (UINT32 *)( (UINT8 *)src + pitch );

	srcminusminus = (UINT32 *)( (UINT8 *)src - 3 * pitch );
	srcplusplus = (UINT32 *)( (UINT8 *)src + 3 * pitch );

	dst = (UINT32 *)( (UINT8 *)dst + pitch );

	moving = _g_moving + _size_x;
	movingminus = moving - _size_x;
	movingplus = moving + _size_x;

	if (_b_show_deinterlace )
		for (y = 1; y < hminus1; ++y )
			{
			x = 0;
			do
				{
				if( !( movingminus[x] | moving[x] | movingplus[x] ) && !s_scenechange )
					dst[x] = 0xff7f7f7f;
				else if( y & 1 )
					{
					if( ( y > 2 ) && ( y < hminus3 ) )
						{
						rpp = (srcminusminus[x] >> 16) & 0xff;
						rp =  (srcminus[x] >> 16) & 0xff;
						rn =  (srcplus[x] >> 16) & 0xff;
						rnn = (srcplusplus[x] >>16) & 0xff;
						gpp = (srcminusminus[x] >> 8) & 0xff;
						gp =  (srcminus[x] >> 8) & 0xff;
						gn =  (srcplus[x] >>8) & 0xff;
						gnn = (srcplusplus[x] >> 8) & 0xff;
						bpp = (srcminusminus[x]) & 0xff;
						bp =  (srcminus[x]) & 0xff;
						bn =  (srcplus[x]) & 0xff;
						bnn = (srcplusplus[x]) & 0xff;
						R = (5 * (rp + rn) - (rpp + rnn)) >> 3;
						if (R > 255) R = 255;
						else if (R < 0) R = 0;
						G = (5 * (gp + gn) - (gpp + gnn)) >> 3;
						if (G > 255) G = 255;
						else if (G < 0) G = 0;
						B = (5 * (bp + bn) - (bpp + bnn)) >> 3;
						if (B > 255) B = 255;
						else if (B < 0) B = 0;
						dst[x] = (0xff000000) | (R << 16) | (G << 8) | B;  
						}
					else
						{
						p1 = srcminus[x];
						p1 &= 0x00fefefe;

						p2 = srcplus[x];
						p2 &= 0x00fefefe;
						dst[x] = (0xff000000) | ((p1>>1) + (p2>>1));
						}
					}
				else
					dst[x] = (0xff000000) | src[x];
				}
			while( ++x < _size_x );

			src = (UINT32 *)( (UINT8 *)src + pitch );
			srcminus = (UINT32 *)( (UINT8 *)src - pitch );
			srcplus = (UINT32 *)( (UINT8 *)src + pitch );
			srcminusminus = (UINT32 *)( (UINT8 *)srcminusminus + pitch );
			srcplusplus = (UINT32 *)( (UINT8 *)srcplusplus + pitch );

			dst = (UINT32 *)((UINT8 *)dst + pitch);

			moving += _size_x;
			movingminus += _size_x;
			movingplus += _size_x;
		}
	else
		for (y = 1; y < hminus1; ++y )
			{
			// Doing line interpolate. Thus, even lines are going through
			// for moving and non-moving mode. Odd line pixels will be subject
			// to the motion test.
			if (y&1)
				{
				x = 0;
				do
					{
					if (!(movingminus[x] | moving[x] | movingplus[x]) && !s_scenechange)
						dst[x] = (0xff000000) | src[x];
					else
						{
						if ((y > 2) && (y < hminus3))
							{
							rpp = (srcminusminus[x] >> 16) & 0xff;
							rp =  (srcminus[x] >> 16) & 0xff;
							rn =  (srcplus[x] >> 16) & 0xff;
							rnn = (srcplusplus[x] >>16) & 0xff;
							gpp = (srcminusminus[x] >> 8) & 0xff;
							gp =  (srcminus[x] >> 8) & 0xff;
							gn =  (srcplus[x] >>8) & 0xff;
							gnn = (srcplusplus[x] >> 8) & 0xff;
							bpp = (srcminusminus[x]) & 0xff;
							bp =  (srcminus[x]) & 0xff;
							bn =  (srcplus[x]) & 0xff;
							bnn = (srcplusplus[x]) & 0xff;
							R = (5 * (rp + rn) - (rpp + rnn)) >> 3;
							if (R > 255) R = 255;
							else if (R < 0) R = 0;
							G = (5 * (gp + gn) - (gpp + gnn)) >> 3;
							if (G > 255) G = 255;
							else if (G < 0) G = 0;
							B = (5 * (bp + bn) - (bpp + bnn)) >> 3;
							if (B > 255) B = 255;
							else if (B < 0) B = 0;
							dst[x] = (0xff000000) | (R << 16) | (G << 8) | B;  
							}
						else
							{
							p1 = srcminus[x];
							p1 &= 0xfffefefe;

							p2 = srcplus[x];
							p2 &= 0xfffefefe;

							dst[x] = (0xff000000) | ((p1>>1) + (p2>>1));
							}
						}
					}
				while(++x < _size_x);
				}
			else
				{
				// Even line; pass it through.
				MEMCPY( dst, src, pitch);
				}
			src = (UINT32 *)((UINT8 *)src + pitch);
			srcminus = (UINT32 *)((UINT8 *)src - pitch);
			srcplus = (UINT32 *)((UINT8 *)src + pitch);
			srcminusminus = (UINT32 *)((UINT8 *)srcminusminus + pitch);
			srcplusplus = (UINT32 *)((UINT8 *)srcplusplus + pitch);

			dst = (UINT32 *)((UINT8 *)dst + pitch);

			moving += _size_x;
			movingminus += _size_x;
			movingplus += _size_x;
		}

	// The last line gets a free ride.
	MEMCPY( dst, src, pitch);
}


void	c_deint_smart::run_frame(UINT8* p_src, UINT8* p_dst )
{
//	UINT32	srcpitch = width * 4;
	UINT32	*src, *dst;
	BOOL	scenechange = FALSE;
	INT32	count;

	src = (UINT32 *)p_src;
	dst = (UINT32 *)p_dst;

	TBUF_ADD( tbuf::CH_VIDEO_PROCESS, 1., nullptr, this );

	/* Not much deinterlacing to do if there aren't at least 2 lines. */
	if( _size_y < 2 ) return;

	count = 0;

	if( _s_smart_motion_map_mode == FRAME_ONLY ) // || s_smart_motion_map_mode == FRAME_AND_FIELD)
		{
		if( _b_smart_colordiff )
			count = smart_32_motion_map_frame_rgb( src );
		else
			count = smart_32_motion_map_frame_luma( src );
		}
	else if( _s_smart_motion_map_mode == FIELD_ONLY )
		{
		/* Field differencing only mode. */
		if( _b_smart_colordiff )
			count = smart_32_motion_map_field_rgb( src );
		else
			count = smart_32_motion_map_field_luma( src );
		}
	else
		{
		// Frame and Field differencing
		if( _b_smart_colordiff )
			count = smart_32_motion_map_framefield_rgb( src );
		else
			count = smart_32_motion_map_framefield_luma( src );
		}

	/* Determine whether a scene change has occurred. */
	if( ( 100L * count ) / ( _size_y * _size_x ) >= _smart_scene_threshold )
		scenechange = TRUE;
//	else scenechange = 0;

	TBUF_ADD( tbuf::CH_VIDEO_PROCESS, 4., nullptr, this );

	/* Perform a denoising of the motion map if enabled. */
	if( !scenechange && _b_smart_highquality )
		{
		smart_32_motion_map_denoise();
		}

	src = (UINT32 *)p_src;
	dst = (UINT32 *)p_dst;

	// Render.
	switch( _smart_blend_mode )
		{
		case COPY:
			smart_32_copy( src, dst, scenechange );
			break;
		case BLEND:
			smart_32_blend( src, dst, scenechange );
			break;
		case CUBIC:
			smart_32_cubic( src , dst, scenechange );
			break;
		case ELA:
	//				deinterlace_smart_ela_32(src , dst, srcpitch, width, height, scenechange, b_show_deinterlace);
			smart_32_blend( src, dst, scenechange );
			break;
		default:
			break;
		}
	TBUF_ADD( tbuf::CH_VIDEO_PROCESS, 1., nullptr, this );

//	return 0;
}

UINT32 c_deint_smart::smart_32_field(UINT8* p_src, UINT8* p_src_next, UINT8* p_dst )
{
	UINT32	srcpitch = _size_x * 4;
	UINT32	*src, *dst;
	INT32	scenechange;
	INT32	count;
	INT32	h2 = _size_y * 2;


	TBUF_ADD( tbuf::CH_VIDEO_PROCESS, 1., nullptr, this );

	// merge field for now
	//todo smart deinterlace with field
	UINT8*	src_full = _merge_src;
	UINT8*	src1 = p_src_next;
	UINT8*	src2 = p_src;

	for( INT32 i = _size_y; i > 0; --i )
		{
		MEMCPY( src_full, src1, srcpitch);
		src_full += srcpitch;
		src1 += srcpitch;
		MEMCPY( src_full, src2, srcpitch);
		src_full += srcpitch;
		src2 += srcpitch;
		}	

	src = (UINT32 *)_merge_src;
	dst = (UINT32 *)p_dst;


	/* Not much deinterlacing to do if there aren't at least 2 lines. */
	if (h2 < 2)
		return 0;

	count = 0;

	if (_s_smart_motion_map_mode == FRAME_ONLY) // || s_smart_motion_map_mode == FRAME_AND_FIELD)
		{
		if (_b_smart_colordiff )
			count = smart_32_motion_map_frame_rgb( src );
		else
			count = smart_32_motion_map_frame_luma( src );
		}
	else if (_s_smart_motion_map_mode == FIELD_ONLY)
		{
		/* Field differencing only mode. */
		if (_b_smart_colordiff )
			count = smart_32_motion_map_field_rgb( src );
		else
			count = smart_32_motion_map_field_luma( src );
		}
	else
		{
		// Frame and Field differencing
		if (_b_smart_colordiff )
			count = smart_32_motion_map_framefield_rgb( src );
		else
			count = smart_32_motion_map_framefield_luma( src );
		}
	
	/* Determine whether a scene change has occurred. */
	if ((100L * count) / ( h2 * _size_x ) >= _smart_scene_threshold )
		scenechange = 1;
	else scenechange = 0;

	TBUF_ADD( tbuf::CH_VIDEO_PROCESS, 4., nullptr, this );

	/* Perform a denoising of the motion map if enabled. */
	if( !scenechange && _b_smart_highquality )
		{
		smart_32_motion_map_denoise();
		}

	src = (UINT32 *)_merge_src;
	dst = (UINT32 *)p_dst;

	// Render.
	switch( _smart_blend_mode )
		{
		case COPY:
			smart_32_copy(src, dst, scenechange );
			break;
		case BLEND:
			smart_32_blend(src, dst, scenechange );
			break;
		case CUBIC:
			smart_32_cubic(src , dst, scenechange );
			break;
		case ELA:
	//				deinterlace_smart_ela_32(src , dst, srcpitch, width, height, scenechange, b_show_deinterlace);
			smart_32_blend(src, dst, scenechange );
			break;
		default:
			break;
		}

	return 0;
}


void	c_deint_smart::run_full(  c_image_flux* flux_in, UINT8* dst, BOOL second_pass )
{

}
