
#ifdef AAA_BDD_POLYGON_H
#error "BDD_POLYGON_H included more than once."
#endif
#define AAA_BDD_POLYGON_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif
#ifndef AAA_POINTS_H
#	include "draw/geo/points.h"
#endif
#ifndef AAA_POLYLINE_H
#	include "draw/geo/polyline.h"
#endif

class	c_bdd_polygon final : public c_bdd_multiple
{
	FACTORY_DECLARE( c_bdd_polygon, c_bdd_multiple );

public:
	static	INT32 CONST		POINT_NB_MAX = 16;

private:
	INT32			_point_nb_ui;
	bool			_b_draw_polygon_ui;
	bool			_b_use_quad_ui;
	bool			_b_draw_polyline_ui;
	bool			_b_draw_polyline_special_ui;
	bool			_b_draw_line_ui;
	bool			_b_draw_line_close_ui;

	REAL			_origin[3];

	REAL			_point_ui[POINT_NB_MAX][3];
	c_points_3d		_points;

	REAL			_polyline_width;
	c_polyline_2d	_polyline;

			void	init();
	FINLINE	void	draw_points_raw( bool b_closed );
public:
	virtual	void	param_init_pt();

	virtual	void	update();
	virtual	void	draw_single();

	virtual bool	set_point( INT32 index, REAL CONST * pt_in );
	virtual	INT32	get_point_nb();
	virtual	REAL*	get_points();
	virtual	REAL*	get_normals();

	virtual	bool	can_implicit() final override { return true; }
};
