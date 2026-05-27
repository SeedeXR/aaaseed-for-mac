
#ifdef AAA_DEF_CYLINDER_H
#error "DEF_CYLINDER_H included more than once."
#endif
#define AAA_DEF_CYLINDER_H 1


#ifndef AAA_DEFORMER_H
#	include "obj_ui/deformer/deformer.h"
#endif

class	c_def_tocylinder final : public c_deformer
{
public:
	FACTORY_DECLARE( c_def_tocylinder, c_deformer );
private:
	static CONSTEXPR INT32 SIDE_NB_MAX = 32;

	struct st_pt_info {
		REAL pos[3];
		REAL nor[3];
	};

	st_pt_info	pts[SIDE_NB_MAX+1]; 

	REAL	_def_amount;

	INT32	_axe_src;
	INT32	_i_u_src;
	INT32	_i_v_src;

	INT32	_axe_dst;
	INT32	_i_u_dst;
	INT32	_i_v_dst;

	bool	_b_flip_uv;

	REAL	_u_offset;
	REAL	_u_factor;
	REAL	_v_factor;
	REAL	_v_offset;

	bool	_b_final;
	REAL	_radius;
	REAL	_cone_angle;

	bool	_b_side;
	bool	_b_side_ui;
	REAL	_side_amount;
	INT32	_side_nb;

	bool	_b_spiral;
	REAL	_spiral_factor_ui;
	REAL	_spiral_factor;

	REAL	_lissajous_ratio;
	REAL	_size_ratio;

	void	apply_no_side(	REAL CONST * src, REAL* dst, INT32 nb );
	void	apply_side(		REAL CONST * src, REAL* dst, INT32 nb );

public:
	virtual	void	update() final override;
	virtual	void	apply( REAL CONST * src, REAL* dst, INT32 nb ) final override;
	virtual	void	param_init_pt() final override;
			void	init();
};

