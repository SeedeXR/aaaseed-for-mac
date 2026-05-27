
#ifdef AAA_BDD_GRID_H
#error "BDD_GRID_H included more than once."
#endif
#define AAA_BDD_GRID_H 1


#ifndef AAA_BDD_UV_H
#	include "obj_ui/bdd/util/bdd_uv.h"
#endif


class	c_bdd_grid final : public c_bdd_uvw
{
	FACTORY_DECLARE( c_bdd_grid, c_bdd_uvw );
private:
public:
			void	init();
	
	virtual	void	build_geo();
	virtual	void	update();

			void	build_grid( REAL z );

	virtual	void	param_init_pt();
};
