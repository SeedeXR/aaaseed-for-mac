
#ifdef AAA_BDD_IMG_MEDIAPIPE_H
#error "BDD_IMG_MEDIAPIPE_H included more than once."
#endif
#define AAA_BDD_IMG_MEDIAPIPE_H 1


#ifndef AAA_BDD_IMG_H
#	include "obj_ui/bdd/bdd_image/bdd_img.h"
#endif


class aaa::c_cv;

class	c_bdd_img_mediapipe final : public c_bdd_img
{
	FACTORY_DECLARE( c_bdd_img_mediapipe, c_bdd_img );
public:
//	static c_bdd_img_contour*	cur;

protected:
	INT32	_dst_img_bind;
	bool	_b_dst_clear;
	bool	_b_draw_in_texture;

	INT32 _frame_detection;
	INT32 _frame_detection_no;

	o_str	_arm_left;
	o_str	_arm_right;
	o_str	_hand_left;
	o_str	_hand_right;
private:


private:
			void				do_process();
			bool				find_blob_contour( INT32 s_how, cv::UMat* dst );
			void				release();
public:
			void				init();
	virtual	void				param_init_pt();

	virtual	void				update();
	virtual	void				draw();
	virtual bool				can_implicit() final override { return true; }

//	virtual	bool				get_points_3d( REAL* dst, INT32 nb );
public:
//	virtual	INT32				get_point_nb();
};
