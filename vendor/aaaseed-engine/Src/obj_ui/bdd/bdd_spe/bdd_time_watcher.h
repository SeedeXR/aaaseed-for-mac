
#ifdef AAA_BDD_TIME_WATCHER_H
#error "BDD_TIME_WATCHER_H included more than once."
#endif
#define AAA_BDD_TIME_WATCHER_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif

class c_tbuf;

class	c_bdd_time_watcher final : public c_bdd
{
	FACTORY_DECLARE(c_bdd_time_watcher,c_bdd);
protected:
//	iter_data it_begin;
//	iter_data it_end;
//	INT32	tdata_nb;

	INT32	_channel_id;
	REAL	_center[3];
	c_tbuf*	_p_tbuf;

	INT32	_i_axe;
	INT32	_i_u;
	INT32	_i_v;
	REAL	_size[3];
	REAL	_size_factor_ui;
	REAL	_size_factor;

	bool	_b_draw_box_ui;
	bool	_b_draw_line_ui;
	bool	_b_draw_point_ui;
	bool	_b_draw_text_ui;
	bool	_b_draw_marker_ui;

	REAL	_marker_per_sec;
	REAL	_marker_time_offset;
	REAL	_marker_space_size;
	REAL	_marker_space_offset;
	FP32	_marker_color_ui[4];
	FP32	_marker_color[4];

	bool	_b_const;	//	use to differentiate between block/curves
	REAL*	_point;
	REAL*	_point_base;
	REAL*	_point_deformed;
	REAL*	_point_to_draw;
	INT32	_point_nb_allocated;
	INT32	_point_nb;
	INT32	_sample_nb;
protected:
			void	update_low();

	FINLINE	void	point_init();
	FINLINE	void	point_alloc( INT32 nb_in);
			void	point_dealloc();
public:

	virtual	void	param_init_pt();
			void	init();

	virtual	void	update();
	virtual	void	draw_point();
	virtual	void	draw_line();
	virtual	void	draw_box();
	virtual	void	draw_marker();
			void	draw_text();
	virtual	void	draw();
};
