
#ifdef AAA_DEF_SPHERICAL_H
#error "DEF_SPHERICAL_H included more than once."
#endif
#define AAA_DEF_SPHERICAL_H 1


#ifndef AAA_DEFORMER_H
#	include "obj_ui/deformer/deformer.h"
#endif

class	c_def_to_sphere final : public c_deformer
{
public:
	FACTORY_DECLARE(c_def_to_sphere,c_deformer);

private:
	INT32	_axe;
	INT32	_i_u;
	INT32	_i_v;

	REAL	_offset[3];
	REAL	_factor[3];
	REAL	_lissajous_ratio;
	REAL	_size_ratio;

public:
	virtual	void	update() final override;
	virtual	void	apply( REAL CONST * src, REAL* dst, INT32 nb ) final override;
	virtual	void	param_init_pt() final override;

			void	init();

};

