
#ifdef AAA_MULTIPLE_H
#error "MULTIPLE_H included more than once."
#endif
#define AAA_MULTIPLE_H 1


#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif
#ifndef	AAA_V_H
#	include "math/v.h"
#endif
#ifndef AAA_SEEDDRAW_H
#	include "draw/seeddraw.h"
#endif

class	c_layer;
class	c_layers;
class	c_module;
class	c_img_2d;

//todo apply brain object here
class	c_multiple : public c_obj_ui
{
	FACTORY_DECLARE( c_multiple, c_obj_ui );
public:
	static	c_multiple*	def;
	static	c_multiple*	cur;
	static	c_multiple*	ui;

	static	bool		b_use_matrix_ui;

/*
	FINLINE	static	void		set_def( c_multiple* mul )	{	def = mul;		}
	FINLINE	static	c_multiple*	get_def()					{	return def;		}

	FINLINE	static	void		set_cur( c_multiple* mul )	{	cur = mul;		}
	FINLINE	static	void		set_cur_null()				{	set_cur( def );	}
	FINLINE	static	c_multiple*	get_cur()					{	return cur;		}

	FINLINE	static	void		set_ui( c_multiple* mul )	{	ui = mul;		}
	FINLINE	static	void		set_ui_null()				{	set_ui( def );	}
	FINLINE	static	c_multiple*	get_ui()					{	return ui;		}
*/

	static	bool		_b_allow_list;

	enum TYPE : INT32
	{
		TYPE_NUMBER = 0,
		TYPE_FACE,
		TYPE_FACE_CROSS,
		TYPE_FACE_THREE,
		TYPE_CUBE,
		TYPE_BOX,
		TYPE_SPHERE,
		TYPE_CIRCLE,
		TYPE_CONE,
		TYPE_AXE,
		TYPE_LAYERS_PREV,
		TYPE_LAYERS_NAME,
		TYPE_MODULE_NAME,
		TYPE_LAYERS,
		TYPE_LAYER,
		TYPE_LUA,
//		TYPE_TRI,
//		TYPE_PREV,
		TYPE_MAX_NB
	};
	enum ALIGN : INT32
	{
		ALIGN_WORLD = 0,
		ALIGN_NORMAL,
		ALIGN_NORMAL_USING_Z,
		ALIGN_NORMAL_OFFSET,
		ALIGN_BILLBOARD,
		ALIGN_RANDOM,
		ALIGN_RANDOM_AXE_X,
		ALIGN_RANDOM_AXE_Y,
		ALIGN_RANDOM_AXE_Z,
		ALIGN_GRADIENT,
		ALIGN_MAX_NB
	};
	enum RENDER : INT32
	{
		RENDER_REGULAR = 0,
		RENDER_MULTIPLE,
		RENDER_BOTH,
		RENDER_MAX_NB
	};
	enum TRA : INT32
	{
		TRA_IMPLICIT = 0,
		TRA_NONE,
		TRA_MAX_NB
	};

	static INT32 CONST PARAMETER_NB	= 16;

private:
	TYPE		_s_type;
	RENDER		_s_render_ui;

	bool		_b_align_normal;
	ALIGN		_s_align_ui;
	INT32		_int_resolution_by_6;
	INT32		_int_resolution_by_12;
	FP32		_resolution_ui;
	INT32		_s_translation_ui;
	INT32		_s_axe_ui;
	INT32		_i_u;
	INT32		_i_v;
	TYPE		_s_type_ui;
	REAL		_size_ui[4];
	REAL		_size[3];
	REAL		_size_jitter_ui;
	REAL		_size_jitter;
	REAL		_size_jitter_exponent_ui;
	REAL		_offset[3];
	INT32		_start_ui;
	INT32		_start;
	INT32		_stop_ui;
	INT32		_stop;
	o_str		_target_name;

	bool		_b_color_from_xz_ui;
	INT32		_color_img_bind;
	INT32		_grad_img_bind;
	c_img_2d*	_img_color;
	c_img_2d*	_img_grad;
	bool		_b_grad_normalize_ui;
	REAL		_grad_min;
	REAL		_grad_max;
	REAL		_grad_min_squared;
	REAL		_grad_max_squared;
	REAL		_grad_size_offset;
	REAL		_grad_size_factor;
	REAL		_grad_size_min_ui;
	REAL		_grad_size_max_ui;
	bool		_b_grad_size_inverse_ui;
	FP32		_uv_scale_ui[2];
	FP32		_uv_factor[2];
	FP32		_uv_offset_ui[2];
	bool		_b_clamped_ui;
	bool		_b_linear_ui;
	INT32		_s_compo;

	REAL		_pos_draw[3];
	REAL		_size_draw[3];
	
	REAL		_parameter[PARAMETER_NB];

	void		update_low();

	FINLINE	TYPE	get_type()		CONST			{ return _s_type; }
public:
	FINLINE	void	bdd_do_force_update()			{ update_low(); }

			void	build_sum_up( o_str& o );
	virtual	void	param_init_pt();

//MULTIPLE
	FINLINE	bool	is_render_regular()		CONST	{ return this ? _s_render_ui!=RENDER_MULTIPLE : true ; }
	FINLINE	bool	is_render_multiple()	CONST	{ return this ? (_s_render_ui!=RENDER_REGULAR && draw::b_multiple_draw_allow) : false ; }


	FINLINE	ALIGN	get_align()				CONST	{ return _s_align_ui; }
	FINLINE	bool	is_align_billboard()	CONST	{ return _s_align_ui == ALIGN_BILLBOARD; }
	FINLINE	bool	is_align_normal()		CONST	{ return this ? _b_align_normal : false; }
	FINLINE	bool	is_align_random()		CONST	{ return ALIGN_RANDOM <= _s_align_ui && _s_align_ui <= ALIGN_RANDOM_AXE_Z; }
	FINLINE	bool	is_tra_implicit()		CONST	{ return _s_translation_ui == TRA_IMPLICIT; }
//	FINLINE	bool	is_tra_none()			CONST	{ return _s_translation_ui == TRA_NONE; }

	FINLINE	INT32	get_axe()				CONST	{ return _s_axe_ui; }
	FINLINE	void	get_axes_index( INT32& i_u, INT32& i_v, INT32& i_axe )	CONST { i_u = _i_u; i_v = _i_v; i_axe = _s_axe_ui; }

	FINLINE	REAL			get_size( INT32 index )			CONST	{ return _size[index];			}
	FINLINE	REAL CONST *	get_size()						CONST	{ return _size;					}

	// this is used by traxs for now (2021 Oct). extend to lua ?
	FINLINE	REAL CONST *	get_pos_draw()					CONST	{ return _pos_draw;				}
	FINLINE	void			set_pos_draw( REAL CONST* pos_in )		{ cpy_v3( _pos_draw, pos_in );	}
	// this is used by traxs for now (2021 Oct). extend to lua ?
	FINLINE	REAL CONST *	get_size_draw()					CONST	{ return _size_draw;			}
	FINLINE	void			set_size_draw( REAL CONST * size_in )	{ cpy_v3( _size_draw, size_in ); }

//	FINLINE	INT32	get_start()						{ return _start; }
//	FINLINE	INT32	get_stop()						{ return _stop; }

	FINLINE	o_str CONST &	get_target_name()		CONST	{ return _target_name; }

			bool	can_implicit();
			bool	can_use_list();

	FINLINE	void	update()
	{
		if( is_render_multiple() )
			update_low();
		cur = this;
	}
	FINLINE	REAL	get_parameter( INT32 index )
	{
		index = IMOD( index, PARAMETER_NB );
		return( _parameter[index] );
	}
	FINLINE	void	set_parameter( INT32 index, REAL val )
	{
		index = IMOD( index, PARAMETER_NB );
		_parameter[index] = val;
	}
private:
	INT32		_imod;
	INT32		_layers_cur;

	INT32		_index;
	INT32		_index_by_dim[3];
	INT32		_nb_by_dim[3];

	REAL		_real_factor[3];

	INT32		_nb;

	bool		_b_bbox_see_on_cur_store;
	c_layer*	_p_layer_cached;
	c_layers*	_p_layers_cached;
	c_module*	_p_module_cached;
	bool		_b_push_done;

public:
	FINLINE	void	set_multiple_layers_cur( INT32 index )	{	_layers_cur = index;	}
	FINLINE	void	init_index()
	{	//todo optimize : call less and do less
		_index = 0;
		_index_by_dim[0] = 0;
		_index_by_dim[1] = 0;
	}
	FINLINE	void	init_index_w()
	{	//hack use of this is a pure hack look at 
		// c_bdd_multiple::draw() and c_bdd_grid::draw()
		_index_by_dim[2] = 0;
	}
	FINLINE	void	set_nb( INT32 nb_u = 1, INT32 nb_v = 1, INT32 nb_w = 1)
	{	//todo get this info from the bdd ?F
//todo was buggy all index 0 consequence ?
		_nb_by_dim[0] = nb_u;
		_nb_by_dim[1] = nb_v;
		_nb_by_dim[2] = nb_w;

		_real_factor[0] = (nb_u > 1) ? REAL(1) / REAL(nb_u-1) : 1;
		_real_factor[1] = (nb_v > 1) ? REAL(1) / REAL(nb_v-1) : 1;
		_real_factor[2] = (nb_w > 1) ? REAL(1) / REAL(nb_w-1) : 1;

		_nb = nb_u * nb_v * nb_w;
		_nb = (_nb>2) ? _nb-1 : 1 ;
	}
	FINLINE	void	set_index( INT32 index_in )
	{
		if( _imod != 1 )
			set_multiple_layers_cur( IMOD( index_in-_start, _imod) + _start );
		else
			set_multiple_layers_cur( _start );
		_index = index_in;
	}
	FINLINE	void	next_index()
	{
		//todoqq	make it work both way and integrate step non integer
		//	common with tex_anim
		set_multiple_layers_cur( _layers_cur+1 );
		if( _layers_cur > _stop )
			set_multiple_layers_cur( _start );
		++_index;
	}
	FINLINE	void	next_index_u()
	{
		next_index();
		++_index_by_dim[0];
	}
	FINLINE	void	next_index_v()
	{
		_index_by_dim[0] = 0;
		++_index_by_dim[1];
	}
	FINLINE	void	next_index_u_hack()
	{
		next_index();
		++_index_by_dim[1];
	}
	FINLINE	void	next_index_v_hack()
	{
		_index_by_dim[1] = 0;
		++_index_by_dim[0];
	}
	FINLINE	void	next_index_w()
	{
		_index_by_dim[0] = 0;
		_index_by_dim[1] = 0;
		++_index_by_dim[2];
	}
	FINLINE	INT32	get_index_u()	CONST	{	return	_index_by_dim[0];	}
	FINLINE	INT32	get_index_v()	CONST	{	return	_index_by_dim[1];	}
	FINLINE	INT32	get_index_w()	CONST	{	return	_index_by_dim[2];	}

	FINLINE	REAL	get_real_u()	{	INT32 i = get_index_u();	return	i ?  i * _real_factor[0] : 0.0f;	}
	FINLINE	REAL	get_real_v()	{	INT32 i = get_index_v();	return	i ?  i * _real_factor[1] : 0.0f;	}
	FINLINE	REAL	get_real_w()	{	INT32 i = get_index_w();	return	i ?  i * _real_factor[2] : 0.0f;	}

	FINLINE	INT32	get_nb_u()		CONST	{	return	_nb_by_dim[0];	}
	FINLINE	INT32	get_nb_v()		CONST	{	return	_nb_by_dim[1];	}
	FINLINE	INT32	get_nb_w()		CONST	{	return	_nb_by_dim[2];	}

	FINLINE	INT32	get_index()		CONST	{	return	_index;	}
	FINLINE	REAL	get_real()		CONST	{	INT32 i = _index;	return	i ? REAL(i) / _nb : 0.0f;	}
	FINLINE	INT32	get_nb()		CONST	{	return	_nb;	}

	//DRAW
private:
			void	set_color_low_r(	REAL CONST * color );
	FINLINE	void	set_color_r(		REAL CONST * color )		{	if( _img_color )	set_color_low_r( color );	}

	typedef void	ALIGN_NOR_THEN_DRAW_FN						(	REAL CONST * CONST pos,	REAL CONST * CONST size,
																	REAL CONST * CONST u,	REAL CONST * CONST v,	REAL CONST * CONST nor	);
	typedef void	(c_multiple::*ALIGN_NOR_THEN_DRAW_FN_PT)	(	REAL CONST * CONST pos,	REAL CONST * CONST size,
																	REAL CONST * CONST u,	REAL CONST * CONST v,	REAL CONST * CONST nor	);
	ALIGN_NOR_THEN_DRAW_FN_PT	_pfn_align_nor_then_draw;

	typedef void	ALIGN_THEN_DRAW_FN							(	REAL CONST * CONST pos,	REAL CONST * CONST size,	INT32 CONST axe );
	typedef void	(c_multiple::*ALIGN_THEN_DRAW_FN_PT)		(	REAL CONST * CONST pos,	REAL CONST * CONST size,	INT32 CONST axe );
	ALIGN_THEN_DRAW_FN_PT		_pfn_align_then_draw;

	ALIGN_THEN_DRAW_FN	align_world_then_draw;
	ALIGN_THEN_DRAW_FN	align_normal_then_draw;
	ALIGN_THEN_DRAW_FN	align_normal_using_z_then_draw;
	ALIGN_THEN_DRAW_FN	align_normal_offset_then_draw;
	ALIGN_THEN_DRAW_FN	align_gradient_then_draw;
	ALIGN_THEN_DRAW_FN	align_billboard_then_draw;
	ALIGN_THEN_DRAW_FN	align_random_then_draw;
	ALIGN_THEN_DRAW_FN	align_random_axe_x_then_draw;
	ALIGN_THEN_DRAW_FN	align_random_axe_y_then_draw;
	ALIGN_THEN_DRAW_FN	align_random_axe_z_then_draw;

	FINLINE	void	pick_fn_for_align_then_draw();

	FINLINE	void	basic_draw(				TYPE CONST type );
	FINLINE	void	draw_one_at_low(		REAL CONST * pos,	REAL CONST * size,	INT32 axe	);
			void	draw_one_at_tgn_low(	REAL CONST * pos,	REAL CONST * size,	REAL CONST * u,	REAL CONST * v,	REAL CONST * nor	);

	FINLINE	void	do_random_rotate();
	FINLINE	void	do_random_rotate_axe( INT32 axe);
public:
	FINLINE	void	draw_one_low(							REAL CONST * CONST size,	INT32 CONST axe	);

public:

	FINLINE	void	align_then_draw(		REAL CONST * pos,	REAL CONST * size,	INT32 axe	)
	{	
		set_pos_draw( pos );
		set_size_draw( size );
		set_color_r( pos );		//maa this should be understood and commented

		(this->*_pfn_align_then_draw)( is_tra_implicit() ? pos : zero_v4fp32, size, axe );
	}
	FINLINE	void	align_then_draw(		REAL CONST * pos,	REAL CONST * size	)
	{
		align_then_draw( pos, size, _s_axe_ui );
	}
	FINLINE	void	align_then_draw(		REAL CONST * pos )
	{
		align_then_draw( pos, _size );
	}
private:
	FINLINE	void	draw_one_at_tgn_offset_private(	REAL CONST * pos,	REAL CONST * size,	REAL CONST * u,	REAL CONST * v,	REAL CONST * nor );
	FINLINE	void	draw_one_at_tgn_private(		REAL CONST * pos,	REAL CONST * size,	REAL CONST * u,	REAL CONST * v,	REAL CONST * nor );


public:
	FINLINE	void	draw_one_at_tgn(				REAL CONST * pos,	REAL CONST * size,	REAL CONST * u,		REAL CONST * v,	REAL CONST * nor	)
	{
		(this->*_pfn_align_nor_then_draw)( pos, size, u, v, nor );
	}
	FINLINE	void	draw_one_at_tgn(				REAL CONST * pos,	REAL CONST * u,		REAL CONST * v,	REAL CONST * nor	)
	{
		draw_one_at_tgn( pos, _size, u, v, nor );
	}

	FINLINE	void	draw_one_at_nor(				REAL CONST * pos,	REAL CONST * size,	REAL CONST * nor	)
	{
		REAL	u[3];
		REAL	v[3];
		build_normal_vectors_v3r( nor, u, v );
		draw_one_at_tgn( pos, size, u, v, nor );
	}
	FINLINE	void	draw_one_at_nor_using_z(		REAL CONST * pos,	REAL CONST * size,	REAL CONST * nor	)
	{
		REAL	u[3];
		REAL	v[3];
		build_normal_vectors_using_z_v3r( nor, u, v );
		draw_one_at_tgn( pos, size, u, v, nor );
	}
	FINLINE	void	draw_one_at_nor(				REAL CONST * pos,	REAL CONST * nor	)
	{
		if( _s_align_ui == ALIGN_NORMAL )
			draw_one_at_nor( pos, _size, nor );
		else
			draw_one_at_nor_using_z( pos, _size, nor );
	}

	bool	begin();
	void	end();
};

