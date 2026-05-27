
#ifdef AAA_BDD_LAYER_SELECTOR_H
#error "BDD_LAYER_SELECTOR_H included more than once."
#endif
#define AAA_BDD_LAYER_SELECTOR_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif

class	c_layer;


class	c_bdd_layer_selector final : public c_bdd 
{
	FACTORY_DECLARE(c_bdd_layer_selector,c_bdd);

public:
	static INT32 CONST SELECTOR_NB			=	5;
	static INT32 CONST LAYER_BY_SELECTOR_NB	=	4;

private:
	INT32		_s_type;
	REAL		_position[3];
	REAL		_selector_ui;
	REAL		_selector;
	REAL		_min;
	REAL		_max;
	INT32		_nb_selector;
	INT32		_nb_to_draw;
	INT32		_index[SELECTOR_NB][LAYER_BY_SELECTOR_NB];
	REAL		_layer_min[SELECTOR_NB];
	REAL		_layer_max[SELECTOR_NB];
	bool		_b_use_min;
	c_layer*	_layer[SELECTOR_NB*LAYER_BY_SELECTOR_NB];

	void	init();
public:
//todoqq	deal with can implicit

	virtual	void	param_init_pt();

	virtual	void	draw();
	virtual	void	update();
};
