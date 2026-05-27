#include "bdd_tri.h"
#include "infrastructure/seedfile.h"
#include <unordered_map>
#include "draw/render.h"
#include "draw/colorrnd.h"
#include "shaders/shading.h"
#include "draw/mat.h"
#include "draw/tex_anim.h"
#include "draw/seedcam.h"
#include "draw/model.h"
#include "spy.h"
#include "obj_ui/deformer/def_node.h"
#include "gol/gol_draw.h"
#include "gol/gol_color.h"

//todo generate normal
//todo deform deal with normal
#if AAA_POLY_COMPILE()
//todo should move in 3Dutil file
//	done with class ans class
static	REAL	base_uv_3[3][2] =
{
	0., 0.,
	.5, 1.,
	0., 1.,
};
static	REAL	base_uv_4[4][2] =
{
	0., 0.,
	1., 0.,
	1., 1.,
	0., 1.,
};
#endif	//#if AAA_POLY_COMPILE()

/*
FINLINE	void	compute_barycenter_v3r( REAL* bar, REAL* point, INT32 nb )
{
	REAL	min[3];
	REAL	max[3];
	REAL	cen[3];
	REAL*	pt;
	REAL	size;
	REAL	dist;
	INT32	count;

	pt = point;			
	cpy_v3( min, pt );
	cpy_v3( max, pt );
	for( INT32 i = nb-1; i > 0; --i )
	{
		pt += 3;
		min_max_v3r( min, max, pt);
	}
	size = dist_v3r( min, max)*.35;
	add_then_scale_v3r( cen, min, max, .5);

	pt = point;
//	count = 1;
	cpy_v3( bar, cen);
	if( nb > 1 )
	{
		count = 0;
		clear_v3( bar);
		for( INT32 i = nb; i > 0; --i )
		{
			dist = dist_v3r( cen, pt);
			if( dist > size )
			{
				add_v3( bar, pt);
				++count;
			}
			pt += 3;
		}
		scale_v3( bar, 1./REAL(count) );
	}
}
*/

namespace {
	//todo we should have a fn in v.cpp for this
	//todo avoid a message at every line of pb
	FINLINE	void	build_triangle_normal_std( REAL* n, REAL* a, REAL* b, REAL* c )
	{
		REAL	v1[3];
		if( is_equal_v3( a, b ) )
		{
	//todo	have a verbose mode at least give feedback on problems
	//		DBG_PRINT_STRING( "BDD_TRI pb a = b in triangle -> force one normal");
			clear_v3( n );
			return;
		}
		if( is_equal_v3( a, c ) )
		{
	//		DBG_PRINT_STRING( "BDD_TRI pb a = c in triangle -> force one normal");
			clear_v3( n );
			return;
		}
		if( is_equal_v3( b, c ) )
		{
	//		DBG_PRINT_STRING( "BDD_TRI pb b = c in triangle -> force one normal");
			clear_v3( n );
			return;
		}
		sub_v3( v1, b, a );
		sub_v3( n, c, a );
		cross_normalize_v3r( n, n, v1 );
	}
}

c_obj_info::c_obj_info()
	:_point_start(0)
	,_point_nb(0)

	,_tri_start(0)
	,_tri_nb(0)

	,_poly_start(0)
	,_poly_nb(0)
	,_poly_index_start(0)

	,_mtl_index(0)
	,_obj_index(0)
	,_obj_info_id(0)
{

	clear_v3( _barycenter );
	clear_v3( _legend_tra );

	_legend_scale = 1;
};



FACTORY_CREATE_PROP_V1( c_bdd_tri, bdd_tri, 3D Object, tri, sub_menu="Geometry"; sel0="Object"; );

c_instance_by_channel< c_bdd_tri, c_bdd_tri::BDD_TRI_CHANNEL_NB_MAX >	c_bdd_tri::inst_by_channel;

bool		c_bdd_tri::_b_verbose = true;
c_bdd_tri*	c_bdd_tri::cur = nullptr;
namespace
{
	//todoqq clean up this concept and optimize
	C_PCHAR_C	str_tex_change[3] =
	{
		"No",
		"FROM_MTL",
		"FROM_TEX_ANIM",
	};
	C_PCHAR_C	str_clipping_type[3] =
	{
		"BY OBJECT",
		"BY CLUSTER ONLY",
		"BY CLUSTER THEN BY OBJECT",
	};
}

namespace n_bdd_tri
{
	CONSTEXPR INT32 BASE_PARAM_NB		= 1 + c_bdd::GEO_PARAM_NB;
	CONSTEXPR INT32 GEO_PARAM_NB		= 15;
	CONSTEXPR INT32 DRAW_PARAM_NB		= 16;
	CONSTEXPR INT32 MORPH_PARAM_NB		= 1;
	CONSTEXPR INT32 INFO_PARAM_NB		= 18;
	CONSTEXPR INT32 SVG_PARAM_NB		= 4;
	CONSTEXPR INT32 CLIPPING_PARAM_NB	= 22;
	CONSTEXPR INT32 GROUP_PARAM_NB		= 6;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	GEO_PARAM_NB
									+	DRAW_PARAM_NB
									+	MORPH_PARAM_NB
									+	CLIPPING_PARAM_NB
									+	INFO_PARAM_NB
									+	SVG_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
	BDD_BASE_PARAMS

	PARAM_DEF_BOOL_OFF( transform_point )

	PARAM_DEF_GROUP( Geo, GEO_PARAM_NB )
		PARAM_DEF_BOOL_OFF(			model_load_trig			)
		PARAM_DEF_SCALE_XYZF(		scale					)
		PARAM_DEF_BOOL_OFF(			scale_to_unit_trig		)
		PARAM_DEF_BOOL_OFF(			scale_by_object			)
		PARAM_DEF_REAL_ONE(			scale_object			)

		PARAM_DEF_POINT_XYZ(		translation				)
		PARAM_DEF_BOOL_OFF(			center_trig				)
		PARAM_DEF_BOOL_OFF(			flip_x_at_read			)
		PARAM_DEF_BOOL_OFF(			flip_yz_at_read			)
		PARAM_DEF_BOOL_OFF(			unit_center_at_read		)

	PARAM_DEF_GROUP( Draw, DRAW_PARAM_NB )
		PARAM_DEF_BOOL_OFF(			vbo_use					)
		PARAM_DEF_BOOL_OFF(			vbo_stream				)
		PARAM_DEF_BOOL_OFF(			draw_contour			)
//		PARAM_DEF_REAL_POS_ONE(		draw_contour_line		)
		PARAM_DEF_COLOR_RGBA(		draw_contour_color		)
		PARAM_DEF_BOOL_OFF(			draw_file_normal		)
		PARAM_DEF_BOOL_OFF(			force_normal			)

		PARAM_DEF_BOOL_OFF(			draw_uv					)
//		PARAM_DEF_REAL_ONE(			draw_uv_factor			)
//we use it for compatibility even when not define AAA_POLY_COMPILE()
		PARAM_DEF_BOOL_OFF(			draw_poly				)

		PARAM_DEF_SYMBO_PSTR_ZERO(	tex_change,				str_tex_change	)
		PARAM_DEF_INT32_ZERO(		draw_mtl_offset			)

		PARAM_DEF_BOOL_OFF(			tex_object_rotation		)
		PARAM_DEF_BOOL_OFF(			tex_object_stick_bbox	)

		PARAM_DEF_INT32_POS(		skip,					1, 0 )
//		PARAM_DEF_BOOL_OFF(			interpolation			)
//		PARAM_DEF_REAL_ZERO(		interpolation_value		)

	PARAM_DEF_GROUP( Morph, MORPH_PARAM_NB )
		PARAM_DEF_BOOL_ON(			morph_pre_process		)

	PARAM_DEF_GROUP( Info, INFO_PARAM_NB )
		PARAM_DEF_INT32(			channel_id,				2, 1, 1, c_bdd_tri::BDD_TRI_CHANNEL_NB_MAX )
		PARAM_DEF_FILENAME(			model_filename,			aaa::file::TYPE_IO_OBJ_WAVEFRONT, 0 )

		PARAM_DEF_INT32_LOCKED(		model_object_nb			)
		PARAM_DEF_INT32_LOCKED(		model_point_nb			)
		PARAM_DEF_INT32_LOCKED(		model_normal_nb			)
		PARAM_DEF_INT32_LOCKED(		model_uv_nb				)
		PARAM_DEF_INT32_LOCKED(		model_triangle_nb		)
		PARAM_DEF_INT32_LOCKED(		model_poly_nb			)
		PARAM_DEF_XYZ_LOCKED(		bounding_box_center		)
		PARAM_DEF_XYZ_LOCKED(		bounding_box_min		)
		PARAM_DEF_XYZ_LOCKED(		bounding_box_max		)
		PARAM_DEF_INT32_LOCKED(		prim_drawn				)

	PARAM_DEF_GROUP( SVG, SVG_PARAM_NB )
		PARAM_DEF_BOOL_ON(			ignore_hidden_layer		)
//		PARAM_DEF_BOOL_OFF(			recursive				)
		PARAM_DEF_BOOL_OFF(			export_layer_name		)
//		PARAM_DEF_BOOL_OFF(			verbose_svg				)

		PARAM_DEF_BOOL_ON(			tessellate_mesh			)	// SC10
		PARAM_DEF_INT32_POS_ZERO(	tessellate_tri_size		)	// SC10

	PARAM_DEF_GROUP( Clipping, CLIPPING_PARAM_NB )
		PARAM_DEF_BOOL_OFF(			draw_by_object			)
		PARAM_DEF_BOOL_OFF(			draw_by_id				)
		PARAM_DEF_BOOL_OFF(			draw_by_name			)
		PARAM_DEF_REF(				name_selector			)

		PARAM_DEF_INT32_POS_ZERO(	object_start			)
		PARAM_DEF_INT32_POS(		object_stop,			0, PARAM_MAX_INT32 )
		PARAM_DEF_INT32_ZERO(		object_offset_id		)
		PARAM_DEF_BOOL_OFF(			random_color_by_id		)

		PARAM_DEF_INT32_POS_ZERO(	object_legend )
		PARAM_DEF_POINT_XYZ(		object_legend_offset	)
		PARAM_DEF_REAL_ONE(			object_legend_scale		)

		PARAM_DEF_BOOL_ON(			clipping				)
		PARAM_DEF_SYMBO_PSTR_ZERO(	clipping_type,			str_clipping_type )
		PARAM_DEF_INT32_XYZ(		cluster_nb,				12, 1,		1, PARAM_MAX_INT32 )
		PARAM_DEF_INT32_LOCKED(		cluster_nb_out			)
		PARAM_DEF_INT32_INF(		cluster_debug,			1, -1		)
		PARAM_DEF_REAL_POS_ZERO(	clip_dist				)
		PARAM_DEF_REAL(				clip_dist_alpha,		1., 0.,		-1., PARAM_MAX_REAL )
	};
}

//todo	should add something to the base class not to save the last values

void	c_bdd_tri::param_init_pt()
{
	c_obj_info*	info;
	INT32		h = param_init_pt_geo();

	param_set_pt( h, _b_transform_point_ui );

	++h;
		param_set_pt( h, _b_model_load_trig_ui );
		param_set_pt_4( h, _scale_factor_ui );
		param_set_pt( h, _b_scale_to_unit_trig_ui );
		param_set_pt( h, _b_scale_by_object_ui );
		param_set_pt( h, _obj_scale );

		param_set_pt_3( h, _tra_ui );
		param_set_pt( h, _b_center_trig_ui );
		param_set_pt( h, _b_flip_x_at_read_ui );
		param_set_pt( h, _b_flip_yz_at_read_ui );
		param_set_pt( h, _b_unit_center_at_read_ui );

	++h;
		param_set_pt( h, _b_vbo_use_ui );
		param_set_pt( h, _b_vbo_stream_ui );
		param_set_pt( h, _b_draw_contour );
		//param_set_pt( h, _contour_line_size );
		param_set_pt_4( h, _color_contour );
		param_set_pt( h, _b_normal_from_file_ui );
		param_set_pt( h, _b_normal_force_ui );

		param_set_pt( h, _b_draw_uv );
//		param_set_pt( h, _uv_factor );
	
		param_set_pt( h, _b_poly_draw_ui );
		param_set_pt( h, _s_tex_change );
		param_set_pt( h, _mtl_offset );
		param_set_pt( h, _b_tex_object_rot_ui );
		param_set_pt( h, _b_tex_object_stick_bbox_ui );

		param_set_pt( h, _skip );	//todo move this to a 0. to 1. param
//		param_set_pt( h, _b_interpolation_ui );
//		param_set_pt( h, _interpolation_value_ui );

	++h;
		param_set_pt( h, _b_morph_pre_process_ui );
		
	++h;
		param_set_pt( h, _channel_id );
		param_set_pt( h, _model_filename );

		param_set_pt( h, _obj_nb );
	//	index_point_for_ui = h;
		param_set_pt( h, _nb_point );
		param_set_pt( h, _nb_normal );
		param_set_pt( h, _nb_uv );
		param_set_pt( h, _nb_tri );
		param_set_pt( h, _nb_poly );

		param_set_pt_3( h, _bbox_origin	);
		param_set_pt_3( h, _bbox_min		);
		param_set_pt_3( h, _bbox_max		);

		param_set_pt( h, _prim_drawn );

	++h;
		param_set_pt( h, _b_svg_ignore_hidden_layer );
//		param_set_pt( h, _b_svg_recursive );
		param_set_pt( h, _b_svg_export_layer_name );
//		param_set_pt( h, _b_verbose_svg );
		param_set_pt( h, _b_tessellate_mesh );
		param_set_pt( h, _tessellate_tri_size );

	++h;
		param_set_pt( h, _b_draw_by_obj_ui );
		param_set_pt( h, _b_draw_by_id_ui );
		param_set_pt( h, _b_draw_by_name_ui );
		param_set_pt( h, _name_selector );
		param_set_pt( h, _obj_start);
		param_set_pt( h, _obj_stop);
		param_set_pt( h, _obj_offset_id );
		param_set_pt( h, _b_random_color_by_id_ui );


	param_set_pt( h, _legend_id );
	info = get_info( _legend_id );
	if( info )
	{
		param_set_pt_3( h, info->_legend_tra );
		param_set_pt( h, info->_legend_scale );
	}
	else
	{
		param_set_pt_null( h );
		param_set_pt_null( h );
		param_set_pt_null( h );
		param_set_pt_null( h );
	}

	param_set_pt( h, _b_clipping_ui );
	param_set_pt( h, _s_clipping_type );
	param_set_pt_3( h, _cluster_nb_ui );
	param_set_pt( h, _cluster_nb_out );
	param_set_pt( h, _cluster_debug );
	param_set_pt( h, _clip_dist );
	param_set_pt( h, _clip_dist_alpha );
	
	err_param_init_pt( h );
}

AAA_ERR	c_bdd_tri::load_do_after( o_str CONST & filename )
{
	//pfizer
	//todoq don't reload a file
	//	check date like for images
	if( !_tri_index_point )	//todo	in case of reload the model is not reloaded
		_b_model_load_needed = true;
	return	AAA_OK;
}

//we generate both and _vao act as a flag
void	c_bdd_tri::vbo_vao_init()
{
	if( _vao )	return;
	// generate VBOs
	GOL::gen_vbo( _vbo_vertex );
	GOL::gen_vbo( _vbo_index );
	GOL::gen_vbo( _vbo_normal );
	GOL::gen_vbo( _vbo_tex );
	_b_vbo_transfered = false;
	// generate VAO
	GOL::gen_vao( _vao );
}

void	c_bdd_tri::vbo_vao_deinit()
{
	for( INT32 i=0; i<MORPH_NB_MAX*2; ++i )
		GOL::delete_vbo( _vbo_attr[i] );

	if( !_vao )	return;

//maa they were never bind so why unbind
//	GOL::unmap_buffer( _vbo_tex );
//	GOL::unmap_buffer( _vbo_normal );
//	GOL::unmap_buffer( _vbo_index );
//	GOL::unmap_buffer( _vbo_vertex );
	GOL::unbind_vao();

	GOL::delete_vbo( _vbo_tex );
	GOL::delete_vbo( _vbo_normal );
	GOL::delete_vbo( _vbo_index );
	GOL::delete_vbo( _vbo_vertex );
	_b_vbo_transfered = false;

	GOL::delete_vao( _vao );
}

void	c_bdd_tri::init_transfo()
{
	_scale_factor[0] = _scale_factor_ui[0] + REAL(1);
	_scale_factor[1] = _scale_factor_ui[1] + REAL(1);
	_scale_factor[2] = _scale_factor_ui[2] + REAL(1);
	_scale_factor[3] = _scale_factor_ui[3] + REAL(1);

	_tra[0] = _tra_ui[0] + REAL(1);
	_tra[1] = _tra_ui[1] + REAL(1);
	_tra[2] = _tra_ui[2] + REAL(1);
}

void	c_bdd_tri::init_pt_data()
{
//	_point_interpolated = nullptr;
	for( INT32 i=0; i<MORPH_NB_MAX*2; ++i )
	{
		_attrib_for_tri[i]	= nullptr;
		_vbo_attr[i]		= 0;
		_attrib_location[i]	= 0;
		_attrib_vbo[i]		= 0;
	}

//	_point_for_normal = nullptr;
	_p_point_to_drawing	= nullptr;

//	_tri_from_file_x3 = nullptr;
	_tri_index_point	= nullptr;
	_tri_index_normal	= nullptr;
	_tri_index_uv		= nullptr;
	_tri_index			= nullptr;

//	_normal_by_point	= nullptr;
//	_normal_by_point_interpolated = nullptr;

	//	_normal_for_tri_interpolated = nullptr;

	_obj_info			= nullptr;

	_nb_point	= 0;
	_nb_normal	= 0;
	_nb_uv		= 0;
	_nb_tri		= 0;
	_nb_poly	= 0;

	_b_to_make_normal		= true;
	_b_valid_data			= false;

	_b_point_need_transform = true;
//	_b_point_for_normal_to_place = true;
	_b_need_build			= true;

#if AAA_POLY_COMPILE()
	_poly_count				= nullptr;
	_poly_index_from_file	= nullptr;
	_poly_index_for_normal	= nullptr;
#endif

	clear_v3( _bbox_origin	);
	clear_v3( _bbox_min	);
	clear_v3( _bbox_max	);

	_prim_drawn		= 0;
	_cluster_nb_out	= 0;

//	_b_draw_normal_from_file = false;
}

void c_bdd_tri::init()
{
	set_draw_before();

	_b_model_load_needed = true;
	_model_filename.erase();

	_vbo_vertex	= 0;
	_vbo_normal	= 0;
	_vbo_tex	= 0;
	_vbo_index	= 0;
	_b_vbo_transfered	= false;
	_vbo_in_vao			= false;

	//these should not be need but are here to be sure
	_b_index_short		= true;
#if AAA_POLY_COMPILE()
	_b_poly_in_file		= false;
#endif
	_b_normal_in_file	= false; 
	_b_uv_in_file		= false;
	_b_skip_clipping	= false;
	_b_point_need_transform = true;

	init_pt_data();
	init_transfo();
	cluster_init();
}

void c_bdd_tri::alloc_obj( INT32 nb )
{
	if( nb != 1 )
		++nb;
	_obj_info = new c_obj_info[nb];
}


void	c_bdd_tri::dealloc_buffer_real( REAL*& buf )
{
	SAFE_DELETE_ARRAY( buf );
}

REAL*	c_bdd_tri::alloc_point_data_for_triangle( REAL*& buf, INT32 nb_tri, INT32 dim, CHAR* str_verbose )
{
	dealloc_buffer_real( buf );
	INT32 nb = nb_tri * 3 * dim;	//	3 for triangle
	buf = new REAL[ nb ];
	return buf;
}

void	c_bdd_tri::alloc_tri( INT32 nb, INT32 nb_normal, INT32 nb_uv )
{
	INT32	nb_index = nb * 3;
	_tri_index_point		= new UINT32[ nb_index ];
	_tri_index_normal		= new UINT32[ nb_index ];
	_tri_index_uv			= new UINT32[ nb_index ];

//	_tri_from_file_x3		= new INT32[ nb_real ];
	//todoq clean _normal_by_face
//	alloc_point_data_for_triangle( _normal_for_tri_interpolated,	nb, 3 );


	if(	_tri_index_point && _tri_index_normal && _tri_index_uv
//			&& _tri_from_file_x3
//			&& _tri_index_normal_out
//			&& _tri_index_point_out
//			&& _normal_for_tri_interpolated
		)
		_nb_tri = nb;
	else
		geo_dealloc();
}

#if AAA_POLY_COMPILE()
void c_bdd_tri::alloc_poly( INT32 nb_tri_in, INT32 nb_poly_in )
{
	if( nb_poly_in )
	{
		_poly_count				= new UINT32[ nb_poly_in ];
		_poly_index_from_file	= new UINT32[ nb_tri_in + 2 * nb_poly_in ];
		_poly_index_for_normal	= new UINT32[ nb_tri_in + 2 * nb_poly_in ];

		if( _poly_count && _poly_index_from_file && _poly_index_for_normal )
			_nb_poly = nb_poly_in;
		else
			geo_dealloc();
	}
}
#endif

bool c_bdd_tri::geo_alloc( INT32 nb_obj, INT32 nb_point, INT32 nb_normal, INT32 nb_uv, INT32 nb_tri, INT32 nb_poly )
{
	c_vertex_data* vd = &_vertex_data[0];

//will have to reintroduce
//	_point_interpolated				= new REAL[ nb_real ];
//	_normal_by_point				= new REAL[ nb_real ];
//	_normal_by_point_interpolated	= new REAL[ nb_real ];

	vd->alloc_point(	nb_point	);
	_nb_point	= vd->get_point_nb();

	vd->alloc_normal(	nb_normal	);
	_nb_normal	= vd->get_normal_nb();

	vd->alloc_uv(		nb_uv		);
	_nb_uv		= vd->get_uv_nb();

	alloc_tri(		nb_tri, nb_point, nb_normal			);
#if AAA_POLY_COMPILE()
	alloc_poly(		nb_tri,	nb_poly	);
#endif
	alloc_obj(		nb_obj			);	//todo check this alloc

	_b_vbo_transfered = false;
	//todo we should check object alloc too
	return	nb_point	==	_nb_point
		||	nb_normal	==	_nb_normal
		||	nb_uv		==	_nb_uv
		||	nb_tri		==	_nb_tri
		||	nb_poly		==	_nb_poly;
}

void c_bdd_tri::geo_dealloc()
{
	for( INT32 i=0; i<5; ++i )
		_vertex_data[i].dealloc();

//	SAFE_DELETE_ARRAY( _point_interpolated );
//	SAFE_DELETE_ARRAY( _point_for_normal );
//	SAFE_DELETE_ARRAY( _normal_by_point );
//	SAFE_DELETE_ARRAY( _normal_by_point_interpolated );
//	SAFE_DELETE_ARRAY( _tri_from_file_x3 );
	SAFE_DELETE_ARRAY( _tri_index_point );
	SAFE_DELETE_ARRAY( _tri_index_normal );
	SAFE_DELETE_ARRAY( _tri_index_uv );

	if( _tri_index )
	{
		if( _b_index_short )
			delete [] ((UINT16*)_tri_index );
		else
			delete [] ((UINT32*)_tri_index );
		_tri_index = nullptr;
	}

	//	dealloc_buffer_real(	_normal_for_tri_interpolated );
	for( INT32 i=0; i<=MORPH_NB_MAX; ++i )
		_vertex_data_out[i].dealloc();

	for( INT32 i=0; i<MORPH_NB_MAX*2; ++i )
		dealloc_buffer_real( _attrib_for_tri[i] );

#if AAA_POLY_COMPILE()
	SAFE_DELETE_ARRAY( _poly_count );
	SAFE_DELETE_ARRAY( _poly_index_from_file );
	SAFE_DELETE_ARRAY( _poly_index_for_normal );
	SAFE_DELETE_ARRAY( _obj_info );
#endif

	cluster_dealloc();
	_b_vbo_transfered = false;
//	_uv_factor_last = -42.2655448;	//hack but reset generation of uv_for_tri
}

CONSTRUCTOR_CREATE(c_bdd_tri)
	,_obj_info(nullptr)	//todo this is here and not in init() to avoid crash in param_printui
	,_obj_nb(0)
	,_b_svg_text(false)
	,_p_tri_to_draw(nullptr)
	,_p_normal_to_draw(nullptr)
	,_p_uv_to_draw(nullptr)
	,_b_random_color_by_id(false)
#if AAA_POLY_COMPILE()
	,_p_poly_index_to_draw(nullptr)
	,_b_poly_draw(false)
#endif
	,_b_vbo_use(false)
	,_vao(0)
	,_shading_unique_id(0)	//hack but should always be good the first time
	,_attr_nb(0)
	,_unique_point_nb(0)
{
	param_init_with( n_bdd_tri::param, n_bdd_tri::PARAM_NB_MAX );//  bdd_tri_param, BDD_TRI_PARAM_NB_MAX);

	init();
}

c_bdd_tri::~c_bdd_tri()
{
	if( cur == this )
		cur = nullptr;
	geo_dealloc();
	vbo_vao_deinit();
}

void	c_bdd_tri::center( REAL* min, REAL* max )
{
	REAL	cen[3];
	add_then_scale_v3r( cen, min, max, -.5 );
	add_v3( _tra_ui, cen ); 
//	mul_scale_v3( _tra_ui, cen, _scale_factor_ui, _scale_factor_ui[3] );
//	cen[0] /=  _scale_factor_ui[0] * _scale_factor_ui[3];
//	cen[1] /=  _scale_factor_ui[1] * _scale_factor_ui[3];
//	cen[2] /=  _scale_factor_ui[2] * _scale_factor_ui[3];
//	cpy_v3( _tra_ui, cen ); 
	_b_center_trig_ui = false;
}

//todo	check if no pb because of _point_for_normal

void	c_bdd_tri::center()
{
	REAL	min[3];
	REAL	max[3];
	min_max_v3r( min, max, _vertex_data[0].get_point(), _nb_point );
	center( min, max );
}

//todo	avoid to much bbox unuseful precal
void	c_bdd_tri::scale_to_unit()
{
	REAL	min[3];
	REAL	max[3];
	min_max_v3r( min, max, _vertex_data[0].get_point(), _nb_point );

	REAL size = aaa::MAX( max[0]-min[0], max[1]-min[1], max[2]-min[2] );
	if( size != 0. )
		_scale_factor_ui[3] /= size;
	else
		_scale_factor_ui[3] = 1.;
	_b_scale_to_unit_trig_ui = false;

	//hack
	//	center(min, max);
}

void	c_bdd_tri::transform_point( c_vertex_data* vd, c_obj_info* info )
{
	REAL	scale[3];
	scale_v3( scale, _scale_factor, _scale_factor[3] );

	REAL	center[3];
	info->_bbox.get_origin( center );

	REAL	vec[3];
	REAL*	dst = vd->get_point_transformed() + info->_point_start * 3;
//todo make it //
	for( INT32 i = info->_point_nb; i > 0; --i )
	{
		sub_v3( vec, dst, center );
		add_scale_v3( dst, center, vec, _obj_scale );
		dst += 3;
	}
}

void	c_bdd_tri::transform_point( REAL* dst, REAL *src, INT32 nb )
{
	REAL	scale[3];
	scale_v3( scale, _scale_factor, _scale_factor[3] );

	mul_add_v3_nb( dst, src, scale, _tra, nb );
}

void	c_bdd_tri::transform_contour()
{
	//	INT32	i;
	REAL	scale[3];
	scale_v3( scale, _scale_factor, _scale_factor[3] );

	for( size_t i = 0; i < _svg_contour.size(); ++i )
	{
		auto& contours = _obj_info[ i + 1 ]._contours;
		contours.clear();
		contours.resize( _svg_contour[ i ]._contours.size() );
		//	std::vector<Contour>	vector_contour_tmp;

		for( size_t j = 0; j < _svg_contour[ i ]._contours.size(); ++j )
		{
			Contour	contour_tmp;
			contour_tmp.resize( _svg_contour[ i ]._contours[ j ].size() );
			for( size_t k = 0; k < _svg_contour[ i ]._contours[ j ].size(); ++k )
			{
				Vertex	tmp;
				tmp.x = _svg_contour[ i ]._contours[ j ][ k ].x * scale[0] + _tra[ 0 ];
				tmp.y = _svg_contour[ i ]._contours[ j ][ k ].y * scale[1] + _tra[ 1 ];
				tmp.z = _svg_contour[ i ]._contours[ j ][ k ].z * scale[2] + _tra[ 2 ];
				//contour_tmp.push_back( tmp );
				contour_tmp[k] =  tmp;
			}
			//_obj_info[ i + 1 ]._contours.push_back( contour_tmp );
			contours[j] = contour_tmp;
		}
	//	_obj_info[ i + 1 ]._contours.push_back( vector_contour_tmp );
	}

	//mul_add_v3_nb( dst, src, scale, _tra, nb );
}


//INT32	hacktest = 0;
void	c_bdd_tri::compute_bbox()
{
	INT32	i;
	if( _obj_nb == 1 )
		i = 0;
	else
		i = _obj_nb;
	//todo use a special case for the global object
	// not to reparse all;
	for( ; i >= 0; --i )
	{
		compute_bbox( &_obj_info[i] );
		/*
		compute_bsphere( &obj_info[i] );
		if( obj_info[i]._bbox.radius_sphere < obj_info[i]._bbox.radius )
			++hacktest;
		else if( obj_info[i]._bbox.radius_sphere > obj_info[i]._bbox.radius )
			--hacktest;
			DBG_PRINT_STRING( "hacktest is %d sp %f %f", hacktest, (DOUBLE)obj_info[i]._bbox.radius_sphere , (DOUBLE)obj_info[i]._bbox.radius);
		*/
		compute_barycenter_and_co( &_obj_info[i] );
	}
	_obj_info->_bbox.get_origin(	_bbox_origin	);
	_obj_info->_bbox.get_min(		_bbox_min		);
	_obj_info->_bbox.get_max(		_bbox_max		);
}

//todo check index
REAL*	c_bdd_tri::get_point_pt( INT32 CONST index )
{
	return get_points() + index*3;
}

FINLINE	c_obj_info*	c_bdd_tri::get_info( INT32 control_id )
{
	//DBG_PRINT_STRING( "get_info(%d) with obj_nb %d", control_id, obj_nb);
	if( _obj_nb == 1 )
		return _obj_info;
	else
	{
		INT32	index = CLAMP( control_id, 1, _obj_nb );
		if( _obj_by_id.empty() )
			return	nullptr;
		else if( _b_draw_by_id_ui )
			return	_obj_by_id[ index - 1 ];
		else
			return	_obj_by_index[ index - 1 ];
	}
}

//hack the calculation have to be cleaned up
C_PCHAR_C	c_bdd_tri::get_name_from_control( INT32 control_id )					{	return get_info( control_id )->_name.get();			}		

//hackqqq name should change
//	should use the object factor too
//	should defebd here and there on info == NULL ???
void	c_bdd_tri::get_tex_center_from_control( DOUBLE* pt, INT32 control_id )
{
	c_obj_info*	info = get_info( control_id );
	info->_bbox.get_origin( pt );
	add_v3( pt, info->_legend_tra );
}

//hackqqq extend to 4 field
void	c_bdd_tri::get_tex_size_from_control( DOUBLE* pt, INT32 control_id )
{
	c_obj_info*	info = get_info( control_id );
	info->_bbox.get_size( pt );
	scale_v3( pt, info->_legend_scale );
}

/*
void	c_bdd_tri::compute_bsphere( c_obj_info* info )
{
	REAL*	(min_pt[3]);
	REAL*	(max_pt[3]);	
	REAL*	pt;
	INT32	i;
	REAL	tmp;
	REAL	span;
	REAL	cen[3];
	REAL	rad;

	//	Graphics gems I page 301 and 723
	//	first pass	find 6 minima/maxima points	
	pt = _point_placed+info->_point_start*3;

	min_pt[0] = pt;
	min_pt[1] = pt;
	min_pt[2] = pt;

	max_pt[0] = pt;
	max_pt[1] = pt;
	max_pt[2] = pt;

	for( i = info->_point_nb-1; i>0; --i )
		{
		pt += 3;

		tmp = *pt;
		if( tmp < min_pt[0][0] )
			min_pt[0] = pt;
		else if( tmp < max_pt[0][0] )
			max_pt[0] = pt;
		tmp = *(pt+1);
		if( tmp < min_pt[1][1] )
			min_pt[1] = pt;
		else if( tmp < max_pt[1][1] )
			max_pt[1] = pt;
		tmp = *(pt+2);
		if( tmp < min_pt[2][2] )
			min_pt[2] = pt;
		else if( tmp < max_pt[2][2] )
			max_pt[2] = pt;
		}

// find the pair maximally separated
	{
	REAL*	pt1;
	REAL*	pt2;
	pt1 = min_pt[0];
	pt2 = max_pt[0];
	span = dist_squared_v3r( min_pt[0], max_pt[0]);

	tmp = dist_squared_v3r( min_pt[1], max_pt[1]);
	if( tmp > span )
		{
		span = tmp;
		pt1 = min_pt[1];
		pt2 = max_pt[1];
		}
	tmp = dist_squared_v3r( min_pt[2], max_pt[2]);
	if( tmp > span )
		{
		pt1 = min_pt[2];
		pt2 = max_pt[2];
		}

	// build center and radius with it
	add_then_scale_v3r( cen, pt1, pt2, .5);
	span = dist_squared_v3r( pt1, cen);
	rad = SQRT( span);
	}

	// second pass increment if necessary
	pt = _point_placed+info->_point_start*3;
	for( i = info->_point_nb; i>0; --i )
		{
		tmp = dist_squared_v3r( pt, cen );
		if( tmp > span )
			{
			REAL	old2new;

			tmp = SQRT( tmp);
			//radius of new sphere
			rad = (rad + tmp) * .5;
			span = rad*rad;

			old2new = tmp-rad;
			//center of new sphere
			cen[0] = ( rad*cen[0] + old2new*pt[0]) / tmp;
			cen[1] = ( rad*cen[1] + old2new*pt[1]) / tmp;
			cen[2] = ( rad*cen[2] + old2new*pt[2]) / tmp;
			}
		pt += 3;
		}
	cpy_v3( info->_bbox.origin, cen );
	info->_bbox.radius_sphere = rad;
}
*/

void	c_bdd_tri::compute_barycenter_and_co( c_obj_info* info )
{
	INT32	nb = info->_tri_nb;
	INT32	index;
	UINT32*	p = _tri_index_point + info->_tri_start * 3;
	REAL	bar[3];
	REAL	f[9];

//	nb = info->_tri_nb;
	
//	p = _tri_index_point + info->_tri_start * 3;
	clear_v3( bar );
	REAL*	point = _vertex_data[0].get_point_transformed();
//todo make it //
	for( INT32 i = nb; i > 0; --i )
	{
		index = *p * 3;
		cpy_v3( f, point + index );
		add_v3( bar, f );
		++p;

		index = *p * 3;
		cpy_v3( f + 3, point + index );
		add_v3( bar, f + 3);
		++p;

		index = *p * 3;
		cpy_v3( f + 6, point + index );
		add_v3( bar, f + 6 );
		++p;
	}
	
	scale_v3( info->_barycenter, bar, 1. / ( REAL(nb) * 3.0f ) );
}


void	c_bdd_tri::compute_bbox( c_obj_info* info )
{
	//todoqq check this
	//todo should calculate distance to origin
	info->_bbox.extend_v3r( _vertex_data[0].get_point_transformed() + info->_point_start * 3, info->_point_nb );

//	compute_barycenter_v3r( info->_barycenter, _point_placed+info->_point_start*3, info->_point_nb);
}

void	c_bdd_tri::transform()
{
	if( is_diff_v4( _scale_factor, _scale_factor_ui) || is_diff_v3( _tra, _tra_ui) )
	{
		cpy_v4( _scale_factor, _scale_factor_ui );
		cpy_v3( _tra, _tra_ui );

		_b_point_need_transform = true;
//		_b_point_for_normal_to_place = true;
		_b_need_build = true;
	}
	if( _b_point_need_transform || _b_transform_point_ui )
	{
		for( INT32 i=0; i<=MORPH_NB_MAX; ++i )
		{
			c_vertex_data& vd = _vertex_data[i];
			REAL* src = vd.get_point_original();
			if( src )
			{
				vd.alloc_point_transformed();
				transform_point( vd.get_point_transformed(), vd.get_point_original(), _nb_point );
				vd.set_transformed( true );
			}
		}
		transform_contour();
		_b_point_need_transform = false;
		compute_bbox();
	}
	if( _b_scale_by_object_ui )
	{
		INT32	i;
		if( _obj_nb == 1 )
			i = 0;
		else
			i = _obj_nb;
		//todo use a special case for the global object
		// not to reparse all;
		for( ; i > 0 ; --i )
			transform_point( &_vertex_data[0], &_obj_info[i] );
	}
}

//todo rename and generalize by passing src and dst
/*
void	c_bdd_tri::prepare_uv_for_tri()
{
	if( !_b_uv_in_file )
		return;

	UINT32*	puvi = _tri_index_uv - 1;
	REAL*	dst = _vertex_data_out.get_uv() - 1;
	REAL*	uv = _vertex_data[0].get_uv();
	for( INT32 i = _nb_tri; i > 0; --i )
	{
		REAL*	src	=	uv + *++puvi * 2;
		*++dst = *src	* _uv_factor;
		*++dst = *++src * _uv_factor;

		src	=	uv + *++puvi * 2;
		*++dst = *src	* _uv_factor;
		*++dst = *++src * _uv_factor;

		src	=	uv + *++puvi * 2;
		*++dst = *src	* _uv_factor;
		*++dst = *++src * _uv_factor;
	}
}
*/

bool	c_bdd_tri::build_exp()
{
/*	if( !_vertex_data[0].get_uv() )
	{
		//todo	we should merge on normal here (with a flag)
		return false;
	}
*/

	SPY_PUSH_RANGE( "vertex sort", spy::COL_1 );

		typedef std::unordered_map< INT64, INT32 > MERGER;
		MERGER	merger;
		MERGER::iterator	it;

		//	here we prepare a merge of the point with the same index for geometry and uv
		//		but we ignore normal
		UINT32*		pp	= _tri_index_point	- 1;
		UINT32*		puv = (_nb_uv > 0) ? (_tri_index_uv	- 1) : nullptr;
		UINT32*		pn	= _tri_index_normal;

//todo make it //
		for( INT32 nb = _nb_tri*3; nb>0; --nb )
		{
			INT64	first = INT64(*++pp) << 32;
			if( puv )
				first |= *++puv;
			it = merger.find( first );
			if( it == merger.end() )
				merger[ first ] = *pn;
			++pn;
		}

		INT32	size = (INT32)merger.size();

	SPY_POP_RANGE();
	GOOD_PRINT_STRING( "point and uv merge will generated %d unique vertex", merger.size() );

	SPY_PUSH_RANGE( "vertex rearange", spy::COL_2 );

		MERGER::iterator	it_end	= merger.end();

		for( INT32 i=0; i<=MORPH_NB_MAX; ++i )
		{
			c_vertex_data& vd_src	= _vertex_data[i];
			if( !vd_src.get_point() )
				continue;

			c_vertex_data& vd_out	= _vertex_data_out[i];
			vd_out.alloc( size, _nb_normal > 0, (_nb_uv > 0) && (i == 0) );

			REAL*	p	=	vd_out.get_point();
			REAL*	n	=	vd_out.get_normal();
			REAL*	uv	=	vd_out.get_uv();

			bool	b_pre_process;
			if( i > 0 )
			{
				b_pre_process = _b_morph_pre_process_ui;
				GOL::gen_vbo( _vbo_attr[(i-1)*2] );
				if( n ) 
					GOL::gen_vbo( _vbo_attr[(i-1)*2+1] );
			}
			else
				b_pre_process = false;

			it = merger.begin();
			if( b_pre_process )
			{
				c_vertex_data&	vd_ref = _vertex_data[0];
				for( ; it!=it_end; ++it )
				{
					INT64	first = it->first;
					INT32	ip	= (first >> 32) * 3;	
					//	copy data
						sub_v3(	p,	vd_src.get_point() + ip,	vd_ref.get_point() + ip		);
						p += 3;
					if( n )
					{
						INT32	in = (it->second) * 3;
						sub_v3(	n,	vd_src.get_normal() + in,	vd_ref.get_normal() + in	);
						n += 3;
					}
					if( uv )
					{
						INT32	iuv	= (first & 0xffffffff) * 2;
						sub_v2(	uv,	vd_src.get_uv()	+ iuv,		vd_ref.get_uv() + iuv		);
						uv += 2;
					}
				}
			}
			else
			{
				for( ; it!=it_end; ++it )
				{
					INT64	first = it->first;
					INT32	ip	= (first >> 32) * 3;	
					//	copy data
						cpy_v3(	p,	vd_src.get_point() + ip		);
						p += 3;
					if( n )
					{
						INT32	in = (it->second) * 3;
						cpy_v3(	n,	vd_src.get_normal() + in	);
						n += 3;
					}
					if( uv )
					{
						INT32	iuv	= (first & 0xffffffff) * 2;
						cpy_v2(	uv,	vd_src.get_uv()	+ iuv		);
						uv += 2;
					}
				}
			}
		}

		INT32 i = 0;
//todo make it //
		for( it	= merger.begin(); it!=it_end; it++ )
		{
			it->second = i;	//	store index
			++i;
		}

		pp	= _tri_index_point	- 1;
		puv = (_nb_uv > 0) ? (_tri_index_uv	- 1) : nullptr;

		if( i > 65536 )
		{
			_b_index_short = false;
			UINT32* dst	= new UINT32[ _nb_tri*3 ];
			_tri_index = dst;
			--dst;
//todo make it //
			for( INT32 nb = _nb_tri*3; nb>0; --nb )
			{
				INT64	first = INT64(*++pp) << 32;
				if( puv )
					first |= *++puv;
				it = merger.find( first );
				*++dst = it->second;
			}
		}
		else
		{
			_b_index_short = true;
			UINT16* dst	= new UINT16[ _nb_tri*3 ];
			_tri_index = dst;
			--dst;
//todo make it //
			for( INT32 nb = _nb_tri*3; nb>0; --nb )
			{
				INT64	first = INT64(*++pp) << 32;
				if( puv )
					first |= *++puv;
				it = merger.find( first );
				*++dst = it->second;
			}
		}

	SPY_POP_RANGE();

	//todo deal with the poly case
	return true;
}

void	c_bdd_tri::build()
{
	if( !build_exp() )
	{
		_vertex_data_out[0].alloc( _nb_tri * 3, _nb_normal > 0, _nb_uv > 0 );

		build_index_tri( _nb_tri );

		build_point_for_tri(	_vertex_data_out[0].get_point(),	_vertex_data[0].get_point()		);
		build_normal_for_tri(	_vertex_data_out[0].get_normal(),	_vertex_data[0].get_normal()	);
		build_uv_for_tri(		_vertex_data_out[0].get_uv(),		_vertex_data[0].get_uv()		);

#if AAA_POLY_COMPILE()
		build_point_for_poly();
#endif

		for( INT32 i=1; i<=MORPH_NB_MAX; ++i )
		{
			c_vertex_data& vd		= _vertex_data[i];
			c_vertex_data& vd_out	= _vertex_data_out[i];
			REAL* src = vd.get_point();
			if( src )
			{
				vd_out.alloc( _nb_tri*3, vd.get_normal()!=nullptr, false );
				build_point_for_tri( vd_out.get_point(), src );
				GOL::gen_vbo( _vbo_attr[(i-1)*2] );
				src = vd.get_normal();
				if( src )
				{
					build_normal_for_tri( vd_out.get_normal(), src );
					GOL::gen_vbo( _vbo_attr[(i-1)*2+1] );
				}
			}
		}
	}

	_b_vbo_transfered = false;
	_b_need_build = false;
}

//todo manage to cut the memmove when not needed
//todo manage than draw could be called without update done 
void	c_bdd_tri::update()
{
	if( c_multiple::cur->is_render_multiple() )
	{
		// render is multiple
		_weight = c_multiple::cur->get_parameter( 1 );
	}
	_prim_drawn = 0;
	if( _b_model_load_trig_ui )
	{
		_b_model_load_needed	= true;
		_b_model_load_trig_ui	= false;
	}
	if( _b_flip_yz_at_read != _b_flip_yz_at_read_ui )
	{
		_b_flip_yz_at_read		= _b_flip_yz_at_read_ui;
		_b_model_load_needed	= true;
	}
	if( _b_flip_x_at_read != _b_flip_x_at_read_ui )
	{
		_b_flip_x_at_read		= _b_flip_x_at_read_ui;
		_b_model_load_needed	= true;
	}

#if AAA_POLY_COMPILE()
	_b_poly_draw = _b_poly_draw_ui && GOL::is_poly_use() && _b_poly_in_file ;
#endif
	if( _b_vbo_use_ui )
	{
		_b_vbo_use = GOL::is_vbo();
		if( _b_vbo_use )
			vbo_vao_init();
	}
	else
		 _b_vbo_use = false;

	if( _b_model_load_needed )
		load_data( _model_filename );
	if( !_b_valid_data )
		return;
	inst_by_channel.set( _channel_id, this );
//	bool	b_loc_inter;
	if( _b_center_trig_ui )
		center();
	if( _b_scale_to_unit_trig_ui )
		scale_to_unit();
	transform();
	_b_skip_clipping = !_b_clipping_ui || !gb_allow_clipping;

	//no interpolation for now
	/*
	if( _b_interpolation_ui && _interpolation_value != 1. )
	{
		if( c_bdd_tri::cur && ( _nb_point == c_bdd_tri::cur->_nb_point ) )
			b_loc_inter = true;
		else
			b_loc_inter = false;
	}
	else
		b_loc_inter = false;

	if( b_loc_inter )
	{
		if( _interpolation_value == 0. )
		{
			//hack
			_b_need_build = true;
			_p_point_to_deform = c_bdd_tri::cur->_p_point_to_deform;
		}
		else
		{
			//hack
			_b_need_build = true;
			interpolate_v3r_list( _point_interpolated, c_bdd_tri::cur->_p_point_to_deform, _vertex_data[0].get_point_transformed(),
								_interpolation_value, _nb_point);
			_p_point_to_deform = _point_interpolated;
		}
	}
	else
	*/

	//todo suppress or redo
	/*
	if( _uv_factor_last != _uv_factor )
	{
		prepare_uv_for_tri();
		_uv_factor_last = _uv_factor;
	}
	*/

	//todo we force normal from file for now
	//if( _nb_normal && _b_draw_normal_from_file )//&& (!_b_poly || !_b_draw_poly) )
//	if( _nb_normal )
	{
		//todoq optimize
		if( _b_need_build )
			build();

		//	_p_point_for_drawing = _point_for_normal;
		_unique_point_nb		=	_vertex_data_out[0].get_point_nb();
		_p_point_to_drawing		=	_vertex_data_out[0].get_point();
		_p_normal_to_draw		=	_vertex_data_out[0].get_normal();
		_p_uv_to_draw			=	_vertex_data_out[0].get_uv();
		//	_p_tri_to_draw		=	_tri_for_normal;
		_p_tri_to_draw			=	_tri_index;
#if AAA_POLY_COMPILE()
		_p_poly_index_to_draw	=	_poly_index_for_normal;
#endif
	}


/*
	else
	{
		_p_point_for_drawing = _p_point;
		_p_tri_to_draw = _tri_index_point;
		_p_uv_to_draw = _vertex_data[0].get_uv();
		_p_poly_index_to_draw = _poly_index_from_file;
		if( !_b_force_normal )
		{
			make_normal();
			//todo	if( c_def_node::get_cur()->is_deforming() )
			//			_b_to_make_normal = true;
			if( c_render::get_cur()->is_gouraud() )
				_p_normal_to_draw = _normal_by_point;
			else
				_p_normal_to_draw = _normal_by_point;
		}
	}
*/

	if( is_normal_draw() )
	{
		if( _b_normal_force_ui )
			_b_normal_do =  false;
		else if( _p_normal_to_draw )
			_b_normal_do =  true;
		else
		{
			_b_normal_do =  false;
			err_print( "no normal to draw" );
		}
	}
	else
		_b_normal_do =  false;

	//todo deformation workonly in multiple
	auto def = c_def_node::get_cur();
	if( def->is_deforming() )
	{
		_vertex_data_out[0].alloc_point_deformed();
		REAL* deform = _vertex_data_out[0].get_point_deformed();
		def->apply( deform, _p_point_to_drawing, _unique_point_nb );
		//todo _b_to_make_normal = true;
		_p_point_to_drawing = deform;
		//todo_b_need_build = true;
	}
/*	else
	{
		if( _p_point != _p_point_to_deform )	//when we switch back to no deformation
			_b_need_build = true;
		_p_point = _p_point_to_deform;
	}
*/
	//no interpolation for now
	//there is a deformer to do this
/*	if( b_loc_inter )
	{
		if( _interpolation_value == 0. )
		{
			_p_normal_to_draw = c_bdd_tri::cur->_p_normal_to_draw; 
		}
		else if( _p_normal_to_draw == _normal_by_point)
		{
			interpolate_v3r_list(	_normal_by_point_interpolated,	c_bdd_tri::cur->_p_normal_to_draw, _normal_by_point, 
									_interpolation_value, _nb_point);
			_p_normal_to_draw = _normal_by_point_interpolated;
		}
		else
		{
			interpolate_v3r_list(	_normal_for_tri_interpolated,	c_bdd_tri::cur->_p_normal_to_draw, _tri_index_normal_out,
									_interpolation_value, _nb_tri*3);
			_p_normal_to_draw = _normal_for_tri_interpolated;
		}
	}
*/
	c_bdd_tri::cur = this;
	cluster_update();
	_b_random_color_by_id = _b_random_color_by_id_ui && c_render::get_cur()->get_random_on_color();
}
			
//todo not called anymore
void	c_bdd_tri::make_normal()
{
#if false
	//todo	optimize (we don't need both)
	if( _b_to_make_normal )
	{
		//INT32	i;
		REAL*	np;

		// force all normal 
		np = _normal_by_point - 1;
//todo make it //
		for( INT32 i = _nb_point; i > 0; --i )
		{
			*++np = 0.;
			*++np = 1.;
			*++np = 0.;
		}

		//INT32*	p = _tri_from_file_x3 - 1;
		UINT32*	p = _tri_index_point - 1;

		REAL	nf[3];

//todo make it //
		for( INT32 i = _nb_tri; i > 0; --i )
		{
			build_triangle_normal_std(	nf,
								_p_point_to_drawing + *++p   * 3 ,
								_p_point_to_drawing + *(p+2) * 3 ,
								_p_point_to_drawing + *(p+1) * 3
							);

			np = _normal_by_point + *p * 3 ;
			cpy_v3( np, nf );

			np = _normal_by_point + *++p * 3 ;
			cpy_v3( np, nf );

			np = _normal_by_point + *++p *3 ;
			cpy_v3( np, nf );
		}

		_b_to_make_normal = false;
	}
#endif
}

//build new index _list for triangle
void	c_bdd_tri::build_index_tri( INT32 nb )
{
	nb *= 3;
	if( nb > 65536 )
	{
		_b_index_short = false;
		UINT32* dst	= new UINT32[ nb ];
		_tri_index = dst;
		--dst;
		for( INT32 i = 0; i < nb; ++i )
			*++dst = i;
	}
	else
	{
		_b_index_short = true;
		UINT16* dst	= new UINT16[ nb ];
		_tri_index = dst;
		--dst;
		for( INT32 i = 0; i < nb; ++i )
			*++dst = i;
	}
}

void	c_bdd_tri::build_point_for_tri( REAL* dst, REAL* src )
{
//	if( !src || !dst || !_tri_from_file_x3 )
	if( !src || !dst || !_tri_index_point )
		return;
//	INT32*	p = _tri_from_file_x3 - 1;
	UINT32*	p = _tri_index_point - 1;
//todo make it //
	for( INT32 i = _nb_tri; i > 0; --i )
	{
		//fill new point array for new index
		cpy_v3( dst, src + *++p * 3);
		dst += 3;
		cpy_v3( dst, src + *++p * 3);
		dst += 3;
		cpy_v3( dst, src + *++p * 3);
		dst += 3;
	}
}

void	c_bdd_tri::build_normal_for_tri( REAL* dst, REAL* src )
{
	if( !src || !dst || !_tri_index_normal )
		return;
	INT32*	pin = (INT32 *)_tri_index_normal -1 ;
//todo make it //
	for( INT32 i = _nb_tri; i > 0; --i )
	{
		//fill new normal array for new index
		cpy_v3( dst, src + (*++pin) * 3 );
		dst += 3;
		cpy_v3( dst, src + (*++pin) * 3 );
		dst += 3;
		cpy_v3( dst, src + (*++pin) * 3 );
		dst += 3;
	}
}
void	c_bdd_tri::build_uv_for_tri( REAL* dst, REAL* src )
{
	if( !src || !dst || !_tri_index_uv )
		return;
	INT32*	pin = (INT32 *)_tri_index_uv - 1 ;
//todo make it //
	for( INT32 i = _nb_tri; i > 0; --i )
	{
		//fill new normal array for new index
		cpy_v2( dst, src + (*++pin) * 2 );
		dst += 2;
		cpy_v2( dst, src + (*++pin) * 2 );
		dst += 2;
		cpy_v2( dst, src + (*++pin) * 2 );
		dst += 2;
	}
}
#if AAA_POLY_COMPILE()
void	c_bdd_tri::build_point_for_poly()
{
	if( !_b_poly_in_file )
		return;

	UINT32*	p_poly_count = _poly_count - 1;
	UINT32*	p_dst = _poly_index_for_normal - 1;
	if( _b_index_short )
	{
		UINT16*	ppn = (UINT16*)_tri_index - 1;
		for( INT32 i = 0; i < _nb_poly; ++i )
		{
			INT32	tmp = *++p_poly_count;
			*++p_dst = *++ppn;
			*++p_dst = *++ppn;
			*++p_dst = *++ppn;
			tmp -= 3;
			for( INT32 j = 0; j < tmp; ++j )
			{
				ppn += 3;
				*++p_dst = *ppn;
			}
		}
	}
	else
	{
		UINT32*	ppn = (UINT32*)_tri_index - 1;
		for( INT32 i = 0; i < _nb_poly; ++i )
		{
			INT32	tmp = *++p_poly_count;
			*++p_dst = *++ppn;
			*++p_dst = *++ppn;
			*++p_dst = *++ppn;
			tmp -= 3;
			for( INT32 j = 0; j < tmp; ++j )
			{
				ppn += 3;
				*++p_dst = *ppn;
			}
		}
	}
}
#endif

/*
void	c_bdd_tri::build_point_for_normal( REAL *src)
{
INT32	i;
INT32	*p;
INT32	*pin;
INT32	retcode = AAA_OK;
REAL	*psrc;
REAL	*pdst;

	p = _p_tri_from_file_x3;
	pin = (INT32 *)_tri_for_normal;

	for( i = _nb_tri; i>0; --i )
		{
		INT32	j;
		for( j=0; j<3; ++j )
			{
			psrc = src+p[j];
			pdst = _point_for_normal+pin[j]*3;
			*pdst++ = *psrc++;
			*pdst++ = *psrc++;
			*pdst++ = *psrc++;
			}
		p+=3;
		pin+=3;
		}

	_b_to_make_normal = true;
}
*/



/*
void	c_bdd_tri::draw_pt()
{
INT32	i;
REAL	*pf;

	pf = point;
	GOL::begin(GL_POINTS);
	for( i = _nb_point; i>0; --i )
		{
		//GOL::texcoord2( *(one_point ), *(one_point+1));
		GOL::vertex3v( pf);
		pf+=3;
		}
	GOL::end();
}
*/


FINLINE bool	c_bdd_tri::is_selected_to_draw( c_obj_info* info )
{
	if( _b_draw_by_id_ui )
	{
		INT32	i = info->_obj_info_id - _obj_offset_id;
		return ( _obj_start <= i && i <= _obj_stop );
	}
	else if( _b_draw_by_name_ui )
	{
		if( strstr( info->_name.get(), _name_selector.get() ) != nullptr )
			return ( _obj_start <= info->_obj_index && info->_obj_index <= _obj_stop );
		return false;
	}
	else
		return ( _obj_start <= info->_obj_index && info->_obj_index <= _obj_stop );
}


void	c_bdd_tri::draw_before_low()
{
	if(	!_b_valid_data
		|| ( _b_vbo_use && _vbo_index && !GOL::b_draw_test_using_array_element )
		|| !GOL::b_draw_avoid_vertex_use
#if AAA_POLY_COMPILE()
		|| 	_b_poly_draw 	//	yes this is a special case : depreciated but useful some time
#endif
		)
		return;

	GOL::enable_client_state_vertex();
	GOL::set_pointer_vertex3( _p_point_to_drawing );

	if( _b_normal_do )
	{
		GOL::enable_client_state_normal();
		GOL::set_pointer_normal( _p_normal_to_draw );
	}
	else
	{
		GOL::disable_client_state_normal();
		if( _b_normal_force_ui )
			GOL::normal3v( unit_y_v4fp32 );
	}

	if( _b_draw_uv && _b_uv_in_file && _p_uv_to_draw )
	{
		GOL::enable_client_state_texcoor();
		GOL::set_pointer_texcoor( 2, GOL_REAL, _p_uv_to_draw );
	}
	else
		GOL::disable_client_state_texcoor();
}
//
//	VBO
//

void	c_bdd_tri::vbo_define_buffer()
{
	if( !_vbo_vertex || !_vbo_index )
		return;

	// bind VBO in order to use
	GLenum usage = _b_vbo_stream_ui ? GL_STREAM_DRAW : GL_STATIC_DRAW;
	UINT32	nb = _unique_point_nb;
	INT32	nb_byte = nb * sizeof(GLfloat);
	//	Point
	GOL::bind_set_buffer(		GL_ARRAY_BUFFER,	_vbo_vertex,		nb_byte * 3,	_p_point_to_drawing,	usage );
	//	Normal
	if( _p_normal_to_draw )
		GOL::bind_set_buffer(	GL_ARRAY_BUFFER,	_vbo_normal,		nb_byte * 3,	_p_normal_to_draw,		usage );	
	//	UV
	if( _p_uv_to_draw )
		GOL::bind_set_buffer(	GL_ARRAY_BUFFER,	_vbo_tex,			nb_byte * 2,	_p_uv_to_draw,			usage );	
	//	Attrib
	for( INT32 i=0; i<MORPH_NB_MAX; ++i )
	{
		//	Attrib Point
		if( REAL* pt = _vertex_data_out[i+1].get_point()  )
			GOL::bind_set_buffer( GL_ARRAY_BUFFER, _vbo_attr[i*2],	nb_byte * 3,	pt,						usage );	
		//	Attrib Normal
		if( REAL* pt = _vertex_data_out[i+1].get_normal()  )
			GOL::bind_set_buffer( GL_ARRAY_BUFFER, _vbo_attr[i*2+1], nb_byte * 3,	pt,						usage );	
	}
	GOL::unbind_buffer( GL_ARRAY_BUFFER );

	// upload index data to VBO
	GOL::bind_set_unbind_buffer( GL_ELEMENT_ARRAY_BUFFER, _vbo_index, _nb_tri * 3 * ( _b_index_short ? sizeof(GLshort) : sizeof(GLint) ), _tri_index, usage );

	_b_vbo_transfered = true;
}

static	C_PCHAR_C	attr_str[c_bdd_tri::MORPH_NB_MAX*2] = { "attrib[0]", "attrib[1]", "attrib[2]", "attrib[3]", "attrib[4]", "attrib[5]", "attrib[6]", "attrib[7]" };
#define	BUFFER_OFFSET(a)	( (CHAR*)nullptr + (a) )

void	c_bdd_tri::vbo_set_buffer_for_obj( c_obj_info* info )
{
	CHAR*	offset_3 = BUFFER_OFFSET(info->_point_start * 3);
	//	pt = _vertex_data._point + info->_point_start * 3;

	GOL::enable_client_state_vertex();			// activate vertex array
												//	needed to be done in Vao
	// bind VBOs for vertex array
	GOL::bind_buffer( GL_ARRAY_BUFFER, _vbo_vertex );
	GOL::set_pointer_vertex3( (REAL CONST *) offset_3 );

	if( _b_normal_do )
	{
		GOL::enable_client_state_normal();
		GOL::bind_buffer( GL_ARRAY_BUFFER, _vbo_normal );		// for normal
		GOL::set_pointer_normal( (REAL CONST *) offset_3 );
	}
	else
		GOL::disable_client_state_normal();

	if( _p_uv_to_draw && _b_draw_uv && _b_uv_in_file )
	{
		GOL::enable_client_state_texcoor();
		//aqua removed next line but maa is unsure
		//GOL::texcoord_enable_pointer( 2, GL_REAL, _p_uv_to_draw );
		GOL::bind_buffer( GL_ARRAY_BUFFER, _vbo_tex );		// for texture coordinates
		//aqua we should perhaps not do all the texture unit
		GOL::set_pointer_texcoor( 2,  GOL_REAL, BUFFER_OFFSET(info->_point_start * 2) );
	}
	else
		GOL::disable_client_state_texcoor();

	//todo	store the location and the buffer so we can update once the morph target
	//				and just set what's need here like when we do glDisableVertexAttribArray
	//				will need a unique id on compiled shader
	GLuint*	p_location = _attrib_location;
	c_shading* shad = c_shading::get_cur();
	if( shad )
	{
		INT32 i_att = 0;
#if 0
		for( INT32 i=1; i<=MORPH_NB_MAX; ++i )
		{
			c_vertex_data& vd = _vertex_data[i];
			if( vd.get_point() )
			{
				INT32 location = shad->get_attribute_location( attr_str[i_att] );
				if( location > -1 )
				{
					*p_location++ = location;
					GOL::enable_vertex_attrib_array( location );
					GOL::bind_buffer( GL_ARRAY_BUFFER, _vbo_attr[i_att] );	
					GOL:set_vertex_attrib_pointer( location, 3, GL_REAL, offset_3 );
				}
			}
			++i_att;
			if( vd.get_normal() )
			{
				INT32 location = shad->get_attribute_location( attr_str[i_att] );
				if ( location > -1 )
				{
					*p_location++ = location;
					GOL::enable_vertex_attrib_array( location );
					GOL::bind_buffer( GL_ARRAY_BUFFER, _vbo_attr[i_att] );
					GOL::set_vertex_attrib_pointer( location, 3, GL_REAL, offset_3 );
				}
			}
			++i_att;
		}
#else
		GLuint*	p_vbo = _attrib_vbo;
		for( INT32 i=1; i<=MORPH_NB_MAX; ++i )
		{
			c_vertex_data& vd = _vertex_data[i];
			if( vd.get_point() )
			{
				INT32 location = shad->get_render_attribute_location( attr_str[i_att] );
				if( location > -1 )
				{
					*p_location++	= location;
					*p_vbo++		= _vbo_attr[i_att];
				}
			}
			++i_att;
			if( vd.get_normal() )
			{
				INT32 location = shad->get_render_attribute_location( attr_str[i_att] );
				if ( location > -1 )
				{
					*p_location++	= location;
					*p_vbo++		= _vbo_attr[i_att];	
				}
			}
			++i_att;
		}
		_attr_nb = INT32( p_location - _attrib_location );
		for( INT32 i=0; i<_attr_nb; ++i )
		{
			INT32 location = _attrib_location[i];
			GOL::enable_vertex_attrib_array( location );
			GOL::bind_buffer( GL_ARRAY_BUFFER, _attrib_vbo[i] );	
			GOL::set_vertex_attrib_pointer( location, 3, GOL_REAL, offset_3 );
		}
#endif
	}
	GOL::unbind_buffer( GL_ARRAY_BUFFER );

	if( !GOL::is_vao() || GOL::b_vao_element_bind )
		GOL::bind_buffer( GL_ELEMENT_ARRAY_BUFFER, _vbo_index );
}

void	c_bdd_tri::vbo_draw( c_obj_info* info )
{
	bool b_vbo_object_adjust;
	if( GOL::is_vao())
	{
		GOL::bind_vao( _vao );
		if( !_vbo_in_vao  )
		{
			_b_vbo_transfered = false;
			_vbo_in_vao = true;
		}
		//todo separate attrib update
		auto	unique_id = c_shading::get_state_unique_cur();
		if( unique_id && _shading_unique_id != unique_id )
		{
			//c_shading* shad = c_shading::get_cur();
			//DBG_PRINT_STRING( "VAO %d / program %d", _vao, shad->get_program_gl() );
			_shading_unique_id = unique_id;
			b_vbo_object_adjust = true;
		}
		else
		{
			b_vbo_object_adjust = false;
		}
	}
	else
	{
		if( _vbo_in_vao )
		{
			_b_vbo_transfered = false;
			_vbo_in_vao = false;
		}
		b_vbo_object_adjust = true;
	}

	//_b_vbo_transfered = false;
	if( !_b_vbo_transfered )
	{
		vbo_define_buffer();
		b_vbo_object_adjust = true;
	}
	if( b_vbo_object_adjust )
		vbo_set_buffer_for_obj( info );

	if( GOL::is_vao() && !GOL::b_vao_element_bind )
		GOL::bind_buffer( GL_ELEMENT_ARRAY_BUFFER, _vbo_index );

//	GOL::check_error_debug( "before glDrawElements" );
	//	glDrawRangeElements( GL_TRIANGLES, info->_tri_start * 3, 0, nb * 3, GL_UNSIGNED_INT, 0 );
	//	glDrawRangeElements()
	
	GOL::draw_elements( GL_TRIANGLES, info->_tri_nb * 3, _b_index_short ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, BUFFER_OFFSET(info->_tri_start * 3) );

	GOL::check_error_debug( "after glDrawElements" );

	if( GOL::is_vao())
	{
		//GOL::unbind_buffer( GL_ELEMENT_ARRAY_BUFFER );
		GOL::unbind_vao();
	}
	else
	{
		// bind with 0, so, switch back to normal pointer operation
		GOL::unbind_buffer( GL_ARRAY_BUFFER );
		//	glDisableClientState( GL_INDEX_ARRAY );
//		GOL::disable_client_state_vertex();		// deactivate vertex array
		GOL::disable_client_state_normal();
		for( INT32 i=0; i<_attr_nb; ++i )
			GOL::disable_vertex_attrib_array( _attrib_location[i] );
		GOL::unbind_buffer( GL_ELEMENT_ARRAY_BUFFER );
	}
}

template< class T >
void	c_bdd_tri::draw_old_school( c_obj_info* info )
{
	//hack since draw_polys
	INT32 nb = c_model::cur->compute_from_resolution( info->_tri_nb );		
	_prim_drawn += nb;

	T*	p = (T*)_p_tri_to_draw + info->_tri_start * 3 - 1;
	REAL* puv = _vertex_data_out[0].get_uv();	// + info->_tri_start * 6;
	INT32	i;

	if( _b_normal_do )
	{
		REAL	*n;
		if( c_render::get_cur()->is_gouraud() )
		{
			i = nb * 3;
			GOL::begin( GL_TRIANGLES );
			if( _b_draw_uv && _b_uv_in_file )
			{
				for( ; i > 0; --i )
				{
					INT32 index = *++p * 3;
					GOL::normal3v( _p_normal_to_draw + index  );
					GOL::texcoord2v( puv + *p * 2 );
					GOL::vertex3v( _p_point_to_drawing + index );
				}
			}
			else
			{
				for( ; i > 0; --i )
				{
					INT32 index = *++p * 3;
					GOL::normal3v( _p_normal_to_draw + index );
					GOL::vertex3v( _p_point_to_drawing + index );
				}
			}
			GOL::end();
		}
		else
		{
			n = _p_normal_to_draw;
			GOL::begin( GL_TRIANGLES );
			if( _b_draw_uv && _b_uv_in_file )
			{
				for( i = nb; i > 0; --i )
				{
					GOL::normal3v( n );

					INT32 index = *++p;
					GOL::texcoord2v( puv				+ index * 2 );
					GOL::vertex3v( _p_point_to_drawing	+ index * 3 );

					index = *++p;
					GOL::texcoord2v(  puv				+ index * 2 );
					GOL::vertex3v( _p_point_to_drawing	+ index * 3 );

					index = *++p;
					GOL::texcoord2v(  puv				+ index * 2 );
					GOL::vertex3v( _p_point_to_drawing	+ index * 3 );

					n += 3;
				}
			}
			else
			{
				for( i = nb; i > 0; --i )
				{
					GOL::normal3v( n );

					GOL::vertex3v( _p_point_to_drawing + *++p * 3 );
					GOL::vertex3v( _p_point_to_drawing + *++p * 3 );
					GOL::vertex3v( _p_point_to_drawing + *++p * 3 );

					n += 3;
				}
			}
			GOL::end();
		}
	}
	else
	{
		if( _b_normal_force_ui )
			GOL::normal3v( unit_y_v4fp32 );
		if( _b_draw_uv && _b_uv_in_file )
		{
			GOL::begin( GL_TRIANGLES );
			for( i = nb; i > 0; --i )
			{
				T index = *++p;
//					GOL::texcoord2v( _vertex_data._uv + *puv++*2 );
				GOL::texcoord2v( puv + index * 2 );
				GOL::vertex3v( _p_point_to_drawing + index * 3 );

				index = *++p;
//					GOL::texcoord2v( _vertex_data._uv + *puv++*2 );
				GOL::texcoord2v( puv + index * 2 );
				GOL::vertex3v( _p_point_to_drawing + index * 3 );

				index = *++p;
//					GOL::texcoord2v( _vertex_data._uv + *puv++*2 );
				GOL::texcoord2v( puv + index * 2 );
				GOL::vertex3v( _p_point_to_drawing + index * 3 );
			}
			GOL::end();
		}
		else
		{
			GOL::begin( GL_TRIANGLES );
			for( i = nb; i > 0; --i )
			{
				GOL::vertex3v( _p_point_to_drawing + *++p * 3);
				GOL::vertex3v( _p_point_to_drawing + *++p * 3 );
				GOL::vertex3v( _p_point_to_drawing + *++p * 3 );
			}
			GOL::end();
		}
	}
}

void	c_bdd_tri::obj_draw_face( c_obj_info* info )
{
	INT32	color_mat;

	if( _clip_dist != 0. )
	{
		REAL	dist;
		//	signed distance to outside
		dist = dist_v3r( _cam_pos_pt, info->_bbox.get_origin() ) - _clip_dist - info->_bbox.get_radius();
		if( dist >= 0. )
			return;
		if( _clip_dist_alpha == 0. )
			dist = 1;
		else if( _clip_dist_alpha <= 1. )
		{
			dist = -dist / ( _clip_dist * _clip_dist_alpha );
			if( dist > 1. )
				dist = 1.;
		}
		//todoqq	deal with no light
		c_materials::get_cur()->front_cur_set_alpha( dist );
	}

	if( info->_mtl_index >=0 && _s_tex_change == 1 )
		tex_2d_bind( info->_mtl_index + _mtl_offset );

	if( _b_random_color_by_id )
		c_color_random::set( info->_obj_index );

	if(	_b_tex_object_rot_ui && c_map::get_cur()->is_tex_proj_object() && !c_map::get_cur()->is_implicit() )
	{
		GLfloat tex_param_s[] = { 0., 0., 0., 0. };
		GLfloat tex_param_t[] = { 0., 0., 0., 0. };
		INT32	axe_u;
		INT32	axe_v;
		INT32	tex_axe = c_map::get_cur()->get_tex_axe();
		axe_build_index_vert( axe_u, axe_v, tex_axe );

		REAL	tex_u = c_map::get_cur()->get_u();
		REAL	tex_v = c_map::get_cur()->get_v();
		if( _b_tex_object_stick_bbox_ui )
			tex_u *= REAL(.5) / info->_bbox.get_radius();

		REAL	f[3];
		REAL	a,b,c;
		REAL	norm;

		info->_bbox.get_origin( f );
		auto fc = c_seedcam::get_cur()->get_position_pt();
//		fc[axe_u] = -fc[axe_u];
//		fc[tex_axe] = fc[tex_axe];

		a = fc[ axe_u ]-f[ axe_u ];
		b = f[ tex_axe ]-fc[ tex_axe ];
		c = -f[ tex_axe ] * a - f[ axe_u ] * b;
		norm = SQRT( a * a + b * b );
		norm = tex_u / norm;
		a *= norm;
		b *= norm;
		c *= norm;

		if( tex_axe == 0 )
		{
			tex_param_s[ tex_axe ] = a;
			tex_param_s[ axe_u ] = b;
		}
		else
		{
			tex_param_s[ tex_axe ] = a;
			tex_param_s[ axe_u ] = -b;
		}
		tex_param_s[ 3 ] = c + c_map::get_cur()->get_u_min() - REAL(.5);

		GOL::texgen_i( GL_S, GL_TEXTURE_GEN_MODE,  GL_OBJECT_LINEAR );
		GOL::texgen_fv( GL_S, GL_OBJECT_PLANE, tex_param_s );

		norm = c_map::get_cur()->get_v() * REAL(.5) / info->_bbox.get_size_half( 1 );
		tex_param_t[ axe_v ] = norm;
		tex_param_t[ 3 ] = c_map::get_cur()->get_v_min() -f[1] * norm - REAL(.5);

		GOL::texgen_i( GL_T, GL_TEXTURE_GEN_MODE,  GL_OBJECT_LINEAR );
		GOL::texgen_fv( GL_T, GL_OBJECT_PLANE, tex_param_t );
	}

	if( _b_random_color_by_id ) 
		color_mat = 0;
	else if( color_mat = c_render::get_cur()->get_random_on_color() )
		c_color_random::begin();

//DRAW MULTIPLE
	if( c_multiple::cur->is_render_multiple() )
	{
		INT32	stop = info->_point_start + info->_point_nb;
		INT32	i = info->_point_start;
		REAL*	p = _p_point_to_drawing + info->_point_start * 3;
		INT32	inc = ( 1 + _skip );
		INT32	inc_r = 3 * inc;
		c_multiple::cur->begin();

		if( c_multiple::cur->is_align_normal() && _b_normal_in_file )
		{
			REAL* n = _vertex_data[0].get_normal() + info->_point_start * 3;
			for( ; i < stop; i += inc )
			{
				c_multiple::cur->set_index( i );
				c_multiple::cur->draw_one_at_nor( p, n );
				p += inc_r;
				n += inc_r;
			}
		}
		else
		{
			for( ; i < stop; i += inc )
			{
				c_multiple::cur->set_index( i );
				c_multiple::cur->align_then_draw( p );
				p += inc_r;
			}
		}
		c_multiple::cur->end();
	}
	else if( GOL::b_draw_avoid_vertex_use )
//	if( !need_normal() )	//bug second condition to avoid crash on Jelie Machine
	{
#if AAA_POLY_COMPILE()
//DRAW POLYGON
		if( _b_poly_draw )
		{
			UINT32*	p_poly_count	=	_poly_count				+ info->_poly_start			- 1;
			UINT32*	p_poly_index	=	_p_poly_index_to_draw	+ info->_poly_index_start	- 1;

			INT32 poly_nb = c_model::cur->compute_from_resolution( info->_poly_nb );
			_prim_drawn += poly_nb;

//DRAW POLYGON TEST VERSION
			GOL::enable_client_state_vertex();
			GOL::set_pointer_vertex3( _p_point_to_drawing );

			//if( is_normal_draw() )
			{
				//if( _b_force_normal )
				{
					GOL::normal3v( unit_y_v4fp32 );
					GOL::disable_client_state_normal();
				}
				//else
				{
				//	GOL::enable_client_state_normal();
				//	GOL::set_pointer_normal( _p_normal_to_draw );
				}
			}
			//else
			{
			//	GOL::disable_client_state_normal();
			}

			//if( _b_draw_uv && _b_uv_in_file )
			{
				//	GOL::enable_client_state_texcoor();
				//	GOL::set_pointer_texcoor( 2, GOL_REAL, _p_uv_to_draw );
			}
			//else
			{
				GOL::texcoord_disable_pointer();
			}
		
			if( GOL::b_draw_test_using_array_element )
			{
				if( _b_draw_uv && !_b_uv_in_file )
				{
					for( INT32 i = 0; i < poly_nb; ++i )
					{
						if( color_mat )
							c_color_random::set();
						GOL::begin( GL_POLYGON );

							INT32 vertex_nb = *++p_poly_count;
							if( vertex_nb == 4 )
							{
								GOL::texcoord2v( base_uv_4[0] );
								GOL::draw_array_element( *++p_poly_index );

								GOL::texcoord2v( base_uv_4[1] );
								GOL::draw_array_element( *++p_poly_index );

								GOL::texcoord2v( base_uv_4[2] );
								GOL::draw_array_element( *++p_poly_index );

								GOL::texcoord2v( base_uv_4[3] );
								GOL::draw_array_element( *++p_poly_index );
							}
							else if( vertex_nb == 3 )
							{
								GOL::texcoord2v( base_uv_3[0] );
								GOL::draw_array_element( *++p_poly_index );

								GOL::texcoord2v( base_uv_3[1] );
								GOL::draw_array_element( *++p_poly_index );

								GOL::texcoord2v( base_uv_3[2] );
								GOL::draw_array_element( *++p_poly_index );
							}
							else
							{
								for( ; vertex_nb > 0; --vertex_nb )
									GOL::draw_array_element( *++p_poly_index );
							}

						GOL::end();
					}//end	for
				}//end	if( _b_draw_uv && !_b_uv_in_file)
				else
				{
					//INT32	j;
					for( INT32 i = 0; i < poly_nb; ++i )
					{
						GOL::begin( GL_POLYGON );
							if( color_mat )
								c_color_random::set();
							INT32 vertex_nb = *++p_poly_count;
							for( ; vertex_nb > 0; --vertex_nb )
								GOL::draw_array_element( *++p_poly_index );
						GOL::end();
					}
				}
			}//end	if( b_draw_test_using_array_element )
			else
			{
//DRAW POLYGON NORMAL
				++p_poly_index;
				for( INT32 i = 0; i < poly_nb; ++i )
				{
					if( color_mat)
						c_color_random::set();
					INT32 vertex_nb = *++p_poly_count;
//					glDrawRangeElements( GL_POLYGON, 0, (int)*p_poly_count, (int)*p_poly_count, GL_UNSIGNED_INT, (void *)p_poly_index );
					GOL::draw_elements( GL_POLYGON, vertex_nb, p_poly_index );
					p_poly_index += vertex_nb;
				}
			}
		}//end	if( _b_poly )
		else
#endif
		{
//DRAW TRIANGLE
			INT32 tri_nb = c_model::cur->compute_from_resolution( info->_tri_nb );
			_prim_drawn += tri_nb;

//DRAW TRIANGLE TEST VERSION
			if( GOL::b_draw_test_using_array_element )
			{
				INT32	inc = 3 + 3 * _skip;
				GOL::begin( GL_TRIANGLES );
				if( _b_index_short )
				{
					UINT16*	p = (UINT16*)_p_tri_to_draw + info->_tri_start * 3;
					for( INT32 i = 0; i < tri_nb * 3; i += inc )
					{
						if( color_mat )
							c_color_random::set();
						GOL::draw_array_element( *(p+i) );
						GOL::draw_array_element( *(p+i+1) );
						GOL::draw_array_element( *(p+i+2) );
					}
				}
				else
				{
					UINT32*	p = (UINT32*)_p_tri_to_draw + info->_tri_start * 3;
					for( INT32 i = 0; i < tri_nb * 3; i += inc )
					{
						if( color_mat )
							c_color_random::set();
						GOL::draw_array_element( *(p+i) );
						GOL::draw_array_element( *(p+i+1) );
						GOL::draw_array_element( *(p+i+2) );
					}
				}
				GOL::end();
			}	//end b_draw_test_using_array_element
			else
			{
//DRAW TRIANGLE VBO
				if( _b_vbo_use && _vbo_index )
				{
					vbo_draw( info );
				}
				else
//DRAW TRIANGLE MODERN
				{
					if( _p_tri_to_draw )
					{
						if( _b_index_short )
						{
							UINT16*	p = (UINT16*)_p_tri_to_draw + info->_tri_start * 3;
							GOL::draw_elements( GL_TRIANGLES, tri_nb * 3, p );
						}
						else
						{
							UINT32*	p = (UINT32*)_p_tri_to_draw + info->_tri_start * 3;
							GOL::draw_elements( GL_TRIANGLES, tri_nb * 3, p );
						}
						//glDrawRangeElements( GL_TRIANGLES, 0, nb*3, nb * 3, GL_UNSIGNED_INT, p );
					}
					else
						err_print( "_p_tri_to_draw is null, can't draw_elements" );
				}
			}
		}
//			GOL::disable_client_state_normal();
	}//end	if( b_draw_avoid_vertex_use)
	else
	{
//DRAW OLD SCHOOL
		if( _b_index_short )
		{
			draw_old_school<UINT16>( info );
			//err_print( "Old school draw with short index Unimplemented" );
			//goto exit;
		}
		else
			draw_old_school<UINT32>( info );
	}

	if( color_mat )
		c_color_random::end();
	if( _b_draw_contour )
	{
		//hack to draw contours 
		c_shading::get_cur()->unbind_render();

		// draw contour
		GOL::push_att();	//	( GL_LINE );
			GOL::push_color4v( _color_contour );
			GOL::push_line_width ( c_render::get_cur()->get_line_size() );
			if( c_render::get_cur()->is_line_smooth() )
				GOL::push_line_smooth( true );

			for( size_t i = 0; i < info->_contours.size(); ++i )
			{
				GOL::begin( GL_LINE_LOOP );
					for( size_t j = 0; j < info->_contours[i].size(); ++j )
					{
						Vertex CONST & vec = info->_contours[i][j];
						GOL::vertex3( REAL(vec.x), REAL(vec.y), REAL(vec.z) );
					}
				GOL::end();
			}
		GOL::pop_att();
	}
}

FINLINE	void	c_bdd_tri::obj_draw_bbox( c_obj_info* info )
{
	info->_bbox.draw();
}

void	c_bdd_tri::draw_normal_point( REAL len )
{
	if( !_b_valid_data )
		return;
	
	INT32	i;
	INT32	index;
	REAL*	n;
	REAL*	pp;
//hack
//should be done on the polys
	if( _b_index_short )
	{
//todo make it in one call
		UINT16* p = (UINT16*)_p_tri_to_draw - 1;
		GOL::begin( GL_LINES );
		for( i = _nb_tri; i > 0; --i )
		{
			index = *++p * 3;
			n = _p_normal_to_draw + index;
			pp = _p_point_to_drawing + index;
			GOL::vertex3v( pp );
			GOL::vertex3( pp[0] + n[0] * len,	pp[1] + n[1] * len,		pp[2] + n[2] * len	);

			index = *++p * 3;
			n = _p_normal_to_draw + index;
			pp = _p_point_to_drawing + index;
			GOL::vertex3v( pp );
			GOL::vertex3( pp[0] + n[0] * len,	pp[1] + n[1] * len,		pp[2] + n[2] * len	);

			index = *++p * 3;
			n = _p_normal_to_draw + index;
			pp = _p_point_to_drawing + index;
			GOL::vertex3v( pp );
			GOL::vertex3( pp[0] + n[0] * len,	pp[1] + n[1] * len,		pp[2] + n[2] * len	);
		}
		GOL::end();
	}
	else
	{
//todo make it in one call
		UINT32* p = (UINT32*)_p_tri_to_draw - 1;
		GOL::begin( GL_LINES );
		for( i = _nb_tri; i > 0; --i )
		{
			index = *++p * 3;
			n = _p_normal_to_draw + index;
			pp = _p_point_to_drawing + index;
			GOL::vertex3v( pp );
			GOL::vertex3( pp[0] + n[0] * len,	pp[1] + n[1] * len,		pp[2] + n[2] * len	);

			index = *++p * 3;
			n = _p_normal_to_draw + index;
			pp = _p_point_to_drawing + index;
			GOL::vertex3v( pp );
			GOL::vertex3( pp[0] + n[0] * len,	pp[1] + n[1] * len,		pp[2] + n[2] * len	);

			index = *++p * 3;
			n = _p_normal_to_draw + index;
			pp = _p_point_to_drawing + index;
			GOL::vertex3v( pp );
			GOL::vertex3( pp[0] + n[0] * len,	pp[1] + n[1] * len,		pp[2] + n[2] * len	);
		}
		GOL::end();
	}
}

FINLINE	void	c_bdd_tri::draw_low( P_FN fn, bool b_skip_clip )
{
	if( b_skip_clip || c_seedcam::get_cur()->bbox_is_in_world( &(_obj_info->_bbox) ) )
	{
		c_obj_info*	 info = _obj_info;
		if( _obj_nb == 1 )
			(this->*fn)( info );
		else if( _s_clipping_type )
		{
			std::vector<c_obj_info*>*				pt;
			std::vector<c_obj_info*>::iterator		it;
			std::vector<c_obj_cluster*>::iterator	itc;
			std::vector<c_obj_cluster*>::iterator	begin;
			std::vector<c_obj_cluster*>::iterator	end;

			bool b_cluster_skip_deep = b_skip_clip || (_s_clipping_type < 2);

			begin = _cluster_list.begin();
			if( _cluster_debug )
			{
				end = _cluster_list.end();
			}
			else
			{ 
				INT32	i = IMOD(_cluster_debug,(INT32)_cluster_list.size());
				//testtest
				advance( begin, i);
				end = begin;
				++end;
			}
			
			c_seedcam*	cam = c_seedcam::get_cur();
			for( itc = begin; itc != end; ++itc )
			{
				if( b_skip_clip || cam->bbox_is_in_world( &(*itc)->_bbox ) )
				{
					pt = &(*itc)->_list;
					for( it=pt->begin(); it!=pt->end(); ++it )
					{
						info = *it;
						if( b_cluster_skip_deep || cam->bbox_is_in_world( &(info->_bbox) ) )
						{
							if( is_selected_to_draw( info ) )
							{
								if( _s_tex_change == 2 && c_tex_anim::cur->is_tex_several() )
									c_tex_anim::cur->bind_this( info->_obj_index );
								(this->*fn)( info);
							}
						}
					}
				}
			}
		}
		else // of  if(_s_clipping_type)
		{
			if( _b_draw_by_obj_ui || c_tex_anim::cur->is_tex_several() )
			{
//				std::vector<c_obj_info*>*			obj_list;
				std::vector<c_obj_info*>::iterator	begin;
				std::vector<c_obj_info*>::iterator	end;
				std::vector<c_obj_info*>::iterator	it;
		//		c_obj_info*					info;
				INT32							i;

				if( _b_draw_by_id_ui )
				{
					i = CLAMP( _obj_start + _obj_offset_id, 0, _obj_nb-1 );
					//testtest
					begin = _obj_by_id.begin();
					advance( begin, i );

					i = CLAMP( _obj_stop + _obj_offset_id, 0, _obj_nb-1 );
					//testtest
					end = _obj_by_id.begin();
					advance( end, i );
				}
				else
				{
					i = CLAMP( _obj_start, 0, _obj_nb-1 );
					//testtest
					begin = _obj_by_index.begin();
					advance( begin, i );

					i = CLAMP( _obj_stop, 0, _obj_nb-1 );
					//testtest
					end = _obj_by_index.begin();
					advance( end, i );
				}
				
				if( _s_tex_change == 2 && c_tex_anim::cur->is_tex_several() )
				{
					c_seedcam*	cam = c_seedcam::get_cur();
					for( it=begin; it <= end; ++it )
					{
						info = *it;
						if( b_skip_clip || cam->bbox_is_in_world( &(info->_bbox) ) )
						{
							c_tex_anim::cur->bind_this( info->_obj_index );
							(this->*fn)( info);
						}
					}
				}
				else
				{
					c_seedcam*	cam = c_seedcam::get_cur();
					for( it=begin; it <= end; ++it )
					{
						info = *it;
						if( b_skip_clip || cam->bbox_is_in_world( &( info->_bbox ) ) )
						{
							(this->*fn)( info );
						}
					}
				}
			}
			else
				(this->*fn)( info );
		}
	}
}

void	c_bdd_tri::draw()
{
	_prim_drawn = 0;
	if( !_b_valid_data ) return;

	if( _clip_dist != 0. )	//	the fade is always on if _clip_dist non zero
		_cam_pos_pt = c_seedcam::get_cur()->get_position_pt();

	if( _b_random_color_by_id )
		c_color_random::begin();
	
//	if( !_b_normal_force_ui )
//		DBG_PRINT_STRING( "added for breakpoint : remove " );

	draw_low( &c_bdd_tri::obj_draw_face, _b_skip_clipping );

	if( _b_random_color_by_id )
		c_color_random::end();
}

void	c_bdd_tri::draw_bbox()
{
	if( !_b_valid_data )	return;
			 
	draw_low( &c_bdd_tri::obj_draw_bbox, _b_skip_clipping );
}

void	c_bdd_tri::cluster_init()
{
	_cluster = nullptr;
	_cluster_nb[0] = 0;
	_cluster_nb[1] = 0;
	_cluster_nb[2] = 0;
	_cluster_nb_total = 0;
}

void	c_bdd_tri::cluster_alloc()
{
	cluster_dealloc();
	INT32 nb	=  _cluster_nb_ui[0] * _cluster_nb_ui[1] * _cluster_nb_ui[2];
	_cluster	=	new c_obj_cluster[ nb ];
	if( _cluster )
	{
		_cluster_nb[0] = _cluster_nb_ui[0];
		_cluster_nb[1] = _cluster_nb_ui[1];
		_cluster_nb[2] = _cluster_nb_ui[2];
		_cluster_nb_total = nb;
	}
}

void	c_bdd_tri::cluster_dealloc()
{
	if( _cluster )
	{
		SAFE_DELETE_ARRAY( _cluster );
		cluster_init();
	}
}

void	c_bdd_tri::cluster_update()
{
	if	(	_s_clipping_type && 
			(	_cluster_nb[0] != _cluster_nb_ui[0]
			||	_cluster_nb[1] != _cluster_nb_ui[1]
			||	_cluster_nb[2] != _cluster_nb_ui[2]
			)
		)
	{
		cluster_alloc();
		cluster_build();
	}

}

void	c_bdd_tri::cluster_build()
{
	if( _s_clipping_type )
	{
		c_obj_info*	info = _obj_info;
		REAL			min[3];
		REAL			max[3];
		info->_bbox.get_min( min );
		info->_bbox.get_max( max );
		for( INT32 i = 0; i < _obj_nb; ++i )
		{
			++info;
			REAL CONST * CONST cen = info->_bbox.get_origin();
			INT32 u = CLAMP( INT32( _cluster_nb[0] * (cen[0]-min[0]) / (max[0]-min[0]) ), 0, _cluster_nb[0]-1 );
			INT32 v = CLAMP( INT32( _cluster_nb[2] * (cen[2]-min[2]) / (max[2]-min[2]) ), 0, _cluster_nb[2]-1 );
			_cluster[u+v*_cluster_nb[0]].add_obj_info_pt( info );
		}
		_cluster_list.clear();
		_cluster_list.resize( _cluster_nb_total );
		c_obj_cluster* pcluster = _cluster;
		for( INT32 i = _cluster_nb_total; i > 0; --i )
		{
			if( !pcluster->_list.empty() )
			{
				//_cluster_list.push_back( pcluster );
				_cluster_list[i] = pcluster;
				pcluster->compute_bbox();
				pcluster->sort_by_mtl();
			}
			++pcluster;
		}
		_cluster_nb_out = (INT32)_cluster_list.size();
	}
}

c_obj_cluster::~c_obj_cluster()
{
	_list.clear();
}

c_obj_cluster::c_obj_cluster()
{
	_list.clear();
}

void	c_obj_cluster::compute_bbox()
{
	if( !_list.empty() )
	{
		c_bbox*	obj_bbox;
		REAL	min[3];
		REAL	max[3];
		REAL	loc_min[3];
		REAL	loc_max[3];

		set_v3( min, REAL_BIG_VALUE);
		set_v3( max, -REAL_BIG_VALUE);

		for( auto const & p_obj_info : _list )
		{
			obj_bbox = &(p_obj_info->_bbox);
			obj_bbox->get_min( loc_min );
			obj_bbox->get_max( loc_max );
			//todo is this opt ?
			min_max_v3r( min, max, loc_min );
			min_max_v3r( min, max, loc_max );
		}
		_bbox.set_min_max( min, max );
	}
}


AAA_ERR	c_bdd_tri::save_do_after( o_str CONST & filename )
{
	save_obj_info();
	return AAA_OK;
}

//INT32	file_csv::write_float( C_PCHAR_C filename, REAL *data, INT32 x, INT32 y)
//INT32	file_csv::read_float( C_PCHAR_C filename, REAL *data, INT32 x, INT32 y);

/*
struct	c_obj_info_less
{
	bool	operator()( CONST c_obj_info* a, CONST c_obj_info* b) 
		CONST
		{
		return (a->_mtl_index < b->_mtl_index);
		}
};
*/

/*
FINLINE	bool	c_obj_info_less( CONST c_obj_info* a, CONST c_obj_info* b)
{
	return (a->_mtl_index < b->_mtl_index);
}
*/

class	c_obj_info_less_by_mtl
{	public:
	bool operator() ( c_obj_info* a, c_obj_info* b )	{	return ( a->_mtl_index < b->_mtl_index );		}
};

class	c_obj_info_less_by_index
{	public:
	bool operator() ( c_obj_info* a, c_obj_info* b )	{	return ( a->_obj_index < b->_obj_index );		}
};

class	c_obj_info_less_by_id
{	public:
	bool operator() ( c_obj_info* a, c_obj_info* b )	{	return ( a->_obj_info_id < b->_obj_info_id );	}
};

void	c_obj_cluster::sort_by_mtl()
{
	if( !_list.empty() )	//todostl is this necessary or already in sort
	{
		std::sort( _list.begin(), _list.end(), c_obj_info_less_by_mtl() );
	}
}

//todo this is bad
void	c_bdd_tri::obj_do_sorts()
{
	SPY_PUSH_RANGE( "obj_do_sorts", spy::COL_1 );
		c_obj_info*	info;
	//INT32					i;
		_obj_by_index.clear();
		_obj_by_id.clear();
		_obj_by_index.resize( _obj_nb );
		_obj_by_id.resize( _obj_nb );
		info = &_obj_info[1];
		for( INT32 i = _obj_nb-1; i >= 0; --i )
		{
			//_obj_by_index.push_back( info );
			//_obj_by_id.push_back( info );
			_obj_by_index[i]	= info;
			_obj_by_id[i]		= info;
			++info;
		}
		std::sort( _obj_by_index.begin(),	_obj_by_index.end(),	c_obj_info_less_by_index()	);
		std::sort( _obj_by_id.begin(),		_obj_by_id.end(),		c_obj_info_less_by_id()		);
	SPY_POP_RANGE();
}
