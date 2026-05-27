#include "bdd_stroke.h"
#include "obj_ui/tracker/wacom/wacom.h"
#include "draw/render.h"
#include "draw/model.h"
#include "draw/line.h"
#include "draw/color.h"
#include "gol/gol_matrix.h"
#include "gol/gol.h"
#include "system/win32/SystemKeyboard.h"
#include "draw/rect.h"
#include "ui/keyboard.h"
#include "spy.h"


FACTORY_CREATE_PROP_V1( c_bdd_stroke, bdd_stroke, Strokes, bdd_stroke, sub_menu="Point"; );

//c_instance_by_channel< c_bdd_stroke, c_bdd_stroke::MOCAP_CHANNEL_NB_MAX >	c_bdd_stroke::inst_by_channel;

namespace	n_bdd_stroke
{
	INT32 CONST	DRAW_METH0D_NB =	3;
	static	C_PCHAR_C	draw_method[c_bdd_stroke::DRAW_METHODE_MAX_NB] =
	{
		"All",
		"by_stroke",
		"by_time",
	};

	CONSTEXPR UINT32 BASE_PARAM_NB		=	c_bdd::GEO_PARAM_NB + 3;
	CONSTEXPR UINT32 INFO_PARAM_NB		=	4;
	CONSTEXPR UINT32 DRAW_PARAM_NB		=	16;
	CONSTEXPR UINT32 ERASE_PARAM_NB	=	7;
	CONSTEXPR UINT32 PLAY_PARAM_NB		=	3;
	CONSTEXPR UINT32 ORIGIN_PARAM_NB	=	4;
	CONSTEXPR UINT32 UI_PARAM_NB		=	2;
//	CONSTEXPR UINT32 RENDER_PARAM_NB	=	15;
//	CONSTEXPR UINT32 NET_PARAM_NB		=	2;
	CONSTEXPR UINT32 GROUP_PARAM_NB	=	6;
	CONSTEXPR UINT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	DRAW_PARAM_NB
									+	INFO_PARAM_NB
									+	ERASE_PARAM_NB
									+	PLAY_PARAM_NB
									+	ORIGIN_PARAM_NB
									+	UI_PARAM_NB
//									+	RENDER_PARAM_NB
//									+	NET_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_BASE_PARAMS

		PARAM_DEF_DIRNAME(		dataset_dir				)
		PARAM_DEF_INT32(		dataset_id,				2, 1,			1, c_bdd_stroke::DATASET_ID_MAX )
		PARAM_DEF_BOOL_OFF(		dataset_save_trig		)

		PARAM_DEF_GROUP(			Draw,		DRAW_PARAM_NB )
			PARAM_DEF_BOOL_OFF(			draw_move				)
			PARAM_DEF_BOOL_OFF(			draw_crosshair			)
			PARAM_DEF_BOOL_OFF(			draw_rectangle			)
//			PARAM_DEF_BOOL_ON(			draw_dataset_empty		)
			PARAM_DEF_FP32_POS(			primitive_size_min,		1, .5	)
			PARAM_DEF_FP32_POS(			primitive_size_max,		2, 10	)
			PARAM_DEF_FP32(				primitive_size_factor,	.1, 1.,	0.0001, PARAM_MAX_FP32 )
			PARAM_DEF_COLOR_RGBGA(		move_color				)
			PARAM_DEF_SYMBO_PSTR_ZERO(	draw_method,			n_bdd_stroke::draw_method )
			PARAM_DEF_REAL_ONE_ZERO(	draw_phase_begin		)	
			PARAM_DEF_REAL_ZERO_ONE(	draw_phase_end			)
			PARAM_DEF_INT32_LOCKED(		draw_stroke_begin		)	
			PARAM_DEF_INT32_LOCKED(		draw_stroke_end			)

		PARAM_DEF_GROUP(			Info,		INFO_PARAM_NB )
//			PARAM_DEF_INT32(			channel_id,					2, 1,		1, c_bdd_mocap::MOCAP_CHANNEL_NB_MAX )
	//no param_lock but be careful see param_init_pt
			PARAM_DEF_BOOL_LOCKED(		stroking				)
			PARAM_DEF_INT32_LOCKED(		stroke_nb				)
			PARAM_DEF_INT32_LOCKED(		point_nb				)
			PARAM_DEF_DOUBLE_LOCKED(	duration				)

		PARAM_DEF_GROUP(			Play,		PLAY_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		record_asked			)
			PARAM_DEF_BOOL_LOCKED(	record					)
			PARAM_DEF_BOOL_OFF(		restart_trig			)

//			PARAM_DEF_BOOL_OFF(		play_asked				)
//			PARAM_DEF_REAL_ONE(		time_factor				)
//			PARAM_DEF_REAL_LOCKED(	time_len				)
//			PARAM_DEF_REAL_ZERO(	phase					)
//			PARAM_DEF_BOOL_ON(		loop					)

		PARAM_DEF_GROUP(			Erase,		ERASE_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		erase_last_trig				)
			PARAM_DEF_BOOL_OFF(		erase_drawn_last_trig		)
			PARAM_DEF_BOOL_OFF(		erase_move_all_trig			)
			PARAM_DEF_BOOL_OFF(		erase_all_trig				)
			PARAM_DEF_BOOL_OFF(		erase_before				)
			PARAM_DEF_BOOL_OFF(		erase_drawn					)
			PARAM_DEF_BOOL_OFF(		erase_after					)

		PARAM_DEF_GROUP_CLOSED(	Origin,		ORIGIN_PARAM_NB )
			PARAM_DEF_POINT_UVA(	origin					)
			PARAM_DEF_BOOL_OFF(		center_strokes_trig		)	

		PARAM_DEF_GROUP_CLOSED(	ui,			UI_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		verbose			)
			PARAM_DEF_BOOL_OFF(		ui_intercept	)

	//todoqq	switch to render instead of draw
//		PARAM_DEF_GROUP( render, RENDER_PARAM_NB )
		
//		PARAM_DEF_GROUP_CLOSED( Net, NET_PARAM_NB )
//			PARAM_DEF_BOOL_OFF(		net_send					)
//			PARAM_DEF_SYMBO_ZERO(	net_channel,				1, 0,		0, c_net::CHANNEL_NB, gstr::no )
	};
}

//todo	should add something to the base class not to save the file values

void	c_bdd_stroke::param_init_pt()
{
	INT32	h = param_init_pt_geo();

	param_set_pt( h, _dataset_dir			);
	param_set_pt( h, _dataset_id_ui			);
	param_set_pt( h, _b_dataset_save_trig	);

	++h;
		param_set_pt( h, _b_draw_deplacement	);
		param_set_pt( h, _b_draw_crosshair		);
		param_set_pt( h, _b_draw_rect			);
//		param_set_pt( h, _b_dataset_empty		);
		param_set_pt( h, _prim_size_min_ui		);
		param_set_pt( h, _prim_size_max_ui		);
		param_set_pt( h, _prim_size_factor_ui	);
		param_set_pt_rgbfa( h, _color_move_ui	);
		param_set_pt( h, _s_draw_method			);
		param_set_pt( h, _draw_phase_begin		);
		param_set_pt( h, _draw_phase_end		);
		param_set_pt( h, _draw_stroke_begin		);
		param_set_pt( h, _draw_stroke_end		);

	++h;
//		param_set_pt( h, _channel_id			);
		param_set_pt( h, _b_stroking			);
		param_set_pt( h, _stroke_nb				);
		param_set_pt( h, _point_nb				);
		param_set_pt( h, _duration				);

	++h;
		param_set_pt( h, _b_record_ui			);
		param_set_pt( h, _b_record				);
		param_set_pt( h, _b_restart_trig_ui		);

	++h;
		param_set_pt( h, _b_erase_last_trig_ui			);
		param_set_pt( h, _b_erase_drawn_last_trig_ui	);
		param_set_pt( h, _b_erase_move_all_trig			);
		param_set_pt( h, _b_erase_all_trig				);
		param_set_pt( h, _b_erase_before_trig			);
		param_set_pt( h, _b_erase_drawn_trig			);
		param_set_pt( h, _b_erase_after_trig			);
//		param_set_pt( h, _b_play_ui				);
//		param_set_pt( h, _b_loop_ui				);
//		param_set_pt( h, _time_factor_ui		);
//		param_set_pt( h, _time_len				);
//		param_set_pt( h, _phase					);

	++h;
		param_set_pt_3(	h,	_origin_ui				);
		param_set_pt(		h,	_b_center_strokes_trig	);

	++h;
		param_set_pt( h, _b_verbose_ui			);
		param_set_pt( h, _b_ui_intercept_ui		);

//	++h;
//		param_set_pt( h, _b_net_send_ui			);
//		param_set_pt( h, _net_channel			);

	err_param_init_pt(h);
}

void	c_bdd_stroke::param_init()
{
}

void	c_bdd_stroke::init()
{
	_dataset_id_ui = 1;
//	set_dataset( _dataset_id_ui );
}

void	 c_bdd_stroke::dealloc()
{
}

CONSTRUCTOR_CREATE(c_bdd_stroke)
	,_stroke_nb		(0)
	,_point_nb		(0)
	,_duration		(0)
	,_b_record		(false)
	,_strokes_cur	(nullptr)
	,_datasets		(-42)
	,_dataset_id	(-42)
	,_b_move		(false)
{
	init();
	param_init_with( n_bdd_stroke::param, n_bdd_stroke::PARAM_NB_MAX ); //  bdd_mocap_param, BDD_MOCAP_PARAM_NB);

	erase_strokes_all();
}

c_bdd_stroke::~c_bdd_stroke()
{
//	if( cur == this )
//		cur = def;
//	if( ui == this )
//		ui = def;
	dealloc();
}

void	c_bdd_stroke::erase_strokes_all()
{

}

void	c_bdd_stroke::erase_stroke_last()
{
	if( _strokes_cur )
	{
		_strokes_cur->erase_last();
		//hack
		_b_contact = false;
	}
}

void	c_bdd_stroke::erase_stroke_drawn_last()
{
	if( _strokes_cur )
		_strokes_cur->erase_drawn_last();
}

void	c_bdd_stroke::erase_stroke_move_all()
{
	if( _strokes_cur )
		_strokes_cur->erase_move_all();
}

void	c_bdd_stroke::restart()
{
	erase_strokes_all();
	_b_restart_trig_ui = false;
}

void	c_bdd_stroke::set_dataset( INT32 dataset_id )
{
	dataset_id = WRAP_ID( dataset_id, DATASET_ID_MAX );
	if( _dataset_id != dataset_id )
	{
		_dataset_id = dataset_id;
		_dataset_id_ui = dataset_id;
		_strokes_cur = _datasets.find( _dataset_id );			
		_b_stroking = false;
		_b_contact = false;
		_draw_stroke_begin = 1;
		_draw_stroke_end = 0;
	}
}

void	c_bdd_stroke::update_info()
{
	if( _strokes_cur )
	{
		auto & strokes = _strokes_cur->_strokes;
		INT32 nb = INT32(strokes.size());
		_stroke_nb = nb;
		_draw_stroke_begin = INT32( nb * _draw_phase_begin	);
		_draw_stroke_end   = INT32( nb * _draw_phase_end	);
		_point_nb = _strokes_cur->_point_nb;
		_duration = _strokes_cur->_msec_duration / 1000.;
	}
	else
	{
		_stroke_nb = 0;
		_point_nb = 0;
		_duration = 0.;
	}
}

//todo manage to cut the memmove when not needed
void	c_bdd_stroke::update()
{
//	std::lock_guard<c_bdd_stroke> guard(*this);

	set_dataset( _dataset_id_ui );

	if( _b_ui_intercept_ui )
		ui_register( this );

//	inst_by_channel.set( _channel_id, this );
// 	c_bdd_stroke::cur = this;

//	load_data();
//	if( _frame_nb_ui != _frame_nb || _node_nb_ui != _node_nb )
//	{
//		dealloc();
//		alloc( _frame_nb_ui, _node_nb_ui );
//	}

	_b_record	= _b_record_ui;
//	_b_play		= _b_play_ui;

	_b_stroking = false;
	
	if( _b_record && g_wacom )
	{
		auto p_new = g_wacom->get_stroke_out();
		INT32 size = p_new->size();
		INT32 index = 0;
	
		if( size > 0 )
		{
			if( modifier::is_ctrl_on() )
			{
				if( _b_move )
				{
					REAL d[3];
					d[2] = 0.;
					sub_v2( d, g_wacom->get_pos(), _pos_store );
					add_v3( _origin_ui, _origin_store, d );
				}
				else
				{
					_b_move = true;
					cpy_v3( _origin_store, _origin_ui );
					cpy_v2( _pos_store, g_wacom->get_pos() );
				}
				_b_stroking = false;
				_b_contact = false;
			}
			else
			{
				_b_move = false;
				if( !_strokes_cur )
				{
					_strokes_cur = _datasets.get(_dataset_id);
					_strokes_cur->_strokes.push_back( new c_stroke_tablet );
				}
				auto & strokes = _strokes_cur->_strokes;

		//		_stroke_all.add_raw( *p_new );
				c_stroke_tablet* ps = strokes.back();

				for( auto const & elt : p_new->get_data() )
				{
					bool b_contact = elt._pos[2] == 0;
					if( _b_contact != b_contact )
					{
						//close current stroke

						 _b_contact = b_contact;

						//create a new one
						ps = new c_stroke_tablet;
						ps->_b_contact = b_contact;
					 
						strokes.push_back( ps );
					}
					ps->add( elt, _origin_ui );
				}
				_point_nb = _strokes_cur->_point_nb + size;
				_strokes_cur->_point_nb = _point_nb;
				_b_stroking = true;
			}
		}
	}

	if( _b_erase_all_trig )
	{
		erase_strokes_all();
		_b_erase_all_trig = false;				
	}
	if( _b_erase_last_trig_ui )
	{
		erase_stroke_last();		
		_b_erase_last_trig_ui = false;			
	}
	if( _b_erase_drawn_last_trig_ui )
	{	
		erase_stroke_drawn_last();	
		_b_erase_drawn_last_trig_ui = false;	
	}
	if( _b_erase_move_all_trig )		
	{	
		erase_stroke_move_all();	
		_b_erase_move_all_trig = false;
	}

	update_info();

	if( _b_erase_before_trig )
	{
		if( _strokes_cur )
		{
			_strokes_cur->erase( 0, _draw_stroke_begin-1 );
			update_info();
		}
		_b_erase_before_trig = false;
	}
	if( _b_erase_drawn_trig )
	{
		if( _strokes_cur )
		{
			_strokes_cur->erase( _draw_stroke_begin, _draw_stroke_end-1 );
			update_info();
		}
		_b_erase_drawn_trig = false;
	}
	if( _b_erase_after_trig )
	{
		if( _strokes_cur )
		{
			_strokes_cur->erase( _draw_stroke_end, _stroke_nb-1 );
			update_info();
		}
		_b_erase_after_trig = false;
	}

	if( _b_center_strokes_trig )
	{
		if( _strokes_cur )
		{
			REAL vec[3];
			cpy_v2( vec, _origin_ui );
			vec[2] = 0.;
			_strokes_cur->offset( _origin_ui );
			clear_v2( _origin_ui );
		}
		_b_center_strokes_trig = false;
	}
	
	if( _b_dataset_save_trig )
	{
		save_dataset_current();
		_b_dataset_save_trig = false;
	}

	_prim_size_min		= _prim_size_factor_ui * _prim_size_min_ui;
	_prim_size_factor	= _prim_size_factor_ui * (_prim_size_max_ui - _prim_size_min_ui);
//	_phase_last = _phase;
//	mem::is_all_ok( "mocap::update() at end" );
}

/*
//todo ?	this is not scaled, update strategy ?
REAL	c_bdd_mocap::compute_len( INT32 node_id )
{
	REAL	len = 0;
	if( _b_valid_data && is_valid_node( node_id ) )
	{
		c_seq_mocap*	seq = &_seq_array[0];	//	store on seq 0 &_seq_array[node_id-1];
												//todo redeal with this it made code unclear
		REAL*	cur = get_data( 0, node_id ) + _offset_tra;
		REAL*	next;
		for( INT32 i=1; i<seq->_frame_nb_rel-1; ++i )
		{
			next = get_data( i, node_id ) + _offset_tra;
			len += dist_v3r( cur, next );
			cur = next;
		}
		next = get_data( 0 ) + _offset_tra;
		len += dist_v3r( cur, next );
	}
	return len;
}

REAL	c_bdd_mocap::get_len( INT32 node_id )
{
	if( _b_valid_data )
	{
		INT32	node_index	= CLAMP( node_id, 1, _node_nb ) - 1;	//todo check
		return	_len_by_node[node_index];
	}
	return 0.;
}
*/


void	c_bdd_stroke::draw_single()
{
//	if( !_b_valid_data ) return;

	REAL size[3];
	c_model::cur->get_size_v3( size );

	FP32	color_move[4];
	c_color::mod->process_rgbfa( color_move, _color_move_ui );

	INT32 pol = c_render::get_cur()->get_draw_primitive();

	GOL::matrix::push();
		GOL::matrix::scale3v( size );

		if( _strokes_cur )
		{
			GOL::matrix::translate3v( _origin_ui );
	
				auto & strokes = _strokes_cur->_strokes;
				INT32 nb = INT32(strokes.size());
				INT32 ib, ie;
				if( _s_draw_method==DRAW_ALL )
				{
					ib = 0;
					ie = nb;
				}
				else
				{
					ib = _draw_stroke_begin;
					ie = _draw_stroke_end;
				}

				for( INT32 i=ib; i<ie; ++i )
				{
					auto const stroke = strokes[i];
					FP32 prim_size = _prim_size_min + _prim_size_factor * stroke->_pressure_max;
					if( pol == GL_POINTS )	GOL::set_point_size( prim_size );
					else					GOL::set_line_width( prim_size );

					if( stroke->_b_contact )
					{
						c_color::get_cur()->draw();
						stroke->draw( pol );
					}
					else
					{
						if( _b_draw_deplacement )
						{
							GOL::color4v( color_move );
							stroke->draw( pol );
						}
					}
				}

				GOL::matrix::translate_negv( _origin_ui );
		}

		c_render* ren = c_render::get_cur();
		DOUBLE CONST * p_fori = g_wacom->get_orientation_factor();
		ren->begin_top();

			GOL::color_cyan();
			GOL::set_line_width( 1. );

			if( _b_stroking && _b_draw_crosshair )
			{
				REAL CONST * pos = g_wacom->get_pos();
				REAL a[3];
				REAL b[3];
				a[2] = b[2] = REAL(0);

				b[0] = pos[0];
				a[0] = b[0];
				a[1] = REAL( p_fori[1] * .5 );
				b[1] = -a[1];
				draw_line(	a,b );

				b[1] = pos[1];
				a[1] = b[1];
				a[0] = REAL( p_fori[0] * .5);
				b[0] = -a[0]; 
				draw_line(	a,b );
			}

			if( _b_draw_rect )
				draw_rect_line( -REAL(p_fori[0] * .5), -REAL(p_fori[1] * .5), REAL(p_fori[0] * .5), REAL(p_fori[1] * .5) );

		ren->end_top();
	GOL::matrix::pop();
}

void	c_bdd_stroke::draw_multiple()
{
}

void 	c_bdd_stroke::make_filename( o_str& filename_dst, o_str CONST & filename_in, INT32 dataset_id, INT32 zero_nb )
{
	if( _dataset_dir.is_empty() )
	{
		filename_dst.set_fname_relative( filename_in );
		filename_dst.drop_fname();
	}
	else
	{
		filename_dst.set( _dataset_dir );
		if( !filename_dst.is_ending_with('/') && !filename_dst.is_ending_with('\\') )
			filename_dst.add_slash();
	}
	filename_dst.add( "dataset_0000/stroke_0000" );
	filename_dst.set_digits( -16, 4, dataset_id );
	if( zero_nb==6 )
		filename_dst.add( "00"	);
	filename_dst.add( ".csv" );
}

o_str& 	c_bdd_stroke::do_before_load_save( o_str CONST & filename_in, INT32 dataset_id, INT32 zero_nb )
{
	c_file::push_vfile();
	o_str& filename = o_str::push_name();
	make_filename( filename, filename_in, dataset_id, zero_nb );
	return filename;
}

void	c_bdd_stroke::do_after_load_save()
{
	o_str::pop_name();
	c_file::pop_vfile();
}

namespace
{
	FINLINE void set_file_digits( o_str& filename, INT32 i, INT32 zero_nb )
	{
		filename.set_digits( -4-zero_nb, zero_nb, i );
	}
};

AAA_ERR	c_bdd_stroke::save_dataset( o_str CONST & filename_in, INT32 dataset_id )
{
	auto dataset = _datasets.find(dataset_id);
	if( dataset )
	{
		DBG_PRINT_STRING( "%s() save dataset %d", __FUNCTION__, dataset_id );
		SPY_PUSH_RANGE2( "save_dataset", spy::FILE_HIGH, filename_in );
			dataset->process_time();
			auto& strokes = dataset->_strokes;	

			o_str& filename = do_before_load_save( filename_in, dataset_id, 6 );
				INT32 i=1;
				for( auto const p : strokes )
				{
					set_file_digits( filename, i, 6 );
					p->save_to_file( filename );
					++i;
				}
				for( ; i<=999999; ++i )
				{
					set_file_digits( filename, i, 6 );
					if( c_file::is_exist_real_file( filename ) )
						c_file::FREMOVE( filename );
					else
						break;
				}
			do_after_load_save();

			filename = do_before_load_save( filename_in, dataset_id, 4 );
				for( i=1; i<=9999; ++i )
				{
					set_file_digits( filename, i, 4 );
					if( c_file::is_exist_real_file( filename ) )
						c_file::FREMOVE( filename );
					else
						break;
				}
			do_after_load_save();
		SPY_POP_RANGE2();

		DBG_PRINT_STRING( "   dataset %d saved", dataset_id );
	}
	return	AAA_OK;
}

void c_bdd_stroke::save_dataset_current()
{
	save_dataset( _filename_store, _dataset_id );
}

/*
AAA_ERR	c_bdd_stroke::save_do_after( o_str CONST & filename_in )
{
	for( INT32 i=1; i<=DATASET_ID_MAX; ++i )
		save_dataset( filename_in, i );
	return	AAA_OK;
}
*/

AAA_ERR	c_bdd_stroke::load_dataset( o_str CONST & filename_in, INT32 dataset_id, INT32 zero_nb )
{
	AAA_ERR retcode = ERR_FILE_NO;
	o_str& filename = do_before_load_save( filename_in, dataset_id, zero_nb );

		set_file_digits( filename, 1, zero_nb );
		if( c_file::is_exist_real_file( filename ) )
		{
			DBG_PRINT_STRING( "%s() read dataset %d : %s", __FUNCTION__, dataset_id, filename.get() );
			SPY_PUSH_RANGE2( "load_dataset", spy::FILE_HIGH, filename );
				auto dataset = _datasets.get(dataset_id);
				auto & strokes = dataset->_strokes;
				dataset->_point_nb = 0;
				INT32 iter_nb = (zero_nb == 4) ? 9999 : 999999;
				for( INT32 i=1; i<=iter_nb; ++i )
				{
					set_file_digits( filename, i, zero_nb );
					//if( c_file::is_exist_real_file( filename ) )	// too slow
					{
						auto ps = new c_stroke_tablet;
						if( ERR( ps->load_from_file( filename ) ) )
						{
							delete ps;
							break;
						}
						//ps->_b_contact = ps->have_pressure(); //in load_from_file now
						strokes.push_back( ps );
						dataset->_point_nb += ps->size();
					}
				}
				dataset->process_time();
			SPY_POP_RANGE2();
			DBG_PRINT_STRING( "\tdataset %d has %d strokes, %d points", dataset_id, strokes.size(), dataset->_point_nb );
			retcode = AAA_OK;
		}
	
	do_after_load_save();
	return retcode;
}

AAA_ERR	c_bdd_stroke::load_do_after( o_str CONST & filename_in )
{
	_filename_store.set(filename_in);
	for( INT32 i=1; i<=DATASET_ID_MAX; ++i )
	{
		if( ERR(load_dataset( filename_in, i, 6 )) )
			load_dataset( filename_in, i, 4 ); 
	}
	return	AAA_OK;
}

//
//	UI
//
bool	c_bdd_stroke::is_mouse_use()
{
	return false;
}

bool	c_bdd_stroke::do_key( INT32 c, bool CONST b_special, INT32* modifiers, INT32* x, INT32* y )
{
	if( !_b_ui_intercept_ui )
		return false;

	bool b_return = false;

	if( _b_verbose_ui )
		DBG_PRINT_STRING( "%s() key %d", __FUNCTION__, c );

	if( modifier::is_ctrl_on(*modifiers) )
	{
		b_return = true;
		if( b_special )
			b_return = false;
		else
		{
			//if( modifier::is_ctrl_on( *modifiers ) )
			switch (c)
			{
			//case keyboard::SPACE:	break;
			case keyboard::BACKSPACE:	_b_erase_all_trig = true;	break;
			default:					b_return = false;			break;
			}
		}
	}
	else if( modifier::is_none(*modifiers) )
	{
		b_return = true;
		if( b_special )
		{
			switch (c)
			{
			//case keyboard::SPACE:	break;
			case 274:	set_dataset( _dataset_id_ui-1 );	break;
			case 275:	set_dataset( _dataset_id_ui+1 );	break;
			default:	b_return = false;					break;
			}
		}
		else
		{
			//if( modifier::is_ctrl_on( *modifiers ) )
			switch (c)
			{
			//case keyboard::SPACE:	break;
			case keyboard::BACKSPACE:	_b_erase_drawn_last_trig_ui = true; break;
			case 127:					_b_erase_all_trig = true;			break;
			case 's': case 'S':			save_dataset_current();				break;
			case 'c': case 'C':			clear_v3( _origin_ui );				break;
//			case 'm': case 'M':			DBG_PRINT_STRING( "move" );			break;
//			case 'f': case 'F':			DBG_PRINT_STRING( "flip" );			break;
			default:					b_return = false;					break;
			}
		}
	}
	return b_return;
}


bool	c_bdd_stroke::mouse_down( FP32& u_start, FP32& v_start )
{
/*
	switch( _s_visu_curve )
	{
	case CURVE_2D :		_p_control_cur->mouse_get_start_uv( u_start, v_start );			break;
	case CURVE_1D :
	default :			u_start = _p_control_cur->get_key_phase();
						v_start = _p_control_cur->get_key_value( _s_visu_axe );
						break;
	}
*/
	return false;
}

void	c_bdd_stroke::mouse_move( FP32 u_in, FP32 v_in )
{
/*
	switch( _s_visu_curve )
	{
	case CURVE_2D :
		if( _b_grid_lock_u && _grid_lock_u != 0. )	u_in = ROUND_FLOOR( u_in, _grid_lock_u );
		if( !_b_ui_lock_u )							_p_control_cur->set_key_value( u_in, 0 );
		if( _b_grid_lock_v && _grid_lock_v != 0. )	v_in = ROUND_FLOOR( v_in, _grid_lock_v );
		if( !_b_ui_lock_v )							_p_control_cur->set_key_value( v_in, 1 );
		break;
	case CURVE_3D :
	case CURVE_1D :
		if( _b_grid_lock_u && _grid_lock_u != 0. )	u_in = ROUND_FLOOR( u_in, _grid_lock_u );
		if( !_b_ui_lock_u )							_p_control_cur->set_key_phase( u_in );
		if( _b_grid_lock_v && _grid_lock_v != 0. )	v_in = ROUND_FLOOR( v_in, _grid_lock_v );
		if( !_b_ui_lock_v )							_p_control_cur->set_key_value( v_in, _s_visu_axe );
		break;
	}
*/
}

void	c_bdd_stroke::mouse_up( FP32 u_in, FP32 v_in )
{
//	_p_control_cur->mouse_up();
}

