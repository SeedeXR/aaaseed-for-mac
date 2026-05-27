
#ifdef AAA_DEF_SCALE_H
#error "DEF_SCALE_H included more than once."
#endif
#define AAA_DEF_SCALE_H 1


#ifndef AAA_DEFORMER_H
#	include "obj_ui/deformer/deformer.h"
#endif

class	c_def_scale final : public c_deformer
{
	FACTORY_DECLARE( c_def_scale,c_deformer)

private:
	REAL	_origin[3];
	INT32	_i_axe;
	REAL	_sca_ui[4];
	REAL	_sca[3];

public:
	virtual	void	update() final override;
	virtual	void	apply( REAL CONST * src, REAL* dst, INT32 nb ) final override;
	virtual	void	param_init_pt() final override;

			void	init();
};

