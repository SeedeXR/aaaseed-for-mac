#include "bpi.h"
#include "draw/bind_img.h"
#include "draw/color.h"
#include "draw/mat.h"
#include "draw/model.h"
#include "draw/picking.h"
#include "draw/seedcam.h"
#include "infrastructure/bind/bind.h"
#include "infrastructure/layer/layer.h"
#include "infrastructure/layer/layers.h"
#include "infrastructure/layer/module.h"
#include "math/aaa_math.h"
#include "math/rand.h"
#include "math/v_base.h"
#include "media/video/movie_ms.h"
#include "obj_ui/flexus.h"
#include "obj_ui/bdd/bdd_geo/bdd_proj_cone.h"
#include "obj_ui/bdd/bdd_geo/bdd_tube_path.h"
#include "obj_ui/bdd/bdd_point/bdd_mocap.h"
#include "obj_ui/bdd/bdd_spe/bdd_movie.h"
#include "obj_ui/deformer/def_node.h"
#include "obj_ui/transfo/transfo_trs.h"

FACTORY_CREATE_V1( c_bpi, bpi, Bpi, bpi );

c_bpi*	bpi = NULL;


namespace n_bpi
{
	static	CONST	INT32	BASE_PARAM_NB			= 2;
	static	CONST	INT32	IN_PARAM_NB				= 2;
	static	CONST	INT32	WORLD_PARAM_NB			= 9;
	static	CONST	INT32	MODULES_BASE_PARAM_NB	= 22;
	static	CONST	INT32	MODULE_NB				= 4;
	static	CONST	INT32	MODULE_PARAM_NB			= 16;
	static	CONST	INT32	MODULES_ALL_PARAM_NB	= MODULE_NB * ( 1 + MODULE_PARAM_NB ) + MODULES_BASE_PARAM_NB;
	static	CONST	INT32	MENUINFO_PARAM_NB		= 14;
	static	CONST	INT32	VIDEO_PARAM_NB			= 1;
	static	CONST	INT32	GROUP_PARAM_NB			= 5;
	static	CONST	INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
											+	IN_PARAM_NB
											+	WORLD_PARAM_NB
											+	MODULES_ALL_PARAM_NB
											+	MENUINFO_PARAM_NB
											+	VIDEO_PARAM_NB
											+	GROUP_PARAM_NB;

	static	ST_PARAM	param[PARAM_NB_MAX] =
	{
		ST_PARAM_BOOL_OFF( active )
		{	NULL,	PARAM_REAL,	"flexus_alpha",		1., .77,	-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
	//		ST_PARAM_GROUP( states, 2 )

		ST_PARAM_GROUP_CLOSED( In, IN_PARAM_NB )
			ST_PARAM_BOOL_OFF( button_1 )
			ST_PARAM_BOOL_OFF( button_2 )

		ST_PARAM_GROUP_CLOSED( World, WORLD_PARAM_NB )
			ST_PARAM_REAL_ZERO( world_cs )
			ST_PARAM_REAL_ZERO( world_ct )
			ST_PARAM_REAL_ZERO( world_ds )
			ST_PARAM_REAL_ZERO( world_rs )
			ST_PARAM_REAL_ZERO( world_rt )
			ST_PARAM_REAL_ZERO( world_interpolation )
			ST_PARAM_REAL_ZERO( world_t_in )
			ST_PARAM_REAL_ZERO( world_t_out )
			ST_PARAM_BOOL_OFF( world_in )

		ST_PARAM_GROUP_CLOSED( Modules, MODULES_ALL_PARAM_NB )
			ST_PARAM_REAL_ONE( module_ds )
			ST_PARAM_REAL_ONE( module_t )
			ST_PARAM_REAL_ONE( module_r )
			ST_PARAM_REAL_ONE( module_angle )
			ST_PARAM_REAL_ONE( module_open_time )
			ST_PARAM_REAL_ONE( module_random_out )
			ST_PARAM_REAL_ONE( module_random_in )
			ST_PARAM_SCALE_XYZ( module_capture_cam )
			ST_PARAM_POINT_XYZ( module_menu_pos_offset )
			ST_PARAM_SCALE_XY( module_menu_size )
			ST_PARAM_REAL_ONE( module_menu_space_y )
			ST_PARAM_REAL_ZERO( module_menu_color_r )
			ST_PARAM_REAL_ZERO( module_menu_color_g )
			ST_PARAM_REAL_ZERO( module_menu_color_b )
			ST_PARAM_REAL_ZERO( module_menu_color_hilited_r )
			ST_PARAM_REAL_ZERO( module_menu_color_hilited_g )
			ST_PARAM_REAL_ZERO( module_menu_color_hilited_b )

			ST_PARAM_GROUP_CLOSED( Module_1, MODULE_PARAM_NB )
				ST_PARAM_REAL_ZERO( m1_open )
				ST_PARAM_REAL_ZERO( m1_s )
				ST_PARAM_REAL_ZERO( m1_y )
				ST_PARAM_REAL_ZERO( m1_t )
				ST_PARAM_POINT_XYZ( m1_world )
				ST_PARAM_REAL_ZERO( m1_color_r )
				ST_PARAM_REAL_ZERO( m1_color_g )
				ST_PARAM_REAL_ZERO( m1_color_b )
				{	NULL,	PARAM_INT32,	"m1_menu_nb",	1., 0,		0, BPI_MENU_ITEM_MAX_NB,	NULL, NULL },
				ST_PARAM_BOOL_OFF( ml_clear_color_change )
				ST_PARAM_REAL_ZERO( m1_color_color_grey )
				ST_PARAM_BOOL_OFF( ml_flexus_change )
				ST_PARAM_REAL_ZERO( m1_flexus_alpha )
				ST_PARAM_BOOL_OFF( ml_anim )
			ST_PARAM_GROUP_CLOSED( Module_2, MODULE_PARAM_NB )
				ST_PARAM_REAL_ZERO( m2_open )
				ST_PARAM_REAL_ZERO( m2_s )
				ST_PARAM_REAL_ZERO( m2_y )
				ST_PARAM_REAL_ZERO( m2_t )
				ST_PARAM_POINT_XYZ( m2_world )
				ST_PARAM_REAL_ZERO( m2_color_r )
				ST_PARAM_REAL_ZERO( m2_color_g )
				ST_PARAM_REAL_ZERO( m2_color_b )
				{	NULL,	PARAM_INT32,	"m2_menu_nb",	1., 0,		0, BPI_MENU_ITEM_MAX_NB,	NULL, NULL },
				ST_PARAM_BOOL_OFF( m2_clear_color_change )
				ST_PARAM_REAL_ZERO( m2_color_color_grey )
				ST_PARAM_BOOL_OFF( m2_flexus_change )
				ST_PARAM_REAL_ZERO( m2_flexus_alpha )
				ST_PARAM_BOOL_OFF( m2_anim )
			ST_PARAM_GROUP_CLOSED( Module_3, MODULE_PARAM_NB )
				ST_PARAM_REAL_ZERO( m3_open )
				ST_PARAM_REAL_ZERO( m3_s )
				ST_PARAM_REAL_ZERO( m3_y )
				ST_PARAM_REAL_ZERO( m3_t )
				ST_PARAM_POINT_XYZ( m3_world )
				ST_PARAM_REAL_ZERO( m3_color_r )
				ST_PARAM_REAL_ZERO( m3_color_g )
				ST_PARAM_REAL_ZERO( m3_color_b )
				{	NULL,	PARAM_INT32,	"m3_menu_nb",	1., 0,		0, BPI_MENU_ITEM_MAX_NB,	NULL, NULL },
				ST_PARAM_BOOL_OFF( ml_clear_color_change )
				ST_PARAM_REAL_ZERO( m3_color_color_grey )
				ST_PARAM_BOOL_OFF( ml_flexus_change )
				ST_PARAM_REAL_ZERO( m3_flexus_alpha )
				ST_PARAM_BOOL_OFF( ml_anim )
			ST_PARAM_GROUP_CLOSED( Module_4, MODULE_PARAM_NB )
				ST_PARAM_REAL_ZERO( m4_open )
				ST_PARAM_REAL_ZERO( m4_s )
				ST_PARAM_REAL_ZERO( m4_y )
				ST_PARAM_REAL_ZERO( m4_t )
				ST_PARAM_POINT_XYZ( m4_world )
				ST_PARAM_REAL_ZERO( m4_color_r )
				ST_PARAM_REAL_ZERO( m4_color_g )
				ST_PARAM_REAL_ZERO( m4_color_b )
				{	NULL,	PARAM_INT32,	"m4_menu_nb",	1., 0,		0, BPI_MENU_ITEM_MAX_NB,	NULL, NULL },
				ST_PARAM_BOOL_OFF( ml_clear_color_change )
				ST_PARAM_REAL_ZERO( m4_color_color_grey )
				ST_PARAM_BOOL_OFF( ml_flexus_change )
				ST_PARAM_REAL_ZERO( m4_flexus_alpha )
				ST_PARAM_BOOL_OFF( ml_anim )
		ST_PARAM_GROUP_CLOSED( MenuInfo, MENUINFO_PARAM_NB )
			{	NULL,	PARAM_REAL,	"menu_info_open",		1., 0,		0, 1,					NULL, NULL },
			{	NULL,	PARAM_REAL,	"menu_info_open_bias",	.75, .5,	BIAS_MIN, BIAS_MAX,		NULL, NULL },
			{	NULL,	PARAM_REAL,	"menu_info_open_time",	0., .5,		0, 1,					NULL, NULL },
			ST_PARAM_POINT_XYZ( menu_info_in )
			ST_PARAM_REAL_ZERO( menu_info_size )
			ST_PARAM_POINT_XYZ( menu_info_out )
			ST_PARAM_POINT_XYZ( menu_info_left )
			ST_PARAM_BOOL_OFF( menu_info_one_hit )
		ST_PARAM_GROUP_CLOSED( Video, VIDEO_PARAM_NB )
			ST_PARAM_REAL_POS_ONE( video_tempo )
		};
}

void	c_bpi::param_init_pt()
{
INT32	h = 0;
	param_set_pt( h, get_pt_active() );
	param_set_pt( h, material_alpha);

	h++;
		param_set_pt( h, joy_b1);
		param_set_pt( h, joy_b2);

	h++;
		param_set_pt( h, world.cs );
		param_set_pt( h, world.ct );
		param_set_pt( h, world.ds );
		param_set_pt( h, world.rs );
		param_set_pt( h, world.rt );
		param_set_pt( h, world_interpolation);
		param_set_pt( h, world_t_in);
		param_set_pt( h, world_t_out);
		param_set_pt( h, b_world_in);

	h++;
		param_set_pt( h, mds);
		param_set_pt( h, mt);
		param_set_pt( h, mr);
		param_set_pt( h, mangle);
		param_set_pt( h, module_open_time);
		param_set_pt( h, module_random_out);
		param_set_pt( h, module_random_in);
		param_set_pt_v3( h, module_capture_camera );
		param_set_pt_v3( h, module_menu_pos_offset );
		param_set_pt( h, module_menu_size_x);
		param_set_pt( h, module_menu_size_y);
		param_set_pt( h, module_menu_space_y);
		param_set_pt_v3( h, module_menu_color );
		param_set_pt_v3( h, module_menu_color_hilited );

	for( INT32 i = 0; i < n_bpi::MODULE_NB; ++i )
		{
		MODULE*	mod = &module[i];
		h++;
		param_set_pt( h, mod->open );
		param_set_pt_v3( h, mod->pos_tube );

		param_set_pt_v3( h, mod->pos_world );
		param_set_pt_v3( h, mod->color );

		param_set_pt( h, mod->menu_nb );
		param_set_pt( h, mod->b_change_clear_color );
		param_set_pt( h, mod->clear_grey );
		param_set_pt( h, mod->b_change_flexus );
		param_set_pt( h, mod->flexus_transparency );
		param_set_pt( h, mod->b_use_anim );
		}

	h++;
		param_set_pt( h, menu_info_open);
		param_set_pt( h, menu_info_open_bias);
		param_set_pt( h, menu_info_open_time);
		param_set_pt_v3( h, menu_info_pos_in );
		param_set_pt( h, menu_info_size);
		param_set_pt_v3( h, menu_info_pos_out );
		param_set_pt_v3( h, menu_info_pos_left );
		param_set_pt( h, menu_info_one_hit);

	h++;
		param_set_pt( h, video_tempo_max);

	err_param_init_pt(h);
}

void	c_bpi::init()
{
	buttton_last = FALSE;
	picked_last = 0;
	menu_info_changing = 0;
	menu_info_open = 0.;
}

CONSTRUCTOR_CREATE(c_bpi)
{
	param_init_with( n_bpi::param, n_bpi::PARAM_NB_MAX ); // bpi_param, BPI_PARAM_NB_MAX);
	init();
}

c_bpi::~c_bpi()
{
}



static	BOOL	b_first_worlds = TRUE;



static	c_layers*	layers_world1;
static	c_layers*	layers_menuinfo;
static	c_layers*	layers_video;
static	c_bdd_movie*	bdd_movie = NULL;
char	str_movie_filename[] = "Video/Mod1/Seq00.mpg";



void	c_bpi::update()
{
BOOL	b_tmp;
//INT32	i;
MODULE*	mod;
REAL	r_tmp;
REAL	dt;

	if( !is_active() || !flexus )
		return;
c_module*	pt_module;
	pt_module = c_module::get_cur();	//todo now this should be changed was done with one module before
	if( !pt_module)
		return;
	dt = flexus->get_dt();

	if( b_first_worlds )
		{
			for( INT32 i = 0; i < n_bpi::MODULE_NB; ++i )
			{
			char str[] = "Env/Module1/Mod/fx";
			INT32	layers_index_module;
			*(str+10) = '1'+i;
			layers_index_module	= pt_module->get_layers_bind_list()->find( str );
			mod = &module[i];
			if( layers_index_module >=0 )
				{
				mod->layers_mod = pt_module->layers_get_from_index(layers_index_module );
				mod->layers_anim = pt_module->layers_get_from_index(++layers_index_module );
				mod->layers_menu = pt_module->layers_get_from_index(++layers_index_module );
				mod->layer_mocap = mod->layers_mod->layer_get(2);
				if( mod->layer_mocap )
					mod->layer_mocap->set_active( FALSE );
				}
			else
				{
				mod->layers_mod = NULL;
				mod->layers_anim = NULL;
				mod->layers_menu = NULL;
				mod->layer_mocap = NULL;
				}
			module_set_active( mod, FALSE);
			}
		mod = &module[0];
		if( mod )
			{
			mod->menu_height[0] = 1.;
			mod->menu_height[1] = 2.;
			mod->menu_height[2] = 1.;
			mod->menu_height[3] = 1.;
			mod->menu_height[4] = 1.;
			mod->menu_height[5] = 1.;
			mod->menu_height_total = 7;
			}

		layers_world1 = pt_module->layers_get_from_name_short( "World1" );
		layers_menuinfo = pt_module->layers_get_from_name_short( "MenuInfo" );
		layers_video = pt_module->layers_get_from_name_short( "Video" );
		if( layers_video )
		{
			bdd_movie = layers_video->layer_get_always(1)->bdd_get_always<c_bdd_movie>();
		}

//		g_app->set_clear_color_custom_to_grey(.5);
		
		menu_info_flux.set_filter_factor(.8);
		module_flux.set_filter_factor(.85);
		menu_flux.set_filter_factor(.75);
		world_exit();
		b_world_visible = FALSE;
		set_flexus_visible( FALSE);
		set_flexus_visible( TRUE);
		mod_cur = NULL;
		seq_cur = 0;
		b_first_worlds = FALSE;
		return;
		}

	if( b_world_in )
		r_tmp = 1.;
	else
		{
		r_tmp = ABS ( flexus->get_coor_in_path()[0] - world.cs ) / (world.ds);
		r_tmp = CLAMP ( (DOUBLE) r_tmp, (DOUBLE)0., (DOUBLE)1. );
		}
	world_interpolation = flexus->get_coor_in_path()[2]/world.ct * r_tmp;
	world_interpolation = CLAMP( world_interpolation, (REAL)0., (REAL)1. );

REAL	tube_start;
REAL	tube_stop;
	c_bdd_tube_path*	bdd_tube_path = flexus->get_bdd_tube_path();
	if( bdd_tube_path )
	{		
		tube_start = bdd_tube_path->get_center() - bdd_tube_path->get_len_before();
		tube_stop = bdd_tube_path->get_center() + bdd_tube_path->get_len_after();
	}
	b_tmp = FALSE;
	if( b_world_in || (tube_start < world.cs && world.cs < tube_stop ) )
		{
		if( !b_world_visible)
			{
			b_world_visible = TRUE;
			b_tmp = TRUE;			
			}
		}
	else
		{
		if( b_world_visible )
			{
			b_world_visible = FALSE;
			b_tmp = TRUE;
			}
		}
	if( b_tmp )
		{
		if( layers_world1 )
			layers_world1->set_active( b_world_visible );
		for( INT32 i = 0; i < n_bpi::MODULE_NB; ++i )
			{
			module_set_active( &module[i], b_world_visible );
			}
		}


	if( b_world_in )
		{
			if( flexus->get_coor_in_path()[2] <= world_t_out )
			world_exit();
		}
	else
		{
		if( flexus->get_coor_in_path()[2] >= world_t_in )
			world_enter();
		}
//VIDEO
	if( video_played >= 0 )
		{
		if( !bdd_movie->is_playing() )
			{
			bdd_movie->close();
			video_played = -1;
			video_tempo = 0.;
			if( mod_cur && (video_to_play < (mod_cur->menu_nb-2)) )
				video_to_play++;
			else
				video_to_play = -1;
			}
		}
	else if( mod_cur )
		{
		video_tempo += dt;
		if( video_tempo > video_tempo_max && video_to_play >= 0)
			{
			play_video( video_to_play);
			}
		}

//INTERFACE
#ifdef	PICKING
c_layer*	layer;

	if( c_picking::is_cur() && (buttton_last != joy_b1) )
		{
		INT32	new_hit = c_picking::cur_get_pick_ref_max();
		if( !joy_b1 )
			{
			if( picked_last == new_hit )
				{
				switch( new_hit )
					{
					case 1:	// open menu
						menu_info_changing = 1.;
						break;
					case 8:	//	close menu
						menu_info_changing = -1.;
						break;
					case 16:
					case 17:
					case 18:
					case 19:
					case 20:
					case 21:
					case 22:
					case 23:
					case 24:
					case 25:
						{
						INT32 index = new_hit - 16;
						if( bdd_movie )
							if ( mod_cur && (index >= mod_cur->menu_nb-1) )
								module_close( mod_cur );
							else
								play_video(index);
						}
						break;
					case 32:	//	modules
					case 33:
					case 34:
					case 35:
						if( b_world_in )
							{
							INT32	index_module = new_hit-32;
							mod = &module[new_hit-32];
							if( mod->opening == 0. )
								{
								if( mod->open == 0. )
									{	// open the module 
									module_close_all();
									mod->opening = 1;	
									video_to_play = 0;
									seq_cur = 0;
									if( mod->layers_menu )
										mod->layers_menu->set_active( TRUE );
									if( mod->layers_anim )
										mod->layers_anim->set_active( TRUE );
									if( mod->layer_mocap )
										mod->layer_mocap->set_active( TRUE );
									}
								else if( mod->open == 1. )
									{	// close the module
									module_close( mod );
									}
								}
							}
						break;
					}
				}
			}
		picked_last = new_hit;
		buttton_last = joy_b1;
		}
	if( menu_info_changing != 0 )
		{
		menu_info_open += menu_info_changing * dt / menu_info_open_time;
		if ( menu_info_open < 0. )
			{
			menu_info_open = 0.;
			menu_info_changing = 0.;
			}
		else if ( menu_info_open > 1. )
			{
			menu_info_open = 1.;
			menu_info_changing = 0.;
			}
		}
//MODULE
REAL	angle_start = - mangle * .5 + .25;
REAL	angle_step = mangle /( n_bpi::MODULE_NB - 1 );
	for( INT32 i = 0; i < n_bpi::MODULE_NB; ++i )
		{
		mod = &module[i];
		//opening
		if( mod->opening != 0 )
			{
			mod->open += mod->opening * dt / module_open_time;
			if ( mod->open < 0. )
				{
				mod->open = 0.;
				mod->opening = 0.;
				if( mod->layers_anim )
					mod->layers_anim->set_active( FALSE );
				if( mod->layer_mocap )
					mod->layer_mocap->set_active( FALSE );
				if( mod->layers_menu )
					mod->layers_menu->set_active( FALSE );
				module_set_active_except( mod, TRUE );
				}
			else if ( mod->open > 1. )
				{
				mod->open = 1.;
				mod->opening = 0.;
				mod_cur = mod;
				video_tempo = video_tempo_max;
				video_to_play = 0;
				module_set_active_except( mod, FALSE );
				}
			if( mod->b_change_clear_color)
				{
//				r_tmp = interpolate( .5, mod->clear_grey, mod->open);
//				g_app->set_clear_color_custom_to_grey(r_tmp);
				}
			if( mod->b_change_flexus)
				{
				r_tmp = interpolate( material_alpha, mod->flexus_transparency, mod->open );
				c_materials::cur->set_alpha( 8, r_tmp );
				if( mod->flexus_transparency == 0. && mod->open == 1. )
					{
					set_flexus_visible( FALSE );
					flexus->set_freeze_ship_position( TRUE );
					}
				else 
					{
					set_flexus_visible( TRUE );
					flexus->set_freeze_ship_position( FALSE );
					}
				}
			if( mod->b_use_anim )
				{
				c_materials::cur->set_alpha( 9, mod->open );
				c_materials::cur->set_alpha( 10, mod->open );
				}
			}
		}
	for( INT32 i = 0; i < n_bpi::MODULE_NB; ++i )
		{
		REAL	src1[3];
		REAL	src2[3];
		REAL	pos[3];		
		mod = &module[i];
		if ( mod->layers_mod )
			{
			//position
//			src1[0] = mds/s_size * COS_INT( i*.25 + time * .5);
//			src1[1] = mds * SIN_INT( i*.25 + time * .5);
			src1[0] = 0;
			src1[1] = 0;
			src1[2] = flexus->get_t_size()*mt;

			src2[0]	= mr/flexus->get_s_size() * COS_INT( angle_start );
			src2[1]	= 0;
			src2[2]	= world.ct + mr * SIN_INT( angle_start );

REAL	inter;
			inter = world_interpolation*1.5;
			inter = CLAMP( inter, (REAL)0., (REAL)1. );
			interpolate_v3r( pos, src1, src2, inter );


			pos[0] += world.cs;
	//		cpy_v3r( &mod->pos_tube[0], pos);
			flexus->get_bdd_mocap()->coor_to_world( &mod->pos_world[0], flexus->get_tube_channel(), pos );
			r_tmp = interpolate( module_random_out, module_random_in, inter );
			add_scale_v3r( pos, mod->pos_world, mod->pos_tube, r_tmp );
			if( flexus->get_deformer_flexus() )
				flexus->get_deformer_flexus()->apply( pos, 1 );

			if ( mod->open != 0. )
				{
				REAL	vec[3];
				make_world_coor( vec, module_capture_camera, flexus->get_pitch_ship_to_world(), module_flux );
				if( mod->open == 1. )
					cpy_v3r( mod->pos_world, vec );
				else
					interpolate_v3r( mod->pos_world, pos, vec, mod->open );
				//menu
				if ( mod->menu_nb )
					{
					//INT32	j;
					REAL	y_step;
					REAL	size[3];
					add_scale_v3r( vec, module_capture_camera, module_menu_pos_offset, mod->open );
					make_world_coor( vec, vec, flexus->get_pitch_ship_to_world(), menu_flux );
					if( mod->open != 1. )
						interpolate_v3r( vec, pos, vec, mod->open );
					y_step = module_menu_size_y / REAL(mod->menu_nb) * mod->open;
					size[0] = mod->open;
					size[2] = module_menu_size_x * mod->open; 
					for( INT32 j = 0; j < mod->menu_nb; ++j )
						{
						c_transfo_trs* transfo;
						c_model*		model;
						c_color*		color;
						layer = mod->layers_menu->layer_get( j );
						if( layer )
							{
							//layer->set_s_bdd( (menu_info_open == 1.)?1:2 );	// draw and pick only what we need
							//layer->set_active( TRUE );	// draw and pick only what we need
							transfo = layer->get_transfo_trs1();
							if( transfo )
								transfo->set_tra( vec );
							model = layer->get_model();
							r_tmp = y_step * mod->menu_height[j];
							if( model )
								{
								size[1] = r_tmp; 
								model->set_size_v3r( &size[0]);
								}
							color = layer->get_color();
							if( color )
								{
								color->set_rgb_v3r( (j==video_to_play) ? module_menu_color_hilited : module_menu_color );
								}
							}
						vec[1] -= (r_tmp + y_step * mod->menu_height[j+1])*.5 + module_menu_space_y;
						}
					}
				}
			else
				cpy_v3r( mod->pos_world, pos );
			//projection cone
			layer = mod->layers_mod->layer_get(1);
			if( layer )
				{
				((c_bdd_proj_cone*)(layer->get_bdd()))->set_interpolation( mod->open );
				}
			}

		angle_start += angle_step;
		}

//MENUINFO	it's the contextual menu
	make_world_coor( menu_info_pos_out, menu_info_pos_in, flexus->get_pitch_ship_to_world(), menu_info_flux, menu_info_pos_left, menu_info_size );

	menu_info_one_hit =  c_picking::is_cur() && (c_picking::cur_get_hits_nb()==1);
	if( layers_menuinfo )
		{
		c_transfo_trs* transfo;
		layer = layers_menuinfo->layer_get( 0);
		if( layer )
			layer->set_s_bdd( (menu_info_open == 1. ) ? 1 : 2 );	// draw and pick only what we need
		for ( INT32 i = 1; i <= 7; ++i )
			{
			layer = layers_menuinfo->layer_get( i);
			if( layer )
				{
				if( menu_info_open != 0. )
					{
					layer->set_active( TRUE );	// draw and pick only what we need
					transfo = layer->get_transfo_trs1();
					if( transfo )
						{
						REAL	tmp = ( REAL(i) - 1.) / 6.;
						REAL	vec[3];
						//bias( tmp, menu_info_open);
						interpolate_v3r( vec, menu_info_pos_left, menu_info_pos_out, tmp );
						transfo->set_tra( vec );
						}
					}
				else
					{
					layer->set_active( FALSE );	// draw and pick only what we need
					}
				}
			}
		}
#endif	//PICKING

}


void	c_bpi::make_world_coor( REAL* out, REAL *in, REAL pitch, c_flux_filter& flux, REAL* left, REAL size )
{
//MENUINFO	it's the contextual menu
REAL	vec[3];
REAL	cos, sin;
REAL	tmp;
	tmp = SQRT(in[0]*in[0]+in[2]*in[2]);
	//	this menu is linked to the ship
	cos = COS_INT( flexus->get_cam_rot()[0]-flexus->get_hmd_rot()[0] ) * COS_INT(pitch);
	sin = SIN_INT( flexus->get_cam_rot()[0]-flexus->get_hmd_rot()[0] ) * COS_INT(pitch);

	cpy_v3r( vec, &flexus->get_cam_pos()[0] );
	vec[0] += in[0] * cos;
	vec[2] += in[0] * sin;
	vec[0] -= in[2] * sin;
	vec[2] += in[2] * cos;
	//	frb want inertia
	flux.put_3d( vec);
	flux.get_3d( vec);
	// we don't want the menu to float
	vec[1] = flexus->get_cam_pos()[1];
	vec[1] += in[1] * COS_INT(pitch) - SIN_INT(pitch)*tmp;
	cpy_v3r( out, vec);

	if( left )
		{
		tmp = menu_info_open;
		bias( tmp, menu_info_open_bias);
		vec[0] -= size * tmp * cos;
		vec[2] -= size * tmp * sin;
		cpy_v3r( left, vec);
		}
}

void	c_bpi::module_close( MODULE* mod )
{
	mod->opening = -1;
	video_to_play = -1;
	if( bdd_movie)
		bdd_movie->close();
	if( mod == mod_cur )
		mod_cur = NULL;
}

void	c_bpi::module_close_all()
{
	for( INT32 i = 0; i < n_bpi::MODULE_NB; ++i )
		{
		MODULE*	mod = &module[i];
		mod->opening = -1.;
		}
	video_to_play = -1;
	if( bdd_movie)
		bdd_movie->close();
	mod_cur = NULL;
	seq_cur = 0;

}

void	c_bpi::module_set_picking( BOOL b_picking_on)
{
c_layer*	layer;
	for( INT32	i = 0; i < n_bpi::MODULE_NB; ++i )
		{
		MODULE*	mod = &module[i];
		if( mod->layers_mod )
			{
			layer = mod->layers_mod->layer_get(0);
			if( layer )
				layer->set_picking_ref( b_picking_on?(32+i):0 );
			}
		}
}

void	c_bpi::module_set_active( MODULE* mod, BOOL b_on)
{
	if( mod->layers_mod )
		mod->layers_mod->set_active( b_on);
	if( !b_on)
		{
		if( mod->layers_anim )
			mod->layers_anim->set_active( b_on);
		if( mod->layer_mocap )
			mod->layer_mocap->set_active( b_on );
		if( mod->layers_menu )
			mod->layers_menu->set_active( b_on);
		}
}

void	c_bpi::module_set_active_except( MODULE* mod, BOOL b_in )
{
	for( INT32 i = 0; i < n_bpi::MODULE_NB; ++i )
		{
		if( &module[i] != mod )
			module_set_active( &module[i], b_in );
		}
}

void	c_bpi::world_enter()
{
	b_world_in = TRUE;
	module_set_picking( TRUE);
}

void	c_bpi::world_exit()
{
	module_close_all();
	b_world_in = FALSE;
	module_set_picking( FALSE);
	video_to_play = -1;
	video_played = -1;

}

void	c_bpi::set_flexus_visible( BOOL in)
{
	if( b_flexus_on != in)
	{
		if( flexus->get_layers_flexus() )
			flexus->get_layers_flexus()->set_active( in );
		if( b_world_visible )
			layers_world1->set_active( in);
		b_flexus_on = in;
	}
}

void	c_bpi::play_video( INT32 index)
{
	video_to_play = index;
	if( bdd_movie )
		{
		bdd_movie->close();
		str_movie_filename[15] = '1'+ video_to_play;
		//movie_choose_and_play_async( str_movie_filename);

		bdd_movie->set_movie_filename( str_movie_filename);
		bdd_movie->open();
		bdd_movie->start();
		video_played = video_to_play;
		}
}

BOOL	b_jump = FALSE;
INT32	c_bpi::get_seq()
{
	if( b_world_in )
		{
		seq_cur++;
/*		if ( b_jump )
			{
			seq_cur++;
			b_jump = FALSE;
			}
		else
			{
			if( video_to_play >= 0 )
				{
				seq_cur = video_to_play * 2;
				}
			b_jump = TRUE;
			}
*/
		seq_cur = IMOD( seq_cur, 9);
		}
	else
		{
		seq_cur = 0;
		b_jump = FALSE;
		}
	return	seq_cur;
}

void	c_bpi::about()
{
	BOX_ABOUT( "Prototype v1.0 de l'exposition \"Regard d'un si�cle\"\n"
					"\tProducteur Biblioth�que Publique d'Information\n"
					"\t\tCentre Georges Pompidou\n"
					"\tProducteur Executif Z-A production Thierry Prieur\n"
					"\tLogiciel M�a\n"
					"\tSc�nographie R, D & Sie . D / B :L\n"
					);
}

