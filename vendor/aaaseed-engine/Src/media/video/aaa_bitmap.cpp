#include "aaa_bitmap.h"
#include "math/rand.h"
#include "aaa_util.h"
#include "err.h"
#include "aaa_mem.h"


static	c_rand_lin rnd;

enum SELECTOR_TYPE : INT32
{
	SEL_ZERO = 0,
	SEL_ONE,
	SEL_RED,
	SEL_RED_INV,
	SEL_GREEN,
	SEL_GREEN_INV,
	SEL_BLUE,
	SEL_BLUE_INV,
	SEL_ALPHA,
	SEL_ALPHA_INV,
	SEL_MAX_NB,
};

class c_our_bitmap
{
public:
	static	REAL	col_min[3];
	static	REAL	col_max[3];
	static	REAL	col[3];

	static	REAL	col_dia[3];
	static	REAL	dia_factor[3];
	static	REAL	dia_int;
	static	REAL	dia_int_factor;

	static	INT32	i_col_min[3];
	static	INT32	i_col_max[3];

};

REAL	c_our_bitmap::col_min[3];
REAL	c_our_bitmap::col_max[3];
REAL	c_our_bitmap::col[3];

REAL	c_our_bitmap::col_dia[3];
REAL	c_our_bitmap::dia_factor[3];
REAL	c_our_bitmap::dia_int;
REAL	c_our_bitmap::dia_int_factor;

INT32	c_our_bitmap::i_col_min[3];
INT32	c_our_bitmap::i_col_max[3];


//output is bgr
FINLINE void	process_direct_16_32( UINT8* src, INT32 w, INT32 h, UINT8* dst )
{
	for( INT32 i = h * w; i > 0; --i )
	{
		INT16	tmp = *(INT16 *)src;
		*dst++ = (tmp & 0x03e0) >> 2;
		*dst++ = (tmp & 0x7c00) >> 7;
		*dst++ = (tmp & 0x001f) << 3;
		*dst++ = 0xff;
		src += 2;
	}
}

FINLINE void	process_direct_16_24( UINT8* src, INT32 w, INT32 h, UINT8* dst )
{
	for( INT32 i = h * w; i > 0; --i )
	{
		INT16	tmp = *(INT16 *)src;
		*dst++ = (tmp & 0x7c00) >> 7;
		*dst++ = (tmp & 0x03e0) >> 2;
		*dst++ = (tmp & 0x001f) << 3;
		src += 2;
	}
}

//output is bgr
FINLINE void	process_direct_24_32( UINT8* src, INT32 w, INT32 h, UINT8* dst8 )
{
	UINT32*	dst = (UINT32*)dst8;
	for( INT32 i = h * w; i > 0; --i )
	{	// val is abgr
		UINT32	val = 0xff000000;
		val |= (*src++) << 0;
		val |= (*src++) << 8;
		val |= (*src++) << 16;
		*dst++ = val;
	}
}

//just to see not tested
FINLINE void	process_direct_24_32_no_faster( UINT8* src8, INT32 w, INT32 h, UINT8* dst8 )
{
	UINT32*	dst = (UINT32*)dst8;
	UINT32* src = (UINT32*)src8;

	for( INT32 i = ( h * w ) >> 2; i > 0; --i )
	{	// val is abgr
		UINT32	data = *src++;
		UINT32	val = 0xff000000;
		val |= ((data>>0)&0xff)<<16;
		val |= ((data>>8)&0xff)<<8;
		val |= ((data>>16)&0xff)<<0;
		*dst++ = val;
		val = 0xff000000;
		val |= ((data>>24)&0xff)<<16;
		data = *src++;
		val |= ((data>>0)&0xff)<<8;
		val |= ((data>>8)&0xff)<<0;
		*dst++ = val;
		val = 0xff000000;
		val |= ((data>>16)&0xff)<<16;
		val |= ((data>>24)&0xff)<<8;
		data = *src++;
		val |= ((data>>0)&0xff)<<0;
		*dst++ = val;
		val = 0xff000000;
		val |= ((data>>8)&0xff)<<16;
		val |= ((data>>16)&0xff)<<8;
		val |= ((data>>24)&0xff)<<0;
		*dst++ = val;
	}
}
FINLINE void	process_direct_24_24( UINT8* src, INT32 w, INT32 h, UINT8* dst )
{
	MEMCPY( dst, src, h*w*3, __FUNCTION__ );
/*
	for( INT32 i=h*w; i>0; --i )
	{
		*dst++ = *(src+2);
		*dst++ = *(src+1);
		*dst++ = *(src+0);
		src += 3;
	}
*/
}

/*
		if( pavi->get_bit_depth() == 16 )
			if( channel_nb == 3 )
//				process_direct_16_24( s_op, pavi->get_psrc(), w, h, pavi->get_pdata() );
				process( in_16(), out_24(), s_op, src, w, h, dst, s_r, s_g, s_b, s_a );
			else
//				process_direct_16_32( s_op, pavi->get_psrc(), w, h, pavi->get_pdata() );
				process( in_16(), out_32(), s_op, src, w, h, dst, s_r, s_g, s_b, s_a );
		else
			if( channel_nb == 3 )
//				process_direct_24_24( s_op, pavi->get_psrc(), w, h, pavi->get_pdata() );
				process( in_24(), out_24(), s_op, src, w, h, dst, s_r, s_g, s_b, s_a );
			else
//				process_direct_24_32( s_op, pavi->get_psrc(), w, h, pavi->get_pdata() );
				process( in_24(), out_32(), s_op, src, w, h, dst, s_r, s_g, s_b, s_a );
*/







struct in_16
{
	static FINLINE void	apply( UINT8* &src, UINT8 &r, UINT8 &g, UINT8 &b )
	{
		UINT16	tmp = *(UINT16 *)src;
		r = (tmp & 0x7c00) >> 7;
		g = (tmp & 0x03e0) >> 2;
		b = (tmp & 0x001f) << 3;
		src += 2;
	}
	static FINLINE INT32	get_byte_per_pixel()	{	return 2;	}
};

struct in_24
{
	static FINLINE void	apply( UINT8* &src, UINT8 &r, UINT8 &g, UINT8 &b )
	{
		b = *src++;
		g = *src++;
		r = *src++;
	}
	static FINLINE INT32	get_byte_per_pixel()	{	return 3;	}
};

/*	
template < INT32 CONST which, bool CONST b_inv >
struct out_8
{
	static FINLINE void	apply( UINT8* &dst, UINT8 r, UINT8 g, UINT8 b, UINT8 a = 0xff)
		{
		if( b_inv )
			{
			if ( which == 0 )
				*dst++  = 0xff - r;
			else if ( which == 1 )
				*dst++  = 0xff - g;
			else if ( which == 2 )
				*dst++  = 0xff - b;
			else if ( which == 3 )
				*dst++  = 0xff - a;
			else
				*dst++  = 0xff;
			}
		else
			{
			if ( which == 0 )
				*dst++  = r;
			else if ( which == 1 )
				*dst++  = g;
			else if ( which == 2 )
				*dst++  = b;  	
			else if ( which == 3 )
				*dst++  = a; 	
			else
				*dst++  = 0;
			}
		}
};
*/
struct use_zero		{	static FINLINE UINT8 apply( UINT8 r, UINT8 g, UINT8 b, UINT8 a )	{	return 0; }			};
struct use_one		{	static FINLINE UINT8 apply( UINT8 r, UINT8 g, UINT8 b, UINT8 a)		{	return 0xff; }		};
struct use_red		{	static FINLINE UINT8 apply( UINT8 r, UINT8 g, UINT8 b, UINT8 a)		{	return r; }			};
struct use_red_inv	{	static FINLINE UINT8 apply( UINT8 r, UINT8 g, UINT8 b, UINT8 a)		{	return 0xff - r; }	};
struct use_green	{	static FINLINE UINT8 apply( UINT8 r, UINT8 g, UINT8 b, UINT8 a)		{	return g; }			};
struct use_green_inv{	static FINLINE UINT8 apply( UINT8 r, UINT8 g, UINT8 b, UINT8 a)		{	return 0xff - g; }	};
struct use_blue		{	static FINLINE UINT8 apply( UINT8 r, UINT8 g, UINT8 b, UINT8 a)		{	return b; }			};
struct use_blue_inv	{	static FINLINE UINT8 apply( UINT8 r, UINT8 g, UINT8 b, UINT8 a)		{	return 0xff - b; }	};
struct use_alpha	{	static FINLINE UINT8 apply( UINT8 r, UINT8 g, UINT8 b, UINT8 a)		{	return a; }			};
struct use_alpha_inv{	static FINLINE UINT8 apply( UINT8 r, UINT8 g, UINT8 b, UINT8 a)		{	return 0xff - a; }	};

template < class sel>
struct out_8
{
	static FINLINE void	apply( UINT8* &dst, UINT8 r, UINT8 g, UINT8 b, UINT8 a = 0xff)
	{
		*dst++  = sel::apply( r, g, b, a);
	}
};

template < class sel_r, class sel_g, class sel_b>
struct out_24
{
	static FINLINE void	apply( UINT8* &dst, UINT8 r, UINT8 g, UINT8 b, UINT8 a = 0xff)
	{
		*dst++  = sel_r::apply( r, g, b, a);
		*dst++  = sel_g::apply( r, g, b, a);
		*dst++  = sel_b::apply( r, g, b, a);
//		out_8< sel_r >::apply( dst, r, g, b, a);
//		out_8< sel_g >::apply( dst, r, g, b, a);
//		out_8< sel_b >::apply( dst, r, g, b, a);
	}
	static FINLINE INT32	get_byte_per_pixel()	{	return 3;	}
};

/*

struct out_24
{
	static FINLINE void	apply( UINT8* &dst, UINT8 r, UINT8 g, UINT8 b, UINT8 a = 0xff)
		{
		*dst++ = r;
		*dst++ = g;
		*dst++ = b;
		}
	static FINLINE INT32	get_byte_per_pixel()	{	return 3;	}
};

*/
template < class sel_r, class sel_g, class sel_b, class sel_a>
struct out_32
{
	static FINLINE void	apply( UINT8* &dst, UINT8 r, UINT8 g, UINT8 b, UINT8 a = 0xff)
		{
		out_24< sel_r, sel_g, sel_b>::apply( dst, r, g, b);
		*dst++  = sel_a::apply( r, g, b, a);
//		out_8< sel_a>::apply( dst, r, g, b, a);
		}
	static FINLINE INT32	get_byte_per_pixel()	{	return 4;	}
};

/*
template <class in, class out, INT32 CONST sel_r, INT32 CONST sel_g, INT32 CONST sel_b, INT32 CONST sel_a>
void	process_low( in, out, INT32 s_op, UINT8* src, INT32 w, INT32 h, UINT8* dst )
*/
template <class in, class out>
void	process_low( in, out, INT32 s_op, UINT8* src, INT32 w, INT32 h, UINT8* dst )
{
	UINT8 r;
	UINT8 g;
	UINT8 b;
	//INT32 i;

	switch( s_op)
		{
/*		case OP_COPY:
			for( i=h*w; i>0; --i )
				{
				in::apply( src, r, g, b);
				out::apply( dst, r, g, b);
				}
			break;
*/
		case OP_COPY:
//		case OP_LUMINANCE:
			for( INT32 i = h * w; i > 0; --i )
			{
				in::apply( src, r, g, b );
				out::apply( dst, r, g, b, 255 );
			}
			break;
		case OP_BLUE_SCREEN_RAW:
			for( INT32 i = h * w; i > 0; --i )
			{
				UINT32	tot;
				in::apply( src, r, g, b );
				if(	c_our_bitmap::i_col_min[0] <= r && r <= c_our_bitmap::i_col_max[0]
					&& c_our_bitmap::i_col_min[1] <= g && g <= c_our_bitmap::i_col_max[1]
					&& c_our_bitmap::i_col_min[2] <= b && b <= c_our_bitmap::i_col_max[2] )
				{
					tot = 0;
				}
				else
					tot = 255;
				out::apply( dst, r, g, b, tot );
			}
			break;
		case OP_BLUE_SCREEN:
		case OP_BLUE_SCREEN_ALPHA:
			for( INT32 i = h * w; i > 0; --i )
			{
				UINT32	tot;
				in::apply( src, r, g, b );
				if(	c_our_bitmap::i_col_min[0] <= r && r <= c_our_bitmap::i_col_max[0]
					&& c_our_bitmap::i_col_min[1] <= g && g <= c_our_bitmap::i_col_max[1]
					&& c_our_bitmap::i_col_min[2] <= b && b <= c_our_bitmap::i_col_max[2] )
				{
					REAL	tmp;
					REAL	size;
/*
					REAL	v[3];
					v[0] = REAL(r)*(1./255.);
					v[1] = REAL(g)*(1./255.);
					v[2] = REAL(b)*(1./255.);
*/
					tmp = (REAL(r)-(c_our_bitmap::col[0]))*c_our_bitmap::dia_factor[0];
					if( tmp < 0 )
						size = -tmp;
					else
						size = tmp;
					if( size >= 1.)
						tot = 255;
					else
					{
						tmp = (REAL(g)-(c_our_bitmap::col[1]))*c_our_bitmap::dia_factor[1];
						if( tmp < 0 )
							size -= tmp;
						else
							size += tmp;
						if( size >= 1.)
							tot = 255;
						else
						{
							tmp = (REAL(b)-(c_our_bitmap::col[2]))*c_our_bitmap::dia_factor[2];
							if( tmp < 0 )
								size -= tmp;
							else
								size += tmp;
							if( size >= 1.)
								tot = 255;
							else if( tmp < c_our_bitmap::dia_int )
								tot = 0;
							else
								tot = INT32( (tmp-c_our_bitmap::dia_int)*c_our_bitmap::dia_int_factor );
						}
					}
				}
				else
					tot = 255;
				if( s_op == OP_BLUE_SCREEN_ALPHA)
					out::apply( dst, tot, tot, tot, 255 );
				else
					out::apply( dst, r, g, b, tot );
			}
			break;
/*		case OP_ALPHA:
			{
			UINT32	tot;
			for( i=h*w; i>0; --i )
				{
				in::apply( src, r, g, b );
				tot = r + g + b;
				if( tot < 48)
					tot = 0;
				else if( tot > (64*3) )
					tot = 0xff ;
				else
					tot = (tot-48)*0xff/(48*3) ;
				out::apply( dst, r, g, b, tot);
				}
			}
			break;
*/
		case OP_CANAL:
			{
				INT32	rnd_val;
				INT32	tmp;
				UINT8*	psrc;
				UINT8*	pdst;
				INT32	j;

				for( INT32 i = h; i > 0; --i )
				{
					//todoqq correct the random so this cooking and third rnd call disapear
					rnd_val = rnd.get_uint32();
					tmp = IMOD( rnd_val>>8, h );
					psrc = src + tmp * w * in::get_byte_per_pixel();
					tmp = IMOD( tmp + ((rnd_val & 0x1f0000)>>16) - 16, h );
					pdst = dst + tmp * w * out::get_byte_per_pixel();
					for( j=w; j>0; --j )
					{
						in::apply( psrc, r, g, b );
						//out::apply( pdst, r, g, b );
						out::apply( pdst, 255, 255, 255, (j*255)/w );
					}
				}
			}
			break;
/*		case OP_SPECIAL:
			{
				UINT32	tot;
				for( i=h*w; i>0; --i )
				{
					in::apply( src, r, g, b );
					if( g > 120 || r > 200 )
						out::apply( dst, r, g, b );
					else
					{
						tot = (r+g+b)/3;
						out::apply( dst, tot, tot, tot );
					}
				}
			}
			break;
		case OP_NOISE:
			for( i=h*w; i>0; --i )
			{
				r = (rnd.get_uint32()>>24) & 0xff;
				out::apply( dst, r, r, r );
			}
			break;
*/		case OP_NOISE_COLOR:
			for( INT32 i = h * w; i > 0; --i )
			{
				UINT8 a;
				r = (rnd.get_uint32()>>24) & 0xff;
				g = (rnd.get_uint32()>>24) & 0xff;
				b = (rnd.get_uint32()>>24) & 0xff;
				a = (rnd.get_uint32()>>24) & 0xff;
				out::apply( dst, r, g, b, a );
			}
			break;
		}
}

/*
template <INT32 CONST sel_r, INT32 CONST sel_g, INT32 CONST sel_b, INT32 CONST sel_a>
void	c_tex_video::process( INT32 s_op, UINT8* src, INT32 w, INT32 h, UINT8* dst)
{
	switch ( sel_a )
		{
		case SEL_ZERO:		process<sel_r, sel_g, sel_b, SEL_ZERO>		( s_op, src, w, h, dst);	break;
		case SEL_ONE:		process<sel_r, sel_g, sel_b, SEL_ONE>		( s_op, src, w, h, dst);	break;
		case SEL_RED:		process<sel_r, sel_g, sel_b, SEL_RED>		( s_op, src, w, h, dst);	break;
		case SEL_RED_INV:	process<sel_r, sel_g, sel_b, SEL_RED_INV>	( s_op, src, w, h, dst);	break;
		case SEL_GREEN:		process<sel_r, sel_g, sel_b, SEL_GREEN>		( s_op, src, w, h, dst);	break;
		case SEL_GREEN_INV:	process<sel_r, sel_g, sel_b, SEL_GREEN_INV>	( s_op, src, w, h, dst);	break;
		case SEL_BLUE:		process<sel_r, sel_g, sel_b, SEL_BLUE>		( s_op, src, w, h, dst);	break;
		case SEL_BLUE_INV:	process<sel_r, sel_g, sel_b, SEL_BLUE_INV>	( s_op, src, w, h, dst);	break;
		case SEL_ALPHA:		process<sel_r, sel_g, sel_b, SEL_ALPHA>		( s_op, src, w, h, dst);	break;
		case SEL_ALPHA_INV:	process<sel_r, sel_g, sel_b, SEL_ALPHA_INV>	( s_op, src, w, h, dst);	break;
		}
}
*/

INT32	g_out;
INT32	g_in;

//todoqq move to format_in and format_out
template <class sel_r, class sel_g, class sel_b, class sel_a>
void process_r( sel_r, sel_g, sel_b, sel_a, INT32 s_op, UINT8* src, INT32 w, INT32 h, UINT8* dst )
{
	switch( g_in )
	{
	case 16:
		switch( g_out )
		{
		case 24:	process_low( in_16(), out_24<sel_r,sel_g,sel_b>(), s_op, src, w, h, dst );			break;
		case 32:	process_low( in_16(), out_32<sel_r,sel_g,sel_b,sel_a>(), s_op, src, w, h, dst );	break;
		}
		break;
	case 24:
		switch( g_out )
		{
		case 24:	process_low( in_24(), out_24<sel_r,sel_g,sel_b>(), s_op, src, w, h, dst );			break;
		case 32:	process_low( in_24(), out_32<sel_r,sel_g,sel_b,sel_a>(), s_op, src, w, h, dst );	break;
		}
		break;
	}
}

template <class sel_g, class sel_b, class sel_a>
void process_g( sel_g, sel_b, sel_a, INT32 s_op, UINT8* src, INT32 w, INT32 h, UINT8* dst, INT32 CONST sel_r )
{
	switch ( sel_r )
	{
		case SEL_ZERO:		process_r<use_zero,sel_g,sel_b,sel_a>		( use_zero(),		sel_g(), sel_b(), sel_a(), s_op, src, w, h, dst );	break;
		case SEL_ONE:		process_r<use_one,sel_g,sel_b,sel_a>		( use_one(),		sel_g(), sel_b(), sel_a(), s_op, src, w, h, dst );	break;
		case SEL_RED:		process_r<use_red,sel_g,sel_b,sel_a>		( use_red(),		sel_g(), sel_b(), sel_a(), s_op, src, w, h, dst );	break;
		case SEL_RED_INV:	process_r<use_red_inv,sel_g,sel_b,sel_a>	( use_red_inv(),	sel_g(), sel_b(), sel_a(), s_op, src, w, h, dst );	break;
		case SEL_GREEN:		process_r<use_green,sel_g,sel_b,sel_a>		( use_green(),		sel_g(), sel_b(), sel_a(), s_op, src, w, h, dst );	break;
		case SEL_GREEN_INV:	process_r<use_green_inv,sel_g,sel_b,sel_a>	( use_green_inv(),	sel_g(), sel_b(), sel_a(), s_op, src, w, h, dst );	break;
		case SEL_BLUE:		process_r<use_blue,sel_g,sel_b,sel_a>		( use_blue(),		sel_g(), sel_b(), sel_a(), s_op, src, w, h, dst );	break;
		case SEL_BLUE_INV:	process_r<use_blue_inv,sel_g,sel_b,sel_a>	( use_blue_inv(),	sel_g(), sel_b(), sel_a(), s_op, src, w, h, dst );	break;
//		case SEL_ALPHA:		process_r<use_alpha,sel_g,sel_b,sel_a>		( use_alpha(),		sel_g(), sel_b(), sel_a(), s_op, src, w, h, dst );	break;
//		case SEL_ALPHA_INV:	process_r<use_alpha_inv,sel_g,sel_b,sel_a>	( use_alpha_inv(),	sel_g(), sel_b(), sel_a(), s_op, src, w, h, dst );	break;
	}
}

template < class sel_b, class sel_a>
void process_b( sel_b, sel_a, INT32 s_op, UINT8* src, INT32 w, INT32 h, UINT8* dst, INT32 CONST sel_r, INT32 CONST sel_g )
{
	switch ( sel_g )
	{
		case SEL_ZERO:		process_g<use_zero,sel_b,sel_a>		( use_zero(),		sel_b(), sel_a(), s_op, src, w, h, dst, sel_r );	break;
		case SEL_ONE:		process_g<use_one,sel_b,sel_a>		( use_one(),		sel_b(), sel_a(), s_op, src, w, h, dst, sel_r );	break;
		case SEL_RED:		process_g<use_red,sel_b,sel_a>		( use_red(),		sel_b(), sel_a(), s_op, src, w, h, dst, sel_r );	break;
		case SEL_RED_INV:	process_g<use_red_inv,sel_b,sel_a>	( use_red_inv(),	sel_b(), sel_a(), s_op, src, w, h, dst, sel_r );	break;
		case SEL_GREEN:		process_g<use_green,sel_b,sel_a>	( use_green(),		sel_b(), sel_a(), s_op, src, w, h, dst, sel_r );	break;
		case SEL_GREEN_INV:	process_g<use_green_inv,sel_b,sel_a>( use_green_inv(),	sel_b(), sel_a(), s_op, src, w, h, dst, sel_r );	break;
		case SEL_BLUE:		process_g<use_blue,sel_b,sel_a>		( use_blue(),		sel_b(), sel_a(), s_op, src, w, h, dst, sel_r );	break;
		case SEL_BLUE_INV:	process_g<use_blue_inv,sel_b,sel_a>	( use_blue_inv(),	sel_b(), sel_a(), s_op, src, w, h, dst, sel_r );	break;
//		case SEL_ALPHA:		process_g<use_alpha,sel_b,sel_a>	( use_alpha(),		sel_b(), sel_a(), s_op, src, w, h, dst, sel_r );	break;
//		case SEL_ALPHA_INV:	process_g<use_alpha_inv,sel_b,sel_a>( use_alpha_inv(),	sel_b(), sel_a(), s_op, src, w, h, dst, sel_r );	break;
	}
}

template <class sel_a>
void process_a( sel_a, INT32 s_op, UINT8* src, INT32 w, INT32 h, UINT8* dst, INT32 CONST sel_r, INT32 CONST sel_g, INT32 CONST sel_b )
{
	switch ( sel_b )
	{
		case SEL_ZERO:		process_b<use_zero,sel_a>		( use_zero(),		sel_a(), s_op, src, w, h, dst, sel_r, sel_g );	break;
		case SEL_ONE:		process_b<use_one,sel_a>		( use_one(),		sel_a(), s_op, src, w, h, dst, sel_r, sel_g );	break;
		case SEL_RED:		process_b<use_red,sel_a>		( use_red(),		sel_a(), s_op, src, w, h, dst, sel_r, sel_g );	break;
		case SEL_RED_INV:	process_b<use_red_inv,sel_a>	( use_red_inv(),	sel_a(), s_op, src, w, h, dst, sel_r, sel_g );	break;
		case SEL_GREEN:		process_b<use_green,sel_a>		( use_green(),		sel_a(), s_op, src, w, h, dst, sel_r, sel_g );	break;
		case SEL_GREEN_INV:	process_b<use_green_inv,sel_a>	( use_green_inv(),	sel_a(), s_op, src, w, h, dst, sel_r, sel_g );	break;
		case SEL_BLUE:		process_b<use_blue,sel_a>		( use_blue(),		sel_a(), s_op, src, w, h, dst, sel_r, sel_g );	break;
		case SEL_BLUE_INV:	process_b<use_blue_inv,sel_a>	( use_blue_inv(),	sel_a(), s_op, src, w, h, dst, sel_r, sel_g );	break;
//		case SEL_ALPHA:		process_b<use_alpha,sel_a>		( use_alpha(),		sel_a(), s_op, src, w, h, dst, sel_r, sel_g );	break;
//		case SEL_ALPHA_INV:	process_b<use_alpha_inv,sel_a>	( use_alpha_inv(),	sel_a(), s_op, src, w, h, dst, sel_r, sel_g );	break;
	}
}

void	process( INT32 bit_in, INT32 bit_out, INT32 s_op, UINT8* src, INT32 w, INT32 h, UINT8* dst, REAL* col_in, REAL* col_dia_in, INT32 CONST sel_r, INT32 CONST sel_g, INT32 CONST sel_b, INT32 CONST sel_a )
{
//INT32	i;

	if( s_op >= OP_BLUE_SCREEN_RAW )
	{
		for( INT32 i = 0; i < 3; ++i )
		{
			c_our_bitmap::col[i] = col_in[i];
			c_our_bitmap::col_dia[i] = col_dia_in[i];

			c_our_bitmap::col_min[i] = (REAL) (c_our_bitmap::col[i] - c_our_bitmap::col_dia[i] * .5);
			c_our_bitmap::col_max[i] = (REAL) (c_our_bitmap::col[i] + c_our_bitmap::col_dia[i] * .5);

			c_our_bitmap::i_col_min[i] = (INT32) ( CLAMP( c_our_bitmap::col_min[i], REAL(0), REAL(1) ) * 255);
			c_our_bitmap::i_col_max[i] = (INT32) ( CLAMP( c_our_bitmap::col_max[i], REAL(0), REAL(1) ) * 255);

			c_our_bitmap::col[i] *= 255.;

			if( c_our_bitmap::col_dia[0] != 0. )
				c_our_bitmap::dia_factor[i] = REAL( 2./(c_our_bitmap::col_dia[i]*255.) );
			else
				c_our_bitmap::dia_factor[i] = REAL( 1000000. );
		}

		c_our_bitmap::dia_int = col_dia_in[3];
		if( c_our_bitmap::dia_int != 1.)
			c_our_bitmap::dia_int_factor = REAL( 255. / ( 1. - c_our_bitmap::dia_int ) );
	}


	g_in = bit_in;
	g_out = bit_out;
//	if( s_op != OP_COPY || sel_r != SEL_RED || sel_g != SEL_GREEN || sel_b != SEL_BLUE )
//		process_a<use_alpha>		( use_alpha(),		s_op, src, w, h, dst, sel_r, sel_g, sel_b);
/*
	switch ( sel_a )
	{
		case SEL_ZERO:		process_a<use_zero>		( use_zero(),		s_op, src, w, h, dst, sel_r, sel_g, sel_b );	break;
		case SEL_ONE:		process_a<use_one>		( use_one(),		s_op, src, w, h, dst, sel_r, sel_g, sel_b );	break;
		case SEL_RED:		process_a<use_red>		( use_red(),		s_op, src, w, h, dst, sel_r, sel_g, sel_b );	break;
		case SEL_RED_INV:	process_a<use_red_inv>	( use_red_inv(),	s_op, src, w, h, dst, sel_r, sel_g, sel_b );	break;
		case SEL_GREEN:		process_a<use_green>	( use_green(),		s_op, src, w, h, dst, sel_r, sel_g, sel_b );	break;
		case SEL_GREEN_INV:	process_a<use_green_inv>( use_green_inv(),	s_op, src, w, h, dst, sel_r, sel_g, sel_b );	break;
		case SEL_BLUE:		process_a<use_blue>		( use_blue(),		s_op, src, w, h, dst, sel_r, sel_g, sel_b );	break;
		case SEL_BLUE_INV:	process_a<use_blue_inv>	( use_blue_inv(),	s_op, src, w, h, dst, sel_r, sel_g, sel_b );	break;
		case SEL_ALPHA:		process_a<use_alpha>	( use_alpha(),		s_op, src, w, h, dst, sel_r, sel_g, sel_b );	break;
		case SEL_ALPHA_INV:	process_a<use_alpha_inv>( use_alpha_inv(),	s_op, src, w, h, dst, sel_r, sel_g, sel_b );	break;
	}
*/
//	else
	{
		switch( g_in)
		{
		case 16:
			switch( g_out )
			{
			case 24:	process_direct_16_24( src, w, h, dst ); break;
			case 32:	process_direct_16_32( src, w, h, dst ); break;
			}
			break;
		case 24:
			switch( g_out )
			{
			case 24:	process_direct_24_24( src, w, h, dst ); break;
			case 32:	process_direct_24_32( src, w, h, dst ); break;
			}
			break;
		}
	}
}


/*
		if( pavi->get_bit_depth() == 16 )
			if( channel_nb == 3 )
//				process_direct_16_24( s_op, pavi->get_psrc(), w, h, pavi->get_pdata() );
				process( in_16(), out_24(), s_op, src, w, h, dst, s_r, s_g, s_b, s_a );
			else
//				process_direct_16_32( s_op, pavi->get_psrc(), w, h, pavi->get_pdata() );
				process( in_16(), out_32(), s_op, src, w, h, dst, s_r, s_g, s_b, s_a );
		else
			if( channel_nb == 3 )
//				process_direct_24_24( s_op, pavi->get_psrc(), w, h, pavi->get_pdata() );
				process( in_24(), out_24(), s_op, src, w, h, dst, s_r, s_g, s_b, s_a );
			else
//				process_direct_24_32( s_op, pavi->get_psrc(), w, h, pavi->get_pdata() );
				process( in_24(), out_32(), s_op, src, w, h, dst, s_r, s_g, s_b, s_a );
*/
