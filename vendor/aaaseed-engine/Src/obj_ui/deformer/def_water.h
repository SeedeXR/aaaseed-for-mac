
#ifdef AAA_DEF_WATER_H
#error "DEF_WATER_H included more than once."
#endif
#define AAA_DEF_WATER_H 1


#ifndef AAA_DEFORMER_H
#	include "obj_ui/deformer/deformer.h"
#endif
#ifndef AAA_DELTA_T_H
#	include "time/delta_t.h"
#endif
#ifndef AAA_IMG_H
#	include "image/img.h"
#endif

class	c_def_water final : public c_deformer
{
public:
	FACTORY_DECLARE( c_def_water, c_deformer );
private:
	REAL			_origin[3];
	INT32			i_src_u_;
	INT32			i_src_v_;
	INT32			i_src_axe_;
	INT32			i_dst_axe_;
	REAL			_offset;
	REAL			scale_;
	REAL			_strenght_ui;
	INT32			_img_index_ui;
	aaa::COMPO		_s_compo_ui;
	bool			_b_gradient_ui;
	bool			_b_clamped_ui;
//	REAL			normal_;	
	
	REAL			wave_speed_;
	REAL			weighting_[2];
	c_delta_t		delta_t_;

	REAL*			prev_;

	REAL*			p_w_;
	REAL*			p_wp_;
	REAL*			p_wpp_;

	REAL*			data_;
	REAL*			data_prev_;
	REAL*			data_prev_prev_;
	INT32			grid_size_x_;
	INT32			grid_size_y_;
	INT32			grid_size_x_ui_;
	INT32			grid_size_y_ui_;
	bool			_b_restart_trig_ui;
	REAL			water_strenght_;
	REAL			dampening_;
	bool			_b_update_img;
	bool			_b_method_complex;
	REAL			seuil_;
	INT32			img_dst_index_;

	REAL			maa_factor_;
	REAL			gradient_factor_;
	REAL			value_factor_;

//	INT32			sample_nb;
	c_img_2d*		img_;
	c_img_2d*		img_dst_;

	REAL			out_min_;
	REAL			out_max_;
	REAL			clamp_min_;
	REAL			clamp_max_;

	bool			_b_drop;
	REAL			drop_by_sec_;
	REAL			drop_strenght_;

	bool			_b_swap;

	void		init_water_simulation_data();
	bool		alloc_water_simulation_data( INT32 size_x, INT32 size_y );
	void		dealloc_water_simulation_data();
	REAL		conv_factor_;

protected:
			void	compute();
			void	import_image();
			void	move_to_tex();
public:
	virtual	void	update() final override;
	virtual	void	apply( REAL CONST * src, REAL* dst, INT32 nb ) final override;
	virtual	void	param_init_pt() final override;

			void	init();
};

