
#ifdef AAA_LCP_H
#error "LCP_H included more than once."
#endif
#define AAA_LCP_H 1


#define	APP_SPECIAL_LCP()			0

#if APP_SPECIAL_LCP()

#ifndef AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif


class	c_lcp final : public c_obj_active_ui
{
	FACTORY_DECLARE(c_lcp,c_obj_active_ui);
private:
	BOOL	b_first;
	REAL	time_next;
	REAL	time_next_trig;

	INT32	level;
	INT32	level_real;
	INT32	level_ask;

	BOOL	b_reg;
	INT32	reg_cur;
	INT32	reg_ask;

	BOOL	b_dep;
	INT32	dep_nb;
	INT32	dep_cur;
	INT32	dep_ask;

	INT32	circ_first;
	INT32	circ_last;
	INT32	asked;
	INT32	circ_nb;

	INT32	circ_cur;

	INT32	iq_ui;
	INT32	s_font;
	REAL	font_size;

	REAL	camera_freq;

	BOOL	b_dif;
	BOOL	b_auto;
	BOOL	b_reading;
	BOOL	b_dif_last;
	UINT32	iq_dif;
	BOOL	b_dif_trig;
	vector<INT32>	dif;
	INT32	dif_start;
	BOOL	b_dif_draw;

	BOOL	b_use_list;
public:

	void	init();
	void	reset();

	void	set_reg( INT32 value);
	void	set_dep( INT32 value);
	void	set_circ( INT32 value);
	void	set_circ_ask( INT32 value);

	void	make_reg_name( CHAR* str, INT32 reg_in);
	void	make_dep_name( CHAR* str, INT32 dep_in);
	void	make_circ_name( CHAR* str, INT32 circ_in);

	void	data_init();
	void	menu_init();
	void	menu_add_circ(INT32 i);
	void	menu_add_dep_circ( INT32 dep_in, INT32 sub_dep);
	void	menu_add_dep(INT32 i);
	void	menu_add();
	void	menu_do( INT32 value);

	BOOL	do_key( UINT8* c, BOOL b_special, INT32* modifiers, INT32* x, INT32* y);
	INT32	get_bdd_tri_id( char* name_in);

	virtual	void	param_init_pt();
	virtual	void	update();

	virtual	void	draw_on_top( INT32 x_size, INT32 y_size, INT32 x_split );

	void	dif_init();
	void	dif_remove();
	void	dif_add();
	void	dif_add_reg(INT32 reg);
	void	dif_add_dep(INT32 dep);
	void	dif_add_circ(INT32 circ);
	REAL	dif_next();

	void	dif_save();
	void	dif_load();

	virtual	AAA_ERR	save_do_after( C_PCHAR_C filename );
	virtual	AAA_ERR	load_do_after( C_PCHAR_C filename );
	};

extern	c_lcp*	lcp;

#endif	//APP_SPECIAL_LCP

