
#ifdef AAA_SEEDMENU_H
#error "SEEDMENU_H included more than once."
#endif
#define AAA_SEEDMENU_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

//extern	void	MAACALLBACK	menu_param_hide(int value);
//extern	void	MAACALLBACK	menu_param_show(int value);

class	c_fifo;
class	c_def_node;

class	c_menu
{
private:
	static	void			param_clipboard( INT32 CONST value );
protected:
	static	INT32			sub_focus;
	static	INT32			g_sub_tracker;
	static	INT32			sub_deformer;
	static	INT32			sub_deformer_focus;
	static	INT32			sub_deformer_remove;
	static	c_def_node*		deformer_ui_last;
private:
	static	void	MAACALLBACK	bank(					INT32 value );
	static	void	MAACALLBACK	bank_load(				INT32 value );
	static	void	MAACALLBACK	bank_save(				INT32 value );
	static	void	MAACALLBACK	camera_format(			INT32 value );
	static	void	MAACALLBACK	bdd_change(				INT32 value );
	static	void	MAACALLBACK	deformer_add(			INT32 value );
	static	void	MAACALLBACK	deformer_focus(			INT32 value );
	static	void	MAACALLBACK	deformer_remove(		INT32 value );
	static	void	MAACALLBACK	focus(					INT32 value );
	static	void	MAACALLBACK	image_inverse(			INT32 value );
	static	void	MAACALLBACK	load(					INT32 value );
	static	void	MAACALLBACK	menu_fn(				INT32 value );
	static	void	MAACALLBACK	param(					INT32 value );
	static	void	MAACALLBACK	param_hide(				INT32 value );
	static	void	MAACALLBACK	param_plug_in(			INT32 value );
	static	void	MAACALLBACK	param_plug_in_main(		INT32 value );
	static	void	MAACALLBACK	param_plug_in_mod(		INT32 value );
	static	void	MAACALLBACK	param_plug_in_group(	INT32 value );
	static	void	MAACALLBACK	param_plug_out(			INT32 value );
	static	void	MAACALLBACK	param_plug_out_main(	INT32 value );
	static	void	MAACALLBACK	param_plug_out_mod(		INT32 value );
	static	void	MAACALLBACK	param_plug_out_group(	INT32 value );
	static	void	MAACALLBACK	param_show(				INT32 value );
	static	void	MAACALLBACK	save(					INT32 value );
	static	void	MAACALLBACK	test(					INT32 value );
	static	void	MAACALLBACK	tex_1D_min(				INT32 value );
	static	void	MAACALLBACK	tex_1D_mag(				INT32 value );
	static	void	MAACALLBACK	tex_2D_min(				INT32 value );
	static	void	MAACALLBACK	tex_2D_mag(				INT32 value );
	static	void	MAACALLBACK	tex_3D_min(				INT32 value );
	static	void	MAACALLBACK	tex_3D_mag(				INT32 value );
	static	void	MAACALLBACK	tracker(				INT32 value );
	static	void	MAACALLBACK	trax_move(				INT32 value );
	static	void	MAACALLBACK	trax_move_main(			INT32 value );
	static	void	MAACALLBACK	trax_move_group(		INT32 value );
	static	void	MAACALLBACK	trax_set_type(			INT32 value );

	static	void				deformer_add( INT32 menu_base, INT32 index_focus );
	static	void				fifo_build( c_fifo* p_fifo );
	static	void				focus_init();
	static	void				param_init();
public:
	static	INT32	main_menu;
	static	INT32	aaaseed_menu;
	static	INT32	sub_bdd;
	static	INT32	focus_index;
	static	INT32	tracker_index;
	static	INT32	tree_param;
	static	INT32	param_list_id;

	static	void	redo_last();
	static	void	init();
	static	void	set_main( INT32 menu_in );

	static	void	deformer_update();
	static	void	focus_update();
	static	void	tracker_update();
	static	void	update();
};


