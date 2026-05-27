
#ifdef AAA_DEF_VORTEX_H
#error "DEF_VORTEX_H included more than once."
#endif
#define AAA_DEF_VORTEX_H 1

#ifndef AAA_DEFORMER_H
#	include "obj_ui/deformer/deformer.h"
#endif

class	c_def_vortex final : public c_deformer
{
public:
	FACTORY_DECLARE( c_def_vortex, c_deformer );
private:
	INT32	_i_axe;

	REAL	_power;
	REAL	_power_a;
	REAL	_freq;
	REAL	_angle;
/*
	REAL	bias;
	REAL	gain;
	REAL	bias_factor;
	REAL	gain_factor;
*/
public:
	virtual	void	update() final override;
	virtual	void	apply( REAL CONST * src, REAL* dst, INT32 nb ) final override;
	virtual	void	param_init_pt() final override;

			void	init();
};

