
#ifdef AAA_BDD_POINt_NETWORK_H
#error "BDD_POINt_NETWORK_H included more than once."
#endif
#define AAA_BDD_POINt_NETWORK_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif

//todoqqq make it dynamic
#define	BDD_POINT_NETWORK_LINK_NB	(4096*8)

class	c_bdd_point_network final : public c_bdd_multiple
{
	FACTORY_DECLARE(c_bdd_point_network,c_bdd_multiple);
protected:

	INT32	nb;
	REAL	size_min;
	REAL	size_max;

	REAL	size_gain;
	REAL	size_bias;
	REAL	size_gain_fac;
	REAL	size_bias_fac;
	REAL	size_ratio;

	REAL	refresh_speed;

	REAL	map_min;
	REAL	map_max;
	REAL	map_speed_min;
	REAL	map_speed_max;

//	REAL	connect_angle_max;
	REAL	connect_dist_min;
	REAL	connect_dist_max;
	INT32	connect_try_max;
//	bool	b_connect_use_y;

	INT32	seed;
	INT32	good_index[BDD_POINT_NETWORK_LINK_NB];

public:

			void	init();
	virtual	void	param_init_pt();

	virtual bool	can_implicit() final override { return true; }
	virtual	void	update();

	virtual	void	draw_single();

//	FINLINE void	draw_link( REAL* a, REAL *b, REAL size, REAL size_ratio, REAL u_start, REAL u_stop );
};
