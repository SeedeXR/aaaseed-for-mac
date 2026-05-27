
#ifdef AAA_BDD_IMG_DETECT_FACE_H
#error "BDD_IMG_DETECT_FACE_H included more than once."
#endif
#define AAA_BDD_IMG_DETECT_FACE_H 1


#ifndef AAA_BDD_IMG_H
#	include "bdd_img.h"
#endif
#ifndef _VECTOR_
#	include <vector>
#endif

struct	CvHaarClassifierCascade;

class	c_bdd_img_detect_face final : public c_bdd_img
{
	FACTORY_DECLARE( c_bdd_img_detect_face, c_bdd_img );

public:
	static c_bdd_img_detect_face*	cur;

protected:
	INT32		_dst_index;

//	REAL		_x_factor;
//	REAL		_y_factor;
//	REAL		_height;

private:
	void		do_process();

	BOOL						_b_haar_open;
	CvHaarClassifierCascade*	_classifier; 
	CvMemStorage*				_face_storage;
	c_img_ipl					_src;
	c_img_ipl					_dst;

	void	close_haar();
	void	open_haar();
	void	do_haar( c_img_ipl* src, c_img_ipl* dst );

	void	release();

public:
			void	init();
	virtual	void	param_init_pt();

	virtual	void	update();
	virtual	void	draw();
	virtual	BOOL	can_implicit() { return TRUE; }
};
