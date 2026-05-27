
#ifdef AAA_BDD_IMG_CV_WRITER_H
#error "BDD_IMG_CV_WRITER_H included more than once."
#endif
#define AAA_BDD_IMG_CV_WRITER_H 1


#ifndef AAA_BDD_IMG_H
#	include "obj_ui/bdd/bdd_image/bdd_img.h"
#endif

#ifndef OPENCV_HIGHGUI_HPP
#	include <opencv2/highgui/highgui.hpp>  // Video write
#endif

#ifndef _CHRONO_
#	ifndef __STDC_LIMIT_MACROS
#		define __STDC_LIMIT_MACROS
#	endif
#	include <chrono>
#endif


class c_speed;
class c_speed_ui;

class	c_thread_compress;



class	c_bdd_img_cv_mov_writer final : public c_bdd_img
{
	FACTORY_DECLARE( c_bdd_img_cv_mov_writer, c_bdd_img );
public:
	enum class CODEC : INT32
	{
		DIALOG = 0,
		RAW,
		I420,
		XVID,
		MJPEG,
		H264,
		HUFFYUV,
		FLV,
		NB_MAX
	};
	static	C_PCHAR_C	codec_str[ INT32(CODEC::NB_MAX) ];

	//enum class MOVIE_TYPE : INT32
	//{
	//	MOVIE_AVI = 0,
	//	MOVIE_MKV,
	//	MOVIE_FLV,
	//	MOVIE_MPG,
	//	NB_MAX
	//};
	//static	C_PCHAR_C	type_str[MOVIE_TYPE::NB_MAX];


protected:
//	cv::Mat				_mat_src_a;
	bool				_b_thread_closing;

	cv::Mat				_mat_img;
//	cv::UMat			_mat_cvt;

	c_thread_compress*	_thread_compress;
	//cv::Mat			_mat_dst;
	cv::VideoWriter		_video_writer;
	//cv::cuda::VideoWriter_GPU	_video_writer_gpu;		// Not supported with CUDA 6.5...

//	bool				_b_movie_save_ui;
//	bool				_b_movie_save;

	o_str				_movie_filename_ui;
//	INT32				_movie_type_ui;
	INT32				_movie_codec_ui;
	bool				_b_movie_run;
	bool				_b_movie_run_ui;
//	bool				_b_movie_run;

	UINT32				_movie_frame_nb;
	REAL				_fps_ui;
	REAL				_quality_ui;
	UINT32				_dropped_frame_nb;

	o_str				_name;

	bool				_b_opened;

	bool				_b_verbose_ui;

	bool				_b_swap_red_blue_ui;
//	bool				_b_flip_h_ui;
	bool				_b_flip_v_ui;

	typedef	std::deque<c_img_2d*>		IMG_CONT;
//	typedef	IMG_CONT::iterator		IMG_ITER;	//todo use auto where it is used

	IMG_CONT			_images_free;
	IMG_CONT			_images_used;
//	INT32				_image_nb_to_keep;
	mutable aaa::MUTEX*	_lock_free;
	mutable aaa::MUTEX*	_lock_used;
	c_img_2d*			_img;

	UINT32				_buffer_free_count;
	UINT32				_buffer_used_count;

	std::chrono::time_point<std::chrono::system_clock>	_last_tick;


	c_speed*			_speed_bdd;
	c_speed_ui*			_speed_bdd_ui;

private:
	AAA_ERR	open( c_img_2d* src );
	void	open_video_writer();
	void	close_video_writer();
	void	close();
	void	do_process();
	void	alloc_buffer();
	void	dealloc_buffer();

public:
			void	stop();
			void	init();
	virtual	void	param_init_pt();

	virtual	void	update();
	virtual	void	draw();
	virtual bool	can_implicit() final override { return true; }

	virtual	void	update_async();
			void	init_thread();
			void	close_thread();
};


