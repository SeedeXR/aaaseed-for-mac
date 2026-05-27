
#ifdef AAA_BDD_IMG_ERODE_DILATE_H
#error "BDD_IMG_ERODE_DILATE_H included more than once."
#endif
#define AAA_BDD_IMG_ERODE_DILATE_H 1


#ifndef AAA_BDD_IMG_H
#	include "bdd_img.h"
#endif

class	c_bdd_img_erode_dilate final : public c_bdd_img
{
	FACTORY_DECLARE( c_bdd_img_erode_dilate, c_bdd_img );
private:

public:

protected:
	bool		_b_dilate_first;

	bool		_b_erode_active;
	INT32		_erode_image_dst;
	c_img_ipl	_ipl_erode;
	INT32		_erode_size;

	bool		_b_dilate_active;
	INT32		_dilate_image_dst;
	c_img_ipl	_ipl_dilate;
	INT32		_dilate_size;

//	bool		_b_verbose_ui;

public:
			c_img_ipl*	do_erode( c_img_ipl* src );
			c_img_ipl*	do_dilate( c_img_ipl* src );

			void		do_process();

			void		release();

public:
			void		init();
	virtual	void		param_init_pt();

	virtual	void		update();
	virtual	void		draw();
	virtual bool		can_implicit() final override { return true; }
};
