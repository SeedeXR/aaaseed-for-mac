
#ifdef AAA_BDD_LINE_3D_H
#error "BDD_LINE_3D_H included more than once."
#endif
#define AAA_BDD_LINE_3D_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif
#ifndef AAA_AAA_CONST_H
#	include "infrastructure/aaa_const.h"
#endif
#ifndef AAA_REGISTRY_GENERIC_H
#	include "infrastructure/factory/registry_generic.h"
#endif
#ifndef AAA_LINE_3D_H
#	include "draw/geo/line_3d.h"
#endif


class	c_bdd_line_3d_master final : public c_obj_ui
{
	FACTORY_DECLARE( c_bdd_line_3d_master, c_obj_ui );
//	friend	class	c_bdd_line_3d;
protected:
	
private:
	bool	_b_net_in_active				;
	bool	_b_net_in_update_continuous		;
	bool	_b_net_out_active				;
	bool	_b_verbose_receive_incoherent	;

	INT32	_net_out_blk_size_max			;
	INT32	_blk_nb_in;
	INT32	_blk_nb_out						;
	INT32	_blk_nb_in_processed			;
	INT32	_blk_nb_in_error				;


public:
	FINLINE	bool	is_net_in_active()				CONST	{ return _b_net_in_active;				}
	FINLINE	bool	is_net_in_update_continuous()	CONST	{ return _b_net_in_update_continuous;	}
	FINLINE	bool	is_net_out_active()				CONST	{ return _b_net_out_active;				}
	FINLINE	bool	is_verbose_receive_incoherent()	CONST	{ return _b_verbose_receive_incoherent; }


	FINLINE	INT32	get_net_out_blk_size_max()		CONST	{ return _net_out_blk_size_max;			}

	FINLINE	void	inc_blk_nb_in()							{ ++_blk_nb_in;				}
	FINLINE	void	inc_blk_nb_out()						{ ++_blk_nb_out;			}
	FINLINE	void	inc_blk_nb_in_processed()				{ ++_blk_nb_in_processed;	}

	virtual	void	param_init_pt();
			void	update();
};

class	c_bdd_line_3d final : public c_bdd_multiple
{
	FACTORY_DECLARE( c_bdd_line_3d, c_bdd_multiple );
public:
	static	CONST	INT32	LINE_3D_CHANNEL_NB_MAX = CHANNEL_NB_MAX;

	struct st_point_send
	{
		REAL	point[3];
		UINT16	dataset_id;
		UINT16	point_nb;
	};

	struct st_net_line_3d
	{
		bool			_b_active;
		INT32			_net_link_index;
		INT32			_net_channel_id;
		INT32			_channel_id_dst;
		INT32			_channel_id_dst_ui;
	//	INT32			_bind_dst;
	//	bool			_b_crop;
	//	bool			_b_compress;
		INT32			_blk_nb_out;
		INT32			_blk_nb_in;
		UINT64_SOON		_sent_size;

		st_point_send*	_data		{ nullptr };
		INT32			_data_nb;
		INT32			_point_nb_max_to_send;
	};

private:
	static	c_instance_by_channel< c_bdd_line_3d, LINE_3D_CHANNEL_NB_MAX > inst_by_channel;
public:
	static	c_bdd_line_3d*	get_from_channel( INT32 channel_id )	{	return inst_by_channel.get( channel_id );	}

	static	CONST	INT32	DATASET_ID_MAX			=	9999;
	static	CONST	INT32	DATASET_ID_DIGIT_MAX	=	4;
	static	c_bdd_line_3d_master* master;

private:
	INT32	_channel_id;

//	bool	_b_ui_edit;
	bool	_b_draw_selected_ui;

//	REAL	_size[3];
//	bool	_b_curve_load_save;
//	UINT32	_point_cur_ui;
//	UINT32	_point_cur;

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

	bool	_b_ui_draw_curve_ui;
	bool	_b_ui_draw_point_ui;
//	bool	_b_ui_draw_number_ui;
//	REAL	_number_scale_ui;

//	bool	_b_len_max_ui;
//	REAL	_len_max_ui;

	c_vector_server< c_line_3d >	_map_curve;
//	c_line_3d*						_curve;

//	bool	_b_curve_index_changed;

	bool	_b_erase_all_trig_ui;

	bool	_b_valid_curve_for_get_point_rnd;
	bool	_b_valid_curve_for_get_point_rnd_compute;

	st_net_line_3d	_st_send;


private:
	INT32	_dataset_id_ui;
	INT32	_dataset_id;

//	INT32	_nb_points_ui;

private:
			void	dealloc();
	//		void	alloc_curve();

private:
	FINLINE	c_line_3d* find_curve(			UINT32 id );	//	find don't recreate an object, return NULL if none	}
	FINLINE	c_line_3d* get_curve(			UINT32 id );	//	get create the object if not found
	FINLINE	c_line_3d* find_curve_valid(	UINT32 id );

private:
//	bool		_b_ui_deform;
//	FP32		_ui_alpha;

//	REAL	_dummy[3];
//	c_edit	_edit;
			void	init_send();
			void	send_helper( INT32 CONST dataset_id, REAL CONST * CONST src, c_line_3d* CONST curve );
public:
			void	init();

	virtual	void	param_init_pt();

//	virtual	AAA_ERR	load_do_after( o_str CONST & filename );
//	virtual	AAA_ERR	save_do_after( o_str CONST & filename );

private:
			void	draw_curves();
			void	draw_points();
			void	draw_points_multiple();

public:
			REAL CONST *	get_control_point(										INT32 index_u	);
			REAL CONST *	get_control_point(					INT32 dataset_id,	INT32 index_u	);
			void			set_control_point(					INT32 dataset_id,	INT32 index_u, REAL CONST * CONST vec );
			void			set_control_point(										INT32 index_u, REAL CONST * CONST vec );
			INT32			get_control_point_nb(				INT32 dataset_id	);
			void			push_control_point_back(			INT32 dataset_id,	REAL CONST * CONST src = nullptr );
			DOUBLE			push_control_point_back_len_max(	INT32 dataset_id,	REAL CONST * CONST src, REAL CONST len_max );
			void			pop_control_point_front(			INT32 dataset_id	);
			void			clear_control_points(				INT32 dataset_id	);
//			void			insert_control_point(									INT32 index,		REAL* src = nullptr );
//			void			delete_control_point(									INT32 index			);

	virtual	void	draw_single();
	virtual	void	draw_multiple();
	virtual	void	update();

//	virtual	bool	do_command( C_PCHAR_C cmd );

//	FINLINE	INT32	get_nb_points( INT32 dataset )				{ return _curve_info_ui[ dataset - 1 ]._nb_points; }
			//INT32	get_nb_points_equi( UINT32 dataset );
			//void	set_points_equi( UINT32 dataset, size_t nb );

			REAL	compute_len( INT32 dataset );

			void	set_dataset( INT32 dataset_id );
			void	set_curve_ui( INT32 id );
			void	clear_datasets();

//			void	get_tra(			REAL* dst,				INT32 dataset, UINT32 index );
//			void	get_tangent(		REAL* dst,				INT32 dataset, UINT32 index );
			void	get_tra(			REAL* dst,				INT32 CONST dataset_id, REAL CONST s );
			void	get_tangent(		REAL* dst,				INT32 CONST dataset_id, REAL CONST s );
			void	get_point_tangent(	REAL* dst,	REAL* tgn,	INT32 CONST dataset_id, REAL CONST s );

	virtual	bool	get_point_rnd(		REAL* CONST dst, REAL CONST t_in ) final override;


			DOUBLE	get_len( INT32 dataset_id );
//			void	coor_to_world_one(	REAL * dst,				INT32 CONST dataset_id, REAL CONST * CONST src );
//			void	coor_to_world(		REAL * dst,				INT32 CONST dataset_id, REAL CONST *       src, INT32 nb );
				
public:

			AAA_ERR	send( st_net_line_3d& st_send );
	static	AAA_ERR	process_blk( UINT8 CONST * CONST data, INT32 CONST len, INT32 CONST net_link_index );
			void	push_control_point_back_receive( st_point_send CONST * elt, UINT32 nb_elt );

};
