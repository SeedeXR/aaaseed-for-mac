
#ifdef AAA_BDD_TORUS_H
#error "BDD_TORUS_H included more than once."
#endif
#define AAA_BDD_TORUS_H 1


#ifndef AAA_BDD_UV_H
#	include "obj_ui/bdd/util/bdd_uv.h"
#endif

class	c_bdd_torus final : public c_bdd_uv
{
	FACTORY_DECLARE( c_bdd_torus, c_bdd_uv );
private:
	REAL	_radius_int;
	REAL	_radius_int_geo;

public:

	virtual	void	build_geo_validate();
	virtual	bool	build_geo_is_needed();

			void	init();
	virtual	void	build_geo();

	virtual	void	param_init_pt();
};
