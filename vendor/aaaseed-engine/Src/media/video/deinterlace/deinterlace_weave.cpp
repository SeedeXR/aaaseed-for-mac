#include "deinterlace_weave.h"
#include "spy.h"
#include "err.h"
#include "../image_flux.h"
#include "infrastructure/param/param_st.h"

FACTORY_CREATE_V1( c_deint_weave, deinterlace_weave, Deinterlace Weave, deinterlace_weave );

namespace n_deint_weave
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

void	c_deint_weave::param_init_pt()
{
INT32	h=0;

	param_set_pt( h, _b_show_deinterlace );
	param_set_pt( h, _field_order );

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_deint_weave)
{
	_min_image_nb = 2;
//	set_name("deint_weave");
	param_init_with( n_deint_weave::param, n_deint_weave::PARAM_NB_MAX);
}

c_deint_weave::~c_deint_weave()
{
	close();
}

BOOL	c_deint_weave::is_field_needed( BOOL b_double_framerate )
{
	if( b_double_framerate )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


void c_deint_weave::weave_32_field(UINT8* src_a, UINT8* src_b, UINT8* dst )
{
// basic deinterlacing based on 2 fields
// We receive Field1 & Field2 and we merge them to give a Frame

	// assuming RGB32 as input
	INT32	pitch = _size_x * 4;

	for( INT32 i = _size_y; i > 0; --i )
	{
		MEMCPY( dst, src_a, pitch );
		dst += pitch;
		src_a += pitch;
		MEMCPY( dst, src_b, pitch );
		dst += pitch;
		src_b += pitch;
	}	
}

void c_deint_weave::run_frame( UINT8* src, UINT8* dst )
{
// basic deinterlacing based on 1 frame
// Weave is a simple copy

	// assuming RGB32 as input
	MEMCPY( dst, src, _size_y * _size_x * 4);
}

void	c_deint_weave::close()
{
}

void	c_deint_weave::init( INT32 size_x, INT32 size_y )
{
}

void	c_deint_weave::run_full( c_image_flux* image_flux, UINT8* dst, BOOL second_pass )
{
	_b_is_processed = FALSE;
	if( _b_doublerate )
	{
		// need 2 frames to begin full deinterlacing
		//to take the same frame happens at the beginning only ?
		if( _image_index <= 2 )
			return;

		c_img_with_lock*	img_a;	// first field
		c_img_with_lock*	img_b;	// second fiel
			
		img_b = image_flux->lock_and_get_image_index( _image_index - 1 );
		img_a = image_flux->lock_and_get_image_index( _image_index );

		// need to check if both fields are available, one can be lock and not the other
		if( !img_a )
		{
			if( !img_b )
			{
				ERR_PRINT_STRING( "nothing ready in image_flux" );
			}
			else
			{
				img_b->unlock();
				ERR_PRINT_STRING( "nothing ready in image_flux" );
			}
		}
		else if( !img_b )
		{
			img_a->unlock();
			ERR_PRINT_STRING( "nothing ready in image_flux" );
		}
		else
		{
			UINT8*	src_a = img_a->get_data_uint8();
			UINT8*	src_b = img_b->get_data_uint8();
		
			if( src_a && src_b )
			{
				_nb_bit_out = img_a->get_byte_per_pixel() * 8;

				TBUF_ADD( tbuf::CH_VIDEO_PROCESS, 1., "video_process", this );
				// field based deinterlacing
				if( _field_order == 0 )	{	weave_32_field( src_a, src_b, dst );	}
				else					{	weave_32_field( src_b, src_a, dst );	}

			}
			img_a->unlock();
			img_b->unlock();

			if( src_a && src_b )
			{
				image_flux->clear_empty();	//todo set_empty() ? set_changed()?
				TBUF_ADD( tbuf::CH_VIDEO_PROCESS, 0., nullptr, this );
				_size_out_y = _size_y * 2;
				_b_is_processed = TRUE;
			}
		}
	}
}
