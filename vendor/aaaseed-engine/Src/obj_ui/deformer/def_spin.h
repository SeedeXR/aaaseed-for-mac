
#ifdef AAA_DEF_SPIN_H
#error "DEF_SPIN_H included more than once."
#endif
#define AAA_DEF_SPIN_H 1


#ifndef AAA_DEFORMER_H
#	include "obj_ui/deformer/deformer.h"
#endif

class	c_def_spin final : public c_deformer
{
public:
	FACTORY_DECLARE(c_def_spin,c_deformer);
private:
	REAL	_bias_factor;
	REAL	_gain_factor;

	REAL	_speed;
	REAL	_strength;

	INT32	_src_axe;
	INT32	_dst_axe;
//	bool	_b_abs;

	REAL	_bias;
	REAL	_gain;

	REAL	_t;	//	time cached here

public:
	virtual	void	update() final override;
	virtual	void	apply( REAL CONST * src, REAL* dst, INT32 nb ) final override;
	virtual	void	param_init_pt() final override;

			void	init();
};

