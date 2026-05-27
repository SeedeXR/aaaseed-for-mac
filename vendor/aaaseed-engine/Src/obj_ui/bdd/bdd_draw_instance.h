
#ifdef AAA_BDD_DRAW_INSTANCE_H
#error "BDD_DRAW_INSTANCE_H included more than once."
#endif
#define AAA_BDD_DRAW_INSTANCE_H 1



#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif

class c_bdd_draw_instance final : public c_bdd
{
	FACTORY_DECLARE( c_bdd_draw_instance, c_bdd );
private:
	INT32	_vertex_nb_ui;
	INT32	_instance_nb_ui;

	bool	_b_draw_ui;

private:
	void	init();

public:
	virtual	void	param_init_pt();

	virtual	void	update();
	virtual	void	draw();
};
