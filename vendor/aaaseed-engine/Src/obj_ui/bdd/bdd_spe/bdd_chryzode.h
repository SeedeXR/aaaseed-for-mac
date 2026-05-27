
#ifdef AAA_BDD_CHRYZODE_H
#error "BDD_CHRYZODE_H included more than once."
#endif
#define AAA_BDD_CHRYZODE_H 1


#ifndef AAA_BDD_H
#include "obj_ui/bdd/util/bdd.h"
#endif

class	c_bdd_chryzode final : public c_bdd
{
	FACTORY_DECLARE(c_bdd_chryzode,c_bdd);
private:
	REAL	_center[3];
	REAL	_size[3];
	INT32	_seed;
	INT32	_mod;
	INT32	_mod_inc;
	INT32	_iter;
	REAL	_factor;
	REAL	_daxe;

	void	draw_one( INT32 CONST inc, INT32 CONST modulo, REAL CONST z );
public:

	virtual	void	param_init_pt();

	virtual	void	update();
	virtual	void	draw();
};
