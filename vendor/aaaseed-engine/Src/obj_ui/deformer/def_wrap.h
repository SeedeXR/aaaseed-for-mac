
#ifdef AAA_DEF_WRAP_H
#error "DEF_WRAP_H included more than once."
#endif
#define AAA_DEF_WRAP_H 1


#ifndef AAA_DEFORMER_H
#	include "obj_ui/deformer/deformer.h"
#endif

class	c_def_wrap final : public c_deformer
{
public:
	FACTORY_DECLARE(c_def_wrap,c_deformer);
private:
	REAL	_origin;
	REAL	_size;
	INT32	_i_axe;
	REAL	_min;
	REAL	_max;

public:
	virtual	void	update() final override;
	virtual	void	apply( REAL CONST * src, REAL* dst, INT32 nb ) final override;
	virtual	void	param_init_pt() final override;

			void	init();
};

