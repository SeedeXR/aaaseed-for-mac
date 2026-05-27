
#ifdef AAA_BDD_PROJ_CONE_H
#error "BDD_PROJ_CONE_H included more than once."
#endif
#define AAA_BDD_PROJ_CONE_H 1


#ifndef AAA_BDD_H
#include "obj_ui/bdd/util/bdd.h"
#endif

class	c_bdd_proj_cone final : public c_bdd 
{
	FACTORY_DECLARE(c_bdd_proj_cone,c_bdd);
private:
	FP32	_src[3];
	FP32	_src_interpolation;

	FP32	_dst[3];
	FP32	_dst_interpolation;
	FP32	_dst_size_x;
	FP32	_dst_size_y;
	bool	_b_dst_in_camera;

	INT32	_axe;
	INT32	_i_u;
	INT32	_i_v;

	FP32	_point[4][3];
	FP32	_proj[4][3];
	FP32	_targ[4][3];
//	REAL	_normal_[5][3];

	bool	_b_src_cap;
	bool	_b_dst_cap;
	bool	_b_side_draw;

protected:
			void	alloc();
			void	dealloc();
public:

			void	init();

	virtual	bool	can_implicit() final override { return true; }
	virtual	void	draw();
	virtual	void	update();

			void	build();

	virtual	void	param_init_pt();

//	virtual	void	draw_normal_point( REAL len );
};
