
#ifdef AAA_AAA_OPENCV_H
#error "AAA_OPENCV_H included more than once."
#endif
#define AAA_AAA_OPENCV_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif


// can only be 410, 412, 4100, 4120, 4130 (2026 January)
#define AAA_LIB_USE_OPENCV_VERSION()		4130

#define AAA_LIB_USE_OPENCV_VERSION_STR()	AAA_STRING( AAA_LIB_USE_OPENCV_VERSION() )

//todo opencv, opengl (opencl), cuda issues need to be solved
#define	AAA_OPENCV_GL_USE()	0

#if   AAA_LIB_USE_OPENCV_VERSION() == 4130
#	define	AAA_LIB_USE_OPENCV_CUDA()	0
#	define	AAA_LIB_USE_OPENCV_VERSION_ASKED() "4.13.0 without CUDA (2026 January)"
#elif AAA_LIB_USE_OPENCV_VERSION() == 4120
#	define	AAA_LIB_USE_OPENCV_CUDA()	0
#	define	AAA_LIB_USE_OPENCV_VERSION_ASKED() "4.12.0 without CUDA (2025 July)"
#elif AAA_LIB_USE_OPENCV_VERSION() == 4100
#	define	AAA_LIB_USE_OPENCV_CUDA()	1
#	define	AAA_LIB_USE_OPENCV_VERSION_ASKED() "4.10.0 with CUDA (2024 June)"
#elif AAA_LIB_USE_OPENCV_VERSION() == 412
#	define	AAA_LIB_USE_OPENCV_CUDA()	1
#	define	AAA_LIB_USE_OPENCV_VERSION_ASKED() "4.1.2 with CUDA (2019 November)"
#elif AAA_LIB_USE_OPENCV_VERSION() == 410
#	define	AAA_LIB_USE_OPENCV_CUDA()	0
#	define	AAA_LIB_USE_OPENCV_VERSION_ASKED() "4.1.0 without CUDA (2019 April)"
#else
#	error AAA_LIB_USE_OPENCV_VERSION() can only be 410, 412, 4100, 4120, 4130 (2026 January)
#endif

#ifndef OPENCV_IMGPROC_HPP
#	include "opencv2/imgproc.hpp"
#endif

#ifndef OPENCV_IMGPROC_TYPES_C_H
#	include	"opencv2/imgproc/types_c.h"
#endif

#if AAA_LIB_USE_OPENCV_CUDA()
#	ifndef OPENCV_CORE_CUDA_HPP
#		include "opencv2/core/cuda.hpp"
#	endif
#endif

#ifndef AAA_POLYLINE_H
#	include "draw/geo/polyline.h"
#endif
#ifndef	AAA_PIXEL_FORMAT_H
#	include "image/pixel_format.h"
#endif

class c_img_2d;
class o_str;

namespace aaa
{
	class	c_cv
	{
	public:
		static INT32				opencv_version_minor;
		static INT32				opencv_version_major;
		static INT32				opencv_version_revision;

		static	bool				b_cuda_start_with;
		static	bool				b_cuda_allow_ui;
		static	bool				b_cuda_use_out;
		static	UINT32				cuda_device_index_used;
		static	UINT32				cuda_device_index_asked_ui;
		static	INT32				cuda_device_nb;

		static	bool				b_opencl_can_out;
		static	bool				b_opencl_allow_ui;
		static	bool				b_opencl_use_out;

		static	o_str				opencl_device_wanted;
//		static	bool				b_opencl_start_with;
//		static	UINT32				ocl_device_use;

		static	bool				b_opengl_can_out;
		static	bool				b_opengl_allow_ui;
		static	bool				b_opengl_use_out;

		static	bool				b_display_info_trig;
		static	bool				b_use_ipp_ui;


		FINLINE	static	bool		is_cuda_use()		{ return b_cuda_use_out; }

		static	void				c_display_info_opencl();
		static	void				c_display_info_cuda();
		static	void				c_display_info();
		static	void				c_init();
		static	void				c_update();
		static	C_PCHAR				get_lib_info_str();

		static	bool				img_to_cv_umat(	c_img_2d* img, cv::UMat& mat );
		static	bool				img_to_cv_mat(	c_img_2d* img, cv::Mat& mat );
		static	INT32				get_cv_type(	aaa::PIXEL_FORMAT format );
		static	aaa::PIXEL_FORMAT	get_pixel_format_from_cv( INT32 cv_type, bool CONST b_bgr );
		static	int					get_opencv_depth_from_pixel_type( aaa::PIXEL_TYPE CONST type_src, C_PCHAR_C fn_name );
	private:
				REAL		_x_factor;
				REAL		_y_factor;

				cv::UMat*	_line_dst;

		FINLINE	void				collect_poly_edges( CvSeq* CONST v, CvContour* CONST edges, void CONST * CONST color, int CONST line_type, int CONST shift );
		FINLINE	void				draw_line( cv::Point CONST p0, cv::Point CONST p1, void CONST * CONST color );
	public:
		FINLINE	void				set_xy_factor( REAL CONST x_factor, REAL CONST y_factor )
									{
										_x_factor = x_factor;
										_y_factor = y_factor;
									}
		FINLINE	void				set_line_dst( cv::UMat * CONST dst )	{ _line_dst = dst; }
		void						process_contour( c_polyline_2d* polyline, std::vector<cv::Point> cont,
													 double CONST * CONST col_external, double CONST * CONST col_hole, int CONST thickness, int CONST line_type );
		void						extract_contour( c_polyline_2d* polyline, std::vector<cv::Point> cont );
		/*void						draw_contours( c_polyline_2d* polyline, cv::UMat* img, std::vector< std::vector<cv::Point> > cont,
													double* externalColor, double* holeColor, 
													INT32  maxLevel, INT32 thickness,
													INT32 line_type );*/
//		void						extract_contours( c_polyline_2d* polyline, std::vector< std::vector<cv::Point> > cont );

		enum class STATUS : UINT32
		{
			NO = 0,
			ASKED,
			STARTED,
			DONE,
			ERR_FLOAT,
			ERR_UINT8_ONLY,
			ERR_CH_1_ONLY,
			ERR_CH_13_ONLY,
			ERR_CH_134_ONLY,
			ERR_CH_123_ONLY,
			ERR_CH_14_ONLY,
			ERR_CH_34_ONLY,
			ERR_CH_4_ONLY,
//			ERR_CHANNEL,
			ERR_CPU,
			ERR_GPU,
			ERR_CV_EXCEPTION,
			ERR_UNIMPLEMENTED
		};
		static bool	build_comment_status( o_str* CONST o, bool CONST b_on, STATUS CONST status );
	};
}	// namespace aaa

