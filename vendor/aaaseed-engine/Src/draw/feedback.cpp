#include "draw/feedback.h"

#if 0	//unused for now

#include "math/rand.h"
#include "draw/seedcam.h"
#include "time/ourtime.h"
#include "draw/color.h"
#include "draw/render.h"
#include "draw/map.h"
#include "draw/face.h"
#include "draw/bind_img.h"
#ifndef	AAA_STR_SYMBO_H
#	include "ui/strsymbo.h"
#endif
#include "math/aaa_math.h"
#include "math/v_base.h"
#include "infrastructure/param/param.h"
#include "infrastructure/viewport.h"
#include "image/img.h"
#include "draw/bind_img.h"

extern	INT32	blend_array[];
extern	CHAR*	blend_str[];
extern	char*	blend_tex_env_mode_str[6];

//c_feedback*	c_feedback::cur = NULL;
//c_feedback*	c_feedback::ui = NULL;

//	this and the three next fn are used to grab only when needed


FACTORY_CREATE_V1( c_feedback, feedback, Feedback, feed );

void c_feedback::init()
{
	strobe_count_ = 0;
	size_x_grabed_ = -1;
	size_y_grabed_ = -1;
}

void c_feedback::update()
{
	if( b_active_ )
		{
		scale_v3r( scale_, scale_ui_, scale_factor_ );

		size_x_ = POW2( size_x_p2_ );
		size_y_ = POW2( size_y_p2_ );

		if( c_render::cur->get_depth() )
			{
			draw_depth_ = -c_seedcam::cur->convert_depth_ui_to_depth( draw_depth_ui_ );
			seed_depth_ = -c_seedcam::cur->convert_depth_ui_to_depth( seed_depth_ui_ );
			}
		c_feedback::cur = this;
		}
//	PRINT_STRING( "draw %f->%f seed_ %f->%f\n",
//			(FLOAT)draw_depth_ui_, (FLOAT)draw_depth_,
//			(FLOAT)seed_depth_ui_, (FLOAT)seed_depth_
//			);

//	draw_depth_ = draw_depth_ui_;
//	seed_depth_ = seed_depth_ui_;
}

namespace	n_feeback
{
	static	const INT32	BASE_PARAM_NB	=	1;
	static	const INT32	GRAB_PARAM_NB	=	10;
	static	const INT32	GEO_PARAM_NB	=	13;
	static	const INT32	RENDER_PARAM_NB	=	10;
	static	const INT32	SEED_PARAM_NB	=	9;
	static	const INT32	GROUP_PARAM_NB	=	4;
	static	const INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
										+	GRAB_PARAM_NB
										+	GEO_PARAM_NB
										+	RENDER_PARAM_NB
										+	SEED_PARAM_NB
										+	GROUP_PARAM_NB;

	static	ST_PARAM	param[PARAM_NB_MAX] =
	{
		ST_PARAM_BOOL_OFF( active )
		ST_PARAM_GROUP( Grabbing, GRAB_PARAM_NB )
			ST_PARAM_BOOL_OFF( grab )
		{	NULL,	PARAM_UINT32_SYMBO_NEG,	"bind_2d_dst",	0, -1,	-1, PARAM_INFINI,	NULL, gstr::default },
			{	NULL,	PARAM_INT32,	"strobe",				0., 1.,	1., 100.,			NULL, NULL },
			{	NULL,	PARAM_SYMBOLIC,	"size_x_p2",			9., 8.,	1., 16,				NULL, gstr::p2 },//,c_feedback::update, NULL },
			{	NULL,	PARAM_SYMBOLIC,	"size_y_p2",			9., 8.,	1., 16,				NULL, gstr::p2 },//clean feedback_update, NULL },
			ST_PARAM_REAL_ZERO( pos_x )
			ST_PARAM_REAL_ZERO( pos_y )
			ST_PARAM_BOOL_ON( grab_alpha )
			ST_PARAM_BOOL_ON( smooth )
			ST_PARAM_BOOLC_OFF( generate_mipmap )

		ST_PARAM_GROUP( Geo, GEO_PARAM_NB )
			ST_PARAM_BOOL_OFF( draw )
			{	NULL,	PARAM_INT32,	"draw_nb_x",		3., 1.,		1., 128.,	NULL, NULL },
			{	NULL,	PARAM_INT32,	"draw_nb_y",		3., 1.,		1., 128.,	NULL, NULL },
			{	NULL,	PARAM_REAL,		"draw_space",		1., 1.,		0., 128.,		NULL, NULL },
			ST_PARAM_REAL_ZERO( x )
			ST_PARAM_REAL_ZERO( y )
			ST_PARAM_REAL_ONE( depth )
			{	NULL,	PARAM_REAL,		"angle",			.5, 0.,		-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,		"angle_bis",		.5, 0.,		-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
			ST_PARAM_SCALE_XYZ_FACTOR( scale )

			ST_PARAM_GROUP( Render, RENDER_PARAM_NB )
			{	NULL,	PARAM_SYMBOLIC,	"tex_env_mode",		1, 0,		0, 3,		NULL, blend_tex_env_mode_str },
	//		ST_PARAM_BOOL_OFF( feedback_random_x )
	//		ST_PARAM_BOOL_OFF( feedback_random_y )
			ST_PARAM_BOOL_OFF( blur )
//			ST_PARAM_BOOL_OFF( blur )
			{	NULL,	PARAM_INT32,	"pass_nb",			2, 1,		1, 16,		NULL, NULL },
			
			{	NULL,	PARAM_SYMBOLIC,	"blend_src",		6, 4,		0, 8,		NULL, blend_str },
			{	NULL,	PARAM_SYMBOLIC,	"blend_dst",		7, 5,		0, 7,		NULL, blend_str },

			ST_PARAM_REAL_ONE( red )
			ST_PARAM_REAL_ONE( green )
			ST_PARAM_REAL_ONE( blue )
			ST_PARAM_REAL_ONE( grey )
			ST_PARAM_REAL_ONE( alpha )
		
		ST_PARAM_GROUP( Seed, SEED_PARAM_NB )
	//todo	this 256 is a hack
			{	NULL,	PARAM_INT32,	"seed_type",	16, 0,		0, 31+256,		NULL, NULL },
			ST_PARAM_REAL_ONE( seed_depth )
			ST_PARAM_BOOL_OFF( seed_smooth )
			{	NULL,	PARAM_REAL,		"seed_width",	1, .1,		0, PARAM_INFINI,		NULL, NULL },

			ST_PARAM_COLOR_RGBGA( seed )
	};
}

CONSTRUCTOR_CREATE(c_feedback)
{
	param_init_with( n_feeback::param, n_feeback::PARAM_NB_MAX );
	init();
}
EMPTY_DESTRUCTOR(c_feedback)

void c_feedback::param_init_pt()
{
INT32	h = 0;

	param_set_pt( h, b_active_ );
	h++;//Grab
		param_set_pt( h, b_grab_ );
		param_set_pt( h, s_grab_dst_ );
		param_set_pt( h, strobe_ );
		param_set_pt( h, size_x_p2_ );
		param_set_pt( h, size_y_p2_ );
		param_set_pt( h, grab_pos_x_ );
		param_set_pt( h, grab_pos_y_ );
		param_set_pt( h, b_grab_alpha_ );
		param_set_pt( h, b_smooth_ui_ );
		param_set_pt( h, b_generate_mipmap_ );

	h++;//Geo
		param_set_pt( h, b_draw_ );
		param_set_pt( h, draw_nb_x_ );
		param_set_pt( h, draw_nb_y_ );
		param_set_pt( h, draw_space_ );
		param_set_pt( h, x_ );
		param_set_pt( h, y_ );
		param_set_pt( h, draw_depth_ui_ );
		param_set_pt( h, angle_ );
		param_set_pt( h, angle_bis_ );
		param_set_pt_v3( h, scale_ui_ );
		param_set_pt( h, scale_factor_ );

	h++;//Render

		param_set_pt( h, tex_env_mode_ );
		param_set_pt( h, b_blur_ );
		param_set_pt( h, pass_nb_ );

		param_set_pt( h, blend_src_ );
		param_set_pt( h, blend_dst_ );

	param_set_pt( h, red_ );
	param_set_pt( h, green_ );
	param_set_pt( h, blue_ );
	param_set_pt( h, grey_ );
	param_set_pt( h, alpha_ );

	h++;//Seed
		param_set_pt( h, seed_ );
		param_set_pt( h, seed_depth_ui_ );
		param_set_pt( h, b_seed_smooth_ );
		param_set_pt( h, seed_width_ );
		param_set_pt_v5( h, seed_color_ );

	err_param_init_pt(h);
}

AAA_ERR	c_feedback::load_do_after( CONST CHAR* CONST filename)
{
	b_smooth_ = !b_smooth_ui_;
	return AAA_OK;
}


//todoq	all that should be organized and accessible
//extern	REAL	x_left_video;
//extern	REAL	y_bottom_video;
//extern	REAL	x_size_video;
//extern	REAL	y_size_video;

BOOL	b_force_grab = TRUE;

void c_feedback::grab()
{
REAL		grab_x;
REAL		grab_y;
c_img*		feed_img;

	feed_img = NULL;	//todoqqq the image is only in the GPU deal with that
//	if( b_active && b_grab_ && ++strobe_count_ >= strobe_ )
	if( b_active_ && b_grab_ && (b_feedback_grab_needed || b_force_grab) )
		{
		feedback_grab_count = 0;	//prepare for valid feedback on next frame
		if( ++strobe_count_ >= strobe_ )
			{
			strobe_count_ = 0;
//			PRINT_STRING("g");

			tex_2d_switch( s_grab_dst_ );
			
			if( s_grab_dst_>= 0)
				{
				feed_img = bind_img::get( s_grab_dst_ );
				if( feed_img )
					{
					size_x_grabed_ = feed_img->get_size_x();
					size_y_grabed_ = feed_img->get_size_y();
					}
				else
					{
					size_x_grabed_ = 0;
					size_y_grabed_ = 0;
					}
				}

//			grab_x = x_left_video + interpolate( 0, (x_size_video-size_x_ ), grab_pos_x_*.5 +.5);
//			grab_y = y_bottom_video + (y_size_video-size_y_ )/2;
			grab_x = viewport_cur->get_x() + interpolate( 0, viewport_cur->get_w()-size_x_,		grab_pos_x_*.5 +.5);
			grab_y = viewport_cur->get_y() + interpolate( 0, (viewport_cur->get_h()-size_y_ )/2,	grab_pos_y_*.5 +.5);
			

//hack ?
//			c_color::cur->pixel_transfert_update();
			if( b_generate_mipmap_ )
				GOL::enable_generate_mipmap();
			GOL::enable_anisotropic_filtering();
			if(	size_x_grabed_ == size_x_ &&	size_y_grabed_ == size_y_ && b_alpha_grabed_ == b_grab_alpha_ )
				{
				glCopyTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, grab_x, grab_y, size_x_, size_y_ );
				}
			else
				{
				glCopyTexImage2D( GL_TEXTURE_2D, 0, b_grab_alpha_ ? GL_RGBA : GL_RGB, grab_x, grab_y, size_x_, size_y_, 0);
				size_x_grabed_	= size_x_;
				size_y_grabed_	= size_y_;
				b_alpha_grabed_	= b_grab_alpha_;
				if( feed_img  )
					{
					feed_img->size_x_ = size_x_;
					feed_img->size_y_ = size_y_;
					}
				}
			if( b_generate_mipmap_ )
				GOL::disable_generate_mipmap();
//			glFinish();
//hack ?
//			c_color::cur->pixel_transfert_reset();
			if( b_smooth_ != b_smooth_ui_ )	//todoqqq we should extend with mipmap generation
				{
				b_smooth_ = b_smooth_ui_;
				if( b_smooth_ )
					{
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					}
				else
					{
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
					}
				}
			}
		}
}

static	c_rand_lin	feedback_rand;

void c_feedback::draw_square( REAL rot, REAL rot_bis, REAL size_x_, REAL size_y_, REAL size_z)
{
REAL	px, py;
REAL	dx, dy;

	glPolygonMode( GL_FRONT, GL_FILL);
	dx = size_x_ * draw_space_ * 2.;
	dy = size_y_ * draw_space_ * 2.;
	px = -dx * (draw_nb_x_-1) * .5;

	GOL::rotate_z(  rot);
	rot_bis *= 360.;
	for( INT32 nbx = draw_nb_x_; nbx > 0; nbx-- )
		{
		py = -dy * (draw_nb_y_-1) * .5;
		for( INT32 nby = draw_nb_y_; nby > 0; nby-- )
			{
			// draw one feedback square
			GOL::push_matrix();
				GOL::translate( px, py, 0. );
				GOL::rotate_z(  rot_bis);
				GOL::begin(GL_TRIANGLE_STRIP);
					GOL::texcoord2v( zero_v3f32		);	GOL::vertex3( -size_x_, -size_y_, draw_depth_ );
					GOL::texcoord2v( unit_x_v3f32	);	GOL::vertex3( size_x_, -size_y_, draw_depth_ );
					GOL::texcoord2v( unit_y_v3f32	);	GOL::vertex3( -size_x_, size_y_, draw_depth_ );
					GOL::texcoord2v( one_v3f32		);	GOL::vertex3( size_x_, size_y_, draw_depth_ );
				GOL::end();
			GOL::pop_matrix();
			py += dy;
			}
		px += dx;
		}

}

void c_feedback::draw()
{
	if( b_active_ )
		{
		tex_2d_switch( s_grab_dst_ );

		GOL::push_attrib( GL_ALL_ATTRIB_BITS);
		GOL::matrix_mode(GL_PROJECTION);
		GOL::push_matrix();
		GOL::matrix_mode(GL_MODELVIEW);
		GOL::push_matrix();

			glDisable( GL_LIGHTING);

			if( b_draw_ || seed_ != 0 )
				{
				GOL::matrix_mode(GL_PROJECTION);
				GOL::load_identity();
				glOrtho(	-viewport_cur->get_w(),	viewport_cur->get_w(),
							-viewport_cur->get_h(),	viewport_cur->get_h(),
							0.0, 1.0);
//				glOrtho(	-x_size_video,	x_size_video,
//							-y_size_video,	y_size_video,
//							0.0, 1.0);
				viewport_cur->set();
				//GOL::viewport( x_left_video, y_bottom_video, x_size_video, y_size_video);
				GOL::matrix_mode(GL_MODELVIEW);

					GOL::color4(	red_ * grey_,
							green_ * grey_,
							blue_ * grey_,
							alpha_ );
				glEnable (GL_BLEND);
				glBlendFunc( blend_array[blend_src_], blend_array[blend_dst_]);
				}
			if( c_render::cur->get_depth() )
				{
				glEnable( GL_DEPTH_TEST);
				glDepthFunc( GL_LESS);	//todoq change the test method in ui
				}
			else
				glDisable( GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
			if(  b_draw_ && ABS(scale_factor_ )>.01 )
				{
				if( feedback_is_grab_ready() )
					{
					GOL::set_tex_env_mode( tex_env_mode_ );

					glDisable( GL_LIGHTING);
					
					glEnable(GL_TEXTURE_2D);
					
					for( INT32 i = pass_nb_; i > 0; i-- )
						{
						GOL::load_identity();
						if( b_blur_ )
							{
							GOL::translate(	x_*feedback_rand.get_float(),	//REAL(size_x_ ),
											y_*feedback_rand.get_float(),	//REAL(size_y_ ),
											0.0);
							}
						else
							GOL::translate( x_*viewport_cur->get_w(), y_*viewport_cur->get_h(), 0.0);
//							GOL::translate( x_*x_size_video, y_*y_size_video, 0.0);
				//		GOL::rotate_y( dummy_float_[0]);
				//		GOL::rotate_x( dummy_float_[1]);
						draw_square( angle_, angle_bis_,
											scale_[0]*size_x_,
											scale_[1]*size_y_,
											scale_[2]);
						}
					}
				feedback_set_grab_needed();
				}
			// draw square seed_
			if( seed_ != 0 && ABS(seed_width_ )>.01 )
				{
				GOL::load_identity();
				glDisable(GL_TEXTURE_2D);
				if( b_seed_smooth_ )
					GOL::enable_line_smooth();
				else
					GOL::line_width( seed_width_*128. );
			
				GOL::color4(	seed_color_[0] * seed_color_[4],
							seed_color_[1] * seed_color_[4],
							seed_color_[2] * seed_color_[4],
							seed_color_[3]);
				if( seed_ > 31 )
					{
					REAL	radius;

					radius = seed_width_ * 128.;
					if( seed_ > 16)
						{		
						glEnable( GL_TEXTURE_2D);
						tex_2d_switch( seed_ -31) ;
						draw_quad_fn( radius, radius, seed_depth_ );
						glDisable( GL_TEXTURE_2D);
						}
					}
				else
					{
					GOL::begin(GL_LINES);
						if( seed_ & 1 )
							{
							GOL::vertex3(-size_x_, -size_y_, seed_depth_ );
							GOL::vertex3(size_x_, -size_y_, seed_depth_ );
							}
						if( seed_ & 2 )
							{
							GOL::vertex3(size_x_, -size_y_, seed_depth_ );
							GOL::vertex3(size_x_, size_y_, seed_depth_ );
							}
						if( seed_ & 4 )
							{
							GOL::vertex3(size_x_, size_y_, seed_depth_ );
							GOL::vertex3(-size_x_, size_y_, seed_depth_ );
							}
						if( seed_ & 8 )
							{
							GOL::vertex3(-size_x_, size_y_, seed_depth_ );
							GOL::vertex3(-size_x_, -size_y_, seed_depth_ );
							}
					GOL::end();
					if( seed_ & 16 )
						{
						REAL	radius;

						radius = seed_width_*128.;
						GOL::begin(GL_POLYGON);
						for( REAL angle = 0.; angle < 1.; angle += .04 )
						{
							GOL::vertex3( COS_INT( angle ) * radius, SIN_INT( angle ) * radius, seed_depth_ );
						}
						GOL::end();
						}
					}
				}
		//todoq deal with multilayers
		GOL::matrix_mode(GL_PROJECTION);
		GOL::pop_matrix();
		GOL::matrix_mode(GL_MODELVIEW);
		GOL::pop_matrix();
		GOL::pop_attrib();
		}
}

#endif  // 0
