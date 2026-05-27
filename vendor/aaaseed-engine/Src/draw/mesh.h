
#ifdef AAA_MESH_H
#error "MESH_H included more than once."
#endif
#define AAA_MESH_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif

class	c_fn1d_fm_add;
class	c_fn1d_phase_pertub;

class	c_bdd_mesh final : public c_bdd
{
	FACTORY_DECLARE(c_bdd_mesh,c_bdd)
private:	
	REAL*	_point;
	REAL*	_normal;
	INT32	_nb_x;
	INT32	_nb_y;
//	bool	_b_to_generate;
	bool	_b_to_make_normal;

	c_fn1d_fm_add*			_pertub_mesh;
	c_fn1d_phase_pertub*	_regul_mesh;

	REAL	_pertub_factor_x;
	REAL	_pertub_factor_z;
	REAL	_factor_z;

	REAL	_pertub_factor_x_last;
	REAL	_pertub_factor_z_last;
	REAL	_factor_z_last;

	INT32	_pertub_type;

public:

	virtual	void	param_init_pt();
			
			void	alloc();
			void	dealloc();
			void	init();

	virtual	void	update();
	virtual	void	draw();
	
			void	make_normal();
			void	resolution_xy_set( INT32 nb_poly_x, INT32 nb_poly_y );
};

