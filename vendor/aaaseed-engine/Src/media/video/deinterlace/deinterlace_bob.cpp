#include "deinterlace_bob.h"
#include "spy.h"
#include "../image_flux.h"
#include "infrastructure/param/param_st.h"

FACTORY_CREATE_V1( c_deint_bob, deinterlace_bob, Deinterlace Bob, deinterlace_bob );

namespace n_deint_bob
{
	static	CONST	INT32	BASE_PARAM_NB	= 2;
	static	CONST	INT32	GROUP_NB		= 0;

	static	CONST	INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
											+	GROUP_NB;

	static	ST_PARAM	param[PARAM_NB_MAX] =
	{
		ST_PARAM_BOOL_OFF( show_deinterlace )
		ST_PARAM_BOOL_OFF( field_order )
	};
}

void	c_deint_bob::param_init_pt()
{
INT32	h=0;

	param_set_pt( h, _b_show_deinterlace );
	param_set_pt( h, _field_order );

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_deint_bob)
{
	_min_image_nb = 2;
//	set_name("deint_bob");
	param_init_with( n_deint_bob::param, n_deint_bob::PARAM_NB_MAX);
}

//void	init_with_size( INT32 p_width, INT32 p_height )
//{
//
//}


void	c_deint_bob::close()
{
}

void	c_deint_bob::init( INT32 size_x, INT32 size_y )
{
}

c_deint_bob::~c_deint_bob()
{
	close();
}

BOOL	c_deint_bob::is_field_needed( BOOL b_double_framerate )
{
	if( b_double_framerate )
	{
		return TRUE;
	}
	else
	{
		return TRUE;	//todo strange to test for nothing
	}
}

void	c_deint_bob::run_frame( UINT8* src, UINT8* dst )
{
//    ERR_PRINT_STRING( "Bob is not a suitable deinterlacing method at half framerate, fallback to copy" );
	simple_copy( src, dst );
}

void	c_deint_bob::run_full( c_image_flux* flux_in, UINT8* dst, BOOL b_second_pass )
{
//// basic deinterlacing based
//	MEMCPY( dst8, src, width * height * 4);

	_b_is_processed = FALSE;
	if( _b_doublerate )
	{
		c_img_with_lock* img_a = nullptr;
		if( _image_index > 2 )
		{
			if( b_second_pass )
			{
				img_a = flux_in->lock_and_get_image_index( _image_index );
			}
			else
			{
				img_a = flux_in->lock_and_get_image_index( _image_index - 1 );
			}
		}
		else
		{
			img_a = flux_in->lock_and_get_image_index( _image_index );
		}

		// need to check if both fields are available, one can be lock and not the other
		if( !img_a )
		{
			ERR_PRINT_STRING( "nothing ready in image_flux" );
		}
		else
		{
			UINT8*	src_a = img_a->get_data_uint8();

			if( src_a )
			{
				_nb_bit_out = img_a->get_byte_per_pixel() * 8;

				TBUF_ADD( tbuf::CH_VIDEO_PROCESS, 1., "video_process", this );

				MEMCPY( dst, src_a, _size_x * _size_y * 4);
			}
			img_a->unlock();

			if( src_a )
			{
				flux_in->clear_empty();	//todo set_empty() ? set_change() ? 
				TBUF_ADD( tbuf::CH_VIDEO_PROCESS, 0., nullptr, this );
				_size_out_y = _size_y;
				_b_is_processed = TRUE;
			}
		}
	}
}


//INT32 c_deint_bob::run_field( UINT8* src, UINT8* src_prev, UINT8* dst8 )
//{
//	// bob copy field, we gain 1 field because the field are not combined
//	MEMCPY( dst8, src, width * height * 4 );
//	return height;
//}