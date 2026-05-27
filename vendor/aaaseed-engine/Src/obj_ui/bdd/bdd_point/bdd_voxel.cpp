#include "bdd_voxel.h"
#include "draw/color.h"
#include "image/bind_img_2d.h"
#include "draw/box.h"
#include "draw/geo/sphere.h"
#include "math/noisturb.h"
#include "image/convert/color_space.h"


//
//	COLOR_MAP
//
void	c_color_map::set_coor(	REAL ou, REAL ov, REAL fu, REAL fv )
{
	set_v2( _off, ou, ov );
	set_v2( _fac, fu, fv );
}
void	c_color_map::init()
{
	set_img(	nullptr );
	set_coor(	0, 0, 1, 1 );
}
c_color_map::c_color_map()
{
	init();
}
void	c_color_map::set_img(	c_img_2d* img )
{
	_img = img;
}
FINLINE	void	c_color_map::get_color_u(	FP32* color, REAL u )			{	_img->get_color4r_from_uv_linear( color, _off[0] + u*_fac[0], _off[1],				false );	}
FINLINE	void	c_color_map::get_color_uv(	FP32* color, REAL u, REAL v )	{	_img->get_color4r_from_uv_linear( color, _off[0] + u*_fac[0], _off[1] + v*_fac[1],  false );	}

//
//	BDD_VOXEL
//
static	INT32 CONST RENDER_TARGET_NB	=	6;
struct st_vox 
{
	INT32	line_nb;
	FP32	pos[3];		//	3d position
	INT32	pixel[2];	//	2d position in image	REAL	color[4];
	bool	b_active;
	bool	b_force;
	FP32	color[RENDER_TARGET_NB][4];
};

static	FINLINE	void	make_valid_buffer_target( INT32& target )
{
	if( target < 0 )
		target = 0;
	else if( target >= RENDER_TARGET_NB )
		target = RENDER_TARGET_NB-1;
}
static	FINLINE	void	make_valid_buffer_target( INT32& a, INT32& b )
{
	make_valid_buffer_target( a );
	make_valid_buffer_target( b );
}
static	FINLINE	void	make_valid_buffer_target( INT32& a, INT32& b, INT32& c )
{
	make_valid_buffer_target( a );
	make_valid_buffer_target( b );
	make_valid_buffer_target( c );
}

FACTORY_CREATE_PROP_V1( c_bdd_voxel, bdd_voxel, Voxel, voxel, sub_menu="Point"; );

enum BLEND_MODE : INT32
{
	BLEND_MODE_REPLACE = 0,
	BLEND_MODE_OVER,
	BLEND_MODE_ADD_ALPHA,
	BLEND_MODE_ADD,
	BLEND_MODE_MAX,
	BLEND_MODE_NB
};
static	C_PCHAR_C	blend_mode_str[] =
{
	"REPLACE",
	"OVER",
	"ADD_USING_ALPHA",
	"ADD",
	"MAX",
};

namespace n_bdd_voxel
	{


	CONSTEXPR INT32 BASE_PARAM_NB			= 12 + c_bdd::GEO_PARAM_NB;
	CONSTEXPR INT32 TRANSFO_PARAM_NB		= 7;
	CONSTEXPR INT32 RENDER_COLOR_PARAM_NB	= 10;
	CONSTEXPR INT32 IMG_PARAM_NB			= 4;
	CONSTEXPR INT32 OUT_PARAM_NB			= 6;
	CONSTEXPR INT32 BOX_PARAM_NB			= 13;
	CONSTEXPR INT32 GROUP_NB				= 4;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	TRANSFO_PARAM_NB
									+	RENDER_COLOR_PARAM_NB
									+	IMG_PARAM_NB
									+	OUT_PARAM_NB
									+	BOX_PARAM_NB
									+	GROUP_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_BASE_PARAMS

		PARAM_DEF_INT32_LOCKED(	point_nb	)
		PARAM_DEF_INT32_LOCKED(	line_nb		)

		PARAM_DEF_SYMBO_PSTR(	blend_mode,	BLEND_MODE_MAX, BLEND_MODE_ADD,		blend_mode_str )

		PARAM_DEF_COLOR_RGBGA_BLACK( color_base )

		PARAM_DEF_GROUP_CLOSED( Render Transfo, TRANSFO_PARAM_NB )
			PARAM_DEF_SCALE_XYZF(			scale		)	
			PARAM_DEF_POINT_XYZ(			offset		)

		PARAM_DEF_GROUP_CLOSED( Render Color, RENDER_COLOR_PARAM_NB )
			PARAM_DEF_COLOR_RGBGA_BLACK(	render_color_offset )
			PARAM_DEF_COLOR_RGBGA(			render_color_factor )

		PARAM_DEF_REAL_ONE(		led_size			)
		PARAM_DEF_BOOL_OFF(		led_as_sphere		)
		PARAM_DEF_INT32(		led_arc_segment_nb,	6, 3,	3, PARAM_MAX_INT32	)

//		PARAM_DEF_IMG_BIND(		bind_dst					)
		PARAM_DEF_BOOL_OFF(		image_compute				)		
		PARAM_DEF_BOOL_ON(		image_compute_use_alpha		)	
	//todo regroup with PARAM_DEF_IMG_SIZE_FORMAT ?
		PARAM_DEF_IMG_SIZE(		image_size_x,		5, 4	)
		PARAM_DEF_IMG_SIZE(		image_size_y,		5, 4	)
		PARAM_DEF_BOOL_OFF(		image_save_trig				)

		PARAM_DEF_GROUP_CLOSED( Out, OUT_PARAM_NB )
			PARAM_DEF_XYZ_LOCKED( min )
			PARAM_DEF_XYZ_LOCKED( max )

		PARAM_DEF_GROUP_CLOSED( Box, BOX_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		box_force	)		
			PARAM_DEF_BOOL_OFF(		box_disable	)		
			PARAM_DEF_POINT_XYZ(	box_center	)
			PARAM_DEF_SCALE_XYZ(	box_range	)
			PARAM_DEF_BOOL_OFF(		box_inverse	)
			PARAM_DEF_COLOR_RGBG(	force )

	};
}

void	c_bdd_voxel::param_init_pt()
{
	INT32	h = param_init_pt_geo();

	param_set_pt(		h,	_point_nb_used		);
	param_set_pt(		h,	_index_nb_used		);

	param_set_pt(		h,	_s_blend_mode		);

	param_set_pt_rgbfa(	h,	_color_base_ui		);

	++h;
		param_set_pt_4(	h,	_scale_ui			);
		param_set_pt_3(	h,	_offset_ui			);

	++h;
		param_set_pt_rgbfa(	h,	_color_offset_ui	);
		param_set_pt_rgbfa(	h,	_color_factor_ui	);

	param_set_pt(		h,	_led_size			);
	param_set_pt(		h,	_b_led_sphere_ui	);
	param_set_pt(		h,	_led_sphere_seg_nb	);

	param_set_pt(		h,	_b_img_compute_ui			);
	param_set_pt(		h,	_b_img_compute_use_alpha_ui	);
//	param_set_pt(		h, _bind_dst_ui					);
	param_set_pt_2(	h,	_pixel_nb_ui				);
//	param_set_pt(		h,	_bind_dst					);
	param_set_pt(		h,	_b_img_save_trig_ui			);

	++h;
		param_set_pt_3(	h,	 _min_out			);
		param_set_pt_3(	h,	 _max_out			);

	++h;
		param_set_pt(		h,	 _b_box_force_ui	);
		param_set_pt(		h,	 _b_box_disable_ui	);
		param_set_pt_3(	h,	 _box_center_ui		);
		param_set_pt_3(	h,	 _box_range_ui		);
		param_set_pt(		h,	 _b_box_inverse_ui	);
		param_set_pt_4(	h,	 _color_force_ui	);


	err_param_init_pt(h);
}

void	c_bdd_voxel::init()
{
	for( INT32 i=0;  i<COLOR_NB;	++i	)
	{
		clear_v4( _colors[i] );
		_colors_hd[i] = _colors[i];
	}
	clear_v4( _color_base_offset );
	clear_v4( _color_base_factor );
	_pixel_nb[0]	=	-42;
	_pixel_nb[1]	=	-42;
	erase_points();
	param_init_with( n_bdd_voxel::param, n_bdd_voxel::PARAM_NB_MAX );

	if( _img )
	{
		FP32 cyan[4] = { 0,.5,.5,1 };
		_img->fill_rgba( cyan );
	}

	for( INT32 i=0;  i<COLOR_MAP_NB;	++i	)
	{
		c_color_map* p = &_map[i];
		p->init();
		_p_map[i]	= p;
	}
}

bool	c_bdd_voxel::alloc_point( INT32 nb )
{
	if( nb <= _point_nb_allocated )
		return true;
	GOOD_PRINT_STRING( "alloc_point before %d", nb );
	nb = MAX( 256, nb*2 );
	_vox = (st_vox *) REALLOC_ALIGNED( _vox, nb*sizeof(st_vox) );
	if( !_vox )
	{
		err_print( "Can't alloc data" );
		return false;
	}
	_point_nb_allocated = nb;
	GOOD_PRINT_STRING( "alloc_point OK now %d", _point_nb_allocated );
	return true;
}
bool	c_bdd_voxel::alloc_index( INT32 nb )
{
	if( nb <= _index_nb_allocated )
		return true;
	GOOD_PRINT_STRING( "alloc_index before %d", nb );
	nb = MAX( 256, nb*2 );
	_index = (INT32 *) REALLOC_ALIGNED( _index, nb*sizeof(INT32) );
	if( !_index )
	{
		err_print( "Can't alloc index" );
		return false;
	}
	_index_nb_allocated = nb;
	GOOD_PRINT_STRING( "alloc_index OK now %d", _index_nb_allocated );
	return true;
}

void	c_bdd_voxel::dealloc()
{
	IF_FREE_ALIGNED_AND_NULL( _vox );
	IF_FREE_ALIGNED_AND_NULL( _index );
}

CONSTRUCTOR_CREATE(c_bdd_voxel)
,_vox(nullptr)
,_index(nullptr)
,_bind_last(-42)
,_img(nullptr)
,_point_nb_allocated(0)
,_index_nb_allocated(0)
,_render_target(0)
,_b_color_map(false)
{
	init();
}

c_bdd_voxel::~c_bdd_voxel()
{
	dealloc();
}

/*
void	c_bdd_voxel::restart()
{
	_b_restart_trig_ui = true;	
}
*/

c_img_2d*	c_bdd_voxel::get_img_valid()
{
	c_img_2d*	img = _img;
	return img->is_valid() ? img : nullptr ;
}
c_img_2d*	c_bdd_voxel::get_img()
{
	//	_bind_dst = bind_img_2d->build_index_from_param( _bind_dst_ui );
	bool	b_ask;
	if(	!_img )
		b_ask = true;
	else
	{
		b_ask = is_diff_v2( _pixel_nb,  _pixel_nb_ui )	||	_bind_last != g_bind_img_2d->get_cur_index();
		if( !b_ask )
		{
			INT32 sx, sy;
			g_bind_img_2d->is_size_cur( sx, sy );
			if( sx != _pixel_nb_ui[0] || sy != _pixel_nb_ui[1] )
				b_ask = true;
		}
	}
	if( b_ask )
	{
		//		INT32	s_bind;
		//		s_bind = c_layer::get_cur()->get_bind_2d();
		//		tex_2d_bind( s_bind );	//if there is no map 
		_img = g_bind_img_2d->get_img_cur( aaa::PIXEL_FORMAT::RGB_8, _pixel_nb_ui[0], _pixel_nb_ui[1], true, nullptr, __FUNCTION__ );
		if( _img->is_valid() )
		{	//todo we need a better strategy for owning img and tex
			_img->erase_filename();
			_img->set_cpu_keep( true );
			_bind_last = g_bind_img_2d->get_cur_index();
		}
	}
	return _img;
}

static	o_str	fname;

void	c_bdd_voxel::update()
{	
	get_img();
	if( _b_img_compute_ui )
		render_in_img( _render_target );

	if( _b_img_save_trig_ui && _img->is_valid() )
	{
		build_snap_filename( fname );
		_img->write( fname );
		_b_img_save_trig_ui = false;
	}
	scale_v3_cpy_v4( _color_base_offset, _color_base_ui );
	sub_v4( _color_base_factor,	FP32(1), _color_base_offset );
	_col_out[3] = _color_base_offset[3];

	scale_v3( _scale, _scale_ui, _scale_ui[3] );
	scale_v3_cpy_v4( _color_factor, _color_factor_ui );
	scale_v3_cpy_v4( _color_offset, _color_offset_ui );

	_b_box = _b_box_force_ui || _b_box_disable_ui;
	if( _b_box )
	{
		//todo rework to avoid conversions
		FP32 r[3];
		FP32 c_m_half[3];
		FP32 t[3];
		FP32 center[3];
		cpy_v3( center, _box_center_ui );
		FP32 range[3];
		cpy_v3( range, _box_range_ui );
		FP32	min_out[3];
		cpy_v3( min_out, _min_out );
		FP32	max_out[3];
		cpy_v3( max_out, _max_out );

		sub_v3( r, max_out, min_out );
		sub_v3( c_m_half, center, FP32(.5) );
		mul_add_v4( t, c_m_half, range, center );
		mul_add_v4( t, r, min_out );
		mul_v3( r, range );
		scale_v3( r, FP32(.5) );

		sub_v3( _box_min, t, r );
		add_v3( _box_max, t, r );
	}

	scale_v3( _color_force, _color_force_ui, _color_force_ui[3] );
	//	make sure alloc is ok
	//_nb_allocated_ui = MAX( _boids._nb_alive_max, _nb_allocated_ui );
//	if( !alloc_point( _point_nb_allocated_ui ) )
//		return;

	//	deal with restart
/*
	if( _delta_t.update() )
	{
		DBG_PRINT_STRING( "boid time restart" );
		restart();
	}
	if( _b_restart_trig_ui )
	{
#if	AAA_DEBUG()
		DBG_PRINT_STRING( "boid restart");
#endif
		_boids.restart();
		_b_restart_trig_ui = false;
	}

	//	need a comment here ?	
	REAL	dt;
	if( _b_real_time )
		dt = MIN( _delta_t.get_dt(), _time_interval_max );
	else
		dt = _time_interval;
	dt *= _time_factor;
*/
}

/*
void	c_bdd_voxel::draw_normal_point( REAL len )
{
	REAL		tmp_scale[3];
	REAL		vec[3];

	scale_v3( tmp_scale, _scale, len );

	GOL::begin( GL_LINES );
		c_boid**	hd = _boids._hd_boid-1;
		for( INT32 i = _nb_to_draw; i > 0; --i )
		{
			c_boid*	b = *++hd;
			GOL::vertex3v( b->get_pos_to_draw() );
			add_mul_v3r( vec, b->get_pos_to_draw(), b->get_speed(), tmp_scale );
			GOL::vertex3v( vec );
		}
	GOL::end();
}
*/

void	c_bdd_voxel::erase_points()
{
	_point_nb_used = 0;
	_index_nb_used = 0;
	clear_v3( _min_out );
	clear_v3( _max_out );
}

FINLINE FP32 CONST*	c_bdd_voxel::build_color_out( st_vox* vox )
{
	if( _b_box )
	{
		bool b_in = is_inside_min_max_v3( vox->pos, _box_min, _box_max ) ^ _b_box_inverse_ui;
		if( b_in )
			return _b_box_force_ui ? _color_force : GOL::fp32_black ;
	}
	if( vox->b_force )
		return _color_force;
	else if( !vox->b_active )
		return GOL::fp32_black;
	else if( _b_img_compute_use_alpha_ui )
	{
		FP32* col = vox->color[_render_target];
		scale_v3( _col_out, col, col[3] );
		return _col_out;
	}
	else
		return vox->color[_render_target];
}

void	c_bdd_voxel::render_in_img( INT32 target )
{
	c_img_2d*	img = get_img_valid();
	if( !img )
		return;
	img->fill_rgba( _color_img_clear );
	//mem::is_all_ok( __FUNCTION__ );
	st_vox*	vox = _vox;
	for( INT32 nb=_point_nb_used; nb>0;  )
	{
		img->set_xy_color3r( vox->pixel[0], vox->pixel[1], build_color_out( vox ) );
		++vox;
		--nb;
	}
	g_bind_img_2d->do_after_a_compute( true );
}

st_vox*	c_bdd_voxel::add_point( REAL* pos )
{
	st_vox* vox = _vox + _point_nb_used;
	++_point_nb_used;
	clear_vf( vox->color[0], RENDER_TARGET_NB*4 );
	cpy_v3( vox->pos, pos );
	min_max_v3r( _min_out, _max_out, pos );
	return vox;
}

void	c_bdd_voxel::add_point_line_y( REAL* pos, REAL dy, INT32 nb, INT32 img_col  )
{
	if( nb <= 0 )
		return;
	INT32	nb_needed = _point_nb_used + nb;
	if( !alloc_point( nb_needed ) )
		return;
	//GOOD_PRINT_STRING( "Alloc Point Done" );
	nb_needed = _index_nb_used + 1;
	if( !alloc_index( nb_needed ) )
		return;

	//	we have space so we will do the addition
	//GOOD_PRINT_STRING( "Do Index" );
	*(_index + _index_nb_used++) = _point_nb_used;
	REAL	p[3];
	//GOOD_PRINT_STRING( "Do Copy" );
	cpy_v3( p, pos );
	//GOOD_PRINT_STRING( "Start Loop" );
	for( INT32 i=0; i<nb; ++i )
	{
		//GOOD_PRINT_STRING( "%d : %4d/%d", img_col, i, nb );
		st_vox* vox = add_point( p );
		vox->line_nb = ( i==0 ) ? nb : 0; 
		vox->b_active = true;
		vox->b_force = false;
		vox->pixel[0] = img_col;
		vox->pixel[1] = nb-i-1;
		p[1] += dy;
	}
}

void	c_bdd_voxel::set_line_y( INT32 index, bool b_active, bool b_forced )
{
	if( 0<=index && index < _index_nb_used )
	{
		index = *(_index + index);
		st_vox* vox = _vox + index;
		INT32 nb = vox->line_nb;
		for( ; nb>0; --nb )
		{
			vox->b_active = b_active;
			vox->b_force = b_forced;
			++vox;
		}
	}
	else
		err_print( "no line y with index %d.", index );
}

//
//	RENDER
//
void	c_bdd_voxel::begin_render()
{
	_render_target = 0;
	_b_swapped = false;
}
void	c_bdd_voxel::end_render()
{
}

void	c_bdd_voxel::before_render( bool b_swap )
{
	if( b_swap )
		SWAP( _colors_hd[0], _colors_hd[1] );
	_b_swapped = b_swap;
}
void	c_bdd_voxel::after_render()
{
	if( _b_swapped )
	{
		SWAP( _colors_hd[0], _colors_hd[1] );
		_b_swapped = false;
	}
}

void	c_bdd_voxel::set_color(		INT32 index, FP32* col )
{
	CLAMP_REF( index, 0, COLOR_NB-1 );
	cpy_v4( _colors[index], col );
}
void	c_bdd_voxel::use_color(		INT32* index,	INT32 nb	)
{
	CLAMP_REF( nb, 1, COLOR_NB );
	for( INT32 i=0; i<nb; ++i )
	{
		_colors_hd[i] = _colors[*index];
		++index;
	}

}

void	c_bdd_voxel::set_color_map(	INT32 index, INT32 bind, REAL u, REAL v, REAL fu, REAL fv		)	//index <0  clear it
{
	_b_color_map = false;
	if( bind < 0 )
		return;

	c_img_2d* img = g_bind_img_2d->get_ready( bind );
	if( !img->is_ok() )
		return;

	CLAMP_REF( index, 0, COLOR_MAP_NB-1 );
	if( index == 0 )
		_b_color_map = true;	// index <=0 
	class c_color_map* p_map = &_map[index];
	p_map->set_img( img );
	p_map->set_coor( u, v, fu, fv );
}

void	c_bdd_voxel::use_color_map(		INT32* index,	INT32 nb	)
{
	CLAMP_REF( nb, 1, COLOR_NB );
	for( INT32 i=0; i<nb; ++i )
	{
		_p_map[i] = &_map[*index];
		++index;
	}
}

//
//	BUFFER
//
void	c_bdd_voxel::clear_buffer(	INT32 dst ) 
{
	make_valid_buffer_target( dst );
	st_vox*	vox = _vox;
	for( INT32 nb =_point_nb_used; nb>0; --nb )
	{
		clear_v4( vox->color[dst] );
		++vox;
	}
}

void	c_bdd_voxel::fill_buffer(	INT32 dst,	FP32* color )
{
	make_valid_buffer_target( dst );
	st_vox*	vox = _vox;
	for( INT32 nb=_point_nb_used; nb>0; --nb )
	{
		cpy_v4( vox->color[dst], color );
		++vox;
	}
}

void	c_bdd_voxel::cpy_buffer(	INT32 dst, INT32 src )
{
	make_valid_buffer_target( dst, src );
	if( src == dst )	{	return;		}
	st_vox*	vox = _vox;
	for( INT32 nb=_point_nb_used; nb>0; --nb )
	{
		cpy_v4( vox->color[dst], vox->color[src] );
		++vox;
	}
}

void	c_bdd_voxel::scale_buffer(	INT32 dst, INT32 src, FP32 f )
{
	if( f == 1. )
	{
		cpy_buffer( dst, src );
		return;
	}
	if( f == 0 )
	{
		clear_buffer( dst );
		return;
	}
	make_valid_buffer_target( dst, src );
	st_vox*	vox = _vox;
	if( dst == src )
	{
		for( INT32 nb=_point_nb_used; nb>0; --nb )
		{
			scale_v4( vox->color[dst], f );
			++vox;
		}
	}
	else
	{
		for( INT32 nb=_point_nb_used; nb>0; --nb )
		{
			scale_v4( vox->color[dst], vox->color[src], f );
			++vox;
		}
	}
}

void	c_bdd_voxel::scale_buffer(	INT32 dst, INT32 src, FP32* color )
{
	make_valid_buffer_target( dst, src );
	st_vox*	vox = _vox;
	if( dst == src )
	{
		for( INT32 nb=_point_nb_used; nb>0; --nb )
		{
			mul_v4( vox->color[dst], color );
			++vox;
		}
	}
	else
	{
		for( INT32 nb=_point_nb_used; nb>0; --nb )
		{
			mul_v4( vox->color[dst], vox->color[src], color );
			++vox;
		}
	}
}

//todo  correct this now not finished
// b over a
void	c_bdd_voxel::add_buffer_over(	 INT32 dst, INT32 a, INT32 b )
{
	make_valid_buffer_target( dst, a, b );
	st_vox*	vox = _vox;
	if( dst == a )
	{
		for( INT32 nb=_point_nb_used; nb>0; --nb )
		{
			FP32 alpha_a = vox->color[dst][3];
			FP32 alpha_b = vox->color[b][3];
			FP32 alpha = alpha_a + alpha_b * ( 1 - alpha_a );
			vox->color[dst][3] = alpha;
			mix_v3( vox->color[dst], (FP32)((1-alpha_b)*alpha_a), vox->color[b], alpha_b );
			++vox;
		}
	}
	else
	{
		for( INT32 nb=_point_nb_used; nb>0; --nb )
		{
			FP32 alpha = vox->color[b][3];
			mix_v3( vox->color[dst], vox->color[dst], (FP32)(1-alpha), vox->color[b], alpha );
			vox->color[dst][3] = vox->color[a][3] + alpha * ( 1 - vox->color[a][3] );
			++vox;
		}
	}
}

void	c_bdd_voxel::add_buffer(	 INT32 dst, INT32 a, INT32 b )
{
	make_valid_buffer_target( dst, a, b );
	st_vox*	vox = _vox;

	if( dst == a )
	{
		for( INT32 nb=_point_nb_used; nb>0; --nb )
		{
			add_v3( vox->color[dst], vox->color[b] );
			FP32 alpha_a = vox->color[dst][3];
			FP32 alpha_b = vox->color[b][3];
			vox->color[dst][3] =  alpha_a + alpha_b * ( 1 - alpha_a );
			++vox;
		}
	}
	else
	{
		for( INT32 nb=_point_nb_used; nb>0; --nb )
		{
			add_v3( vox->color[dst], vox->color[a], vox->color[b] );
			FP32 alpha_a = vox->color[a][3];
			FP32 alpha_b = vox->color[b][3];
			vox->color[dst][3] = alpha_a + alpha_b * ( 1 - alpha_a );
			++vox;
		}
	}
}
void	c_bdd_voxel::sub_buffer(	 INT32 dst, INT32 a, INT32 b )
{
	make_valid_buffer_target( dst, a, b );
	st_vox*	vox = _vox;
	if( dst == a )
	{
		for( INT32 nb=_point_nb_used; nb>0; --nb )
		{
			sub_v3( vox->color[dst], vox->color[b] );
			++vox;
		}
	}
	else
	{
		for( INT32 nb=_point_nb_used; nb>0; --nb )
		{
			sub_v3( vox->color[dst], vox->color[a], vox->color[b] );
			++vox;
		}
	}
}

void	c_bdd_voxel::mul_buffer(	 INT32 dst, INT32 a, INT32 b )
{
	make_valid_buffer_target( dst, a, b );
	st_vox*	vox = _vox;
	if( dst == a )
	{
		for( INT32 nb=_point_nb_used; nb>0; --nb )
		{
			mul_v3( vox->color[dst], vox->color[b] );
			++vox;
		}
	}
	else
	{
		for( INT32 nb=_point_nb_used; nb>0; --nb )
		{
			mul_v3( vox->color[dst], vox->color[a], vox->color[b] );
			++vox;
		}
	}
}

void	c_bdd_voxel::min_buffer(	 INT32 dst, INT32 a, INT32 b )
{
	make_valid_buffer_target( dst, a, b );
	st_vox*	vox = _vox;
	if( dst == a )
	{
		for( INT32 nb=_point_nb_used; nb>0; --nb )
		{
			min_v3( vox->color[dst], vox->color[b] );
			++vox;
		}
	}
	else
	{
		for( INT32 nb=_point_nb_used; nb>0; --nb )
		{
			min_v3( vox->color[dst], vox->color[a], vox->color[b] );
			++vox;
		}
	}
}

void	c_bdd_voxel::max_buffer(	 INT32 dst, INT32 a, INT32 b )
{
	make_valid_buffer_target( dst, a, b );
	st_vox*	vox = _vox;
	if( dst == a )
	{
		for( INT32 nb=_point_nb_used; nb>0; --nb )
		{
			max_v3( vox->color[dst], vox->color[b] );
			++vox;
		}
	}
	else
	{
		for( INT32 nb=_point_nb_used; nb>0; --nb )
		{
			max_v3( vox->color[dst], vox->color[a], vox->color[b] );
			++vox;
		}
	}
}

void	c_bdd_voxel::mix_buffer(	 INT32 dst, INT32 a, INT32 b, REAL fa, REAL fb )
{
	if( fa == 0 )
	{
		scale_buffer( dst, b, fb );
		return;
	}
	if( fb == 0 )
	{
		scale_buffer( dst, a, fa );
	}
	make_valid_buffer_target( dst, a, b );
	st_vox*	vox = _vox;
	if( dst == a )
	{
		for( INT32 nb=_point_nb_used; nb>0; --nb )
		{
			mix_v3( vox->color[dst], fa, vox->color[b], fb );
			++vox;
		}
	}
	else
	{
		for( INT32 nb=_point_nb_used; nb>0; --nb )
		{
			mix_v3( vox->color[dst], vox->color[a], fa, vox->color[b], fb );
			++vox;
		}
	}
}

void	c_bdd_voxel::rgb_to_hsv(	INT32	dst,	INT32	src		)
{
	make_valid_buffer_target( dst, src );
	st_vox*	vox = _vox;
	//todoopt deal with this case	if( dst == src )
	for( INT32 nb=_point_nb_used; nb>0; --nb )
	{
		aaa::color::hsv_from_rgb( vox->color[dst], vox->color[src] );
		++vox;
	}
}
void	c_bdd_voxel::hsv_to_rgb(	INT32	dst,	INT32	src		)
{
	make_valid_buffer_target( dst, src );
	st_vox*	vox = _vox;
	//todoopt deal with this case	if( dst == src )
	for( INT32 nb=_point_nb_used; nb>0; --nb )
	{
		aaa::color::rgb_from_hsv( vox->color[dst], vox->color[src] );
		++vox;
	}
}


struct st_op_possible
{
	C_PCHAR	name;
	INT32	buf_nb;
	INT32	id;
};
enum	OP : INT32
{
	ID_IMG_COLOR_CLEAR = 0,
	ID_TARGET,
	ID_CLEAR,
	ID_FILL,
	ID_SCALE,
	ID_COPY,
	ID_ADD,
	ID_ADD_OVER,
	ID_SUB,
	ID_MUL,
	ID_MIN,
	ID_MAX,
	ID_MIX,
	ID_INTERPOLATE,
	ID_NOISE_2D,
	ID_NOISE_3D,
	ID_RGB_TO_HSV,
	ID_HSV_TO_RGB,
	ID_RENDER_TEXTURE,
	ID_OP_MAX
};

//we should do it with std and use hashing
//	quick hack use first letter
static	st_op_possible	ops_pos[]  =
{
	{	"set_target",			1,	ID_TARGET			},
	{	"clear",				1,	ID_CLEAR			},
	{	"fill",					1,	ID_FILL				},
	{	"scale",				1,	ID_SCALE			},
	{	"cpy",					2,	ID_COPY				},
	{	"copy",					2,	ID_COPY				},
	{	"add",					2,	ID_ADD				},
	{	"add_over",				2,	ID_ADD_OVER			},
	{	"sub",					2,	ID_SUB				},
	{	"mul",					2,	ID_MUL				},
	{	"mix",					2,	ID_MIX				},
	{	"interpolate",			2,	ID_INTERPOLATE		},
	{	"min",					2,	ID_MIN				},
	{	"max",					2,	ID_MAX				},
	{	"image_color_clear",	0,	ID_IMG_COLOR_CLEAR	},
	{	"noise_2d",				0,	ID_NOISE_2D			},
	{	"noise_3d",				0,	ID_NOISE_3D			},
	{	"rgb_to_hsv",			1,	ID_RGB_TO_HSV		},
	{	"hsv_to_rgb",			1,	ID_HSV_TO_RGB		},
	{	"render_texture",		0,	ID_RENDER_TEXTURE	},
};

bool	c_bdd_voxel::render_op(	st_render_op*	op	)
{
	C_PCHAR_C name =	op->name.get();
	INT32	i	=	0;
	for( i=0; i<sizeof(ops_pos); ++i )
	{
		if( str_is_equal( name, ops_pos[i].name ) )
			break;
	}
	if( i>=sizeof(ops_pos) )
	{
		err_print( "don't know this operation %s", name );
		op->err = "don't know this operation";
		return false;
	}

	bool	b_ret = true;
	INT32	id	=	ops_pos[i].id;
	switch( op->nb_buf )
	{
	case 0:
		switch( id )
		{
		case ID_IMG_COLOR_CLEAR:	cpy_v4(				_color_img_clear,	op->args );				break;
		case ID_NOISE_2D:			render_noise_2d(	op->args,	&( op->args[3] ),
															op->args[6], op->args[7], op->args[8],
															op->args[9] >= 0.5
															);
									break;
		case ID_NOISE_3D:			render_noise_3d(	op->args,			&( op->args[3] )	);											break;
		case ID_RENDER_TEXTURE:		render_texture(		INT32(op->args[0]),	op->args[1],	op->args[2],	op->args[3], op->args[4] );		break;
		default:
			op->err = "this operation need buffers";
			b_ret = false;
			break;
		}
		break;
	case 1:
		{
			INT32	dst	=	op->buf[0]-1;	
			switch( id )
			{
			case ID_TARGET:
				_render_target = dst;
				make_valid_buffer_target( _render_target );
				break;
			case ID_CLEAR:				clear_buffer(	dst							);	break;
			case ID_FILL:				fill_buffer(	dst,			op->args	);	break;
			case ID_SCALE:				scale_buffer(	dst,	dst,	op->args	);	break;
			case ID_RGB_TO_HSV:			rgb_to_hsv(		dst,	dst					);	break;
			case ID_HSV_TO_RGB:			hsv_to_rgb(		dst,	dst					);	break;
			default:
				op->err = "this operation don't need just one buffer";
				b_ret = false;
				break;
			}
		}
		break;
	case 2:
		{
			INT32	dst	=	op->buf[0]-1;
			INT32	bu1	=	op->buf[1]-1;
			switch( id )
			{
			case ID_SCALE:				scale_buffer(	dst,			bu1,		op->args	);	break;
			case ID_COPY:				cpy_buffer(		dst,			bu1			);	break;
			case ID_ADD_OVER:			add_buffer_over(dst,	dst,	bu1			);	break;
			case ID_ADD:				add_buffer(		dst,	dst,	bu1			);	break;
			case ID_SUB:				sub_buffer(		dst,	dst,	bu1			);	break;
			case ID_MUL:				mul_buffer(		dst,	dst,	bu1			);	break;
			case ID_MIN:				min_buffer(		dst,	dst,	bu1			);	break;
			case ID_MAX:				max_buffer(		dst,	dst,	bu1			);	break;
			case ID_MIX:				mix_buffer(		dst,	dst,	bu1,		op->args[0],			op->args[1]	);	break;
			case ID_INTERPOLATE:		mix_buffer(		dst,	dst,	bu1,		REAL(1)-op->args[0],	op->args[0]	);	break;
			case ID_RGB_TO_HSV:			rgb_to_hsv(		dst,			bu1			);	break;
			case ID_HSV_TO_RGB:			hsv_to_rgb(		dst,			bu1			);	break;
			default:
				op->err = "this operation don't need two buffers";
				b_ret = false;
				break;
			}
		}
		break;
	case 3:
		{
			INT32	dst	=	op->buf[0]-1;
			INT32	bu1	=	op->buf[1]-1;
			INT32	bu2	=	op->buf[2]-1;
			switch( id )
			{
			case ID_ADD:				add_buffer(		dst,	bu1,	bu2			);	break;
			case ID_SUB:				sub_buffer(		dst,	bu1,	bu2			);	break;
			case ID_MUL:				mul_buffer(		dst,	bu1,	bu2			);	break;
			case ID_MIN:				min_buffer(		dst,	bu1,	bu2			);	break;
			case ID_MAX:				max_buffer(		dst,	bu1,	bu2			);	break;
			case ID_MIX:				mix_buffer(		dst,	bu1,	bu2,		op->args[0],			op->args[1]	);	break;
			case ID_INTERPOLATE:		mix_buffer(		dst,	bu1,	bu2,		REAL(1)-op->args[0],	op->args[0]	);	break;
				op->err = "this operation don't need three buffers";
				b_ret = false;
				break;
			}
		}
		break;
	}
	return b_ret;
}

//
//	PRIM
//
FINLINE	void	c_bdd_voxel::render_voxel( st_vox*	vox, FP32 CONST * CONST color )
{
	FP32*	dst	=	vox->color[_render_target];
	switch( _s_blend_mode )
	{
	case BLEND_MODE_REPLACE:
		mul_add_v4( dst, color, _color_factor,  _color_offset );
		break;
	case BLEND_MODE_OVER:
		{
			FP32	col[4]; 
			mul_add_v4( col, color, _color_factor,  _color_offset );
			dst[3] = dst[3] + col[3] - dst[3] * col[3];
			REAL over = OVER_ONE_AS_REAL( dst[3] );
			mix_v3( dst, dst[3]*(1-col[3])*over, col, col[3]*over );
		}
		break;
	case BLEND_MODE_ADD_ALPHA:
		{
			FP32	col[4];
			mul_add_v4( col, color, _color_factor,  _color_offset );
			mix_v3( dst, 1-col[3], col, col[3] );
			dst[3] = dst[3] + col[3] - dst[3] * col[3];

		}
		break;
	case BLEND_MODE_ADD:
		{
			FP32	col[4];
			mul_add_v4( col, color, _color_factor,  _color_offset );
			add_scale_v3( dst, col, col[3] );
			dst[3] = dst[3] + col[3] - dst[3] * col[3];
		}
		break;
	default:
	case BLEND_MODE_MAX:
		{
			FP32	col[4];
			mul_add_v4( col, color, _color_factor,  _color_offset );
			//scale_v3( col, col[3] );
			max_v3( dst, col );
			dst[3] = dst[3] + col[3] - dst[3] * col[3];
		}
		break;
	}
	
}
FINLINE	void	c_bdd_voxel::build_color( FP32* color, FP32 CONST t )
{
	if( _b_color_map )
		_p_map[0]->get_color_u( color, t );
	else
		interpolate_v4( color, _colors_hd[0], _colors_hd[1], t );
}
FINLINE	void	c_bdd_voxel::render_voxel( st_vox* vox, FP32 CONST t )
{
	FP32 color[4];
	build_color( color, t );
	render_voxel( vox, color );
}
FINLINE	void	c_bdd_voxel::render_voxel( st_vox* vox, FP32 CONST t, FP32 CONST * CONST color_in  )
{
	FP32 color[4];
	build_color( color, t );
	mul_v4( color, color_in );
	render_voxel( vox, color );
}

void	c_bdd_voxel::render_plane_axe( INT32 axe, REAL beg, REAL end )
{
	beg = transform_pos_axe( beg, axe );
	end = transform_pos_axe( end, axe );
	if( beg > end )
	{
		SWAP( beg, end );
		before_render( true );
	}
	else
		before_render( false );

	REAL	factor	=	OVER_ONE_AS_REAL(end - beg);
	st_vox*	vox;
	INT32*	hdi		=	_index;
	if( axe==1 )	//	Y VERTCAL
	{
		for( INT32 nb_line=_index_nb_used; nb_line>0; --nb_line )
		{
			vox = _vox + *hdi;
			++hdi;
			for( INT32	nb = vox->line_nb; nb>0; --nb )
			{
				REAL val = vox->pos[1];
				if( beg <= val && val <= end )
				{
					render_voxel( vox, (val-beg) * factor );
				}
				++vox;
			}
		}
	}
	else
	{
		for( INT32 nb_line=_index_nb_used; nb_line>0; --nb_line )
		{
			vox = _vox + *hdi;
			++hdi;
			REAL val = vox->pos[axe];
			if( beg <= val && val <= end )
			{
				for( INT32	nb = vox->line_nb; nb>0; --nb )
				{
					render_voxel( vox, (val-beg) * factor );
					++vox;
				}
			}
		}
	}
	after_render();
}

void	c_bdd_voxel::render_sphere(	REAL CONST* CONST pos_in, REAL size, REAL delta_full,	REAL delta_grad	)
{
	REAL	pos[3];
	transform_pos( pos, pos_in );
	REAL	radius	=	size * REAL(.5);
	REAL	re		=	radius + delta_full * REAL(.5);
	REAL	re2		=	re * re;
	REAL	ri		=	radius - delta_full * REAL(.5);
	REAL	ri2		=	(ri>0) ? ri * ri : REAL(0);

	REAL	ra		=	radius - delta_grad * REAL(.5);
	REAL	fa		=	OVER_ONE_AS_REAL( ra - ri );
			ra		=	(ra>0) ? ra * ra : REAL(0);

	REAL	rb		=	radius + delta_grad * REAL(.5);
	REAL	fb		=	OVER_ONE_AS_REAL( rb - re );
			rb		=	(rb>0) ? rb * rb : REAL(0);

	st_vox*	vox;
	INT32*	hdi		=	_index;
	for( INT32 nb_line=_index_nb_used; nb_line>0; --nb_line )
	{
		vox = _vox + *hdi;
		++hdi;
		REAL d2 = pos[0] - vox->pos[0];
		d2 = d2 * d2;
		if( d2 < re2 )
		{
			REAL tmp =  pos[2] - vox->pos[2];
			d2 += tmp * tmp;
			if( d2 < re2 )
			{	//	ok now we can try point by point
				bool b_in = false;
				for( INT32 nb = vox->line_nb; nb>0; --nb )
				{
					tmp = pos[1] - vox->pos[1];
					tmp = d2 + tmp * tmp;
					if( tmp < re2 )
					{
						b_in = true;
						if( tmp > ri2 )
						{
							if( tmp < ra )
								render_voxel( vox, ( SQRT(tmp) - ri ) * fa );
							else if( rb < tmp )
								render_voxel( vox, ( SQRT(tmp) - re ) * fb );
							else
								render_voxel( vox, 1 );
						}
					}
					else if ( b_in )
					{	//we where in, so we are too far now in this line
						break;
					}
					++vox;
				}
			}
		}
	}
}

void	c_bdd_voxel::render_boule(	REAL CONST* CONST pos_in, REAL size, REAL inside_cano )
{
	REAL	pos[3];
	transform_pos( pos, pos_in);
	REAL	radius	=	size * REAL(.5);
	REAL	r2		=	radius * radius;
	REAL	off		=	CLAMP_01( inside_cano ) * radius;
	REAL	fac		=	OVER_ONE_AS_REAL( radius - off );
	off	= off * off;

	st_vox*	vox;
	INT32*	hdi		=	_index;
	for( INT32 nb_line=_index_nb_used; nb_line>0; --nb_line )
	{
		vox = _vox + *hdi;
		++hdi;
		REAL d2 = pos[0] - vox->pos[0];
		d2 = d2 * d2;
		if( d2 < r2 )
		{
			REAL tmp =  pos[2] - vox->pos[2];
			d2 += tmp * tmp;
			if( d2 < r2 )
			{	//	ok now we can try point by point
				bool	b_in = false;
				for( INT32 nb = vox->line_nb; nb>0; --nb )
				{
					tmp = pos[1] - vox->pos[1];
					tmp = d2 + tmp * tmp;
					if( tmp < r2 )
					{
						b_in = true;
						if( tmp <= off )
							render_voxel( vox, 1 );
						else
							render_voxel( vox, ( radius - SQRT(tmp) ) * fac );
					}
					else if ( b_in )
					{	//we where in, so we are too far now in this line
						break;
					}
					++vox;
				}
			}
		}
	}
}

void	c_bdd_voxel::render_noise_2d(	FP32 CONST* pos, FP32 CONST* size, FP32 offset, FP32 factor, FP32 size_change, bool b_only_slice )
{
	st_vox*	vox;
	INT32*	hdi		=	_index - 1;
	REAL	vec[3];
	for( INT32 nb_line=_index_nb_used; nb_line>0; --nb_line )
	{
		vox = _vox + *++hdi;
		vec[0] = vox->pos[0];
		vec[1] = 0;
		vec[2] = vox->pos[2];
		mul_add_v3( vec, size, pos );
		REAL vb = noise3( vec );
		vb = offset + vb * factor;
		REAL ve = vb + size_change;
		REAL f = OVER_ONE_AS_REAL( ve - vb );
		for( INT32 nb = vox->line_nb; nb>0; --nb )
		{
			REAL y = vox->pos[1];
			if( b_only_slice )
			{
				y = ABS( (y - vb) * f );
				if( INSIDE_01(y) )
					render_voxel( vox, y );
			}
			else
			{
				if( y <= vb )
					y = 0;
				else if( ve <= y )
					y = 1;
				else
					y = (y - vb) * f;
				render_voxel( vox, y );
			}
			++vox;
		}
	}
}

void	c_bdd_voxel::render_noise_3d(	FP32 CONST* pos, FP32 CONST* size )
{
	st_vox*	vox;
	INT32*	hdi		=	_index - 1;
	REAL	vec[3];
	for( INT32 nb_line=_index_nb_used; nb_line>0; --nb_line )
	{
		vox = _vox + *++hdi;
		for( INT32 nb = vox->line_nb; nb>0; --nb )
		{
			mul_add_v3( vec, vox->pos, size, pos );
			REAL v = noise3( vec );
			render_voxel( vox, v );
			++vox;
		}
	}
}

void	c_bdd_voxel::render_texture(	INT32 color_map_id, REAL center_z_offset, REAL offset, REAL size, REAL inside_cano )
{
	c_color_map* map = get_color_map( color_map_id );
	INT32*	hdi		=	_index - 1;
	REAL	y;
	FP32	color[4];
	size /= 2;
	if( center_z_offset != 0. )
	{
		offset += center_z_offset;
	}
	for( INT32 nb_line=_index_nb_used; nb_line>0; --nb_line )
	{
		st_vox*	vox = _vox + *++hdi;
		REAL u = vox->pos[0]; 
		REAL w = vox->pos[2];
		if( center_z_offset != 0. )
		{
			w += center_z_offset;
			REAL a = REAL( ATAN2_TURN( w, u ) );
//			if( a <.25 )		{	a += .5;	}
//			else if( a > .25 )	{	a -= .5;	}
			w = SQRT( u*u + w*w );
			u = a * REAL(PI) * offset;
		}
		w -= offset;
		w = ABS( w );
		if( w < size )
		{
			w /= size;
			w = (w <= inside_cano) ? 1 : ( 1 - w ) / (1 - inside_cano );
			for( INT32 nb = vox->line_nb; nb>0; --nb )
			{
				y = vox->pos[1];
				map->get_color_uv( color, u, y );
				render_voxel( vox, w, color );
				++vox;
			}
		}
	}
}

void	c_bdd_voxel::render_cube(	REAL CONST* CONST pos_in, REAL size, REAL inside_cano )
{
	REAL	pos[3];
	transform_pos( pos, pos_in );

	REAL	radius	=	size * REAL(.5);
	REAL	off		=	CLAMP_01( inside_cano ) * radius;
	REAL	fac		=	OVER_ONE_AS_REAL( radius - off );
	off	= off * off;

	st_vox*	vox;
	INT32*	hdi		=	_index;
	for( INT32 nb_line=_index_nb_used; nb_line>0; --nb_line )
	{
		vox = _vox + *hdi;
		++hdi;
		REAL d = ABS( pos[0] - vox->pos[0] );
		if( d < radius )
		{
			d = MAX( d,  ABS( pos[2] - vox->pos[2] ) );
			if( d < radius )
			{	//	ok now we can try point by point
				bool	b_in = false;
				for( INT32 nb = vox->line_nb; nb>0; --nb )
				{
					REAL tmp = MAX( d,  ABS( pos[1] - vox->pos[1] ) );
					if( tmp < radius )
					{
						b_in = true;
						if( tmp <= off )
							render_voxel( vox, 1 );
						else
							render_voxel( vox, ( radius - tmp ) * fac );
					}
					else if ( b_in )
					{	//we where in, so we are too far now in this line
						break;
					}
					++vox;
				}
			}
		}
	}
}

void	c_bdd_voxel::render_segment(	REAL CONST* CONST a, REAL CONST* CONST b, REAL size, REAL inside_cano )
{
	REAL	pos_a[3];
	transform_pos( pos_a, a );
	REAL	pos_b[3];
	transform_pos( pos_b, b );

	REAL	radius	=	size * REAL(.5);
	REAL	r2		=	radius * radius;
	REAL	off		=	CLAMP_01( inside_cano ) * radius;
	REAL	fac		=	OVER_ONE_AS_REAL( radius - off );
	off	= off * off;

	REAL	vb[3];
	sub_v3( vb, pos_b, pos_a );
	REAL nvb2 =  vb[0] * vb[0] + vb[2] * vb[2];
	REAL over_nvb2 = OVER_ONE_AS_REAL( nvb2 );
	REAL nvb3 =  vb[0] * vb[0] + vb[1] * vb[1] + vb[2] * vb[2];
	REAL over_nvb3 = OVER_ONE_AS_REAL( nvb3 );

	st_vox*	vox;
	INT32*	hdi		=	_index;
	for( INT32 nb_line=_index_nb_used; nb_line>0; --nb_line )
	{
		vox = _vox + *hdi;
		++hdi;

		REAL	vp[3];
		vp[0] = vox->pos[0] - pos_a[0];
		vp[2] = vox->pos[2] - pos_a[2];		
		REAL t = vp[0] * vb[0] + vp[2] * vb[2];
		REAL da;
		REAL db;
		if( t <= 0 )
		{
			da = vp[0]*vp[0] + vp[2]*vp[2];
			if( da > r2 )
				continue;
			REAL tmp =  vox->pos[0] - pos_b[0];
			db = tmp*tmp;
			tmp = vox->pos[2] - pos_b[2];
			db += tmp*tmp;

		}
		else if( t >= nvb2 )
		{
			REAL tmp =  vox->pos[0] - pos_b[0];
			db = tmp*tmp;
			tmp = vox->pos[2] - pos_b[2];
			db += tmp*tmp;
			if( db > r2 )
				continue;
			da = vp[0]*vp[0] + vp[2]*vp[2];
		}
		else
		{
			REAL f = t * over_nvb2;
			REAL	tmp;
			tmp	=	vox->pos[0] - ( pos_a[0] + f * vb[0] );
			da	=	tmp * tmp;
			tmp	=	vox->pos[2] - ( pos_a[2] + f * vb[2] );
			da += tmp * tmp;
			if( da > r2 )
				continue;
			da = vp[0]*vp[0] + vp[2]*vp[2];
			tmp =  vox->pos[0] - pos_b[0];
			db = tmp*tmp;
			tmp = vox->pos[2] - pos_b[2];
			db += tmp*tmp;

		}
		INT32 nb = vox->line_nb;
		--vox;
		bool	b_in = false;
		for( ; nb>0; --nb )
		{
			++vox;
			vp[1] = vox->pos[1] - pos_a[1];
			REAL t3 = t  + vp[1] * vb[1];
			REAL d;
			if( t3 <= 0 )
			{
				d = da + vp[1]*vp[1];
			}
			else if( t3 >= nvb3 )
			{
				REAL tmp =  vox->pos[1] - pos_b[1];
				d = db + tmp*tmp;
			}
			else
			{
				REAL f = t3 * over_nvb3;
				REAL	tmp;
				tmp	=	vox->pos[0] - ( pos_a[0] + f * vb[0] );
				d	=	tmp * tmp;
				tmp	=	vox->pos[2] - ( pos_a[2] + f * vb[2] );
				d += tmp * tmp;
				if( d > r2 )
					continue;
				tmp	=	vox->pos[1] - ( pos_a[1] + f * vb[1] );
				d += tmp * tmp;
			}
			if( d > r2 )
			{
				if( b_in )
					break;
				continue;
			}
			//if( b_in )
			//	break;
			b_in = true;
			if( d <= off )
				render_voxel( vox, 1 );
			else
				render_voxel( vox, ( radius - SQRT(d) ) * fac );
		}
	}
}

void	c_bdd_voxel::render_cylinder_y(	REAL CONST* CONST pos_in, REAL size, REAL delta_full, REAL delta_grad )
{
	REAL	pos[3];
	transform_pos( pos, pos_in );

	REAL	radius	=	size * REAL(.5);
	REAL	re		=	radius + delta_full * REAL(.5);
	REAL	re2		=	re * re;
	REAL	ri		=	radius - delta_full * REAL(.5);
	REAL	ri2		=	(ri>0) ? ri * ri : REAL(0);

	REAL	ra		=	radius - delta_grad * REAL(.5);
	REAL	fa		=	OVER_ONE_AS_REAL( ra - ri );
			ra		=	(ra>0) ? ra * ra : REAL(0);

	REAL	rb		=	radius + delta_grad * REAL(.5);
	REAL	fb		=	OVER_ONE_AS_REAL( rb - re );
			rb		=	(rb>0) ? rb * rb : REAL(0);

	FP32	color[4];

	st_vox*	vox;
	INT32*	hdi		=	_index;
	for( INT32 nb_line=_index_nb_used; nb_line>0; --nb_line )
	{
		vox = _vox + *hdi;
		++hdi;
		REAL d2 = pos[0] - vox->pos[0];
		d2 = d2 * d2;
		if( d2 < re2 )
		{
			REAL tmp =  pos[2] - vox->pos[2];
			d2 += tmp * tmp;
			if( d2 < re2 && d2 > ri2 )
			{	//	ok now we can try point by point
				if( d2 < ra )
					build_color( color, ( SQRT(d2) - ri ) * fa );
				else if( rb < d2 )
					build_color( color, ( SQRT(d2) - re ) * fb );
				else
					build_color( color, 1. );
				for( INT32 nb = vox->line_nb; nb>0; --nb )
				{
					render_voxel( vox, color );
					++vox;
				}
			}
		}
	}
}

//
//	DRAW
//
void	c_bdd_voxel::draw_single()
{
	if( _point_nb_used <= 0 )
	{
		return;
	}

	INT32	nbu	=	_led_sphere_seg_nb;
	INT32	nbv	=	nbu / 2 + 1;
	
	REAL	pos[3];
	st_vox*	vox = _vox;
	for( INT32 nb_pt=_point_nb_used; nb_pt>0;  )
	{
		mul_add_v3( _col_out, build_color_out( vox ), _color_base_factor, _color_base_offset );
		c_color::mod->draw_color( _col_out );
			
		//todo avoid copy
		cpy_v3( pos, vox->pos );
		if( _b_led_sphere_ui )
		{
#if 1	//faster
			draw_sphere_at( _led_size, pos, nbu, nbv );
#else
			GOL::translatev(		pos );
			draw_sphere(			_led_size, nbu, nbv );
			GOL::translate_negv(	pos );
#endif
		}
		else
		{
#if 1	//faster
			draw_cube_at( _led_size, pos );
#else
			GOL::translatev(		pos );
			draw_cube(				_led_size );
			GOL::translate_negv(	pos );
#endif
		}
		//GOOD_PRINT_STRING( "%g %g %g    %g %g %g", col[0], col[1], col[2], vox->pos[0], vox->pos[1], vox->pos[2] );
		++vox;
		--nb_pt;
	}
}

//static	c_rand	the_rand;
void	c_bdd_voxel::draw_multiple()
{
	if( _point_nb_used <= 0 )
		return;

	c_multiple*	mcur = c_multiple::cur;
	if( !mcur->begin() )
		goto exit;


	mcur->set_nb( _point_nb_used );
	REAL	col[4];
	col[3] = _color_base_offset[3];
	REAL	pos[3];
	{
		st_vox*	vox = _vox;
		for( INT32 i=0; i<_point_nb_used; ++i )
		{
			mcur->set_index( i );
			mul_add_v3( _col_out, build_color_out( vox ), _color_base_factor, _color_base_offset );
			c_color::mod->draw_color( _col_out );
			//todo avoid copy
			cpy_v3( pos, vox->pos );
			mcur->align_then_draw( pos );
			++vox;
		}
	}
exit:
	mcur->end();
}

//
//	POINT API
//
