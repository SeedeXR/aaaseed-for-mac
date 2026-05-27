
#ifdef AAA_BDD_TUBE_PATH_H
#error "BDD_TUBE_PATH_H included more than once."
#endif
#define AAA_BDD_TUBE_PATH_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif

class	c_circle;
class	c_ring_attrib;
class	c_curve_3d;
class	c_bdd_mocap;
class	c_bdd_tube_path final : public c_bdd_multiple 
{
	FACTORY_DECLARE( c_bdd_tube_path, c_bdd_multiple );
private:
	REAL*			_uv;
	c_circle*		_rings;
	c_ring_attrib*	_rings_attrib;

	bool			_b_curve3d;
	bool			_b_curve3d_ui;
	c_curve_3d*		_curve;
	c_bdd_mocap*	_bdd_mocap;

	REAL			_len;

	REAL			_len_before;
	REAL			_center;
	REAL			_len_after;

	REAL			_s_begin;
	REAL			_s_end;

	INT32			_s_draw;
	bool			_b_loop;
	INT32			_channel_id_ui;
	INT32			_channel_id;
	INT32			_node_id_ui;
	INT32			_node_id;

	bool			_b_ring_open;
	REAL			_angle_offset;
	REAL			_angle_range;
	bool			_b_ring_open_ui;
	REAL			_angle_offset_ui;
	REAL			_angle_range_ui;
	REAL			_angle_range_asked;

	bool			_b_ring_nb_fix;
	UINT32			_ring_nb_ui;
	INT32			_ring_nb;
	REAL			_ring_len;
	REAL			_ring_ds;
	INT32			_ring_nb_drawn;

	INT32			_ring_segment_nb_ui;
	INT32			_ring_point_nb_asked;
	INT32			_ring_point_nb;

	INT32			_ring_start;
	INT32			_ring_center;
	INT32			_ring_stop;
	bool			_b_build_needed;
	bool			_b_compute_normal_needed;

	bool 			_b_map_refresh;
	REAL			_map_factor_u;
	REAL			_map_factor_v;

	bool			_b_force_rebuild;

	REAL			_scale[2];

	bool			_b_deform_last;
	bool			_b_allow_marker_fn;

	INT32			_bind_def;

	FINLINE	INT32		build_ring_index( INT32 index );
	FINLINE	REAL		get_ring_ds();

	FINLINE	c_circle*	get_ring_direct( INT32 ii );
	FINLINE	c_circle*	get_ring( INT32 index );

	FINLINE	c_ring_attrib*	get_ring_attrib_direct( INT32 ii );
	FINLINE	c_ring_attrib*	get_ring_attrib( INT32 index );
	FINLINE	void			get_ring_and_attrib( INT32 index, c_circle*& ring, c_ring_attrib*& att );

	FINLINE	bool		is_section_allowed_direct( INT32 ii );
	FINLINE	bool		is_section_allowed( INT32 index );

	
			void	draw_section_one_old_school_uv( INT32 sec_cur, INT32 sec_next );
			void	draw_section_one_old_school( INT32 sec_cur, INT32 sec_next );
	template<bool b_uv>
			void	draw_section_one(	INT32 sec_cur,		INT32 sec_next );
	FINLINE	void	deal_with_marker_direct( c_ring_attrib* att, INT32 ring_index );

	template<bool b_uv>
			void	draw_section(		INT32 sec_start,	INT32 sec_stop );

			void	update_rings();
			void	deform_rings();

			void	set_scale( REAL* a );

public:
			void	init();
			void	alloc(); 
			void	dealloc_rings();
			void	dealloc_uv();
			void	dealloc();

	virtual bool	can_implicit() final override { return true; }
	virtual	void	draw_single();
	virtual	void	draw_multiple();
	virtual	void	update();
//
//	RING
//
//			void	init_ring();
			void	init_ring(				INT32 index );
			void	init_ring_all();

			REAL*	get_ring_center(		INT32 index );
			REAL*	get_ring_normal(		INT32 index );

			void	update_ring_angle_range(	c_circle*	ring,	REAL angle_range	);
			void	update_ring_angle_offset(	c_circle*	ring,	REAL angle_offset	);
			void	update_ring_radius(			c_circle*	ring,	REAL* radius		);

			void	set_ring_center_offset(	INT32 index, REAL o_u, REAL o_v, REAL o_axe=0. );
			void	get_ring_center_offset(	INT32 index, REAL& ru, REAL& rv );
			void	set_ring_radius(		INT32 index, REAL ru, REAL rv );
			void	get_ring_radius(		INT32 index, REAL& ru, REAL& rv );

	
			void	set_ring_angle_offset(	INT32 index, REAL angle_offset );
			void	set_ring_angle_range(	INT32 index, REAL angle_range );
			void	set_ring_enabled(		INT32 index, bool b );
			void	set_ring_marker(		INT32 index, C_PCHAR fn_name, INT32 id );

			void	set_ring_texture(		INT32 index, INT32 tex_index );
			void	set_ring_uv(			INT32 index, REAL u_min, REAL u_max, REAL v_min, REAL v_max, REAL v_min_end, REAL v_max_end );

			INT32	get_ring_nb()			{ return _ring_nb; }
			void	get_ring_from_s(		REAL s, INT32& c, INT32& n, REAL& inter );

			void	build_normal();
			void	build();

	virtual	void	param_init_pt();

			void	draw_line();
			void	draw_ring_line();
			void	draw_ring_polygon();
			void	draw_ring_tri();

//			REAL*	get_uv( INT32 a );
			void	draw_tube();

	FINLINE	void	draw_ring_multiple_one( INT32 sec_cur,		INT32 sec_next );
			void	draw_ring_multiple(		INT32 sec_start,	INT32 sec_stop );

			void	draw_ring_normal_one(	INT32 sec_index );
			void	draw_normal_point(		INT32 sec_start,	INT32 sec_stop );

	virtual	void	draw_normal_point( REAL len );
	virtual	void	draw_normal_face( REAL len );

	virtual void	save_obj_file( FILE* file );

	FINLINE	REAL	get_center()		{ return _center;		}
	FINLINE	REAL	get_len()			{ return _len;			}
	FINLINE	REAL	get_len_before()	{ return _len_before;	}
	FINLINE	REAL	get_len_after()		{ return _len_after;	}
};
