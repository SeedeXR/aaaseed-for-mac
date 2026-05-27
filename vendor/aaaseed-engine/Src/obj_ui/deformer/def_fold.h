


#ifdef AAA_DEF_FOLD_H
#error "DEF_FOLD_H included more than once."
#endif
#define AAA_DEF_FOLD_H 1


#ifndef AAA_DEFORMER_H
#	include "obj_ui/deformer/deformer.h"
#endif

class	c_def_fold final : public c_deformer
{
public:
	FACTORY_DECLARE(c_def_fold,c_deformer);
private:
	REAL	origin;
	REAL	rotation;
	bool	b_rotation;
	INT32	s_how;
	REAL	offset;

	INT32	i_axe;
	INT32	i_u;
	INT32	i_v;
public:
	virtual	void	update() final override;
	virtual	void	apply( REAL CONST * src, REAL* dst, INT32 nb ) final override;
	virtual	void	param_init_pt() final override;

			void	init();
};


