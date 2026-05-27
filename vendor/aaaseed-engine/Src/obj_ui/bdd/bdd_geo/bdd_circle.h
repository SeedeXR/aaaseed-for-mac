
#ifdef AAA_BDD_CIRCLE_H
#error "BDD_CIRCLE_H included more than once."
#endif
#define AAA_BDD_CIRCLE_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif
#ifndef AAA_POINTS_H
#	include "draw/geo/curve_flat.h"
#endif

class	c_bdd_circle final : public c_bdd_multiple
{
	FACTORY_DECLARE(c_bdd_circle,c_bdd_multiple);
private:
	REAL		_center_ui[3];
	REAL		_normal_ui[3];
	INT32		_s_draw;
	bool		_b_sym_ui;
	bool		_b_direct_ui;
	REAL		_angle_offset_ui;

	INT32		_point_nb_ui;

	c_circle	_circle;

public:
			void	init();

	virtual	bool	can_implicit() final override {	return true; }

	virtual	void	draw_single();
	virtual	void	update();

	virtual	void	param_init_pt();

	virtual	INT32	get_point_nb()	{	return _circle.get_point_nb();			}
	virtual	REAL*	get_points()	{	return _circle.get_points_to_draw();	}	
	virtual	REAL*	get_normals()	{	return _circle.get_normals();			}
};


