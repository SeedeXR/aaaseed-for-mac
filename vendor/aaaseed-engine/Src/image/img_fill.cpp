
#include "img.h"
#ifndef AAA_BITMAP_CONVERT_H
#	include "image/convert/bitmap_convert.h"
#endif

static	UINT32	colorbar_abgr[8] =
{
	0xffffffff,
	0xff00ffff,
	0xffffff00,
	0xff00ff00,
	0xffff00ff,
	0xff0000ff,
	0xffff0000,
	0xff000000
};

void c_img_2d::fill_with_colorbar()
{
	UINT8* data8 = get_data_valid_rgb_uint8( __FUNCTION__ );
	if( !data8 )	{	return;	}

	if( get_channel_nb() != 4 )
	{
		ERR_PRINT_STRING( "%s() : implemented only for RGBA", __FUNCTION__ );
		return;
	}

	UINT32*	data	=	(UINT32*)data8;
	INT32	sx		=	get_size_x();
	REAL	sx_over	=	get_size_x_over();
	INT32	sline	=	get_byte_pitch()  / 4;
	INT32	sy		=	get_size_y();

	UINT32	color;
	UINT32*	pt;

	for( INT32 i=0; i<sx; ++i )
	{
		INT32	index = INT32( 8*(REAL(i)+.5) * sx_over );
		color = colorbar_abgr[index];
		pt = data + i;
		for( INT32 j=sy; j>0; --j)
		{
			*pt = color;
			pt += sline;
		}
	}
}

void c_img_2d::fill_with_colorbar_grey( INT32 nb )	//	nb = 0 means as much as the resolution allow
{
	UINT8* data8 = get_data_valid_rgb_uint8( __FUNCTION__ );
	if( !data8 )	{	return;	}

	if( get_channel_nb() != 4 )
	{
		ERR_PRINT_STRING( "%s() : implemented only for RGBA", __FUNCTION__ );
		return;
	}

	if( nb == 0 )
		nb = 256;

	UINT32*	data	=	(UINT32*)data8;
	INT32	sx		=	get_size_x();
	INT32	sline	=	get_byte_pitch()  / 4;
	INT32	sy		=	get_size_y();

	UINT32	color;
	UINT32*	pt;

	for( INT32 i=0; i<sx; ++i )
	{
		INT32	g = ( 255 * INT32( nb * REAL(i) / sx ) ) / (nb-1);

		color = (0xff<<24) + (g<<16) + (g<<8) + g;
		pt = data + i;
		for( INT32 j=sy/2; j>0; --j )
		{
			*pt = color;
			pt += sline;
		}

		g = 255 -g;
		color = (0xff<<24) + (g<<16) + (g<<8) + g;
		for( INT32 j=sy-sy/2; j>0; --j )
		{
			*pt = color;
			pt += sline;
		}
	}
}

void	c_img_2d::fill_with_2x2( INT32 pat, FP32* col_0, FP32* col_1 )
{
	UINT8* data8 = get_data_valid_rgb_uint8( __FUNCTION__ );
	if( !data8 )	{	return;	}

	if( get_channel_nb() != 4 )
	{
		ERR_PRINT_STRING( "%s() : implemented only for RGBA", __FUNCTION__ );
		return;
	}

	UINT32	c0	=	bitcon::color_to_u32rgba8( col_0 );
	UINT32	c1	=	bitcon::color_to_u32rgba8( col_1 );

	UINT32*	data		=	(UINT32*)data8;
	INT32	sx			=	get_size_x();
	INT32	line_stride	=	get_byte_pitch()  / 4;
	INT32	sy			=	get_size_y();

	UINT32*	pt_line = data;
	for( INT32 j=sy; j>0; --j )
	{
		UINT32* pt = pt_line-1;
		UINT32	ca;
		UINT32	cb;
		if( j & 0x1 )
		{ 
			ca = ( pat & 0x01 ) ? c1 : c0;
			cb = ( pat & 0x02 ) ? c1 : c0;
		}
		else
		{ 
			ca = ( pat & 0x04 ) ? c1 : c0;
			cb = ( pat & 0x08 ) ? c1 : c0;
		}
		for( INT32 i=0; i<sx; ++i )
		{
			*++pt = ( i & 0x1 ) ? cb : ca;
		}
		pt_line += line_stride;
	}
}