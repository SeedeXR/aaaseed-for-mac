
#include "bdd_curve3d.h"
#include "obj_ui/deformer/def_node.h"
#include "draw/model.h"
#include "gol/gol.h"
#include "draw/aaa_glut.h"
#include "infrastructure/obj/command.h"
#include "draw/picking.h"
#include "shaders/shading.h"
#include "infrastructure/layer/layer.h"
#include "draw/seedcam.h"
#include "ui/seed_ui.h"
#include "draw/axe.h"
#include "draw/color.h"
#include "file/file_io.h"
#include "asc_line.h"
#include "ui/alphabet.h"
#include "math/billboard.h"
#include "infrastructure/layer/layers.h"
#include "draw/guf.h"
#include "ui/keyboard.h"
#include "math/rand.h"
#include "infrastructure/param/param_focus.h"


FACTORY_CREATE_PROP_V1( c_bdd_curve_3d, bdd_curve_3d, Curve 3d, bdd_curve_3d, sub_menu="UI"; );

namespace{
	CONSTEXPR C_PCHAR_C g_curve3d_str[ c_curve_3d::CURVE_TYPE_MAX_NB ] =
	{
		"Linear",
		"Catmull Rom",
		//"Cubic B-Spline",
		//"Nurbs",
		//"Bezier",
	};
}

void	c_bdd_curve_3d::c_curve_info::init()
{
//	_nb_markers	= 0;
	_s_type		= c_curve_3d::CURVE_LINEAR;
	_lod		= c_curve_3d::DEFAULT_LOD;
	_tightness	= c_curve_3d::DEFAULT_TIGHTNESS;
	_b_loop		= false;
}

void	c_bdd_curve_3d::c_curve_info::clear()
{
	init();
}

c_bdd_curve_3d::c_curve_info::c_curve_info()
{
	init();
}

void	c_bdd_curve_3d::c_curve_info::save( C_PCHAR_C name_info )
{
	FILE*	f = c_file::FOPEN( name_info, "wt" );
	if( !f )
	{
		ERR_PRINT_STRING( "%s : error opening file %s", __FUNCTION__, name_info );
		return;
	}

		fprintf( f, "curve_info ( Version %d )\n{\n",	0							);
		fprintf( f, "\tcurve_type = \"%s\" ;\n",		g_curve3d_str[ _s_type ]	);
		fprintf( f, "\tcurve_loop = %s ;\n",			_b_loop ? "ON" : "OFF"		);
		fprintf( f, "\tcurve_lod = %d ;\n",				_lod						);
		fprintf( f, "\tcurve_tightness = %f ;\n}\n",	_tightness					);

	c_file::FCLOSE( f );
}

void	c_bdd_curve_3d::c_curve_info::load( C_PCHAR_C name_info )
{
	FILE*	f = c_file::FOPEN( name_info, "rt" );
	if( !f )
	{
		ERR_PRINT_STRING( "%s : error opening file %s", __FUNCTION__, name_info );
		return;
	}

		INT32 CONST	buf_len = 256;
		CHAR	buffer[ buf_len ];
		asc_line::get_next_line_no_empty( f, buffer, buf_len );
		asc_line::get_next_line_no_empty( f, buffer, buf_len );
		CHAR	param[ 256 ];
		if( asc_line::get_next_line_no_empty( f, buffer, buf_len ) <= 0 )
			goto error;
		if( sscanf( buffer, " curve_type = \"%[^\"]\" ;", param ) <= 0 )		//	read till "
			goto error;
		_s_type = c_curve_3d::CURVE_CATMULLROM;
		if( strncmp( param, "Linear", strlen( "Linear" ) ) == 0 )
			_s_type = c_curve_3d::CURVE_LINEAR;

		if( asc_line::get_next_line_no_empty( f, buffer, buf_len ) <= 0 )
			goto error;
		if( sscanf( buffer, " curve_loop = %s ;", param ) <= 0 )
			goto error;
		if( strcmp( param, "ON" ) == 0 )
			_b_loop = true;

		if( asc_line::get_next_line_no_empty( f, buffer, buf_len ) <= 0 )
			goto error;
		INT32	lod;
		if( sscanf( buffer, " curve_lod = %d ;", &lod ) <= 0 )
			goto error;
		_lod = lod;

		if( asc_line::get_next_line_no_empty( f, buffer, buf_len ) <= 0 )
			goto error;
		REAL	val_real;
		if( sscanf( buffer, " curve_tightness = %f ;", &val_real ) <= 0 )
			goto error;
		_tightness = val_real;

	c_file::FCLOSE( f );
	return;

error:
	debug_break( "bad format in file %s", name_info );
	c_file::FCLOSE( f );
	return;
	
}

enum	MOVE_CONSTRAINT : INT32
{
	MOVE_CONSTRAINT_NONE = 0,
	MOVE_CONSTRAINT_XY,
	MOVE_CONSTRAINT_XZ,
	MOVE_CONSTRAINT_YZ,
	MOVE_CONSTRAINT_X,
	MOVE_CONSTRAINT_Y,
	MOVE_CONSTRAINT_Z,
	MOVE_CONSTRAINT_MAX_NB,
};


static CONSTEXPR C_PCHAR_C g_plane_move_str[ MOVE_CONSTRAINT_MAX_NB ] =
{
	"Camera",
	"Plane XY",
	"Plane XZ",
	"Plane YZ",
	"X",
	"Y",
	"Z",
};

//hack
//	this and the is_changed are a quick hack
static	void bdd_curve_3d_param_update( c_param* param )	//param
{
	auto focus_param_obj = focus_param::get_obj();
	if( focus_param_obj )
	{
		if( c_bdd_curve_3d::is_instance(focus_param_obj) )
		{
			c_bdd_curve_3d*	obj = (c_bdd_curve_3d*)focus_param_obj;
			obj->force_curve_update();
		}
	}
}
void	c_bdd_curve_3d::force_curve_update()
{
	if( _curve )
		_curve->ask_update_points();
}

c_instance_by_channel< c_bdd_curve_3d, c_bdd_curve_3d::CURVE3D_CHANNEL_NB_MAX >	c_bdd_curve_3d::inst_by_channel;


namespace	n_bdd_curve_3d
{
	c_rand_lin	rand;

	CONSTEXPR INT32	BASE_PARAM_NB	=	5 + c_bdd::GEO_PARAM_NB;
	CONSTEXPR INT32	DRAW_PARAM_NB	=	15;
	CONSTEXPR INT32	UI_PARAM_NB		=	3;
	CONSTEXPR INT32	CURVE_PARAM_NB	=	5;
	CONSTEXPR INT32	POINT_PARAM_NB	=	4;
//	CONSTEXPR INT32	MARKER_PARAM_NB	=	7;
	CONSTEXPR INT32	GROUP_NB		=	4;	//was 5 with markers
	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
									+	DRAW_PARAM_NB
									+	UI_PARAM_NB
									+	CURVE_PARAM_NB
									+	POINT_PARAM_NB
//									+	MARKER_PARAM_NB
									+	GROUP_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_BASE_PARAMS

		PARAM_DEF_INT32(		channel_id,			2, 1,									1, c_bdd_curve_3d::CURVE3D_CHANNEL_NB_MAX )
//		PARAM_DEF_POINT_XYZ( center )
		PARAM_DEF_BOOL_ON(		curve_load_save		)
		PARAM_DEF_BOOL_OFF(		erase_all			)

		PARAM_DEF_GROUP( Drawing, DRAW_PARAM_NB )
			PARAM_DEF_BOOL_ON(			draw				)
			PARAM_DEF_BOOL_OFF(			draw_all_curves		)
			PARAM_DEF_BOOL_OFF(			draw_selected		)
			PARAM_DEF_UINT32(			draw_dataset_begin,	1, 1,									1, c_bdd_curve_3d::DATASET_ID_MAX	)
			PARAM_DEF_UINT32(			draw_dataset_end,	1, c_bdd_curve_3d::DATASET_ID_MAX,		1, c_bdd_curve_3d::DATASET_ID_MAX	)
			PARAM_DEF_REAL_ONE_ZERO(	draw_s_begin		)
			PARAM_DEF_REAL_ZERO_ONE(	draw_s_end			)
			PARAM_DEF_BOOL_OFF(			draw_force_color	)
			PARAM_DEF_BOOL_ON(			draw_point			)
			PARAM_DEF_BOOL_ON(			draw_curve			)
			PARAM_DEF_BOOL_OFF(			draw_number			)
			PARAM_DEF_REAL_ONE(			draw_number_scale	)
			PARAM_DEF_BOOL_OFF(			graduation			)
			PARAM_DEF_REAL_POS_ZERO(	graduation_step		)
			PARAM_DEF_REAL_POS_ONE(		graduation_scale	)

		PARAM_DEF_GROUP( Ui, UI_PARAM_NB )
//			PARAM_DEF_BOOL_OFF(			free_cam			)
			PARAM_DEF_SYMBO_PSTR_ZERO(	constraint,			g_plane_move_str )
			PARAM_DEF_FP32_ONE(			ui_alpha			)
//			PARAM_DEF_BOOL_OFF(			ui_draw				)
			PARAM_DEF_BOOL_OFF(			ui_intercept		)
//			PARAM_DEF_BOOL_OFF(			ui_lock				)
		//	PARAM_DEF_INT32(			curve_lod,			20, 10,		1, PARAM_MAX_INT32 )
			//PARAM_DEF_BOOL_OFF(		show_marker			)

		PARAM_DEF_INT32(				dataset_id,			2, 1,			1, c_bdd_curve_3d::DATASET_ID_MAX )
		PARAM_DEF_GROUP( Curve, CURVE_PARAM_NB )
			PARAM_DEF_INT32_LOCKED(			nb_u				)
			PARAM_DEF_SYMBO_SAVE_NOT_PSTR(	curve_type,			g_curve3d_str )
			PARAM_DEF_BOOL_OFF_SAVE_NOT(	curve_loop			)
			PARAM_DEF_INT32_SAVE_NOT(		curve_lod,			20, c_curve_3d::DEFAULT_LOD,		2, 8192 )
			PARAM_DEF_REAL_INF_SAVE_NOT(	curve_tightness,	0., c_curve_3d::DEFAULT_TIGHTNESS	)

			PARAM_DEF_GROUP( Point, POINT_PARAM_NB )
				PARAM_DEF_BASE_FN(	TYPE_INT32,				ui_u,		1, 0,	0, PARAM_MAX_INT32,				bdd_curve_3d_param_update	)
				PARAM_DEF_BASE_FN(	TYPE_REAL|M_SAVE_NOT,	pos_x,		1, 0,	PARAM_MIN_REAL, PARAM_MAX_REAL,	bdd_curve_3d_param_update	)
				PARAM_DEF_BASE_FN(	TYPE_REAL|M_SAVE_NOT,	pos_y,		1, 0,	PARAM_MIN_REAL, PARAM_MAX_REAL,	bdd_curve_3d_param_update	)
				PARAM_DEF_BASE_FN(	TYPE_REAL|M_SAVE_NOT,	pos_z,		1, 0,	PARAM_MIN_REAL, PARAM_MAX_REAL,	bdd_curve_3d_param_update	)

		PARAM_DEF_BOOL_OFF(				coor_to_world_use_z )
/*
		PARAM_DEF_GROUP_CLOSED( Marker, MARKER_PARAM_NB )
			PARAM_DEF_BOOL_OFF(				marker_draw		)
			PARAM_DEF_INT32(				nb_marker,		1, 1,	1, PARAM_MAX_INT32	)
			PARAM_DEF_INT32(				marker_ui_u,	1, 0,	1, PARAM_MAX_INT32	)
			PARAM_DEF_REAL_ZERO_SAVE_NOT(	marker_u		)
			PARAM_DEF_REAL_ZERO_SAVE_NOT(	marker_v		)
			PARAM_DEF_REAL_ZERO_SAVE_NOT(	marker_axe		)
			PARAM_DEF_STR_SAVE_NOT(			marker_text		)
*/	
	};
}

void	c_bdd_curve_3d::param_init_pt()
{
	INT32	h = param_init_pt_geo();

	param_set_pt( h, _channel_id			);
	param_set_pt( h, _b_curve_load_save		);
	param_set_pt( h, _b_erase_all_trig_ui	);

	++h;
		param_set_pt( h, _b_draw_ui				);
		param_set_pt( h, _b_ui_draw_all_ui		);
		param_set_pt( h, _b_draw_selected_ui	);
		param_set_pt( h, _draw_dataset_begin_ui );
		param_set_pt( h, _draw_dataset_end_ui	);
		param_set_pt( h, _draw_s_begin_ui		);
		param_set_pt( h, _draw_s_end_ui			);
		param_set_pt( h, _draw_force_color_ui	);	
		param_set_pt( h, _b_ui_draw_point_ui	);
		param_set_pt( h, _b_ui_draw_curve_ui	);
		param_set_pt( h, _b_ui_draw_number_ui	);
		param_set_pt( h, _number_scale_ui		);
		param_set_pt( h, _b_graduation_ui		);
		param_set_pt( h, _graduation_step		);
		param_set_pt( h, _graduation_scale		);

	++h;
	//	param_set_pt( h, _b_free_cam				);
		param_set_pt( h, _s_move_constraint			);
		param_set_pt( h, _ui_alpha					);
	//	param_set_pt( h, _b_ui_draw_ui				);
		param_set_pt( h, _edit.get_ui_intercept_pt() );
	//	param_set_pt( h, _b_ui_lock					);
	//	param_set_pt( h, _curve_lod					);
	//	param_set_pt( h, _b_marker_show				);
	//	param_set_pt( h, _b_compute_forced			);
		param_set_pt( h, _dataset_id_ui				);

	++h;
		param_set_pt( h, _nb_points_ui				);
		param_set_pt( h, _curve_info->_s_type		);
		param_set_pt( h, _curve_info->_b_loop		);
		param_set_pt( h, _curve_info->_lod			);
		param_set_pt( h, _curve_info->_tightness	);

//	//todo	typical problem to be solved, see bdd_grid_adjustable
	++h;
	//	param_set_max_no_inc( h, _controls->get_nb_u() );
		param_set_pt( h, _point_cur_index_ui );
	//	//param_set_pt_even_NULL( h, _u.get_point_pt( _iu_cur_ui ) );				//pbtosolve
		//hack	this need a generic solution in the architecture
		if( _curve->get_control_point_nb() > 0 && _point_cur_index_ui < _curve->get_control_point_nb() )
			param_set_pt_3( h, (REAL*) _curve->get_control_point( _point_cur_index_ui ) );
		else
			param_set_pt_3( h, _dummy );	//pbtosolve

	param_set_pt( h, _b_coor_to_world_use_z_ui );

/*
	++h;
		param_set_pt( h, _b_draw_marker_ui );
		param_set_pt( h, _marker_nb_ui );
		param_set_pt( h, _marker_cur_ui );
		//	//param_set_pt_even_NULL( h, _u.get_point_pt( _iu_cur_ui ) );				//pbtosolve

		//if( _map_controls.get( _dataset_id )->get_nb_u() > 0 )
		//{
		//	param_set_pt_3( h, _map_controls.get( _dataset_id )->get_point( _iu_cur_ui, 0 ) );
		//}
		//else
		{
			param_set_pt_3( h, _dummy );	//pbtosolve
		}
		param_set_pt( h, _marker_str );
*/
	err_param_init_pt( h );
}

void	c_bdd_curve_3d::init()
{
	param_init_with( n_bdd_curve_3d::param, n_bdd_curve_3d::PARAM_NB_MAX );
	clear_v3( (REAL*) _dummy );
}

/*
FINLINE	void	c_bdd_curve_3d::alloc_curve()
{
	//HEAP_IS_CORRUPT();
	if ( _dataset_id > 0 )
	{
		_curve = get_curve( _dataset_id );

		//_controls = _map_controls.get( _dataset_id );
		//_controls_def = _map_controls_def.get( _dataset_id );

		//if( _controls && _controls->get_nb_u() != _nb_points[ _dataset_id - 1 ] )
		//{
		//	_controls->set_nb( _nb_points[ _dataset_id - 1 ], 1, false );
		//	_controls_def->set_nb( _nb_points[ _dataset_id - 1 ], 1 );
		//	//HEAP_IS_CORRUPT();
		//}
	}
}
*/

FINLINE void	c_bdd_curve_3d::sync_curve_and_info( c_curve_3d* curve, c_curve_info* info )
{
	curve->set_type(	info->_s_type	);
	curve->set_loop(	info->_b_loop	);
	curve->set_lod(		info->_lod		);
	//	curve->set_tightness( curve_info->_curve_tightness );
	curve->update();
}

void	c_bdd_curve_3d::sync_curve( INT32 id )
{
	sync_curve_and_info( get_curve( id ), get_info( id ) );
}

void	c_bdd_curve_3d::set_dataset( INT32 dataset_id )
{
	if( _dataset_id != dataset_id )
	{
		dataset_id = WRAP_ID( dataset_id, DATASET_ID_MAX );
		_dataset_id = dataset_id;
		_b_curve_index_changed = true;
		_curve_info = get_info( dataset_id );
		_curve = find_curve( dataset_id );
		if( !_curve )
		{
			_curve = get_curve( dataset_id );
			sync_curve_and_info( _curve, _curve_info );
		}
	}
}

void	c_bdd_curve_3d::set_curve_ui( INT32 id )
{
	_dataset_id_ui = WRAP_ID( id, DATASET_ID_MAX );
	set_dataset( _dataset_id_ui );
	set_point_control_cur( _point_cur_index );
}


CONSTRUCTOR_CREATE( c_bdd_curve_3d )
,_dataset_id								(	-1		)
,_dataset_id_ui								(	1		)
,_curve										(	nullptr	)
//,_map_curve								(	-42		)
//,_map_curve_info							(	-42		)
,_point_cur_index							(	0		)
,_draw_dataset_begin						(	1		)
,_draw_dataset_end							(	1		)
,_b_ui_edit									(	false	)
,_point_cur_index_ui						(	0		)				//need initialization because of the hack to change point in Param
,_b_valid_curve_for_get_point_rnd_compute	(	true	)
{
//HEAP_IS_CORRUPT();
//	c_init();
	set_dataset( _dataset_id_ui ); //we need one here to avoid crash in init
	init();
//	update_markers();
}

//todo too violent will ne to 
void	c_bdd_curve_3d::clear_datasets()
{
	_curve_info = nullptr;
	_map_curve_info.clear_elts();
	_curve = nullptr;
	_map_curve.clear_elts();
	_dataset_id = -1;
}

//todo dealloc generic strategy
void	c_bdd_curve_3d::dealloc()
{
	_curve_info = nullptr;
	_map_curve_info.dealloc_elts();
	_curve = nullptr;
	_map_curve.dealloc_elts();
	_dataset_id = -1;
}

c_bdd_curve_3d::~c_bdd_curve_3d()
{
	dealloc();
//todo dealloc
/*
	if( _markers )
	{
		_markers->dealloc();
	}
*/
}

#include "file/dirparser.h"

namespace {
	//these two function to count quicky the number of loops to do reading files
	FINLINE INT32 get_number( C_PCHAR str, INT32 digit_nb )
	{
		INT32 nb = 0;
		do 
		{
			INT32 digit = *str - '0';
			if( INSIDE_MIN_MAX( digit, 0, 9 ) )
				nb = nb*10 + digit;
			else
				break;
			++str;
		}
		while(--digit_nb);
		return nb;
	}

	INT32 find_max( o_str CONST & filename )
	{
		o_str& dir = o_str::push_name();
		o_str& name = o_str::push_name();
			dir.set_dir_name( filename );
			name.set_fname_relative( filename, dir );
			name.add( ".*" );
			INT32 len = name.get_len() - 1;	//we need it ti get the number in the loop 

			INT32 nb = 0;
			c_dir_parser pa( dir.get(), name.get() );

			while( pa.get_next() )
			{
				if( !pa.is_dir() )
				{
					C_PCHAR_C str = pa.get_name();
					nb = MAX( nb, get_number( str+len, c_bdd_curve_3d::DATASET_ID_DIGIT_MAX ) );
	//				PRINT_STRING( "  %s %d\n", str, nb );
				}
			}
		o_str::pop_name();
		o_str::pop_name();
		return nb;
	}
}

namespace {
	CONST CHAR  numb_ext[]		= ".0000" ;
	CONST CHAR  data_ext[]		= "curve_data";
	CONST CHAR  info_ext[]		= "curve_info";
//	CONST CHAR  marker_ext[]	= "curve_marker";

	FINLINE void make_name( INT32 i, o_str& name, o_str& name_info )
	{
		if( i>1 )
		{
			if( i==2 )
			{
				name		.add( numb_ext );
				name_info	.add( numb_ext );
			}
			name		.set_digits( -c_bdd_curve_3d::DATASET_ID_DIGIT_MAX, c_bdd_curve_3d::DATASET_ID_DIGIT_MAX, i );
			name_info	.set_digits( -c_bdd_curve_3d::DATASET_ID_DIGIT_MAX, c_bdd_curve_3d::DATASET_ID_DIGIT_MAX, i );
		}
	}
};

AAA_ERR	c_bdd_curve_3d::load_do_after( o_str CONST & filename )
{
	if( !_b_curve_load_save )
		return AAA_OK;

	c_file::push_vfile();	//not saved in the vfile we want to access it as a single file

		o_str&	name = o_str::push_name( filename );
		name.replace_ext( data_ext );
		INT32 nb = find_max(name);	// find higher index present

		o_str&	name_info = o_str::push_name( filename );
		name_info.replace_ext( info_ext );
		nb = aaa::MAX( nb, find_max(name_info), 1 );	// find higher index present
	/*
		o_str&	name_marker = o_str::push_name( filename );
		name_marker.replace_ext( marker_ext );
	*/
			for( INT32 i = 1; i <= nb; ++i )
			{
				make_name( i, name, name_info );

				if( c_file::is_exist(name) )
				{
					c_curve_3d* _curve = get_curve( i );
					_curve->read_from_file( name.get() );
				}
				if( c_file::is_exist(name_info) )
				{
					c_curve_info* info  = get_info( i );
					info->load( name_info.get() );
					sync_curve( i );
				}
				// load/reactivate markers todoqqq
	/*
				if( c_file::is_exist( name_marker ) )
				{
					// load marker
					update_markers();
					_markers->read_marker_from_file( name );
					curve_info->_nb_markers = _markers->get_nb_u();
				}
	*/
			}

		o_str::pop_name();
		o_str::pop_name();

	c_file::pop_vfile();

	return AAA_OK;
}

AAA_ERR	c_bdd_curve_3d::save_do_after( o_str CONST & filename )
{
	if( !_b_curve_load_save )
		return AAA_OK;

	c_file::push_vfile();

		o_str&	name = o_str::push_name( filename );
		name.replace_ext( data_ext );
		o_str&	name_info = o_str::push_name( filename );
		name_info.replace_ext( info_ext );

			//hack this was done too fast see below
			//todoopt we need to parse dir and remove unused files
			//	not try to remove 10000 files like here
			for( INT32 i = 1; i <= DATASET_ID_MAX; ++i )
			{
				make_name( i, name, name_info );

				c_curve_3d* curve = find_curve( i );
				c_curve_info* info;

				if( curve && curve->get_control_point_nb()>0 )
				{
					// do not save for now!!! check for bug first!!!
					curve->write_in_file( name.get() );
					info = find_info( i );
				}
				else
				{
					//hack not optimum
					//if( c_file::is_exist( name ) )
						c_file::FREMOVE( name );
					info = nullptr;
				}
				if( info )
					info->save( name_info.get() );
				else
				{
					//hack not optimum
					//if( c_file::is_exist( name_info ) )
						c_file::FREMOVE( name_info );
				}
				//write marker
	/*			if( _map_markers.is_exist(i) )
				{
					strnum::make( pt_num_marker, DATASET_ID_DIGIT_MAX, i );
					_map_markers.get(i)->write_marker_in_file( name );
				}
	*/
			}

		o_str::pop_name();
		o_str::pop_name();

	c_file::pop_vfile();

	return AAA_OK;
}

//todo check seem not to work in MEU 
void	c_bdd_curve_3d::center_cam()
{
	UINT32 nb = _curve->get_control_point_nb();
	if( nb > 0 && _point_cur_index < nb )
	{
		REAL*	pt = _curve->get_control_point( _point_cur_index );
		if( pt )
		{
			auto cam = c_layers::get_layers_camera_used();
			//auto cam = c_seedcam::get_ui();
			cam->set_center( pt );
			cam->set_tra( .0f, .0f );
		}
	}
}

INT32	c_bdd_curve_3d::get_control_point_nb( INT32 dataset_id )
{
	dataset_id = WRAP_ID( dataset_id, DATASET_ID_MAX );
	c_curve_3d*	curve = find_curve( dataset_id );
	return curve ? curve->get_control_point_nb() : 0;
}

void	c_bdd_curve_3d::push_control_point_back( INT32 dataset_id, REAL CONST * CONST src )
{
	dataset_id = WRAP_ID( dataset_id, DATASET_ID_MAX );
	c_curve_3d*	curve = get_curve( dataset_id );
	curve->add_control_point( src );
//	set_point_control_cur( curve->get_control_point_nb(), false );
}

void	c_bdd_curve_3d::insert_control_point( INT32 index, REAL CONST * CONST src )
{
	_curve->insert_control_point( index, src );
	set_point_control_cur( index + 1 );
}

void	c_bdd_curve_3d::delete_control_point( INT32 index )
{
	//todo refine we should use cur stuff
	_curve->delete_control_point( index );
	if( index != 0 )
		set_point_control_cur( index -1 );
}

void	c_bdd_curve_3d::pop_control_point_front( INT32 dataset_id )
{
	dataset_id = WRAP_ID( dataset_id, DATASET_ID_MAX );
	c_curve_3d*	curve = find_curve( dataset_id );
	if( curve )
		return curve->delete_control_point( 0 );
}

void	c_bdd_curve_3d::clear_control_points( INT32 dataset_id )
{
	dataset_id = WRAP_ID( dataset_id, DATASET_ID_MAX );
	c_curve_3d*	curve = find_curve( dataset_id );
	if( curve )
		return curve->clear();
}

//maa	Marker are fucked up and in a strange state so we suspend it for now (august 2012)
/*
FINLINE	void	c_bdd_curve_3d::alloc_markers()
{
	//HEAP_IS_CORRUPT();
	if ( _dataset_id > 0 )
	{
		_markers = _map_markers.get( _dataset_id );

		if( _markers && _markers->get_nb_u() != _curve_info->_nb_markers )
		{
			_markers->set_nb( _curve_info->_nb_markers, 1, false );
			//HEAP_IS_CORRUPT();
		}
	}
}

void	c_bdd_curve_3d::update_markers()
{
	_markers = _map_markers.get( _dataset_id );
	alloc_markers();
}
*/
/*
void	c_bdd_curve_3d::add_marker()
{
	++(_curve_info->_nb_markers);

	if( _curve_info->_nb_markers == 1 )
	{
		REAL	zero[] = { .0f, .0f, .0f };
		set_marker( 0, zero, "" );
		set_marker_cur( 0 );
	}
	else
	{
		//REAL*	pt = _map_controls.get( _dataset_id )->get_point( _point_cur_index, 0 );
		//REAL	delta[] = { .1f, .1f, .0f };
		//add_v3r( delta, pt );
		//set_control_point( _nb_points[ _dataset_id - 1 ]-1, delta );
		////_p_controls->set_point( _nb_u_ui-1, 0, pt );
		//set_point_control_cur( _nb_points[ _dataset_id - 1 ]-1 );
	}
}

void	c_bdd_curve_3d::insert_marker()
{
	if( _point_cur_index == _curve_info->_nb_markers-1 )
	{
		add_marker();
		return;
	}

	++(_curve_info->_nb_points);

	if( _curve_info->_nb_points == 1 )
	{
		REAL	zero[] = { .0f, .0f, .0f };
		set_control_point( 0, zero );
		set_point_control_cur( 0 );
	}
	else
	{
		//for( INT32 i = _nb_points[ _dataset_id - 1 ]-1; i > _point_cur_index; --i )
		//{
		//	set_control_point( _dataset_id, i, _map_controls.get( _dataset_id )->get_point( i-1, 0 ) );
		//	//_controls->set_point( i, 0, _controls->get_point( i-1, 0 ) );
		//}
		//REAL	dst[3];
		//if( _nb_points[ _dataset_id - 1 ] > 1 )
		//{
		//	if( _point_cur_index >= _nb_points[ _dataset_id - 1 ]-2 )
		//	{
		//		set_control_point( _dataset_id, _map_controls.get( _dataset_id )->get_point( _nb_points[ _dataset_id - 1 ]-1, 0 ) );
		//		//_controls->set_point( 1, 0, _controls->get_point( _nb_u_ui-1, 0 ) );
		//	}
		//	else
		//	{
		//	interpolate_v3r( dst, _map_controls.get( _dataset_id )->get_point( _point_cur_index, 0 ), _map_controls.get( _dataset_id )->get_point( _point_cur_index + 2, 0 ), 0.5 );
		//	//_controls->set_point( _iu_cur+1, 0, dst );
		//	set_control_point( _dataset_id, _point_cur_index+1, dst );
		//	}
		//}
		//else
		//{
		//	//_controls->set_point( 1, 0, _controls->get_point( 0, 0 ) );
		//	set_control_point( _dataset_id, 0, _map_controls.get( _dataset_id )->get_point( 0, 0 ) );
		//}

		set_point_control_cur( _point_cur_index + 1 );
	}
}
*/

void	c_bdd_curve_3d::update()
{
	_b_ui_edit = _edit.is_ui_intercept();
	if( _b_ui_edit )
	{
		ui_register( this );
		//_b_ui_draw_ui = true;
	}
	inst_by_channel.set( _channel_id, this );

	//_cam_edit_index = c_layers::cur->get_camera_ui_index();

	if( _b_erase_all_trig_ui )
	{
		_b_erase_all_trig_ui = false;
		dealloc();
	}

	c_model::cur->get_size_v3( _size );

	//todo not perfect but decent eventually infinite loop
	if( _b_ui_draw_all_ui )
	{
		_draw_dataset_begin	= 1;
		_draw_dataset_end	= DATASET_ID_MAX;
		_draw_s_begin_begin	= 0.;
		_draw_s_begin_end	= 1.;
		_draw_s_end_begin	= 0.;
		_draw_s_end_end		= 1.;
	}
	else
	{
		if( _b_draw_selected_ui )
		{
			_draw_dataset_begin	= _dataset_id;
			_draw_dataset_end	= _dataset_id;
		}
		else
		{
			if( _draw_dataset_begin_ui > _draw_dataset_end_ui )
			{
				_draw_dataset_begin	= _draw_dataset_end_ui;
				_draw_dataset_end	= _draw_dataset_begin_ui;
			}
			else
			{
				_draw_dataset_begin	= _draw_dataset_begin_ui;
				_draw_dataset_end	= _draw_dataset_end_ui;
			}
		}

		_draw_s_begin_begin	= _draw_s_begin_ui;
		_draw_s_end_end		= _draw_s_end_ui;
		if( _draw_dataset_begin == _draw_dataset_end )
		{
			_draw_s_begin_end	= _draw_s_end_end;
			_draw_s_end_begin	= _draw_s_begin_begin;
		}
		else
		{
			_draw_s_begin_end	= 1.0;
			_draw_s_end_begin	= 0.0;
			//we need to do this to avoid infinite loop in get_point_rnd()
			if( _draw_s_begin_begin == 1. )
			{
				++_draw_dataset_begin;
				_draw_s_begin_begin = 0.; 
			}
			if( _draw_s_end_end == 0. )
			{
				--_draw_dataset_end;
				_draw_s_end_end = 1.; 
			}
			if( _draw_dataset_begin == _draw_dataset_end )
				_draw_s_begin_end = _draw_s_end_end;
		}
	}

	_b_valid_curve_for_get_point_rnd_compute	= true;
//	update_markers();

//	HEAP_IS_CORRUPT();

	if( g_picked_def.get_number() > 0 )
	{
		auto layer_id = c_layer::get_cur()->get_obj_ui_id();
		if( g_picked_def.picked_by_id( layer_id ) )
		{
			g_picked_def.print();
			//analyse picked
			st_picked	picked;
			picked.sub_id.clear();
			g_picked_def.get_highest_by_id( &picked, layer_id );
			g_picked_def.print();
			if( picked.nb == 2 )
			{
				// point selected
				INT32	iu = picked.sub_id.back() - 1;
				picked.sub_id.pop_back();
				INT32	data_id = picked.sub_id.back() - 1;
				picked.sub_id.pop_back();

				_dataset_id_ui = data_id;
				_point_cur_index_ui = iu;
			}
			// remove picked for this layer
			g_picked_def.remove_by_id( layer_id );
		}
	}

	set_dataset( _dataset_id_ui );
	set_point_control_cur( _point_cur_index_ui );
	sync_curve( _dataset_id );
	_nb_points_ui = _curve ? _curve->get_control_point_nb() : 0;

	//do_deform();
	/*
	for( UINT32 j = _draw_dataset_begin; j <= _draw_dataset_end; ++j )
	{
		_controls = _map_controls.get( j );
		_controls_def = _map_controls_def.get( j );
		if( _controls && _controls_def && _b_ui_deform )
		{
			auto def = c_def_node::get_cur();
			if( def->is_deforming() )
				def->apply( _controls_def->get_points(), _controls->get_points(), _controls->get_nb() );
		}
	}
	*/
	_cam_used = c_seedcam::get_cur();
}

//
//	DRAW
//

//	FINLINE	void	c_bdd_curve_3d::set_color_white(	REAL alpha_in )		{	c_color::cur->draw_with_color_scaled( 1.f, 1.f, 1.f, alpha_in );	}
//	FINLINE	void	c_bdd_curve_3d::set_color_red(		REAL alpha_in )		{	c_color::cur->draw_with_color_scaled( 1.f, 0.f, 0.f, alpha_in );	}
//	FINLINE	void	c_bdd_curve_3d::set_color_green(	REAL alpha_in )		{	c_color::cur->draw_with_color_scaled( 0.f, 1.f, 0.f, alpha_in );	}

FINLINE	void	set_color_construction(				REAL alpha_in = 1.f )	{	c_color::get_cur()->draw_with_color_scaled( 0.f, .5f, 1.f, alpha_in );	}
FINLINE	void	set_color_construction_selected(	REAL alpha_in = 1.f )	{	c_color::get_cur()->draw_with_color_scaled( 0.f, 1.f, 1.f, alpha_in );	}
FINLINE	void	set_color_construction_bis(			REAL alpha_in = 1.f )	{	c_color::get_cur()->draw_with_color_scaled( 0.f, .8f, 1.f, alpha_in );	}

/*
void	c_bdd_curve_3d::draw_marker( INT32 u )
{
	// todo
	// draw marker
}
*/

void	c_bdd_curve_3d::draw_points()
{
	if( c_picking::is_cur() )
	{
		c_picking*	picking = c_picking::get_cur();
		picking->push_name( -1 );
		for( UINT32 j = _draw_dataset_begin; j <= _draw_dataset_end; ++j )
		{
			c_curve_3d*	curve = find_curve( j );
			if( curve && (curve->get_control_point_nb() > 0) )
			{
				picking->set_pick_ref( j + 1 );
				INT32	i_end = curve->get_control_point_nb() - 1;
				for( INT32 i = 0; i <= i_end; ++i )
				{
					picking->cur_push_name( i + 1 );
					picking->cur_push_name( 2 );
						if( j == _dataset_id )
						{
							//if ( i == 0 || i == i_end )	set_color_construction( 1.0f );
							if( i == _point_cur_index )
								set_color_ui_selected( 1.0f );
							else
								set_color_ui( 1.0f );
						}
						else
							set_color_ui( 1.0f );
						GOL::begin( GL_POINTS );
							GOL::vertex3v( (REAL*) curve->get_control_point( i ) );
						GOL::end();
					picking->cur_pop_name();
					picking->cur_pop_name();
				}
			}
		}
		picking->cur_pop_name();
	}
	else
	{
		//todo drawing is old school
		for( UINT32 j = _draw_dataset_begin; j <= _draw_dataset_end; ++j )
		{
			c_curve_3d*	curve = find_curve( j );
			if( curve && (curve->get_control_point_nb() > 0) )
			{
				GOL::begin( GL_POINTS );
					INT32	i_end = curve->get_control_point_nb() - 1;
					if( _draw_force_color_ui )
					{
						for( INT32 i = 0; i <= i_end; ++i )
						{
							if( j == _dataset_id )
							{
								//if ( i == 0 || i == i_end )	set_color_construction( 1.0f );
								if( i == _point_cur_index )
									set_color_ui_selected( 1.0f );
								else
									set_color_ui( 1.0f );
							}
							else
								set_color_ui( 1.0f );
							GOL::vertex3v( (REAL*) curve->get_control_point( i ) );
						}
					}
					else
					{
						for( INT32 i = 0; i <= i_end; ++i )
							GOL::vertex3v( (REAL*) curve->get_control_point( i ) );
					}
				GOL::end();
			}
		}
	}

}

//todo implement the align mode
//todo deal with tex_anim
void	c_bdd_curve_3d::draw_points_multiple()
{
	REAL CONST * CONST size = c_multiple::cur->get_size();
	INT32 axe = c_multiple::cur->get_axe();

	for( UINT32 j = _draw_dataset_begin; j <= _draw_dataset_end; ++j )
	{
		c_curve_3d*	curve = find_curve( j );
		if( curve && (curve->get_control_point_nb() > 0) )
		{
			INT32	nb = curve->get_control_point_nb();
			if( nb > 0 )
			{
				c_multiple::cur->set_nb( nb, _draw_dataset_end );
				if( c_multiple::cur->is_align_normal() )
				{
					REAL*	p;
					if( nb > 1 )
					{
						--nb;
						REAL*	n;
						REAL	nor[3];
						for( INT32 i = 0; i < nb; ++i )
						{
							//todo this just a hack
							p = (REAL*) curve->get_control_point( i );
							n = (REAL*) curve->get_control_point( i+1 );
							sub_v3( nor, n, p );
							c_multiple::cur->draw_one_at_nor( p, size, &nor[0] );
							c_multiple::cur->next_index_u();
						}
						c_multiple::cur->draw_one_at_nor( n, size, &nor[0] );
					}
					else
					{
						//todo bad but should not crash
						p = (REAL*) curve->get_control_point( 0 );
						c_multiple::cur->draw_one_at_nor( p, size, p );
					}
				}
				else
				{
					for( INT32 i = 0; i < nb; ++i )
					{				
						c_multiple::cur->align_then_draw( (REAL*)curve->get_control_point( i ), size, axe );
						c_multiple::cur->next_index_u();
					}
				}
			}
		}
		c_multiple::cur->next_index_v();
	}
}

void	c_bdd_curve_3d::draw_numbers()
{
	for( UINT32 j = _draw_dataset_begin; j <= _draw_dataset_end; ++j )
	{
		c_curve_3d*	curve = find_curve( j );
		if( curve && (curve->get_control_point_nb() > 0) )
		{
			INT32 i_end = curve->get_control_point_nb() - 1;
			for( INT32 i = 0; i <= i_end; ++i )
			{
				if( j == _dataset_id )
				{
					//if ( i == 0 || i == i_end )
					//	set_color_construction( 1.0f );
					if( i == _point_cur_index )	
						set_color_ui_selected( 1.0f );
					else
						set_color_ui( 1.0f );
				}
				else
					set_color_ui( 1.0f );

				GOL::matrix::push();
					REAL*	f = (REAL*) curve->get_control_point( i );
					GOL::matrix::translate3v( f );
					billboard::do_y( f );
					GOL::matrix::scale( _number_scale_ui );
					aaa::alphabet::draw_int32( i, 0 );
				GOL::matrix::pop();
			}
		}
	}
}

void	c_bdd_curve_3d::draw_graduation()
{
	INT32	dec_nb;
	if( _graduation_step == F_FLOOR( _graduation_step ) )
		dec_nb = 0;
	else
		dec_nb = 2;

	for( UINT32 j = _draw_dataset_begin; j <= _draw_dataset_end; ++j )
	{
		c_curve_3d*	curve = find_curve( j );
		if( curve && (curve->get_control_point_nb() > 0) )
		{
			REAL ds     = _graduation_step;
			REAL s_end  = REAL(curve->get_len());
			REAL s_over = OVER_ONE_AS_REAL(s_end);
			for( REAL s = 0; s <= s_end; s += ds ) 
			{
				REAL pos[3];
				REAL tangent[3];
				GOL::matrix::push();
					curve->get_point_tangent( pos, tangent, s * s_over );
					GOL::matrix::build_matrix_pos_nor_then_mul( pos, tangent );
					GOL::matrix::scale( _graduation_scale );
					GOL::matrix::rotate_z( -.25 );
					n_axe::draw_units();
				GOL::matrix::pop();

				GOL::matrix::push();
					GOL::matrix::translate3v( pos );
					billboard::do_y( pos );
					GOL::matrix::scale( _number_scale_ui );
					set_color_construction_bis( 1.0f );
					aaa::alphabet::draw_real( s, dec_nb );
				GOL::matrix::pop();
			}
		}
	}
}

/*
void	c_bdd_curve_3d::draw_markers()
{
	for( UINT32 j = _draw_dataset_begin; j <= _draw_dataset_end; ++j )
	{
		_markers = _map_markers.get( j );
		//if( _markers && _controls->get_nb_u() )
		//{
		//	if( j != _dataset_id )
		//	{
		//		set_color_white();
		//	}
		//	INT32	i_end = _map_markers.get( j )->get_nb_u() - 1;
		//	for( INT32 i = 0; i <= i_end; ++i )
		//	{
		//		if( ( i == _point_cur_index ) && ( j == _dataset_id ) )
		//		{
		//			set_color_ui_selected( 1.0f );
		//			draw_marker( i );
		//			set_color_ui( 1.0f );
		//		}
		//		else
		//		{
		//			draw_marker( i );
		//		}
		//	}
		//}
	}
}
*/

namespace {
	c_guf		guf(256);
}

void	c_bdd_curve_3d::draw_curves()
{
	GOL::unbind_vao_secu();

	if( GOL::b_draw_avoid_vertex_use )
		guf.draw_before( false, false );
	if( _draw_force_color_ui )
		set_color_construction( 1.0f );
	for( UINT32 j = _draw_dataset_begin; j <= _draw_dataset_end; ++j )
	{
		c_curve_3d*	curve = find_curve_valid( j );
		if( curve )
		{
			UINT32	points_nb = curve->get_point_curve_nb();
			if( points_nb > 0 )
			{
				if( _draw_force_color_ui )
				{
					if( j == _dataset_id )
						set_color_ui_selected( 1.0f );
					else
						set_color_ui( 1.0f );
				}

				DOUBLE s_begin, s_end;
				if( j == _draw_dataset_begin )
				{
					s_begin = _draw_s_begin_begin;
					s_end = _draw_s_begin_end;
				}
				else if( j == _draw_dataset_end )
				{
					s_begin = _draw_s_end_begin;
					s_end = _draw_s_end_end;
				}
				else
				{
					s_begin = 0.;
					s_end	= 1.;
				}

				// todo use curve data, and use buffer array to speed up display
				bool	b_loop_spe = !curve->is_loop() && curve->get_type() == c_curve_3d::CURVE_CATMULLROM && s_begin==0. && s_end==1.;
				INT32	nb_draw;
				REAL* pt = curve->get_point_curve_pt();

				if( GOL::b_draw_avoid_vertex_use  )
				{
					if( guf.get_nb() < points_nb + 2 )	// max for alloc
					{
						guf.draw_after();
						guf.alloc_point( points_nb + 2, __FUNCTION__ );
						guf.draw_before( false, false );
					}
					REAL*	p = guf.get_point();
					if( b_loop_spe )
					{
						cpy_v3( p, curve->get_control_point( 0 ) );
						p += 3;
						cpy_v3n( p, pt, points_nb );
						cpy_v3( p, curve->get_control_point( curve->get_control_point_nb()-1 ) );
						nb_draw = points_nb + 2;
					}
					else
					{					
						INT32 begin_prev, begin_next;
						curve->find_index( s_begin, begin_prev, begin_next );
						pt += begin_prev*3;

						INT32 end_prev, end_next;
						curve->find_index( s_end, end_prev, end_next );
						nb_draw = MAX( end_next - begin_prev + 1, 0 );

						cpy_v3n( p, pt, nb_draw );
					}
					guf.draw_low( GL_LINE_STRIP, nb_draw );
				}
				else
				{	//	old school
					GOL::begin( GL_LINE_STRIP );
						if( b_loop_spe )
							GOL::vertex3v( curve->get_control_point( 0 ) );

						//todo do a fn for than
						for( UINT32 i = 0; i < points_nb; ++i )
						{
							GOL::vertex3v( pt );
							pt += 3;
						}

						if( b_loop_spe )
							GOL::vertex3v( curve->get_control_point( curve->get_control_point_nb()-1 ) );
					GOL::end();
				}
			}
		}
	}
	if( GOL::b_draw_avoid_vertex_use )
		guf.draw_after();
}

namespace{
	REAL CONSTEXPR	DELTA = 5.f;
	REAL CONSTEXPR	DEL_B = 6.f;
	REAL CONSTEXPR	DEL_C = 1.f;
}

void	c_bdd_curve_3d::draw_constraint( INT32 iu, INT32 iv, INT32 ia, REAL* v )
{
	REAL	vec[3];
	vec[ia] = v[ia];
	c_color::mod->draw_red( _ui_alpha );
		vec[iv] = -DELTA;
			vec[iu] = -DEL_B;
				GOL::vertex3v(	vec	);
			vec[iu] = DEL_B;	
				GOL::vertex3v(	vec	);
		vec[iv] = DELTA;
			vec[iu] = -DEL_B;
				GOL::vertex3v(	vec	);
			vec[iu] = DEL_B;
				GOL::vertex3v(	vec	);

	c_color::mod->draw_green( _ui_alpha );
		vec[iu] = -DELTA;
			vec[iv] = -DEL_B;
				GOL::vertex3v(	vec	);
			vec[iv] = DEL_B;
				GOL::vertex3v(	vec	);
		vec[iu] = DELTA;
			vec[iv] = -DEL_B;
				GOL::vertex3v(	vec	);
			vec[iv] = DEL_B;
				GOL::vertex3v(	vec	);

	REAL c = v[ia];
	c_color::mod->draw_blue( _ui_alpha );
		vec[iu] = -DELTA;
		vec[iv] = -DELTA;
			vec[ia] = c-1;
				GOL::vertex3v(	vec	);
			vec[ia] = c+1;
				GOL::vertex3v(	vec	);
		vec[iu] = DELTA;
		vec[iv] = -DELTA;
			vec[ia] = c-1;
				GOL::vertex3v(	vec	);
			vec[ia] = c+1;
				GOL::vertex3v(	vec	);
		vec[iu] = -DELTA;
		vec[iv] = DELTA;
			vec[ia] = c-1;
				GOL::vertex3v(	vec	);
			vec[ia] = c+1;
				GOL::vertex3v(	vec	);
		vec[iu] = DELTA;
		vec[iv] = DELTA;
			vec[ia] = c-1;
				GOL::vertex3v(	vec	);
			vec[ia] = c+1;
				GOL::vertex3v(	vec	);

	cpy_v3( vec, v );
	c_color::mod->draw_red( _ui_alpha );
		GOL::vertex3v( vec );
		vec[iu] = 0; 
		GOL::vertex3v( vec );

	cpy_v3( vec, v );
	c_color::mod->draw_green( _ui_alpha );
		GOL::vertex3v( vec );
		vec[iv] = 0; 
		GOL::vertex3v( vec );

	cpy_v3( vec, v );
	c_color::mod->draw_blue( _ui_alpha );
		GOL::vertex3v( vec );
		vec[ia] = 0;
		GOL::vertex3v( vec );

}

void	c_bdd_curve_3d::draw_constraint()
{
	if( _curve->get_control_point_nb() > 0 && _point_cur_index < _curve->get_control_point_nb() )
	{
		REAL*	pt = (REAL*)_curve->get_control_point( _point_cur_index );
		if ( pt )
		{
			c_color::mod->draw_white( _ui_alpha );
			GOL::begin( GL_LINES );
				switch ( _s_move_constraint )
				{
				case MOVE_CONSTRAINT_X:
					//*dst = u_in;
					GOL::vertex3( -DELTA, *(pt+1), *(pt+2) );
					GOL::vertex3(  DELTA, *(pt+1), *(pt+2) );
					break;
				case MOVE_CONSTRAINT_Y:
					//*(dst+1) = v_in;
					GOL::vertex3( *(pt), -DELTA , *(pt+2) );
					GOL::vertex3( *(pt),  DELTA , *(pt+2) );
					break;
				case MOVE_CONSTRAINT_Z:
					//*(dst+2) = u_in;
					GOL::vertex3( *(pt), *(pt+1), -DELTA );
					GOL::vertex3( *(pt), *(pt+1),  DELTA );
					break;
				case MOVE_CONSTRAINT_XZ:	draw_constraint( 0, 2, 1, pt );	break;
				case MOVE_CONSTRAINT_YZ:	draw_constraint( 1, 2, 0, pt );	break;
				case MOVE_CONSTRAINT_XY:
				default:					draw_constraint( 0, 1, 2, pt );	break;
				}
			GOL::end();
		}
	}
}

void	c_bdd_curve_3d::draw_single()
{
	begin_ui();

	if( is_ui_intercept() && _s_move_constraint > 0 )
		draw_constraint();
	//	Draw point
	if(	is_ui_intercept() || ( _b_draw_ui && _b_ui_draw_point_ui ) )
		draw_points();
	if( is_ui_intercept() || ( _b_draw_ui && _b_ui_draw_curve_ui ) )
		draw_curves();
	if( is_ui_intercept() || ( _b_draw_ui && _b_ui_draw_number_ui ) )
		draw_numbers();
	if( _b_graduation_ui && _graduation_step != 0. )
		draw_graduation();

	//if( _b_marker_show )
	//{
	//	for( size_t j = 1; j <= DATASET_ID_MAX; ++j )
	//	{
	//		_controls = _map_controls.get( j );
	//		_controls_def = _map_controls_def.get( j );
	//		if( _controls )
	//		{
	//			if( _b_ui_draw_all_ui || ( j == _dataset_id ) )
	//			{
	//				if( j != _dataset_id )
	//				{
	//					set_color_white();
	//				}
	//				INT32	i_end = _map_controls.get( j )->get_nb_u() - 1;
	//				for( size_t i = 0; i <= i_end; ++i )
	//				{
	//					if( ( i == _iu_cur ) && ( j == _dataset_id ) )
	//					{
	//						set_color_ui_selected( 1.0f );
	//						draw_marker( i );
	//						set_color_ui( 1.0f );
	//					}
	//					else
	//					{
	//						draw_marker( i );
	//					}
	//				}
	//			}
	//		}
	//	}
	//}
	//if( _b_draw_marker_ui )
	//	draw_markers();

	end_ui();
}

void	c_bdd_curve_3d::draw_multiple()
{

//	if( c_multiple::cur->is_render_multiple() )
	{
//		c_multiple::cur->set_nb( _nb_points[ _dataset_id - 1 ] );
		if( !c_multiple::cur->begin() )
			goto exit;
	}

	draw_points_multiple();

//	if( c_multiple::cur->is_render_multiple() )
exit:
		c_multiple::cur->end();
}

FINLINE	void	c_bdd_curve_3d::set_color_ui( REAL alpha_in )
{
	if( _b_ui_edit )
		c_color::mod->draw_green( alpha_in );
	else
		set_color_construction( alpha_in );
}

FINLINE	void	c_bdd_curve_3d::set_color_ui_selected( REAL alpha_in )
{
	if( _b_ui_edit )
		c_color::mod->draw_red( alpha_in );
	else
		set_color_construction_selected( alpha_in );
}

void	c_bdd_curve_3d::set_point_control_cur( INT32 iu, bool b_send_command )
{
	INT32 nb = _curve->get_control_point_nb();
	if( nb > 0 )
	{
		iu = IMOD( iu, nb );
		if( _point_cur_index != iu || _b_curve_index_changed )
		{
			_point_cur_index = iu;
			_b_curve_index_changed = false;
			//_point_cur_index_ui = iu;
			if( b_send_command )
			{
				CHAR	str[1024];
				sprintf( str, "do_command( \"set_point_control_cur( %d )\" )", _point_cur_index );
				command_send( this, str );
			}
			//if we change point during interaction
			if( ui::bdd_eating_mouse == this )
				compute_mouse_start();
		}
		_point_cur_index_ui = _point_cur_index;
	}
}

/*
void	c_bdd_curve_3d::set_marker_cur( INT32 iu, bool b_send_command )
{
	if( _map_markers.get( _dataset_id )->get_nb_u() > 0 )
	{
		iu = IMOD( iu, _map_markers.get( _dataset_id )->get_nb_u() );
		if( _marker_cur != iu )
		{
			_marker_cur = iu;
			_marker_cur_ui = iu;
			if( b_send_command )
			{
				CHAR	str[1024];
				sprintf( str, "do_command( \"set_point_marker_cur( %d )\" )", _marker_cur );
				command_send( this, str );
			}
		}
	}
}
*/

REAL CONST *	c_bdd_curve_3d::get_control_point( INT32 dataset, INT32 index_u )
{
	return get_curve( dataset )->get_control_point( index_u );
}
REAL CONST *	c_bdd_curve_3d::get_control_point( INT32 index_u )
{
	return get_control_point( _dataset_id, index_u );
}


void	c_bdd_curve_3d::set_control_point( INT32 dataset, INT32 index_u, REAL CONST * CONST vec )
{
	get_curve( dataset )->set_control_point( index_u, vec );
	//_b_compute_needed = true; //hack should be only by dataset
}

void	c_bdd_curve_3d::set_control_point( INT32 index_u, REAL CONST * CONST vec )
{
	set_control_point( _dataset_id, index_u, vec );
}

/*
void	c_bdd_curve_3d::set_marker( INT32 dataset, INT32 index, REAL* vec, CHAR* text )
{
	_map_markers.get( dataset )->set_point( index, 0, vec );
	_map_markers.get( dataset )->set_text( index, 0, text );
	//_b_compute_needed = true;	//hack should be only by dataset
}

void	c_bdd_curve_3d::set_marker( INT32 index, REAL* vec, CHAR* text )
{
	set_marker( _dataset_id, index, vec, text );
}
*/

bool	c_bdd_curve_3d::do_action( c_edit::ACTION action )
{
	bool	b_return = true;
	switch ( action )
	{
	case c_edit::SET_POINT_NEXT:	set_point_control_cur( _point_cur_index + 1 );					break;
	case c_edit::SET_POINT_PREV:	set_point_control_cur( _point_cur_index - 1 );					break;
	case c_edit::SET_POINT_BEGIN:	set_point_control_cur( 0 );										break;
	case c_edit::SET_POINT_END:		set_point_control_cur( _curve->get_control_point_nb() - 1 );	break;

	case c_edit::INSERT_POINT:		insert_control_point( _point_cur_index );						break;
	case c_edit::DELETE_POINT:		delete_control_point( _point_cur_index );						break;

	//todo	todo check if it works correctly with AAASeed network remote 
	case c_edit::SET_CURVE_NEXT:	set_curve_ui( _dataset_id_ui + 1 );								break;
	case c_edit::SET_CURVE_PREV:	set_curve_ui( _dataset_id_ui - 1 );								break;
	case c_edit::SET_CURVE_BEGIN:	set_curve_ui( 1 );												break;
	case c_edit::SET_CURVE_END:		set_curve_ui( (INT32)_map_curve.size() );						break;

	//todo we expose it in the enum so we should implement it 
	case c_edit::INSERT_CURVE:		b_return = false;												break;

//	case c_edit::FLIP_LOCK:			_b_ui_lock = !_b_ui_lock;										break;
	case c_edit::CENTER_CAM:		center_cam();													break;

	default:						b_return = false;												break;
	}
	return b_return;
}

bool	c_bdd_curve_3d::do_action( o_str CONST & action_asked )
{
	c_edit::ACTION action = c_edit::find_action_from_str( action_asked );
	return do_action( action );
}

bool	c_bdd_curve_3d::enum_command( o_str& o )
{
	o.set( "set_control_point( dataset, index_u, x,y,z )" );
	o.add_newline();
	o.add( "set_point_control_cur( index_u )" );
	o.add_newline();
	c_edit::enum_command( o );
	return true;
}

bool	c_bdd_curve_3d::do_command( C_PCHAR_C cmd )
{
	bool	retcode = true;
	INT32	dataset;
	INT32	i_u;
	REAL	vec[3];
//	CHAR	buffer[1024];
//	DBG_PRINT_STRING( cmd );
	if( sscanf( cmd, "\"set_control_point( %d, %d, %f, %f, %f )", &dataset, &i_u, &vec[0], &vec[1], &vec[2] ) == 5 )
		set_control_point( dataset, i_u, vec );
	else if( sscanf( cmd, "\"set_point_control_cur( %d )", &i_u ) == 1 )
		set_point_control_cur( i_u, false );	//	avoid ping pong
/*
	if( sscanf( cmd, "\"set_marker( %d, %d, %f, %f, %f, %s )", &dataset, &i_u, &vec[0], &vec[1], &vec[2], buffer ) == 5 )
		set_marker( dataset, i_u, vec, buffer );
	else if( sscanf( cmd, "\"set_marker_cur( %d )", &i_u ) == 1 )
		set_marker_cur( i_u, false );
*/
	else
	{
		o_str& action_asked = o_str::push_name( cmd );
			retcode = do_action( action_asked );
		o_str::pop_name();
		if( !retcode ) 
			ERR_PRINT_STRING( "%s() unknown command %s", __FUNCTION__, cmd );
	}
	return retcode;
}
bool	c_bdd_curve_3d::do_key( INT32 c, bool CONST b_special, INT32* modifiers, INT32* x, INT32* y )
{
	c_edit::ACTION action = _edit.find_action_from_key( c, b_special, modifiers, x, y );
	if( action != c_edit::DO_NOTHING )
		return do_action( action );

	bool	b_return = false;
	if( is_ui_intercept() )
	{
		if( b_special )
		{
			if( modifier::is_none() )
			{
				b_return = true;
				switch ( c )
				{
				case 0:	// avoid warning
				default:				b_return = false;							break;
				}
			}
			else if( modifier::is_ctrl_on() )
			{
				b_return = true;
				switch ( c )
				{					
				case 0:	// avoid warning
				default:				b_return = false;							break;
				}
			}
			else if( modifier::is_alt_on() )
			{
				b_return = true;
				switch ( c )
				{
				//case keyboard::END:		set_marker_cur( _nb_markers[ _dataset_id]-1 );	break;
				//case keyboard::HOME:		set_marker_cur( 0 );							break;
				//case keyboard::RIGHT:
				//case keyboard::UP:		set_marker_cur( _marker_cur_ui + 1 );			break;
				//case keyboard::DOWN:
				//case keyboard::LEFT:		set_marker_cur( _marker_cur_ui - 1 );			break;
				//case keyboard::INSERT:	insert_marker();								break;
				//case keyboard::INSERT:	insert_point( _point_cur_index);						break;

				case 0:	// avoid warning
				default:
					b_return = false;
					break;
				}
			}
		}
		else
		{
			if( modifier::is_ctrl_on() )
			{
				b_return = true;
				switch ( c )
				{
				//case 'z':
				//case 'Z':
				//	if ( _point_down_dataset_id == _dataset_id_ui && _point_down_cur == _point_cur_index )
				//	{
				//		c_curve_3d*	curve = get_curve( _dataset_id_ui );
				//		if( curve->get_control_points_nb() > 0 )
				//		{
				//			curve->set_control_point( _point_cur_index, _point_mouse_down );
				//			//REAL	_point_mouse_down[3];
				//			//_point_down_dataset_id = _dataset_id_ui;
				//		}
				//	}
				//	break;
				case 0:	// avoid warning
				default:
					b_return = false;
					break;
				}
			}
			else if( modifier::is_alt_on() )
			{
			}
			else
			{
				b_return = true;
				switch ( c )
				{
				case 0:	// avoid warning
				default:
					b_return = false;
					break;
				}
			}
		}
	}
	else
	{
		if( b_special )
		{
		}
		else
		{
			if( modifier::is_alt_on() )
			{
				b_return = true;
				switch ( c )
				{
				case 0:	// avoid warning
				default:
					b_return = false;
					break;
				}
			}
			else
			{
			}
		}
	}
	return b_return;
}

INT32	c_bdd_curve_3d::get_constraint_axe()
{
	INT32 axe;
	switch( _s_move_constraint )
	{
	case MOVE_CONSTRAINT_YZ:
	case MOVE_CONSTRAINT_X:		axe = 0;	break;
	case MOVE_CONSTRAINT_XZ:
	case MOVE_CONSTRAINT_Y:		axe = 1;	break;
	case MOVE_CONSTRAINT_XY:
	case MOVE_CONSTRAINT_Z:		axe = 2;	break;
	case MOVE_CONSTRAINT_NONE:
	default:					axe = 1;	break;
	}
	return axe;
}
void	c_bdd_curve_3d::get_constraint_axe( INT32& i_u, INT32& i_v, INT32& i_axe )
{
	switch( _s_move_constraint )
	{
	case MOVE_CONSTRAINT_X:		i_u =  0;	i_v = -1;	i_axe = -1;		break;
	case MOVE_CONSTRAINT_Y:		i_u = -1;	i_v =  1;	i_axe = -1;		break;
	case MOVE_CONSTRAINT_Z:		i_u = -1;	i_v = -1;	i_axe =  2;		break;

	case MOVE_CONSTRAINT_YZ:	i_u = -1;	i_v =  1;	i_axe =  2;		break;
	case MOVE_CONSTRAINT_XZ:	i_u =  0;	i_v = -1;	i_axe =  2;		break;
	case MOVE_CONSTRAINT_XY:	i_u =  0;	i_v =  1;	i_axe = -1;		break;

	default:
	case MOVE_CONSTRAINT_NONE:	i_u =  0;	i_v =  1;	i_axe =  2;		break;
	}
}

bool	c_bdd_curve_3d::mouse_wheel( FP32 factor )
{
	// scroll wheel mode in z direction
	//GOOD_PRINT_STRING( "%s() factor = %f", __FUNCTION__, factor );
	REAL*	pt = _curve->get_control_point( _point_cur_index );
	if( pt )
	{
		REAL	vec[3];
		CHAR	str[1024];
		cpy_v3( vec, pt );
		INT32 axe = get_constraint_axe();
		//if( !_b_ui_lock )
		{
			vec[ axe ] += factor * .1f;
		}
		set_control_point( _dataset_id, _point_cur_index, vec );
		sprintf( str, "do_command( \"set_control_point( %d, %d, %f, %f, %f )\" )", _dataset_id, _point_cur_index, vec[0], vec[1], vec[2] );
		command_send( this, str );
		return true;
	}
	return false;
}


void	c_bdd_curve_3d::move_constraint( REAL* dst, REAL CONST * CONST src, REAL CONST * CONST tra )
{
	cpy_v3( dst, src );
//	if( _b_ui_lock )	return;

	INT32 sel[3];
	get_constraint_axe( sel[0], sel[1], sel[2] );
	if( sel[0]>= 0 )
		dst[sel[0]] = tra[0];
	if( sel[1]>= 0 )
		dst[sel[1]] = tra[1];
	if( sel[2]>= 0 )
		dst[sel[2]] = tra[2];
}

bool	c_bdd_curve_3d::compute_mouse_start()
{
	REAL*	pt = _curve->get_control_point( _point_cur_index );
	cpy_v3( _point_mouse_down, pt );

	//c_seedcam::get_ui();
	//c_layers::get_layers_camera_used();
	if( _cam_used )
	{
		_cam_used->get_mouse_in_world( _xyz, _point_mouse_down );
		return true;
	}

	err_print( "no camera ?" );
	return false;
}

bool	c_bdd_curve_3d::mouse_down( FP32& u_start, FP32& v_start )
{
//	if( _edit.is_ui_intercept() && !_b_ui_lock && c_seedcam::get_ui() )
	if( _edit.is_ui_intercept() && c_layers::get_layers_camera_used() )
//	if( _edit.is_ui_intercept() )	//done during AquaLoyal
	{
		//GOOD_PRINT_STRING( "%s() point %d", __FUNCTION__, _iu_cur );
		if( _curve->get_control_point_nb() > 0 )
		{
			//_point_down_dataset_id = _dataset_id;
			//_point_down_cur = _point_cur_index;
			bool	ret = compute_mouse_start();
			u_start = *_point_mouse_down;
			v_start = *( _point_mouse_down + 1 );
			return ret;
		}
	}
	return false;
}

//todomatrix after change by Maa (2023 October) this have to be tested and adapted
void	c_bdd_curve_3d::mouse_move( FP32 u_in, FP32 v_in )
{
//	if( _edit.is_ui_intercept() && !_b_ui_lock && c_seedcam::get_ui() )
	//c_seedcam* cam = c_layers::get_layers_camera_used();
	if( !_cam_used )
	{
		ERR_PRINT_STRING( "no cam ?" );
	}
	else
	{
		if( _edit.is_ui_intercept() )
		{
			if( _curve->get_control_point_nb() > 0 )
			{
				CHAR	str[1024];
				// ask Maa can we update camera? needed when stereo, or make sure that mat_proj and other are calculated in seedcam
				// todo problem when camera is stereo
				//c_seedcam*	cam = c_layers::cur->get_camera( _cam_edit_index );\
				// 				//	c_seedcam*	cam = c_seedcam::cur;
				//	c_seedcam*	cam = c_seedcam::get_ui();

				FP32	tra[3];
				_cam_used->get_mouse_in_world( tra, _point_mouse_down );
				sub_v3( tra, _xyz );

				REAL*	pt = _curve->get_control_point( _point_cur_index );
				add_v3( tra, _point_mouse_down );
				REAL	final[3];
				move_constraint( final, pt, tra );

				set_control_point( _dataset_id, _point_cur_index, final );
				sprintf( str, "do_command( \"set_control_point( %d, %d, %f, %f, %f )\" )", _dataset_id, _point_cur_index, final[0], final[1], final[2] );
				command_send( this, str );
			}
		}
	}
}

void	c_bdd_curve_3d::get_tra( REAL* dst, INT32 CONST dataset_id, REAL CONST s )
{
	c_curve_3d* curve = find_curve_valid( dataset_id );
	if( curve )
		curve->get_point( dst, s );
	else
		clear_v3( dst );
}
void	c_bdd_curve_3d::get_tangent( REAL* dst, INT32 CONST dataset_id, REAL CONST s )
{
	c_curve_3d* curve = find_curve_valid( dataset_id );
	if( curve )
		curve->get_tangent( dst, s );
	else
		clear_v3( dst );
}
void	c_bdd_curve_3d::get_point_tangent( REAL* dst, REAL* tgn, INT32 CONST dataset_id, REAL CONST s )
{
	c_curve_3d* curve = find_curve_valid( dataset_id );
	if( curve )
		curve->get_point_tangent( dst, tgn, s );
	else
	{
		clear_v3( dst );
		clear_v3( tgn );
	}
}


//ignore time
bool	c_bdd_curve_3d::get_point_rnd( REAL* CONST dst, REAL CONST t_in )
{
	//todo not perfect but decent eventually infinite loop
	//	perhaps build a list of valid curve first
	//	 and ponderate using s_len of each curve
	//	easy but require dealing better with max size than the cur CONST of 9999
	if( _b_valid_curve_for_get_point_rnd_compute )
	{
		_b_valid_curve_for_get_point_rnd_compute	= false;
		_b_valid_curve_for_get_point_rnd			= false;
		for( UINT32 i=_draw_dataset_begin; i <=_draw_dataset_end; ++i )
		{
			if( find_curve_valid( i ) )
			{
				_b_valid_curve_for_get_point_rnd	= true;
				break;
			}
		}
	}

	if( _b_valid_curve_for_get_point_rnd )
	{
		REAL fnb = 	_draw_dataset_end - _draw_dataset_begin + _draw_s_end_end - _draw_s_begin_begin;
		for(;;)
		{
			REAL f = n_bdd_curve_3d::rand.get_fp32_max(fnb);
			UINT32 id;
			REAL tmp = _draw_s_begin_end - _draw_s_begin_begin;
			if( f < tmp )
			{
				id = _draw_dataset_begin;
				if( c_curve_3d* curve = find_curve_valid( id ) )
				{
					curve->get_point( dst, f + _draw_s_begin_begin );
					break;
				}
			}
			else
			{
				f -= tmp;
				INT32 nb = I_FLOOR( fnb - tmp );
				if( f < nb )
				{
					nb = I_FLOOR( f );
					UINT32 id = _draw_dataset_begin + nb + 1;
					if( c_curve_3d* curve = find_curve_valid( id ) )
					{
						curve->get_point( dst, f-nb );
						break;
					}
				}
				else
				{
					UINT32 id = _draw_dataset_end;
					if( c_curve_3d* curve = find_curve_valid( id ) )
					{
						curve->get_point( dst, _draw_s_end_begin + f-nb );
						break;
					}
				}
			}
		}
		return true;
	}

	clear_v3( dst );
	return false;
}

REAL	c_bdd_curve_3d::get_len( INT32 dataset_id )
{
	if( c_curve_3d* curve = find_curve_valid( dataset_id ) )
		return REAL(curve->get_len());
	return REAL(0);
}

//todo compute several points
FINLINE void	c_bdd_curve_3d::coor_to_world_one_low( REAL* dst, REAL CONST * CONST src, c_curve_3d* CONST curve )
{
	//todoopt this is here so if src and dst are equal we are still ok but this is not optimal deformer should handle
//	REAL	s = src[0];
//	REAL	y = src[1];
//	REAL	t = src[2];

	REAL	tgn[3], axe_z[3], axe_y[3];

	//find point and tangent
	curve->get_point_tangent( dst, tgn, src[0] );

	if( _b_coor_to_world_use_z_ui )
	{
		build_normal_vectors_using_z_v3r( tgn, axe_y, axe_z );
		add_scale_v3( dst, axe_z,  -src[1]	);	//	minus to reverse the sign
		add_scale_v3( dst, axe_y,  src[2]	);	//	dst[1] += y;
	}
	else
	{
		build_normal_vectors_v3r( tgn, axe_y, axe_z );
		add_scale_v3( dst, axe_z, -src[2]	);	//	minus to reverse the sign
		add_scale_v3( dst, axe_y, src[1]	);	//	dst[1] += y;
	}

}

void	c_bdd_curve_3d::coor_to_world_one( REAL* dst, INT32 CONST dataset_id, REAL CONST * CONST src )
{
	if( c_curve_3d* curve = find_curve_valid( dataset_id ) )
		coor_to_world_one_low( dst, src, curve );
}

void	c_bdd_curve_3d::coor_to_world( REAL* dst, INT32 CONST dataset_id, REAL CONST * src, INT32 nb )
{
	if( c_curve_3d* curve = find_curve_valid( dataset_id ) )
	{
		for( ; nb>0; --nb )
		{
			coor_to_world_one_low( dst, src, curve );
			dst += 3;
			src += 3;
		}
	}
}
