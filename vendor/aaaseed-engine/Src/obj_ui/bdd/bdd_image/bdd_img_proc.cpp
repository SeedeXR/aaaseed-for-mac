#include "obj_ui/bdd/bdd_image/bdd_img_proc.h"
#include "math/rand.h"
#include "image/bind_img_2d.h"
#include "infrastructure/namer.h"
#include "gol/gol_color.h"
#include "image/img_compo.h"


FACTORY_CREATE_PROP_V1( c_bdd_img_proc, bdd_img_proc, Image from fns , image_proc, sub_menu="Image"; sel0="Image_Proc"; );

static	C_PCHAR_C	str_type_comb[10] = 
{
	"Raw fn1",
	"Raw fn2",
	"Interpolate",
	"Mult",
	"Min",
	"Max",
	"Cumul",
	"Modulate",
	"Kitchen1",
	"Kitchen2"
};

namespace n_bdd_img_proc
{
	CONSTEXPR INT32 BASE_NB_MAX		=	c_bdd::NO_GEO_PARAM_NB + 12;
	CONSTEXPR INT32 IMAGE_NB_MAX	=	16;
	CONSTEXPR INT32 GROUP_NB_MAX	=	1;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_NB_MAX
									+	IMAGE_NB_MAX
									+	GROUP_NB_MAX;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_NO_GEO_BASE_PARAMS

		PARAM_DEF_BOOL_OFF(			active )
		PARAM_DEF_BOOL_OFF(			do_rand )
		PARAM_DEF_SYMBO_PSTR_ZERO(	type,		str_type_comb )
		PARAM_DEF_BOOL_ON(			flip_fn )

		PARAM_DEF_NONE( fn1 )
		PARAM_DEF_NONE( fn2 )

			PARAM_DEF_REAL_INF(		rx,		1., .1	)
			PARAM_DEF_REAL_INF(		ry,		1., .1	)
			PARAM_DEF_REAL_INF(		ra,		1., .1	)

			PARAM_DEF_REAL_INF(		factor,	1., .1	)

		PARAM_DEF_GROUP( Image, IMAGE_NB_MAX )
			PARAM_DEF_BOOL_ON(			alpha )

			PARAM_DEF_INT32_XY(			size_pixel,		256, 64,		1, 4096	)


			PARAM_DEF_REAL_INF(			size_x,			10., 1	)
			PARAM_DEF_REAL_INF(			size_y,			10., 1	)
			PARAM_DEF_REAL_INF(			size_z,			10., 1	)
			PARAM_DEF_REAL_INF(			size_factor,	10., 1	)

			PARAM_DEF_REAL_INF(			cx,				.5, 0	)
			PARAM_DEF_REAL_INF(			cy,				.5, 0	)

			PARAM_DEF_REAL_ZERO(		sax				)
			PARAM_DEF_REAL_INF(			sfx,			1., .1	)
			PARAM_DEF_REAL_ZERO(		say				)
			PARAM_DEF_REAL_INF(			sfy,			1., .1	)


			PARAM_DEF_REAL_ONE(			move_green		)
			PARAM_DEF_REAL_ONE(			move_blue		)
			PARAM_DEF_REAL_ONE(			move_alpha		)

			PARAM_DEF_BOOL_OFF(				palette_use		)
			PARAM_DEF_BIND_2D_CURRENT_SEL(	palette_bind	)
			
	};
}


void	c_bdd_img_proc::param_init_pt()
{
	INT32	h = param_init_pt_no_geo();

	param_set_pt( h, get_pt_active() );
	param_set_pt( h, _b_do_rand );
	param_set_pt( h, type );
	param_set_pt( h, _b_flip );

	param_attach_obj( h, &fn1 );
	param_attach_obj( h, &fn2 );

	param_set_pt( h, rx );
	param_set_pt( h, ry );
	param_set_pt( h, ra );

	param_set_pt( h, factor );

	++h;
		param_set_pt( h, _b_alpha_generate );

		param_set_pt_2( h, size_pixel );

		param_set_pt_4( h, size_ui );

		param_set_pt( h, cx );
		param_set_pt( h, cy );

		param_set_pt( h, sax );
		param_set_pt( h, sfx );
		param_set_pt( h, say );
		param_set_pt( h, sfy );

		param_set_pt( h, move_green );
		param_set_pt( h, move_blue );
		param_set_pt( h, move_alpha );

		param_set_pt( h, _b_pal );
		param_set_pt( h, pal_bind );

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_bdd_img_proc)
{
	fn1.set_name( "Fn1" );
	fn1.set_root( this );
	fn2.set_name( "Fn2" );
	fn2.set_root( this );
	
	pt_fn1 = nullptr;
	pt_fn2 = nullptr;

	param_init_with( n_bdd_img_proc::param, n_bdd_img_proc::PARAM_NB_MAX );
}
EMPTY_DESTRUCTOR(c_bdd_img_proc)

//todo deal with error

AAA_ERR	c_bdd_img_proc::save_do_after( o_str CONST & filename_in )
{
	o_str& filename = o_str::push_name( filename_in );
		filename.drop_ext();
		filename.add_char( '1' );
		fn1.save_to_file_add_ext( filename );

		filename.set_char( -1, '2' );
		fn2.save_to_file_add_ext( filename );
	o_str::pop_name();
	return AAA_OK;
}

AAA_ERR	c_bdd_img_proc::load_do_after( o_str CONST & filename_in )
{
	o_str& filename = o_str::push_name( filename_in );
		filename.drop_ext();
		filename.add_char( '1' );
		c_namer::set_file_only( &fn1, filename );
		fn1.load_from_file_add_ext( filename );

		filename.set_char( -1, '2' );
		c_namer::set_file_only( &fn2, filename );
		fn2.load_from_file_add_ext( filename );
	o_str::pop_name();
	return AAA_OK;
}

static	c_rand_lin	hazard;

void c_bdd_img_proc::rand()
{
	factor = hazard.get_fp32_max(5);
	sfx = hazard.get_fp32_01();
	sax = hazard.get_fp32_01();
	sfy = hazard.get_fp32_01();
	say = hazard.get_fp32_01();

	rx = hazard.get_fp32_01();
	ry = hazard.get_fp32_01();
	ra = hazard.get_fp32_max(.1);

	fn1.freq_factor = hazard.get_fp32_max(5);
	fn2.freq_factor = hazard.get_fp32_01();
}

void c_bdd_img_proc::update()
{
	if( is_active() )
	{
		bdd_img_proc_cur = this;

		if( _b_do_rand )
			rand();

		fn1.update();
		fn2.update();

		scale_v3( size, size_ui, size_ui[3]);

		if( _b_flip )
		{
			pt_fn1 = &fn2;
			pt_fn2 = &fn1;
		}
		else
		{
			pt_fn1 = &fn1;
			pt_fn2 = &fn2;
		}

		if( _b_pal )
		{
			c_img_2d* img;
			img = g_bind_img_2d->get_ready( pal_bind );
			if( img && img->get_data_valid_rgb_uint8( __FUNCTION__ ) )
			{
				for( INT32 i=0; i<256; ++i )
				{
					*((INT32*)&pal[i]) = *(INT32*)img->get_color4ubv_from_uv(  REAL(i)/aaa::img::REAL_NEARLY_256, 0.);
				}
			}
		}
	}
}

FINLINE	REAL c_bdd_img_proc::compute_3fv_low( REAL* vec )
{
	REAL	f = .0;

	switch( type)
	{
	case 0:	//	raw 1
		f = fn1.compute( vec );
		break;
	case 1:	//	raw 2
		f = fn2.compute( vec );
		break;
	case 2:	//	interpolate add
		f = interpolate( pt_fn1->compute( vec ), pt_fn2->compute( vec ), factor );
		break;
	case 3:	//	mult
		f = pt_fn1->compute( vec ) * pt_fn2->compute( vec );
		break;
	case 4:
		f = MIN( fn1.compute( vec ), fn2.compute( vec ));
		break;
	case 5:
		f = MAX( fn1.compute( vec ), fn2.compute( vec ));
		break;
	case 6:	//	cumulate
		{
		REAL	loc[3];

		f = pt_fn2->compute( vec);

		loc[0] = vec[0];
		loc[1] = vec[1];
		loc[2] = REAL(452.356);

		loc[1] += pt_fn2->compute( loc);

		loc[0] += f;
		loc[2] = 0;

		f = pt_fn1->compute( loc);
		}
		break;
	case 7:	//	freq modulate
		{
		REAL	loc[3];
		f = pt_fn2->compute( vec);
		loc[0] = vec[0];
		loc[1] = vec[1];
		loc[2] = f;
		f = pt_fn1->compute( loc);
		}
		break;
	case 8:
		{
		REAL	loc[3];
		loc[0] = vec[0];
		loc[1] = vec[1];
		loc[2] = factor/pt_fn2->freq_factor;
		f = pt_fn1->compute( loc);
		}
		break;
	case 9:
		{
		REAL	loc[3];
		loc[0] = vec[0];
		loc[1] = vec[1];
		loc[2] = factor/pt_fn2->freq_factor;
		f = pt_fn1->compute( loc);

		loc[1] += SIN_TURN( vec[0] * rx + vec[1] * ry ) * ra / pt_fn2->freq_factor;
		loc[0] = f * factor / pt_fn2->freq_factor;
		f = pt_fn2->compute( loc);
		}
		break;
	}
	return f;
}

REAL c_bdd_img_proc::compute_3fv( REAL * vec )
{
	if( is_active() )
	{
		if( !pt_fn1 )
			update();
		return compute_3fv_low(vec);
	}
	return 0.;
}

//	this is ok because we work on 4 channel and that default alignment of pixel is 4 bytes
//todo we deal with a 4 channel image only
//todoq deal with UINT32 directly
void c_bdd_img_proc::compute( c_img_2d *img )
{
	UINT8*	data = img->get_data_uint8();
	if( !data )
		return;

	img->erase_filename();

	REAL	vec[3];
	UINT8	*pt;
	REAL	dx;
	REAL	dy;
	REAL	fx;
	REAL	fy;
	REAL	f;

	INT32	size_x; 
	INT32	size_y;
	INT32	byte_per_line;

	size_x = img->get_size_x();
	size_y = img->get_size_y();
	byte_per_line = img->get_byte_pitch();
	vec[2] = 0;

	dx = size[0]/size_x;
	dy = size[1]/size_y;

	fy = -cy * size[1];
	for( INT32 y = 0; y < size_y; ++y )
	{
		pt = data + y * byte_per_line;

		vec[1] = fy + SIN_TURN( (REAL)fy * sfy ) * say;

		fx = -cx*size[0];
		if( _b_pal )
		{
			UINT8*	src;
			for( INT32 x = size_x; x > 0; --x )
			{
				vec[0] = fx + SIN_TURN((REAL)fx * sfx) * sax;

				f = compute_3fv_low( vec );
				f *= aaa::img::REAL_NEARLY_256;
				src = pal[INT32(f)];
				*pt++ = src[0];
				*pt++ = src[1];
				*pt++ = src[2];
				*pt++ = src[3];

				fx += dx;
			}
		}
		else
		{
			for( INT32 x = size_x; x > 0; --x )
			{
				vec[0] = fx + SIN_TURN((REAL)fx * sfx) * sax;

				f = compute_3fv_low( vec);
				f *= aaa::img::REAL_NEARLY_256;
				*pt++ = UINT8(f);

				if( move_green != 0. )
				{
					vec[0] += move_green * sax;
					f = compute_3fv_low( vec ) * aaa::img::REAL_NEARLY_256;
				}
				*pt++ = UINT8(f);

				if( move_blue != 0. )
				{
					vec[0] += move_blue * sax;
					f = compute_3fv_low( vec ) * aaa::img::REAL_NEARLY_256;
				}
				*pt++ = UINT8(f);

				if( _b_alpha_generate )
				{
					if( move_alpha != 0. )
					{
						vec[0] += move_alpha * sax;
						f = compute_3fv_low( vec) * aaa::img::REAL_NEARLY_256;
					}
					*pt++ = UINT8(f);
				}
				else
					*pt++ = 255;

				fx += dx;
			}
		}
		fy += dy;
	}
}

void	c_bdd_img_proc::draw()
{
	if( is_active() )
	{
		c_img_2d* img = g_bind_img_2d->get_img_cur( aaa::PIXEL_FORMAT::RGBA_8, size_pixel[0], size_pixel[1], true, zero_v4fp32, __FUNCTION__ );
		if( img )
		{
			compute( img );
			g_bind_img_2d->do_after_a_compute();
		}
	}
}

c_bdd_img_proc	*bdd_img_proc_cur;
