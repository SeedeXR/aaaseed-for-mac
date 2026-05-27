
#ifdef AAA_BDD_CELL_DRAW_H
#error "BDD_CELL_DRAW_H included more than once."
#endif
#define AAA_BDD_CELL_DRAW_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif

class c_ftfont;

//todo do bdd_multiple too
class	c_bdd_cell_draw final : public c_bdd
{
	FACTORY_DECLARE( c_bdd_cell_draw, c_bdd );
public:
	static	c_bdd_cell_draw*	cur;

private:
	//todo make it dynamic
	CONSTEXPR static INT32 LEVEL_MAX = 16;

protected:
	bool		_b_active_only;
	INT32		_level_max_ui;

	UINT32		_obj_id_begin_ui;
	UINT32		_obj_level_up_ui;
	c_obj_ui*	_obj_begin;

	bool		_b_obj_draw;
	REAL		_obj_size;
	bool		_b_text_draw;
	FP32		_text_size_ui;
	FP32		_text_factor_hori_ui;
	FP32		_text_tra_ui[3];
	c_ftfont*	_ft_font;

	INT32		_s_font;
	bool		_b_font_outline;
	FP32		_font_size_cached;

	bool		_b_connex_draw;
	REAL		_angle_begin;
	REAL		_angle_end;

	REAL		_size_factor;
	REAL		_level_translate;
//	REAL		_level_translate_factor;
	REAL		_level_radius;
	FP32		_level_tra_factor_ui;	//unused
	FP32		_level_rot_factor_ui;
//	FP32		_level_size_factor[LEVEL_MAX];
//	FP32		_level_tra_factor[LEVEL_MAX];
 
	REAL		_alpha_begin_ui;
	REAL		_alpha_end_ui;

	INT32		_s_draw_obj_def;
	INT32		_obj_nb_u;
	INT32		_obj_nb_v;
	FP32		_obj_color_ui[4];
	FP32		_obj_color[4];
	INT32		_s_distrib_type_ui;

	FP32		_text_color_ui[4];
	FP32		_text_color[4];

	bool		_b_branch_draw;
	FP32		_branch_color_ui[4];
	FP32		_branch_color[4];

	bool		_b_root_draw;
	FP32		_root_color_ui[4];
	FP32		_root_color[4];

	REAL		_cell_pos_begin[3];
	REAL		_cell_size_begin;

//	UINT32		_list_base;
private:
			void	draw_obj_internal( c_obj_ui* CONST obj, INT32 level );
public:

	virtual	void	param_init_pt();

	virtual	void	update();
	virtual	void	draw();

	FINLINE	void	draw_text( c_obj_ui* CONST obj, INT32 CONST level );
	FINLINE	void	branch_make_pos( REAL* CONST pos, REAL CONST size, INT32 CONST index, INT32 CONST nb_u, INT32 CONST nb_v );
	FINLINE	void	draw_branches( c_obj_ui* CONST caller, REAL CONST size, REAL CONST * CONST pos, INT32 CONST level );

//	void	prepare_lists( );
	void	cell_draw_obj_default( REAL CONST size );

//	FINLINE	REAL	get_obj_size()	{	return _obj_size; }
//	FINLINE	REAL	get_text_size()	{	return _text_size; }

	FINLINE	void	set_color( FP32* CONST color, INT32 CONST level );
};


