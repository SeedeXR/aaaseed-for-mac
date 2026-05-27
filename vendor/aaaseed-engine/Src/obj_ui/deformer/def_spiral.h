
#ifdef AAA_DEF_SPIRAL_H
#error "DEF_SPIRAL_H included more than once."
#endif
#define AAA_DEF_SPIRAL_H 1


#ifndef AAA_DEFORMER_H
#	include "obj_ui/deformer/deformer.h"
#endif

class	c_def_spiral final : public c_deformer
{
public:
	FACTORY_DECLARE( c_def_spiral,c_deformer);
private:
	REAL	_origin[3];
	INT32	_i_axe;
	INT32	_i_u;
	INT32	_i_v;
	REAL	_freq;
	REAL	_phase_offset;

	bool	_b_clamp;
	REAL	_limit_min;
	REAL	_limit_max;

public:
	virtual	void	update() final override;
	virtual	void	apply( REAL CONST * src, REAL* dst, INT32 nb ) final override;
	virtual	void	param_init_pt() final override;

			void	init();
};

