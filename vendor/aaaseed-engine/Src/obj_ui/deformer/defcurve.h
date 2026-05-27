
#ifdef AAA_DEFCURVE_H
#error "DEFCURVE_H included more than once."
#endif
#define AAA_DEFCURVE_H 1


#ifndef AAA_DEFORMER_H
#	include "obj_ui/deformer/deformer.h"
#endif

class	c_def_curve final : public c_deformer
{
public:
	FACTORY_DECLARE(c_def_curve,c_deformer);
private:
	INT32	_s_curve_ui;
	REAL	_flip_point_ui;
	REAL	_time_factor_ui;
	REAL	_space_freq_ui;
	REAL	_strength_ui;
	REAL	_offset_ui;
	INT32	_src_axe_ui;
	INT32	_s_src_ui;
	INT32	_dst_axe_ui;
	INT32	_s_dst_ui;
	bool	_b_abs;
	REAL	_phase_offset_ui;
	REAL	_min_ui;
	REAL	_max_ui;

	REAL	_t;	//	time cached here
	REAL	_bias_ui;
	REAL	_gain_ui;

	REAL	_src_center_ui[3];
	REAL	_dst_center_ui[3];
private:
	REAL	_bias_factor;
	REAL	_gain_factor;

public:

	virtual	void	update() final override;
	virtual	void	apply( REAL CONST * src, REAL* dst, INT32 nb ) final override;
	virtual	void	param_init_pt() final override;
};

