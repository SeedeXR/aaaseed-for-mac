
#ifdef AAA_BDD_CURVE_3D_H
#error "BDD_CURVE_3D_H included more than once."
#endif
#define AAA_BDD_CURVE_3D_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif
#ifndef AAA_AAA_CONST_H
#	include "infrastructure/aaa_const.h"
#endif
#ifndef AAA_REGISTRY_GENERIC_H
#	include "infrastructure/factory/registry_generic.h"
#endif
#ifndef AAA_CURVES_H
#	include "math/curves.h"
#endif
#ifndef AAA_BDD_EDIT_H
#	include "bdd_edit.h"
#endif

class	c_bdd_curve_3d  final : public c_bdd_multiple
{
	FACTORY_DECLARE( c_bdd_curve_3d, c_bdd_multiple );
public:
	static	CONST	INT32	CURVE3D_CHANNEL_NB_MAX = CHANNEL_NB_MAX;
private:
	static	c_instance_by_channel< c_bdd_curve_3d, CURVE3D_CHANNEL_NB_MAX > inst_by_channel;
public:
	static	c_bdd_curve_3d*	get_from_channel( INT32 channel_id )	{	return inst_by_channel.get( channel_id );	}

	static	CONST	INT32	DATASET_ID_MAX			=	9999	;
	static	CONST	INT32	DATASET_ID_DIGIT_MAX	=	4		;

private:
	INT32	_channel_id;

	REAL	_size[3];

//	bool	_b_marker_show;
//	bool	_b_compute_forced;
//	bool	_b_compute_needed;
//	bool	_b_ui_draw_ui;

	bool	_b_ui_edit;
	bool	_b_draw_selected_ui;

	bool	_b_curve_load_save;

	bool	_b_draw_ui;
	bool	_b_ui_draw_all_ui;
	UINT32	_draw_dataset_begin;
	UINT32	_draw_dataset_end;
	UINT32	_draw_dataset_begin_ui;
	UINT32	_draw_dataset_end_ui;
	REAL	_draw_s_begin_ui;
	REAL	_draw_s_end_ui;
	REAL	_draw_s_begin_begin;
	REAL	_draw_s_begin_end;
	REAL	_draw_s_end_begin;
	REAL	_draw_s_end_end;

	bool	_draw_force_color_ui;
	bool	_b_ui_draw_curve_ui;
	bool	_b_ui_draw_point_ui;
	bool	_b_ui_draw_number_ui;
	REAL	_number_scale_ui;

	UINT32	_point_cur_index_ui;
	UINT32	_point_cur_index;

	bool	_b_curve_index_changed;

	bool	_b_erase_all_trig_ui;

	INT32	_s_move_constraint;

	bool	_b_valid_curve_for_get_point_rnd;
	bool	_b_valid_curve_for_get_point_rnd_compute;

	class	c_curve_info
	{
	public:
//		INT32	_nb_markers;
		c_curve_3d::CURVE3D_TYPE	_s_type;
		UINT32						_lod;
		REAL						_tightness;
		bool						_b_loop;

		void	init();
		c_curve_info();

		void clear();
		//~c_curve_info();
		void load( C_PCHAR_C name_info );
		void save( C_PCHAR_C name_info );
	};

	//c_map_server< INT32, c_curve_info >	_map_curve_info;
	c_vector_server< c_curve_info >			_map_curve_info;
	c_curve_info*							_curve_info;

protected:
//	FINLINE	bool 			is_info_from_id(  INT32 id );
	FINLINE	c_curve_info* 	get_info_from_id( INT32 id );
private:
//	bool	_b_ui_lock;

	// var for mouse down, need to keep original point in 3d with current depth
	FP32		_xyz[3];
	REAL		_point_mouse_down[3];
	c_seedcam*	_cam_used;

//	INT32	_point_down_dataset_id;
//	INT32	_point_down_cur;

	//c_map_server< INT32, c_curve_3d >		_map_curve;
	c_vector_server< c_curve_3d >			_map_curve;
	c_curve_3d*								_curve;

	INT32		_dataset_id_ui;
	INT32		_dataset_id;

	INT32		_nb_points_ui;

	bool		_b_coor_to_world_use_z_ui;

private:
			void	dealloc();
	//			void	alloc_curve();
	FINLINE void	sync_curve_and_info( c_curve_3d* curve, c_curve_info* info );
	FINLINE	void	sync_curve( INT32 id );

	FINLINE	c_curve_info* 	find_info(			UINT32 id )	//	find don't recreate an object, return NULL if none
															{	return  _map_curve_info.find( id );	}
	FINLINE	c_curve_info* 	get_info(			UINT32 id )	//	get create the object if not found
															{	return  _map_curve_info.get( id );	}
public:
	FINLINE	c_curve_3d*		find_curve(			UINT32 id )	//	find don't recreate an object, return NULL if none
															{	return  _map_curve.find( id );		}
	FINLINE	c_curve_3d*		get_curve(			UINT32 id ) //	get create the object if not found
															{	return  _map_curve.get( id );		}
	FINLINE	c_curve_3d*		find_curve_valid(	UINT32 id )	{
																c_curve_3d* curve = find_curve( id );
																return (curve && curve->is_valid_curve()) ? curve : nullptr;
															}
private:
//	bool		_b_ui_deform;
	FP32		_ui_alpha;
	bool		_b_graduation_ui;
	REAL		_graduation_step;
	REAL		_graduation_scale;

/*
	bool		_b_draw_marker;
	UINT32		_marker_nb_ui;
	UINT32		_marker_cur_ui;
	UINT32		_marker_cur;
	o_str		_marker_str;

	c_map_server< INT32, c_point3d_marker >	_map_markers;
	c_map_server< INT32, c_point3d_marker >	_map_markers_def;

	c_point3d_marker*	_markers;		//	the points are there
	c_point3d_marker*	_markers_def;	//	this is use for the deformer
*/
	REAL	_dummy[3];
	c_edit	_edit;
	
public:
			void	init();
	virtual	void	param_init_pt();

	virtual	AAA_ERR	load_do_after( o_str CONST & filename );
	virtual	AAA_ERR	save_do_after( o_str CONST & filename );

	//todoqq sub class bdd_ui ? or policies
	//
	//	INTERCEPT_UI
	//
	virtual	bool	can_ui_intercept()			{	return true; }
	virtual bool	set_ui_intercept( bool b )	{	return _edit.set_ui_intercept( b ); }
	virtual bool	is_ui_intercept()			{	return _edit.is_ui_intercept(); }

			bool	do_action(		o_str CONST & action_asked );
			bool	do_action(		c_edit::ACTION action );
	virtual	bool	do_key(			INT32 c, bool CONST b_special, INT32* modifiers, INT32* x, INT32* y );

	virtual	bool	do_command(		C_PCHAR_C cmd );
	virtual	bool	enum_command(	o_str& o );

	virtual	bool	mouse_down(		FP32& u_start, FP32& v_start );
	virtual	void	mouse_move(		FP32 u_in, FP32 v_in );
	virtual	bool	mouse_wheel(	FP32 factor );
//	virtual	bool	mouse_cam()						{ return _b_free_cam; }

public:
			void	force_curve_update();
private:
//			void	draw_marker( INT32 u );
			void	draw_curves();
			void	draw_points();
			void	draw_points_multiple();
			void	draw_numbers();
//			void	draw_markers();
			void	draw_graduation();
			void	center_cam();
			INT32	get_constraint_axe();
			void	get_constraint_axe( INT32& i_u, INT32& i_v, INT32& i_axe );
			void	move_constraint( REAL* dst, REAL CONST * CONST src, REAL CONST * CONST tra );

			void	draw_constraint( INT32 iu, INT32 iv, INT32 ia, REAL* v );
			void	draw_constraint();
/*			void	update_markers();
			void	alloc_markers();
			void	insert_marker();
			void	delete_marker();
			void	add_marker();
*/
			void	save_curve_info( CHAR* name_info, size_t id );
			void	load_curve_info( CHAR* name_info, size_t id );

	FINLINE	void	coor_to_world_one_low( REAL* dst, REAL CONST * CONST src, c_curve_3d* CONST curve );
			bool	compute_mouse_start();

public:
			INT32	get_control_point_nb(		INT32 dataset_id	);
			void	push_control_point_back(	INT32 dataset_id,	REAL CONST * CONST src = nullptr );
			void	insert_control_point(		INT32 index,		REAL CONST * CONST src = nullptr );
			void	delete_control_point(		INT32 index			);
			void	pop_control_point_front(	INT32 dataset_id	);
			void	clear_control_points(		INT32 dataset_id	);

	virtual	void	draw_single();
	virtual	void	draw_multiple();
	virtual	void	update();


//	FINLINE	INT32	get_nb_points( INT32 dataset )				{ return _curve_info_ui[ dataset - 1 ]._nb_points; }
			//INT32	get_nb_points_equi( UINT32 dataset );
			//void	set_points_equi( UINT32 dataset, size_t nb );

			REAL	compute_len( INT32 CONST dataset );

			REAL CONST *	get_control_point(					INT32 index_u	);
			REAL CONST *	get_control_point( INT32 dataset,	INT32 index_u	);
			void			set_control_point(					INT32 index_u,	REAL CONST * CONST vec );
			void			set_control_point( INT32 dataset,	INT32 index_u,	REAL CONST * CONST vec );
			void			set_point_control_cur( INT32 iu, bool b_send_command=true );

			void	set_dataset( INT32 dataset_id );
			void	set_curve_ui( INT32 id );
			void	clear_datasets();

//			void	set_marker( INT32 index, REAL* vec, CHAR* text );
//			void	set_marker( INT32 dataset, INT32 index, REAL* vec, CHAR* text );
//			void	set_marker_cur( INT32 index, bool b_send_command = true );

//	FINLINE	void	set_color_white(			REAL alpha_in = 1.f );
//	FINLINE	void	set_color_red(				REAL alpha_in = 1.f );
//	FINLINE	void	set_color_green(			REAL alpha_in = 1.f );
//	FINLINE	void	set_color_construction(		REAL alpha_in = 1.f );
//	FINLINE	void	set_color_construction_bis(	REAL alpha_in = 1.f );
	FINLINE	void	set_color_ui(				REAL alpha_in = 1.f );
	FINLINE	void	set_color_ui_selected(		REAL alpha_in = 1.f );

			//void	get_tra( REAL* dst, INT32 dataset, UINT32 index );
			//void	get_tangent( REAL* dst, INT32 dataset, UINT32 index );
			void	get_tra(			REAL* dst,				INT32 CONST dataset_id, REAL CONST s );
			void	get_tangent(		REAL* dst,				INT32 CONST dataset_id, REAL CONST s );
			void	get_point_tangent(	REAL* dst,	REAL* tgn,	INT32 CONST dataset_id, REAL CONST s );

	virtual	bool	get_point_rnd(		REAL* CONST pos, REAL CONST t_in ) final override;

			REAL	get_len( INT32 dataset_id );
			void	coor_to_world_one(	REAL* dst,				INT32 CONST dataset_id, REAL CONST * CONST src );
			void	coor_to_world(		REAL* dst,				INT32 CONST dataset_id, REAL CONST *       src, INT32 nb );
};
