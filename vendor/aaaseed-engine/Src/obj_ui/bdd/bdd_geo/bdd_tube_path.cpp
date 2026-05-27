#include "obj_ui/bdd/bdd_geo/bdd_tube_path.h"

#include "obj_ui/bdd/bdd_geo/bdd_circle.h"
#include "obj_ui/bdd/bdd_edit/bdd_curve3d.h"
#include "obj_ui/bdd/bdd_point/bdd_mocap.h"

#include "draw/model.h"
#include "draw/render.h"
#include "bdd_util.h"
#include "obj_ui/deformer/def_node.h"
#include "draw/tex_anim.h"
#include "infrastructure/layer/layer.h"
#include "language/lua/aaalua_wrap.h"

#include "draw/guf.h"

//todo	normal are perhaps badly computed on first and last ring in loop and not loop node

FACTORY_CREATE_PROP_V1( c_bdd_tube_path, bdd_tube_path, Tube, tube_path, sub_menu="Geometry"; );

namespace {	c_guf	guf;	}

class	c_ring_attrib	
{
public:
	bool	_b_enabled;
	REAL	_center_offset[3];
	REAL	_radius_factor[2];
	REAL	_angle_offset;
	REAL	_angle_range;
	bool	_build_normal_needed;
	INT32	_s_tex_id;
	bool	_b_tex;
	REAL	_tex_u;
	REAL	_tex_u_next;
	REAL	_tex_v;
	REAL	_tex_dv;
	REAL	_tex_v_next_min;
	REAL	_tex_dv_next;
	INT32	_marker_id;
	o_str	_marker_fn;

	void	init()
					{
						_b_enabled				= true;
						clear_v3( _center_offset );
						set_v2( &_radius_factor[0], REAL(1) );
						_angle_offset			= 0.;
						_angle_range			= 1.;
						_build_normal_needed	= true;
						_s_tex_id				= -1;	//mean no change
						_b_tex					= false;
						//	we dont't need to init the _tex... members because they are used only when _b_tex is true
						_marker_id				= 0;
						_marker_fn.erase();
					}
	c_ring_attrib()
		{ init(); }
};


static	C_PCHAR_C	str_draw[4] =
{
	"RING_LINE",
	"RING_POLYGON",
	"RING_TRI",
	"TUBE"
};

//todo in 32 bits we can explode the memory
static	CONST	INT32	RING_POINT_MAX_NB	= 1024 * 128;
static	CONST	INT32	RING_MAX_NB			= 1024 * 256;

namespace n_bdd_tube_path
{
	CONSTEXPR INT32 BASE_PARAM_NB	= 12 + c_bdd::GEO_PARAM_NB;
	CONSTEXPR INT32 RING_PARAM_NB	= 9;
	CONSTEXPR INT32 MAP_PARAM_NB	= 3;
	CONSTEXPR INT32 GROUP_PARAM_NB	= 2;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	RING_PARAM_NB
									+	MAP_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_BASE_PARAMS

		PARAM_DEF_BOOL_OFF(			b_curve		)
		PARAM_DEF_INT32(			channel,	2, 1,	1, CHANNEL_NB_MAX )
		PARAM_DEF_INT32(			node,		2, 1,	1., PARAM_MAX_INT32 )
		PARAM_DEF_REAL_LOCKED(		len_total	)
		PARAM_DEF_REAL_ZERO(		s_begin		)
		PARAM_DEF_REAL_ZERO(		len_before	)
		PARAM_DEF_REAL_ZERO(		center		)
		PARAM_DEF_REAL_ZERO(		len_after	)
		PARAM_DEF_REAL_ONE(			s_end		)
		PARAM_DEF_SYMBO_PSTR_ZERO(	draw,		str_draw )
		PARAM_DEF_BOOL_ON(			loop		)

		PARAM_DEF_GROUP( Ring, RING_PARAM_NB )
			PARAM_DEF_BOOL_ON(		ring_open		)
			PARAM_DEF_REAL_ZERO(	angle_offset	)
			PARAM_DEF_REAL_ONE(		angle_range		)
			PARAM_DEF_BOOL_ON(		ring_nb_fix		)
			PARAM_DEF_INT32(		ring_nb,			2, 32,	2, RING_MAX_NB )
			PARAM_DEF_INT32_LOCKED(	ring_nb_drawn	)
			PARAM_DEF_REAL_POS_ONE(	ring_len		)
			PARAM_DEF_INT32(		ring_segment_nb,	2, 32,	2, RING_POINT_MAX_NB )
			PARAM_DEF_BOOL_ON(		marker_fn_allow	)
		
		PARAM_DEF_GROUP( Map, MAP_PARAM_NB )
			PARAM_DEF_BOOL_ON(		map_compute		)
			PARAM_DEF_REAL_ONE(		map_factor_u	)
			PARAM_DEF_REAL_ONE(		map_factor_v	)

		PARAM_DEF_BOOL_OFF(		force_rebuild )
	};
}

void	c_bdd_tube_path::param_init_pt()
{
	INT32	h = param_init_pt_geo();

	param_set_pt( h, _b_curve3d_ui	);
	param_set_pt( h, _channel_id_ui	);
	param_set_pt( h, _node_id_ui	);
	param_set_pt( h, _len			);
	param_set_pt( h, _s_begin		);
	param_set_pt( h, _len_before	);
	param_set_pt( h, _center		);
	param_set_pt( h, _len_after		);
	param_set_pt( h, _s_end			);
	param_set_pt( h, _s_draw		);
	param_set_pt( h, _b_loop		);

	++h;
		param_set_pt( h, _b_ring_open_ui		);
		param_set_pt( h, _angle_offset_ui		);
		param_set_pt( h, _angle_range_ui		);
		param_set_pt( h, _b_ring_nb_fix			);
		param_set_pt( h, _ring_nb_ui			);
		param_set_pt( h, _ring_nb_drawn			);
		param_set_pt( h, _ring_len				);
		param_set_pt( h, _ring_segment_nb_ui	);
		param_set_pt( h, _b_allow_marker_fn		);

	++h;
		param_set_pt( h,	_b_map_refresh	);
		param_set_pt( h,	_map_factor_u	);
		param_set_pt( h,	_map_factor_v	);

	param_set_pt( h, _b_force_rebuild );
	
	err_param_init_pt(h);
}

void c_bdd_tube_path::init()
{
	param_init_with( n_bdd_tube_path::param, n_bdd_tube_path::PARAM_NB_MAX ); // bdd_tube_path_param, BDD_TUBE_PATH_PARAM_NB_MAX);
}

void c_bdd_tube_path::alloc()
{
	if(	_ring_nb != _ring_nb_ui )
	{
		dealloc_rings();
		dealloc_uv();
	}
	if(	_ring_point_nb != _ring_point_nb_asked )
	{
		dealloc_uv();
	}

	if( !_rings )
	{
		_rings			= new c_circle		[	_ring_nb_ui ];
		_rings_attrib	= new c_ring_attrib	[	_ring_nb_ui ];
		if( _rings && _rings_attrib  )
		{
			_ring_nb		= _ring_nb_ui;

			c_circle*	pt = &_rings[0];
			for( INT32 i = 0; i < _ring_nb; ++i )
				pt++->set_direct( false );

			_b_build_needed = true;
		}
		else
		{
			dealloc();
			err_print( "can't allocate uvs for bdd_tube_path" );
		}
	}

	if( !_uv )
	{
		_uv				= new REAL	[	(_ring_nb_ui+1) * (_ring_point_nb_asked+1) * 2 ];
		if( _uv )
		{
			_ring_point_nb	= _ring_point_nb_asked;
			_b_build_needed = true;
		}
		else
		{
			dealloc();
			err_print( "can't allocate rings for bdd_tube_path" );
		}
	}
}

void c_bdd_tube_path::dealloc_rings()
{
	SAFE_DELETE_ARRAY( _rings );
	SAFE_DELETE_ARRAY( _rings_attrib );
	_ring_nb = 0;
}
void c_bdd_tube_path::dealloc_uv()
{
	SAFE_DELETE_ARRAY( _uv );
	_ring_point_nb = 0;
}
void c_bdd_tube_path::dealloc()
{
	dealloc_rings();
	dealloc_uv();
}

CONSTRUCTOR_CREATE(c_bdd_tube_path)
,_ring_ds(0.)
,_ring_start(0)
,_ring_stop(0)
,_ring_center(0)
,_node_id(0)
,_channel_id(0)
,_b_deform_last(false)
,_b_build_needed(true)
,_ring_point_nb(0)
,_rings(nullptr)
,_rings_attrib(nullptr)
,_uv(nullptr)
,_ring_nb(0)
,_b_ring_open(true)
,_b_compute_normal_needed(true)
,_curve(nullptr)
,_bdd_mocap(nullptr)
,_len(0)
{
	init();
}

c_bdd_tube_path::~c_bdd_tube_path()
{
	dealloc();
}

FINLINE	c_circle*		c_bdd_tube_path::get_ring_direct( INT32 ii )			{	return &_rings[ ii ];									}
FINLINE	c_ring_attrib*	c_bdd_tube_path::get_ring_attrib_direct( INT32 ii )		{	return &_rings_attrib[ ii ];							}
FINLINE	bool			c_bdd_tube_path::is_section_allowed_direct( INT32 ii )	{	return get_ring_attrib_direct( ii )->_b_enabled;		}

FINLINE	INT32			c_bdd_tube_path::build_ring_index( INT32 index )		{	return _b_loop ? IMOD( index, _ring_nb ) : CLAMP( index, 0, _ring_nb-1 );	}

FINLINE	c_circle*		c_bdd_tube_path::get_ring( INT32 index )				{	return get_ring_direct(				build_ring_index( index ) );	}
FINLINE	c_ring_attrib*	c_bdd_tube_path::get_ring_attrib( INT32 index )			{	return get_ring_attrib_direct(		build_ring_index( index ) );	}
FINLINE	void			c_bdd_tube_path::get_ring_and_attrib( INT32 index, c_circle*& ring, c_ring_attrib*& att )	
{
	index = build_ring_index( index );
	ring	= get_ring_direct( index );
	att		= get_ring_attrib_direct( index );
}
FINLINE	bool			c_bdd_tube_path::is_section_allowed( INT32 index )		{	return is_section_allowed_direct(	build_ring_index( index ) );	}

void	c_bdd_tube_path::update_ring_angle_offset( c_circle* ring, REAL angle_offset )
{
	ring->set_angle_offset( _angle_offset - _angle_range_ui * REAL(.5) + REAL(.5) + angle_offset );
}
void	c_bdd_tube_path::update_ring_angle_range( c_circle*	ring, REAL angle_range )
{
	ring->set_angle_range( _angle_range * angle_range );
}
void	c_bdd_tube_path::update_ring_radius( c_circle*	ring, REAL* radius )
{
	REAL rad[2];
	mul_v2( rad, radius, _scale );
	ring->set_radius( rad );
	
}
void	c_bdd_tube_path::init_ring( INT32 index )
{
	//hack
	if( !_rings_attrib )
		return;	//	protect at init but should not stay

	c_circle*		ring;
	c_ring_attrib*	att;
	get_ring_and_attrib( index, ring, att );

	att->init();
	ring->set_build_sin_cos_needed();

	update_ring_angle_offset(	ring,	att->_angle_offset	);
	update_ring_angle_range(	ring,	att->_angle_range	);
	REAL	rad[2];
	scale_v2( rad, att->_radius_factor,  REAL(.5) );
	update_ring_radius(			ring,	rad );
}

void	c_bdd_tube_path::init_ring_all()
{
	//hack
	if( !_rings_attrib )
		return;	//	protect at init but should not stay

	for( INT32 i=0; i<_ring_nb; ++i )
		init_ring( i );
}

void	c_bdd_tube_path::set_ring_angle_offset( INT32 index, REAL angle_offset )
{
	//hack
	if( !_rings_attrib )
		return;	//	protect at init but should not stay

	c_circle*		ring;
	c_ring_attrib*	att;
	get_ring_and_attrib( index, ring, att );

	att->_angle_offset = angle_offset;
	update_ring_angle_offset( ring, angle_offset );
}
void	c_bdd_tube_path::set_ring_angle_range( INT32 index, REAL angle_range )
{
	//hack
	if( !_rings_attrib )
		return;	//	protect at init but should not stay

	c_circle*		ring;
	c_ring_attrib*	att;
	get_ring_and_attrib( index, ring, att );

	att->_angle_range = angle_range;
	update_ring_angle_range( ring, angle_range );
}
void	c_bdd_tube_path::set_ring_center_offset(	INT32 index, REAL o_u, REAL o_v, REAL o_axe )
{
	//hack
	if( !_rings_attrib )
		return;	//	protect at init but should not stay

	c_circle*		ring;
	c_ring_attrib*	att;
	get_ring_and_attrib( index, ring, att );

	att->_center_offset[0] = o_u;
	att->_center_offset[1] = o_v;
	att->_center_offset[2] = o_axe;

	REAL offset[2];
	mul_scale_v2( offset, att->_center_offset, _scale, .5 );
	ring->set_offset( offset );
}
void	c_bdd_tube_path::get_ring_center_offset( INT32 index, REAL& ru, REAL& rv )
{
	//hack
	if( !_rings_attrib )
		return;	//	protect at init but should not stay

	c_ring_attrib*	att		=  get_ring_attrib(index);
	ru = att->_center_offset[0] * _scale[0];
	rv = att->_center_offset[1] * _scale[1];
}
void	c_bdd_tube_path::set_ring_radius( INT32 index, REAL ru, REAL rv )
{
	//hack
	if( !_rings_attrib )
		return;	//	protect at init but should not stay

	c_circle*		ring;
	c_ring_attrib*	att;
	get_ring_and_attrib( index, ring, att );

	att->_radius_factor[0] = ru;
	att->_radius_factor[1] = rv;

	REAL	rad[2];
	scale_v2( rad, att->_radius_factor,  REAL(.5) );
	update_ring_radius(	ring,	rad );
}
void	c_bdd_tube_path::get_ring_radius( INT32 index, REAL& ru, REAL& rv )
{
	//hack
	if( !_rings_attrib )
		return;	//	protect at init but should not stay

	c_ring_attrib*	att		=  get_ring_attrib(index);
	ru = att->_radius_factor[0] * _scale[0] * REAL(.5);
	rv = att->_radius_factor[1] * _scale[1] * REAL(.5);
}
void	c_bdd_tube_path::set_ring_enabled( INT32 index, bool b )
{
	//hack
	if( !_rings_attrib )
		return;	//	protect at init but should not stay

	 get_ring_attrib(index)->_b_enabled = b;
}
REAL*	c_bdd_tube_path::get_ring_center( INT32 index )
{
	return get_ring( index )->get_center();
}
REAL*	c_bdd_tube_path::get_ring_normal( INT32 index )
{
	return get_ring( index )->get_normal();
}

void	c_bdd_tube_path::set_scale( REAL* a )
{
	if( is_diff_v2( _scale, a ) )
	{
		cpy_v2( _scale, a );	//todo update on a ring base to optimize ?
		_b_build_needed = true;
	}
}

//	todo cache in obj to opt
FINLINE	REAL	c_bdd_tube_path::get_ring_ds()
{
	return _ring_ds;
}

void	c_bdd_tube_path::get_ring_from_s( REAL s, INT32& c, INT32& n, REAL& inter )
{
	REAL	f = s / get_ring_ds();
	c = I_FLOOR( f );
	inter = f - c;
	c = build_ring_index( c );
	n = build_ring_index( c+1 );
}

void	c_bdd_tube_path::set_ring_marker( INT32 index, C_PCHAR fn_name, INT32 id )
{
	//hack
	if( !_rings_attrib )
		return;	//	protect at init but should not stay

	c_ring_attrib*	att		=  get_ring_attrib( index );
	att->_marker_fn.set( fn_name );
	att->_marker_id = id;
}

void	c_bdd_tube_path::set_ring_texture( INT32 index, INT32 tex_index )
{
	//hack
	if( !_rings_attrib )
		return;	//	protect at init but should not stay

	c_ring_attrib*	att		=  get_ring_attrib( index );
	att->_s_tex_id = tex_index;
}

void	c_bdd_tube_path::set_ring_uv( INT32 index, REAL u, REAL u_next, REAL v_min, REAL v_max, REAL v_next_min, REAL v_next_max )
{
	//hack
	if( !_rings_attrib )
		return;	//	protect at init but should not stay

	c_ring_attrib*	att		=  get_ring_attrib(index);
	att->_b_tex				= true;

	att->_tex_u				= u;
	att->_tex_u_next		= u_next;

	att->_tex_v				= v_min;
	att->_tex_dv			= (v_max - v_min) / _ring_point_nb ;

	att->_tex_v_next_min	= v_next_min;
	att->_tex_dv_next		= (v_next_max - v_next_min) / _ring_point_nb ;
}

void	c_bdd_tube_path::build()
{
	if( _b_curve3d )
	{
		if( !_curve )
		{
			ERR_PRINT_STRING( "%s() no curve can't continue", __FUNCTION__ );
			return;
		}
	}
	else
	{
		if( !_bdd_mocap )
		{
			ERR_PRINT_STRING( "%s() no mocap can't continue", __FUNCTION__ );
			return;
		}
	}

	REAL	s = 0;
	REAL	ds = get_ring_ds();

	c_circle*		ring	= &_rings[0];
	c_ring_attrib*	att		= &_rings_attrib[0];

	for( INT32 i = 0; i < _ring_nb; ++i )
	{
		REAL	vec[3];
		REAL	nor[3];

		if( _b_curve3d )
			_curve->get_point_tangent( vec, nor, s );
		else
			_bdd_mocap->get_point_tangent( vec, nor, _node_id, s );

		ring->set_point_nb( _ring_point_nb );
		ring->set_center( vec );
		ring->set_normal( nor );
		ring->set_offset( att->_center_offset );
		//	done around the center
		update_ring_angle_range( ring, att->_angle_range );
		update_ring_angle_offset( ring, att->_angle_offset );

		REAL rad[2];
		mul_scale_v2( rad, _scale, att->_radius_factor, .5 );
		ring->set_radius( rad );

		s += ds;
		++ring;
		++att;
	}

	_b_build_needed = false;
	_b_compute_normal_needed = true;
}

void	c_bdd_tube_path::update_rings()
{
	c_circle*	pt;
	//	extend by 1 to have correct normal
	INT32	start = MAX0( _ring_start-1 );
	INT32	stop = MIN( _ring_stop+1, _ring_nb-1 );
	for( INT32 i = start; i <= stop; ++i )
	{
		INT32 ii = build_ring_index( i );
		pt = get_ring_direct( ii );
		if( pt->is_build_needed() )
		{
			pt->update();

			//get_ring_attrib(ii-1)->_build_normal_needed = true;
			get_ring_attrib_direct(ii)->_build_normal_needed = true;
			//get_ring_attrib(ii+1)->_build_normal_needed = true;

			_b_compute_normal_needed = true;
		}
	}
}

void	c_bdd_tube_path::deform_rings()
{
	//	extend by 1 to have correct normal
	INT32	start = MAX0( _ring_start-1 );
	INT32	stop = MIN( _ring_stop+1, _ring_nb-1 );
	for( INT32 i = start; i <= stop; ++i )
	{
		c_circle*		ring;
		c_ring_attrib*	att;
		get_ring_and_attrib( i, ring, att );

		ring->deform();

		//get_ring_attrib(ii-1)->_build_normal_needed = true;
		att->_build_normal_needed = true;
		//get_ring_attrib(ii+1)->_build_normal_needed = true;
	}
	_b_compute_normal_needed = true;
}

void	c_bdd_tube_path::build_normal()
{
	for( INT32 i = _ring_start; i <= _ring_stop; ++i )
	{
		INT32 ii = build_ring_index( i );
		if( get_ring_attrib_direct( ii )->_build_normal_needed )
		{
			c_circle*	p = get_ring( ii - 1 );
			c_circle*	c = get_ring_direct( ii );
			c_circle*	n = get_ring( ii + 1 );

			compute_normals_cano_v3r(	c->get_rays(),
										n->get_points_to_draw(),
										c->get_points_to_draw(),
										p->get_points_to_draw(),
										_ring_point_nb, !_b_ring_open );

			get_ring_attrib_direct( i )->_build_normal_needed = false;
		}
	}
}

void	c_bdd_tube_path::update()
{
	_ring_point_nb_asked = _ring_segment_nb_ui;
	_angle_range_asked = _angle_range_ui;
	if( _angle_range_ui == 1. && !_b_ring_open_ui )
	{
		_angle_range_asked = _angle_range_ui * (_ring_point_nb_asked-1) / _ring_point_nb_asked;
	}
	else
	{
		++_ring_point_nb_asked;
	}

	if( _node_id != _node_id_ui || _channel_id != _channel_id_ui || _b_curve3d != _b_curve3d_ui )
	{
		_b_build_needed = true;
		_b_curve3d = _b_curve3d_ui;
	}
	else if( _b_curve3d )
	{
		if( _curve && _curve->is_changed() )	//todo use a number here
		{
			_b_build_needed = true;
		}
	}
	_channel_id = _channel_id_ui;
	_node_id = _node_id_ui;

	_len = 0;
	if( _b_curve3d )
	{
		c_bdd_curve_3d*	bdd_curve = c_bdd_curve_3d::get_from_channel( _channel_id );
		if( !bdd_curve ) return;

		//curve->set_points_equi( _node_id, _ring_nb );
		_curve = bdd_curve->find_curve_valid( _node_id );
		if( !_curve )
			return;
		_len = REAL(_curve->get_len());
	}
	else
	{
		_bdd_mocap = c_bdd_mocap::get_from_channel( _channel_id );
		if( !_bdd_mocap )
			return;
		_len = _bdd_mocap->get_len( _node_id );
	}

	REAL ds;
	if( _b_ring_nb_fix )
	{
		INT32 nb =  _b_loop ? _ring_nb : (_ring_nb-1) ;
		ds = OVER_ONE_AS_REAL( nb );
	}
	else
	{
		if( _ring_len == 0. )
			_ring_nb_ui = RING_MAX_NB;
		else
			_ring_nb_ui	= UINT32( DIV( _len, _ring_len ) ) + 1 ;
		if( _ring_nb_ui >= RING_MAX_NB )
		{
			_ring_nb_ui = RING_MAX_NB;
			_ring_len = _len / _ring_nb_ui;
		}
		ds = DIV( _ring_len, _len );
	}

	if( _ring_ds != ds )
	{
		_ring_ds = ds;
		_b_build_needed = true;
	}
	
	if(	_ring_nb != _ring_nb_ui		||	_ring_point_nb != _ring_point_nb_asked	)
	{
		guf.alloc_point( (_ring_point_nb_asked+1)*2, __FUNCTION__ ); 
		alloc();
	}
//	else if( rendering_cur->is_tex_changed() )

	REAL	size[2];
	c_model::cur->get_size_v2( size );
	SWAP( size[0], size[1] );	//	follow the same inversion than the ring uv, bad tube coordonates, should be corrected
	set_scale( size );

	if(		_b_ring_open	!=	_b_ring_open_ui
		||	_angle_offset	!=	_angle_offset_ui
		||	_angle_range	!=	_angle_range_asked
		)
	{
		_b_ring_open	=	_b_ring_open_ui;
		_angle_offset	=	_angle_offset_ui;
		_angle_range	=	_angle_range_asked;
		_b_build_needed = true;
	}
	if( _b_build_needed || _b_force_rebuild )	//hack should deal with update of mocap
	{
		build();
	}
		
	if( _b_build_needed )
	{
		ERR_PRINT_STRING( "%s() build didn't complete can't continue", __FUNCTION__ );
		return;
	}

	REAL	l_start		= _center - _len_before;
	REAL	l_stop		= _center + _len_after;
	REAL	l_center	= _center;

	ORDER( l_start, l_stop );
	if ( (l_stop-l_start) > 1. )
	{
		l_start = l_center - REAL(.5);
		l_stop  = l_center + REAL(.5);
	}
	else if( l_center < l_start )
		l_center = l_start;
	else if ( l_stop < l_center )
		l_center = l_stop;
	if( !_b_loop )
	{
		CLAMP_REF( l_start,		_s_begin,	_s_end );
		CLAMP_REF( l_center,	_s_begin,	_s_end );
		CLAMP_REF( l_stop,		_s_begin,	_s_end );
	}

	INT32 tmp = _b_loop ? _ring_nb : (_ring_nb-1);
	_ring_start		= I_FLOOR( l_start	* tmp );
	_ring_center	= I_FLOOR( l_center	* tmp );
	_ring_stop		= I_FLOOR( l_stop	* tmp );		//todo we could adjust better

	_ring_nb_drawn = _ring_stop - _ring_start + 1;

	//	warranty that _ring_center is always in the right range
	INT32	i = IMOD( _ring_center, _ring_nb );
	if( i != _ring_center )	
	{	//translate
		i = i - _ring_center;
		_ring_start += i;
		_ring_center += i;
		_ring_stop += i;
	}

	//todo	limit range when too big
	//	if( rendering_cur->is_tex_changed() )
	//todoqq it should be a sub class of bdd_uv and be updated only iswhen needed
	if ( c_layer::get_cur()->is_need_uv() && _b_map_refresh )
		bdd_util_build_uv( _uv, _ring_point_nb+1, _ring_nb+1, false, false, c_map::get_cur()->is_flip_uv(), _map_factor_u, _map_factor_v );
//			REAL(_ring_point_nb_asked-1) / _ring_point_nb_asked, 64. );
	update_rings();

	bool b_deform = c_def_node::get_cur()->is_deforming();
	if( b_deform || _b_deform_last )
	//hack _b_deform_last because I don't call update on bdd_circle
	{
		deform_rings();
		_b_compute_normal_needed = true;
	}
	_b_deform_last = b_deform;
	//todo we do do it too much
	if( is_normal_needed() && _b_compute_normal_needed )
	{
		build_normal();	//todo optimize
		_b_compute_normal_needed = false;
	}
}

void	c_bdd_tube_path::draw_ring_normal_one( INT32 sec_index )
{
	c_circle* cur = get_ring( sec_index );

	REAL	vec[3];

	REAL*	pt = cur->get_points_to_draw();
	REAL*	normal = cur->get_rays();

	GOL::begin( GL_LINES );
		for( INT32 i = _ring_point_nb; i > 0; --i )
		{
			GOL::vertex3v( pt );
			add_scale_v3( vec, pt, normal, c_render::get_cur()->get_normal_len_point() );
			GOL::vertex3v( vec );
			pt += 3;
			normal += 3;
		}
	GOL::end();
}

void	c_bdd_tube_path::draw_normal_face( REAL len )
{
	if( _b_build_needed )
		return;
	/*
	REAL	nor[3];
	REAL	vec[3];

	scale_v3r( nor, normal, rendering_cur->get_normal_len_face() );
	GOL::begin( GL_LINES);
		GOL::vertex3v( center);
		add_v3r( vec, center, nor);
		GOL::vertex3v( vec);
	GOL::end();
	*/
}

FINLINE	static void	fprintf_poi_nor( FILE* file, INT32 a, INT32 b, INT32 c, INT32 d)
{
	fprintf( file, "f %d//%d %d//%d %d//%d %d//%d\n",
					   a,  a, b,  b, c,  c, d,  d );
}

void	c_bdd_tube_path::save_obj_file( FILE* file )
{	//todo should be complete and close/open and with deformation
	fprintf( file, "# %s\n", get_name_str() );

	c_circle* c = &_rings[0];
	for( INT32 i = 0; i < _ring_nb; ++i )
	{
		fprintf( file, "# Circle %d\n", i);
		c->save_obj_file_points( file );
		c->save_obj_file_normals( file );
		++c;
	}

	INT32	count = 1;
	INT32	point_nb = _rings->get_point_nb();

	for( INT32 i = 1; i < _ring_nb; ++i )
	{
		fprintf( file, "# Section %d\n", i );
		for( INT32 j = point_nb - 1; j > 0; --j )
		{
			fprintf_poi_nor( file,	count, count+1, count+point_nb+1, count+point_nb );
			++count;
		}
		fprintf_poi_nor( file,	count, count-point_nb+1, count+1, count+point_nb );
		++count;
	}
}	

FINLINE	void	c_bdd_tube_path::deal_with_marker_direct( c_ring_attrib* att, INT32 ring_index )
{
	if( _b_allow_marker_fn && !att->_marker_fn.is_empty() )
	{
		g_lua_wrap_cur->do_fn_pass_iir3( (c_obj_ui*)this, "aaa", att->_marker_fn.get(), att->_marker_id, ring_index, get_ring_direct( ring_index )->get_center() );
	}
}

void	c_bdd_tube_path::draw_ring_line()
{
	c_circle*	pt;
	for( INT32 i = _ring_start; i <= _ring_stop; ++i )
	{
		pt = get_ring( i );
		pt->draw_line( _b_ring_open );
	}
}

void	c_bdd_tube_path::draw_ring_polygon()
{
	c_circle*	pt;
	for( INT32 i = _ring_start; i <= _ring_stop; ++i )
	{
		c_tex_anim::cur->bind_this( i );
		pt = get_ring( i );
		pt->draw_polygon();
	}
}

void	c_bdd_tube_path::draw_ring_tri()
{
	for( INT32 i = _ring_start; i <= _ring_stop; ++i )
	{
		c_tex_anim::cur->bind_this(i);
		c_circle* pt = get_ring( i );
		pt->draw_tri();
	}
}

FINLINE void	c_bdd_tube_path::draw_section_one_old_school( INT32 sec_cur, INT32 sec_next )
{
	c_circle* cur	=	get_ring_direct( sec_cur );
	c_circle* next	=	get_ring_direct( sec_next );

	INT32	nb = _ring_point_nb;

	REAL*	c = cur->get_points_to_draw();
	REAL*	n = next->get_points_to_draw();

	REAL*	cn = nullptr;
	REAL*	nn = nullptr;
	if( is_normal_draw() )
	{
		cn = cur->get_rays();
		nn = next->get_rays();
	}

	--nb;
	GOL::begin( c_render::get_cur()->get_draw_primitive() );
		if ( is_normal_draw() )
		{
			//todoq	normal ne marche en mode list
			//		verifier calcul
			//	semble etre pb plus generic
			GOL::normal3v( cn );
			GOL::vertex3v( c );

			GOL::normal3v( nn );
			GOL::vertex3v( n );

			for( ; nb>0; --nb  )
			{
				cn	+= 3;
				c	+= 3;
				GOL::normal3v( cn );
				GOL::vertex3v( c );

				nn	+= 3;
				n	+= 3;
				GOL::normal3v( nn );
				GOL::vertex3v( n );
			}

			if( !_b_ring_open )
			{
				cn = cur->get_rays();
				c = cur->get_points_to_draw();
				GOL::normal3v( cn );
				GOL::vertex3v( c );

				nn = next->get_rays();
				n = next->get_points_to_draw();
				GOL::normal3v( nn );
				GOL::vertex3v( n );
			}
		}
		else
		{
			GOL::vertex3v( c );
			GOL::vertex3v( n );
			for( ; nb>0; --nb )
			{
				c	+= 3;
				GOL::vertex3v( c );
				n	+= 3;
				GOL::vertex3v( n );
			}
			if( !_b_ring_open )
			{
				c = cur->get_points_to_draw();
				GOL::vertex3v( c );
				n = next->get_points_to_draw();
				GOL::vertex3v( n );
			}
		}
	GOL::end();
}

FINLINE void	c_bdd_tube_path::draw_section_one_old_school_uv( INT32 sec_cur, INT32 sec_next )
{
	c_circle* cur	=	get_ring_direct( sec_cur );
	c_circle* next	=	get_ring_direct( sec_next );

	REAL*	c_uv = _uv + 2 * sec_cur;
	REAL*	n_uv = c_uv + 2;
	INT32	step_uv = (_ring_nb + 1) * 2;

	//REAL*	p;
	INT32	nb = _ring_point_nb;

	REAL*	c = cur->get_points_to_draw();
	REAL*	n = next->get_points_to_draw();

	REAL*	cn = nullptr;
	REAL*	nn = nullptr;
	if( is_normal_draw() )
	{
		cn = cur->get_rays();
		nn = next->get_rays();
	}

	--nb;
	GOL::begin( c_render::get_cur()->get_draw_primitive() );
		if( is_normal_draw() )
		{
			//todoq	normal ne marche en mode list
			//		verifier calcul
			//	semble etre pb plus generic
			GOL::texcoord2v( c_uv );
			GOL::normal3v( cn );
			GOL::vertex3v( c );

			GOL::texcoord2v( n_uv );
			GOL::normal3v( nn );
			GOL::vertex3v( n );

			for( ; nb>0; --nb )
			{
				//	next points
				c_uv += step_uv;
				cn	 += 3;
				c	 += 3;
				GOL::texcoord2v( c_uv );
				GOL::normal3v( cn );
				GOL::vertex3v( c );

				n_uv += step_uv;
				nn	 += 3;
				n	 += 3;
				GOL::texcoord2v( n_uv );
				GOL::normal3v( nn );
				GOL::vertex3v( n );
			}

			if( !_b_ring_open )
			{
				c_uv +=	step_uv;
				cn = cur->get_rays();
				c = cur->get_points_to_draw();
				GOL::texcoord2v( c_uv );
				GOL::normal3v( cn );
				GOL::vertex3v( c );

				n_uv +=	step_uv;
				nn = next->get_rays();
				n = next->get_points_to_draw();
				GOL::texcoord2v( n_uv );
				GOL::normal3v( nn );
				GOL::vertex3v( n );
			}
		}
		else
		{
			GOL::texcoord2v( c_uv );
			GOL::vertex3v( c );

			GOL::texcoord2v( n_uv );
			GOL::vertex3v( n );

			for( ; nb>0; --nb )
			{
				//	next points
				c_uv +=step_uv;
				c	 += 3;
				GOL::texcoord2v( c_uv );
				GOL::vertex3v( c );

				n_uv +=step_uv;
				n	 += 3;
				GOL::texcoord2v( n_uv );
				GOL::vertex3v( n );
			}
			if( !_b_ring_open )
			{
				c_uv += step_uv;
				c = cur->get_points_to_draw();
				GOL::texcoord2v( c_uv );
				GOL::vertex3v( c );

				n_uv += step_uv;
				n = next->get_points_to_draw();
				GOL::texcoord2v( n_uv );
				GOL::vertex3v( n );
			}
		}
	GOL::end();
}

//todo deal with 3d texture
template<bool b_uv>
FINLINE void	c_bdd_tube_path::draw_section_one( INT32 sec_cur, INT32 sec_next )
{
	c_ring_attrib* att = get_ring_attrib_direct( sec_cur );
	if( att->_b_enabled )
	{
		if( att->_s_tex_id >= 0 )
		{
			GOL::c_tex_unit*	tu = GOL::get_tex_unit_cur();
			if( !tu->is_bind_2d( att->_s_tex_id ) )
			{
				tex_2d_bind( att->_s_tex_id );
				c_map::get_cur()->do_wrap_2d();
			}
		}
		else
		{
			if( c_tex_anim::cur->is_tex_several() )
				c_tex_anim::cur->bind_this( sec_cur );
			else
			{
				GOL::c_tex_unit*	tu = GOL::get_tex_unit_cur();
				if( !tu->is_bind_2d( _bind_def ) )
				{
					tex_2d_bind( _bind_def );
					c_map::get_cur()->do_wrap_2d();
				}
			}
		}

		c_circle* cur	= get_ring_direct( sec_cur );
		c_circle* next	= get_ring_direct( sec_next );

		UINT32	nb	= _ring_point_nb;

		if( GOL::b_draw_avoid_vertex_use && ((nb+1)*2) < guf.get_nb() )
		{
			GOL::unbind_vao_secu();

			REAL* c = cur->get_points_to_draw();
			REAL* n = next->get_points_to_draw();

			REAL* dst = guf.get_point();
			dst = cpy_alternate_v3( dst, c, n, nb );
			if( !_b_ring_open )
			{
				//++nb;				//	yes this need to be here, other solution will be to change nb before but this is faster
				cpy_v3( dst, c );	//	do the last loop
				dst += 3;
				cpy_v3( dst, n );
			}
			if( is_normal_draw() )
			{
				REAL* cn = cur->get_rays();
				REAL* nn = next->get_rays();
				dst	= guf.get_normal();
				dst = cpy_alternate_v3( dst, cn, nn, nb );
				if( !_b_ring_open )
				{
					cpy_v3( dst, cn );
					dst += 3;
					cpy_v3( dst, nn );
				}
			}
			if( b_uv )
			{
				dst = guf.get_uv();
				if( att->_b_tex )
				{
					REAL v = att->_tex_v;
					REAL ve = att->_tex_v_next_min;
					--dst;
					for( INT32 i=nb; i>0; --i )
					{  
						*++dst = att->_tex_u;
						*++dst = v;
						v += att->_tex_dv;
						*++dst = att->_tex_u_next;
						*++dst = ve;
						ve += att->_tex_dv_next;
					}
					if( !_b_ring_open )
					{
						*++dst = att->_tex_u;
						*++dst = v;
						*++dst = att->_tex_u_next;
						*++dst = ve;
					}
				}
				else
				{
					REAL*	c_uv = _uv + 2 * sec_cur;
					REAL*	n_uv = c_uv + 2;
					INT32	step_uv = (_ring_nb + 1) * 2;

					for( INT32 i=nb; i>0; --i )
					{  
						cpy_v2( dst, c_uv );
						dst += 2;
						c_uv += step_uv;
						cpy_v2( dst, n_uv );
						dst += 2;
						n_uv += step_uv;
					}
					if( !_b_ring_open )
					{
						cpy_v2( dst, c_uv );
						dst += 2;
						cpy_v2( dst, n_uv );
					}
				}
			}
/*
			//	out because same speed but more complex to implement special case ( loop )
			else
			{
				if( !hindex_ready )
				{
					INT32*	p = HINDEX-1;
					for( INT32 i=0; i<H_NB; ++i )
					{  
						*++p = i;
						*++p = i + H_NB;
					}
				}

				INT32	byte_size = nb * sizeof(REAL) * 3;

				MEMCPY( HPOINT,			c,	byte_size );
				MEMCPY( HPOINT+H_NB*3,	n,	byte_size );

				if( need_normal() )
				{
					MEMCPY( HNORMAL,		cn,	byte_size );		
					MEMCPY( HNORMAL+H_NB*3,	nn,	byte_size );
				}
			}
*/
			if( !_b_ring_open )
				++nb;
			guf.draw( c_render::get_cur()->get_draw_primitive(), nb*2, is_normal_draw(), b_uv );
		}
		else
		{
			if( b_uv )
				draw_section_one_old_school_uv( sec_cur, sec_next );
			else
				draw_section_one_old_school( sec_cur, sec_next );
		}
	}
	deal_with_marker_direct( att, sec_cur );
}

//////
FINLINE	void	c_bdd_tube_path::draw_ring_multiple_one( INT32 sec_cur, INT32 src_next )
{
	sec_cur = build_ring_index( sec_cur );
	c_circle* cur	= get_ring_direct( sec_cur );
	c_circle* next	= get_ring( src_next );

	REAL*	c = cur->get_points_to_draw();

	c_tex_anim::cur->bind_this( sec_cur );
	c_multiple::cur->set_index( sec_cur * _ring_point_nb );
	if( c_multiple::cur->is_align_normal() )
	{
		REAL	vu[3];
		REAL	vv[3];
		REAL*	nor = cur->get_rays();
		REAL*	n = next->get_points_to_draw();
	
		sub_v3( vu, c, n );
		if( _b_ring_open )
			sub_then_scale_v3r( vv, c, c+3, 1. );
		else
			sub_then_scale_v3r( vv, c+(_ring_point_nb-1)*3, c+3, .5 );
		c_multiple::cur->draw_one_at_tgn( c, vu, vv, nor );

		c_multiple::cur->next_index_u();
		c += 3;
		n += 3;
		nor += 3;
		for( INT32 i = _ring_point_nb - 1; i > 1; --i )
		{
			sub_v3( vu, c, n );
			sub_then_scale_v3r( vv, c-3, c+3, .5 );
			c_multiple::cur->draw_one_at_tgn( c, vu, vv, nor );
			c_multiple::cur->next_index_u();
			c += 3;
			n += 3;
			nor += 3;
		}

		sub_v3( vu, c, n );
		if( _b_ring_open )
			//was .5 instead of 1. for for ALIGN_NORMAL_OFFSET
			sub_then_scale_v3r( vv, c-3, c, 1. );
		else
			sub_then_scale_v3r( vv, c-3, cur->get_points_to_draw(), .5 );
		c_multiple::cur->draw_one_at_tgn( c, vu, vv, nor );
	}
	else
	{
		for( INT32 i = _ring_point_nb; i > 0; --i )
		{
			c_multiple::cur->align_then_draw( c );
			c_multiple::cur->next_index_u();
			c += 3;
		}
	}
	c_multiple::cur->next_index_v();
}

void	c_bdd_tube_path::draw_normal_point( INT32 sec_start, INT32 sec_stop )
{
	if( _b_build_needed )
		return;
	if ( sec_start < sec_stop )
	{
		for( INT32 i = sec_start; i < sec_stop; ++i )
		if( is_section_allowed( i ) )
			draw_ring_normal_one( i );
	}
	else
	{
		for( INT32 i = sec_start; i >= sec_stop; --i )
		if( is_section_allowed( i ) )
			draw_ring_normal_one( i );
	}
}		

void	c_bdd_tube_path::draw_normal_point( REAL len )
{
	if( _b_build_needed )
		return;
	draw_normal_point( _ring_start, _ring_center );
	draw_normal_point( _ring_stop, _ring_center );
}

template<bool b_uv>
void	c_bdd_tube_path::draw_section( INT32 sec_start, INT32 sec_stop )
{
	if ( sec_start < sec_stop )
	{
		INT32 ic = build_ring_index( sec_start );
		INT32 in;	
		for( INT32 i = sec_start; i < sec_stop; ++i )
		{
			in = build_ring_index( i + 1 );
			draw_section_one<b_uv>( ic, in );
			ic = in;
		}
	}
	else
	{
		INT32 ic;
		INT32 in = build_ring_index( sec_start );
		for( INT32 i = sec_start; i > sec_stop; --i )
		{
			ic =  build_ring_index( i - 1 );
			draw_section_one<b_uv>( ic, in );
			in = ic;
		}
	}
}	

void	c_bdd_tube_path::draw_tube()
{
	if( c_layer::get_cur()->is_need_uv() )
	{
		draw_section<true>( _ring_start,	_ring_center );
		draw_section<true>( _ring_stop,		_ring_center );
	}
	else
	{
		draw_section<false>( _ring_start,	_ring_center );
		draw_section<false>( _ring_stop,	_ring_center );
	}
}

void	c_bdd_tube_path::draw_ring_multiple( INT32 sec_start, INT32 sec_stop )
{
	if ( sec_start < sec_stop )
	{
		for( INT32 i = sec_start; i < sec_stop; ++i )
		if( is_section_allowed( i ) )
			draw_ring_multiple_one(	i, i+1 );
	}
	else							
	{	
		for( INT32 i = sec_start; i > sec_stop; --i )
		if( is_section_allowed( i-1 ) )
			draw_ring_multiple_one(	i-1, i );
	}
}	

void	c_bdd_tube_path::draw_multiple()
{
	if( _b_build_needed )
		return;
	c_multiple::cur->set_nb( _ring_point_nb, _ring_nb );
	draw_ring_multiple( _ring_start,	_ring_center );
	draw_ring_multiple( _ring_stop,		_ring_center );
}

void	c_bdd_tube_path::draw_single()
{
	if( _b_build_needed )
		return;
	GOL::c_tex_unit*	tu = GOL::get_tex_unit_cur();
	_bind_def =  tu->get_bind_2d();
	switch( _s_draw )
	{
	case 0:	draw_ring_line();		break;
	case 1:	draw_ring_polygon();	break;
	case 2:	draw_ring_tri();		break;
	case 3:	draw_tube();			break;
	}
}

