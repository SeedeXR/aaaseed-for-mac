
#ifdef AAA_CELL_DRAW_H
#error "CELL_DRAW_H included more than once."
#endif
#define AAA_CELL_DRAW_H 1


#ifndef AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif

class	c_cell_draw final : public c_obj_active_ui
{
	FACTORY_DECLARE(c_cell_draw,c_obj_active_ui);
private:
protected:
	BOOL	b_active_only;
	INT32	level_max;

	REAL	obj_size;
	REAL	text_size;
	INT32	s_font;
	BOOL	b_font_outline;
	REAL	font_size_cache;

	REAL	angle_start;
	REAL	angle_stop;

	REAL	size_factor;
	REAL	level_translate;
	REAL	level_radius;
	REAL	alpha_max;
	REAL	alpha_min;

	INT32	s_draw_obj_def;
	INT32	obj_nb_u;
	INT32	obj_nb_v;
	REAL	obj_color_ui[4];
	REAL	obj_color[4];
	INT32	s_distrib_type;

	REAL	text_color_ui[4];
	REAL	text_color[4];

	BOOL	b_branch_draw;
	REAL	branch_color_ui[4];
	REAL	branch_color[4];

	BOOL	b_root_draw;
	REAL	root_color_ui[4];
	REAL	root_color[4];

	REAL	cell_pos_start[3];
	REAL	cell_size_start;

	UINT32	list_base_;
public:

	virtual	void	param_init_pt();

	virtual	void	update();
	virtual	void	draw();

	FINLINE	void	draw_text( REAL	size, CONST CHAR* CONST txt );
	FINLINE	void	branch_make_pos( REAL* pos, REAL size, INT32 index, INT32 nb_u, INT32 nb_v );
	FINLINE	void	draw_branches( c_obj_ui* caller, REAL size, REAL* pos, INT32 level);

			void	prepare_lists( );
			void	draw_obj( REAL	size, REAL* pos );
			void	draw( c_obj_ui* caller, INT32 level, CONST CHAR* CONST txt);

	FINLINE	REAL	get_obj_size()	{	return obj_size; }
	FINLINE	REAL	get_text_size()	{	return text_size; }

	FINLINE	void	set_color( REAL* color, INT32 level);
};

extern	c_cell_draw* cell_draw_cur;

