
#ifdef AAA_FN_3D_H
#error "FN_3D_H included more than once."
#endif
#define AAA_FN_3D_H 1


#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif

class	c_fn_3d final : public c_obj_ui
{
	FACTORY_DECLARE(c_fn_3d,c_obj_ui);
public:
	INT32	type;

	INT32	dim_ui;
	INT32	dim;

	INT32	axe;
	INT32	axe_u;
	INT32	axe_v;

	INT32	method_ui;
	INT32	method;

	REAL	offset[3];
	REAL	offset_ui[3];

	REAL	freq[3];
	REAL	freq_ui[3];

	bool	b_abs[3];
	bool	b_squared[3];
	REAL	mod[3];
	REAL	mod_offset_ui[3];
	REAL	mod_offset[3];

	REAL	freq_factor;
	INT32	harm;

	bool	b_output_clamp;
	REAL	_gain;
	REAL	_bias;

	REAL	_gain_factor;
	REAL	_bias_factor;

	INT32	_s_output_curve_ui;
	REAL	output_freq;
	REAL	output_phase;
	REAL	output_min;
	REAL	output_max;

	REAL	output_center;
	REAL	output_factor;

	REAL	scaling_factor;

	REAL	(*norm)(REAL*);

private:
protected:
public:

	FINLINE	void	alloc() {};
	FINLINE	void	dealloc() {};

			void	update();
	virtual	void	param_init_pt();

			REAL	compute( REAL * where);
};


