
#ifdef AAA_BDD_FACE_H
#error "BDD_FACE_H included more than once."
#endif
#define AAA_BDD_FACE_H 1


#ifndef AAA_BDD_H
#include "obj_ui/bdd/util/bdd.h"
#endif

enum LOCK_TYPE : INT32
{
	LOCK_SIZE,
	LOCK_CENTER,
	LOCK_SPACE_ABS,
	LOCK_SPACE_RELATIVE,
	LOCK_TYPE_MAX_NB,
};

class	c_bdd_face final : public c_bdd_multiple 
{
	FACTORY_DECLARE( c_bdd_face, c_bdd_multiple );
//worms
private:
public:
	INT32	_face_axe;
	INT32	_face_u;
	INT32	_face_v;
	REAL	_face_size_ui[4];
	REAL	_face_size[3];
	REAL	_face_offset[3];
	REAL	_face_offset_ui[4];

	REAL	_center[3];

	bool	_b_face_image_ratio;

	INT32		_bdd_dimension;
	INT32		_bdd_nb[3];
	LOCK_TYPE	_s_lock[3];
	REAL		_bdd_size[3];
	REAL		_space[3];

	REAL	_rot_speed[3];
	REAL	_rot_origin[3];
	REAL	_rot_factor;
	REAL	_rot_offset_ui[3];
	REAL	_rot_offset[3];
//	REAL	corner[4][3];	
//	REAL	uv[4][3];
	REAL	_normal[4][3];
	REAL	_tex_offset_ui[4];
	REAL	_tex_offset[3];

	void	init();
	void	alloc(); 
	void	dealloc();

public:

	virtual	void	param_init_pt();

	virtual	void	update();
	virtual	void	draw_single();
	virtual	void	draw_multiple();

	void	build();
	void	draw_0d();
	void	draw_1d( INT32 bdd_axe );
	void	draw_2d( INT32 bdd_axe );
	void	draw_3d( INT32 bdd_axe );
};


