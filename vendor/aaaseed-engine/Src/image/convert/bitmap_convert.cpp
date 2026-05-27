#include "image/convert/bitmap_convert.h"
#include "color_space.h"


using namespace aaa;

/*
void	swap_rgb( UINT8* data, INT32 nb, INT32 channel_nb )
{
	if( channel_nb == 3 )
	{
		UINT8 tmp;
		for( ; nb>0; --nb )
		{
			SWAP( *data, *(data+2);
			data += 3;
		}
	}
}
void	swap_rgb( UINT8* dst, UINT8* src, INT32 nb, INT32 channel_nb )
{
	if( channel_nb == 3 )
	{
		for( ; nb>0; --nb )
		{
			*dst++ = *(src+2);
			*dst++ = *(src+1);
			*dst++ = *(src);
			src += 3;
		}
	}
}
*/

void bitcon::init()
{
}


//#include "convert.h"
//extern "C" {
//	bool FPU_enabled, MMX_enabled, ISSE_enabled,  ;
//};

//usedRESTRICT
void	bitcon::bgr8_to_rgba8_fast( UINT8 CONST * RESTRICT src8, UINT8* RESTRICT dst8, INT32 nb, UINT8 CONST alpha )
{
/*
	if( texture_flux_master->get_convert_to_rgb_mmx() )
	{
		MMX_enabled = true;
		ISSE_enabled = texture_flux_master->get_convert_to_rgb_sse();
#if AAA_WIN64()
		rgb24torgb32( src8, dst8, nb );
#else
		//todo red and blue should be flipped
		DIBconvert_24_to_32(dst8, 0, src8, 0, nb, 1);
#endif	//#if AAA_WIN64()
		return;
	}
*/
	UINT32*	RESTRICT dst = (UINT32*)dst8;
	INT32			t;
	UINT32 CONST	a = alpha << 24;
	--src8;
	--dst;
#if 1
	for( ;nb>3; nb-=4 )
	{
		t = a | (*++src8<<16);
		t |= (*++src8)<<8;
		*++dst = t | *++src8;
		t = a | (*++src8<<16);
		t |= (*++src8)<<8;
		*++dst = t | *++src8;
		t = a | (*++src8<<16);
		t |= (*++src8)<<8;
		*++dst = t | *++src8;
		t = a | (*++src8<<16);
		t |= (*++src8)<<8;
		*++dst = t | *++src8;
	}
	for( ;nb>0; --nb )
	{
		t = a | (*++src8<<16);
		t |= (*++src8)<<8;
		*++dst = t | *++src8;
	}
#else
	nb = shift_len_check_align( __FUNCTION__, nb, 2 );
	for( ;nb>0; --nb )
	{
		t = 0xff000000 | (*++src8<<16);
		t |= (*++src8)<<8;
		*++dst = t | *++src8;
		t = 0xff000000 | (*++src8<<16);
		t |= (*++src8)<<8;
		*++dst = t | *++src8;
		t = 0xff000000 | (*++src8<<16);
		t |= (*++src8)<<8;
		*++dst = t | *++src8;
		t = 0xff000000 | (*++src8<<16);
		t |= (*++src8)<<8;
		*++dst = t | *++src8;
	}
#endif
}

//used
void	bitcon::rgb8_to_rgba8_fast( UINT8 CONST * RESTRICT src8, UINT8* RESTRICT dst8, INT32 nb, UINT8 CONST alpha )
{
	UINT32 CONST	a = alpha << 24;
/*
#if !AAA_NEW_DESIGN()
#if !AAA_WIN64()
//	if( texture_flux_master->is_convert_to_rgb_mmx() )
	if( texture_flux_master->is_convert_to_rgb_sse() )
	{
		MMX_enabled = true;
		ISSE_enabled = texture_flux_master->is_convert_to_rgb_sse();
		if( ISSE_enabled )
		{	//la version non ISSE est bugg�
			DIBconvert_24_to_32( dst8, 0, (UINT8*)src8, 0, nb, 1, a );
			return;
		}
	}
#endif //#if !AAA_WIN64()
#endif //AAA_NEW_DESIGN
*/
//todo not endian safe
#if 1
	UINT32*			RESTRICT dst = (UINT32*)dst8;
	UINT32 CONST *	RESTRICT src = (UINT32 CONST *)src8;
	UINT32			s;
	UINT32			t;

	--src;
	--dst;
	for( ;nb>3; nb-=4 )
	{
		s = *++src;
		*++dst = a | ( s & 0xffffff );
		t = a | ( (s>>24) & 0xff);
		s = *++src;
		*++dst = t | ( (s & 0xffff) << 8 );
		t = a | (s>>16);
		s = *++src;
		*++dst = t | ( (s & 0xff) << 16 );
		*++dst = a | ( s >> 8 );
	}
	if( nb == 0 )	return;
		s = *++src;
		*++dst = a | ( s & 0xffffff );
	if( --nb == 0 )	return;
		t = a | ( (s>>24) & 0xff);
		s = *++src;
		*++dst = t | ( (s & 0xffff) << 8 );
	if( --nb == 0 )	return;
		t = a | (s>>16);
		s = *++src;
		*++dst = t | ( (s & 0xff) << 16 );
	if( --nb == 0 )	return;
		*++dst = a | ( s >> 8 );
#else
	UINT32*	RESTRICT dst = (UINT32*)dst8;
	INT32	t;
	--src8;
	--dst;
	for( ;nb>3; nb-=4 )
	{
		t = 0xff000000 | *++src8;
		t |= (*++src8)<<8;
		*++dst = t | (*++src8<<16);
		t = 0xff000000 | *++src8;
		t |= (*++src8)<<8;
		*++dst = t | (*++src8<<16);
		t = 0xff000000 | *++src8;
		t |= (*++src8)<<8;
		*++dst = t | (*++src8<<16);
		t = 0xff000000 | *++src8;
		t |= (*++src8)<<8;
		*++dst = t | (*++src8<<16);
	}
	for( ;nb>0; --nb )
	{
		t = 0xff000000 | *++src8;
		t |= (*++src8)<<8;
		*++dst = t | (*++src8<<16);
	}
#endif
}

//used
//todo not endian safe
void	bitcon::bgra8_to_rgba8_fast( UINT8 CONST * RESTRICT src8, UINT8* RESTRICT dst8, INT32 nb )
{
	UINT32*			RESTRICT dst = (UINT32*)dst8;
	--dst; 
#if 1
	UINT32 CONST *	RESTRICT src = (UINT32 CONST *)src8;
	INT32			t;
	--src;
	for( ; nb > 3; nb -= 4 )
	{
		t = *++src;
		*++dst = (t & 0xff00ff00) | ((t>>16) & 0xff) | ((t&0xff) << 16);
		t = *++src;
		*++dst = (t & 0xff00ff00) | ((t>>16) & 0xff) | ((t&0xff) << 16);
		t = *++src;
		*++dst = (t & 0xff00ff00) | ((t>>16) & 0xff) | ((t&0xff) << 16);
		t = *++src;
		*++dst = (t & 0xff00ff00) | ((t>>16) & 0xff) | ((t&0xff) << 16);
	}
	for( ; nb > 0; --nb )
	{
		t = *++src;
		*++dst = (t & 0xff00ff00) | ((t>>16) & 0xff) | ((t&0xff) << 16);
	}
#else
	for( ; nb>0; --nb )
	{
		*++dst = (*src8 << 24) | (*(src8 + 1) << 16) | (*(src8 + 2) << 8) | 0xff;
		src8 += 3;
	}
#endif
}

//not used
//todo not endian safe
void	bitcon::bgra8_to_rgba8_slow( UINT8 CONST * RESTRICT src8, UINT8* RESTRICT dst8, INT32 nb )
{
	UINT32*	RESTRICT dst = (UINT32*)dst8;
	INT32 t;

	--src8;
	--dst;
	for( ; nb>3; nb-=4 )
	{
		t = *++src8<<16;
		t |= *++src8<<8;
		t |= *++src8;
		*++dst = t | (*++src8<<24);
		t = *++src8<<16;
		t |= *++src8<<8;
		t |= *++src8;
		*++dst = t | (*++src8<<24);
		t = *++src8<<16;
		t |= *++src8<<8;
		t |= *++src8;
		*++dst = t | (*++src8<<24);
		t = *++src8<<16;
		t |= *++src8<<8;
		t |= *++src8;
		*++dst = t | (*++src8<<24);
	}
	for( ; nb>0; --nb )
	{
		t = *++src8<<16;
		t |= *++src8<<8;
		t |= *++src8;
		*++dst = t | (*++src8<<24);
	}
}

//used
void	bitcon::rgb8_to_rgba8_slow( UINT8 CONST * RESTRICT src8, UINT8* RESTRICT dst8, INT32 nb, UINT8 CONST alpha )
{
	UINT32*			RESTRICT dst = (UINT32*)dst8;
	INT32			r,g,b;
	UINT32 CONST	a = PACK_RGBA( 0,0,0, alpha );
	--src8;
	for( ;nb>3; nb-=4 )
	{
	//	PIXEL 1
		r = *++src8;
		g = *++src8;
		b = *++src8;
		*dst++ = a | PACK_RGB( r,g,b );
	//	PIXEL 2
		r = *++src8;
		g = *++src8;
		b = *++src8;
		*dst++ = a | PACK_RGB( r,g,b );
	//	PIXEL 3
		r = *++src8;
		g = *++src8;
		b = *++src8;
		*dst++ = a | PACK_RGB( r,g,b );
	//	PIXEL 4
		r = *++src8;
		g = *++src8;
		b = *++src8;
		*dst++ = a | PACK_RGB( r,g,b );
	}
	for( ;nb>0; --nb )
	{
		r = *++src8;
		g = *++src8;
		b = *++src8;
		*dst++ = a | PACK_RGB( r,g,b );
	}
}

//used
//todo not endian safe
void	bitcon::rgba8_to_rgba8_incrust_green( UINT8 CONST * RESTRICT src8, UINT8* RESTRICT dst8, INT32 nb, UINT8 CONST * CONST RESTRICT lut )
{
	UINT32*	RESTRICT src32 = (UINT32*)src8;
	UINT32*	RESTRICT dst32 = (UINT32*)dst8;
	for( ; nb > 0; --nb )
	{
		UINT32 u32 = *src32++;
		SET_BYTE_3( u32, lut[GET_BYTE_1(u32)] );	// lut[g] -> a
		*dst32++ = u32;
	}
}

//todo not endian safe
void	bitcon::bgra8_to_rgba8_incrust_green( UINT8 CONST * RESTRICT src8, UINT8* RESTRICT dst8, INT32 nb, UINT8 CONST * CONST RESTRICT lut )
{
	UINT32*	RESTRICT src32 = (UINT32*)src8;
	UINT32*	RESTRICT dst32 = (UINT32*)dst8;
	for( ; nb > 0; --nb )
	{
		UINT32 u32 = *src32++;
#if AAA_ENDIAN_LITTLE()
		UINT32 g = (u32 >> 8) & 0xFF;
		*dst32++ =	((u32 & 0x000000FFu) << 16) |
					((u32 & 0x00FF0000u) >> 16) |
					( u32 & 0x0000FF00u)        |
					( UINT32(lut[g]) << 24);
#else
		UINT32 g = (u32 >> 16) & 0xFF;
		*dst32++ =	((u32 & 0xFF000000u) >> 16) |
					((u32 & 0x0000FF00u) << 16) |
					( u32 & 0x00FF0000u)        |
					UINT32(lut[g];
#endif
	}
}
/*
//used
void	bitcon::rgb8_to_bgra8_incrust( UINT8* RESTRICT src8, UINT8* RESTRICT  dst8, INT32 nb, UINT8 CONST * CONST RESTRICT lut )
{
	if( !b_tab_grey8_init )
		init_tab_grey8();

	UINT32*	RESTRICT dst = (UINT32*)dst8;
	UINT32	d;
	UINT32	g;
	UINT32	t;
	--src8;
	--dst;
	for( ;nb>0; --nb )
	{
		d = *++src8;
		g = int16_red2grey_s8[d];

		t = *++src8;
		d |= t<<8;
		g += int16_green2grey_s8[t];

		t = *++src8;
		d |= t<<16;
		g += int16_blue2grey_s8[t];

		d |= lut[ g>>8 ]<<24; 
		*++dst = d;
	}
}
*/
//todo not endian safe
namespace {
	//todo check name ok
	FINLINE	UINT32	RGB8_TO_ARGB8_LUT( INT32 CONST r, INT32 CONST g, INT32 CONST b, UINT8 CONST * CONST lut )
	{
		return PACK_RGBA( r,g,b, lut[color::rgb_to_grey( r,g,b )] );
	}
	FINLINE	UINT32	RGB8_TO_RGBA8_LUT( UINT32 CONST rgb, UINT8 CONST * CONST RESTRICT lut )
	{
		UINT32 u32 = rgb;
		SET_BYTE_3( u32, lut[ color::rgb_to_grey(GET_BYTE_0(rgb), GET_BYTE_1(rgb), GET_BYTE_2(rgb)) ] );
		return u32;
	}
};

//used
//todo not endian safe
void	bitcon::bgr8_to_rgba8_incrust( UINT8 CONST * RESTRICT src8, UINT8* RESTRICT dst8, INT32 nb, UINT8 CONST * CONST RESTRICT lut )
{
	//if( !b_tab_grey8_init )
	//	init_tab_grey8();

	UINT32*			RESTRICT dst = (UINT32*)dst8;
	UINT32 CONST *	RESTRICT src = (UINT32 CONST *)src8;
	UINT32	d;
	UINT32	s;
	--src;
	--dst;
#if 1
	for( ;nb>3; nb-=4 )
	{
	//	pixel 1
		s = *++src;
		*++dst = RGB8_TO_RGBA8_LUT( s & 0xffffff, lut ); 
	//	pixel 2
		d = *++src;
		*++dst = RGB8_TO_RGBA8_LUT( s>>24 | (d & 0xffff) << 8, lut ); 
	//	pixel 3
		s = *++src;
		*++dst = RGB8_TO_RGBA8_LUT( (d>>16) | (s&0xff)<<16, lut ); 
	//	pixel 4
		*++dst = RGB8_TO_RGBA8_LUT( s>>8, lut ); 
	}
//	pixel 1
	if( nb == 0 )	return;
	s = *++src;
	*++dst = RGB8_TO_RGBA8_LUT( s & 0xffffff, lut ); 
//	pixel 2
	if( nb == 1 )	return;
	d = *++src;
	*++dst = RGB8_TO_RGBA8_LUT( s>>24 | (d & 0xffff) << 8, lut ); 
//	pixel 3
	if( nb == 2 )	return;
	*++dst = RGB8_TO_RGBA8_LUT( ((d>>16)<<8) | (s>>24), lut ); 
#else
	UINT32	g;
	nb = shift_len_check_align( __FUNCTION__, nb, 2 );
	for( ;nb>0; --nb )
	{
		//	pixel 1
		s = *++src;

		d = s & 0xffffff;
		g = U32_RED2GREY_SHIFT8(d&0xff) + U32_GREEN2GREY_SHIFT8((d>>8)&0xff) + U32_BLUE2GREY_SHIFT8((d>>16)&0xff);

		*++dst = d | lut[ g>>8 ]<<24; 

		//	pixel 2
		d = (s>>24) & 0xff;
		g = U32_RED2GREY_SHIFT8(d);

		s = *++src;
		d |= (s & 0xffff) << 8;

		g += U32_GREEN2GREY_SHIFT8(s&0xff) + U32_BLUE2GREY_SHIFT8((s>>8)&0xff);

		*++dst = d | lut[ g>>8 ]<<24;

		//	pixel 3
		d = (s>>16) & 0xffff;
		g = U32_RED2GREY_SHIFT8(d&0xff) + U32_GREEN2GREY_SHIFT8((d>>8)&0xff);

		s = *++src;
		t = s & 0xff;
		d |= t<<16;
		g += U32_BLUE2GREY_SHIFT8(t);

		*++dst = d | lut[ g>>8 ]<<24; 

		//	pixel 4
		d = s>>8;
		g = U32_RED2GREY_SHIFT8(d&0xff) + U32_GREEN2GREY_SHIFT8((d>>8)&0xff) + U32_BLUE2GREY_SHIFT8((d>>16)&0xff);

		*++dst = d | lut[ g>>8 ]<<24;
	}
#endif
}


//use
void	bitcon::rgb8_to_rgba8_incrust( UINT8 CONST * RESTRICT src8, UINT8* RESTRICT dst8, INT32 nb, UINT8 CONST * CONST RESTRICT lut )
{
	//if( !b_tab_grey8_init )
	//	init_tab_grey8();

	UINT32*			RESTRICT dst = (UINT32*)dst8;
	UINT32 CONST *	RESTRICT src = (UINT32 CONST *)src8;
	UINT32			d;
	UINT32			s;
	--src;
	--dst;
	for( ;nb>3; nb-=4 )
	{
	//	pixel 1
		s	=	*++src;
		*++dst	=	RGB8_TO_ARGB8_LUT(	GET_BYTE_0(s),	GET_BYTE_1(s),	GET_BYTE_2(s),	lut	);
	//	pixel 2
		d	=	*++src;
		*++dst	=	RGB8_TO_ARGB8_LUT(	GET_BYTE_3(s),	GET_BYTE_0(d),	GET_BYTE_1(d),	lut ); 
	//	pixel 3
		s	=	*++src;
		*++dst	=	RGB8_TO_ARGB8_LUT(	GET_BYTE_2(d),	GET_BYTE_3(d),	GET_BYTE_0(s),	lut	); 
	//	pixel 4
		*++dst	=	RGB8_TO_ARGB8_LUT(	GET_BYTE_1(s),	GET_BYTE_2(s),	GET_BYTE_2(s),	lut	);
	}
//	pixel 1
	if( nb == 0 )	return;
	s	=	*++src;
	*++dst	=	RGB8_TO_ARGB8_LUT(	GET_BYTE_0(s),	GET_BYTE_1(s),	GET_BYTE_2(s),	lut	);
//	pixel 2
	if( nb == 1 )	return;
	d	=	*++src;
	*++dst	=	RGB8_TO_ARGB8_LUT(	GET_BYTE_3(s),	GET_BYTE_0(d),	GET_BYTE_1(d),	lut ); 
//	pixel 2
	if( nb == 2 )	return;
	s	=	*++src;
	*++dst	=	RGB8_TO_ARGB8_LUT(	GET_BYTE_2(d),	GET_BYTE_3(d),	GET_BYTE_0(s),	lut	); 
}
// SC08
// 
//todo not endian safe
//used
//	Y = (54 * R + 183 * G + 19 * B)/256;
//used
//todoqqq do it with component
void	bitcon::rgb8_to_r8_fast( UINT8 CONST * RESTRICT src8, UINT8* RESTRICT dst8, INT32 nb )
{
//	if( !b_tab_grey8_init )
//		init_tab_grey8();
	--src8;
	--dst8;
	for( ;nb>0; --nb )
	{
		UINT32 CONST t = color::U16_RED2GREY_SHIFT8(*++src8) + color::U16_GREEN2GREY_SHIFT8(*++src8) + color::U16_BLUE2GREY_SHIFT8(*++src8);
		*++dst8 = t >> 8;
	}
}
// SC08
//todo not endian safe
//	Y = (54 * R + 183 * G + 19 * B)/256;
void	bitcon::rgba8_to_r8_fast( UINT8 CONST * RESTRICT src8, UINT8* RESTRICT dst8, INT32 nb )
{
//	if( !b_tab_grey8_init )
//		init_tab_grey8();
	--src8;
	--dst8;
	for( ; nb > 0; --nb )
	{
		UINT32 CONST t = color::U16_RED2GREY_SHIFT8(*++src8) + color::U16_GREEN2GREY_SHIFT8(*++src8) + color::U16_BLUE2GREY_SHIFT8(*++src8);
		*++dst8 = t >> 8;
		++src8;
	}
}

void	bitcon::rgb8_green_to_r8( UINT8 CONST * RESTRICT src8, UINT8* RESTRICT dst8, INT32 nb )
{
	src8 += 2;
	--dst8;
	for( ;nb>0; --nb )
	{
		*++dst8 = *src8;
		src8 += 3;
	}
}
void	bitcon::rgba8_green_to_r8( UINT8 CONST * RESTRICT src8, UINT8* RESTRICT dst8, INT32 nb )
{
	src8 += 2;
	--dst8;
	for( ;nb>0; --nb )
	{
		*++dst8 = *src8;
		src8 += 4;
	}
}

void	bitcon::yuyv_to_r8_fast( UINT8 CONST * RESTRICT src8, UINT8* RESTRICT dst8, INT32 nb )
{
	--dst8;
	for( ; nb > 0; --nb )
	{
		*++dst8 = *src8;
		src8 += 2;
	}
}

//todo not endian safe
void	bitcon::rgb8_to_r8_lut( UINT8 CONST * RESTRICT src8, UINT8* RESTRICT dst8, INT32 nb, UINT8 CONST * CONST RESTRICT lut )
{
	//if( !b_tab_grey8_init )
	//	init_tab_grey8();
	--src8;
	--dst8;
	for( ; nb > 0; --nb )
	{
		UINT32 CONST t = color::U16_RED2GREY_SHIFT8(*++src8) + color::U16_GREEN2GREY_SHIFT8(*++src8) + color::U16_BLUE2GREY_SHIFT8(*++src8);
		*++dst8 = lut[t >> 8];
	}
}
// SC08
//todo not endian safe
//	Y = (54 * R + 183 * G + 19 * B)/256;
void	bitcon::rgba8_to_r8_lut( UINT8 CONST * RESTRICT src8, UINT8* dst8, INT32 nb, UINT8 CONST * CONST RESTRICT lut )
{
	//if( !b_tab_grey8_init )
	//	init_tab_grey8();
	--src8;
	--dst8;
	for( ; nb > 0; --nb )
	{
		UINT32 CONST t = color::U16_RED2GREY_SHIFT8(*++src8) + color::U16_GREEN2GREY_SHIFT8(*++src8) + color::U16_BLUE2GREY_SHIFT8(*++src8);
		*++dst8 = lut[t >> 8];
		++src8;
	}
}

/*
void	bitcon::rgb8_to_bgra8_incrust_green( UINT8* RESTRICT src8, UINT8* RESTRICT dst8, INT32 nb, UINT8 CONST * CONST RESTRICT lut )
{
	nb = shift_len( __FUNCTION__, nb, 2 );

	UINT32*	RESTRICT dst = (UINT32*)dst8;
	//	UINT32*	RESTRICT src = (UINT32*)src8;
	UINT32	t;
	//	UINT32	s;
	--src8;
	--dst;
	for( ;nb>0; --nb )
	{
		t = *++src8;
		t |= (*++src8)<<8;
		t |= lut[*src8]<<24; 
		t |= (*++src8<<16);
		*++dst = t;
		t = *++src8;
		t |= (*++src8)<<8;
		t |= lut[*src8]<<24; 
		t |= (*++src8<<16);
		*++dst = t;
		t = *++src8;
		t |= (*++src8)<<8;
		t |= lut[*src8]<<24; 
		t |= (*++src8<<16);
		*++dst = t;
		t = *++src8;
		t |= (*++src8)<<8;
		t |= lut[*src8]<<24; 
		t |= (*++src8<<16);
		*++dst = t;
	}
}


void	bitcon::rgb8_to_bgra8_incrust_green( UINT8* RESTRICT src8, UINT8* RESTRICT dst8, INT32 nb, UINT8* RESTRICT lut )
{
	UINT32*	RESTRICT dst = (UINT32*)dst8;
	UINT32	t;
	--src8;
	--dst;
	for( ;nb>0; --nb )
	{
		t = *++src8;
		t |= (*++src8)<<8;
		t |= lut[*src8]<<24; 
		t |= (*++src8<<16);
		*++dst = t;
	}
}
*/

//todo not endian safe
void	bitcon::rgb8_to_bgra8_incrust_green( UINT8 CONST * RESTRICT src8, UINT8* RESTRICT dst8, INT32 nb, UINT8 CONST * CONST RESTRICT lut )
{
	UINT32*	RESTRICT dst = (UINT32*)dst8;
	UINT32	t;
	INT32	a;
	--src8;
	--dst;
	for( ; nb > 0; --nb )
	{
		t = *++src8;
		a = (*src8) << 1;

		t |= (*++src8)<<8;
		a -= *src8;

		t |= (*++src8<<16);
		a -= *src8;

		a = CLAMP( (a>>1)+128, 0 , 255 );
		t |= lut[a]<<24; 
		*++dst = t;
	}
}

//GREY
//used
void	bitcon::r8_to_rgba8_fast( UINT8 CONST * RESTRICT src8, UINT8* RESTRICT dst8, INT32 nb, UINT8 CONST alpha )
{
	UINT32*			RESTRICT dst	= (UINT32*)dst8;
	UINT8			r8;
	nb = shift_len_check_align( __FUNCTION__, nb, 2 );

	UINT32 CONST a32 = PACK_RGBA( 0,0,0, alpha );
	--src8;
	--dst;
	for( ; nb > 0; --nb )
	{
		r8 = *++src8;
		*++dst = a32 | PACK_RGB( r8,r8,r8 );

		r8 = *++src8;
		*++dst = a32 | PACK_RGB( r8,r8,r8 );

		r8 = *++src8;
		*++dst = a32 | PACK_RGB( r8,r8,r8 );

		r8 = *++src8;
		*++dst = a32 | PACK_RGB( r8,r8,r8 );
	}
}

//unused
void	bitcon::r8_to_rgba8_slow( UINT8 CONST * RESTRICT src8, UINT8* RESTRICT dst8, INT32 nb, UINT8 CONST alpha )
{
	UINT8	g;

	nb = shift_len_check_align( __FUNCTION__, nb, 2 );

	--src8;
	--dst8;
	for( ; nb > 0; --nb )
	{
		g = *++src8;
		*++dst8 = g;
		*++dst8 = g;
		*++dst8 = g;
		*++dst8 = alpha;
		g = *++src8;
		*++dst8 = g;
		*++dst8 = g;
		*++dst8 = g;
		*++dst8 = alpha;
		g = *++src8;
		*++dst8 = g;
		*++dst8 = g;
		*++dst8 = g;
		*++dst8 = alpha;
		g = *++src8;
		*++dst8 = g;
		*++dst8 = g;
		*++dst8 = g;
		*++dst8 = alpha;
	}
}

//used
void	bitcon::r8_to_rgba8_lut( UINT8 CONST * RESTRICT src8, UINT8* RESTRICT dst8, INT32 nb, UINT8 CONST * CONST RESTRICT lut )
{
	nb = shift_len_check_align( __FUNCTION__, nb, 2 );

	UINT32*	RESTRICT dst = (UINT32*)dst8;
	UINT32	r8;
	UINT32 CONST a32 = PACK_RGBA( 0,0,0, 0xff );

	--src8;
	--dst;
	for( ; nb > 0; --nb )
	{
		r8 = lut[*++src8];
		*++dst =  a32 | PACK_RGB( r8,r8,r8 );
		r8 = lut[*++src8];
		*++dst =  a32 | PACK_RGB( r8,r8,r8 );
		r8 = lut[*++src8];
		*++dst =  a32 | PACK_RGB( r8,r8,r8 );
		r8 = lut[*++src8];
		*++dst =  a32 | PACK_RGB( r8,r8,r8 );
	}
}

//used
void	bitcon::r8_to_r8_lut( UINT8 CONST * RESTRICT src8, UINT8* RESTRICT dst8, INT32 nb, UINT8 CONST * CONST RESTRICT lut )
{
	nb = shift_len_check_align( __FUNCTION__, nb, 2 );

	--src8;
	--dst8;
	for( ; nb > 0; --nb )
	{
		*++dst8 = lut[*++src8];
		*++dst8 = lut[*++src8];
		*++dst8 = lut[*++src8];
		*++dst8 = lut[*++src8];
	}
}

bool	bitcon::disparity16_to_rgba( UINT8 CONST * RESTRICT src8, UINT8* RESTRICT dst8, INT32 nb, REAL CONST min, REAL CONST max, UINT8 CONST alpha )
{
	// Convert short disparity data to 8-bits per pixel
	UINT8*			RESTRICT dst		= dst8;
	UINT16 CONST *	RESTRICT pt		= (UINT16 CONST *)src8;

	UINT32 CONST	i_min	= (UINT32)( min * ( 256. * 255. ) );
	UINT32 CONST	i_max	= (UINT32)( max * ( 256. * 255. ) );
	REAL   CONST	factor	= REAL(255.) * OVER_ONE_AS_REAL(max - min);

	UINT32	t;
	UINT8	out;
	--dst;
	--pt;
	for( ; nb > 0; --nb )
	{
		t = *++pt;	//todo 2025 Nov redone check it is ok (was UINT8)
		if( *pt >= 0xFF00 )
		{
			// bgra
			*++dst = 0x0;
			*++dst = 0x0;
			*++dst = 0x7f;
			*++dst = alpha;
		}
		else if( t <= i_min )
		{
			*++dst = 0xff;
			*++dst = 0;
			*++dst = 0;
			*++dst = alpha;
		}
		else if( t >= i_max )
		{
			*++dst = 0x0;
			*++dst = 0xff;
			*++dst = 0x0;
			*++dst = alpha;
		}
		else
		{
			out = UINT8( ( t - i_min ) * factor ) ;
			*++dst = out;
			*++dst = out;
			*++dst = out;
			*++dst = alpha;
		}
	}
	return true;
}

//used check src order
//todo not endian safe
void	bitcon::r16_to_rgba8_fast( UINT8 CONST * RESTRICT src8, UINT8* RESTRICT dst8, INT32 nb, REAL CONST min, REAL CONST max, UINT8 CONST alpha )
{
	UINT32*	RESTRICT dst					= (UINT32*)dst8;
	UINT32 CONST *	RESTRICT src			= (UINT32 CONST *)src8;
	UINT32 CONST 	i_min		= UINT32(min*(256.*255.-1.));
	UINT32 CONST 	i_max		= UINT32(max*(256.*255.-1.));
	UINT32 CONST	i_factor	= ( min != max ) ? UINT32( (256.*256.)/(max-min) ) : 0;
	UINT32 CONST	a			= alpha << 24;
	UINT32			t;
	UINT32			out;

	nb = shift_len_check_align( __FUNCTION__, nb, 1 );

	--src;
	--dst;
	for( ; nb > 0; --nb )
	{
		t = UINT32( (*++src & 0xffff0000) >> 16 );
		if( t >= 0xff00 )
			out = a | 0x00ffffff;
		else 
		{
			if( t <= i_min )
				out = a;
			else if( t >= i_max )
				out = a | 0x00ffffff;
			else
			{
				out = ( ( t - i_min ) * i_factor ) >> 24;
				out =  a | out | out << 8 | out << 16;
			}
		}
		*++dst = out;
		t = UINT32( *src & 0xffff );
		if( t >= 0xff00 )
			out = a | 0x00ffffff;
		else 
		{
			if( t <= i_min )
				out = a;
			else if( t >= i_max )
				out = a | 0x00ffffff;
			else
			{
				out = ( ( t - i_min ) * i_factor ) >> 24;
				out =  a | out | out << 8 | out << 16;
			}
		}
		*++dst = out;
	}
}

//used but wrong
//todo not endian safe
void	bitcon::r16_to_rgba8_slow( UINT8 CONST * RESTRICT src8, UINT8* RESTRICT dst8, INT32 nb, REAL CONST min, REAL CONST max, UINT8 CONST alpha )
{
	UINT32*			RESTRICT dst		= (UINT32*)dst8;
	UINT16 CONST *	RESTRICT src		= (UINT16 CONST *)src8;
	UINT32 CONST	i_min	= UINT32(min*(256.*255.-1.));
	UINT32 CONST	i_max	= UINT32(max*(256.*255.-1.));
	UINT32 CONST	i_factor	= ( min != max ) ? UINT32( (256.*256.)/(max-min) ) : 0;
	UINT32 CONST	a			= alpha << 24;
	UINT32			t;
	UINT32			out;



/*	if( nb & 3 )
		convert_err_print( __FUNCTION__, 8 );
	nb >>= 2;
*/
	--src;
	--dst;
	for( ; nb > 0; --nb )
	{
		t = UINT32(( *++src & 0xffff ));
		if( t >= 0xff00 )
			out = 0;
		else 
		{
			if( t <= i_min )
				out = 0x7f0000ff;
			else if( t >= i_max )
				out = 0x7f00ff00;
			else
			{
				out = ( ( t - i_min ) * i_factor ) >> 24;
				out =  a | out | out << 8 | out << 16;
			}
		}
		*++dst = out;
	}
}

//todo faster
void	bitcon::rgba8_incrust( UINT8 * RESTRICT p8, INT32 nb, UINT8 CONST * CONST RESTRICT lut )
{
	UINT32*	RESTRICT p = (UINT32*)p8;
	--p;
	for( ; nb > 0; --nb )
	{
		//	Color
		INT32 CONST v = *++p & 0xffffff;
		*p = v | lut[color::rgb_to_grey( v&0xff, (v>>8)&0xff, v>>16 )] << 24;
	}
}

//todo faster
void	bitcon::bgra8_incrust( UINT8 * RESTRICT p8, INT32 nb, UINT8 CONST * CONST RESTRICT lut )
{
	UINT32*	RESTRICT p = (UINT32*)p8;
	--p;
	for( ; nb > 0; --nb )
	{
		//	Color
		INT32 CONST v = *++p & 0xffffff;
		*p = v | lut[color::rgb_to_grey( (v>>16), (v>>8)&0xff, v&0xff )] << 24;
	}
}