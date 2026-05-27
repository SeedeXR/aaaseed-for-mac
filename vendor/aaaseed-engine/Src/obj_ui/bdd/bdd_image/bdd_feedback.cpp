#include "bdd_feedback.h"
#include "draw/seedcam.h"
#include "image/bind_img.h"
#include "image/bind_img_2d.h"
#include "infrastructure/viewport.h"
#include "math/rand.h"
#include "draw/map.h"
#include "infrastructure/aaa_window.h"
#include "fbo/fbo.h"
#include "infrastructure/layer/app.h"
#include "draw/render.h"
#include "gol/gol_draw.h"
#include "gol/gol_color.h"
#include "spy.h"


// bdd_tex_copy replace this old old bdd from the AAA beginning

FACTORY_CREATE_PROP_V1( c_bdd_feedback, bdd_feedback, Feedback, feedback, sub_menu="Image"; );

bool		c_bdd_feedback::b_allow;
bool		c_bdd_feedback::b_grab_needed;
INT32		c_bdd_feedback::grab_count = 2;

void	c_bdd_feedback::reset_grab()
{
	b_grab_needed = false;
	++grab_count;
}

void	c_bdd_feedback::set_grab_needed()
{
	b_grab_needed = true;
}

bool	c_bdd_feedback::is_grab_ready()
{
	return( grab_count <= 1 );
}

//hack should be initialize on
void c_bdd_feedback::init()
{
	_strobe_count = 0;
	_tex_size_u = 0;
	_tex_size_v = 0;	
}

static	C_PCHAR_C	str_size_grab[2] =
{
	"VIEW_WIDTH",
	"VIEW_HEIGHT"
};

namespace n_bdd_feedback
{
	CONSTEXPR INT32 BASE_NB_MAX			=	c_bdd::GEO_PARAM_NB+2;
	CONSTEXPR INT32 GRAB_NB_MAX			=	13;
	CONSTEXPR INT32 GRAB_TO_IMG_NB_MAX	=	4;
	CONSTEXPR INT32 GEO_NB_MAX			=	15;
	CONSTEXPR INT32 RENDER_NB_MAX		=	0;
	CONSTEXPR INT32 SEED_NB_MAX			=	8;
	CONSTEXPR INT32 GROUP_NB_MAX		=	4;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_NB_MAX
									+	GRAB_NB_MAX
									+	GRAB_TO_IMG_NB_MAX
									+	GEO_NB_MAX
									+	RENDER_NB_MAX
									+	SEED_NB_MAX
									+	GROUP_NB_MAX;
	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_BASE_PARAMS
		PARAM_DEF_BIND_2D_CURRENT_SEL(	bind_2d_dst	)
		PARAM_DEF_BOOL_OFF(			verbose_grab )

		PARAM_DEF_GROUP( Grabbing to texture, GRAB_NB_MAX )
			PARAM_DEF_BOOL_OFF(		grab_to_tex		)
			PARAM_DEF_INT32(		strobe,			0, 1,			1, 100	)
			PARAM_DEF_BASE_STR(		TYPE_SYMBOLIC,	size_x_p2,		9, 8,		1, PT_NB_STR(gstr::p2),			gstr::p2		)	//,c_feedback::update, nullptr },
			PARAM_DEF_BASE_STR(		TYPE_SYMBOLIC,	size_y_p2,		9, 8,		1, PT_NB_STR(gstr::p2),			gstr::p2		)	//clean feedback_update, nullptr },
			PARAM_DEF_BASE_STR(		TYPE_SYMBO_NEG,	size_x,			1, 0,		-2, PARAM_MAX_INT32,			str_size_grab	)
			PARAM_DEF_BASE_STR(		TYPE_SYMBO_NEG,	size_y,			1, 0,		-2, PARAM_MAX_INT32,			str_size_grab	)
			PARAM_DEF_UV_LOCKED(	tex_size		)
			PARAM_DEF_POINT_XY(		pos				)
			PARAM_DEF_BOOL_ON(		grab_alpha		)
			PARAM_DEF_BOOL_OFF(		mipmap_generate )
			PARAM_DEF_BOOL_ON(		smooth )
		PARAM_DEF_GROUP( Grabbing to image, GRAB_TO_IMG_NB_MAX )
			PARAM_DEF_BOOL_OFF(		grab_to_image )
			PARAM_DEF_BOOL_ON(		grab_to_image_avoid_gpu_transfert )
			PARAM_DEF_BASE_STR(		TYPE_SYMBO_NEG,	grab_to_image_size_x,		16, 1.,	-2, PARAM_MAX_INT32,	str_size_grab )
			PARAM_DEF_BASE_STR(		TYPE_SYMBO_NEG,	grab_to_image_size_y,		16, 1.,	-2, PARAM_MAX_INT32,	str_size_grab )

		PARAM_DEF_GROUP( Geo, GEO_NB_MAX )
			PARAM_DEF_BOOL_OFF(		draw )

			PARAM_DEF_POINT_XYZ(	center )

			PARAM_DEF_INT32_XYZ(	draw_nb,		3., 1.,		1., 128.	)

			PARAM_DEF_REAL_POS_ONE(	draw_space	)

			PARAM_DEF_POINT_XYZ(	rotation )

			PARAM_DEF_REAL_INF(		scale_x,		.5, 1.	)	//clean feedback_update, nullptr },feedback_update, nullptr },
			PARAM_DEF_REAL_INF(		scale_y,		.5, 1.	)	//clean feedback_update, nullptr },feedback_update, nullptr },
			PARAM_DEF_REAL_INF(		scale_z,		.5, 1.	)	//clean feedback_update, nullptr },feedback_update, nullptr },
			PARAM_DEF_REAL_INF(		scale_factor,	.5, 1.	)	//clean feedback_update, nullptr },feedback_update, nullptr },

//-		PARAM_DEF_GROUP( Render, RENDER_NB_MAX )
	//		PARAM_DEF_BOOL_OFF( feedback_random_x )
	//		PARAM_DEF_BOOL_OFF( feedback_random_y )

//			PARAM_DEF_BOOL_OFF( blur )
//			{	nullptr,	PARAM_INT32,	"pass_nb",			2., 1.,		1., 16.,	nullptr, nullptr },
		
		PARAM_DEF_GROUP( Seed, SEED_NB_MAX )
//todo	this 256 is a hack
			PARAM_DEF_INT32(		seed_type,		16, 0.,	0, 31+256	)
			PARAM_DEF_REAL_ONE(		seed_depth	)	//clean feedback_update, nullptr },feedback_update, nullptr },
		//	PARAM_DEF_BOOL_OFF(		seed_smooth )
			PARAM_DEF_REAL_POS(		seed_width,		1., .1	)

			PARAM_DEF_COLOR_RGBGA(	seed )
	};
}


CONSTRUCTOR_CREATE( c_bdd_feedback )
{
	param_init_with( n_bdd_feedback::param, n_bdd_feedback::PARAM_NB_MAX );
	init();
}

c_bdd_feedback::~c_bdd_feedback()
{
}

void c_bdd_feedback::param_init_pt()
{
	INT32	h = param_init_pt_geo();

	param_set_pt(		h, _s_grab_dst );
	param_set_pt(		h, _b_verbose_grab );

	++h;	//Grab to tex
		param_set_pt(		h, _b_grab_ui );
		param_set_pt(		h, _strobe );
		param_set_pt(		h, _size_x_p2 );
		param_set_pt(		h, _size_y_p2 );
		param_set_pt(		h, _size_x_ui );
		param_set_pt(		h, _size_y_ui );
		param_set_pt(		h, _tex_size_u );
		param_set_pt(		h, _tex_size_v );
		param_set_pt(		h, _grab_pos_x );
		param_set_pt(		h, _grab_pos_y );
		param_set_pt(		h, _b_grab_alpha_ui );
		get_param(h)->set_comment( "ON should have no effetcs" );
		param_set_pt(		h, _b_mipmap_generate_ui );
		param_set_pt(		h, _b_smooth_ui );

	++h;	//Grab to img
		param_set_pt(		h, _b_grab_to_img_ui );
		param_set_pt(		h, _b_grab_to_img_avoid_gpu_transfer_ui );
		param_set_pt(		h, _grab_to_img_size_x );
		param_set_pt(		h, _grab_to_img_size_y );

	++h;	//Geo
		param_set_pt(		h, _b_draw_ui );
		param_set_pt_3(	h, _center );
		param_set_pt_3(	h, _draw_nb );
		param_set_pt(		h, _draw_space );
		param_set_pt_3(	h, _rotation );
		param_set_pt_4(	h, _scale_ui );

	++h;	//Seed
		param_set_pt(		h, _seed );
		param_set_pt(		h, _seed_depth_ui );
	//	param_set_pt(		h, b_seed_smooth );
		param_set_pt(		h, _seed_width );
		param_set_pt_rgbfa(	h, _seed_color );

	err_param_init_pt( h );
}

void c_bdd_feedback::update()
{
	//todo	if( b_grab || b_draw )
	{
		scale_v3( _scale, _scale_ui, _scale_ui[3] );

		c_viewport*	viewport	=	c_viewport::get_cur();

		if( _size_x_ui == 0 )
			_size_x = POW2( _size_x_p2 );
		else if( _size_x_ui == -1 )
			_size_x = viewport->get_sx();
		else if( _size_x_ui == -2 )
			_size_x = viewport->get_sy();
		else
			_size_x = _size_x_ui;

		if( _size_y_ui == 0 )
			_size_y = POW2( _size_y_p2 );
		else if( _size_y_ui == -1 )
			_size_y = viewport->get_sx();
		else if( _size_y_ui == -2 )
			_size_y = viewport->get_sy();
		else
			_size_y = _size_y_ui;
	}
 }

namespace {
	//	used to optimize the feedback grab
	static	INT32	tex_w_grabed_def = -1;
	static	INT32	tex_h_grabed_def = -1;
	static	bool	b_tex_alpha_grabed_def = false;
}

void c_bdd_feedback::grab()
{
	c_img_2d*	feed_img = nullptr;	//hack the image is only in the GPU deal with that
	INT32	tex_w_grabed;
	INT32	tex_h_grabed;
	bool	b_tex_alpha_grabed;
	bool	b_sub;

	grab_count = 0;	//prepare for valid feedback on next frame
	if( ++_strobe_count < _strobe )
		return;
	_strobe_count = 0;

	tex_w_grabed = 0;
	tex_h_grabed = 0;
//	GOL::push_attrib( GL_ALL_ATTRIB_BITS );
//	GOL::push_attrib( GL_TEXTURE_BIT );
	tex_2d_bind_no_gpu_move( _s_grab_dst );
		
//todo	move to something like feed_img = bind_img::cur_get_img( w, h, channel_nb, b_texture_size_min_, texture_rgba_def_, __FUNCTION__ );
//		but don't work with -1 texture : no img
	if( _s_grab_dst >= 0 )
	{
		feed_img = g_bind_img_2d->get_always( _s_grab_dst );

		if( feed_img )
		{
			tex_w_grabed = feed_img->get_size_x();
			tex_h_grabed = feed_img->get_size_y();
			b_tex_alpha_grabed = feed_img->has_alpha();
			feed_img->set_gpu_in_sync( _s_grab_dst );
		}
		else
			ERR_PRINT_STRING( "%s can't allocate image", __FUNCTION__ );
	}
	else
	{
		tex_w_grabed = tex_w_grabed_def;
		tex_h_grabed = tex_h_grabed_def;
		b_tex_alpha_grabed = b_tex_alpha_grabed_def;
	}
	//todo	deal with overlap
	c_viewport*	viewport	=	c_viewport::get_cur();
	GLsizei grab_x = viewport->get_x() + interpolate( 0, viewport->get_sx()-_size_x, _grab_pos_x * REAL(.5) + REAL(.5) );
	GLsizei grab_y = viewport->get_y() + interpolate( 0, viewport->get_sy()-_size_y, _grab_pos_y * REAL(.5) + REAL(.5) );
		
	//hack
//			c_color::cur->pixel_transfert_update();

	//todo done below : which one is necessary
	if( GOL::is_anisotropic_filtering() )
		GOL::enable_anisotropic_filtering_2d();
/*		if( b_verbose_grab_ )
			VERBOSE_PRINTF( "BDD_FEEDBACK : grabbed %d x %d / size %d x %d", tex_w_grabed, tex_h_grabed, _size_x, _size_y );
*/
	INT32	w,h;
	GOL::adjust_tex_size_to_valid( w, h, _size_x, _size_y );

	if( 	tex_w_grabed >= w
		&&	tex_h_grabed >= h
		&&	b_tex_alpha_grabed == _b_grab_alpha_ui
		 )
	{
		if( tex_w_grabed != w || tex_h_grabed != h )
			b_sub = false;
		else
			b_sub = true;
	}
	else
		b_sub = false;

	TBUF_ADD_OBJ( tbuf::CH_GPU_READ, 1., "read_from_gpu", this );

	if( b_sub )
	{
		//todoq regroup with tex
		//todoq deal with swizzle
		GOL::tex_copy_sub_image_2d( 0,	0,0,	grab_x,grab_y, _size_x,_size_y	);
	}
	else
	{
		if( _b_verbose_grab )
		{
			VERBOSE_PRINT_STRING( "BDD_FEEDBACK : grab texture redefine size" );
			VERBOSE_PRINT_STRING( "BDD_FEEDBACK : w x h %d x %d", w,h );
		}

		//todo refine by picking what we want as type in params
		GOL::INTERNAL_TYPE	internal_type	= GOL::INTERNAL_TYPE::UINT_8;
		GLenum internal_format = GOL::make_internal_format_gl( _b_grab_alpha_ui ? 4 : 3, internal_type );

		//todoq regroup with tex
		//todoq deal with swizzle
		GOL::tex_copy_image_2d( 0, internal_format,	grab_x,grab_y, w,h, 0 );

		if( feed_img )
		{
			//hack we should use the format of where we draw
			feed_img->set_size_format( w,h, _b_grab_alpha_ui ? aaa::PIXEL_FORMAT::RGBA_8 : aaa::PIXEL_FORMAT::RGB_8 );
		}
		else
		{
			tex_w_grabed_def = w;
			tex_h_grabed_def = h;
			b_tex_alpha_grabed_def = _b_grab_alpha_ui;
		}

		tex_w_grabed = w;
		tex_h_grabed = h;

		//hack	complete it with format, type, internal_format, size
		//todo move glCopyTexImage2D to tex infact
		tex2d.store( _size_x, _size_y, internal_format, internal_type, 0 );
	}
	if( _b_mipmap_generate_ui )
		GOL::generate_mipmap_2d();

	TBUF_ADD_OBJ( tbuf::CH_GPU_READ, 0., nullptr, this );
	_tex_size_u = REAL( _size_x ) / REAL( tex_w_grabed );
	_tex_size_v = REAL( _size_y ) / REAL( tex_h_grabed );

	if( GOL::is_anisotropic_filtering() )
		GOL::enable_anisotropic_filtering_2d();

//			GOL:finish();
//hack
//			c_color::cur->pixel_transfert_reset();

	//hack we should extend with mipmap generation
//	if( b_smooth != b_smooth_ui )	
	{
//		_b_smooth = _b_smooth_ui;
		INT32 mode = _b_smooth_ui ? GL_LINEAR : GL_NEAREST ;
		GOL::set_tex_2d_min_mag( mode, mode );
	}
}

static	c_rand_lin	feedback_rand;

void c_bdd_feedback::draw_squares( REAL* pos_in, REAL* rot, REAL* size )
{
	//INT32	i_x, i_y, i_z;
	REAL	pos[3];
	REAL	dx, dy, dz;
	REAL	size_x;
	REAL	size_y;
	REAL	size_z;
	FP32 CONST *	uv = c_map::get_quad_uv();
	
	size_x = size[0];
	size_y = size[1];
	size_z = size[2];

	GOL::matrix::translate3v( pos_in );

	dx = _draw_space;
	dy = _draw_space;
	dz = _draw_space;

	pos[0] = -dx * ( _draw_nb[0] - 1 ) * REAL(.5);
	for( INT32 i_x = _draw_nb[0]; i_x > 0; --i_x )
	{
		pos[1] = -dy * ( _draw_nb[1] - 1 ) * REAL(.5);
		for( INT32 i_y = _draw_nb[1]; i_y > 0; --i_y )
		{
			pos[2] = -dz * ( _draw_nb[2] - 1 ) * REAL(.5);
			for( INT32 i_z = _draw_nb[2]; i_z > 0; --i_z )
			{
				// draw one feedback square
				GOL::matrix::push();
					GOL::matrix::translate3v( pos );
					GOL::matrix::rotatev( rot );
					GOL::begin( GL_TRIANGLE_STRIP );
						GOL::texcoord2v( uv );
						GOL::vertex3( -size_x, -size_y, 0 );
						GOL::texcoord2v( uv + 2 );
						GOL::vertex3( size_x, -size_y, 0 );
						GOL::texcoord2v( uv + 4 );
						GOL::vertex3( -size_x, size_y, 0 );
						GOL::texcoord2v( uv + 6 );
						GOL::vertex3( size_x, size_y, 0 );
					GOL::end();
				GOL::matrix::pop();
				pos[2] += dz;
			}
			pos[1] += dy;
		}
		pos[0] += dx;
	}

}

void c_bdd_feedback::draw_low()
{
	if( is_grab_ready() )
	{
		GOL::matrix::push();

			REAL	pos[3];
			cpy_v3( pos, _center );

			REAL	sca[3];
			sca[0] = _scale[0];
			sca[1] = _scale[1];
			sca[2] = 1.;

			draw_squares( pos, _rotation, sca );

	/*
			INT32	i;
			for( i=pass_nb; i>0; --i )
			{
				if ( b_blur )
				{
					pos[0] = feedback_rand.get_fp32();	//REAL( _size_x ),
					pos[1] = feedback_rand.get_fp32();	//REAL( _size_y ),
				}
	*/
	//		}
		GOL::matrix::pop();
	}
	set_grab_needed();
}


void	draw_quad_fn( REAL sx, REAL sy, REAL z )
{
	GOL::begin( c_render::get_cur()->get_draw_primitive() );
		if( c_layer::get_cur()->is_normal_draw() )
		{
			if( sx * sy >= 0 )
				GOL::normal3v( unit_z_v4fp32 );
			else
				GOL::normal3v( unit_z_neg_v4fp32 );
		}
		if ( c_layer::get_cur()->is_need_uv() )
		{
			GOL::texcoord2v(	unit_y_v4fp32	);
			GOL::vertex3(	-sx	,	 sy	,	z	);
			GOL::texcoord2v(	zero_v4fp32		);
			GOL::vertex3(	-sx	,	-sy	,	z	);
			GOL::texcoord2v(	 one_v4fp32		);
			GOL::vertex3(	 sx	,	 sy	,	z	);
			GOL::texcoord2v(	 unit_x_v4fp32	);
			GOL::vertex3(	 sx	,	-sy	,	z	);
		}
		else
		{
			GOL::vertex3(	-sx	,	 sy	,	z	);
			GOL::vertex3(	-sx	,	-sy	,	z	);
			GOL::vertex3(	 sx	,	 sy	,	z	);
			GOL::vertex3(	 sx	,	-sy	,	z	);
		}
	GOL::end();
}

void c_bdd_feedback::draw_seed()
{
	GOL::push_att();	// GL_CURRENT_BIT | GL_ENABLE_BIT );
		GOL::push_color4( 	_seed_color[0] * _seed_color[4],
							_seed_color[1] * _seed_color[4],
							_seed_color[2] * _seed_color[4],
							_seed_color[3] );
		GOL::matrix::push();
			GOL::matrix::load_identity();
		
			if( _seed > 31 )
			{
				REAL radius = _seed_width * REAL(128);	
				if( _seed > 16 )
				{		
					GOL::push_texture_dim(2);
					// hack, does not work if bank img is not 32
					tex_2d_bind( _seed - 31 ) ;
					draw_quad_fn( radius, radius, _seed_depth_ui );
				}
			}
			else
			{
				REAL sx = REAL(_size_x);
				REAL sy = REAL(_size_y);
				GOL::begin( GL_LINES );
					
					if( _seed & 1 )
					{
						GOL::vertex3(	-sx,	-sy,	_seed_depth_ui	);
						GOL::vertex3(	 sx,	-sy,	_seed_depth_ui	);
					}
					if( _seed & 2 )
					{
						GOL::vertex3(	 sx,	-sy,	_seed_depth_ui	);
						GOL::vertex3(	 sx,	 sy,	_seed_depth_ui	);
					}
					if( _seed & 4 )
					{
						GOL::vertex3(	 sx,	 sy,	_seed_depth_ui	);
						GOL::vertex3(	-sx,     sy,	_seed_depth_ui	);
					}
					if( _seed & 8 )
					{
						GOL::vertex3(	-sx,	 sy,	_seed_depth_ui	);
						GOL::vertex3(	-sx,	-sy,	_seed_depth_ui	);
					}
				GOL::end();
				if ( _seed & 16 )
				{
					REAL	radius = _seed_width * REAL(128);
					GOL::begin( GL_TRIANGLE_FAN );
						for( REAL angle = 0.; angle < 1.; angle += REAL(.04) )
						{
							GOL::vertex3( COS_TURN(angle) * radius, SIN_TURN(angle) * radius, _seed_depth_ui );
						}
					GOL::end();
				}
			}
		GOL::matrix::pop();
	GOL::pop_att();
}

c_img_2d*	img_for_sound;

namespace {
	static	bool	b_force_grab = true;
};

void	c_bdd_feedback::draw()
{
	if( _b_grab_ui && ( b_grab_needed || b_force_grab ) )
		grab();
	if( _b_grab_to_img_ui )
	{
		INT32	index = _s_grab_dst;
		if( index <= 0 )	//todo this a quick hack
		{
			index = 31;
		}

		INT32	grab_size_x;
		INT32	grab_size_y;

		c_viewport*	viewport	=	c_viewport::get_cur();

		if( _grab_to_img_size_x == 0 )
			grab_size_x = 1;
		else if( _grab_to_img_size_x == -1 )
			grab_size_x = viewport->get_sx();
		else if( _grab_to_img_size_x == -2 )
			grab_size_x = viewport->get_sy();
		else
			grab_size_x = _grab_to_img_size_x;

		if( _grab_to_img_size_y == 0 )
			grab_size_y = 1;
		else if( _grab_to_img_size_y == -1 )
			grab_size_y = viewport->get_sx();
		else if( _grab_to_img_size_y == -2 )
			grab_size_y = viewport->get_sy();
		else
			grab_size_y = _grab_to_img_size_y;

		c_img_2d* p_img = g_bind_img_2d->get_with_image_size( index, grab_size_x, grab_size_y, aaa::PIXEL_FORMAT::RGBA_8 );
		if( p_img )
		{
			//p_img->lock();
			INT32	src = 0;
			c_fbo*	fbo = c_fbo::get_cur();
			if( fbo )
			{
				//	fbo->generate_mipmap(); //not need for the next read_pixels
				src = fbo->get_color_attachment_first();
			}
			
			p_img->read_pixels(	viewport->get_x() + ( viewport->get_sx() - grab_size_x ) / 2,
								viewport->get_y() + ( viewport->get_sy() - grab_size_y ) / 2,
								grab_size_x, grab_size_y, src );

			//p_img->unlock();
			img_for_sound = p_img;
			if( _b_grab_to_img_avoid_gpu_transfer_ui )		
			{	//	to avoid a later transfert to texture
				p_img->set_gpu_in_sync( index );
			}
		}
		else
		{
			img_for_sound = nullptr;
		}
	}
	if( _b_draw_ui )
		draw_low();
	if( _seed != 0 )
		draw_seed();
}
