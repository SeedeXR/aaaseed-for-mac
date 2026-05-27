
#ifdef AAA_DEF_PANOSCOPE_H
#error "DEF_PANOSCOPE_H included more than once."
#endif
#define AAA_DEF_PANOSCOPE_H 1


#ifndef AAA_DEFORMER_H
#	include "obj_ui/deformer/deformer.h"
#endif

class	c_def_panoscope final : public c_deformer
{
public:
	FACTORY_DECLARE(c_def_panoscope,c_deformer);
private:

	INT32	axe;
	INT32	i_u;
	INT32	i_v;

	REAL	origin[3];

	REAL	bias;
	REAL	gain;
	REAL	bias_factor;
	REAL	gain_factor;

public:

	virtual	void	update() final override;
	virtual	void	apply( REAL CONST * src, REAL* dst, INT32 nb ) final override;
	virtual	void	param_init_pt() final override;

			void	init();
};

