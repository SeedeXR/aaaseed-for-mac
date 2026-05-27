
#ifdef AAA_DEF_BDD_UV_H
#error "DEF_BDD_UV_H included more than once."
#endif
#define AAA_DEF_BDD_UV_H 1


#ifndef AAA_DEFORMER_H
#	include "obj_ui/deformer/deformer.h"
#endif

class	c_bdd_uv;
class	c_bdd_grid_adjustable;

class	c_def_bdd_uv final : public c_deformer
{
public:
	FACTORY_DECLARE( c_def_bdd_uv, c_deformer );

private:
	o_str					_target_name_symbo;
	c_bdd_uv*				_bdd_target;
	c_bdd_grid_adjustable*	_bdd_target_adjust;	//todo generalize to bdd_uv
	bool					_b_reveal_uv;

	REAL					_u[2];
	REAL					_v[2];

public:
	virtual	void	update() final override;
	virtual	void	apply( REAL CONST * src, REAL* dst, INT32 nb ) final override;
	virtual	void	param_init_pt() final override;
			void	init();
};

