
#ifdef AAA_BDD_MIGUEL_H
#error "BDD_MIGUEL_H included more than once."
#endif
#define AAA_BDD_MIGUEL_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif

class	c_bdd_datagrid;
class	c_bdd_tex2d;

class	c_bdd_miguel_2003nb final : public c_bdd_multiple
{
	FACTORY_DECLARE(c_bdd_miguel_2003nb,c_bdd_multiple);
protected:
	REAL	center[3];
	REAL	radius_int;
	REAL	radius_ext;

	REAL	b_point_on_top;
	REAL*	points;
	REAL*	points_def;

	REAL	gain;
	REAL	bias;
	REAL	gain_fac;
	REAL	bias_fac;

	REAL	speed_min;
	REAL	speed_max;
	REAL	speed_gain;
	REAL	speed_bias;
	REAL	speed_gain_fac;
	REAL	speed_bias_fac;

	INT32	s_type;
	INT32	s_font;
	REAL	text_size;
	REAL	text_offset_vert;
	REAL	text_rot_speed_y;
	REAL	text_rot_speed_z;

	INT32	point_nb;

	c_bdd_datagrid*	p_datagrid;
	c_bdd_tex2d*	p_tex;
public:
	INT32	point_nb_to_draw;
	REAL*	points_to_draw;
	REAL*	angles;
private:
			void	alloc();
			void	dealloc();
			void	do_deform();
public:

			void	init();
	virtual	void	param_init_pt();

			void	build_one(INT32 index);
			void	build();

	virtual bool	can_implicit() final override { return true; }
	virtual	void	update();

	virtual	void	draw_single();
	virtual	void	draw_multiple();
};

#define	BDD_POINT_NETWORK_LINK_NB	(4096*8)

class	c_bdd_point_network_2003nb final : public c_bdd_multiple
{
	FACTORY_DECLARE(c_bdd_point_network_2003nb,c_bdd_multiple);
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

	REAL	connect_angle_max;
	REAL	connect_dist_max;
	INT32	connect_try_max;
	bool	b_connect_use_y;

	INT32	seed;
	INT32	good_index[BDD_POINT_NETWORK_LINK_NB];

public:

			void	init();
	virtual	void	param_init_pt();

	virtual bool	can_implicit() final override { return true; }
	virtual	void	update();

	virtual	void	draw_single();
	virtual	void	draw_multiple();
};
