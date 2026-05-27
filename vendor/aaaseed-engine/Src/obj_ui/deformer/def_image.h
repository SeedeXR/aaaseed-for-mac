
#ifdef AAA_DEF_IMAGE_H
#error "DEF_IMAGE_H included more than once."
#endif
#define AAA_DEF_IMAGE_H 1


#ifndef AAA_DEFORMER_H
#	include "obj_ui/deformer/deformer.h"
#endif
#ifndef AAA_IMG_H
#	include "image/img.h"
#endif

class	c_def_image final : public c_deformer
{
public:
	FACTORY_DECLARE(c_def_image,c_deformer);
private:
	INT32		_i_src_u;
	INT32		_i_src_v;
	INT32		_i_src_axe;
	INT32		_i_dst_axe;
	REAL		_offset;
	bool		_b_scale_by_model;
	REAL		_scale[ 2 ];
	REAL		_scale_ui[ 3 ];
	bool		_b_origin_at_image_center;
	REAL		_origin[ 3 ];
	REAL		_rotation;
	REAL		_strenght;
	INT32		_img_index;
	bool		_b_use_rgb;
	aaa::COMPO	_s_compo;
	bool		_b_gradient;
	bool		_b_linear_ui;
	bool		_b_clamped_ui;
	bool		_b_white_multiply;
//	bool		_b_parallel_ui;
	REAL		_normal;
	INT32		_ellipse_nb;
	bool		_b_ellipse;
	REAL		_r_ui[ 3 ];
	REAL		_r[ 2 ];

	c_img_2d*	_img;
private:
	template <bool B_ROT>
			void	apply_private( REAL CONST * src, REAL* dst, INT32 nb );
public:
	virtual	void	update() final override;
	virtual	void	apply( REAL CONST * src, REAL* dst, INT32 nb ) final override;
	virtual	void	param_init_pt() final override;

			void	init();
};

