
#ifdef AAA_BDD_IMG_H
#error "BDD_IMG_H included more than once."
#endif
#define AAA_BDD_IMG_H 1


#ifndef	AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif
#ifndef AAA_OPEN_CV_IMAGE_H
#	include "wrapper/open_cv_image.h"
#endif
#ifndef AAA_IMG_H
#	include "image/img.h"
#endif

//todo consider bdd_img et bdd_multiple

class	c_bdd_img_base : public c_bdd
{
	FACTORY_ABSTRACT_DECLARE( c_bdd_img_base, c_bdd );
private:
	INT32			_process_index				{-1};
	INT32			_src_img_index_ui			{0};
	UINT32			_unique_id_last				{0};

protected:
	FINLINE void	inc_process_index()						{	++_process_index;					}
			bool	process_img_changed( c_img_2d* img );

	FINLINE INT32	get_process_index()			CONST		{	return _process_index;				}
	FINLINE INT32*	get_process_index_pt()					{	return &_process_index;				}
	FINLINE INT32	get_src_img_index_ui()		CONST		{	return _src_img_index_ui;			}
	FINLINE INT32*	get_src_img_index_ui_pt()				{	return &_src_img_index_ui;			}

	FINLINE UINT32&	get_unique_id_last_ref()				{	return _unique_id_last;				}
	FINLINE void	set_unique_id_last( UINT32 CONST id )	{	_unique_id_last = id;				}
};

class c_pbo;
class c_opencv_pass;

//todo bdd_img< nb_src, nb_dst, b_resize >
class	c_bdd_img : public c_bdd_img_base
{
	friend c_opencv_pass;
	FACTORY_ABSTRACT_DECLARE( c_bdd_img, c_bdd_img_base );

public:
	static	C_PCHAR_C		str_img_base[2];
#define	PARAM_DEF_IMG_DST( name )	PARAM_DEF_BASE_STR(	TYPE_SYMBO_NEG,	name,	0, -1,	-1, PARAM_MAX_INT32,	c_bdd_img::str_img_base )
#define	PARAM_DEF_IMG_DST2( name )	PARAM_DEF_BASE_STR(	TYPE_SYMBO_NEG,	name,	-1, -2,	-2, PARAM_MAX_INT32,	c_bdd_img::str_img_base )
	enum CV_ROTATION : INT32
	{
		CV_ROT_NONE = 0,
		CV_ROT_90,
		CV_ROT_180,
		CV_ROT_270,
		CV_ROT_MAX
	};
	enum CV_RESIZE_SRC : INT32
	{
		RESIZE_SRC_NONE = 0,
		RESIZE_SRC_A,
		RESIZE_SRC_B,
		RESIZE_SRC_MAX
	};
	static	C_PCHAR_C	str_rotation[CV_ROT_MAX];
	static	C_PCHAR_C	str_src_2_resize[RESIZE_SRC_MAX];

	static INT32 CONST RESIZE_INTER_MAX = 5;
	static C_PCHAR_C	str_interpolation_mode[RESIZE_INTER_MAX];

protected:
	static	c_bdd_img*	bdd_img_cur;
	
	static	bool		check_src_dst( c_img_ipl CONST * CONST src, c_img_ipl CONST * CONST dst )		{	return src->get_ipl() && dst->get_ipl();	}

private:
	c_img_ipl			_ipl_in;
	c_img_ipl			_ipl_mono;

	bool				_b_rotate_ui;
	UINT32				_rotate_angle_ui;
	cv::UMat			_rotated;
	cv::UMat			_flipped;

	bool				_b_crop;
	bool				_b_crop_ui;
	REAL				_crop_left_ui;
	REAL				_crop_right_ui;
	REAL				_crop_bottom_ui;
	REAL				_crop_top_ui;
	INT32				_crop_size[2];
	cv::UMat			_cropped;

	bool				_b_resize;
	bool				_b_resize_ui;
	INT32				_s_resize_inter_ui;
	REAL				_resize_factor[2];
	cv::UMat			_resized;

	cv::UMat			_resized_b;
	INT32				_resize_size[2];

	cv::UMat			_mono;
	cv::UMat			_mono_sub;
	cv::UMat			_mono_b;
	cv::UMat			_float;

#if AAA_LIB_USE_OPENCV_CUDA()
	cv::cuda::GpuMat	_rotated_gpu;
	cv::cuda::GpuMat	_flipped_gpu;
	//cv::cuda::GpuMat	_cropped_gpu;
	cv::cuda::GpuMat	_resized_gpu;
	cv::cuda::GpuMat	_resized_b_gpu;
	cv::cuda::GpuMat	_mono_gpu;
	cv::cuda::GpuMat	_mono_b_gpu;
	cv::cuda::GpuMat	_float_gpu;
	std::vector<cv::cuda::GpuMat>	_rgb_split_gpu;
	std::vector<cv::cuda::GpuMat>	_rgb_split_b_gpu;
#endif


//	std::vector<cv::UMat>			_rgb_split;
//	cv::UMat						_rgb_extract;

//	std::vector<cv::UMat>			_rgb_split_b;

	bool				_b_init_start_long;
#if AAA_LIB_USE_OPENCV_CUDA()
	bool	convert_to_mono_gpu(	cv::cuda::GpuMat*&	src,	aaa::PIXEL_FORMAT pf,	aaa::COMPO  compo, cv::cuda::GpuMat*	dst, std::vector<cv::cuda::GpuMat>*	split );
#endif
	bool	convert_to_mono(		cv::UMat*&			src,	aaa::PIXEL_FORMAT pf,	aaa::COMPO  compo, cv::UMat*			dst	);

	bool				_b_size_changed;

protected:
	c_img_2d*			_img_start;

	cv::UMat*			_src_cur;
	cv::UMat			_src_umat;
//	cv::UMat*			_dst_cur;
#if AAA_LIB_USE_OPENCV_CUDA()
	cv::cuda::GpuMat*	_src_cur_gpu;
	cv::cuda::GpuMat	_src_gpumat;
//	cv::cuda::GpuMat*	_dst_cur_gpu		{};	//todo maa say : not clean
#endif
//todo
#if AAA_OPENCV_GL_USE()
	cv::ogl::Texture2D*	_texture_in;
#endif
//	cv::ogl::Texture2D*	_texture_out;

	bool				_b_src_is_gpu;

	bool				_b_src_bgr;

	// For 2 sources : Mix, Stereo, Calibration
	bool				_b_src_b;
	bool				_b_src_b_bgr;
	cv::UMat*			_src_b_cur;
	cv::UMat			_src_b_umat;
#if AAA_LIB_USE_OPENCV_CUDA()
	cv::cuda::GpuMat*	_src_b_cur_gpu;
	cv::cuda::GpuMat	_src_b_gpumat;
#endif
	INT32				_src_b_img_index;
	INT32				_src_b_size[2];
	INT32				_src_b_size_last[2];

	//bool				_b_float_b_ui;
	//bool				_b_float_b;

	//bool				_b_mono_b_ui;
	//bool				_b_mono_b;


	// Resize sources if different
	UINT32				_src_2_resize_ui;

	// Swap src
	bool				_b_swap_src_ui;

	//	these 2 should be private but there are not due to c_bdd_opencl_img
	bool				_b_content_changed;

	UINT32				_unique_id_last_b;

	bool				_b_float_ui;
	bool				_b_float;

	bool				_b_mono_ui;		
	bool				_b_mono;
	bool				_b_cuda_use_ui;
	bool				_b_opencl_ui;
	bool				_b_opengl_tex_ui;
	REAL				_mono_compo_factor_1_ui;
	REAL				_mono_compo_factor_2_ui;
	REAL				_mono_compo_offset_ui;
	bool				_b_mono_equalize_histogram_ui;

	bool				_b_always_update_ui;	//todo use this until we have a clean mechanism to detect change of params 
	bool				_b_verbose_ui;

	bool				_b_src_img_prev_ui;
	c_img_ipl*			_p_in_to_use;
	aaa::PIXEL_FORMAT	_src_pixel_format;
	aaa::PIXEL_FORMAT	_pixel_format_out;
	o_str				_o_src_pixel_format;
	o_str				_o_pixel_format_out;
	INT32				_src_size_ui[2];
	INT32				_src_size[2];
	INT32				_src_size_last[2];

	aaa::COMPO			_s_compo_ui;
	aaa::COMPO			_s_compo_b_ui;

	INT32				_dst_img_index_ui;

	bool				_b_keep_data_ui;
	INT32				_dst_size_out[2];

	c_bdd_img*			_prev;

private:
			INT32			param_init_pt_start();
			void			param_init_pt_src(		INT32& h, bool b_src_2, bool b_resize );
			void			param_init_pt_src_b(	INT32& h );
			void			param_init_pt_resize(	INT32& h );

			bool			update_mat_src_private( bool& b_change_it );
			bool			update_mat_src_2_private( bool b_size_equal );
protected:
			void			update_img_dst( c_img_2d* img, INT32 index );
			INT32			build_dst_index( INT32 index );
			void			force_mono();		

			void			validate_dst_change( c_img_2d* img );
			void			validate_dst_change( INT32 index );
			
	virtual	bool			update_img_src();
			void			release_img_src();

			INT32			param_init_pt_start_src_resize( bool b_src_2, bool b_resize );

	FINLINE	c_img_ipl*		get_in_to_use()				{	return _p_in_to_use;					}
			c_img_ipl*		get_in_mono_to_use();	
	FINLINE	ipl::p_ipl		get_in_ipl_to_use()			{	return get_in_to_use()->get_ipl();		}
	FINLINE	ipl::p_ipl		get_in_ipl_mono_to_use()	{	return get_in_mono_to_use()->get_ipl();	}

			c_img_2d*		update_part_low( INT32 src_img_index, UINT32& unique_id, INT32& src_size_x, INT32& src_size_x_last, INT32& src_size_y, INT32& src_size_y_last );
			c_img_2d*		update_part_1();
			bool			update_mat_src( bool& b_change_it );
			bool			update_mat_src_2( bool b_size_equal = true );
		//	bool			update_mat_low( c_img_2d* img );

		//	bool			build_cv_mat(		cv::Mat& mat,	UINT32 img_index, cv::UMat& mat_ref, CHAR* mess, aaa::PIXEL_TYPE type = aaa::PIXEL_TYPE::UNDEFINED );
		//	bool			build_cv_mat(		cv::UMat& mat,	UINT32 img_index, cv::UMat& mat_ref, CHAR* mess, aaa::PIXEL_FORMAT type = aaa::PIXEL_FORMAT::UNKNOWN );
		//	bool			build_cv_mat(		cv::UMat& mat,	UINT32 img_index, UINT32 sx, UINT32 sy, CHAR* mess, aaa::PIXEL_FORMAT type );

template< class MAT >	// handle MAT and cv::cuda::GpuMat
			bool			is_mat_equal(		MAT* mat_a,	MAT* mat_b );

			//void			build_dst_index(	cv::UMat& dst,				INT32 index, C_PCHAR_C mess );
			void			update_dst_index(	cv::UMat& mat,				INT32 index_dst, C_PCHAR_C mess, bool CONST b_bgr, bool CONST b_move_to_gpu = false, c_pbo CONST * pbo = nullptr );
#if AAA_LIB_USE_OPENCV_CUDA()																																			    
			void			update_dst_index(	cv::cuda::GpuMat& gpu_mat,	INT32 index_dst, C_PCHAR_C mess, bool CONST b_bgr, bool CONST b_move_to_gpu = false, c_pbo CONST * pbo = nullptr );
#endif																																									    
			void			update_dst_index(	cv::Mat& mat,				INT32 index_dst, C_PCHAR_C mess, bool CONST b_bgr, bool CONST b_move_to_gpu = false, c_pbo CONST * pbo = nullptr );
			void			update_dst_index(								INT32 index_dst, C_PCHAR_C mess, bool CONST b_bgr, bool CONST b_move_to_gpu = false, c_pbo CONST * pbo = nullptr );
			void			update_dst_index(								INT32 index_dst, C_PCHAR_C mess,				   bool CONST b_move_to_gpu = false, c_pbo CONST * pbo = nullptr );																																									    

			void			update_after_process();

public:
	FINLINE	bool			is_gpu_use()				{	return _b_cuda_use_ui && aaa::c_cv::is_cuda_use();	}
	FINLINE	bool			is_content_changed()		{	return _b_content_changed;							}
	FINLINE	bool			is_size_changed()			{	return _b_size_changed;								}
	FINLINE	bool			is_src_bgr()				{	return _b_src_bgr;									}
	FINLINE	bool			is_src_b_bgr()				{	return _b_src_b_bgr;								}

			bool			build_dst_image( c_img_ipl& dst, INT32 index, c_img_ipl& ref, C_PCHAR_C name, INT32 channel_nb=1 );

	static	INT32	CONST	RESIZE_PARAM_NB			=	18;
	static	INT32	CONST	SRC_B_PARAM_NB			=	6;

	static	INT32	CONST	SUPER_PARAM_NB			=	SUPER::NO_GEO_PARAM_NB + 26;
	static	INT32	CONST	SUPER_SRC_2_PARAM_NB	=	SUPER_PARAM_NB + SRC_B_PARAM_NB;
	static	INT32	CONST	SUPER_RESIZE_PARAM_NB	=	SUPER_PARAM_NB + RESIZE_PARAM_NB;
	static	INT32	CONST	SUPER_2_RESIZE_PARAM_NB	=	SUPER_PARAM_NB + NO_GEO_PARAM_NB + RESIZE_PARAM_NB;

#define	BDD_IMG_BASE_PARAMS_LOW(nb)\
	BDD_NO_GEO_BASE_PARAMS\
	PARAM_DEF_BOOL_OFF(			active				)\
	PARAM_DEF_BOOL_OFF(			force_compute		)\
	PARAM_DEF_INT32_LOCKED(		process_index		)\
	PARAM_DEF_GROUP_CLOSED(		img_src, ((nb)+22)	)\
		PARAM_DEF_BOOL_OFF(			verbose					)\
		PARAM_DEF_NONE(				prev					)\
		PARAM_DEF_BIND_2D_ALONE(	image_src				)\
		PARAM_DEF_BOOL_OFF(			image_src_prev			)\
		PARAM_DEF_INT32_LOCKED_XY(	image_src_size			)\
		PARAM_DEF_STR_LOCKED(		image_src_format		)\
		PARAM_DEF_BIND_2D_ALONE(	image_dst_base			)\
		PARAM_DEF_INT32_LOCKED_XY(	image_dst_size			)\
		PARAM_DEF_STR_LOCKED(		image_dst_format		)\
		PARAM_DEF_BOOL_ON(			move_to_aaaseed			)\
		PARAM_DEF_BOOL_OFF(			use_cuda				)\
		PARAM_DEF_BOOL_OFF(			use_opencl				)\
		PARAM_DEF_BOOL_OFF(			use_opengl_tex			)\
		PARAM_DEF_BOOL_OFF(			mono					)\
		PARAM_DEF_SYMBO_PSTR(		image_component,		1, INT32(aaa::COMPO::GREY),		aaa::str_compo )\
		PARAM_DEF_REAL_ONE(			mono_compo_factor1		)\
		PARAM_DEF_REAL_ONE(			mono_compo_factor2		)\
		PARAM_DEF_REAL_ZERO(		mono_compo_offset		)\
		PARAM_DEF_BOOL_OFF(			mono_equalize_histogram	)\
		PARAM_DEF_BOOL_OFF(			float					)
	//	PARAM_DEF_BOOL_OFF(			use_pbo					)

#define	BDD_IMG_SRC_B_PARAMS\
		PARAM_DEF_BIND_2D_ALONE(	image_b					)\
		PARAM_DEF_INT32_LOCKED_XY(	image_b_src_size		)\
		PARAM_DEF_SYMBO_PSTR(		image_b_component,		1, INT32(aaa::COMPO::GREY),		aaa::str_compo )\
		PARAM_DEF_BOOL_OFF(			swap_src				)\
		PARAM_DEF_SYMBO(			resize_src,				1, 0,	c_bdd_img::RESIZE_SRC_MAX-1,	c_bdd_img::str_src_2_resize )
	
#define	BDD_IMG_CROP_RESIZE_ROTATE_PARAMS\
		PARAM_DEF_GROUP_CLOSED(	Crop_,	7		)\
			PARAM_DEF_BOOL_OFF(			crop				)\
			PARAM_DEF_REAL_POS_ZERO(	crop_left			)\
			PARAM_DEF_REAL_POS_ZERO(	crop_right			)\
			PARAM_DEF_REAL_POS_ZERO(	crop_bottom			)\
			PARAM_DEF_REAL_POS_ZERO(	crop_top			)\
			PARAM_DEF_INT32_LOCKED_XY(	crop_size			)\
		PARAM_DEF_GROUP_CLOSED(	Resize_, 6		)\
			PARAM_DEF_BOOL_OFF(			resize				)\
			PARAM_DEF_REAL_POS_ONE(		resize_fx			)\
			PARAM_DEF_REAL_POS_ONE(		resize_fy			)\
			PARAM_DEF_INT32_LOCKED_XY(	resize_size			)\
			PARAM_DEF_SYMBO_PSTR_ZERO(	resize_interpolation,	c_bdd_img::str_interpolation_mode	)\
		PARAM_DEF_GROUP_CLOSED(	Rotate_,	2	)\
			PARAM_DEF_BOOL_OFF(			rotate				)\
			PARAM_DEF_SYMBO(			rotate_angle,		1, 0,	c_bdd_img::CV_ROT_MAX-1,		c_bdd_img::str_rotation )


#define	BDD_IMG_BASE_PARAMS\
	BDD_IMG_BASE_PARAMS_LOW( 0 )

#define	BDD_IMG_BASE_2_PARAMS\
	BDD_IMG_BASE_PARAMS_LOW( c_bdd_img::SRC_B_PARAM_NB )\
	BDD_IMG_SRC_B_PARAMS

#define	BDD_IMG_BASE_RESIZE_PARAMS\
	BDD_IMG_BASE_PARAMS_LOW( c_bdd_img::RESIZE_PARAM_NB )\
	BDD_IMG_CROP_RESIZE_ROTATE_PARAMS

#define	BDD_IMG_BASE_2_RESIZE_PARAMS\
	BDD_IMG_BASE_PARAMS_LOW( c_bdd_img::SRC_B_PARAM_NB + c_bdd_img::RESIZE_PARAM_NB )\
	BDD_IMG_SRC_B_PARAMS\
	BDD_IMG_CROP_RESIZE_ROTATE_PARAMS

};


