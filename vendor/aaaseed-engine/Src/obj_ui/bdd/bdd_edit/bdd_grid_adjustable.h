
#ifdef AAA_BDD_GRID_ADJUSTABLE_H
#error "BDD_GRID_ADJUSTABLE_H included more than once."
#endif
#define AAA_BDD_GRID_ADJUSTABLE_H 1


#ifndef AAA_BDD_UV_H
#	include "obj_ui/bdd/util/bdd_uv.h"
#endif
#ifndef AAA_POINTS_H
#	include "draw/geo/points.h"
#endif
#ifndef AAA_REGISTRY_GENERIC_H
#	include "infrastructure/factory/registry_generic.h"
#endif

//todo move to his own file or registry

class	c_bdd_grid_adjustable final : public c_bdd_uvw
{
	FACTORY_DECLARE( c_bdd_grid_adjustable, c_bdd_uvw );
public:
	static	CONST	INT32	DATASET_ID_MAX = 32;
private:
	struct st_undo {
		INT32		dataset_id = -42;
		INT32		iu;
		INT32		iv;
		REAL		pt[3];
	};

	bool			_b_deform;
	bool			_b_deform_ui;
	bool			_b_compute_force_ui;
	bool			_b_compute_needed;
	bool			_b_grid_reset_trig_ui;
	bool			_b_pos_reset_trig_ui;
	bool			_b_ui_draw_ui;
	bool			_b_ui_draw_selected_ui;
	bool			_b_ui_draw_curve_ui;
	bool			_b_ui_draw_point_ui;
	REAL			_ui_tgn_size_ui;

	INT32			_iu_cur_ui;
	INT32			_iv_cur_ui;
	INT32			_iu_cur;
	INT32			_iv_cur;

	bool			_b_grid_draw_ui;

	bool			_b_need_update_param_spe;

	bool			_b_catmull_ui;
	bool			_b_catmull;
	bool			_b_edge_auto_ui;
	bool			_b_curve_inherit_last_ui;
	bool			_b_ui_intercept_ui;
	bool			_b_intercept;
	bool			_b_ui_flip_u_ui;
	bool			_b_ui_flip_v_ui;
	REAL			_ui_start[2];	// needed for flip
	bool			_b_ui_lock_u_ui;
	bool			_b_ui_lock_v_ui;
	REAL			_ui_size_ui;

	REAL			_ui_pos_ui[3];
	FP32			_ui_alpha_ui;
	
	bool			_b_negate_u_trig_ui;
	bool			_b_symmetry_u_trig_ui;
	bool			_b_symmetry_u_ui;
	bool			_b_symmetry_v_trig_ui;
	bool			_b_symmetry_v_ui;

	INT32			_nb_u_new_ui;
	INT32			_nb_v_new_ui;
	bool			_b_nb_new_trig_ui;

	FP32			_pt_last[3];

	c_map_server< INT32, c_grid_point3d >	_map_controls;

	c_grid_point3d*	_controls;		//	the points are there
	//allocated stock are 
	c_grid_point3d*	_controls_def;	//	this is used for the deformer
	c_grid_point3d*	_controls_old;	//	when we change the nb of control point we need a second structure to compute the new from the old

	c_grid_point3d*	_p_controls;
	c_grid_point3d*	_p_controls_def_to_draw;

	c_points_1d		_u;
	c_points_1d		_v;

	INT32			_dataset_id_ui;
	INT32			_dataset_id;

	bool			_b_ui_deform_ui;
	INT32			_gad_nb_u_ui;
	INT32			_gad_nb_v_ui;

	//to make it better than just one
	st_undo			_undo_store;
	st_undo			_redo_store;

private:
	void	cpy_cur_to_st_undo(		st_undo * CONST pt );
	void	process_st_undo_to_cur( st_undo * CONST pt );
	void	undo();
	void	redo();

public:
	virtual	void	param_init_pt();

	virtual	AAA_ERR	load_do_after( o_str CONST & filename );
	virtual	AAA_ERR	save_do_after( o_str CONST & filename );

	//
	//	UI_INTERCEPT_UI
	//
	virtual	bool	can_ui_intercept()			{	return true; }
	virtual bool	set_ui_intercept( bool b )	{	return _b_ui_intercept_ui = b; }
	virtual bool	is_ui_intercept()			{	return _b_ui_intercept_ui; }

	virtual	bool	do_key( INT32 c, bool CONST b_special, INT32* modifiers, INT32* x, INT32* y );

	virtual	bool	mouse_down( FP32& u_start, FP32& v_start );
	virtual	void	mouse_move( FP32 u_in, FP32 v_in );

private:
			bool	is_control_point_moveable(		INT32 CONST iu, INT32 CONST iv );
			void	build_control_grid_one_point(	INT32 CONST iu, INT32 CONST iv );
			void	build_control_grid();
			void	build_control_grid_from_old();

			void	update_param_spe();
protected:
			void	alloc_curve();
			void	update_curve( INT32 CONST dataset_id );		
public:
			void	draw_cat_line_oriented( REAL CONST * CONST a, REAL CONST * CONST b, REAL CONST * CONST c, REAL CONST * CONST d, INT32 nb, REAL size_in );	
			
	virtual	void	get_point_from_uv( c_grid_point3d* controls, REAL* CONST dst, REAL CONST u, REAL CONST v );
	virtual	void	get_point_from_uv( REAL* CONST dst, REAL CONST u, REAL CONST v );
	virtual	void	build_geo();

	virtual	void	draw();
	virtual	void	update();

	virtual	bool	do_command(		C_PCHAR_C cmd );
	virtual	bool	enum_command(	o_str& o );

			void	build( REAL CONST z );
			void	draw_ui_point( INT32 CONST u, INT32 CONST v, REAL CONST size_in, REAL CONST * CONST pos_in, bool CONST b_cross );
			void	draw_ui_tgn( REAL CONST * CONST c, REAL CONST * CONST dir );
			void	draw_ui( REAL CONST size_in, REAL CONST * CONST pos_in, FP32 CONST alpha_in );
			
			void	negate_u();
			void	do_symmetry_u();
			void	do_symmetry_v();

			REAL*	get_control_point( INT32 CONST dataset, INT32 CONST index_u, INT32 CONST index_v );
			REAL*	get_control_point(						INT32 CONST index_u, INT32 CONST index_v );
			void	set_control_point( INT32 CONST dataset,	INT32 CONST index_u, INT32 CONST index_v, REAL CONST * CONST vec ) ;
			void	set_control_point(						INT32 CONST index_u, INT32 CONST index_v, REAL CONST * CONST vec );
			void	set_point_control_cur( INT32 CONST iu, INT32 CONST iv, bool CONST b_send_command=true );

	FINLINE	REAL*	get_control_point_def(				INT32 CONST u, INT32 CONST v )	CONST { return _p_controls_def_to_draw->get_point( u,v ); }
	FINLINE	void	get_control_point_def( REAL* dst,	INT32 CONST u, INT32 CONST v, REAL CONST size_in, REAL CONST * CONST pos_in=nullptr ) CONST
			{	
				REAL*	src = get_control_point_def( u,v );
				dst[_i_u] = src[0] * size_in;
				dst[_i_v] = src[1] * size_in;
				dst[_i_axe] = src[2] * size_in;
				if( pos_in )
					add_v3( dst, pos_in );
			}
	FINLINE	void	get_control_point( REAL* dst, INT32 CONST u, INT32 CONST v, REAL CONST size_in ) CONST
			{
				REAL*	src = _p_controls->get_point( u,v );
				if( src )
				{
					dst[_i_u]	= src[0] * size_in;
					dst[_i_v]	= src[1] * size_in;
					dst[_i_axe] = src[2] * size_in;
				}
				else
					clear_v3(dst);	
			}
			bool	get_uv_from_pos( REAL& u, REAL& v, REAL CONST * CONST vec ) CONST { return _p_controls->get_uv_from_pos( u, v, vec ); }
			void	generate_uv_from_pos( REAL* puv, INT32 CONST nb_u, INT32 CONST nb_v, INT32 CONST dataset_id );

	FINLINE	void	set_color_construction( FP32 CONST alpha_in=1. );
	FINLINE	void	set_color_ui(			FP32 CONST alpha_in=1. );
	FINLINE	void	set_color_ui_selected(	FP32 CONST alpha_in=1. );

			void	do_auto_edge( INT32 CONST dataset );
};
