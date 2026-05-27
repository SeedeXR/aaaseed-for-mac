
#ifdef AAA_BDD_SPHERE_H
#error "BDD_SPHERE_H included more than once."
#endif
#define AAA_BDD_SPHERE_H 1


#ifndef AAA_BDD_UV_H
#	include "obj_ui/bdd/util/bdd_uv.h"
#endif

class	c_bdd_sphere final : public c_bdd_uv
{
	FACTORY_DECLARE( c_bdd_sphere, c_bdd_uv );
public:
			void	init();
	virtual	void	build_geo();

	virtual	void	param_init_pt();
};
