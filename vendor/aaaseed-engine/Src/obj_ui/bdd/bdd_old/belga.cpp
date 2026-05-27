#include "belga.h"
#include "infrastructure/layer/layers.h"
#include "draw/mat.h"
#include "math/aaa_math.h"
#include "draw/bind_img.h"
#include "time/ourtime.h"
#include "math/rand.h"
#include "draw/model.h"
#include "draw/render.h"
#include "infrastructure/layer/layer.h"
#include "draw/seedcam.h"
#include "obj_ui/com/net.h"

FACTORY_CREATE_V1( c_belga, belga, Belga, belga );

BOOL		b_mode_wait;

c_belga*	belga = NULL;

#define	LAYERS_CAMERA			2
#define	LAYERS_BODYRENDER		9
#define	LAYERS_RING				39
#define	LAYERS_SOL				40
#define	LAYERS_MENU				41
#define	LAYERS_PICTO			42
#define	LAYERS_LOGO				43
#define	LAYERS_SOUSTITRE		44
#define	LAYERS_SOUND_START		50
#define	LAYERS_SOUND_STOP		53

INT32	borne_loc_nb = 1;

REAL	ring_cur;

#define	READY		0
#define	PLAY		1
#define	WAIT		2

INT32	player_state[4];
INT32	belga_state;

void	c_belga::players_ready(BOOL b_snd)
{
	player_cur = -1;
	player_state[0] = READY;
	if( net)
		net->send_ascii( 1, "S");
//	the_button[0] = FALSE;

	player_state[1] = READY;
	if( net)
		net->send_ascii( 2, "S");
//	the_button[1] = FALSE;

	player_state[2] = READY;
	if( net)
		net->send_ascii( 3, "S");
//	the_button[2] = FALSE;

	player_state[3] = READY;
	if( net)
		net->send_ascii( 4, "S");
//	the_button[3] = FALSE;

	b_mode_wait = TRUE;
	map_fond = 0;
	map_sphere = 0;

	if ( b_snd )
		snd_click_out = TRUE;

	borne_loc_nb = IMOD( borne_loc_nb, 4) + 1;
}

void	c_belga::player_play(INT32 index)
{
//INT32	i;

	for( INT32 i = 0; i < 4; ++i )
		{
		if ( i == index )
			{
			player_state[i] = PLAY;
			//start joystick
			if( net)
				net->send_ascii( i+1, "T");
			player_cur = i;
			}
		else
			{
			player_state[i] = WAIT;
			//start joystick
			if( net)
				net->send_ascii( i+1, "S");
			}
		}
	b_mode_wait = FALSE;
	map_fond = 1+index;
	map_sphere = 1+index;
//	map_sphere = 2+INT32(n_aaatime::get())&0x3;
	snd_click_in = TRUE;


	time_last_joystick_move = n_aaatime::get();
}

namespace n_belga
{
	static	CONST	INT32	BASE_PARAM_NB				= 2;
	static	CONST	INT32	STATES_PARAM_NB				= 2;
	static	CONST	INT32	PLAYER_PARAM_NB				= 5;
	static	CONST	INT32	RING_PARAM_NB				= 12;
	static	CONST	INT32	MAP_PARAM_NB				= 2;
	static	CONST	INT32	JOYSTICK_OUT_PARAM_NB		= 8;
	static	CONST	INT32	JOYSTICK_CONTROL_PARAM_NB	= 19;
	static	CONST	INT32	JOYSTICK_ACCEL_PARAM_NB		= 9;
	static	CONST	INT32	BORNE_PARAM_NB				= 7;
	static	CONST	INT32	SOUND_PARAM_NB				= 8;
	static	CONST	INT32	SUBTITLE_PARAM_NB			= 2;
	static	CONST	INT32	GROUP_PARAM_NB				= 10;

	static	CONST	INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
											+	STATES_PARAM_NB
											+	PARAM_NB_MAX
											+	RING_PARAM_NB
											+	MAP_PARAM_NB
											+	JOYSTICK_OUT_PARAM_NB
											+	JOYSTICK_CONTROL_PARAM_NB
											+	JOYSTICK_ACCEL_PARAM_NB
											+	BORNE_PARAM_NB
											+	SOUND_PARAM_NB
											+	SUBTITLE_PARAM_NB
											+	GROUP_PARAM_NB;


	static	ST_PARAM	param[PARAM_NB_MAX] =
	{
		ST_PARAM_BOOL_OFF( active )

		ST_PARAM_GROUP( states, STATES_PARAM_NB )
			ST_PARAM_BOOL_OFF( MonoScreen )
			ST_PARAM_BOOL_OFF( mode_wait )
		{	NULL,	PARAM_REAL,	"tempo_out",	15., 10.,	1., PARAM_INFINI,	NULL, NULL },

		ST_PARAM_GROUP( player, PLAYER_PARAM_NB )
			{	NULL,	PARAM_INT32,	"player_blue",		.1, 0.,		0, 2,	NULL, NULL },
			{	NULL,	PARAM_INT32,	"player_white",		.1, 0.,		0, 2,	NULL, NULL },
			{	NULL,	PARAM_INT32,	"player_violet",	.1, 0.,		0, 2,	NULL, NULL },
			{	NULL,	PARAM_INT32,	"player_black",		.1, 0.,		0, 2,	NULL, NULL },
			{	NULL,	PARAM_INT32,	"player_cur",		.1, 0.,		-1, 3,	NULL, NULL },

		ST_PARAM_GROUP( ring, RING_PARAM_NB )
			{	NULL,	PARAM_REAL,	"ring_range",				.1, .2,		-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,	"ring_angle",				.25, 0.,	-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,	"ring_cur",					.0, 1.,		-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,	"ring_rot",					.25, 0.,	-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,	"ring_sticker_rot",			.25, 0.,	-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,	"ring_sticker_rot_factor",	.25, 0.,	-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,	"ring_alpha",				.25, 0.,	-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,	"ring_alpha_bias",			.5, .5,		BIAS_MIN, BIAS_MAX,				NULL, NULL },
			{	NULL,	PARAM_REAL,	"ring_angle_delta",			.25, 0.,	-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_INT32,	"ring_map",				.1, 0.,		0, 2,							NULL, NULL },
			{	NULL,	PARAM_INT32,	"ring_select",			.1, 0.,		0, 19.,							NULL, NULL },
			{	NULL,	PARAM_REAL,	"ring_rot_select",			.25, 0.,	-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },

		ST_PARAM_GROUP( map, MAP_PARAM_NB )
			{	NULL,	PARAM_INT32,	"map_fond",		.2, 1.,		0, 4,	NULL, NULL },
			{	NULL,	PARAM_INT32,	"map_sphere",	.2, 1.,		0, 4,	NULL, NULL },

		ST_PARAM_GROUP( joystick out, JOYSTICK_OUT_PARAM_NB )
			{	NULL,	PARAM_REAL,	"joystick_lr",		.5, .5,		-.5, .5,	NULL, NULL },
			{	NULL,	PARAM_REAL,	"joystick_du",		.5, .5,		-.5, .5,	NULL, NULL },
			{	NULL,	PARAM_REAL,	"joystick_rot",		.5, .5,		-.5, .5,	NULL, NULL },
			ST_PARAM_BOOL_OFF( b_joystick_button_1 )
			ST_PARAM_BOOL_OFF( b_joystick_button_2 )
			ST_PARAM_BOOL_OFF( b_joystick_button_3 )
			ST_PARAM_BOOL_OFF( b_joystick_button_4 )
			ST_PARAM_BOOL_OFF( b_joystick_button_5 )

		ST_PARAM_GROUP( joystick control, JOYSTICK_CONTROL_PARAM_NB )
			{	NULL,	PARAM_REAL,	"joystick_neutral",		.0, 0.1,	0, 1,				NULL, NULL },
			{	NULL,	PARAM_REAL,	"joystick_gain",		75, .5,		GAIN_MIN, GAIN_MAX, NULL, NULL },
			{	NULL,	PARAM_REAL,	"joystick_filter",		.5, .5,		0, 1,				NULL, NULL },
			{	NULL,	PARAM_REAL,	"joystick_factor",		0., 1.,		0, 1,				NULL, NULL },

			ST_PARAM_REAL_POS_ONE( joystick_ring_vanish )
			ST_PARAM_BOOL_OFF( joystick_local_use )
			ST_PARAM_BOOL_OFF( trigger_joystick_start )
			ST_PARAM_BOOL_OFF( trigger_joystick_stop )

			ST_PARAM_GROUP( joy accel, JOYSTICK_ACCEL_PARAM_NB )
				ST_PARAM_REAL_POS_ONE( joy_accel_threshold )
				ST_PARAM_REAL_POS_ONE( joy_accel_step_more )
				ST_PARAM_REAL_POS_ONE( joy_accel_step_less )
				ST_PARAM_REAL_POS_ONE( joy_accel_max )
				ST_PARAM_REAL_POS_ZERO( joy_accel_value_0 )
				ST_PARAM_REAL_POS_ZERO( joy_accel_value_1 )
				ST_PARAM_REAL_POS_ZERO( joy_accel_value_2 )
				ST_PARAM_REAL_POS_ONE( joy_accel_feedback_start )
				ST_PARAM_BOOL_OFF( feedback_on )

		ST_PARAM_GROUP( Borne, BORNE_PARAM_NB )
			{	NULL,	PARAM_INT32,	"borne_ident",	0., 1.,		0, 5,	NULL, NULL },
			{	NULL,	PARAM_REAL,		"volume",		0., 1.,		0, 1,	NULL, NULL },
			{	NULL,	PARAM_INT32,	"page_nb",		1., 0.,		0, 28,	NULL, NULL },
			ST_PARAM_BOOL_OFF( trigger_page_send )
			ST_PARAM_BOOL_OFF( trigger_page_flash )
			ST_PARAM_BOOL_OFF( trigger_halt )
			ST_PARAM_BOOL_OFF( trigger_quit )
			ST_PARAM_BOOL_OFF( trigger_switch_off )
			
		ST_PARAM_GROUP( Sound, SOUND_PARAM_NB )
			ST_PARAM_BOOL_OFF( snd_nappe_start )
			ST_PARAM_BOOL_OFF( snd_nappe_stop )
			ST_PARAM_BOOL_OFF( snd_click_in )
			ST_PARAM_BOOL_OFF( snd_click_out )
			ST_PARAM_BOOL_OFF( snd_varispeed_start )
			ST_PARAM_BOOL_OFF( snd_varispeed_stop )
			{	NULL,	PARAM_REAL,	"snd_varispeed_volume",		1., 0.,		0, 1,		NULL, NULL },
			{	NULL,	PARAM_REAL,	"snd_varispeed_speed",		1., 0.,		0, 16.,		NULL, NULL },

		ST_PARAM_GROUP( subtitle, SUBTITLE_PARAM_NB )
			ST_PARAM_BOOL_OFF( subtitle )
			{	NULL,	PARAM_INT32,	"subtitle_bind",	2., 1.,		0., 6.,		NULL, NULL },
	};
}


void	c_belga::param_init_pt()
{
INT32	h=0;
INT32	i=0;
	param_set_pt( h, b_active);

	h++;
	param_set_pt( h, b_monoscreen);
	param_set_pt( h, b_mode_wait);
	param_set_pt( h, tempo_out);

	h++;
	param_set_pt_v4( h, player_state );
	param_set_pt( h, player_cur);

	h++;
	param_set_pt( h, ring_yaw_range);
	param_set_pt( h, ring_angle);
	param_set_pt( h, ring_cur);
	param_set_pt( h, ring_rot);
	param_set_pt( h, ring_sticker_rot);
	param_set_pt( h, ring_sticker_rot_factor);
	param_set_pt( h, ring_alpha);
	param_set_pt( h, ring_alpha_bias);
	param_set_pt( h, ring_angle_delta);
	param_set_pt( h, ring_map);
	param_set_pt( h, ring_select);
	param_set_pt( h, ring_rot_select);

	h++;
	param_set_pt( h, map_fond);
	param_set_pt( h, map_sphere);

	h++;
	param_set_pt_v3( h, joy );
	param_set_pt_v5( h, b_joy_but );

	h++;
	param_set_pt( h, joy_neutral);
	param_set_pt( h, joy_gain);
	param_set_pt( h, joy_filter);
	param_set_pt( h, joy_factor);
	param_set_pt( h, joy_ring_vanish);
	param_set_pt( h, b_joy_loc);
	param_set_pt( h, trig_joy_start);
	param_set_pt( h, trig_joy_stop);

	h++;
	param_set_pt( h, joy_accel_threshold);
	param_set_pt( h, joy_accel_step_more);
	param_set_pt( h, joy_accel_step_less);
	param_set_pt( h, joy_accel_max);
	param_set_pt_v3( h, joy_accel_value );
	param_set_pt( h, joy_accel_feedback_start);
	param_set_pt( h, b_feedback_on);

	h++;
	param_set_pt( h, borne_cur);
	param_set_pt( h, vol_ui);
	param_set_pt( h, page_nb);
	param_set_pt( h, trig_page);
	param_set_pt( h, trig_page_flash);
	param_set_pt( h, trig_halt);
	param_set_pt( h, trig_quit);
	param_set_pt( h, trig_switch_off);

	h++;
	param_set_pt( h, snd_nappe_start);
	param_set_pt( h, snd_nappe_stop);
	param_set_pt( h, snd_click_in);
	param_set_pt( h, snd_click_out);
	param_set_pt( h, snd_varispeed_start);
	param_set_pt( h, snd_varispeed_stop);
	param_set_pt( h, snd_varispeed_volume);
	param_set_pt( h, snd_varispeed_speed);

	h++;
	param_set_pt( h, b_soustitre_on);
	param_set_pt( h, soustitre_bind);

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_belga)
{
	param_init_with( belga_param, BELGA_PARAM_NB_MAX);
	init();
}

c_belga::~c_belga()
{
}

JOYCAPS joy_cap;
JOYINFO joyinfo;


void	c_belga::init()
{
	vol = -1;

UINT	wNumDevs;
//UINT	wDeviceID; 
BOOL	bDev1Attached;
//BOOL	bDev2Attached; 
 
	b_joy_loc_ok = FALSE;
	if((wNumDevs = joyGetNumDevs()) != 0)
		{
		bDev1Attached = joyGetPos(JOYSTICKID1,&joyinfo) != JOYERR_UNPLUGGED; 
//		bDev2Attached = wNumDevs == 2 && joyGetPos(JOYSTICKID2,&joyinfo) != JOYERR_UNPLUGGED; 
//		if(bDev1Attached || bDev2Attached)   // decide which joystick to use 
//			wDeviceID = bDev1Attached ? JOYSTICKID1 : JOYSTICKID2; 
		if( joyGetDevCaps( JOYSTICKID1, &joy_cap, sizeof(JOYCAPS)) == JOYERR_NOERROR )
			{
			GOOD_PRINT_STRING( "Joystick driven by %s", joy_cap.szPname);
			b_joy_loc_ok = TRUE;
			}
		if ( !b_joy_loc_ok )
			BOX_ERR( "no local joystick");
		}
	joy_averager[0] = &ave1;
	joy_averager[1] = &ave2;

	the_button[ 0 ] = FALSE;
	the_button[ 1 ] = FALSE;
	the_button[ 2 ] = FALSE;
	the_button[ 3 ] = FALSE;
	
	soustitre_time_switch = n_aaatime::get() - 100;
	joy_accel_value[0] = 0;
	joy_accel_value[1] = 0;
	joy_accel_value[2] = 0;
}

void	c_belga::joy_process( INT32 i)
{
REAL	jo;
REAL	jo_abs;
	jo = joy[i]-.5;
	jo_abs = ABS(jo);
	if ( jo_abs > joy_neutral )
		{
		jo += (jo<0)?joy_neutral:-joy_neutral;
		jo *= .5/(.5-joy_neutral);

		time_last_joystick_move = n_aaatime::get();
		}
	else
		jo = 0;
	if( jo_abs > joy_accel_threshold )
		joy_accel_value[i] += joy_accel_step_more;
	else
		joy_accel_value[i] -= joy_accel_step_less;
	joy_accel_value[i] = CLAMP( joy_accel_value[i], (REAL)0., joy_accel_max);

	jo += .5;
	gain( jo, joy_gain);
	jo -= .5;
	if ( jo >= 0 )
		jo += joy_accel_value[i];
	else
		jo -= joy_accel_value[i];

	joy_averager[i]->put( jo);
	joy[i] = joy_averager[i]->get_average();
}

BOOL	b_first = TRUE;

static	c_rand_lin	rnd;

void	c_belga::update()
{
	REAL	f;
	REAL	tri;
	REAL	tmp;
	CHAR	str[1024];	//todo should be dynamic

	if( !b_active)
		return;
	if( b_first )
		{
		b_first = FALSE;
		snd_nappe_start = TRUE;
		snd_varispeed_start = TRUE;
		if( !net || net->is_master() )
			players_ready(TRUE);
		cam = layers_get_existing_from_index(LAYERS_CAMERA)->get_camera();
		}
	layers_get_existing_from_index(LAYERS_RING)->set_active( (!net || net->host_id == 100) && !b_mode_wait);
	layers_get_existing_from_index(LAYERS_SOL)->set_active( b_mode_wait);
	layers_get_existing_from_index(LAYERS_MENU)->set_active( (b_monoscreen||(net&&net->host_id == 101)) && !b_mode_wait );

	if( b_monoscreen||!net||net->is_master() )
		{
		//INT32	i;
		for( INT32 i = LAYERS_SOUND_START; i < LAYERS_SOUND_STOP; ++i )
			{
			layers_get_existing_from_index(i)->set_active( TRUE );
			}
		snd_nappe_start = FALSE;
		if( trig_joy_start )
			{
			trig_joy_start = FALSE;
			if( net)
				net->send_ascii( borne_cur, "T");  
			}
		if( trig_joy_stop )
			{
			trig_joy_stop = FALSE;
			if( net)
				net->send_ascii( borne_cur, "S");
			}
		if ( vol != vol_ui )
			{
			vol = vol_ui;
			sprintf( str, "V,%f", (FLOAT)vol );	//todoq	deal one time in your life with the multiple argument stuff
			if( net)
				net->send_ascii( 0, str);  
			}
		if( trig_page )
			{
			trig_page = FALSE;
			sprintf( str, "I,%d", page_nb );
			if( net)
				net->send_ascii( borne_cur, str);  
			}
		if( trig_page_flash )
			{
			trig_page_flash = FALSE;
			sprintf( str, "P,%s", "c:\\Belgacom\\Tests\\flash.htm" );
			if( net)
				net->send_ascii( borne_cur, str);  
			}
		if( trig_halt )
			{
			trig_halt = FALSE;
			if( net)
				net->send_ascii( borne_cur, "H");  
			}
		if( trig_quit )
			{
			trig_quit = FALSE;
			if( net)
				net->send_ascii( borne_cur, "Q");  
			}
		if( trig_switch_off )
			{
			trig_switch_off = FALSE;
			if( net)
				net->send_ascii( borne_cur, "X");  
			}

		if ( net && !net->is_slave() )
			net_receive();

		if( n_aaatime::get() <= 0. )
			soustitre_time_switch = 0.;
		if( n_aaatime::get() >= soustitre_time_switch )
			{
			if( b_soustitre_on || !b_mode_wait)
				{
				b_soustitre_on = FALSE;
				soustitre_time_switch = n_aaatime::get()+rnd.get_ufloat()*10.+5.;
				}
			else
				{
				b_soustitre_on = TRUE;
				soustitre_time_switch = n_aaatime::get()+3.;
				soustitre_bind = rnd.get_ufloat()*6.99;
				}
			}


	/*		else
				{
				joy[0] = 0.5;
				joy[1] = 0.5;
				joy[2] = 0.5;
				b_joy_but[0] = 0;
				b_joy_but[1] = 0;
				}
	*/

		//go back to 0. to 1.
		joy[0] = REAL(joy_raw[0]) / 65535;
		joy[1] = REAL(65535 -joy_raw[1]) / 65535;
		joy[2] = REAL(joy_raw[2]) / 65535;
		for( INT32 i = 0; i < 5; ++i )
			{
			b_joy_but[i] = ((joy_raw[3] & (1<<i)) != 0);
			}
		//	process it
		joy_averager[0]->set_filter_factor(joy_filter);
		joy_averager[1]->set_filter_factor(joy_filter);
		joy_process(0);
		joy_process(1);
		b_feedback_on =  (joy_accel_value[0]+joy_accel_value[1]) > joy_accel_feedback_start;


		cam->set_rot( 1, cam->get_rot(1)+joy[1]*joy_factor);
		cam->set_rot( 0, cam->get_rot(0)-joy[0]*joy_factor);

		snd_varispeed_speed = .2 + (ABS( joy[0]) + ABS( joy[1]))*32.;

		if( !b_monoscreen)
			layers_get_existing_from_index(LAYERS_CAMERA)->get_camera()->net_send();
static	INT32 count = 0;
		layers_get_existing_from_index(LAYERS_BODYRENDER)->layer_get_always(0)->get_render()->set_front_mode(
			(b_joy_but[1]==TRUE) ? ++count&1 : 0 );

		}
	else if ( net && net->is_slave() )
		{
		c_net_blk*	blk_in;
		while( 	blk_in = net->blk_take_by_type( BLK_OBJ_DATA ) )
			{
			layers_get_existing_from_index(LAYERS_CAMERA)->get_camera()->net_receive( blk_in->get_data_pt() );
			net->blk_delete( blk_in);
			}
		}

	//not with b_first because b_monoscreen
	layers_get_existing_from_index(LAYERS_LOGO)->set_active( b_monoscreen||!net||net->host_id == 101 );
	layers_get_existing_from_index(LAYERS_SOUSTITRE)->set_active( b_monoscreen||!net||net->host_id == 100 );
	layers_get_existing_from_index(LAYERS_PICTO)->set_active( b_monoscreen||(net&&net->host_id == 99));


	f = -cam->get_rot(1);
	tmp = .5 + f*7*2;
	ring_cur = I_FLOOR( FMOD( tmp, 7) );
	tri = FMOD( tmp, 1.) - .5;
	f += ring_angle_delta * tri;
	ring_angle =  FMOD( f, 1.);
	ring_rot = cam->get_rot(0);
	ring_select = ring_cur*4;
	ring_sticker_rot = ring_rot * ring_sticker_rot_factor;
	ring_rot_select = FMOD(ring_sticker_rot, .25);
	ring_alpha = (COS_INT(tri)+1.)*.5;
	bias( ring_alpha, ring_alpha_bias);
	ring_alpha -= joy_ring_vanish*ABS(joy[1]);
	mat_set_alpha( 9, ring_alpha );

/*		if ( button_old != b_joy_but[1] )
			{
			button_old = b_joy_but[1];
			if( button_old )
				{
				sprintf( str, "I,%d", ring_select );
				net->send_ascii( borne_cur, str);  
				}
			}
*/
	if( layers_get_existing_from_index(LAYERS_PICTO)->is_active() )
		{
		//INT32		i;
		c_layer*	pt;
		for( INT32 i = 0; i < 4; ++i )
			{
			pt = layers_get_existing_from_index(LAYERS_PICTO)->layer_get_always(i);
			pt->get_model()->set_size_factor( (IMOD(IMOD(pt->get_bind_2d(),32),3)==1)?1.38:1 );
			}
		}

}

INT32	ring_order[28] =
{
	1,	2,	3,	4,
	5,	6,	7,	8,
	9,	10,	11,	12,
	13,	14,	13,	14,
	19,	19,	19,	19,
	15,	16,	15,	16,
	17,	18,	17,	18
};

INT32	ring_back_order[28] =
{
	1,	1,	1,	1,
	1,	1,	1,	0,
	1,	1,	1,	1,
	0,	0,	0,	0,
	1,	1,	1,	1,
	1,	0,	1,	0,
	1,	0,	1,	0,
};

void	c_belga::net_receive()
{
	c_net_blk*	blk_in;
	while( 	blk_in = net->blk_take_by_type( BLK_ASCII ) )
		{
		INT32 sender;
		INT32	tmp = 0;
		INT32	p[4];


		snd_click_out = FALSE;

		if( b_joy_loc_ok && b_joy_loc)
			{
			if ( joyGetPos(JOYSTICKID1,&joyinfo) == JOYERR_NOERROR )
				{
				p[0] = joyinfo.wXpos;
				p[1] = joyinfo.wYpos;
				p[2] = joyinfo.wZpos;
				p[3] = joyinfo.wButtons;
				tmp = 4;
				sender = borne_loc_nb;
				}
			else
				{
				ERR_PRINT_STRING( "Can't read local joystick" );
				}
			}
		else if ( blk_in->get_len() != 0 )
			{
			CHAR*	pt = blk_in->get_data_pt();
			switch( *pt)
				{
				case 'J':
				case 'j':
					//read local joystick 
					tmp = sscanf( pt, "J,%d,%d,%d,%d", p, p+1, p+2, p+3);
					if( tmp != 4)
						ERR_PRINT_STRING( "Pb in joystick block");
					break;
				}
			}
		if( tmp  == 4)
			{
			if ( player_cur < 0  )
				{
				joy_raw[0] = 65536/2;
				joy_raw[1] = 65536/2;
				joy_raw[2] = 65536/2;
				joy_raw[3] = p[3];
				if ( (p[3] & 0x02) )
					{
					if( net)
						net->send_ascii( sender, "T");
					if( !the_button[ sender-1 ] )
						{
						the_button[ sender-1 ] = TRUE;
						player_play( sender-1);
						}
					}
				else
					{
					the_button[ sender-1 ] = FALSE;
					}
				}
			else
				{
				joy_raw[0] = p[0];
				joy_raw[1] = p[1];
				joy_raw[2] = p[2];
				joy_raw[3] = p[3];
				if( (sender-1) == player_cur )
					{
					if ( (p[3] & 0x02) )
						{
						if( !the_button[ sender-1 ] )
							{
							INT32	page_index;
							CHAR	str[1024];	//todo should be dynamic ?
							REAL	f;

							page_index = ring_cur * 4;
							f = FMOD( ring_sticker_rot, 1.);
							f *= 4;
							f = F_FLOOR(f);
							page_index += (INT32)f;
							page_index = ring_order[page_index];
							sprintf( str, "I,%d", page_index );

							the_button[ sender-1 ] = TRUE;
							players_ready(TRUE);
							if( net)
								net->send_ascii( sender, str);  
							}
						}
					else
						the_button[ sender-1 ] = FALSE;
					}
				else
					{
					the_button[ sender-1 ] = (p[3] & 0x02) != 0 ;
					}
				}
			if ( player_cur >= 0 && (n_aaatime::get()-time_last_joystick_move) > tempo_out )
				{
				players_ready(FALSE);
				}
			}
		net->blk_delete( blk_in);
		}
}

/*	REAL		joystick_du;
	REAL		joystick_lr;
	REAL		joystick_rot;
	BOOL		b_joystick_button[5];
*/

void	belga_bind_ring( INT32 in)
{
/*	if ( IMOD(c_layer::cur->get_layer_index(),4) == 2)
		tex_2d_switch( 31+ring_order[INT32(ring_cur)*4+in] );
	else
		tex_2d_switch( 62+ring_back_order[INT32(ring_cur)*4+in] );
*/
}
