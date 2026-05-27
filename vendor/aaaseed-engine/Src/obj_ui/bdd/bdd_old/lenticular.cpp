#include "lenticular.h"
#include "image/bind_img_2d.h"
#include "infrastructure/param/param_declare.h"


FACTORY_CREATE_V1( c_lenti, lenti, Lenticular, lent );

c_lenti* c_lenti::cur = nullptr;

namespace n_lenticular
{
	CONSTEXPR INT32 BASE_PARAM_NB	= 5;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_INT32(	image_start,			0, 1,		0, 100	)
		PARAM_DEF_INT32(	image_nb,				3, 4,		1, 100	)
		PARAM_DEF_BOOL_OFF(	image_increasing		)
		PARAM_DEF_BOOL_OFF(	lens_vertical			)
		PARAM_DEF_INT32(	pixel_by_image_by_lens,	1, 2,		1, 100	)
	};
}

void	c_lenti::param_init_pt_static()
{
	INT32	h = 0;

	param_set_pt( h, _img_start				);
	param_set_pt( h, _img_nb				);
	param_set_pt( h, _b_img_increasing		);
	param_set_pt( h, _b_lens_vertical		);	
	param_set_pt( h, _pixel_by_img_by_lens	);

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE( c_lenti )
{
	param_init_with( n_lenticular::param, n_lenticular::PARAM_NB_MAX ); // lenticular_param, LENTICULAR_PARAM_NB_MAX );
}
EMPTY_DESTRUCTOR( c_lenti )

//todo	deal with x aligned and stride
//todo	the access to the image struct should be done thru an object an fns
AAA_ERR		c_lenti::save_image( o_str CONST & filename )
{
	IF_THIS_NULL()
		return ERR_OBJ_NULL;

	AAA_ERR		retcode = ERR_ANY;
	INT32		index_step;
	INT32		width_src = 0;
	INT32		height_src = 0;
	c_img_2d*	img;
	c_img_2d*	img_dst;
	//INT32	i;
	//INT32	x;
	//INT32	y;
	INT32		offset_dst;
	UINT8*		p_src;
	UINT8*		p_dst;
	INT32		step_dst;
	INT32		step_src;
	INT32		lens_nb;

	INT32	index = _img_start;
	INT32	index_stop = _img_start + _img_nb;
	for( INT32 i = index; i !=  index_stop; ++i )
	{
		img = g_bind_img_2d->refresh( i );
		if( img )
		{
			if( i == index )
			{
				width_src = img->get_size_x();
				height_src = img->get_size_y();
				if( _b_lens_vertical )
				{
					if( IMOD( width_src, _pixel_by_img_by_lens) != 0 )
					{
						box_err( "image %s should have a width multiple of %d", g_bind_img_2d->get_bind()->get_str(i), _pixel_by_img_by_lens );
						goto exit;
					}
				}
				else
				{
					if( IMOD( height_src, _pixel_by_img_by_lens) != 0 )
					{
						box_err( "image %s should have a height multiple of %d", g_bind_img_2d->get_bind()->get_str(i), _pixel_by_img_by_lens );
						goto exit;
					}
				}
			}
			else if( width_src != img->get_size_x() || height_src != img->get_size_y() )
			{
				box_err( "image %s should have %dx%d size", g_bind_img_2d->get_bind()->get_str(i), width_src, height_src );
				goto exit;
			}
		}
		else
		{
			box_err( "Can't load image %s", g_bind_img_2d->get_bind()->get_str(i) );
			goto exit;
		}
	}

	GOOD_PRINT_STRING_NO_CR( "Building lenticular image" );
	if( _b_lens_vertical )
		img_dst = c_img_2d::img_init_with_size( nullptr, width_src * _img_nb, height_src, aaa::PIXEL_FORMAT::RGB_8, __FUNCTION__ );
	else
		img_dst = c_img_2d::img_init_with_size( nullptr, width_src, height_src * _img_nb, aaa::PIXEL_FORMAT::RGB_8, __FUNCTION__ );

	if( img_dst )
	{
		UINT8* data_src = img->get_data_uint8();
		UINT8* data_dst = img_dst->get_data_uint8();

		if( data_src && data_dst )
		{
			{
				bool	b_loc_increasing = _b_img_increasing;
				if( !_b_lens_vertical)
					b_loc_increasing = !b_loc_increasing;
				if( b_loc_increasing)
				{
					index = _img_start;
					index_stop = _img_start + _img_nb;
					index_step = 1;
				}
				else
				{
					index = _img_start + _img_nb - 1;
					index_stop = _img_start - 1;
					index_step = -1;
				}
			}
			if( _b_lens_vertical )
			{
				lens_nb = width_src / _pixel_by_img_by_lens;
				step_dst = img_dst->get_byte_per_pixel() * (_img_nb-1) * _pixel_by_img_by_lens;
				offset_dst = 0;
				//	treat each image one by one
				for( ; index != index_stop; index += index_step)
				{
					img = g_bind_img_2d->get_ready( index);
					step_src = ( img->get_channel_nb() == 4 ) ? 1 : 0 ;
					//	then line by line
					for( INT32 y = 0; y < height_src; ++y )
					{
						p_src = data_src + img->get_byte_pitch()*y;
						p_dst = data_dst + img_dst->get_byte_per_pixel()*( img_dst->get_size_x()*y + offset_dst);
						//	then lens by lens
						for ( INT32 x = 0; x < lens_nb; ++x )
						{
							for ( INT32 i = 0; i < _pixel_by_img_by_lens; ++i )
							{
								*p_dst++ = *p_src++;
								*p_dst++ = *p_src++;
								*p_dst++ = *p_src++;
								p_src += step_src;	//	make sure we jump unused channel
							}
							p_dst += step_dst;	//	skip space for the other images
						}
						if ( (y & 0x3f) == 0 )
							PRINT_STRING(".");
					}
					offset_dst += _pixel_by_img_by_lens;
				}
			}
			else
			{
				lens_nb = height_src / _pixel_by_img_by_lens;
				step_dst = img_dst->get_byte_per_pixel() * (_img_nb-1) * _pixel_by_img_by_lens;
				offset_dst = 0;
				//	treat each image one by one
				for( ; index != index_stop; index += index_step)
				{
					img = g_bind_img_2d->get_ready( index);
					step_src = ( img->get_channel_nb() == 4 ) ? 1 : 0 ;
					//	then lens by lens
					for( INT32 y = 0; y < lens_nb; ++y )
					{
						p_src = data_src + img->get_byte_pitch() * y * _pixel_by_img_by_lens;
						p_dst = data_dst + img_dst->get_byte_per_pixel() * ( img_dst->get_size_x() * (y*_pixel_by_img_by_lens*_img_nb + offset_dst) );
						//	then line by line
						for( INT32 i = 0; i < _pixel_by_img_by_lens; ++i )
						{
							for ( INT32 x = 0; x < width_src; ++x )
							{
								*p_dst++ = *p_src++;
								*p_dst++ = *p_src++;
								*p_dst++ = *p_src++;
								p_src += step_src;	//	make sure we jump unused channel
							}
						}
						if( (y & 0x3f) == 0 )
							PRINT_STRING(".");
					}
					offset_dst += _pixel_by_img_by_lens;
				}
			}
			retcode = img_dst->write( filename );
		}
		delete img_dst;
		PRINT_STRING("\n");
	}
exit:
	return retcode;
}
