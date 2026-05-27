#include "bdd_voronoi.h"
#include "obj_ui/bdd/bdd_point/bdd_boid.h"
#include "gol/gol.h"
#include "math/voronoi_diagram_generator.h"
#include "draw/model.h"
#include "ui/alphabet.h"
#include "draw/tex_anim.h"
#include "draw/guf.h"
#include "draw/line.h"
#include "image/bind_img_2d.h"
#include "draw/color.h"
#include "spy.h"


FACTORY_CREATE_PROP_V1( c_bdd_voronoi, bdd_voronoi, Voronoi, bdd_voronoi, sub_menu="Procedural"; );

using namespace voronoi;

namespace n_bdd_voronoi
{
	CONSTEXPR INT32	BASE_PARAM_NB		=	18	+	c_bdd::GEO_PARAM_NB;
	CONSTEXPR INT32	SELECT_PARAM_NB		=	10;
	CONSTEXPR INT32	VORONOI_PARAM_NB	=	23;
	CONSTEXPR INT32	DELAUNAY_PARAM_NB	=	8;
	CONSTEXPR INT32	PROCESS_PARAM_NB	=	12;
	CONSTEXPR INT32	GROUP_NB			=	4;
	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
									+	PROCESS_PARAM_NB
									+	SELECT_PARAM_NB
									+	VORONOI_PARAM_NB
									+	DELAUNAY_PARAM_NB
									+	GROUP_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_BASE_PARAMS
		PARAM_DEF_REF(			bdd_src )
		PARAM_DEF_AXE_Z(		bdd_src_axe )
		PARAM_DEF_POINT_XYZ(	center )

		PARAM_DEF_GROUP_CLOSED( Select, SELECT_PARAM_NB )
// index_begin / index_end should be checked and refined
			PARAM_DEF_INT32_POS(	index_begin,	128,	0			)
			PARAM_DEF_INT32_POS(	index_end,		128,	1024*1024	)
			PARAM_DEF_REAL_INF(		limit_min_u,	0,		-4			)
			PARAM_DEF_REAL_INF(		limit_min_v,	0,		-4			)
			PARAM_DEF_REAL_INF(		limit_min_axe,	0,		-4			)
			PARAM_DEF_REAL_INF(		limit_max_u,	0,		4			)
			PARAM_DEF_REAL_INF(		limit_max_v,	0,		4			)
			PARAM_DEF_REAL_INF(		limit_max_axe,	0,		4			)
			PARAM_DEF_REAL(			poly_area_min,	.1,	0,		0,	256	)
			PARAM_DEF_REAL(			poly_area_max,	1.,	256,	0,	256	)

		PARAM_DEF_REAL_ONE_ZERO(	dist_min				)

		PARAM_DEF_REAL_ONE(		scale_axe				)

		PARAM_DEF_BOOL_ON(		draw_boundary			)

		PARAM_DEF_BOOL_OFF(		draw_site				)
		PARAM_DEF_REAL_INF(		draw_site_size,			1., .1	)
		PARAM_DEF_BOOL_OFF(		draw_site_number		)
		PARAM_DEF_REAL_INF(		draw_site_number_size,	1., .1	)
		PARAM_DEF_BOOL_OFF(		delaunay				)

		PARAM_DEF_GROUP_CLOSED( Delaunay More, DELAUNAY_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		delaunay_draw_edge_number		)
			PARAM_DEF_BOOL_OFF(		delaunay_draw_edge_orientation	)
			PARAM_DEF_REAL(			delaunay_draw_line_size,		.1, 1,	0.001, PARAM_MAX_REAL		)
			PARAM_DEF_COLOR_RGBGA(	delaunay_draw_color				)
	
		PARAM_DEF_BOOL_OFF(		voronoi					)

		PARAM_DEF_GROUP_CLOSED( Voronoi More, VORONOI_PARAM_NB )
			PARAM_DEF_BOOL_OFF(			voronoi_draw_vertice			)
			PARAM_DEF_BOOL_ON(			voronoi_draw_edge				)
			PARAM_DEF_BOOL_ON(			voronoi_draw_poly				)
			PARAM_DEF_BOOL_OFF(			voronoi_draw_poly_before		)
			PARAM_DEF_REAL_ONE(			voronoi_draw_poly_size			)
			PARAM_DEF_REAL_ZERO(		voronoi_draw_poly_size_barycenter	)

			PARAM_DEF_BOOL_OFF(			voronoi_draw_poly_as_triangle	)
			PARAM_DEF_BOOL_OFF(			voronoi_draw_triangle			)
			PARAM_DEF_REAL(				voronoi_draw_poly_line_size,	.1, 1,	0.001, PARAM_MAX_REAL )
			PARAM_DEF_COLOR_RGBGA(		voronoi_draw_poly_color			)

			PARAM_DEF_BOOL_OFF(				image_color_use					)
			PARAM_DEF_REAL_ZERO_ONE(		image_color_alpha_thereshold	)
			PARAM_DEF_BOOL_OFF(				image_color_by_triangle			)
			PARAM_DEF_BOOL_OFF(				image_color_by_point			)
			PARAM_DEF_BIND_2D_CURRENT_SEL(	image_color_bind				)
			PARAM_DEF_BOOL_ON(				image_color_clamped				)
			PARAM_DEF_SCALE_UV(				image_color_size				)
			PARAM_DEF_REAL_ZERO(			image_color_z_influence			)
	
		PARAM_DEF_BOOL_OFF(		draw_edge_link			)
		PARAM_DEF_BOOL_OFF(		draw_vertex				)
		PARAM_DEF_BOOL_OFF(		draw_EL					)

		PARAM_DEF_GROUP_CLOSED( Process, PROCESS_PARAM_NB )
			PARAM_DEF_INT32_POS(	site_index_begin,		128, 0			)
			PARAM_DEF_INT32_POS(	site_index_end,			128, 1024*1024	)
			PARAM_DEF_INT32_LOCKED(	site_submited_nb		)
			PARAM_DEF_INT32_LOCKED(	site_unique_nb			)
			PARAM_DEF_REAL(			EL_hash_factor,			2, 1,						1., 1000000 )
			PARAM_DEF_REAL(			PQ_hash_factor,			2, 1,						1., 1000000 )
			PARAM_DEF_DOUBLE(		precision,				0.000001, 1./(1024*64),		0., 100.	)
			PARAM_DEF_BOOL_OFF(		verbose					)
			PARAM_DEF_BOOL_OFF(		escape					)
			PARAM_DEF_INT32_POS(	escape_nb_site,			1, 0			)
			PARAM_DEF_INT32_POS(	escape_nb_edge,			1, 0			)
			PARAM_DEF_INT32_POS(	escape_nb_half_edge,	1, 0			)

		PARAM_DEF_BOOL_OFF(		save_trig					)
	};
}

void	c_bdd_voronoi::param_init_pt()
{
	INT32	h = param_init_pt_geo();

	param_set_pt_attach_obj(	h,	_target_name_symbo,		_bdd_target_src		);
	param_set_pt(				h,	_target_axe				);
	param_set_pt_3(				h,	_center					);

	++h;
		param_set_pt(	h,	_index_begin				);
		param_set_pt(	h,	_index_end					);
		param_set_pt_3(	h,	_limit_min_ui				);
		param_set_pt_3(	h,	_limit_max_ui				);
		param_set_pt(	h,	_voronoi_poly_area_min_ui	);
		param_set_pt(	h,	_voronoi_poly_area_max_ui	);

	param_set_pt(		h,	_dist_min				);

	param_set_pt(		h,	_scale_axe				);

	param_set_pt(		h,	_b_draw_boundary		);

	param_set_pt(		h,	_b_site_src				);
	param_set_pt(		h,	_site_size_ui			);
	param_set_pt(		h,	_b_site_src_number		);
	param_set_pt(		h,	_number_size_ui			);

	param_set_pt(		h,	_b_delaunay_ui			);

	++h;
		param_set_pt(		h,	_b_delaunay_edge_number_ui		);
		param_set_pt(		h,	_b_delaunay_orientation_ui		);
		param_set_pt(		h,	_delaunay_line_size				);
		param_set_pt_rgbfa(	h,	_delaunay_color_ui				);

	param_set_pt(		h,	_b_voronoi_ui			);

	++h;
		param_set_pt(		h,	_b_vertice						);
		param_set_pt(		h,	_b_edge							);
		param_set_pt(		h,	_b_voronoi_poly					);
		param_set_pt(		h,	_b_voronoi_poly_before			);
		param_set_pt(		h,	_voronoi_poly_size				);
		param_set_pt(		h,	_voronoi_poly_size_barycenter	);
		param_set_pt(		h,	_b_voronoi_poly_as_triangle		);
		param_set_pt(		h,	_b_voronoi_triangle				);
		param_set_pt(		h,	_voronoi_poly_line_size			);
		param_set_pt_rgbfa(	h,	_voronoi_poly_color_ui			);
		param_set_pt(		h,	_b_img_color_use				);
		param_set_pt(		h,	_voronoi_draw_alpha_threshold	);
		param_set_pt(		h,	_b_img_color_by_triangle		);
		param_set_pt(		h,	_b_img_color_by_point			);
		param_set_pt(		h,	_s_image_color_bind				);
		param_set_pt(		h,	_b_img_color_clamped_ui			);
		param_set_pt_2(		h,	_img_color_size					);
		param_set_pt(		h,	_img_color_z_influence			);


	param_set_pt(		h,	_b_edge_link_ui			);
	param_set_pt(		h,	_b_vertex				);
	param_set_pt(		h,	_b_EL					);

	++h;
		param_set_pt(		h,	_vdg->_site_index_begin			);
		param_set_pt(		h,	_vdg->_site_index_end			);
		param_set_pt(		h,	_point_nb						);
		param_set_pt(		h,	_vdg->_site_nb					);
		param_set_pt(		h,	_vdg->_EL_hash_factor			);
		param_set_pt(		h,	_vdg->_PQ_hash_factor			);
		param_set_pt(		h,	_vdg->_precision				);
		param_set_pt(		h,	_vdg->_b_verbose				);
		param_set_pt(		h,	_vdg->_b_escape					);
		param_set_pt(		h,	_escape_nb_site					);
		param_set_pt(		h,	_escape_nb_edge					);
		param_set_pt(		h,	_escape_nb_half_edge			);

	param_set_pt(		h,	_b_save_poly_trig_ui	);

	err_param_init_pt(h);
}

void	c_bdd_voronoi::init()
{
	_vdg = new voronoi::generator;
	param_init_with( n_bdd_voronoi::param, n_bdd_voronoi::PARAM_NB_MAX);
}

bool	c_bdd_voronoi::alloc_point( INT32 point_nb )
{
	if( _point_nb_allocated < point_nb )
	{
		_point = (REAL*) REALLOC( _point, point_nb * sizeof(REAL) * 3 );
		if( !_point )
		{
			_point_nb_allocated = 0;
			return false;
		}
		_point_nb_allocated = _point_nb;
	}
	return true;
}

void	c_bdd_voronoi::dealloc_point()
{
	FREE_AND_NULL( _point );
}

void	c_bdd_voronoi::alloc()
{
}

void	c_bdd_voronoi::dealloc()
{
	SAFE_DELETE(_vdg );
	dealloc_point();
}

CONSTRUCTOR_CREATE(c_bdd_voronoi)
,_bdd_target_src(nullptr)
,_point(nullptr)
,_point_nb(0)
,_point_nb_allocated(0)
,_vdg(nullptr)
{
	init();
	alloc();
}

c_bdd_voronoi::~c_bdd_voronoi()
{
	dealloc();
}

//void	c_bdd_voronoi::restart()
//{
////	_b_restart_trig_ui = true;	
//}

void	c_bdd_voronoi::update()
{
	_bdd_target_src = update_bdd_target( _target_name_symbo, _bdd_target_src );

	INT32	nb	= 0;
	if( _bdd_target_src )
	{	//we import from a bdd
		c_bdd*	bdd = _bdd_target_src;

		INT32 _point_nb	=	MIN( bdd->get_point_nb(), _index_end+1 );
		if( _point_nb <= 0 )
			return;

		if( !alloc_point( _point_nb ) )
			return;

		if( !_bdd_target_src->get_points_3d( _point, _point_nb ) )
			return;

		SPY_PUSH_RANGE( "Voronoi Import", spy::COL_1 );
			INT32	i_u, i_v;	
			axe_build_index( i_u, i_v, _target_axe );

			REAL*	dst			= _point;
			REAL*	src			= _point + _index_begin*3;
			INT32	index		= _index_begin;
			INT32	index_end	= _index_begin + _point_nb;
			--index;
			while( ++index < index_end )
			{
				REAL v[3];
				v[0] = *(src+i_u);
				v[1] = *(src+i_v);
				v[2] = *(src+_target_axe);
				src += 3;

				if(	is_inside_v3( v, _limit_min_ui, _limit_max_ui ) )
				{
					cpy_v3( dst, v );
					dst += 3;
					++nb;
				}
			}
		SPY_POP_RANGE();
	}
	else
	{	// add mode where we set it directly
		return;
	}

	REAL	size[3];
	c_model::cur->get_size_v3( size );
	scale_v2( size, REAL(.5) );

	FP32 CONST * col = c_color::get_cur()->get_color_pt();
	scale_v3_cpy_v4(	_voronoi_poly_color,	_voronoi_poly_color_ui	);
	mul_v4(			_voronoi_poly_color,	col );

	scale_v3_cpy_v4(	_delaunay_color,		_delaunay_color_ui		);
	mul_v4(			_delaunay_color,		col );

	_vdg->set_escape_nb_site(		_escape_nb_site			);
	_vdg->set_escape_nb_edge(		_escape_nb_edge			);
	_vdg->set_escape_nb_half_edge(	_escape_nb_half_edge	);

	_vdg->set_generate_voronoi(		_b_voronoi_ui				);
	_vdg->set_generate_delaunay(	_b_delaunay_ui				);

	SPY_PUSH_RANGE( "Voronoi Generate", spy::COL_2 );
		_vdg->generate_voronoi( _point, nb, _center[0]-size[0], _center[0]+size[0], _center[1]-size[1], _center[1]+size[1], _dist_min, _b_edge_link_ui );
	SPY_POP_RANGE();
	
	SPY_PUSH_RANGE( "Voronoi Preparate", spy::COL_3 );
		prepare_poly();
	SPY_POP_RANGE();

	if( _b_save_poly_trig_ui )
	{
		save_poly();
		_b_save_poly_trig_ui = false;
	}
/*
	//	deal with restart
	if( _delta_t.update() )
	{
		DBG_PRINT_STRING( "boid time restart" );
		restart();
	}
	if( _b_restart_trig_ui )
	{
		_b_restart_trig_ui = false;
	}
*/
}


void	c_bdd_voronoi::prepare_poly()
{
//	REAL inter		= _voronoi_poly_size;
	REAL inter_bary	= _voronoi_poly_size_barycenter;

	bool CONST b_area = (_voronoi_poly_area_min_ui != 0.) || (_voronoi_poly_area_max_ui != 256.);

	c_img_2d*	img_map = nullptr;
	if( _b_img_color_use || _img_color_z_influence > 0. )
	{
		img_map = g_bind_img_2d->get_ready( _s_image_color_bind );
		img_map = ( img_map->is_ok() && img_map->is_data_valid(__FUNCTION__) ) ? img_map : nullptr;
	}

	bool	b_draw_cell = !img_map || !(_b_img_color_by_triangle || _b_img_color_by_point );
	REAL	su = OVER_ONE_AS_REAL( _img_color_size[0] );
	REAL	sv = OVER_ONE_AS_REAL( _img_color_size[1] );

	//todoopt make it parallel
	INT32 nb_site = _vdg->_site_nb;
	struct Site* s = _vdg->get_sites_src();
	for( INT32 i=0; i<nb_site; ++i, ++s )
	{
		s->b_use = false;
		if( !_b_draw_boundary && s->b_boundary )
			continue;

		std::vector<struct PolygonPoint> CONST * CONST	pl = s->point_list;

		INT32 CONST nb_point = (INT32)pl->size();
		if( nb_point == 0 )
			continue;

		REAL*	c = s->bary;
		set_v3( c, s->x, s->y, s->z * _scale_axe );
		_vdg->sort_voronoi_site( s );

		if( b_area )
		{
			REAL area = 0.0f;
			struct Site* p = (*pl)[0].site;
			struct Site* p2;
			for( INT32 j=0; j<nb_point-1; ++j )
			{
				p2 = (*pl)[j+1].site;
				area += REAL(p->x * p2->y - p->y * p2->x);
				p = p2;
			}
			p2 = (*pl)[0].site;
			area += REAL(p->x * p2->y - p->y * p2->x);
			area *= REAL(.5);
			if( !INSIDE( area, _voronoi_poly_area_min_ui, _voronoi_poly_area_max_ui ) )	
				continue;
		}


		if( inter_bary != 0. )
		{
			REAL b[3];
			clear_v3( b );
			for( INT32 j=0; j<nb_point; ++j )
			{
				struct Site* p = (*pl)[j].site;
				add_v3( b, REAL(p->x), REAL(p->y), REAL(p->z * _scale_axe) );	//todo	p->z need _to be done at some point
			}
			scale_v3( b, 1./nb_point );
			interpolate_v3( c, b, inter_bary );					
		}

		if( b_draw_cell )
		{	//	we draw Voronoi cell in one color
			//	we set the color once
			if(	img_map )
			{
				img_map->get_valid_color4r_from_uv_nearest( s->color, c[0] * su + REAL(.5), c[1] * sv + REAL(.5), _b_img_color_clamped_ui );
				if( s->color[3] < _voronoi_draw_alpha_threshold )
					continue;

				INT32  j;
				for( j=nb_point-1; j>=0; --j )
				{
					struct Site* p = (*pl)[j].site;
					if( _voronoi_draw_alpha_threshold > img_map->get_valid_value_from_uv(  REAL(p->x) * su + REAL(.5), REAL(p->y) * sv + REAL(.5), _b_img_color_clamped_ui, aaa::COMPO::ALPHA ) )
						break;
				}
				if( j>=0 )
					continue;
				mul_v4( s->color, _voronoi_poly_color );
				if( _img_color_z_influence )
					c[2] += _img_color_z_influence * REAL(.333333) * ( s->color[0] + s->color[1] + s->color[2] );
			}
		}
		s->b_use = true;
	}

}

namespace {
	c_guf		guf;
}

void c_bdd_voronoi::draw_edge( struct	Edge* e )
{
	if( !e )
		return;

	struct Site* s1	= e->reg[0];
	struct Site* s2	= e->reg[1];
	if( s1 && s2 )
		draw_line(	REAL(s1->x), REAL(s1->y), REAL(s1->z) * _scale_axe,
					REAL(s2->x), REAL(s2->y), REAL(s2->z) * _scale_axe );

	s1	= e->end_point[0];
	s2	= e->end_point[1];
	if( s1 && s2 )
		draw_line(	REAL(s1->x), REAL(s1->y), REAL(s1->z) * _scale_axe,
					REAL(s2->x), REAL(s2->y), REAL(s2->z) * _scale_axe );
	
	GOL::begin( GL_POINTS );
		if( s1 )
			GOL::vertex3( REAL(s1->x), REAL(s1->y), REAL(s1->z) * _scale_axe );
		if( s2 )
			GOL::vertex3( REAL(s2->x), REAL(s2->y), REAL(s2->z) * _scale_axe );
	GOL::end();
}

void	c_bdd_voronoi::draw_voronoi_poly()
{
	GOL::push_att();
	SPY_PUSH_RANGE( "Voronoi Draw poly", spy::COL_4 );

		c_img_2d*	img_map = nullptr;
		if ( _b_img_color_use )
		{
			img_map = g_bind_img_2d->get_ready( _s_image_color_bind );
			img_map = ( img_map->is_ok() && img_map->is_data_valid(__FUNCTION__) ) ? img_map : nullptr;
		}

		bool	b_draw_cell = !img_map || !(_b_img_color_by_triangle || _b_img_color_by_point );
		bool	b_use_draw_arrays = GOL::b_draw_avoid_vertex_use && b_draw_cell;

		REAL inter		= _voronoi_poly_size;
		REAL inter_bary	= _voronoi_poly_size_barycenter;

		GOL::push_color4v( _voronoi_poly_color );
		GOL::push_line_width( _voronoi_poly_line_size );
		REAL su = OVER_ONE_AS_REAL( _img_color_size[0] );
		REAL sv = OVER_ONE_AS_REAL( _img_color_size[1] );
		INT32	prim = _b_voronoi_poly_as_triangle ? GL_TRIANGLE_FAN : GL_POLYGON ;

		REAL ub, ue, vb, ve;
		c_map* map = c_map::get_cur();

		bool	b_need_uv = map->is_implicit();	//	refine this condition and usage
		if( b_need_uv )
		{
			if( map->is_flip_uv() )
			{
				ub	= map->get_v_min();
				ue	= map->get_v_max();
				vb	= map->get_u_min();
				ve	= map->get_u_min();
			}
			else
			{
				ub	= map->get_u_min();
				ue	= map->get_u_min();
				vb	= map->get_v_min();
				ve	= map->get_v_max();
			}
		}

		if( b_use_draw_arrays )
		{
			GOL::unbind_vao_secu();
			guf.draw_before( false, b_need_uv && prim == GL_TRIANGLE_FAN );
			if( b_need_uv && prim == GL_TRIANGLE_FAN )
			{
				REAL*	uv = guf.get_uv();
				if( uv )
				{
					*uv		=	ub;
					*++uv	=	vb;
					for( INT32 j=guf.get_nb()-1; j>0; --j )
					{
						*++uv	=	ue;
						*++uv	=	ve;
					}
				}
			}
		}

		FP32	color[4];
		REAL	tmp[3];
		REAL	ci[3];
		REAL	v1[3];
		REAL	v2[3];	

		INT32 nb_site = _vdg->_site_nb;
		struct Site* s = _vdg->get_sites_src();
		

		GOL::normal3v( unit_z_v4fp32 );
		for( INT32 i=0; i<nb_site; ++i, ++s )
		{
			if( s->b_use )
			{		
				std::vector<struct PolygonPoint>*	l = s->point_list;
				//if( l )
				//{
				INT32 nb_point = (INT32)l->size();
				if( nb_point == 0 )
					continue;
				//	{
				REAL*	c = s->bary;

				if( b_draw_cell )
				{	//	we draw Voronoi cell in one color
					//	we set the color once
					if(	img_map )
						GOL::color4v( s->color );	

					if( b_use_draw_arrays )
					{	
						UINT32 nb_point_to_draw = nb_point;
						if( prim == GL_TRIANGLE_FAN )
							nb_point_to_draw += 2;
						if( guf.get_nb() < nb_point_to_draw )
						{
							//we build a bigger table for texture coor
							guf.draw_after();
							guf.alloc_point( nb_point_to_draw, __FUNCTION__ );
							guf.draw_before( false, b_need_uv && prim == GL_TRIANGLE_FAN );
							//	this only for the case prim == GL_TRIANGLE_FAN
							if( b_need_uv )
							{
								REAL*	uv = guf.get_uv();
								*uv		=	ub;
								*++uv	=	vb;
								for( INT32 j=nb_point_to_draw-1; j>0; --j )
								{
									*++uv	=	ue;
									*++uv	=	ve;
								}
							}
						}

						REAL*	point = guf.get_point();
						if( prim == GL_TRIANGLE_FAN )
						{
							cpy_v3( point, c );
							point += 2;
						}
						else
							--point;
						for( INT32 j=0; j<=(prim == GL_TRIANGLE_FAN ? nb_point : nb_point-1); ++j )
						{
							struct Site* p = (*l)[  j==nb_point ? 0 : j ].site;
							set_v3( ++point, p->x, p->y, p->z * _scale_axe );	//todo	p->z need _to be done at some point
							interpolate_v2( point, c, point, inter );
							point +=2;
							*point = c[2];
						}
						guf.draw_low( prim, nb_point_to_draw );
					}
					else
					{		
						GOL::begin( prim );
						if( prim == GL_TRIANGLE_FAN )
						{
							if( b_need_uv )
								GOL::texcoord2( ub, vb );
							GOL::vertex3v( c );
						}
						if( b_need_uv )
							GOL::texcoord2( ue, ve );
						for( INT32 j=0; j<=(prim == GL_TRIANGLE_FAN ? nb_point : nb_point-1); ++j )
						{
							struct Site* p = (*l)[  j==nb_point ? 0 : j ].site;
							set_v3( v1, p->x, p->y, p->z * _scale_axe );	//todo	p->z need _to be done at some point
							interpolate_v2( v1, c, v1, inter );
							v1[2] = c[2];
							GOL::vertex3v( v1 );
						}
						GOL::end();
					}
				}
				else if( _b_voronoi_triangle )
				{
					if( _b_img_color_by_triangle )
					{
						l->push_back( *(l->begin()) );

						GOL::begin( GL_TRIANGLES );
							struct Site* p = (*l)[0].site;
							set_v3( v1, p->x, p->y, p->z * _scale_axe );	//todo	p->z need _to be done at some point
							v1[2] = c[2];
							for( INT32 j=1; j<=nb_point; ++j )
							{
								p = (*l)[j].site;
								set_v3( v2, p->x,p->y,p->z * _scale_axe );	//todo	p->z need _to be done at some point
								v2[2] = c[2];

								center_v3r( ci, c, v1, v2 );

								img_map->get_valid_color4r_from_uv_nearest( color, ci[0] * su + REAL(.5), ci[1] * sv + REAL(.5), _b_img_color_clamped_ui );
								if( color[3] >= _voronoi_draw_alpha_threshold )
								{
									tmp[2] = ci[2];
									mul_v4( color, _voronoi_poly_color );
									GOL::color4v( color );

									if( b_need_uv )
										GOL::texcoord2( ub, vb );
									interpolate_v2( tmp, ci, c, inter );
									GOL::vertex3v( tmp );

									if( b_need_uv )
										GOL::texcoord2( ue, ve );
									interpolate_v2( tmp, ci, v1, inter );
									GOL::vertex3v( tmp );
									interpolate_v2( tmp, ci, v2, inter );
									GOL::vertex3v( tmp );
								}
								cpy_v3( v1, v2 );
							}
						GOL::end();
					}
					else	//	color by point
					{		
						GOL::begin( prim );
						if( b_need_uv )	{	GOL::texcoord2( ub, vb );	}
						img_map->mul_valid_and_set_color4v_from_uv( _voronoi_poly_color, c[0] * su + REAL(.5), c[1] * sv + REAL(.5), _b_img_color_clamped_ui );

						GOL::vertex3v( c );

						if( b_need_uv )
							GOL::texcoord2( ue, ve );
						for( INT32 j=0; j<=nb_point; ++j )
						{
							struct Site* p = (*l)[ j==nb_point ? 0 : j ].site;
							set_v3( v1, p->x, p->y, p->z * _scale_axe );	//todo	p->z need _to be done at some point
							interpolate_v2( v1, c, v1, inter );
							v1[2] = c[2];
							img_map->mul_valid_and_set_color4v_from_uv( _voronoi_poly_color, v1[0] * su + REAL(.5), v1[1] * sv + REAL(.5), _b_img_color_clamped_ui );
							GOL::vertex3v( v1 );
						}
						GOL::end();
					}
				}
				else if( _b_img_color_by_triangle )
				{
					l->push_back( *(l->begin()) );

					GOL::begin( GL_TRIANGLES );
					struct Site* p = (*l)[0].site;
					set_v3( v1, p->x,p->y,p->z * _scale_axe );	//todo	p->z need _to be done at some point
					interpolate_v2( v1, c, v1, inter );
					v1[2] = c[2];
					for( INT32 j=1; j<=nb_point; ++j )
					{
						p = (*l)[j].site;
						set_v3( v2, p->x,p->y,p->z * _scale_axe );	//todo	p->z need _to be done at some point
						interpolate_v2( v2, c, v2, inter );
						v2[2] = c[2];
					
						center_v2r( ci, c, v1, v2 );
						img_map->get_valid_color4r_from_uv_nearest( color, ci[0] * su + REAL(.5), ci[1] * sv + REAL(.5), _b_img_color_clamped_ui );
						if( color[3] >= _voronoi_draw_alpha_threshold )
						{
							mul_v4( color, _voronoi_poly_color );
							GOL::color4v( color );

							if( b_need_uv )	{	GOL::texcoord2( ub, vb );	}
							GOL::vertex3v( c );
							if( b_need_uv )	{	GOL::texcoord2( ue, ve );	}
							GOL::vertex3v( v1 );
							GOL::vertex3v( v2 );
						}
						cpy_v3( v1, v2 );
					}
					GOL::end();
				}
				else	//	color by point
				{		
					GOL::begin( prim );
						if( b_need_uv )	{	GOL::texcoord2( ub, vb );	}
						img_map->mul_valid_and_set_color4v_from_uv( _voronoi_poly_color, c[0] * su + REAL(.5), c[1] * sv + REAL(.5), _b_img_color_clamped_ui );
						GOL::vertex3v( c );

						if( b_need_uv )
							GOL::texcoord2( ue, ve );
						for( INT32 j=0; j<=nb_point; ++j )
						{
							struct Site* p = (*l)[ j==nb_point ? 0 : j ].site;
							set_v3( v1, p->x, p->y, p->z * _scale_axe );	//todo	p->z need _to be done at some point
							interpolate_v2( v1, c, v1, inter );
							v1[2] = c[2];
							img_map->mul_valid_and_set_color4v_from_uv( _voronoi_poly_color, v1[0] * su + REAL(.5), v1[1] * sv + REAL(.5), _b_img_color_clamped_ui );
							GOL::vertex3v( v1 );
						}
					GOL::end();
				}
				//	}
				//	else
				//		ERR_PRINT_STRING( "l with 0 point" );
				//}
				//else
				//	ERR_PRINT_STRING( "empty l" );
			}
		}
		if( b_use_draw_arrays )
			guf.draw_after();
	SPY_POP_RANGE();
	GOL::pop_att();
}

void	c_bdd_voronoi::save_poly()
{
	REAL inter		= _voronoi_poly_size;
	REAL inter_bary = _voronoi_poly_size_barycenter;

	REAL v1[3];

	o_str str_pt;
	o_str str_f;

	INT32 nb_site = _vdg->_site_nb;
	struct Site* s = _vdg->get_sites_src();
	INT32 fi = 0;

	for( INT32 i=0; i<nb_site; ++i, ++s )
	{
		if( s->b_use )
		{		
			std::vector<struct PolygonPoint>*	l = s->point_list;
			INT32 nb_point = (INT32)l->size();
			if( nb_point == 0 )
				continue;					
			REAL*	c = s->bary;

			for( INT32 j=0; j<nb_point; ++j )
			{
				struct Site*	p = (*l)[j].site;
				set_v3( v1, p->x, p->y, p->z * _scale_axe );	//todo	p->z need _to be done at some point
				interpolate_v2( v1, c, v1, inter );
				v1[2] = c[2];
				str_pt.add( "v " );
					str_pt.add( v1[0] );
				str_pt.add_space();
					str_pt.add( v1[1] );
				str_pt.add_space( );
					str_pt.add( v1[2] );
				str_pt.add_char( '\n' );
			}

			str_f.add_char( 'f' );
			for( INT32 j=0; j<nb_point; ++j )
			{
				str_f.add_space();
				str_f.add( ++fi );
			}
			str_f.add_char( '\n' );
		}
	}
	str_pt.add( str_f );
	str_pt.write_file( "voronoi.obj" );
}

void	c_bdd_voronoi::draw_single()
{
	if( !_vdg )
		return;

#if 1
	if( _b_voronoi_ui && _b_voronoi_poly && _b_voronoi_poly_before )
		draw_voronoi_poly();

	if( _b_site_src )
	{
		SPY_PUSH_RANGE( "Voronoi draw site", spy::COL_4 );
		GOL::push_att();
			INT32 nb = _vdg->_site_nb;
			GOL::push_color3v( GOL::fp32_yellow );
			REAL d = _site_size_ui * REAL(.5);
			GOL::begin( GL_LINES );
				struct Site* s = _vdg->get_sites_src() - 1;
				for( INT32 i=0; i<nb; ++i )
				{
					++s;
					REAL x = REAL(s->x);
					REAL y = REAL(s->y);
					REAL z = REAL(s->z) * _scale_axe;
					GOL::vertex3( x-d, y-d, z );
					GOL::vertex3( x+d, y+d, z );
					GOL::vertex3( x-d, y+d, z );
					GOL::vertex3( x+d, y-d, z );
				}
			GOL::end();
		GOL::pop_att();
		SPY_POP_RANGE();

	}

	if( _b_site_src_number )
	{
		GOL::push_att( );
		INT32 nb = _vdg->_site_nb;
		struct Site*	s = _vdg->get_sites_src() - 1;
		GOL::push_color3v( GOL::fp32_yellow );
		DOUBLE d = 0.01;
		for( INT32 i=0; i<nb; ++i )
		{
			++s;
			aaa::alphabet::draw_int32_xyz( i, REAL(s->x),REAL(s->y),REAL(s->z)*_scale_axe, _number_size_ui, _number_size_ui );
		}
		GOL::pop_att();
	}
	
	if( _b_delaunay_ui )
	{
		SPY_PUSH_RANGE( "Voronoi draw delaunay", spy::COL_5 );
		GOL::push_att();	//	( GL_CURRENT_BIT | GL_LINE_BIT );

			GOL::push_color4v( _delaunay_color );
			GOL::push_line_width( _delaunay_line_size );

			_vdg->reset_segment_delaunay_iterator();
			GOL::begin( GL_LINES );
				//go through the vertices in the graph - if either of the two ends of a line is in a cell that has
				//a value between the two thresholds, ignore the line, since it only exists because of the 
				//performance enhancement done earlier.  Otherwise store it in the list of voronoi vertices
				while( struct Edge* e = _vdg->get_segment_delaunay_next() )
				{
					//if( e->b_how )
					//	GOL::color4( _delaunay_color );
					//else
					//	GOL::color3( 0, .5, 1 );
					Site* a = e->reg[0];
					GOL::vertex3( REAL(a->x),REAL(a->y),REAL(a->z)*_scale_axe  );
	//					glVertex2d( (s->a[0] + s->b[0])*.5, (s->a[1] + s->b[1])*.5 - .02 );
	//					glVertex2d( (s->a[0] + s->b[0])*.5, (s->a[1] + s->b[1])*.5 - .02 );
					Site* b =  e->reg[1];
					GOL::vertex3( REAL(b->x),REAL(b->y),REAL(b->z)*_scale_axe );
					if( _b_delaunay_orientation_ui )
					{
						//REAL x,y,z;
						//x = a->x * .85 + b->x *.15;
						//y = a->y * .85 + b->y *.15; 
						//z = (a->z * .85 + b->z *.15) *_scale_axe;
						//GOL::vertex3( x, y, z );
						
						//if( e->a==1 && e->b<0 )
						//	GOL::vertex3( x+e->b*_number_size_ui*2, y-e->a*_number_size_ui*2, z );
						//else
						//	GOL::vertex3( x-e->b*_number_size_ui*2, y+e->a*_number_size_ui*2, z );
						Site* c1 = e->end_point[0];
						Site* c2 = e->end_point[1];
						if( c1 && c2 )
						{
							GOL::vertex3( REAL(c1->x),REAL(c1->y),REAL(c1->z)*_scale_axe  );
							GOL::vertex3( REAL(c2->x),REAL(c2->y),REAL(c2->z)*_scale_axe  );
						}
						/*
						x = a->x * .15 + b->x *.85;
						y = a->y * .15 + b->y *.85; 
						z = (a->z * .15 + b->z *.85) *_scale_axe;
						GOL::vertex3( x, y, z );
						if( e->a==1 && e->b <0 )
							GOL::vertex3( x-e->b*_number_size*2, y+e->a*_number_size*2, z );
						else
							GOL::vertex3( x+e->b*_number_size*2, y-e->a*_number_size*2, z );
						*/
					}
				}
			GOL::end();
			if( _b_delaunay_edge_number_ui )
			{
				_vdg->reset_segment_delaunay_iterator();
				while( struct Edge* e = _vdg->get_segment_delaunay_next() )
				{
					Site* CONST a = e->reg[0];
					Site* CONST b = e->reg[1];
					aaa::alphabet::draw_int32_xyz( e->edge_id,	REAL(a->x + b->x) * REAL(.5),
																REAL(a->y + b->y) * REAL(.5), 
																REAL(a->z + b->z) * REAL(.5)*_scale_axe,
																_number_size_ui, _number_size_ui );
				}
			} 
		GOL::pop_att();
		SPY_POP_RANGE();
	}

	if( _b_vertice )
	{
		SPY_PUSH_RANGE( "Voronoi draw vertice", spy::COL_6 );
			_vdg->reset_vertice_iterator();
			GOL::begin( GL_POINTS );
				while( Site* site = _vdg->get_vertice_next() )
				{
					GOL::vertex3( REAL(site->x), REAL(site->y), REAL(site->z)*_scale_axe );
				}
			GOL::end();
		SPY_POP_RANGE();
	}

	if( _b_edge )
	{
		SPY_PUSH_RANGE( "Voronoi draw edge", spy::COL_1 );
			_vdg->reset_segment_voronoi_iterator();
			GOL::begin( GL_LINES );
			while( st_segment_voronoi* s = _vdg->get_segment_voronoi_next() )
			{
				Site* a = s->a;
				Site* b = s->b;
				if( _b_draw_boundary || !(a->b_boundary || b->b_boundary) )
				{
					GOL::vertex3( REAL(a->x), REAL(a->y), REAL(a->z) * _scale_axe );
					GOL::vertex3( REAL(b->x), REAL(b->y), REAL(b->z) * _scale_axe );
				}
			}
			GOL::end();
		SPY_POP_RANGE();
	}

	if( _b_voronoi_ui && _b_voronoi_poly && !_b_voronoi_poly_before )
		draw_voronoi_poly();

	GOL::push_att();
		if( _b_edge_link_ui )
		{
			GOL::push_color3v( GOL::fp32_green );
			_vdg->reset_iterator_vertex_pair_final();
			GOL::begin( GL_LINES );
				//go through the vertex pairs in the graph - if either of the two ends of a line is in a cell that has
				//a value between the two thresholds, ignore the line, since it only exists because of the 
				//performance enhancement done earlier.  Otherwise store it in the list of voronoi vertices
				REAL	x1, x2, y1, y2;
				while( _vdg->get_next_vertex_pair_final( x1, y1, x2, y2 ) )
				{
					GOL::vertex2( x1, y1 );
					GOL::vertex2( x2, y2 );
				}	
			GOL::end();
		}

		if( _b_vertex )
		{
			GOL::push_color3v( GOL::fp32_red );
			_vdg->reset_iterator_vertex_final();
			GOL::begin( GL_POINTS );
				//go through the vertices in the graph - if either of the two ends of a line is in a cell that has
				//a value between the two thresholds, ignore the line, since it only exists because of the 
				//performance enhancement done earlier.  Otherwise store it in the list of voronoi vertices
				REAL	x1, y1;
				while( _vdg->get_next_vertex_final( x1, y1 ) )
					GOL::vertex2( x1, y1 );
			GOL::end();
		}
	GOL::pop_att();
	
	if( _b_EL )
	{
		GOL::push_att();
		
		struct Halfedge*	EL = _vdg->EL_get_left_end();

		GOL::push_color3v( GOL::fp32_yellow );
		GOL::begin( GL_LINE_STRIP );
		while( EL )
		{
			struct	Site*	vertex = EL->vertex;
			if( vertex )
				GOL::vertex3( REAL(vertex->x), REAL(vertex->y), REAL(vertex->z) * _scale_axe );
			EL = EL->EL_right;
		}
		GOL::end();

		EL = _vdg->EL_get_left_end();
		while( EL )
		{
			struct Site*	vertex = EL->vertex;
			if( vertex )
			{
				GOL::color_yellow();
				GOL::begin( GL_POINTS );
					GOL::vertex3( REAL(vertex->x), REAL(vertex->y), REAL(vertex->z) * _scale_axe );
				GOL::end();
			}
			struct Edge*	he = EL->EL_edge;
			GOL::color_cyan();
			draw_edge( he );


			struct Halfedge*	PQ_next = EL->PQ_next;
			if( PQ_next )
			{
				GOL::color_magenta();
				draw_edge( PQ_next->EL_edge );
			}
			EL = EL->EL_right;
		}
		
		GOL::pop_att();
	}
#else
	PolygonPoint* pSitePoints;

	INT32	nb;
	for( INT32 i = 0; i < _point_nb; i++	)
	{
		_vdg->getSitePoints( i, &nb, &pSitePoints );
		if( nb == 0 )
		{
			err_print( "-- no points for %d\n", i );
		}
		else
		{
		//	printf("update temp_child_4076440_0 set resultgeom = st_setsrid('POLYGON((");
			GOL::begin( GL_LINE_LOOP );
				REAL z = REAL(0);
				for( INT32 j = 0; j < nb; j++ )
				{
					z += .01;
					if( z > 1. )
						z = 0.;
					GOL::vertex3( pSitePoints[j].x, pSitePoints[j].y, z );
					//GOL::vertex2( pSitePoints[j].x, pSitePoints[j].y );
					//printf("%.15lf %.15lf,", pSitePoints[j].x, pSitePoints[j].y, (pSitePoints[j].angle/M_PI)*180);
				}
			GOL::end();
			//printf("%.15lf %.15lf", pSitePoints[0].x, pSitePoints[0].y, (pSitePoints[j].angle/M_PI)*180);
			//printf("))'::geometry,4326) where id = %d;\n", sites[i].id);
		}
	}
#endif
}

void	c_bdd_voronoi::draw_multiple()
{
	if( !_vdg )
		return; 
	INT32	nb = _vdg->get_vertice_nb();
	if( _b_vertice && nb > 0 )
	{
		_vdg->reset_vertice_iterator();
		c_multiple::cur->set_nb( nb );
		//	if( c_multiple::cur->is_align_normal() )
		REAL v[3];
		v[2] = REAL(0);
		while( Site* site = _vdg->get_vertice_next() )
		{
			INT32 id = site->site_id;
			c_multiple::cur->set_index( id );
			c_tex_anim::cur->bind_this( id );
			v[0] = REAL(site->x);
			v[1] = REAL(site->y);
			c_multiple::cur->align_then_draw( v );
		}
	}
	if( _b_edge )
	{
		_vdg->reset_segment_voronoi_iterator();
		while( st_segment_voronoi* s = _vdg->get_segment_voronoi_next() )
		{
			Site* a = s->a;
			Site* b = s->b;
			REAL	pos[3];
			pos[0] = REAL(a->x + b->x) * REAL(.5);
			pos[1] = REAL(a->y + b->y) * REAL(.5);
			pos[2] = REAL(a->z + b->z) * REAL(.5);
			REAL	nor[3];
			nor[0] = REAL(a->x - b->x);
			nor[1] = REAL(a->y - b->y);
			nor[2] = REAL(a->z - b->z);
			c_multiple::cur->draw_one_at_nor( pos, nor );
		}
	}
}


INT32	c_bdd_voronoi::get_segment_nb()
{
	return	_vdg->get_segment_voronoi_nb();
}
void	c_bdd_voronoi::get_segment( REAL* a, REAL* b, INT32 seg_index )
{
	st_segment_voronoi*	s = _vdg->get_segment_voronoi( seg_index );
	auto* pt = s->a;
	a[0] = REAL(pt->x);
	a[1] = REAL(pt->y);
	a[2] = REAL(pt->z);
	pt = s->b;
	b[0] = REAL(pt->x);
	b[1] = REAL(pt->y);
	b[2] = REAL(pt->z);
}


INT32	c_bdd_voronoi::get_point_nb()
{
	return _vdg->get_vertice_nb();
}

bool	c_bdd_voronoi::get_point( REAL* CONST dst, INT32 CONST index )
{
	Site** hd_site = _vdg->get_vertice( index );
	if( hd_site )
	{
		Site* s = *hd_site;
		set_v3( dst, s->x, s->y, s->z );
		return true;
	}
	clear_v3( dst );
	return false;
}