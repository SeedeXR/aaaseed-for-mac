
#ifdef AAA_WAVE_H
#error "WAVE_H included more than once."
#endif
#define AAA_WAVE_H 1


#ifndef	AAA_DEFORMER_H
#	include "obj_ui/deformer/deformer.h"
#endif

class	c_fn1d_fm;
class	c_fn1d_phase_pertub;

class	c_def_wave final : public c_deformer
{
	FACTORY_DECLARE(c_def_wave,c_deformer);

	//	param directly in the interface or focus
	REAL	_param_len;		
	REAL	_param_height;	//	in % of the len
	REAL	_param_shape;	//	from flat to sharp
	REAL	_param_wind;		//	wind factor
	REAL	_param_phase;

	bool	_b_pertub_on;
	REAL	_pertub_x;
	REAL	_pertub_z;
	REAL	_pertub_factor;
	//	more pertub param

	INT32	_pertub_resolution;
	REAL	_pertub_fractal_dim;
	REAL	_pertub_gain;
	REAL	_pertub_bias;
	INT32	_pertub_seed;
	
	bool	_b_regul;
	REAL	_regul_max;
	REAL	_regul_height;
	REAL	_regul_shape;
	//	more regul param
	bool	_b_regul_smooth;
	INT32	_regul_wave_nb;
	REAL	_regul_gain;
	REAL	_regul_bias;
	INT32	_regul_seed;
	
	//	turbulence for height param
	bool	_b_turb;
	REAL	_turb_x_ui;
	REAL	_turb_phase_ui;
	REAL	_turb_change_ui;
	REAL	_turb_strenght;
	INT32	_turb_harm;
	INT32	_turb_seed;

	REAL	_turb_x;
	REAL	_turb_phase;
	REAL	_turb_change;

	REAL	_turb_gain;
	REAL	_turb_bias;

	REAL	_turb_gain_factor;
	REAL	_turb_bias_factor;
	REAL	_f_turb[3];

	REAL	_ry;		//	real wave height
	REAL	_rz;
	REAL	_wind;		//	wind factor
	REAL	_phase_cur;

#ifndef	MAX_PLUGIN
	REAL	_period;
	REAL	_param_angle;		//	angle
	REAL	_the_cos;		//	cos of angle
	REAL	_the_sin;		//	sin of angle
	REAL	_the_cos_time_k;		//	cos of angle * k
	REAL	_the_sin_time_k;		//	sin of angle * k
	REAL	_the_cos_per_x;		//	cos of angle scaled for perturbation along x
	REAL	_the_sin_per_x;		//	sin of angle scaled for perturbation along x
#else
	REAL	param_flex;
	REAL	k;		//	1./len
	REAL	per_x;	//	perturbation along x	
#endif
	bool	_b_propagate;

	c_fn1d_fm*				_pertub;
	c_fn1d_phase_pertub*	_regul;
	REAL*	_phase_table;
	REAL*	_ry_table;
	REAL*	_rz_table;

	REAL	_frequency;
	REAL	_f_sample_nb;
	INT32	_i_sample_nb;
//	used to recompute the phase only when needed
	REAL	_len_last;
	REAL	_height_last;
	REAL	_shape_last;
	REAL	_regul_height_last;
	REAL	_regul_shape_last;
	bool	_b_regul_smooth_last;

public:

	void	init();
	void	compute_phase();
	
	REAL	height_change_get();
	REAL	phase_get( REAL f_in );

	virtual	void	update() final override;
	virtual	void	apply( REAL CONST * src, REAL* dst, INT32 nb ) final override;
#ifndef	MAX_PLUGIN	// year ago it was a 3dmax plugin
	virtual	void	param_init_pt() final override;
#else
	void	set_all_values(REAL &p_len, REAL &p_height, REAL &p_shape, REAL &p_wind, 
					REAL &p_phase, INT32 active, REAL &p_pertub_factor, 
					REAL &p_pertub_x, REAL &p_pertub_z, INT32 p_pertub_state,
					INT32 p_pertub_resolution, REAL &p_pertub_fractal_dim, 
					REAL &p_pertub_gain, REAL &p_pertub_bias, INT32 p_pertub_seed,
					REAL &p_regul_max, REAL &p_regul_height, REAL &p_regul_shape,
					INT32 p_regul_state, INT32 p_regul_wave_nb, REAL &p_regul_gain,
					REAL &p_regul_bias, INT32 p_regul_seed, REAL &p_flex,
					INT32 p_turb_state, REAL &p_turb_strenght,REAL &p_turb_x,
					REAL &p_turb_phase, REAL &p_turb_change,  INT32	p_turb_harm,
					REAL &p_turb_bias, REAL &p_turb_gain );
	char *	name();
#endif

	void	draw_curve();
	void	draw_phase_in_rect(REAL x, REAL y, REAL w, REAL h);
	void	draw_phase_break_in_rect(REAL x, REAL y, REAL w, REAL h);
	void	draw_interpo(REAL x, REAL y, REAL w, REAL h);
};

/*
#define	PARAM_WAVE_LEN		0
#define	PARAM_WAVE_HEIGHT	1
#define	PARAM_WAVE_SHAPE	2
#define	PARAM_WAVE_WIND		3
#define	PARAM_WAVE_ORIGIN	4
#define	PARAM_WAVE_ANGLE	5
#define	PARAM_WAVE_PERIOD	6
#define	PARAM_WAVE_TIME		7
#define	PARAM_WAVE_PERTUB	8
#define	PARAM_WAVE_REGUL	9
#define	PARAM_WAVE_UPDATE	10
#define	PARAM_WAVE_ACTIVE	11

#define	PARAM_WAVE8LAST		12

*/

