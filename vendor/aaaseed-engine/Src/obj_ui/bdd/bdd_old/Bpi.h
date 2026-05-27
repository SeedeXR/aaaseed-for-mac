
#ifdef AAA_BPI_H
#error "BPI_H included more than once."
#endif
#define AAA_BPI_H 1


#define	APP_SPECIAL_BPI() 0

#if	APP_SPECIAL_BPI()

#ifndef AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif
#ifndef AAA_FLUX_FILTER_H
#	include "infrastructure/flux_filter.h"
#endif
#ifndef AAA_DELTA_T_H
#	include "time/delta_t.h"
#endif

class	c_layer;
class	c_layers;

class	c_bpi final : public c_obj_active_ui
{
	FACTORY_DECLARE(c_bpi,c_obj_active_ui);
#define BPI_MENU_ITEM_MAX_NB	8
typedef	struct	world
	{
	REAL	cs;
	REAL	ct;
	REAL	rs;
	REAL	rt;
	REAL	ds;
	}	WORLD;
typedef	struct	module
	{
	c_layers*	layers_mod;
	c_layers*	layers_anim;
	c_layers*	layers_menu;
	c_layer*	layer_mocap;
	REAL	pos_tube[3];
	REAL	pos_world[3];
	REAL	color[3];
	REAL	open;
	REAL	opening;
	INT32	menu_nb;
	REAL	menu_height[BPI_MENU_ITEM_MAX_NB];
	REAL	menu_height_total;
	BOOL	b_change_clear_color;
	REAL	clear_grey;
	BOOL	b_change_flexus;
	REAL	flexus_transparency;
	BOOL	b_use_anim;
	}	MODULE;
private:
	void	set_flexus_visible( BOOL in);
public:
	//Flexus
	REAL	material_alpha;

	BOOL	b_flexus_on;
	//	World
	BOOL	b_world_visible;
	WORLD	world;
	REAL	world_interpolation;
	REAL	world_t_in;
	REAL	world_t_out;
	REAL	b_world_in;
	
	//	Module
	MODULE	module[4];
	REAL	module_open_time;
	REAL	module_random_out;
	REAL	module_random_in;
	REAL	module_capture_camera[3];
	c_flux_filter	module_flux;
	//	menu
	REAL	module_menu_pos_offset[3];
	REAL	module_menu_size_x;
	REAL	module_menu_size_y;
	REAL	module_menu_space_y;
	c_flux_filter	menu_flux;
	REAL	module_menu_color[3];
	REAL	module_menu_color_hilited[3];
	MODULE* mod_cur;

	//	interface
	BOOL	buttton_last;
	INT32	picked_last;

	//	MenuInfo
	REAL	menu_info_open;
	REAL	menu_info_open_bias;
	REAL	menu_info_open_time;
	REAL	menu_info_pos_in[3];
	REAL	menu_info_size;
	REAL	menu_info_pos_out[3];
	REAL	menu_info_pos_left[3];
	BOOL	menu_info_one_hit;
	c_flux_filter	menu_info_flux;
	//	video
	INT32	video_to_play;
	INT32	video_played;
	REAL	video_tempo_max;
	REAL	video_tempo;

	REAL	menu_info_changing;
	REAL	seq_cur;

	BOOL	joy_b1;
	BOOL	joy_b2;

	REAL	mds;
	REAL	mt;
	REAL	mr;
	REAL	mangle;

private:
	void	make_world_coor( REAL* out, REAL *in, REAL pitch, c_flux_filter& flux, REAL* left = NULL, REAL size = 0 );
	void	module_close_all();
	void	module_close( MODULE* mod);
	void	module_set_picking( BOOL b_picking_on);
	void	module_set_active( MODULE* mod, BOOL b_on);
	void	module_set_active_except( MODULE* mod, BOOL b_in );
	void	world_enter();
	void	world_exit();
	void	play_video( INT32 index);

public:
			INT32	get_seq();

			void	init();
	virtual	void	param_init_pt();

	virtual	void	update();

	virtual	void	about();
};

extern	c_bpi*		bpi;

#endif	//APP_SPECIAL_BPI

