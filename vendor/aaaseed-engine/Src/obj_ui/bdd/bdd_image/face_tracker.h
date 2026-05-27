
#ifdef AAA_FACE_TRACKER_H
#error "FACE_TRACKER_H included more than once."
#endif
#define AAA_FACE_TRACKER_H 1


#ifndef AAA_OBJ_H
#	include "infrastructure/obj/obj.h"
#endif
#ifndef OPENCV_CORE_MAT_HPP
#	include <opencv2/core/mat.hpp>
#endif
#ifndef AAA_AAA_UTIL_H
#	include "aaa_util.h"
#endif
#ifndef AAA_V_BASE_H
#	include "math/v_base.h"
#endif
#ifndef AAA_AAA_STR_H
#	include "aaa_str.h"
#endif
#ifndef AAA_ERR_H
#	include "err.h"
#endif

class c_speed;
class c_face_engine;

class c_face_tracker final : public c_obj
{
public:
	enum TYPE : INT32
	{
		TYPE_DLIB = 0,
		TYPE_CLM,
		TYPE_NB
	};
	static C_PCHAR_C	type_str[ static_cast<INT32>( TYPE_NB ) ];


	static CONST	INT32		FACE_NB_MAX = 8;

	c_face_engine*	_engines[TYPE_NB];
	c_face_engine*	_engine				{	nullptr	};

	c_speed*	_speed_detect			{	nullptr	};
	c_speed*	_speed_landmark			{	nullptr	};

//protected:
	INT32		_detect_sx				{0};
	INT32		_detect_sy				{0};

	INT32		_face_detected_nb		{0};

	INT32		_detect_frame_skip_ui	{3};	
	INT32		_failures_max_ui;
	INT32		_detect_pyramid_level_max_ui		{32};

	INT32		_s_type_ui				{TYPE_DLIB};
	INT32		_s_type_used			{TYPE_NB};

	UINT32		_model_max_ui			{1};

	DOUBLE		_filter_ui;
	INT32		_average_nb;

	o_str		_dlib_landmark_detection_filename;

	class c_face_info
	{
	private:
			INT32		_point_nb	{0};

	static	CONST	INT32		POINT_NB_MAX = 68;	
	public:
	static	INT32 CONST AVERAGE_NB_MAX = 16;
			REAL		_tra[3];
			REAL		_rot[3];
			bool		_b_active	{false};
			REAL		_certainty;

			DOUBLE		_point_raw	[AVERAGE_NB_MAX+1][POINT_NB_MAX][2];	//	store so can always filter
			DOUBLE		_point_cano	[POINT_NB_MAX][2];	//	filtered point in image coordinates
			DOUBLE		_point		[POINT_NB_MAX][3];	//	filtered point in bdd size
			DOUBLE		_point_3d	[POINT_NB_MAX][3];	//	point_3d if face_engine produce it
		
			bool		_b_visibility	[POINT_NB_MAX];
	public:
				c_face_info();
				void	update_from_raw( DOUBLE filter, INT32 average_nb, REAL* size, bool b_invent_3d );
				AAA_ERR set_point_nb( INT32 nb );
		FINLINE	INT32	get_point_nb()	{	return _point_nb;	}
		FINLINE	void	set_point_raw( INT32 i, DOUBLE x, DOUBLE y )
		{
			if( INSIDE_MIN_MAX( i, 0, _point_nb-1 ) )
				set_v2( _point_raw[0][i], x, y );
			else
				debug_break( "bad point index" );
		}
	};

	std::vector< bool >				_active_models;
	std::vector< c_face_info >		_face_infos;

	bool	_b_cv_draw;
	bool	_b_cv_draw_tracker;  
	bool	_b_cv_draw_box;


private:
	static	 c_face_engine*	get_clm();
	static	 c_face_engine*	get_dlib( C_PCHAR_C landmark_detection_fname );
protected:
			void	begin_detect();
			void	end_detect();

public:
	c_face_tracker();
	~c_face_tracker();

			void			update_engine( int argc = 0, char** argv = nullptr );
			c_face_engine*	get_engine( INT32 s_type );
	FINLINE	c_face_engine*	get_engine()				{ return _engine; }

			void			set_size_detect( INT32 sx, INT32 sy );

			void			reset();

			void			update( cv::UMat* detect, cv::UMat* landmark, cv::Mat_< float >& img_depth );

			void			get_cube_pos_rot( INT32 i, REAL* pos, REAL* rot );

			void			store_landmark( REAL sx, REAL sy );
			void			process_landmark( REAL* size );

			bool			is_model_active_all();
			INT32			get_model_active_nb();


};

class c_face_engine : public c_obj
{
protected:
	c_face_tracker* _face_tracker	{nullptr};
	UINT32			_model_nb		{0};


public:
	virtual void	deinit() = 0;
	virtual void	init( UINT32 model_nb, int argc = 0, char** argv = nullptr ) = 0;
	virtual void	reset() = 0;

	virtual void	detect(			cv::UMat* img,	cv::Mat_< float >& img_depth )	= 0;
	virtual void	find_landmark(	cv::UMat* img,	cv::Mat_< float >& img_depth )	= 0;

	virtual void	store_landmark(	c_face_tracker::c_face_info& info, INT32 model_index, REAL size_x, REAL size_y ) = 0;
	virtual bool	is_point_3d_produced()	{ return false;	}
public:
//	c_face_engine();
//	~c_face_engine();

			void	set_tracker( c_face_tracker* ft );
//			void	gene_landmark_from_cano( REAL* size, c_face_tracker::c_face_info& info, bool b_3d );

			void	get_cube_pos_rot( INT32 i, REAL* pos, REAL* rot );

	FINLINE	INT32	get_model_nb()						{ return _model_nb; }


	virtual	bool	is_float_forced()	{ return false; }
	virtual	bool	is_float_used()		{ return false; }
	virtual	bool	is_grey_forced()	{ return false; }
	virtual	bool	is_grey_used()		{ return false; }
	virtual	bool	is_bgr_used()		{ return false; }
	virtual	bool	is_rgb_used()		{ return false; }
};

