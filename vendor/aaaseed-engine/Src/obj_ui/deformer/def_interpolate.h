
#ifdef AAA_DEF_INTERPOLATE_H
#error "DEF_INTERPOLATE_H included more than once."
#endif
#define AAA_DEF_INTERPOLATE_H 1


#ifndef AAA_DEFORMER_H
#	include "obj_ui/deformer/deformer.h"
#endif

class	c_bdd;
class	c_def_interpolate final : public c_deformer
{
public:
	FACTORY_DECLARE(c_def_interpolate,c_deformer);
private:
	REAL		_r_interpolate;
	REAL		_start;
	REAL		_size;
	o_str		_target_name_symbo;
	c_bdd*		_bdd_target;
	bool		_b_flip;
	bool		_b_middle;
	REAL		_middle_center;
	REAL		_bias;
	REAL		_gain;
	REAL		_bias_factor;
	REAL		_gain_factor;
public:

	virtual	void	update() final override;
	virtual	void	apply( REAL CONST * src, REAL* dst, INT32 nb ) final override;
	virtual	void	param_init_pt() final override;

			void	init();
};

