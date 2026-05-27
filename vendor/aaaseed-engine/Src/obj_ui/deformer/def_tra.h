
#ifdef AAA_DEF_TRA_H
#error "DEF_TRA_H included more than once."
#endif
#define AAA_DEF_TRA_H 1


#ifndef AAA_DEFORMER_H
#	include "obj_ui/deformer/deformer.h"
#endif

class	c_def_tra final : public c_deformer
{
public:
	FACTORY_DECLARE(c_def_tra,c_deformer);
private:
//	REAL	origin[3];
	INT32	_i_axe;
	REAL	_tra_ui[3];
	REAL	_tra[3];
public:

	virtual	void	update();
	virtual	void	apply( REAL CONST * src, REAL* dst, INT32 nb );
	virtual	void	param_init_pt();

			void	init();
};

