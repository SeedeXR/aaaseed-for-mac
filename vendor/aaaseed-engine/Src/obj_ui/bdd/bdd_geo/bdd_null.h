
#ifdef AAA_BDD_NULL_H
#error "BDD_NULL_H included more than once."
#endif
#define AAA_BDD_NULL_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif

class	c_bdd_null final : public c_bdd_multiple
{
	FACTORY_DECLARE( c_bdd_null, c_bdd_multiple );

private:
	REAL	_origin[3];
	INT32	_s_dim;
	REAL	_size[3];
	INT32	_s_draw_text;
	o_str	_text;
	void	init();

public:
//todo add tex obj
// add deal with limits 

	virtual	void	param_init_pt();

	virtual	void	update();
	virtual	void	draw_single();
	virtual	void	draw_multiple();

	virtual	INT32	get_point_nb() final override;
	virtual	REAL*	get_point_pt( INT32 CONST index ) final override;
};
