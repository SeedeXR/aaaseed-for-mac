
#ifdef AAA_BDD_UNIVIEW_H
#error "BDD_UNIVIEW_H included more than once."
#endif
#define AAA_BDD_UNIVIEW_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif

class	c_bdd_uniview final : public c_bdd_multiple
{
	FACTORY_DECLARE(c_bdd_uniview,c_bdd_multiple);
private:
	REAL	_origin[3];
	REAL	_size[3];
	INT32	_blk_received;

			void	init();
public:
	virtual	void	param_init_pt();

	virtual	void	update();
	virtual	void	draw_single();
	virtual	void	draw_multiple();

	virtual	INT32	get_point_nb() final override;
	virtual	REAL*	get_point_pt( INT32 CONST index ) final override;
};
