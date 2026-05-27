#include "bdd_face.h"
#include "draw/tex_anim.h"
#include "obj_ui/deformer/def_node.h"
#include "draw/render.h"
#include "draw/model.h"
#include "time/aaa_time.h"
#include "gol/gol_draw.h"
#include "gol/gol_list.h"
#include "image/bind_img_2d.h"

FACTORY_CREATE_PROP_V1( c_bdd_face, bdd_face, Faces rotated, face, sub_menu="Old"; );

static	C_PCHAR_C str_mult_lock[LOCK_TYPE_MAX_NB] =
{
	"SIZE",
	"CENTER",
	"SPACE_ABSOLUTE",
	"SPACE_RELATIVE"
};

namespace n_bdd_face
{
	CONSTEXPR INT32 BASE_PARAM_NB		= 4 + c_bdd::GEO_PARAM_NB;
	CONSTEXPR INT32 GEO_PARAM_NB		= 13;
	CONSTEXPR INT32 FACE_PARAM_NB		= 10;
	CONSTEXPR INT32 ROTATION_PARAM_NB	= 10;
	CONSTEXPR INT32 GROUP_PARAM_NB		= 3;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	GEO_PARAM_NB
									+	FACE_PARAM_NB
									+	ROTATION_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_BASE_PARAMS

		PARAM_DEF_GROUP( Geo, GEO_PARAM_NB )
			PARAM_DEF_POINT_XYZ(		center )

			PARAM_DEF_SYMBO_PSTR_ONE(	bdd_dimension,	gstr::dim )
			PARAM_DEF_INT32_POS(		bdd_nb_u,		1, 3	)
			PARAM_DEF_INT32_POS(		bdd_nb_v,		1, 3	)
			PARAM_DEF_INT32_POS(		bdd_nb_axe,		1, 3	)

			PARAM_DEF_SYMBO(			lock_u,			0, 1,	LOCK_TYPE_MAX_NB-1,	str_mult_lock )
			PARAM_DEF_SYMBO(			lock_v,			0, 1,	LOCK_TYPE_MAX_NB-1,	str_mult_lock )	
			PARAM_DEF_SYMBO(			lock_axe,		0, 1,	LOCK_TYPE_MAX_NB-1,	str_mult_lock )

			PARAM_DEF_SCALE_UVA(		space )
		PARAM_DEF_GROUP( Face, FACE_PARAM_NB )
			PARAM_DEF_SYMBO_PSTR_ONE(	face_axe,		gstr::uva )
			PARAM_DEF_BOOL_OFF(			face_image_ratio )

			PARAM_DEF_SCALE_UVAF(		face_size )
			PARAM_DEF_POINT_UVAF(		face_offset )

		PARAM_DEF_GROUP( Rotation, ROTATION_PARAM_NB )
			PARAM_DEF_POINT_XYZ(		rotation_speed )
			PARAM_DEF_REAL_ZERO(		rotation_speed_factor )

			PARAM_DEF_POINT_XYZ(		rotation_origin )
			PARAM_DEF_POINT_XYZ(		rotation_offset )

		PARAM_DEF_SCALE_UVA(	tex_offset )
		PARAM_DEF_REAL_ZERO(	tex_offset_factor )
//		PARAM_DEF_NONE( bdd_tri )
//		{	nullptr,	PARAM_NONE,		"",			1., .0,		-PARAM_INFINI, PARAM_INFINI,	nullptr, nullptr },
	};
}

void	c_bdd_face::param_init_pt()
{
	INT32 h = param_init_pt_geo();

	++h;
		param_set_pt_3(	h, _center				);
		param_set_pt(		h, _bdd_dimension		);
		param_set_pt_3(	h, _bdd_nb				);
		param_set_pt_3(	h, _s_lock				);
		param_set_pt_3(	h, _space				);

	++h;
		param_set_pt(		h, _face_axe			);
		param_set_pt(		h, _b_face_image_ratio	);
		param_set_pt_4(	h, _face_size_ui		);
		param_set_pt_4(	h, _face_offset_ui		);

	++h;
		param_set_pt_3(	h, _rot_speed			);
		param_set_pt(		h, _rot_factor			);

		param_set_pt_3(	h, _rot_origin			);
		param_set_pt_3(	h, _rot_offset_ui		);

		param_set_pt_4(	h, _tex_offset_ui		);

//	if( c_layer::get_cur() )	//	&& s_multiple_render_type==c_multiple::RENDER_TRI)
//		param_attach_obj_no_inc( h, c_layer::get_cur()->get_bdd_tri() );
//	++h;
		
	err_param_init_pt(h);
}

void c_bdd_face::init()
{
	param_init_with( n_bdd_face::param, n_bdd_face::PARAM_NB_MAX ); // dd_face_param, BDD_MULT_PARAM_NB_MAX);
}

void c_bdd_face::alloc()	{}
void c_bdd_face::dealloc()	{}

CONSTRUCTOR_CREATE(c_bdd_face)
{
	init();
	alloc();
}

c_bdd_face::~c_bdd_face()
{
	dealloc();
}

void	c_bdd_face::build()
{
}

void	c_bdd_face::update()
{
	REAL		tmp = _face_size_ui[3];
	//INT32		i;
	INT32		size_x;
	INT32		size_y;

	c_model::cur->get_size_v3( _bdd_size );
//hack
	_face_size[2] = _face_size_ui[2] * tmp;
	if ( _b_face_image_ratio && g_bind_img_2d->is_size_cur( size_x, size_y ) )
	{
		tmp /= MAX( size_x, size_y);
		_face_size[0] = _face_size_ui[0] * size_x * tmp;
		_face_size[1] = _face_size_ui[1] * size_y * tmp;
	}
	else
	{
		_face_size[0] = _face_size_ui[0] * tmp;
		_face_size[1] = _face_size_ui[1] * tmp;
	}

	axe_build_index_vert( _face_u, _face_v, _face_axe );

	tmp = _face_offset_ui[3] * REAL(.5);
	_face_offset[_face_u]	= _face_offset_ui[0] * _face_size[0] * tmp;
	_face_offset[_face_v]	= _face_offset_ui[1] * _face_size[1] * tmp;
	_face_offset[_face_axe] = _face_offset_ui[2] * _face_size[2] * tmp;


	switch( _bdd_dimension )
	{
	case 1:	tmp = REAL( _bdd_nb[2]						 ); break;
	case 2:	tmp = REAL( _bdd_nb[0]*_bdd_nb[1]            ); break;
	case 3:	tmp = REAL( _bdd_nb[0]*_bdd_nb[1]*_bdd_nb[2] );	break;
	}
	tmp = OVER_ONE_AS_REAL(tmp);
	scale_v3( _rot_offset, _rot_offset_ui, tmp );

	for( INT32 i = 0; i < 3; ++i )
	{
		tmp = _face_size[i];
		if ( tmp < 0 )
			tmp = -tmp;
		switch( _s_lock[i] )
		{
		case LOCK_SIZE:				_bdd_size[i] -= tmp;	break;
		case LOCK_CENTER:			break;
		case LOCK_SPACE_ABS:		_bdd_size[i] = (_bdd_nb[i]-1) * (tmp+_space[i]);			break;
		case LOCK_SPACE_RELATIVE:	_bdd_size[i] = (_bdd_nb[i]-1) * (tmp*(REAL(1)+_space[i]));	break;
		}
	}
}

void	c_bdd_face::draw_0d()
{
	REAL	f[3];
	REAL*	p_u = &f[_face_u];
	REAL*	p_v = &f[_face_v];
	REAL*	p_axe = &f[ _face_axe ];
	REAL	u,v;

	c_multiple::cur->set_nb( 1 );

	GOL::matrix::translate3v( _face_offset );

	u = _face_size[0];
	v = _face_size[1];
	if( is_normal_draw() )
	{
		if( u * v >= 0 )
			*p_axe = 1.;
		else
			*p_axe = -1.;
		*p_u = 0.;
		*p_v = 0.;
		GOL::normal3v( f );
	}

	if ( _face_axe == 0 )
	{
		u = - u;
	}
	u *= .5;
	v *= .5;
	*p_axe = 0.;

	//todo	should be gol_begin_poly_base ...
	GOL::begin( c_render::get_cur()->get_draw_primitive() );
		if ( c_map::get_cur()->is_implicit() )
		{
			FP32 CONST *	uv = c_map::get_quad_uv();

			GOL::texcoord2v( uv );
			*p_u = -u + _center[_face_u];
			*p_v =  v + _center[_face_v];
			GOL::vertex3v( f );

			GOL::texcoord2v( uv+2 );
			*p_v = -v + _center[_face_v];
			GOL::vertex3v( f );
			
			GOL::texcoord2v( uv+4 );
			*p_u =  u + _center[_face_u];
			*p_v =  v + _center[_face_v];
			GOL::vertex3v( f );
			
			GOL::texcoord2v( uv+6 );
			*p_v = -v + _center[_face_v];
			GOL::vertex3v( f );
		}
		else
		{
			if( c_render::get_cur()->is_gouraud() )
			{
				GOL::normal3v( _normal[0] );
				*p_u = -u + _center[_face_u];
				*p_v =  v + _center[_face_v];
				GOL::vertex3v( f );

				GOL::normal3v( _normal[1] );
				*p_v = -v + _center[_face_v];
				GOL::vertex3v( f );
				
				GOL::normal3v( _normal[2] );
				*p_u =  u + _center[_face_u];
				*p_v =  v + _center[_face_v];
				GOL::vertex3v( f );
				
				GOL::normal3v( _normal[3] );
				*p_v = -v + _center[_face_v];
				GOL::vertex3v( f );
			}
			else
			{
				*p_u = -u + _center[_face_u];
				*p_v =  v + _center[_face_v];
				GOL::vertex3v( f );

				*p_v = -v + _center[_face_v];
				GOL::vertex3v( f );
				
				*p_u =  u + _center[_face_u];
				*p_v =  v + _center[_face_v];
				GOL::vertex3v( f );
				
				*p_v = -v + _center[_face_v];
				GOL::vertex3v( f );
			}
		}				
	GOL::end();
}

void	c_bdd_face::draw_1d( INT32 bdd_axe )
{
	REAL	f[3];
	//axe_build_index_vert
	REAL*	p_u = &f[ (bdd_axe == 2)?0:2 ];
	REAL*	p_v = &f[ (bdd_axe == 1)?0:1 ];
	REAL*	p_axe = &f[ bdd_axe ];
	REAL	t = REAL(aaa::time::get());
	
	REAL	start;
	REAL	step;
	REAL	r[3];

	c_multiple::cur->set_nb( _bdd_nb[2] );

	step = _bdd_size[2];
	start = -step * REAL(.5);
	step /= (_bdd_nb[2]-1);

	add_scale_v3( r, _rot_origin, _rot_speed, t*_rot_factor );

	auto def = c_def_node::get_cur();
	if( def->is_deforming() )
	{
		for( INT32 i = _bdd_nb[2]; i>0; --i )
		{
			c_tex_anim::cur->bind_next();
			GOL::matrix::push();
				cpy_v3( f, _center );	//todo not optimum
				*p_axe += start;
				def->apply( f, 1 );
				GOL::matrix::translate3v( f );
				GOL::matrix::rotate_xyzv( r );
				GOL::matrix::translate3v( _face_offset );
				GOL::call_list( get_render_list_multiple_name() );
				start += step;
				add_v3( r, _rot_offset );
			GOL::matrix::pop();
		}
	}
	else
	{
		cpy_v3( f, _center );
		for( INT32 i = _bdd_nb[2]; i>0; --i )
		{
			c_tex_anim::cur->bind_next();
			GOL::matrix::push();
				*p_axe = start;
				GOL::matrix::translate3v( f );
				GOL::matrix::rotate_xyzv( r );
				GOL::matrix::translate3v( _face_offset );
				GOL::call_list( get_render_list_multiple_name() );
				start += step;
				add_v3( r, _rot_offset );
			GOL::matrix::pop();
		}
	}
}

void	c_bdd_face::draw_2d( INT32 bdd_axe )
{
	//INT32	iu,iv;
	REAL	f[3];
	REAL	fbis[3];
	//axe_build_index_vert
	REAL*	p_u = &f[ (bdd_axe == 2)?0:2 ];
	REAL*	p_v = &f[ (bdd_axe == 1)?0:1 ];
	REAL*	p_axe = &f[ bdd_axe ];
	REAL	t = REAL(aaa::time::get());

	REAL	start_u;
	REAL	start_v;
	REAL	step_u;
	REAL	step_v;
	REAL	r[3];

	c_multiple::cur->set_nb( _bdd_nb[0], _bdd_nb[1] );

	step_u = _bdd_size[0];
	start_u = -step_u * REAL(.5);
	step_u /= _bdd_nb[0]-1;

	step_v = _bdd_size[1];
	start_v = -step_v * REAL(.5);
	step_v /= _bdd_nb[1]-1;

	add_scale_v3( r, _rot_origin, _rot_speed, t*_rot_factor );

//todo	perhaps clear check around

	auto def = c_def_node::get_cur();
	if( !def->is_deforming() )
		def = nullptr;

	*p_axe = 0;
	*p_u = start_u;
	for( INT32 iv = _bdd_nb[1]; iv>0; --iv )
	{
		*p_v = start_v;
		for( INT32 iu = _bdd_nb[0]; iu>0; --iu )
		{
			c_tex_anim::cur->bind_next();
			GOL::matrix::push();
				add_v3( fbis, f, _center );
				if( def )
					def->apply( fbis, 1 );
				GOL::matrix::translate3v( fbis );
				GOL::matrix::rotate_xyzv( r );
				GOL::matrix::translate3v( _face_offset );
				GOL::call_list( get_render_list_multiple_name() );
				add_v3( r, _rot_offset );
			GOL::matrix::pop();
			*p_v += step_u;
		}
		*p_u += step_v;
	}
}

void	c_bdd_face::draw_3d( INT32 bdd_axe )
{
	//INT32	iu,iv,ia;
	REAL	tmp;

	INT32	i_u;
	INT32	i_v;
	INT32	i_axe = bdd_axe;
	axe_build_index_vert( i_u, i_v, i_axe );

	c_multiple::cur->set_nb( _bdd_nb[0], _bdd_nb[1], _bdd_nb[2] );

	REAL	start[3];
	REAL	step[3];
	REAL	r[3];
	REAL	dr[3];
	REAL	t = REAL(aaa::time::get()) * _rot_factor;
	for( INT32 i = 0; i < 3; ++i )
	{
		tmp = _bdd_size[i];
		if( _bdd_nb[i] == 1 )
			start[i] = 0;
		else
		{
			start[i] = -tmp * REAL(.5);
			step[i] = tmp / (_bdd_nb[i]-1);
		}
		r[i] = _rot_speed[i] * t + _rot_origin[i];
		dr[i] = _rot_offset[i];
	}

	auto def = c_def_node::get_cur();
	if( !def->is_deforming() )
		def = nullptr;

	REAL	f[3];
	REAL*	p_u	= &f[i_u];
	REAL*	p_v = &f[i_v];
	REAL*	p_axe = &f[ i_axe ];

	*p_axe = start[2];
	for( INT32 ia = _bdd_nb[2]; ia > 0; --ia )
	{
		*p_v = start[1];
		for( INT32 iv = _bdd_nb[1]; iv > 0; --iv )
		{
			*p_u = start[0];
			for( INT32 iu = _bdd_nb[0]; iu > 0; --iu )
			{
				REAL	fbis[3];
				c_tex_anim::cur->bind_next();
				GOL::matrix::set_texture();
				GOL::matrix::push();
				GOL::matrix::set_modelview();
				GOL::matrix::push();
					add_v3( fbis, f, _center );
					if( def )
						def->apply( fbis, 1 );
					GOL::matrix::set_texture();
					GOL::matrix::translate(	fbis[i_u]   * _tex_offset[0] ,
									fbis[i_v]   * _tex_offset[1] ,
									fbis[i_axe] * _tex_offset[2] );	
					GOL::matrix::set_modelview();
					GOL::matrix::translate3v( fbis );

					GOL::matrix::rotate_xyzv( r );
					GOL::matrix::translate3v( _face_offset );
					GOL::call_list( get_render_list_multiple_name());
					add_v3( r, dr );
				GOL::matrix::set_texture();
				GOL::matrix::pop();
				GOL::matrix::set_modelview();
				GOL::matrix::pop();
				*p_u += step[0];
			}
			*p_v += step[1];
		}
		*p_axe += step[2]; 
	}
}

/*
#include "draw/color.h"
void	c_bdd_face::draw_3d(INT32 bdd_axe)
{
INT32	iu,iv,ia;
REAL	tmp;
REAL	f[3];
INT32	i_u;
INT32	i_v;
INT32	i_axe = bdd_axe;
		axe_build_index_vert( i_u, i_v, i_axe);
REAL*	p_u = &f[i_u];
REAL*	p_v = &f[i_v];
REAL*	p_axe = &f[ i_axe ];
REAL	t = aaa::time::get()*rot_factor;
	
REAL	start[3];
REAL	step[3];
REAL	r[3];
REAL	dr[3];

REAL	col[4];
	cpy_v4r( col, c_color::cur->get_color_pt() );


	c_multiple::cur->set_nb( bdd_nb[0], bdd_nb[1], bdd_nb[2] );

	for( iu=0; iu<3; ++iu )
		{
		tmp = bdd_size[iu];
		if( bdd_nb[iu] == 1 )
			start[iu] = 0;
		else
			{
			start[iu] = -tmp*.5;
			step[iu] = tmp / (bdd_nb[iu]-1);
			}
		r[iu] = rot_speed[iu]*t + rot_origin[iu];
		dr[iu] = rot_offset[iu];
		}

	auto def = c_def_node::get_cur();
	if( !def->is_deforming() )
		def = nullptr;

	*p_axe = start[2];
	for( ia = bdd_nb[2]; ia>0; --ia )
		{
		*p_v = start[1];
		for( iv = bdd_nb[1]; iv>0; --iv )
			{
			*p_u = start[0];
			for( iu = bdd_nb[0]; iu>0; --iu )
				{
				REAL tmp;
				INT32 n;
				n = (bdd_nb[0]+2)/2;
				tmp = REAL(n-ABS(iu-n))/REAL(n);
				n = (bdd_nb[1]+2)/2;
				tmp += REAL(n-ABS(iv-n))/REAL(n);
				tmp /= 2.;
				GOL::color4( col[0],col[1],col[2], col[3]*tmp ); 
				c_tex_anim::cur->bind_next();
				GOL::matrix::set_texture();
				GOL::matrix::push();	
				GOL::matrix::set_modelview();
				GOL::matrix::push();
					if (def)
						{
						REAL	fbis[3];
						cpy_v3r( fbis, f);
						def->apply( fbis, 1);
						GOL::matrix::set_texture();
						GOL::translate(	fbis[i_u]*tex_offset[0],
										fbis[i_v]*tex_offset[1],
										fbis[i_axe]*tex_offset[2] );	
						GOL::matrix::set_modelview();
						GOL::translatev( fbis);
						}
					else
						{
						GOL::matrix::set_texture();
						GOL::translate(	f[i_u]*tex_offset[0],
										f[i_v]*tex_offset[1],
										f[i_axe]*tex_offset[2] );
						GOL::matrix::set_modelview();
						GOL::translatev( f);
						}
					GOL::rotate_xyzv( r);
					GOL::translatev( face_offset);
					GOL::call_list( get_render_list_multiple_name());
					add_v3( r, dr);
				GOL::matrix::set_texture();
				GOL::matrix::pop();	
				GOL::matrix::set_modelview();
				GOL::matrix::pop();
				*p_u += step[0];
				}
			*p_v += step[1];
			}
		*p_axe += step[2];
		}
}
*/

void	c_bdd_face::draw_single()
{
}

//todoqq real deal with multiple
void	c_bdd_face::draw_multiple()
{
	scale_v3( _tex_offset, _tex_offset_ui, _tex_offset_ui[3] );
	_tex_offset[0] *= c_map::get_cur()->get_u();
	_tex_offset[1] *= c_map::get_cur()->get_v();

	if( _bdd_dimension > 0 )
	{
		//	if( c_multiple::cur->is_render_multiple() && c_multiple::cur->can_use_list() )
		if( c_multiple::cur->is_render_multiple() )
			multiple_compile_one( _face_size, _face_axe);
		//todo because normal is not updated
		//	else
		//		render_quad_normal_build( &normal[0][0], face_axe, bdd_size[0], false );
	}

	switch( _bdd_dimension )
	{
	case 0:	draw_0d();							break;
	case 1:	draw_1d( c_model::cur->get_axe() );	break;
	case 2:	draw_2d( c_model::cur->get_axe() );	break;
	case 3:	draw_3d( c_model::cur->get_axe() );	break;
	}
}
