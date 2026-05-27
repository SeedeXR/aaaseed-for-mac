
#ifdef AAA_BDD_IMG_SMOOTH_H
#error "BDD_IMG_SMOOTH_H included more than once."
#endif
#define AAA_BDD_IMG_SMOOTH_H 1


#ifndef AAA_BDD_IMG_H
#	include "obj_ui/bdd/bdd_image/bdd_img.h"
#endif

class	c_bdd_img_smooth final : public c_bdd_img
{
	FACTORY_DECLARE( c_bdd_img_smooth, c_bdd_img );
private:
	INT32		_smooth_image_dst_ui;
	c_img_ipl	_ipl_smooth;
	bool		_b_smooth_ui;
	INT32		_s_smooth_ui;
	INT32		_smooth_size_x_ui;
	INT32		_smooth_size_y_ui;

	//bool		_b_grow;

	// Brightness filter
	bool		_b_bc_active_ui;
	INT32		_bc_image_dst_ui;
	c_img_ipl	_ipl_bc;
	REAL		_bc_min_ui;
	REAL		_bc_max_ui;
	REAL		_bc_min;
	REAL		_bc_max;
	uchar		_bc_lut[256];
	CvMat*		_bc_lutmat;

public:
	//	void	background_set_mask (void * vaPoints, int xGrid, int yGrid );
	void	bc_update_lut();
	void	do_bc(				c_img_ipl CONST * CONST src, c_img_ipl* CONST dst );
	void	do_background(		c_img_ipl CONST * CONST src, c_img_ipl* CONST dst );
	bool	do_smooth_opencv(	c_img_ipl CONST * CONST src, c_img_ipl* CONST dst );
	bool	do_smooth_maa(		c_img_ipl CONST * CONST src, c_img_ipl* CONST dst );
	bool	do_smooth(			c_img_ipl CONST * CONST src, c_img_ipl* CONST dst );
	bool	do_grow(			c_img_ipl CONST * CONST src, c_img_ipl* CONST dst );
	void	do_process();

	void	close();

public:
			void	init();
	virtual	void	param_init_pt();

	virtual	void	update();
	virtual	void	draw();
	virtual bool	can_implicit() final override { return true; }
};
