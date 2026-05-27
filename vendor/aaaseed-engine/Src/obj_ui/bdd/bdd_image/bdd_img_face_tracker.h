
#ifdef AAA_BDD_IMG_FACE_TRACKER_H
#error "BDD_IMG_FACE_TRACKER_H included more than once."
#endif
#define AAA_BDD_IMG_FACE_TRACKER_H 1


#ifndef AAA_BDD_IMG_H
#	include "obj_ui/bdd/bdd_image/bdd_img.h"
#endif

class c_face_tracker;
class c_speed;
//class c_speed_ui;


class	c_bdd_img_face_tracker final : public c_bdd_img
{
public:
private:
	FACTORY_DECLARE( c_bdd_img_face_tracker, c_bdd_img );

	INT32				_model_active_nb	{0};
	bool				_b_mark_image		{false};

	c_face_tracker*		_face_tracker		{nullptr};


	bool				_b_draw_cube_ui;
	bool				_b_draw_points_ui;
	bool				_b_draw_face_2d_ui;
	bool				_b_draw_face_3d_ui;
	bool				_b_draw_face_number_ui;
	REAL				_draw_face_number_size_ui;

	INT32				_dataset_id			{0};

	REAL				_size[3];

	REAL				_detect_resize_ui;
	INT32				_detect_size_x		{0};
	INT32				_detect_size_y		{0};

	cv::Mat_< float >	_depth;
//	cv::Mat_< uchar >	_grayscale_image;
	cv::UMat			_detect;		//	used for face detection when different from landmarking
	cv::UMat			_landmark;		//	used for face landmarking
//	cv::UMat			_mat_dst_a;

	bool				_b_keep_rgb;

public:
	static INT32 CONST	FACE_UI_NB = 4;

protected:
	c_speed*			_speed_bdd			{nullptr};
//	c_speed_ui*			_speed_bdd_ui		{nullptr};

private:	
			void	update_tracker();

			void	dealloc();

			void	draw_points(	DOUBLE* pts, INT32 nb );
			void	draw_line(		DOUBLE* pts, int start, int stop, bool b_loop );
			void	draw_landmark(	DOUBLE* pts, INT32 nb, bool b_text );

public:
			void	init();
	virtual	void	param_init_pt();

	virtual	void	update();
	virtual	void	draw();
	virtual bool	can_implicit() final override { return true; }


	FINLINE	INT32	get_face_dataset();

	virtual	INT32	get_point_dataset_nb() final override;
	virtual	bool	set_point_dataset( INT32 dataset_id )  final override;

	virtual	INT32	get_point_nb() final override;
	virtual	bool	get_point( REAL* CONST dst, INT32 CONST index ) final override;
};
