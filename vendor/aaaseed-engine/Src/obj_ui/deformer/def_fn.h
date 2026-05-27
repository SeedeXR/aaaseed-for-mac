
#ifdef AAA_DEF_FN_H
#error "DEF_FN_H included more than once."
#endif
#define AAA_DEF_FN_H 1


#ifndef AAA_DEFORMER_H
#	include "obj_ui/deformer/deformer.h"
#endif

class	c_def_fn final : public c_deformer
{
public:
	FACTORY_DECLARE(c_def_fn,c_deformer);
private:
	REAL	freq;
	INT32	harm_nb;

	REAL	strenght;
	INT32	dst_axe;

	REAL	move_speed;
	INT32	move_index;

	REAL	move_value;
public:

	virtual	void	update() final override;
	virtual	void	apply( REAL CONST * src, REAL* dst, INT32 nb ) final override;
	virtual	void	param_init_pt() final override;
};

