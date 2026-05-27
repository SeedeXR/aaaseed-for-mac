
#ifdef AAA_BDD_IMG_CV_MIX_H
#error "BDD_IMG_CV_MIX_H included more than once."
#endif
#define AAA_BDD_IMG_CV_MIX_H 1


#ifndef AAA_BDD_IMG_H
#	include "obj_ui/bdd/bdd_image/bdd_img.h"
#endif

class	c_bdd_img_cv_mix final : public c_bdd_img
{
	FACTORY_DECLARE( c_bdd_img_cv_mix, c_bdd_img );
private:

public:

protected:
#if AAA_LIB_USE_OPENCV_CUDA()
	cv::cuda::GpuMat	_mat_gpu_dst;
#endif
	cv::UMat			_mat_dst;

	INT32		_img_src_b;
	INT32		_img_dst;
	//c_img_ipl	_ipl_dst;
	//c_img_ipl	_ipl_src_b;
	INT32		_s_mix;
	REAL		_factor_a;
	REAL		_factor_b;
	REAL		_offset;
	REAL		_scale;
private:
	void	close();
	void	do_process();
	//bool	do_mix( c_img_ipl* src_a, c_img_ipl* src_b, c_img_ipl* dst );
	bool	do_mix_cpu( cv::UMat& src_a, cv::UMat& src_b, cv::UMat& dst );
#if AAA_LIB_USE_OPENCV_CUDA()
	bool	do_mix_gpu( cv::cuda::GpuMat& src_a, cv::cuda::GpuMat& src_b, cv::cuda::GpuMat& dst );
#endif

public:
			void	init();
	virtual	void	param_init_pt();

	virtual	void	update();
	virtual	void	draw();
	virtual bool	can_implicit() final override { return true; }
};
