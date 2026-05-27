
#ifdef AAA_BDD_UV_H
#error "BDD_UV_H included more than once."
#endif
#define AAA_BDD_UV_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif
#ifndef AAA_MAP_H
#	include "draw/map.h"
#endif

class	c_bdd_uv : public c_bdd_multiple 
{
	FACTORY_ABSTRACT_DECLARE( c_bdd_uv, c_bdd_multiple );
public:
	static CONSTEXPR	INT32	MAX_ELT_NB = 32 * 1024 * 1024;

	enum class DRAW_MODE : INT32
	{
		REGULAR = 0,
		POINT,
		LINE_U,
		MAX_NB
	};
	static CONSTEXPR C_PCHAR_C	draw_mode_str[INT32(DRAW_MODE::MAX_NB)]
	{
		"Regular",
		"Point",
		"Line_U",
	};

//#define PARAM_DEF_BDD_UV_NB()	\
//	PARAM_DEF_INT32(		nb_u,		2, 16,	1, BDD_UV_MAX_ELT_NB )	\
//	PARAM_DEF_INT32(		nb_v,		2, 16,	1, BDD_UV_MAX_ELT_NB )
//#define PARAM_DEF_BDD_UV_NB_AXE()	\
//	PARAM_DEF_BDD_UV_NB	\
//	PARAM_DEF_INT32(		nb_axe,		2, 1,	1, BDD_UV_MAX_ELT_NB )

	static CONSTEXPR	INT32	PARAM_UV_NB = 5;
#define PARAM_DEF_BDD_UV()\
	PARAM_DEF_SYMBO_PSTR(	draw_mode,	1,0,	c_bdd_uv::draw_mode_str	)\
	PARAM_DEF_REAL_ZERO(	u_center	)\
	PARAM_DEF_REAL_ONE(		u_range		)\
	PARAM_DEF_REAL_ZERO(	v_center	)\
	PARAM_DEF_REAL_ONE(		v_range		)

	static CONSTEXPR	INT32	PARAM_UV_TURN_NB = PARAM_UV_NB + 1;
#define PARAM_DEF_BDD_UV_TURN()	\
	PARAM_DEF_BDD_UV()\
	PARAM_DEF_REAL_ZERO(	turn		)

private:
	INT32		_nb_point_allocated;
	INT32		_nb_index_hexa_allocated;
protected:
	INT32		_nb_u;
	INT32		_nb_v;

	INT32		_i_u;
	INT32		_i_v;
	INT32		_i_axe;
	bool		_b_axe_logic_vert;

	REAL		_center_ui[3];

	INT32		_nb_u_ui;
	INT32		_nb_v_ui;
	INT32		_nb_axe_ui;

	INT32		_i_axe_geo;
	REAL		_center_geo[3];
	REAL		_size_geo[3];
	INT32		_nb_u_geo;
	INT32		_nb_v_geo;

	REAL		_u_center_ui;
	REAL		_u_range_ui;
	REAL		_v_center_ui;
	REAL		_v_range_ui;
	REAL		_turn_ui;		//	thanks to Gavri for the idea

	REAL		_u_center_geo;
	REAL		_u_range_geo;
	REAL		_v_center_geo;
	REAL		_v_range_geo;
	REAL		_turn_geo;

	REAL*		_point;
	REAL*		_point_def;
	REAL*		_point_to_draw;
	REAL*		_uv;
	REAL*		_normal;
	REAL*		_normal_def;
	REAL*		_normal_to_draw;
private:
	UINT32*		_index_strip;
	UINT32*		_index_hexa;
	DRAW_MODE	_draw_mode_ui;
protected:
//	bool		_b_allow_along_u_ui;
	bool		_b_field_ui;

//	INT32		_net_channel_ui;	//todo extend, was only exposed in bdd_sphere

	c_map_last	_map_last;
	bool		_b_normal_compute_needed;
	bool		_b_normal_compute_done;
private:
			void	alloc_geo(); 
			void	dealloc_geo();

			void	draw_single_low_old_school();
			void	draw_single_low();
			void	draw_single_field();
			void	draw_multiple_low();
			void	draw_multiple_field();

protected:
	void	update_bdd_uv();

	void	param_set_pt_center(	INT32& h );
	void	param_set_pt_nb(		INT32& h );
	void	param_set_pt_nb_axe(	INT32& h );
	void	param_set_pt_uv(		INT32& h );
	void	param_set_pt_uv_turn(	INT32& h );

	void	do_deform_and_normal();

	template <typename T>
	FINLINE	void	build_u_du( T& u, T& du )
	{
		u =  _u_center_ui - _u_range_ui * T(.5);
		du = _u_range_ui * OVER_ONE_AS_FP32( _nb_u_ui - 1 );
	}
	template <typename T>
	FINLINE	void	build_umin_umax_du( T& u_min, T& u_max, T& du )
	{
		build_u_du( u_min, du );
		u_max = _u_center_ui + _u_range_ui * T(.5);
	}
	template <typename T>
	FINLINE	void	build_v_dv( T& v, T& dv )
	{
		v = _v_center_ui - _v_range_ui * T(.5);
		dv = _v_range_ui * OVER_ONE_AS_FP32( _nb_v_ui - 1 );
	}
	template <typename T>
	FINLINE	void	build_vmin_vmax_dv( T& v_min, T& v_max, T& dv )
	{
		build_v_dv( v_min, dv );
		v_max = _v_center_ui + _v_range_ui * T(.5);
	}

public:
	virtual	void	build_geo_validate();
	virtual	bool	build_geo_is_needed();

	virtual bool	can_implicit() final override { return true; }

			REAL*	compute_normals();
	virtual	void	draw_normal_point( REAL len );

	virtual	void	draw_multiple();
	virtual	void	draw_single();
	virtual	void	update();

	virtual	INT32	get_point_nb() final override;
	virtual	REAL*	get_points() final override;
	virtual	REAL*	get_point_pt(	INT32 CONST index ) final override;
	virtual bool	set_point(		INT32 CONST index, REAL CONST * CONST src ) final override;

	virtual	void	get_point_from_uv( REAL* dst, REAL u, REAL v );	//	bdd_grid_adjustable have his own and Maa only need this one for now
	virtual	void	generate_uv_from_pos( REAL* uv, INT32 CONST nb_u, INT32 nb_v, INT32 CONST dataset_id );

	virtual	void	build_geo() = 0;

	virtual	void	param_init();
};

class	c_bdd_uvw : public c_bdd_uv
{
	FACTORY_ABSTRACT_DECLARE( c_bdd_uvw, c_bdd_uv );

protected:
	INT32	_nb_axe_allocated;
	INT32	_nb_axe_ui;

	REAL	_size[3];

private:
			void	set_axe_coor( REAL coor );

public:
			void	draw_instanced();
			void	update_uvw();

	virtual	void	draw();
};
