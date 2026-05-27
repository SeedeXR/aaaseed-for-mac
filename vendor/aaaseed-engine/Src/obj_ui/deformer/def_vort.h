
#ifdef AAA_DEF_VORT_H
#error "DEF_VORT_H included more than once."
#endif
#define AAA_DEF_VORT_H 1


#ifndef AAA_DEFORMER_H
#	include "obj_ui/deformer/deformer.h"
#endif

class	c_def_vort final : public c_deformer
{
public:
	FACTORY_DECLARE(c_def_vort,c_deformer);

private:
	INT32	_i_axe;

	REAL	_freq;
//	REAL	_angle;

public:
	virtual	void	update() final override;
	virtual	void	apply( REAL CONST * src, REAL* dst, INT32 nb ) final override;
	virtual	void	param_init_pt() final override;

			void	init();

};

