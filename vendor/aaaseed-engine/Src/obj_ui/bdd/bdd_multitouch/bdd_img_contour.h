
#ifdef AAA_BDD_IMG_CONTOUR_H
#error "BDD_IMG_CONTOUR_H included more than once."
#endif
#define AAA_BDD_IMG_CONTOUR_H 1


#ifndef AAA_BDD_IMG_H
#	include "obj_ui/bdd/bdd_image/bdd_img.h"
#endif
#ifndef AAA_BLOB_H
#	include "blob.h"
#endif
#ifndef _VECTOR_
#	include <vector>
#endif

class aaa::c_cv;

//todo should it be a bdd_multiple like bdd_blob_base
//todo should we unify with other blob stuff
class	c_bdd_img_contour final : public c_bdd_img
{
	FACTORY_DECLARE( c_bdd_img_contour, c_bdd_img );
public:
	static c_bdd_img_contour*	cur;

protected:
	BLOBS_CONT		_blob;
	BLOBS_CONT		_blob_discarded;

	INT32			_contour_img_dst;
//	bool			_b_dst_mono;
//	bool			_b_dst_alpha;
	bool			_b_dst_clear;
	INT32			_s_contour_mode;
	REAL			_threshold;

	REAL			_center[3];
	REAL			_center_ui[3];
	REAL			_size[3];

	bool			_b_draw_in_texture;
	cv::UMat		_contour_dst;
	//c_img_ipl		_ipl_contour_draw;

	aaa::c_cv*		_aaa_cv;

	INT32			_contour_nb;
	UINT32			_contour_point_nb;
	UINT32			_contour_discarded_nb;
	bool			_b_blob_point_nb_filter;
	INT32			_blob_point_nb_min;
	INT32			_blob_point_nb_max;

	bool			_b_blob_area_filter;
	REAL			_blob_area_min;
	REAL			_blob_area_max;

	bool			_b_blob_sx_filter;
	REAL			_blob_sx_min;
	REAL			_blob_sx_max;

	bool			_b_blob_sy_filter;
	REAL			_blob_sy_min;
	REAL			_blob_sy_max;

	bool			_b_center_draw_discarded_ui;
	bool			_b_center_draw_ui;
	REAL			_center_size_ui;

	bool			_b_contour_draw_discarded_ui;
	bool			_b_contour_draw_ui;
	REAL			_contour_draw_alpha_ui;
	bool			_b_contour_draw_as_quad_ui;

	bool			_b_bounding_draw_ui;
	bool			_b_bounding_draw_ellipse_ui;
	REAL			_bounding_draw_alpha_ui;
	REAL			_bounding_draw_size_ui[3];

	bool			_b_contour_wanted;
	bool			_b_poly_approx_ui;
	REAL			_poly_approx_factor_ui;
	bool			_b_bounding_find_angle;
	bool			_b_find_y_max;
	REAL			_find_y_max_distance;

	bool			_b_fiducial;
	REAL			_fiducial_size_min;
	INT32			_fiducial_discarded_nb;
	INT32			_fiducial_kept_nb;
	bool			_b_fiducial_draw;

	o_str			_target_name_symbo;
	c_bdd*			_bdd_target;
	REAL			_transfer_translate[3];
	REAL			_transfer_scale_ui[4];

private:
	REAL			_x_factor;
	REAL			_y_factor;
	INT32			_band_nb;
//	REAL			_w_pos;

private:
			void				do_process();
			bool				find_blob_contour( INT32 s_how, cv::UMat* dst );
			//void				find_blob_contour( c_img_ipl* src, c_img_ipl* ipl_dst, INT32 s_how );
			void				release();
public:
			void				init();
	virtual	void				param_init_pt();

	virtual	void				update();
	virtual	void				draw();
	virtual bool				can_implicit() final override { return true; }

	virtual	UINT32				get_blob_nb()			{	return (UINT32)_blob.size();	}
	virtual	c_blob CONST &		get_blob_index(INT32 i)	{	return _blob[i];				}
			BLOBS_CONT CONST &	get_blobs()				{	return _blob;					}

	virtual	void				transfer_blobs_to( BLOBS_CONT& blobs );
	virtual	bool				get_points_3d( REAL* dst, INT32 nb );
public:
	virtual	INT32				get_point_nb();
};
