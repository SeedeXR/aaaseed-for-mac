
#ifdef AAA_BDD_IMG_GRADIENT_H
#error "BDD_IMG_GRADIENT_H included more than once."
#endif
#define AAA_BDD_IMG_GRADIENT_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif
#ifndef	AAA_PIXEL_FORMAT_H
#	include "image/pixel_format.h"
#endif

class c_img_2d;

class	c_bdd_img_gradient final : public c_bdd
{
	FACTORY_DECLARE(c_bdd_img_gradient,c_bdd);
public:
	static INT32 CONST COLOR_INT_NB = 3;
private:
	REAL	_start;
	REAL	_stop;
	FP32	_gamma[5];
	REAL	_center[2];
	REAL	_offset_phase;
	REAL	_factor;
	REAL	_factor_v;
	REAL	_rounded[2];
	REAL	_gain;
	REAL	_bias;
	DOUBLE	_gain_fac;
	DOUBLE	_bias_fac;
	REAL	_clamp;
	bool	_b_invert;
	bool	_b_curve_sphere;
	bool	_b_interpolate_hsv;
//	INT32	_pixel_nb[2];
	FP32	_start_color[4];
	FP32	_stop_color[4];

	REAL	_color_u[COLOR_INT_NB];
	FP32	_color[COLOR_INT_NB][4];
	INT32	_color_nb;

	INT32	_s_type				{-42};
	bool	_b_force_nearest;

	REAL	_mid_point_ui;
	REAL	_mid_point;
	REAL	_mid_point_factor_a;
	REAL	_mid_point_factor_b;

//	REAL	_noise;

	INT32					_channel_nb_ui;
	GOL::INTERNAL_TYPE		_s_channel_type_ui;
	REAL	_start_ui;
	REAL	_stop_ui;
	FP32	_gamma_ui[5];
	REAL	_center_ui[2];
	REAL	_offset_phase_ui;
	REAL	_factor_ui;
	REAL	_factor_v_ui;
	REAL	_rounded_ui[2];
	REAL	_gain_ui;
	REAL	_bias_ui;
	REAL	_clamp_ui;
	bool	_b_curve_sphere_ui;
	bool	_b_invert_ui;
	bool	_b_interpolate_hsv_ui;
	INT32	_pixel_nb_ui[2];
	FP32	_start_color_ui[5];
	FP32	_stop_color_ui[5];

	bool	_b_color_int[COLOR_INT_NB];
	FP32	_color_int_u[COLOR_INT_NB];
	FP32	_color_int[COLOR_INT_NB][4];

	bool	_b_color_int_ui[COLOR_INT_NB];
	FP32	_color_int_u_ui[COLOR_INT_NB];
	FP32	_color_int_ui[COLOR_INT_NB][5];

	INT32	_s_type_ui;
	bool	_b_force_nearest_ui;
//	REAL	_noise_ui;

	INT32	_bind_last;

	bool	_b_texture_size_min;
	bool	_b_verbose;

	UINT32	_color_min_uint32;
	UINT32	_color_max_uint32;

	UINT64	_color_min_uint64;
	UINT64	_color_max_uint64;

private:
	FINLINE	REAL		compute_inter( REAL val, FP32*& a, FP32*& b );
	FINLINE REAL		make_u( INT32 iu, REAL su );
	FINLINE REAL		make_v( INT32 iv, REAL sv );
//	FINLINE	REAL		interpolate_central( INT32 index, REAL inter );
	FINLINE	DOUBLE		compute_value_first( DOUBLE val );
//	FINLINE	DOUBLE		compute_value_second( DOUBLE val );

	FINLINE	FP32*	compute_pixel_low(		DOUBLE val );
	FINLINE	UINT32		compute_pixel_uint32(	DOUBLE val );
	FINLINE	FP32*	compute_pixel_fp32(		DOUBLE val );
	FINLINE	UINT64		compute_pixel_uint64(	DOUBLE val );

	FINLINE	DOUBLE		compute_value( DOUBLE val );
	FINLINE	UINT32		compute_pixel( DOUBLE val );
	template< aaa::PIXEL_FORMAT FORMAT >
			void		compute_low( c_img_2d* img );
			void		compute(	c_img_2d* img );
public:
	void	init();
	
	virtual	void	draw();
	virtual	void	update();


	virtual	void	param_init_pt();
};
