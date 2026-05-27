#include "bdd_line_3d.h"
#include "math/rand.h"
#include "obj_ui/com/net_link.h"
#include "spy.h"
#include "draw/primitive.h"

namespace {
	CONSTEXPR INT32 BLK_SIZE_MAX = 1500;	// c_net::BLK_SIZE_MAX;
	UINT8 buf_send[BLK_SIZE_MAX]; //todo adjust to real net data
	CONSTEXPR INT32 BLK_HEADER_SIZE = 16;
	CONSTEXPR INT32 BLK_END_HEADER_SIZE = 0;
	CONSTEXPR INT32 BLK_SECURITY_SIZE = 128;
	CONSTEXPR INT32 BLK_OVERHEAD_SIZE = BLK_HEADER_SIZE + BLK_END_HEADER_SIZE + BLK_SECURITY_SIZE;
	CONSTEXPR INT32 BLK_DATA_SIZE_MAX = BLK_SIZE_MAX - BLK_OVERHEAD_SIZE;

	CONSTEXPR UINT32 BLK_VERSION	= 1;
	CONSTEXPR UINT32 BLK_SIGNATURE = 0x004200 | (BLK_VERSION << 24);

	enum BLK_OP : UINT32
	{
		OP_INVALID		= 0,
		OP_PUSH			= 1,
		OP_CLEAR		= 2,
//		OP_ERASE		= 2,	//todo
//		OP_END			= 2,
//		IMG_END_BLK		= 5

	};
	enum BLK_COMP : UINT32
	{
		COMP_INVALID	= 0,
		COMP_NO			= 1,
	};
	FINLINE INT32	MAKE_CODE(						BLK_OP op,	BLK_COMP comp )	
	{
		return BLK_SIGNATURE | (op << 16) | comp;
	}
	FINLINE void	GET_OP_COMP(	UINT32 data,	BLK_OP& op,	BLK_COMP& comp )
	{
		if( (data & 0xff00ff00) == BLK_SIGNATURE )
		{	//todo more check here
			op		= BLK_OP((data >> 16) & 0xff);
			comp	= BLK_COMP(data & 0xff);
		}
		else
		{
			op		= OP_INVALID;
			comp	= COMP_INVALID;
		}
	}
	CONSTEXPR	INT32	POINT_NET_SIZE = sizeof(c_bdd_line_3d::st_point_send);
	CONSTEXPR	INT32	make_point_net_nb_max( INT32 size )
	{
		return (size - BLK_OVERHEAD_SIZE) / POINT_NET_SIZE;
	}
	CONSTEXPR	INT32	POINT_NB_MAX_TO_SEND = make_point_net_nb_max( BLK_SIZE_MAX );
}



FACTORY_CREATE_V1( c_bdd_line_3d_master, bdd_line_3d_master, Bdd Line Master, bdd_line_3d_master );

namespace n_bdd_line_3d_master
{
	CONSTEXPR INT32 BASE_PARAM_NB	= 0;
	CONSTEXPR INT32 NET_PARAM_NB	= 9;
	CONSTEXPR INT32 GROUP_PARAM_NB	= 0;
				    
	CONSTEXPR INT32 PARAM_NB	=	BASE_PARAM_NB
								+	NET_PARAM_NB
								+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB] =
	{
	//	PARAM_DEF_GROUP( Net, NET_PARAM_NB )
		PARAM_DEF_BOOL_ON(				net_in_active				)
		PARAM_DEF_BOOL_OFF(				net_in_update_continuous	)
		PARAM_DEF_BOOL_ON(				net_out_active				)
		PARAM_DEF_INT32(				net_out_blk_size_max,		1024*16, 1024*32,	1024, c_net::BLK_SIZE_MAX )
		PARAM_DEF_INT32_ZERO_SAVE_NOT(	blk_nb_out					)	
		PARAM_DEF_INT32_ZERO_SAVE_NOT(	blk_nb_in					)
		PARAM_DEF_INT32_ZERO_SAVE_NOT(	blk_nb_in_processed			)
		PARAM_DEF_INT32_ZERO_SAVE_NOT(	blk_nb_in_error				)
		PARAM_DEF_BOOL_OFF(				verbose_receive_incoherent	)
	};

}

CONSTRUCTOR_CREATE( c_bdd_line_3d_master )
{
	param_init_with( n_bdd_line_3d_master::param, n_bdd_line_3d_master::PARAM_NB );
}


void	c_bdd_line_3d_master::param_init_pt()
{
	INT32	h = 0;

//	++h;
		param_set_pt( h, _b_net_in_active				);
		param_set_pt( h, _b_net_in_update_continuous	);
		param_set_pt( h, _b_net_out_active				);
		param_set_pt( h, _net_out_blk_size_max			);
		param_set_pt( h, _blk_nb_out					);
		param_set_pt( h, _blk_nb_in						);
		param_set_pt( h, _blk_nb_in_processed			);
		param_set_pt( h, _blk_nb_in_error				);
		param_set_pt( h, _b_verbose_receive_incoherent	);
		
	err_param_init_pt( h );
}

void	c_bdd_line_3d_master::update()
{
	_blk_nb_in_error = _blk_nb_in - _blk_nb_in_processed;
}

EMPTY_DESTRUCTOR( c_bdd_line_3d_master )

c_bdd_line_3d_master*	c_bdd_line_3d::master = nullptr;



FACTORY_CREATE_PROP_V1( c_bdd_line_3d, bdd_line_3d, Lines 3d, bdd_lines_3d, sub_menu="Line"; );

c_instance_by_channel< c_bdd_line_3d, c_bdd_line_3d::LINE_3D_CHANNEL_NB_MAX >	c_bdd_line_3d::inst_by_channel;


namespace	n_bdd_line_3d
{
	C_PCHAR_C	str_channel_dst[] = { "Same" };

	c_rand_lin	rand;

	CONSTEXPR INT32	BASE_PARAM_NB	=	4 + c_bdd::GEO_PARAM_NB;
	CONSTEXPR INT32	DRAW_PARAM_NB	=	8;
	CONSTEXPR INT32	NET_PARAM_NB	=	8;
	CONSTEXPR INT32	GROUP_NB		=	2;
	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
									+	DRAW_PARAM_NB
									+	NET_PARAM_NB
									+	GROUP_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_BASE_PARAMS

		PARAM_DEF_INT32(	channel_id,			2, 1,		1, c_bdd_line_3d::LINE_3D_CHANNEL_NB_MAX )
//		PARAM_DEF_BOOL_ON(	curve_load_save		)
		PARAM_DEF_BOOL_OFF(	erase_all			)
//		PARAM_DEF_BOOL_OFF(	len_max_active		)
//		PARAM_DEF_REAL_POS_ONE(	len_max				)

		PARAM_DEF_INT32(	dataset_id,			2, 1,		1, c_bdd_line_3d::DATASET_ID_MAX )

		PARAM_DEF_GROUP(	Draw,	DRAW_PARAM_NB )
			PARAM_DEF_BOOL_OFF(			draw_all_curves		)
			PARAM_DEF_BOOL_OFF(			draw_selected		)
			PARAM_DEF_UINT32(			draw_dataset_begin,	1, 1,									1, c_bdd_line_3d::DATASET_ID_MAX	)
			PARAM_DEF_UINT32(			draw_dataset_end,	1, c_bdd_line_3d::DATASET_ID_MAX,		1, c_bdd_line_3d::DATASET_ID_MAX	)
			PARAM_DEF_REAL_ONE_ZERO(	draw_s_begin		)
			PARAM_DEF_REAL_ZERO_ONE(	draw_s_end			)
			PARAM_DEF_BOOL_ON(			draw_point			)
			PARAM_DEF_BOOL_ON(			draw_curve			)


		PARAM_DEF_GROUP(	Net,	NET_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		net_send				)
			PARAM_DEF_NET_LINK(		net_link_index,			1, 0	)
			PARAM_DEF_INT32(		net_channel_id,			2, 1,	1, 256*256-1		)
			PARAM_DEF_SYMBO_ZERO(	channel_id_dst,			1, 0,	0, c_bdd_line_3d::LINE_3D_CHANNEL_NB_MAX, str_channel_dst )
			PARAM_DEF_INT32_LOCKED(	blk_point_max_to_send	)
			PARAM_DEF_INT32_LOCKED(	blk_nb_out				)
			PARAM_DEF_INT32_LOCKED(	blk_nb_in				)
			PARAM_DEF_INT32_LOCKED(	memory_size				)	//UINT64_SOON

		PARAM_DEF_NONE(		Master	)
	};
}

void	c_bdd_line_3d::param_init_pt()
{
	INT32	h = param_init_pt_geo();

	param_set_pt( h, _channel_id			);
//	param_set_pt( h, _b_curve_load_save );
	param_set_pt( h, _b_erase_all_trig_ui	);
//	param_set_pt( h, _b_len_max_ui			);
//	param_set_pt( h, _len_max_ui			);

	param_set_pt( h, _dataset_id_ui			);

	++h;
		param_set_pt( h, _b_ui_draw_all_ui		);
		param_set_pt( h, _b_draw_selected_ui	);
		param_set_pt( h, _draw_dataset_begin_ui );
		param_set_pt( h, _draw_dataset_end_ui	);
		param_set_pt( h, _draw_s_begin_ui		);
		param_set_pt( h, _draw_s_end_ui			);
		param_set_pt( h, _b_ui_draw_point_ui	);
		param_set_pt( h, _b_ui_draw_curve_ui	);
		//param_set_pt( h, _b_ui_draw_number_ui	);
		//param_set_pt( h, _number_scale_ui		);

//todo attach master
	++h;
		param_set_pt( h, _st_send._b_active				);
		param_set_pt( h, _st_send._net_link_index		);
		param_set_pt( h, _st_send._net_channel_id		);
		param_set_pt( h, _st_send._channel_id_dst_ui	);
		param_set_pt( h, _st_send._point_nb_max_to_send	);
		param_set_pt( h, _st_send._blk_nb_out			);
		param_set_pt( h, _st_send._blk_nb_in			);
		param_set_pt( h, _st_send._sent_size				);

	param_attach_obj( h, master	);

	err_param_init_pt( h );
}

void	c_bdd_line_3d::init_send()
{
	SAFE_NEW_ARRAY( _st_send._data, POINT_NB_MAX_TO_SEND );
	_st_send._data_nb		= 0;
	_st_send._point_nb_max_to_send = POINT_NB_MAX_TO_SEND;
	_st_send._blk_nb_in		= 0;
	_st_send._blk_nb_out	= 0;
	_st_send._sent_size		= 0;
}

void	c_bdd_line_3d::init()
{
	param_init_with( n_bdd_line_3d::param, n_bdd_line_3d::PARAM_NB_MAX );
	init_send();
}

FINLINE	c_line_3d*		c_bdd_line_3d::find_curve(			UINT32 CONST id )	//	find don't recreate an object, return NULL if none
{	
	return  _map_curve.find( id );
}
FINLINE	c_line_3d*		c_bdd_line_3d::get_curve(			UINT32 CONST id )	//	get create the object if not found
{
	return  _map_curve.get( id );
}
FINLINE	c_line_3d*		c_bdd_line_3d::find_curve_valid(	UINT32 CONST id )
{
	c_line_3d* curve = find_curve( id );
	if( curve && curve->is_valid_curve() )
		return curve;
	return nullptr;
}

void	c_bdd_line_3d::set_dataset( INT32 dataset_id )
{
	if( _dataset_id != dataset_id )
	{
		dataset_id = WRAP_ID( dataset_id, DATASET_ID_MAX );
		_dataset_id = dataset_id;
//		_b_curve_index_changed = true;
//		_curve = find_curve( dataset_id );
//		if( !_curve )
//			_curve = get_curve( dataset_id );
	}
}

CONSTRUCTOR_CREATE( c_bdd_line_3d )
,_dataset_id								(	-1		)
,_dataset_id_ui								(	1		)
//,_curve									(	nullptr	)
,_draw_dataset_begin						(	1		)
,_draw_dataset_end							(	1		)
,_b_valid_curve_for_get_point_rnd_compute	(	true	)
{
	DBG_HEAP_IS_CORRUPT();
//	c_init();
	set_dataset( _dataset_id_ui ); //we need one here to avoid crash in init
	init();
}

void	c_bdd_line_3d::clear_datasets()
{
//	_curve = nullptr;
	_map_curve.clear_elts();
	_dataset_id = -1;
	//todo send erase
}

//todo dealloc generic strategy
void	c_bdd_line_3d::dealloc()
{
	clear_datasets();
	SAFE_DELETE_ARRAY( _st_send._data );
}

c_bdd_line_3d::~c_bdd_line_3d()
{
	dealloc();
}

REAL CONST *	c_bdd_line_3d::get_control_point( INT32 dataset_id, INT32 index_u )
{
	dataset_id = WRAP_ID( dataset_id, DATASET_ID_MAX );
	c_line_3d*	curve = find_curve( dataset_id );
	return curve ? curve->get_control_point( index_u ) : nullptr;
}

REAL CONST *	c_bdd_line_3d::get_control_point( INT32 index_u )
{
	return get_control_point( _dataset_id, index_u );
}

void	c_bdd_line_3d::set_control_point( INT32 dataset_id, INT32 index_u, REAL CONST * CONST vec )
{
	dataset_id = WRAP_ID( dataset_id, DATASET_ID_MAX );
	c_line_3d*	curve = find_curve( dataset_id );
	if( curve )
		curve->set_control_point( index_u, vec );
	//_b_compute_needed = true; //hack should be only by dataset
}

void	c_bdd_line_3d::set_control_point( INT32 index_u, REAL CONST * CONST vec )
{
	set_control_point( _dataset_id, index_u, vec );
}

INT32	c_bdd_line_3d::get_control_point_nb( INT32 dataset_id )
{
	dataset_id = WRAP_ID( dataset_id, DATASET_ID_MAX );
	c_line_3d*	curve = find_curve( dataset_id );
	return curve ? curve->get_control_point_nb() : 0;
}

void	c_bdd_line_3d::send_helper( INT32 CONST dataset_id, REAL CONST * CONST src, c_line_3d*	CONST curve )
{
	INT32			nb	= _st_send._data_nb;
	st_point_send&	elt	= _st_send._data[nb];
	cpy_v3( elt.point, src );
	elt.dataset_id = dataset_id;
	elt.point_nb = curve->get_control_point_nb();
	++nb;
	_st_send._data_nb = nb;
	//get_net_out_blk_size_max
	if( nb >= _st_send._point_nb_max_to_send )
		send( _st_send );
}

void	c_bdd_line_3d::push_control_point_back( INT32 dataset_id, REAL CONST * CONST src )
{
	dataset_id = WRAP_ID( dataset_id, DATASET_ID_MAX );
	c_line_3d*	curve = get_curve( dataset_id );
	curve->push_control_point( src );
//	set_point_control_cur( curve->get_control_point_nb(), false );
	if( _st_send._b_active )
		send_helper( dataset_id, src, curve );
}

DOUBLE	c_bdd_line_3d::push_control_point_back_len_max(	INT32 dataset_id,	REAL CONST * CONST src, REAL CONST len_max )
{
	dataset_id = WRAP_ID( dataset_id, DATASET_ID_MAX );
	c_line_3d*	curve = get_curve( dataset_id );
	curve->push_control_point( src );
//	set_point_control_cur( curve->get_control_point_nb(), false );
	if( _st_send._b_active )
		send_helper( dataset_id, src, curve );

	while( curve->get_len() > len_max )
		curve->pop_control_point( 1 );

	return curve->get_len();
}

void	c_bdd_line_3d::push_control_point_back_receive( st_point_send CONST * elt, UINT32 nb_elt )
{
	for( UINT32 i=0; i<nb_elt; ++i )
	{	
		INT32 dataset_id = WRAP_ID( elt->dataset_id, DATASET_ID_MAX );
		c_line_3d*	curve = get_curve( dataset_id );
		curve->push_control_point( elt->point );
		INT32 nb = curve->get_control_point_nb() - elt->point_nb;
		curve->pop_control_point( nb );
		++elt;
	}
	++_st_send._blk_nb_in;
}

void	c_bdd_line_3d::pop_control_point_front( INT32 dataset_id )
{
	dataset_id = WRAP_ID( dataset_id, DATASET_ID_MAX );
	c_line_3d*	curve = find_curve( dataset_id );
	if( curve )
		return curve->pop_control_point( 1 );
}

void	c_bdd_line_3d::clear_control_points( INT32 dataset_id )
{
	dataset_id = WRAP_ID( dataset_id, DATASET_ID_MAX );
	c_line_3d*	curve = find_curve( dataset_id );
	if( curve )
		return curve->clear();
}

void	c_bdd_line_3d::update()
{
	//todo check what happen before
	//todo then check what happen when object destroyed
	inst_by_channel.set( _channel_id, this );

	_st_send._point_nb_max_to_send = MIN( POINT_NB_MAX_TO_SEND, make_point_net_nb_max( master->get_net_out_blk_size_max() ) );

	if( _b_erase_all_trig_ui )
	{
		_b_erase_all_trig_ui = false;
		clear_datasets();
	}
	else if( _st_send._data_nb >= 0 )
		send( _st_send );
//	c_model::cur->get_size_v3( _size );

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

//	_b_valid_curve_for_get_point_rnd_compute	= true;

	set_dataset( _dataset_id_ui );
//	_nb_points_ui = _curve ? _curve->get_control_point_nb() : 0;

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
}

//
//	DRAW
//

/*
void	c_bdd_curve_3d::draw_marker( INT32 u )
{
	// todo
	// draw marker
}
*/


void	c_bdd_line_3d::draw_points()
{
	//todo drawing is old school
	for( UINT32 j = _draw_dataset_begin; j <= _draw_dataset_end; ++j )
	{
		c_line_3d*	curve = find_curve( j );
		if( curve )
		{
			INT32 nb_draw = curve->get_control_point_nb();
			if( nb_draw > 0 )
			{
				REAL* pt = curve->get_control_point( 0 );
				c_prim3::draw( pt, GL_POINTS, nb_draw );
			}
		}
	}
}

//todo implement the align mode
//todo deal with tex_anim
void	c_bdd_line_3d::draw_points_multiple()
{
	REAL CONST * CONST size = c_multiple::cur->get_size();
	INT32 axe = c_multiple::cur->get_axe();

	for( UINT32 j = _draw_dataset_begin; j <= _draw_dataset_end; ++j )
	{
		c_line_3d*	curve = find_curve( j );
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
							c_multiple::cur->draw_one_at_nor( p, size, nor );
							c_multiple::cur->next_index_u();
						}
						c_multiple::cur->draw_one_at_nor( n, size, nor );
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



//todo optimnize / put drawing code in c_line_3d
void	c_bdd_line_3d::draw_curves()
{
	GOL::unbind_vao_secu();

	for( UINT32 j = _draw_dataset_begin; j <= _draw_dataset_end; ++j )
	{
		c_line_3d*	curve = find_curve_valid( j );
		if( curve )
		{
			UINT32	points_nb = curve->get_point_curve_nb();
			if( points_nb > 0 )
			{
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

				INT32 begin_prev, begin_next;
				curve->find_index( s_begin, begin_prev, begin_next );
				REAL* pt = curve->get_control_point( begin_prev );

				INT32 end_prev, end_next;
				curve->find_index( s_end, end_prev, end_next );
				INT32 nb_draw = MAX( end_next - begin_prev + 1, 0 );

				c_prim3::draw( pt, GL_LINE_STRIP, nb_draw );
			}
		}
	}
}

void	c_bdd_line_3d::draw_single()
{
	begin_ui();

		//	Draw point
		//if( is_ui_intercept() || _b_ui_draw_point_ui )			draw_points();
		//if( is_ui_intercept() || _b_ui_draw_curve_ui )			draw_curves();
		if(	_b_ui_draw_point_ui )
			draw_points();
		if( _b_ui_draw_curve_ui )
			draw_curves();


	end_ui();
}

void	c_bdd_line_3d::draw_multiple()
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


//void	c_bdd_line_3d::set_control_point( INT32 dataset, INT32 index_u, REAL* vec )
//{
//	get_curve( dataset )->set_control_point( index_u, vec );
//}
//
//void	c_bdd_line_3d::set_control_point( INT32 index_u, REAL* vec )
//{
//	set_control_point( _dataset_id, index_u, vec );
//}


void	c_bdd_line_3d::get_tra( REAL* dst, INT32 CONST dataset_id, REAL CONST s )
{
	c_line_3d* curve = find_curve_valid( dataset_id );
	if( curve )
		curve->get_point( dst, s );
	else
		clear_v3( dst );
}
void	c_bdd_line_3d::get_tangent( REAL* dst, INT32 CONST dataset_id, REAL CONST s )
{
	c_line_3d* curve = find_curve_valid( dataset_id );
	if( curve )
		curve->get_tangent( dst, s );
	else
		clear_v3( dst );
}
void	c_bdd_line_3d::get_point_tangent( REAL* dst, REAL* tgn, INT32 CONST dataset_id, REAL CONST s )
{
	c_line_3d* curve = find_curve_valid( dataset_id );
	if( curve )
		curve->get_point_tangent( dst, tgn, s );
	else
	{
		clear_v3( dst );
		clear_v3( tgn );
	}
}

//ignore time
bool	c_bdd_line_3d::get_point_rnd( REAL* CONST dst, REAL CONST t_in )
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
			REAL f = n_bdd_line_3d::rand.get_fp32_max(fnb);
			UINT32 id;
			REAL tmp = _draw_s_begin_end - _draw_s_begin_begin;
			if( f < tmp )
			{
				id = _draw_dataset_begin;
				if( c_line_3d* curve = find_curve_valid( id ) )
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
					if( c_line_3d* curve = find_curve_valid( id ) )
					{
						curve->get_point( dst, f-nb );
						break;
					}
				}
				else
				{
					UINT32 id = _draw_dataset_end;
					if( c_line_3d* curve = find_curve_valid( id ) )
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

DOUBLE	c_bdd_line_3d::get_len( INT32 dataset_id )
{
	if( c_line_3d* curve = find_curve_valid( dataset_id ) )
		return curve->get_len();
	return 0;
}


AAA_ERR	c_bdd_line_3d::send(	st_net_line_3d& st_send )
{
	_st_send._channel_id_dst	= _st_send._channel_id_dst_ui == 0	? _channel_id : _st_send._channel_id_dst_ui;

	if( !master->is_net_out_active() || _st_send._data_nb <= 0 )
	{
		_st_send._data_nb = 0;
		return AAA_OK;
	}

	UINT8* pt;
	pt = c_net::write_value( buf_send, UINT32(0) );	//len for decode or 0 when no compression
	pt = c_net::write_value( pt, st_send._channel_id_dst	);
	pt = c_net::write_value( pt, MAKE_CODE( OP_PUSH, COMP_NO ) );
	pt = c_net::write_value( pt, _st_send._data_nb	);

	SPY_PUSH_RANGE( "line_3d::send()", spy::SYNC );
		
		INT32 byte_nb = POINT_NET_SIZE * _st_send._data_nb;
		MEMCPY( pt, _st_send._data, byte_nb, __FUNCTION__ );
		pt += byte_nb;
		byte_nb = INT32( pt - buf_send );

		net->sendto( st_send._net_link_index, st_send._net_channel_id, c_net::BLK_LINE_3D, buf_send, byte_nb );

		st_send._data_nb = 0;
		++st_send._blk_nb_out;
		master->inc_blk_nb_out();
		st_send._sent_size = byte_nb;

	SPY_POP_RANGE();

	return AAA_OK;
}

AAA_ERR	c_bdd_line_3d::process_blk( UINT8 CONST * CONST data, INT32 CONST len, INT32 CONST net_link_index )
{
	master->inc_blk_nb_in();
	INT32 len_out;
	UINT8* pt = c_net::read_value( data, &len_out );
	//todo add compression type
	if( len_out )	// len means we have to decompress 
	{
		if( len_out < 0 )
		{
			if( master->is_verbose_receive_incoherent() )
				ERR_PRINT_STRING( "BLK_LINE_3D got a negative len : %d", len );
		}
		else
		{
			if( master->is_verbose_receive_incoherent() )
				ERR_PRINT_STRING( "BLK_LINE_3D with a too big len : %d", len );
		}
		net->get_link(net_link_index)->inc_img_err_nb();	//todo move to other counter
		return ERR_DATA_INCOHERENT;
	}

	INT32 channel_id_dst;
	pt = c_net::read_value( pt, &channel_id_dst );

	c_bdd_line_3d* bdd = get_from_channel( channel_id_dst );
	if( !bdd )
	{
		//todo refine (infact if the receive dont have corresponding curve object ... it is correct
		//		so should not be treatead as incoherent
		if( master->is_verbose_receive_incoherent() )
			ERR_PRINT_STRING( "BLK_LINE_3D with a non existing bdd_line_3d channel  : %d", channel_id_dst );
		return ERR_DATA_SKIPPED;
	}

	BLK_OP		op;
	BLK_COMP	comp;
	GET_OP_COMP( *(UINT32*)pt, op, comp );
	pt += 4;

	switch( op )
	{
	case OP_PUSH:
		{
			UINT32 nb;
			pt = c_net::read_value( pt, &nb );
			bdd->push_control_point_back_receive( (st_point_send*)pt, nb );	
			net->get_link(net_link_index)->inc_img_received_nb();	//todo move to other counter
		}
		break;
	default:
		ERR_PRINT_STRING( "Unknown opcode for BLK_LINE_3D : %d", op );
		net->get_link(net_link_index)->inc_img_err_nb();	//todo move to other counter
		return ERR_DATA_INCOHERENT;
	}

	master->inc_blk_nb_in_processed();
	return AAA_OK;


}
