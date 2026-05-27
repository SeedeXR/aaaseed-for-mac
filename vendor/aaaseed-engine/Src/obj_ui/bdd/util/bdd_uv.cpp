#include "bdd_uv.h"
#include "draw/colorrnd.h"
#include "draw/render.h"
#include "draw/model.h"
#include "bdd_util.h"
#include "obj_ui/deformer/def_node.h"
#include "draw/tex_anim.h"
#include "draw/guf.h"
#include "spy.h"


//todo deal more seriously with nb_axe > 0 by generating all the point instead of just moving the grid
// in particular normals are fucked up in this case

FACTORY_ABSTRACT_CREATE_V1( c_bdd_uv, bdd_uv, Bdd UV );


CONSTRUCTOR_ABSTRACT_CREATE(c_bdd_uv)
,_point						(nullptr)
,_point_def					(nullptr)
,_point_to_draw				(nullptr)
,_normal					(nullptr)
,_normal_def				(nullptr)
,_normal_to_draw			(nullptr)
,_uv						(nullptr)
,_index_strip				(nullptr)
,_index_hexa				(nullptr)

,_nb_point_allocated		(0)
,_nb_index_hexa_allocated	(0)
,_nb_u						(0)
,_nb_v						(0)
,_b_normal_compute_needed	(false)
,_turn_geo					(REAL(-45456.454))
{
	param_init();
}

c_bdd_uv::~c_bdd_uv()
{
	dealloc_geo();
}

void c_bdd_uv::param_init()
{
	_i_u	= 0;	//	we initialize it to avoid crashes on bdd which only draw
	_i_v	= 1;
	_i_axe	= 2;

	_nb_u	= 0;
	_nb_v	= 0;
	_nb_point_allocated	= 0;

	_nb_u_geo = 0;
	_nb_v_geo = 0;

//	_b_allow_along_u_ui = false;
	_b_field_ui = false;
//	_net_channel_ui = 0;
}

void c_bdd_uv::alloc_geo()
{
	if( _nb_u != _nb_u_ui || _nb_v != _nb_v_ui )
	{
		INT32	nb_point = _nb_u_ui * _nb_v_ui;
		INT32	nb_index_hexa = 2 * 3 * (_nb_u_ui-1) * (_nb_v_ui-1);

		if( _nb_point_allocated < nb_point || _nb_index_hexa_allocated < nb_index_hexa )
		{
			_point			= (REAL *)		REALLOC_ALIGNED( _point,		sizeof(REAL)	* 3 * nb_point );
			_point_def		= (REAL *)		REALLOC_ALIGNED( _point_def,	sizeof(REAL)	* 3 * nb_point );

			_normal			= (REAL *)		REALLOC_ALIGNED( _normal,		sizeof(REAL)	* 3 * nb_point );
			_normal_def		= (REAL *)		REALLOC_ALIGNED( _normal_def,	sizeof(REAL)	* 3 * nb_point );

			_uv				= (REAL *)		REALLOC_ALIGNED( _uv,			sizeof(REAL)	* 2 * nb_point );

			//todo deal with 8/16/32 bits index
			//	for index we allocate too much but it have to fit the case where we draw along u or v
			INT32 nb_index_strip = MAX( (_nb_u_ui-1)*_nb_v_ui, (_nb_v_ui-1)*_nb_u_ui ) * 2;
			_index_strip	= (UINT32 *)	REALLOC_ALIGNED( _index_strip,	sizeof(UINT32)	* nb_index_strip	);
			_index_hexa		= (UINT32 *)	REALLOC_ALIGNED( _index_hexa,	sizeof(UINT32)	* nb_index_hexa		);			

			if( _point && _point_def && _uv && _normal && _normal_def && _index_strip && ( _index_hexa || (nb_index_hexa==0) ) )
			{
				_nb_point_allocated			= nb_point;
				_nb_index_hexa_allocated	= nb_index_hexa;
			}
			else
			{
				box_err( "%s() didn't got the memory for %d x %d elts", __FUNCTION__, _nb_u_ui, _nb_v_ui );
				dealloc_geo();
			}
		}

		if( _nb_point_allocated > 0 )
		{
			_nb_u = _nb_u_ui;
			_nb_v = _nb_v_ui;
//			if( _b_allow_along_u_ui && _nb_u_ui > _nb_v_ui )
			if( _nb_u >= _nb_v )
				c_guf_index::build_index_strip_along_u(	_nb_u, _nb_v, _index_strip );
			else
				c_guf_index::build_index_strip_along_v(	_nb_u, _nb_v, _index_strip );
			//todo deal and alloc separately
			c_guf_index::build_index_for_hexa( _nb_u, _nb_v, _index_hexa );
		}
		else
		{
			_nb_u = 0;
			_nb_v = 0;
		}

		_map_last.force_change_state();
	}
}

void c_bdd_uv::dealloc_geo()
{
	FREE_ALIGNED_AND_NULL( _point );
	FREE_ALIGNED_AND_NULL( _point_def );

	FREE_ALIGNED_AND_NULL( _normal );
	FREE_ALIGNED_AND_NULL( _normal_def );

	FREE_ALIGNED_AND_NULL( _uv );

	FREE_ALIGNED_AND_NULL( _index_strip );
	FREE_ALIGNED_AND_NULL( _index_hexa );

	_nb_point_allocated = 0;
}

//todo	generalize
void c_bdd_uv::update_bdd_uv()
{
	alloc_geo();

	if( _b_axe_logic_vert )
		c_model::cur->get_axes_vert( _i_u, _i_v, _i_axe );
	else
		c_model::cur->get_axes( _i_u, _i_v, _i_axe );

	if( _map_last.is_changed( c_map::get_cur() ) )
	{
		c_bdd_uv* src_bdd = c_map::get_cur()->get_uv_src_bdd();
		if( src_bdd )
			src_bdd->generate_uv_from_pos( _uv, _nb_u_ui,_nb_v_ui, c_map::get_cur()->get_uv_src_bdd_dataset_id() );
		else	
			bdd_util_build_uv( _uv, _nb_u_ui,_nb_v_ui, false, false, true );
		_map_last.store_change( c_map::get_cur() );
	}

	if( c_multiple::cur )
		c_multiple::cur->set_nb( _nb_u_ui,_nb_v_ui );
}

void	c_bdd_uv::build_geo_validate()
{
	_i_axe_geo		= _i_axe;
	_nb_u_geo		= _nb_u_ui;
	_nb_v_geo		= _nb_v_ui;
	cpy_v3( _center_geo, _center_ui );
	c_model::cur->get_size_v3( _size_geo );
	_u_center_geo	= _u_center_ui;
	_u_range_geo	= _u_range_ui;
	_v_center_geo	= _v_center_ui;
	_v_range_geo	= _v_range_ui;
	_turn_geo		= _turn_ui;
	_b_normal_compute_needed = false;
}

bool	c_bdd_uv::build_geo_is_needed()
{	
	return	_i_axe_geo		!= _i_axe
		||	_nb_u_geo		!= _nb_u_ui
		||	_nb_v_geo		!= _nb_v_ui
		||	is_diff_v3( _center_geo, _center_ui )
		||	c_model::cur->is_diff_size_v3r( _size_geo )
		|| 	_u_center_geo	!= _u_center_ui
		||	_u_range_geo	!= _u_range_ui
		||	_v_center_geo	!= _v_center_ui
		||	_v_range_geo	!= _v_range_ui
		||	_turn_geo		!= _turn_ui;
}

REAL*	c_bdd_uv::compute_normals()
{
	if( _b_normal_compute_needed && _nb_u_geo > 1 && _nb_v_geo > 1 )	
	{	
		if( !_b_normal_compute_done )
		{
			INT32	offset = _nb_v_geo * 3;	//todo we should change order increment by u then v in the construction too 

			REAL*	nor = _normal_def;

			REAL*	p = _point_to_draw - offset;
			REAL*	c = _point_to_draw;
			REAL*	n = _point_to_draw + offset;

			compute_normals_cano_v3r( nor, n, c, c, _nb_v_geo, false );

			for( INT32 i=_nb_u_geo - 2; i > 0; --i )
			{
				nor += offset;
				p += offset;
				c += offset;
				n += offset;
				compute_normals_cano_v3r( nor, n, c, p, _nb_v_geo, false );
			}

			nor += offset;
			p += offset;
			c += offset;
			compute_normals_cano_v3r( nor, n, c, p, _nb_v_geo, false );

			_b_normal_compute_done = true;
		}
		return _normal_def;
	}
	return _normal;
}

//todo	deal with the fact that normals should be normalized in this case only
//todo draw in one call: use draw_arrays
void c_bdd_uv::draw_normal_point( REAL len )
{
	INT32	nb_elt = _nb_u_geo * _nb_v_geo;
	if( nb_elt > 0 )	//todo move to drawable
	{
		REAL*	pt = _point_to_draw;
		REAL*	nor = _normal_to_draw;
		REAL	vec[3];
		GOL::begin( GL_LINES );
			for( ; nb_elt > 0; --nb_elt )
			{
				GOL::vertex3v( pt );
				add_scale_v3( vec, pt, nor, len );
				GOL::vertex3v( vec );
				pt += 3;
				nor += 3;
			}
		GOL::end();
	}
}

void c_bdd_uv::draw_multiple_low()
{
	REAL*	c = _point_to_draw;

	c_tex_anim::cur->bind_this( 0 );
	if( c_multiple::cur->is_align_normal() && _nb_u_geo > 1 && _nb_v_geo > 1 )
	{
		REAL*	nor = _normal_to_draw;
		REAL*	p = _point_to_draw;
		REAL*	n = _point_to_draw + _nb_u_geo * 3;

		REAL	vec_u[3];
		REAL	vec_v[3];

		sub_v3(	vec_u,	c+3,	c );
		sub_v3(	vec_v,	n,		p );
		c_multiple::cur->draw_one_at_tgn( c, vec_u, vec_v, nor );
		p += 3;	c += 3;	n += 3;	nor += 3;
		c_multiple::cur->next_index_u_hack();
		c_tex_anim::cur->bind_next();

		for( INT32 iu = _nb_u_geo - 2; iu > 0; --iu )
		{
			sub_then_scale_v3r( vec_u, c + 3, c - 3, .5 );
			sub_v3( vec_v, n, p );		
			c_multiple::cur->draw_one_at_tgn( c, vec_u, vec_v, nor );
			p += 3; c += 3; n += 3;	nor += 3;
			c_multiple::cur->next_index_u_hack();
			c_tex_anim::cur->bind_next();
		}

		sub_v3( vec_u, c, c - 3 );
		sub_v3( vec_v, n, p );
		c_multiple::cur->draw_one_at_tgn( c, vec_u, vec_v, nor );


		p = _point_to_draw;
		c += 3; n += 3; nor += 3;
		c_multiple::cur->next_index_u_hack();
		c_tex_anim::cur->bind_next();

		c_multiple::cur->next_index_v_hack();

		for( INT32 iv = _nb_v_geo - 2; iv > 0; --iv )
		{
			sub_v3( vec_u, c + 3, c );
			sub_then_scale_v3r( vec_v, n, p, .5 );
			c_multiple::cur->draw_one_at_tgn( c, vec_u, vec_v, nor );
			p += 3; c += 3; n += 3; nor += 3;
			c_multiple::cur->next_index_u_hack();
			c_tex_anim::cur->bind_next();

			for( INT32 iu = _nb_u_geo - 2; iu > 0; --iu )
			{
				sub_then_scale_v3r( vec_u, c + 3, c - 3, .5 );
				sub_then_scale_v3r( vec_v, n, p, .5 );
				c_multiple::cur->draw_one_at_tgn( c, vec_u, vec_v, nor );
				p += 3; c += 3; n += 3; nor += 3;
				c_multiple::cur->next_index_u_hack();
				c_tex_anim::cur->bind_next();
			}

			sub_v3( vec_u, c, c - 3 );
			sub_then_scale_v3r( vec_v, n, p, .5 );			
			c_multiple::cur->draw_one_at_tgn( c, vec_u, vec_v, nor );
			p += 3; c += 3; n += 3; nor += 3;
			c_multiple::cur->next_index_u_hack();
			c_tex_anim::cur->bind_next();

			c_multiple::cur->next_index_v_hack();
		}

		n -= 3 * _nb_u_geo;

		sub_v3( vec_u, c + 3, c );
		sub_v3( vec_v, n, p );
		c_multiple::cur->draw_one_at_tgn( c, vec_u, vec_v, nor );
		p += 3; c += 3; n += 3; nor += 3;
		c_multiple::cur->next_index_u_hack();
		c_tex_anim::cur->bind_next();

		for( INT32 iu = _nb_u_geo - 2; iu > 0; --iu )
		{
			sub_then_scale_v3r( vec_u, c + 3, c - 3, .5 );
			sub_v3( vec_v, n, p );
			c_multiple::cur->draw_one_at_tgn( c, vec_u, vec_v, nor );
			p += 3; c += 3; n += 3; nor += 3;
			c_multiple::cur->next_index_u_hack();
			c_tex_anim::cur->bind_next();
		}

		sub_v3( vec_u, c, c - 3 );
		sub_v3( vec_v, n, p );
		c_multiple::cur->draw_one_at_tgn( c, vec_u, vec_v, nor );

		c_multiple::cur->next_index_u_hack();
		c_tex_anim::cur->bind_next();

		c_multiple::cur->next_index_v_hack();

	}
	else
	{
		//todoopt should I prefetch like this always ?
		REAL CONST * CONST size = c_multiple::cur->get_size();
		INT32 axe = c_multiple::cur->get_axe();

		for( INT32 v = _nb_v_geo; v > 0; --v )
		{
			for( INT32 u = _nb_u_geo; u > 0; --u )
			{
				c_multiple::cur->align_then_draw( c, size, axe );
				c += 3;
				c_multiple::cur->next_index_u_hack();
				c_tex_anim::cur->bind_next();
			}
			c_multiple::cur->next_index_v_hack();
		}
	}
}

//hack	this is unfinished
void c_bdd_uv::draw_multiple_field()
{
	REAL*	a = _point;
	REAL*	b = _point_to_draw;
	INT32	axe = c_multiple::cur->get_axe();
	REAL	size_axe = *( c_multiple::cur->get_size() + axe );
	REAL	size[3];
	cpy_v3( size, c_multiple::cur->get_size() );

	c_tex_anim::cur->bind_this( 0);
	if( c_multiple::cur->is_align_normal() )
	{
		REAL	nor[3];
		for( INT32 u = _nb_u_geo - 1; u >= 0; --u )
		{
			for( INT32 v = _nb_v_geo; v > 0; --v )
			{
				sub_v3( nor, b, a );
				//	size[axe] = size_axe * dist_v3r( a, b);
				c_multiple::cur->draw_one_at_nor( a, size, nor );
				a += 3;
				b += 3;
				c_multiple::cur->next_index_u_hack();
				c_tex_anim::cur->bind_next();
			}
			c_multiple::cur->next_index_v_hack();
		}
	}
	else
	{
		//todoopt should I prefetch like this always ?
//		REAL	size[3];
//		cpy_v3( size, c_multiple::cur->get_size() );
		for( INT32 u = _nb_u_geo - 1; u >= 0; --u )
		{
			for( INT32 v = _nb_v_geo; v > 0; --v )
			{
				size[axe] = size_axe * dist_v3r( a, b );
				c_multiple::cur->align_then_draw( a, size, axe );
				a += 3;
				b += 3;
				c_multiple::cur->next_index_u_hack();
				c_tex_anim::cur->bind_next();
			}
			c_multiple::cur->next_index_v_hack();
		}
	}	
}

void	c_bdd_uv::draw_multiple()
{
	if( _nb_u_geo > 0 && _nb_v_geo > 0 )	//todo should specialize draw_multiple_low for nb_u == 1 and nb_v == 1
	{
		if( _b_field_ui )
			draw_multiple_field();
		else
			draw_multiple_low();
	}
}

void c_bdd_uv::draw_single_low_old_school()
{
	bool   CONST use_normal = is_normal_draw();
	GLenum CONST primitive = c_render::get_cur()->get_draw_primitive();

	REAL*	p_point_r = _point_to_draw;
	REAL*	p_point_l = _point_to_draw + _nb_v_geo * 3;

	if( c_render::get_cur()->get_random_on_color() )
	{	
		c_color_random::begin();
		if( c_map::get_cur()->is_implicit() )
		{
			REAL* p_uv_r = _uv;
			REAL* p_uv_l = _uv + _nb_v_geo * 2;

			if( use_normal )
			{
				REAL* p_normal_r = _normal_to_draw;
				REAL* p_normal_l = _normal_to_draw + _nb_v_geo * 3;

				for( INT32 u = _nb_u_geo - 1; u > 0; --u )
				{
					c_color_random::set();
					c_tex_anim::cur->bind_this( u );
					GOL::begin( primitive );
						for( INT32 v = _nb_v_geo; v > 0; --v )
						{
							GOL::normal3v( p_normal_r );	p_normal_r += 3;
							GOL::texcoord2v( p_uv_r );		p_uv_r += 2;
							GOL::vertex3v( p_point_r );		p_point_r += 3;

							GOL::normal3v( p_normal_l );	p_normal_l += 3;
							GOL::texcoord2v( p_uv_l );		p_uv_l += 2;
							GOL::vertex3v( p_point_l );		p_point_l += 3;
						}
					GOL::end();
				}
			}
			else
			{
				for( INT32 u = _nb_u_geo - 1; u > 0; --u )
				{
					c_color_random::set();
					c_tex_anim::cur->bind_this( u);
					GOL::begin( primitive );
						for( INT32 v = _nb_v_geo; v > 0; --v )
						{
							GOL::texcoord2v( p_uv_r );		p_uv_r += 2;
							GOL::vertex3v( p_point_r );		p_point_r += 3;

							GOL::texcoord2v( p_uv_l );		p_uv_l += 2;
							GOL::vertex3v( p_point_l );		p_point_l += 3;
						}
					GOL::end();
				}
			}
		}
		else
		{
			if( use_normal )
			{
				REAL* p_normal_r = _normal_to_draw;
				REAL* p_normal_l = _normal_to_draw + _nb_v_geo * 3;

				for( INT32 u = _nb_u_geo - 1; u > 0; --u )
				{
					c_color_random::set();
					c_tex_anim::cur->bind_this( u );
					GOL::begin( primitive );
						for( INT32 v = _nb_v_geo; v > 0; --v )
						{
							GOL::normal3v( p_normal_r );	p_normal_r += 3;
							GOL::vertex3v( p_point_r );		p_point_r += 3;

							GOL::normal3v( p_normal_l );	p_normal_l += 3;
							GOL::vertex3v( p_point_l );		p_point_l += 3;
						}
					GOL::end();
				}
			}
			else
			{
				for( INT32 u = _nb_u_geo - 1; u > 0; --u )
				{
					c_color_random::set();
					c_tex_anim::cur->bind_this( u );
					GOL::begin( primitive );
						for( INT32 v = _nb_v_geo; v > 0; --v )
						{
							GOL::vertex3v( p_point_r );		p_point_r += 3;
							GOL::vertex3v( p_point_l );		p_point_l += 3;
						}
					GOL::end();
				}
			}
		}
		c_color_random::end();
	}
	else
	{	
		if( c_map::get_cur()->is_implicit() )
		{
			REAL* p_uv_r = _uv;
			REAL* p_uv_l = _uv + _nb_v_geo * 2;

			if( use_normal )
			{
				REAL* p_normal_r = _normal_to_draw;
				REAL* p_normal_l = _normal_to_draw + _nb_v_geo * 3;

				for( INT32 u = _nb_u_geo - 1; u > 0; --u )
				{
					c_tex_anim::cur->bind_this( u );
					GOL::begin( primitive );
						for( INT32 v = _nb_v_geo; v > 0; --v )
						{
							GOL::normal3v( p_normal_r );	p_normal_r += 3;
							GOL::texcoord2v( p_uv_r );		p_uv_r += 2;
							GOL::vertex3v( p_point_r );		p_point_r += 3;

							GOL::normal3v( p_normal_l );	p_normal_l += 3;
							GOL::texcoord2v( p_uv_l );		p_uv_l += 2;
							GOL::vertex3v( p_point_l );		p_point_l += 3;
						}
					GOL::end();
				}
			}
			else
			{
				for( INT32 u = _nb_u_geo - 1; u > 0; --u )
				{
					c_tex_anim::cur->bind_this( u );
					GOL::begin( primitive );
						for( INT32 v = _nb_v_geo; v > 0; --v )
						{
							GOL::texcoord2v( p_uv_r );		p_uv_r += 2;
							GOL::vertex3v( p_point_r );		p_point_r += 3;

							GOL::texcoord2v( p_uv_l );		p_uv_l += 2;
							GOL::vertex3v( p_point_l );		p_point_l += 3;
						}
					GOL::end();
				}
			}
		}
		else
		{
			if( use_normal )
			{
				REAL* p_normal_r = _normal_to_draw;
				REAL* p_normal_l = _normal_to_draw + _nb_v_geo * 3;

				for( INT32 u = _nb_u_geo - 1; u > 0; --u )
				{
					c_tex_anim::cur->bind_this( u );
					GOL::begin( primitive );
						for( INT32 v = _nb_v_geo; v > 0; --v )
						{
							GOL::normal3v( p_normal_r );	p_normal_r += 3;
							GOL::vertex3v( p_point_r );		p_point_r += 3;

							GOL::normal3v( p_normal_l );	p_normal_l += 3;
							GOL::vertex3v( p_point_l );		p_point_l += 3;
						}
					GOL::end();
				}
			}
			else
			{
				for( INT32 u = _nb_u_geo - 1; u > 0; --u )
				{
					c_tex_anim::cur->bind_this( u );
					GOL::begin( primitive );
						for( INT32 v = _nb_v_geo; v > 0; --v )
						{
							GOL::vertex3v( p_point_r );		p_point_r += 3;
							GOL::vertex3v( p_point_l );		p_point_l += 3;
						}
					GOL::end();
				}
			}
		}
	}
}


void c_bdd_uv::draw_single_low()
{
	if( !GOL::b_draw_avoid_vertex_use )
	{
		draw_single_low_old_school();
		return;
	}

	SPY_PUSH_RANGE( "bdd_uv", spy::RENDER );
	REAL*	p_uv		= c_map::get_cur()->is_implicit() ? _uv : nullptr ;
	REAL*	p_normal	= is_normal_draw() ? _normal_to_draw : nullptr ;

	//DBG_PRINT_STRING( "\tbind_vao(0)" );
	GOL::unbind_vao_secu();
	//DBG_PRINT_STRING( "\tdraw_before_static(0)" );
	c_guf::draw_before_static( _point_to_draw, p_normal, p_uv );
	GLenum CONST primitive = c_render::get_cur()->get_draw_primitive();
	UINT32 nb; 
	UINT32 max = _nb_v_geo - 1; 

	if( c_map::get_cur()->is_tex_hexa() )
	{
		nb = (_nb_u_geo-1) * 6;
		max = _nb_v_geo - 1;
		if( c_tex_anim::cur->is_tex_several() && primitive != GL_TRIANGLES )
		{
			for( UINT32 iv=0; iv < max; ++iv )
			{
				c_tex_anim::cur->bind_this_w( iv, _nb_u_geo );
				//DBG_PRINT_STRING( "\tglDrawElements( %d ) hexa", iu );
				GOL::draw_elements( GL_TRIANGLES, nb, GL_UNSIGNED_INT, _index_hexa + iv*nb );
			}
		}
		else
		{
			GOL::draw_elements( GL_TRIANGLES, nb*max, GL_UNSIGNED_INT, _index_hexa );
		}
	}
	else
	{	
		if( c_render::get_cur()->get_random_on_color() )
		{	
			c_color_random::begin();
			nb = _nb_v_geo * 2;
			for( UINT32 iu=0; iu < max; ++iu )
			{
				c_color_random::set();
				c_tex_anim::cur->bind_this_w( iu, _nb_u_geo );
				//DBG_PRINT_STRING( "\tglDrawElements( %d ) random", iu );
				GOL::draw_elements( primitive, nb, GL_UNSIGNED_INT, _index_strip + iu * nb );
			}
			c_color_random::end();
		}
		//todo deal with GL_TRIANGLES case to draw in one call
	//	else if( _b_allow_along_u_ui && _nb_u_geo > _nb_v_geo )
		else if( _draw_mode_ui != DRAW_MODE::REGULAR )
		{
			switch( _draw_mode_ui )
			{
			case DRAW_MODE::POINT:
				GOL::draw_arrays( GL_POINTS, _nb_u_geo * _nb_v_geo );
				break;
			case DRAW_MODE::LINE_U:
			default:
				nb = _nb_u_geo;
				for( INT32 iv = 0; iv < _nb_v_geo; ++iv )
					GOL::draw_arrays( GL_LINE_STRIP, nb, iv*nb );
				break;
			}
		}
		else if( _nb_u_geo >= _nb_v_geo )
		{
			max = _nb_v_geo - 1; 
			if( max <= 0 )
			{	// one line of point : no strip
				GOL::draw_arrays( primitive, _nb_u_geo );
			}
			else
			{
				nb = _nb_u_geo * 2;
				for( UINT32 iv = 0; iv < max; ++iv )
				{
					//DBG_PRINT_STRING( "\tbind_this_w( %d )", iu );
					//todo
					//c_tex_anim::cur->bind_this_w( iu, _nb_u );
					GOL::draw_elements( primitive, nb, GL_UNSIGNED_INT, _index_strip + iv * nb );
				}
			}
		}
		else
		{
			max = _nb_u_geo - 1; 
			if( max <= 0 )
			{	// one line of point : no strip
				GOL::draw_arrays( primitive, _nb_v_geo );
			}
			else
			{
				nb = _nb_v_geo * 2;
				for( UINT32 iu = 0; iu < max; ++iu )
				{
					//DBG_PRINT_STRING( "\tbind_this_w( %d )", iu );
					c_tex_anim::cur->bind_this_w( iu, _nb_u_geo );
#if 0
					DBG_PRINT_STRING( "%d - glDrawElements( %d )", iu, nb);
					for (INT32 i = 0; i < nb / 2; ++i)
					{
						INT32* p = _index + iu * nb + i * 2;
						INT32 i1 = *p;
						INT32 i2 = *(p + 1);
						DBG_PRINT_STRING("\t%d - %d\t%d", i, i1, i2);
						DBG_PRINT_STRING("\t\t\t %g\t%g\t%g", *(_point_to_draw + i1 * 3), *(_point_to_draw + i1 * 3 + 1)), *(_point_to_draw + i1 * 3 + 2) );
						DBG_PRINT_STRING("\t\t\t\t\t\t %g\t%g", *(_uv + i1 * 2), *(_uv + i1 * 2 + 1));
						DBG_PRINT_STRING("\t\t\t %g\t%g\t%g", *(_point_to_draw + i2 * 3), *(_point_to_draw + i2 * 3 + 1), *(_point_to_draw + i2 * 3 + 2));
						DBG_PRINT_STRING("\t\t\t\t\t\t %g\t%g", *(_uv + i2 * 2), *(_uv + i2 * 2 + 1));
					}
#endif
					GOL::draw_elements( primitive, nb, GL_UNSIGNED_INT, _index_strip + iu * nb );
				}
			}
		}
	}
	//DBG_PRINT_STRING( "\tdraw_after_static()" );
	c_guf::draw_after_static();
	SPY_POP_RANGE();
}

void c_bdd_uv::draw_single_field()
{
	REAL*	a = _point;
	REAL*	b = _point_to_draw;

	if( c_render::get_cur()->get_random_on_color() )
	{	
		c_color_random::begin();
		if( c_map::get_cur()->is_implicit() )
		{
			REAL*	p_uv = _uv;
			for( INT32 u = _nb_u_geo; u > 0; --u )
			{
				c_color_random::set();
				c_tex_anim::cur->bind_this( u );
				GOL::begin( GL_LINES );
				for( INT32 v = _nb_v_geo; v > 0; --v )
				{
					GOL::texcoord2v( p_uv );	p_uv += 2;
					GOL::vertex3v( a );			a += 3;
					GOL::vertex3v( b );			b += 3;
				}
				GOL::end();
			}
		}
		else
		{
			for( INT32 u = _nb_u_geo; u > 0; --u )
			{
				c_color_random::set();
				c_tex_anim::cur->bind_this( u );
				GOL::begin( GL_LINES );
				for( INT32 v = _nb_v_geo; v > 0; --v )
				{
					GOL::vertex3v( a );			a += 3;
					GOL::vertex3v( b );			b += 3;
				}
				GOL::end();
			}
		}
		c_color_random::end();
	}
	else
	{	
		if( c_map::get_cur()->is_implicit() )
		{
			REAL	*p_uv = _uv;
			for( INT32 u = _nb_u_geo; u > 0; --u )
			{
				c_tex_anim::cur->bind_this( u );
				GOL::begin( GL_LINES );
				for( INT32 v = _nb_v_geo; v > 0; --v )
				{
					GOL::texcoord2v( p_uv );	p_uv += 2;
					GOL::vertex3v( a );			a += 3;
					GOL::vertex3v( b );			b += 3;
				}
				GOL::end();
			}
		}
		else
		{
			for( INT32 u = _nb_u_geo; u > 0; --u )
			{
				c_tex_anim::cur->bind_this( u );
				GOL::begin( GL_LINES );
				for( INT32 v = _nb_v_geo; v > 0; --v )
				{
					GOL::vertex3v( a );			a += 3;
					GOL::vertex3v( b );			b += 3;
				}
				GOL::end();
			}
		}
	}
}

void	c_bdd_uv::draw_single()
{
	INT32 nb_elt = _nb_u_geo * _nb_v_geo;
	if( nb_elt > 1 )	//todo should specialize draw_single_low for nb_u == 1 and nb_v == 1
	{
		if( _b_field_ui )
			draw_single_field();
		else
			draw_single_low();
	}
}


void	c_bdd_uv::do_deform_and_normal()
{
	auto def = c_def_node::get_cur();
	if( def->is_deforming() )
	{
		def->apply( _point_def, _point, _nb_u_geo * _nb_v_geo );
		_point_to_draw = _point_def;
		_b_normal_compute_needed = true;
		_b_normal_compute_done = false;
	}
	else
	{
		_point_to_draw = _point;
		_normal_to_draw = _normal;
	}

	if( is_normal_needed() )
		_normal_to_draw = compute_normals();
}

void	c_bdd_uv::update()
{
//	if( _net_channel_ui && net && net->is_slave() )
//		net_receive_param( _net_channel, get_param_nb() );

	update_bdd_uv();
	if(	build_geo_is_needed() )
		build_geo();
	do_deform_and_normal();

//	if( _net_channel_ui && net && net->is_master() )
//		net_send_param( _net_channel, get_param_nb() );
}

INT32	c_bdd_uv::get_point_nb()
{
	return _nb_u_geo * _nb_v_geo;
}

REAL*	c_bdd_uv::get_point_pt( INT32 CONST index )
{
	return _point_to_draw + IMOD( index, _nb_u_geo * _nb_v_geo ) * 3;
}
bool	c_bdd_uv::set_point(	INT32 CONST index, REAL CONST * CONST src )
{	
	REAL * CONST pt = get_point_pt(index);
	if( pt )
	{
		cpy_v3( pt, src );
		_b_normal_compute_needed = true;
		_b_normal_compute_done = false;
		return true;
	}
	return false;
}

REAL*	c_bdd_uv::get_points()
{
	return _point_to_draw;
}

// perhaps we can manage to do a default one here
void	c_bdd_uv::get_point_from_uv( REAL* dst, REAL u, REAL v )
{
	err_print_method_unimplemented( __FUNCTION__ );
	clear_v3( dst );
}

void	c_bdd_uv::generate_uv_from_pos( REAL* uv, INT32 CONST nb_u, INT32 nb_v, INT32 CONST dataset_id )
{
	REAL	u_start;
	REAL	v_start;
	REAL	du, dv;

	//	u_start = c_map::cur->get_u_max();
	//	du = c_map::cur->get_u_min() - u_start;
	u_start = 0.;
	du = OVER_ONE_AS_REAL(nb_u-1);

	//	v_start = c_map::cur->get_v_min();
	//	dv = c_map::cur->get_v_max() - v_start;
	v_start = 0.;
	dv = OVER_ONE_AS_REAL(nb_v-1);

	REAL v = v_start;
	for( nb_v; nb_v > 0; --nb_v )
	{
		REAL u = u_start;
		for( INT32 i_u = nb_u; i_u > 0; --i_u )
		{
			REAL vec[3];
			get_point_from_uv( vec, u,v );
			*uv++ = vec[0];		
			*uv++ = vec[1];
			u += du;
		}
		v += dv;
	}
}

FACTORY_ABSTRACT_CREATE_V1(c_bdd_uvw, bdd_uvw, Bdd UVW);

CONSTRUCTOR_ABSTRACT_CREATE(c_bdd_uvw)
	,_nb_axe_ui(1)
{
	clear_v3(_size);
	param_init();
}

c_bdd_uvw::~c_bdd_uvw()
{
}

void	c_bdd_uv::param_set_pt_center(	INT32& h )
{
	param_set_pt_3( h, _center_ui );	
}
void	c_bdd_uv::param_set_pt_nb(		INT32& h )
{
	param_set_pt(	h, _nb_u_ui );
	param_set_pt(	h, _nb_v_ui );
}
void	c_bdd_uv::param_set_pt_nb_axe(	INT32& h )
{
	param_set_pt_nb(	h	);
	param_set_pt(	h,	_nb_axe_ui );
}
void	c_bdd_uv::param_set_pt_uv(		INT32& h )
{
	param_set_pt(	h, _draw_mode_ui );
	param_set_pt(	h, _u_center_ui	);
	param_set_pt(	h, _u_range_ui	);
	param_set_pt(	h, _v_center_ui );
	param_set_pt(	h, _v_range_ui	);
}
void	c_bdd_uv::param_set_pt_uv_turn(	INT32& h )
{
	param_set_pt_uv(	h );
	param_set_pt(	h, _turn_ui	);
}

void c_bdd_uvw::set_axe_coor( REAL coor )
{
	REAL* p_point = _point + _i_axe;
	for( INT32 i = _nb_u_ui * _nb_v_ui; i > 0; --i )
	{
		*p_point = coor;
		p_point += 3;
	}
}

void c_bdd_uvw::update_uvw()
{
	c_model::cur->get_size_v3( _size );
	update_bdd_uv();
}

void c_bdd_uvw::draw_instanced()
{
	//DBG_PRINT_STRING( "\tbind_vao(0)" );
	GOL::unbind_vao_secu();
	//DBG_PRINT_STRING( "\tdraw_before_static(0)" );
	c_guf::draw_before_static( _point_to_draw, nullptr, nullptr );

		//DBG_PRINT_STRING( "\tbind_this_w( %d )", iu );
		//c_tex_anim::cur->bind_this_w( iu, _nb_u );
		SPY_PUSH_RANGE( "bdd_uv", spy::RENDER );
			GOL::draw_arrays_instanced( c_render::get_cur()->get_draw_primitive(), _nb_v_geo, _nb_axe_ui );
			//glDrawArrays( GL_LINE_STRIP, 0, _nb_v );
		SPY_POP_RANGE();

	//DBG_PRINT_STRING( "\tdraw_after_static()" );
	c_guf::draw_after_static();
}

void c_bdd_uvw::draw()
{
	if( _nb_u_geo * _nb_v_geo <= 0 )
		return;

	{
		//CHAR* str = get_my_filename();
		//DBG_PRINT_STRING( "\tdraw %d, %s", _nb_elt, (str&&*str) ? str : "no filename" );
	}

	if( c_multiple::cur->is_render_multiple() )
	{
		if( !c_multiple::cur->begin() )
			goto exit;
		c_multiple::cur->init_index_w();
	}

	if( _nb_axe_ui != 1 && _nb_u_geo == 1 )
	{
		//DBG_PRINT_STRING( "\tdraw_w(0)" );
		// deformation in this case don't have a lot of sense but not a big cost 
		do_deform_and_normal();
		draw_instanced();
	}
	else
	{
		if( _nb_axe_ui != 1 )
		{
			REAL z = -_size[2] * REAL(.5) + _center_ui[_i_axe];
			REAL dz = _nb_axe_ui != 1 ? _size[2] / REAL(_nb_axe_ui - 1) : REAL(0);
			for( INT32 iw = 0; iw < _nb_axe_ui; ++iw )
			{
				//DBG_PRINT_STRING( "\tdraw_w( %d )", iw );
				set_axe_coor(z);
				do_deform_and_normal();
				draw_w(iw);
				z += dz;
			}
			set_axe_coor( _center_ui[_i_axe] );	//todo avoid this with more test at if ( build_geo_is_needed
		}
		else
		{
			draw_w(0);
		}
		c_tex_anim::cur->set_iw(0);
	}
	if( c_multiple::cur->is_render_multiple() )
exit:
		c_multiple::cur->end();
}
