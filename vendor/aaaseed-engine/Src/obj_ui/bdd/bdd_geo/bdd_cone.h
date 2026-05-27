
#ifdef AAA_BDD_CONE_H
#error "BDD_CONE_H included more than once."
#endif
#define AAA_BDD_CONE_H 1


#ifndef AAA_BDD_UV_H
#	include "obj_ui/bdd/util/bdd_uv.h"
#endif


class	c_bdd_cone final : public c_bdd_uv 
{
	FACTORY_DECLARE( c_bdd_cone, c_bdd_uv );
private:
	REAL	_radius_top;
	REAL	_radius_top_ui;

	REAL	_lissajous_ratio_ui;
	REAL	_lissajous_ratio;

//todo add
//	bool	_b_cap_top;
//	bool	_b_cap_top_ui;
//	bool	_b_cap_bottom;	
//	bool	_b_cap_bottom_ui;

public:
	virtual	void	build_geo_validate();
	virtual	bool	build_geo_is_needed();
	virtual	void	get_point_from_uv( REAL* dst, REAL u, REAL v );

			void	init();
	virtual void	build_geo();
	virtual	void	param_init_pt();
};
