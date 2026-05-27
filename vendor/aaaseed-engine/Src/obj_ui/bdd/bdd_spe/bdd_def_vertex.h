
#ifdef AAA_BDD_DEF_VERTEX_H
#error "BDD_DEF_VERTEX_H included more than once."
#endif
#define AAA_BDD_DEF_VERTEX_H 1


#ifndef	AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif

class	c_bdd_def_vertex final : public c_bdd
{
	FACTORY_DECLARE( c_bdd_def_vertex, c_bdd );
private:
	INT32	_s_screen_def;
	REAL	_cos_freq;
	REAL	_cos_limit;
	REAL	_z_factor;
	REAL	_y_origin;
	REAL	_y_factor;

	INT32	_net_channel;
public:

	virtual	void	update();
	virtual	void	draw();

	virtual	void	param_init_pt();
};

extern	c_bdd_def_vertex*	c_bdd_def_vertex_cur;


