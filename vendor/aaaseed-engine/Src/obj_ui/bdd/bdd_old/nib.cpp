#include "nib.h"
#include "obj_ui/bdd/bdd_point/particle.h"

#include "draw/model.h"
#include "draw/seedcam.h"
#include "image/img.h"
#include "infrastructure/layer/layer.h"
#include "infrastructure/layer/layers.h"
#include "infrastructure/layer/module.h"
#include "math/gainbias.h"
#include "math/noisturb.h"
#include "math/aaa_math.h"
#include "math/rand.h"
#include "math/v_base.h"
#include "obj_ui/bdd/bdd_geo/bdd_face.h"
#include "obj_ui/com/midi.h"
#include "obj_ui/com/net.h"
#include "time/ourtime.h"

FACTORY_CREATE_V1( c_nib, nib, Nib, nib );

REAL	c_nib_talisman::target_radius;
REAL	c_nib_talisman::target_dist_in;

REAL	g_part_dist_max;
REAL	g_part_dist_alpha;
REAL	g_part_alpha_base;

REAL	g_nif_dist_color_int;
REAL	g_nif_dist_color_ext;

static	BOOL	b_first = TRUE;
c_bdd_particle*	bdd_part = NULL;
static c_img *img_sound = NULL;
static	c_rand_lin	talisman_rnd;

char	str_verbose[2048];

REAL	stop_time;
BOOL	b_stop = FALSE;

typedef enum DRAGON_ACTIVITY_TYPE
{
	DRAGON_WANDERING = 0,
	DRAGON_CHASING,
	DRAGON_BRINGING_BACK,
	DRAGON_ACTIVITY_MAX_NB
}	DRAGON_ACTIVITY_TYPE;

char* dragon_activity_str[DRAGON_ACTIVITY_MAX_NB] =
{
	"Wandering",
	"Chasing",
	"Bringing Back",
};

//#define	LAYER_RING_SHADOW	9
//#define	LAYER_RUTELIN		24
//#define	LAYER_RING			40
//#define	LAYER_FONTAIN		49
//#define	LAYER_JEWEL			50

c_nib*		nib = NULL;

namespace n_nib
{
	static	CONST	INT32	BASE_PARAM_NB			= 9;
	static	CONST	INT32	NIF_PARAM_NB			= 2;
	static	CONST	INT32	PART_PARAM_NB			= 3;
	static	CONST	INT32	SHIP_PARAM_NB			= 26;
	static	CONST	INT32	CAMERA_PARAM_NB			= 17;
	static	CONST	INT32	PLACE_PARAM_NB			= 4 * NIB_PLACE_NB;
	static	CONST	INT32	STORM_PARAM_NB			= 5;
	static	CONST	INT32	TALSIMAN_PARAM_NB		= 2;
	static	CONST	INT32	RING_PARAM_NB			= 9;
	static	CONST	INT32	RUTELIN_PARAM_NB		= 9;
	static	CONST	INT32	SCHWERT_PARAM_NB		= 13;
	static	CONST	INT32	DRAGON_PARAM_NB			= 16;
	static	CONST	INT32	SOUND_PARAM_NB			= 7;
	static	CONST	INT32	SOUND_IN_PARAM_NB		= 0;
	static	CONST	INT32	GROUP_PARAM_NB			= 12;
	static	CONST	INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
											+	PART_PARAM_NB
											+	NIF_PARAM_NB
											+	SHIP_PARAM_NB
											+	CAMERA_PARAM_NB
											+	PLACE_PARAM_NB
											+	STORM_PARAM_NB
											+	TALSIMAN_PARAM_NB
											+	RING_PARAM_NB
											+	RUTELIN_PARAM_NB
											+	SCHWERT_PARAM_NB
											+	DRAGON_PARAM_NB
											+	SOUND_PARAM_NB
											+	SOUND_IN_PARAM_NB
											+	GROUP_PARAM_NB;

	static	CONST	INT32	NIB_NET_TO_SEND	=	  PARAM_NB_MAX
												- SOUND_PARAM_NB
												- 1 ;

	static	ST_PARAM	param[PARAM_NB_MAX] =
	{
		ST_PARAM_BOOL_OFF( active )
		ST_PARAM_BOOL_OFF( trig_reset_tra )
		ST_PARAM_BOOL_OFF( trig_reset_tra_bottom )
		ST_PARAM_BOOL_OFF( trig_reset_rot )
		ST_PARAM_BOOL_OFF( sound_active )
		ST_PARAM_BOOL_ON( udp )
		ST_PARAM_BOOL_OFF( verbose )
		ST_PARAM_BOOL_OFF( grid )
		ST_PARAM_BOOL_OFF( borgnole )
			
		ST_PARAM_GROUP_CLOSED( Nif, NIF_PARAM_NB )
			{	NULL,	PARAM_REAL,	"nif_dist_color_int",	1., 10.,	0, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,	"nif_dist_color_ext",	1., 20.,	0, PARAM_INFINI,	NULL, NULL },

		ST_PARAM_GROUP_CLOSED( Particle, PART_PARAM_NB )
			{	NULL,	PARAM_REAL,	"particle_dist_max",	1., 50.,	-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,	"particle_dist_alpha",	1., 30.,	-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,	"particle_alpha_base",	1., .0,		-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },

	//		ST_PARAM_BOOL_OFF( connect_to_sound )
		ST_PARAM_GROUP_CLOSED( Ship, SHIP_PARAM_NB )
			ST_PARAM_BOOL_OFF( ship_active )

			{	NULL,	PARAM_REAL,	"ship_y_0",			0., -1.,	-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,	"ship_y_0_speed",	0., 1.,		-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,	"ship_y_1",			0., -1.,	-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,	"ship_y_1_speed",	0., 1.,		-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,	"ship_y_2",			0., -1.,	-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,	"ship_y_2_speed",	0., 1.,		-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,	"ship_y_3",			0., -1.,	-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,	"ship_y_3_speed",	0., 1.,		-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },

			ST_PARAM_REAL_ONE( ship_dist_center )
			ST_PARAM_REAL_ZERO( ship_tra_center )
			ST_PARAM_REAL_ONE( ship_dist_outside )
			ST_PARAM_REAL_ZERO( ship_tra_outside )

			ST_PARAM_BOOL_OFF( ship_fast )
			ST_PARAM_REAL_ZERO( ship_tra_fast )

			ST_PARAM_REAL_ZERO( ship_tra_horizontal_factor )
			ST_PARAM_REAL_ZERO( ship_rot_normal )
			ST_PARAM_REAL_ZERO( ship_rot_fast )
			ST_PARAM_REAL_ZERO( ship_side_right )
			ST_PARAM_REAL_ZERO( ship_up )
			ST_PARAM_REAL_ZERO( ship_forward )
			ST_PARAM_REAL_ZERO( ship_right )
			ST_PARAM_REAL_ZERO( ship_roll_influence )
			{	NULL,	PARAM_REAL,	"ship_roll_filter",		.5, 0,			0, FLUX_FILTER_MAX,				NULL, NULL },

			{	NULL,	PARAM_REAL,	"ship_max_up",			-1, -.04,		-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,	"ship_max_bottom",		-31, -31.98,	-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },

		ST_PARAM_GROUP_CLOSED( Camera, CAMERA_PARAM_NB )
			{	NULL,	PARAM_INT32,	"central_host",		0., 4.,			0, 255,							NULL, NULL },
			{	NULL,	PARAM_REAL,	"turn_by_host",			1., 1./6.,		-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,	"turn_offset",			0., 0.0775,		-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,	"focal",				90., 50.,		-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
			ST_PARAM_POINT_XYZ( pos )
			ST_PARAM_POINT_XYZ( rot )
			ST_PARAM_POINT_XYZ( euler )
			ST_PARAM_POINT_XYZ( displace )

			ST_PARAM_REAL_ZERO( turbulence_value )

		ST_PARAM_GROUP_CLOSED( Places, PLACE_PARAM_NB )
			ST_PARAM_POINT_XYZ( place1 )
			{	NULL,	PARAM_REAL,	"place1_radius",		2., 1.,	-PARAM_INFINI, PARAM_INFINI, NULL, NULL },

		ST_PARAM_GROUP_CLOSED( Storm, STORM_PARAM_NB )
			ST_PARAM_POINT_XYZ( storm )
			{	NULL,	PARAM_REAL,	"storm_radius",		2., 1.,	-PARAM_INFINI, PARAM_INFINI, NULL, NULL },
			ST_PARAM_REAL_ZERO( storm_value )

		ST_PARAM_GROUP_CLOSED( Talisman, TALSIMAN_PARAM_NB )
		ST_PARAM_REAL_ZERO( talisman_y_bottom )
		ST_PARAM_REAL_ZERO( talisman_radius )

		ST_PARAM_GROUP_CLOSED( Ring, RING_PARAM_NB )
		ST_PARAM_REAL_ZERO( ring_radius_int )
		ST_PARAM_REAL_ZERO( ring_radius_ext )
			ST_PARAM_POINT_XYZ( ring )
			ST_PARAM_REAL_ZERO( ring_speed_max )
			ST_PARAM_REAL_ZERO( ring_accel_max )
			ST_PARAM_REAL_ZERO( ring_y_top )
			ST_PARAM_REAL_ZERO( ring_y_bottom )

		ST_PARAM_GROUP_CLOSED( Rutelin, RUTELIN_PARAM_NB )
		ST_PARAM_REAL_ZERO( rutelin_radius_int )
		ST_PARAM_REAL_ZERO( rutelin_radius_ext )
			ST_PARAM_POINT_XYZ( rutelin )
			ST_PARAM_REAL_ZERO( rutelin_speed_max )
			ST_PARAM_REAL_ZERO( rutelin_accel_max )
			ST_PARAM_REAL_ZERO( rutelin_y_top )
			ST_PARAM_REAL_ZERO( rutelin_y_bottom )

		ST_PARAM_GROUP_CLOSED( Schwert, SCHWERT_PARAM_NB )
		ST_PARAM_REAL_ZERO( schwert_radius_int )
		ST_PARAM_REAL_ZERO( schwert_radius_ext )
			ST_PARAM_POINT_XYZ( schwert )
			{	NULL,	PARAM_REAL,	"schwert_rot_threshold",	0., 17./127.,		-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
			ST_PARAM_POINT_XYZ( schwert_rot )
			ST_PARAM_REAL_ZERO( schwert_speed_max )
			ST_PARAM_REAL_ZERO( schwert_accel_max )
			ST_PARAM_REAL_ZERO( schwert_y_top )
			ST_PARAM_REAL_ZERO( schwert_y_bottom )

		ST_PARAM_GROUP_CLOSED( Dragon, DRAGON_PARAM_NB )
			ST_PARAM_BOOL_OFF( dragon_active )
			{	NULL,	PARAM_SYMBOLIC,	"dragon_activity",				1., 0.,		0., 3.,							NULL, dragon_activity_str },
			{	NULL,	PARAM_REAL,		"dragon_chasing_radius",		1., 18.,	-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,		"dragon_wandering_radius",		1., 32,		-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
			ST_PARAM_REAL_POS_ONE( dragon_accel_max )
			ST_PARAM_REAL_POS_ONE( dragon_speed_max )
			ST_PARAM_REAL_POS_ONE( dragon_accel_max_return )
			ST_PARAM_REAL_POS_ONE( dragon_speed_max_return )
			{	NULL,	PARAM_REAL,		"dragon_target_radius",			0., .25,	0., PARAM_INFINI,				NULL, NULL },
			{	NULL,	PARAM_REAL,		"dragon_target_radius_town",	0., .25,	0., PARAM_INFINI,				NULL, NULL },
			ST_PARAM_POINT_XYZ( dragon )
			ST_PARAM_POINT_XYZ( dragon_speed )

		ST_PARAM_GROUP_CLOSED( Sound, SOUND_PARAM_NB )
			ST_PARAM_REAL_ZERO( rgba_size_u )
			ST_PARAM_REAL_ZERO( rgba_size_v )
			ST_PARAM_REAL_ZERO( microphone_offset )
			ST_PARAM_REAL_ZERO( dragon_dist_max )
			ST_PARAM_REAL_ZERO( forward_factor )
			ST_PARAM_REAL_ZERO( slide_factor )
			ST_PARAM_REAL_ZERO( roll_factor )
	};
}

void	c_nib::param_init_pt()
{
	INT32	h=0;

	param_set_pt( h, get_pt_active() );
	param_set_pt( h, b_trig_reset_tra );
	param_set_pt( h, b_trig_reset_tra_bottom );
	param_set_pt( h, b_trig_reset_rot );
	param_set_pt( h, b_sound );
	param_set_pt( h, b_udp );
	param_set_pt( h, b_verbose );

	param_set_pt( h, b_grid );
	param_set_pt( h, b_borgnole );

	h++;
	param_set_pt( h, g_nif_dist_color_int );
	param_set_pt( h, g_nif_dist_color_ext );

	h++;
	param_set_pt( h, g_part_dist_max );
	param_set_pt( h, g_part_dist_alpha );
	param_set_pt( h, g_part_alpha_base );

	h++;
	param_set_pt( h, b_ship_active );

	for( INT32 i = 0; i < 4; ++i )
	{
		param_set_pt( h, ship_y[i] );
		param_set_pt( h, ship_speed[i] );
	}

	param_set_pt( h, ship_center_dist );
	param_set_pt( h, ship_center_speed );
	param_set_pt( h, ship_outside_dist );
	param_set_pt( h, ship_outside_speed );

	param_set_pt( h, b_ship_fast );
	param_set_pt( h, ship_tra_factor_fast_ui );

	param_set_pt( h, ship_tra_hori_factor );
	param_set_pt( h, ship_rot_factor_normal_ui );
	param_set_pt( h, ship_rot_factor_fast_ui );

	param_set_pt_v4( h, ship_in );
	param_set_pt( h, ship_roll_influence );
	param_set_pt( h, ship_roll_filter );
	param_set_pt( h, ship_max_up );
	param_set_pt( h, ship_max_bottom );

	h++;
	param_set_pt( h, central_host );
	param_set_pt( h, turn_by_host );
	param_set_pt( h, turn_offset );
	param_set_pt( h, focal );

	param_set_pt_v3( h, cam_pos );
	param_set_pt_v3( h, cam_rot );
	param_set_pt_v3( h, cam_euler );
	param_set_pt_v3( h, cam_disp );

	param_set_pt( h, cam_turb_value );

	h++;
	for( INT32 i = 0; i < NIB_PLACE_NB; ++i )
	{
		param_set_pt_v3( h, place[i].pos );
		param_set_pt( h, place[i].radius );
	}

	h++;
	param_set_pt_v3( h, storm.pos );
	param_set_pt( h, storm.radius );
	param_set_pt( h, storm_value );

	h++;
	param_set_pt( h, c_nib_talisman::target_radius );
	param_set_pt( h, c_nib_talisman::target_dist_in );

	h++;
	param_set_pt( h, ring.radius_int );
	param_set_pt( h, ring.radius_ext );
	param_set_pt_v3( h, ring.pos );
	param_set_pt( h, ring.speed_max );
	param_set_pt( h, ring.accel_max );
	param_set_pt( h, ring.top );
	param_set_pt( h, ring.bottom );

	h++;
	param_set_pt( h, rutelin.radius_int );
	param_set_pt( h, rutelin.radius_ext );
	param_set_pt_v3( h, rutelin.pos );
	param_set_pt( h, rutelin.speed_max );
	param_set_pt( h, rutelin.accel_max );
	param_set_pt( h, rutelin.top );
	param_set_pt( h, rutelin.bottom );

	h++;
	param_set_pt( h, schwert.radius_int );
	param_set_pt( h, schwert.radius_ext );
	param_set_pt_v3( h, schwert.pos );
	param_set_pt( h, schwert_rot_threshold );	
	param_set_pt_v3( h, schwert_rot );
	param_set_pt( h, schwert.speed_max );
	param_set_pt( h, schwert.accel_max );
	param_set_pt( h, schwert.top );
	param_set_pt( h, schwert.bottom );

	h++;
	param_set_pt( h, b_dragon_active );
	param_set_pt( h, dragon_activity );
	param_set_pt( h, dragon_chasing_radius );
	param_set_pt( h, dragon_wandering_radius );
	param_set_pt( h, dragon_accel_max );
	param_set_pt( h, dragon_speed_max );
	param_set_pt( h, dragon_accel_max_return );
	param_set_pt( h, dragon_speed_max_return );
	param_set_pt( h, dragon_target_radius );
	param_set_pt( h, dragon_target_radius_town );
	param_set_pt_v3( h, dragon_pos );
	param_set_pt_v3( h, dragon_speed );

	h++;
	param_set_pt( h, rgba_size_u );
	param_set_pt( h, rgba_size_v );
	param_set_pt( h, mic_offset );
	param_set_pt( h, dragon_dist_max );	
	param_set_pt( h, sound_forward_factor );	
	param_set_pt( h, sound_slide_factor );	
	param_set_pt( h, sound_roll_factor );	

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_nib)
{
	param_init_with( n_nib::param, n_nib::PARAM_NB_MAX ); // nib_param, NIB_PARAM_NB_MAX);

	init();
}

c_nib::~c_nib()
{
}

void	c_nib::init()
{
	clear_v3r( cam_pos);
	clear_v3r( cam_rot);
	clear_v3r( cam_euler);
	cam_angle = 0;
	dragon_activity = DRAGON_WANDERING;
	for( INT32 i=0; i<PART_CLOSE_NB_MAX; ++i )
		part_to_send[i] = NULL;

}

void	c_nib::udp_begin()
{
	udp_pt = &udp_buf[0];
	udp_send( 0);
}

void	c_nib::udp_end()
{
	net->sendto( 4, 0, BLK_MAX_BY_OPCODE, (UINT8*)udp_buf, udp_pt-udp_buf );
}


void	c_nib::udp_send( INT32 value)
{
	*(INT32*)udp_pt = htonl(value);
	udp_pt += 4;
}

extern	REAL	feedback_angle;
extern	REAL	feedback_angle_bis;
extern	REAL	feedback_scale_factor;

void	c_nib::send_value( INT32 value )
{
	if( b_udp )
	{
		udp_send( value );
	}
}

//#ifdef	DEBUG
//INT32	tmp_last;
//#endif

static	BOOL	b_first_bug = TRUE;
void	c_nib::send_value_on_three( REAL val)
{
	val = CLAMP( val, (REAL)0., (REAL)1.);
	INT32	tmp = val * (128*128*128-1);
/*
#if	AAA_DEBUG()
INT32	tmp2;
	tmp2 = (tmp>>14) & 0x7f;
	if( ABS( tmp2 - tmp_last) > 30 )
		{
		if( b_first_bug )
			b_first_bug = FALSE;
		else if( MessageBox(NULL, "Bug hauteur : Debug ?", "Confirmation", MB_OKCANCEL|MB_ICONWARNING|MB_TOPMOST) == IDOK)
			debug_break();
		}
	tmp_last = tmp2;
#endif
*/
	send_value( tmp );
}

void	c_nib::send_value_on_two( REAL val)
{
	val = CLAMP( val, (REAL)0., (REAL)1.);
	INT32 tmp = val * (128*128-1);		
	send_value( (tmp>>7) & 0x7f );
}


void	c_nib::send_rgba_low( REAL* pos, INT32 which)
{
	INT32	color;
	//INT32	i;
	INT32	start;
	INT32	stop;

	REAL	u = pos[0] / rgba_size_u + .5;
	REAL	v = pos[2] / rgba_size_v + .5;
	if( 0.<u && u<1. && 0.<v && v<1. )
		color = *(INT32 *) img_sound->get_color4ubv_from_uv( v, u);
	else
	{
//		u = pos[0] / rgba_big_size_u + .5;
//		v = pos[2] / rgba_big_size_v + .5;		
//		if( 0.<u && u<1. && 0.<v && v<1. )
//			color = *(INT32 *) img_bind[rgba_big_bind]->get_color4ubv_from_uv( v, u);
//		else
			color = 0xff00ff00;
	}
	if( which )
	{
		start = 2;
		stop = 2;
	}
	else
	{
		start = 0;
		stop = 3;
	}
	if( b_verbose )
		sprintf( str_verbose, "RGB " );
	for( INT32 i=start; i<=stop; ++i )
	{
		REAL	tmp;
		tmp = ((color>>(i*8)) & 0xff) / 255.;
		send_value_on_two( tmp);
	}
}

REAL	c_nib::send_prox_azi( REAL* center, REAL* pos, REAL dist_max)
{
	V3D		vec;
	REAL	angle;
	REAL	prox;

	angle = .0;
	if( dist_max != 0 )
	{
		sub_v3r( vec, pos, center );
		prox = norm_v3r( vec) / dist_max;
		prox = 1. - CLAMP( prox, REAL(0.), REAL(1.) );
		if( prox != 0.)
		{
			angle = ATAN2_INT( vec[0], vec[2]);
			angle += cam_angle - .5;
			angle = FMOD( angle, 1. );
		}
	}
	else
		prox = 0.;

	if( b_verbose )
		sprintf( str_verbose + strlen( str_verbose ), "Proximity %f angle %f\n", prox, angle );

	send_value_on_two( prox );
	send_value_on_two( angle );
	return prox;
}

/*
very old send ring
REAL	rb, rs;
REAL	vec[3];
REAL	tmp;

	rb = ABS( model_ring->get_size_index(0) ) * .5;
	rs = ABS( model_ring->get_size_index(2) ) * .5;
	rb -= rs;

//	send_value( 0, nib->ring_pos[1] );		//A	hauteur
//	send_value( 1, rb );					//B	rayon anneau
//	send_value( 2, rs );					//C rayon boudin

	sub_v3r( vec, ring_pos, pos);
	tmp = norm_v3r( vec);	// distance au centre
	send_value_on_two( control, tmp/ring_dist_center_max );

	//reduce the problem in Two dimension
	tmp = SQRT( vec[0]*vec[0] + vec[2]*vec[2]);
	tmp -= rb;
	tmp = SQRT( tmp*tmp + vec[1]*vec[1]);
	tmp -= rs;
	send_value_on_two( control+2, tmp/ring_dist_skin_max );
*/

void	c_nib::update_sound()
{
//INT32 i;

//find the position for all
	if( b_udp )
	{
		REAL	x = mic_offset;
		REAL	z = mic_offset;
		REAL	tmp;
		REAL	schwert_factor;

		for( INT32 i = 0; i < 5; ++i )
			cpy_v3r( mic[i], cam_pos );

		mic[1][0] += -x * cos_y + z * sin_y;
		mic[1][2] += -x * sin_y - z * cos_y;

		mic[2][0] +=  x * cos_y + z * sin_y;
		mic[2][2] +=  x * sin_y - z * cos_y;

		mic[3][0] +=  x * cos_y - z * sin_y;
		mic[3][2] +=  x * sin_y + z * cos_y;

		mic[4][0] += -x * cos_y - z * sin_y;
		mic[4][2] += -x * sin_y + z * cos_y;

		//	rgba
//		bind_img::get_image_data( &img_bind[rgba_bind], rgba_bind );
//		bind_img::get_image_data( &img_bind[rgba_big_bind], rgba_big_bind );

		udp_begin();
		for( INT32 i = 0; i < 5; ++i )
			send_rgba_low( mic[i], i );

		//	ring
		send_prox_azi( mic[0], ring.pos, ring.radius_ext );

		//	rutelin
		send_prox_azi( mic[0], rutelin.pos, rutelin.radius_ext);		

		//	schwert
		send_prox_azi( mic[0], schwert.pos, schwert.radius_ext);

		//	dragon
		send_prox_azi( mic[0], dragon_pos, dragon_dist_max);

		//	position
		if( b_verbose )
			sprintf( str_verbose, "Position " );
		send_value_on_three( 1.-ABS(cam_pos[1])/32.);

		//	forward
		tmp = .5 - ship_in[2]*sound_forward_factor;
		tmp = CLAMP( tmp, REAL(0.), REAL(1.) );
		send_value_on_two( tmp);

		// do the schwert move from thierry
		//	get a distance factor first
		schwert_factor = dist_v3r( cam_pos, schwert.pos) / schwert.radius_ext;
		schwert_factor = 1. - CLAMP( schwert_factor, REAL(0.), REAL(1.));
		schwert_factor = schwert_factor * .8 + .2;
		// do the schwert forward influence
		if( b_schwert_rot_state[0] )
		{
			if( (tmp -.5) < schwert_rot_threshold )
				b_schwert_rot_state[0] = FALSE;
		}
		else
		{
			if( (tmp -.5) > schwert_rot_threshold )
			{
				schwert_rot[0] = talisman_rnd.get_ufloat() * schwert_factor;
				b_schwert_rot_state[0] = TRUE;
			}
		}

		//	slide_right
		tmp = .5 + ship_in[0]*sound_slide_factor;
		tmp = CLAMP( tmp, REAL(0.), REAL(1.) );
		send_value_on_two( tmp);
		// do the schwert side influence
		if( b_schwert_rot_state[1] )
		{
			if( ABS(tmp -.5) < schwert_rot_threshold )
				b_schwert_rot_state[1] = FALSE;
		}
		else
		{
			if( ABS(tmp -.5) > schwert_rot_threshold )
			{
				schwert_rot[1] = talisman_rnd.get_ufloat() * schwert_factor;
				b_schwert_rot_state[1] = TRUE;
			}
		}

		//	roll
		tmp = .5 + ship_in[3]*sound_roll_factor;
		tmp = CLAMP( tmp, REAL(0.), REAL(1.) );
		send_value_on_two( tmp);

		// do the schwert roll influence
		if( b_schwert_rot_state[2] )
		{
			if( ABS(tmp -.5) < schwert_rot_threshold )
				b_schwert_rot_state[2] = FALSE;
		}
		else
		{
			if( ABS(tmp -.5) > schwert_rot_threshold )
			{
				schwert_rot[2] = talisman_rnd.get_ufloat() * schwert_factor;
				b_schwert_rot_state[2] = TRUE;
			}
		}

		//	turbulence
		if( b_verbose )
			sprintf( str_verbose, "Turbulence " );
		send_value_on_two( cam_turb_value);
/*		for( i=1; i<5; ++i )
			send_value_on_two( 1+i*2, get_turb_val( mic[i]) );
*/
		//	particule
		if( bdd_part )
		{
			INT32 nb;
			//INT32 i;
			PART_SORT_CONT<c_particle*>::iterator	it_mark;
			PART_SORT_CONT<c_particle*>::iterator	it_src;
			PART_SORT_CONT<c_particle*>&			psrc = bdd_part->get_part_close();
			c_particle* pp;

			if( b_verbose )
				sprintf( str_verbose, "Particle " );
			nb = psrc.size();
//			send_value( PART_CLOSE_NB_MAX*8+1, nb );
			nb = MIN( nb, PART_CLOSE_NB_MAX);
			//	search the same
			for( INT32 i=0; i<PART_CLOSE_NB_MAX; ++i )
			{
				BOOL	b_found;
				pp = part_to_send[i];
				if( pp )	//	there is one
				{		//	we search for the same one
					b_found = FALSE;	
					for( it_src = psrc.begin(); it_src != psrc.end(); ++it_src )
					{	
						if( *it_src == pp )	//	same
						{	//remove from list in
							psrc.erase(it_src);
							b_found = TRUE;
							--nb;
							break;
						}
					}
					if( !b_found )
						part_to_send[i] = NULL;	//not found
				}
			}
			//	now all the nonull in the array are the one we were following
			//	we still need to file if possible the null spots
			INT32 n = 0;
			while( nb > 0)
			{
				pp = part_to_send[n];
				if( !pp )	//	there is none
				{		//	get closest
					REAL dist = REAL_BIG_VALUE;
					for( it_src = psrc.begin(); it_src != psrc.end(); ++it_src )
					{
						if( dist > (*it_src)->get_sort() )
						{
							dist = (*it_src)->get_sort();
							it_mark = it_src;
						}
					}
					part_to_send[n] = *it_mark;
					psrc.erase(it_mark);
					--nb;
				}
				++n;
			}			

			for( INT32 i = 0; i < PART_CLOSE_NB_MAX; ++i )
			{
				pp = part_to_send[i];
				if( pp )
					send_prox_azi( cam_pos, pp->get_pos_to_draw(), bdd_part->get_part_close_dist() );
				else
					send_prox_azi( cam_pos, NULL, 0.);
			}
		}

		//	places
		if( b_verbose )
			sprintf( str_verbose, "Places " );
//		for( i=0; i<NIB_PLACE_NB; ++i )
//			send_prox_azi( 1+i*4, cam_pos, place[i].pos, place[i].radius);
		send_prox_azi( cam_pos, place[0].pos, place[0].radius);
		storm_value = send_prox_azi( cam_pos, storm.pos, storm.radius);

		storm_value = storm_value * (1 - c_midi::static_get_control( 1, 1 ));

//		send_value_on_two( 4, cam_rot[2]*10.+.5);
		udp_end();
	}
}


void	c_nib::update_dragon()
{
	REAL	 dist;
	V3D		pos_futur;
	V3D		accel;
	REAL	dt = delta_t.get_dt();
	REAL	accel_max;
	REAL	speed_max;

	accel_max = dragon_accel_max;
	speed_max = dragon_speed_max;
	dist = SQRT( cam_pos[0]*cam_pos[0] + cam_pos[2]*cam_pos[2] );

	if	(	ship_in[0] == 0.
		&&	ship_in[1] == 0.
		&&	ship_in[2] == 0.
		&&	ship_in[3] == 0.
		)
	{
		if( !b_stop)
		{
			b_stop = TRUE;
			stop_time = n_aaatime::get();
		}
	}
	else
		b_stop = FALSE;
	if( dragon_activity != DRAGON_BRINGING_BACK )
	{
		if( b_stop && n_aaatime::get() > (stop_time + 60.) && (dist > dragon_target_radius_town) )
			dragon_activity = DRAGON_CHASING;
		else if( dist < dragon_chasing_radius )
			dragon_activity = DRAGON_WANDERING;
		else
			dragon_activity = DRAGON_CHASING;
	}
	switch( dragon_activity )
	{
	case DRAGON_WANDERING:
		dragon_target[0] = cam_pos[0];
		dragon_target[1] = 0.;
		dragon_target[2] = cam_pos[2];
		normalize_scale_v3r( dragon_target, dragon_wandering_radius );
		dragon_target[1] = cam_pos[1];

		dist = dist_v3r( dragon_pos, dragon_target ) + 2.;

		dragon_target[0] += dist * .25 * COS_INT( time *.36333);
		dragon_target[2] += dist * .25 * SIN_INT( time *.0856);
		dragon_target[1] += dist * .25 * COS_INT( time*.01 )*COS_INT( time*.24 );
		break;
	case DRAGON_CHASING:
		dist = dist_v3r( dragon_pos, cam_pos );
		if( dist > dragon_target_radius )
		{
			cpy_v3r( dragon_target, cam_pos);

			dist = MAX(dist-4.,0.);

			dragon_target[0] += dist * .25 * COS_INT( dist *.01333);
			dragon_target[2] += dist * .25 * SIN_INT( dist *.0856);
			dragon_target[1] += dist * .05 * COS_INT( dist *.02 );
			break;
		}
		else
			dragon_activity = DRAGON_BRINGING_BACK;
	case DRAGON_BRINGING_BACK:	//bringing back
		dragon_target[0] = 0.;
		dragon_target[1] = -.038;
		dragon_target[2] = 0.;
		dist = dist_v3r( dragon_pos, dragon_target );
		if( dist > dragon_target_radius_town )
		{
			accel_max = dragon_accel_max_return;
			speed_max = dragon_speed_max_return;
			dist = SQRT( dist);
			accel_max *= dist;
			speed_max *= dist;
			dist = MAX(dist-1.,0.);

			dragon_target[0] += dist * .25 * COS_INT( time * .36333);
			dragon_target[2] += dist * .25 * SIN_INT( time * .36333);
			dragon_target[1] += MIN( (REAL)dist, (REAL)2.) * .25 * (SIN_INT( time * .2)-1.);
		}
		else
			dragon_activity = DRAGON_WANDERING;
		break;
	}
		
	add_scale_v3r( pos_futur, dragon_pos, dragon_speed, speed_max/accel_max );

		//	choose accel
	sub_v3r( accel, dragon_target, pos_futur);
	accel[1] *= 5;
	limit_v3r( accel, accel_max);

	//	calculate speed
	add_scale_v3r( dragon_speed, accel, dt);
	limit_v3r( dragon_speed, speed_max);

	//	calculate pos
	add_scale_v3r( dragon_pos, dragon_speed, dt); 
/*
	sub_v3r( vec, dragon_target, dragon_pos );
	normalize_v3r( vec);
	add_scale_v3r( dragon_speed, vec, dragon_accel_max*delta_t.get_dt() );
	tmp = norm_v3r( dragon_speed );
	if( tmp > dragon_speed_max )
		scale_v3r( dragon_speed, dragon_speed_max/tmp );
	add_scale_v3r( dragon_pos, dragon_speed, delta_t.get_dt() );
*/
	if( dragon_activity == DRAGON_BRINGING_BACK )
		cpy_v3r( cam_pos, dragon_pos );
//		add_scale_v3r( cam_pos, dragon_speed, delta_t.get_dt() );
}

REAL	c_nib::get_turb_val( REAL* pos)
{
	V3D vec;

	vec[0] = pos[2]/128.*16.;
	vec[1] = (delta_t.get_t() - (pos[1]/32.+1.)*180.)*.02;
	vec[2] = pos[0]/128.*16.;
	return fractalsum( vec, 1., 1);
}

void	c_nib::net_send()
{
//#ifdef	NET
	if (net)
	{
		CHAR*	end;
		CHAR	buf[ n_nib::NIB_NET_TO_SEND * sizeof(DOUBLE)];
		param_init_pt();	//call
		end = param_write_to_mem( buf, n_nib::NIB_NET_TO_SEND);
		net->send( 2, BLK_OBJ_DATA, (UINT8*)buf, end-buf);
	}
//#endif
}

void	c_nib::net_receive( CHAR* pt)
{
//#ifdef	NET
	if( pt)
	{
		param_init_pt();	//call
		param_read_from_mem( pt, n_nib::NIB_NET_TO_SEND );
	}
	else
		ERR_PRINT_STRING( "c_nib::net_receive NULL pointer");
//#endif
}

BOOL	c_nib::net_receive()
{	
	BOOL	b_in = FALSE;
//#ifdef	NET
	while( c_net_blk* blk_in = net->blk_take_by_type_channel( BLK_OBJ_DATA, 2 ) )
	{
		net_receive( (CHAR*)blk_in->get_data_pt() );
		b_in = TRUE;
		net->blk_free( blk_in );
	}
//#endif
	return b_in;
}

REAL		pitch_last;
REAL		yaw_last;
c_seedcam*	nib_cam;

void	c_nib::update()
{
	if( !is_active() )
		return;
	REAL	dt;
	REAL	tmp;

	if( b_first )
	{
		c_layers* ls;
		ls = module_get_layers_from_name_short( "ParticleLayer1");
		if( ls )
			bdd_part = ls->layer_get(1)->bdd_get_always<c_bdd_particle>();
		ls = module_get_layers_from_name_short( "Camera");
		nib_cam = ls->get_camera(0);

		ring.make_target();
		rutelin.make_target();
		schwert.make_target();
		if( net && net->is_master() )
			img_sound = img_read( img_sound, "Maps\\Nov2000\\SoundMap_4096.tga" );
		b_schwert_rot_state[0] = FALSE;
		b_schwert_rot_state[1] = FALSE;
		b_schwert_rot_state[2] = FALSE;
		b_first = FALSE;
	}
//todo make fn for this
	if ( delta_t.update() )
	{
	}
	dt = delta_t.get_dt();
	time = n_aaatime::get_real_time();

	if( net && net->is_slave() )
	{
		if( net_receive() )
		{
			nib_cam->set_center( cam_pos );
			nib_cam->set_rot( cam_rot );
			cam_euler[1] += (net->get_host_id()-central_host) * turn_by_host + turn_offset;
			nib_cam->set_euler( cam_euler );
			nib_cam->set_focal( focal );
		}
	}
	else
	{
		if( b_ship_fast )
		{
			ship_tra_factor = ship_tra_factor_fast_ui;
			ship_rot_factor = ship_rot_factor_fast_ui;
		}
		else
		{
			REAL	dist;

			dist = dist_v3r( cam_pos, place[0].pos );
			if( ship_outside_dist != ship_center_dist )
				dist = ( dist - ship_center_dist) / ( ship_outside_dist - ship_center_dist);
			else
				dist = 1.;
			dist = CLAMP( dist, REAL(0.), REAL(1.) );
			ship_tra_factor = interpolate(	ship_center_speed,
											ship_outside_speed,
											dist );
			
			dist = CLAMP( cam_pos[1], ship_y[3], ship_y[0]);
			for( INT32 i=0; i<3; ++i )
			{
				if( ship_y[i] >= dist && dist >= ship_y[i+1] )
				{
					if( ship_y[i] != ship_y[i+1] )
						dist = ( dist - ship_y[i]) / ( ship_y[i+1] - ship_y[i]);
					ship_tra_factor *= interpolate( ship_speed[i],
													ship_speed[i+1],
													dist );
				}
			}
			ship_rot_factor = ship_rot_factor_normal_ui;
		}
		if( nib_cam )
		{
			if( b_ship_active)
				{
				REAL	x,y,z;
				REAL	pitch;
				REAL	yaw;
				if ( b_trig_reset_tra )
				{
					clear_v3r( cam_pos);
					cam_pos[1] = -1;
					b_trig_reset_tra = FALSE;
				}
				if ( b_trig_reset_tra_bottom )
				{
					cam_pos[1] = -32;
					b_trig_reset_tra_bottom = FALSE;
				}
				if ( b_trig_reset_rot )
				{
					clear_v3r( cam_rot);
					cam_angle = 0.;
					b_trig_reset_rot = FALSE;
				}

				cam_angle += ship_in[3]*dt*ship_rot_factor;
				
				flux_ship_roll.set_filter_factor( ship_roll_filter);
				flux_ship_pitch.set_filter_factor( ship_roll_filter);
				flux_ship_yaw.set_filter_factor( ship_roll_filter);
				flux_ship_roll.put( ship_in[3]);
//todoqq 				cam_rot[2] = flux_ship_roll.get();

				if( !b_dragon_active || dragon_activity != DRAGON_BRINGING_BACK )
				{
					y = cam_pos[1] + ship_in[1]*dt*ship_tra_factor;
					y = CLAMP( y, ship_max_bottom, ship_max_up);
					y -= cam_pos[1];
					cam_pos[1] += y;
					tmp = dt*ship_tra_factor*ship_tra_hori_factor;
					z = ship_in[2]*tmp;
					x = ship_in[0]*tmp;

					GET_SIN_COS_INT( sin_y, cos_y, cam_angle );

					cam_pos[0] += x * cos_y - z * sin_y;
					cam_pos[2] += x * sin_y + z * cos_y;


				}
				if( dt!= 0. )
				{
/*
					x /= dt;
					y /= dt;
					z /= dt;
*/
					x = ship_in[0];
					y = ship_in[1];
					z = ship_in[2];
				}
//				if( SQRT(x*x+z*z) > .01 )
				{

					pitch = ATAN2_INT( y, SQRT( x * x + z * z ) );
					yaw =	ATAN2_INT( z, x ) + .25;
				}
/*				else
				{
					pitch = 0;
					yaw = 0.;
				}
*/				
				{	//algo angle cont
					REAL f;
					f = pitch - pitch_last;
					if( f > .5)	//	the only solution is quaternion
						pitch -= F_FLOOR(f+.5);
					else if( f < -.5 )
						pitch -= CEIL(f-.5);
					pitch_last = pitch;
				}
				{	//algo angle cont
					REAL f;
					f = yaw - yaw_last;
					if( f > .5)	//	the only solution is quaternion
						yaw -= F_FLOOR(f+.5);
					else if( f < -.5 )
						yaw -= CEIL(f-.5);
					yaw_last = yaw;
				}
				pitch *= -SQRT(x*x+z*z) * ship_roll_influence;

				flux_ship_pitch.put( pitch);
				pitch = flux_ship_pitch.get();
				flux_ship_yaw.put( yaw);
				yaw = flux_ship_yaw.get();

				nib_cam->set_center( cam_pos );

				clear_v3r( cam_rot);
				cam_rot[0] = cam_angle+yaw;
				cam_rot[1] = pitch;
				nib_cam->set_rot( cam_rot);

				clear_v3r( cam_euler);
				cam_euler[1] = -yaw + (net->get_host_id()-central_host) * turn_by_host + turn_offset;
				nib_cam->set_euler( cam_euler );
				cam_euler[1] = -yaw;
				nib_cam->set_focal( focal );
			}
			else
			{
				nib_cam->get_coor( cam_pos);
			}
			
			cam_turb_value = get_turb_val( cam_pos);

			sub_v3r( cam_disp, cam_pos_last, cam_pos );
			//todo	understand
		//	cam_disp_[1] = -cam_disp_[1]; 
			cpy_v3r( cam_pos_last, cam_pos );

			ring.move( delta_t.get_dt(), &rutelin, &schwert);
			rutelin.move( delta_t.get_dt(), &ring, &schwert);
			schwert.move( delta_t.get_dt(), &rutelin, &ring);

		}
		if( b_dragon_active)
			update_dragon();
		if( b_sound )
			update_sound();
		net_send();
	}
}
	

c_nib_talisman::c_nib_talisman()
{
	clear_v3r(speed );
}

void	c_nib_talisman::make_target()
{
	REAL	a;
	REAL	d;

	target[1] = interpolate( top, bottom, talisman_rnd.get_ufloat() );
	a = talisman_rnd.get_ufloat();
	d = interpolate( 0, target_radius-radius_int, talisman_rnd.get_ufloat() );
	target[0] = d * COS_INT(a);
	target[2] = d * SIN_INT(a);
	if( nib->b_verbose)
		VERBOSE_PRINTF( "new target at %f %f %f", (FLOAT)target[0], (FLOAT)target[1], (FLOAT)target[2]);  
}

void	c_nib_talisman::move( REAL dt, c_nib_talisman* ta, c_nib_talisman* tb)
{
	V3D		pos_futur;
	V3D		accel;
	V3D		mod;
	REAL	norm;
	REAL	tmp;
	REAL	keep;
	//	do we reach the target
	norm = dist_v3r( target, pos);
	if( norm < target_dist_in || (ABS(target[1]-pos[1])<.001) )	//	yes
		make_target();			//	get a new one


	add_scale_v3r( pos_futur, pos, speed, speed_max/accel_max);
	sub_v3r( accel, target, pos_futur);
	limit_v3r( accel, accel_max );

	keep = accel[1];
	accel[1] = 0;

	//	avoid	ta
	if( ta )
	{
		mod[0] = pos[0] - ta->pos[0];
		mod[1] = 0.;
		mod[2] = pos[2] - ta->pos[2];
		norm = norm_v3r( mod );
		tmp = radius_int+ta->radius_int;
		if( norm <= tmp+accel_max )
		{
			if( norm > accel_max )
				scale_v3r( mod, accel_max/norm );
			add_v3r( accel, mod );
		}
	}
		//	avoid	tb
	if( tb )
	{
		mod[0] = pos[0] - tb->pos[0];
		mod[1] = 0.;
		mod[2] = pos[2] - tb->pos[2];
		norm = norm_v3r( mod );
		tmp = radius_int+tb->radius_int;
		if( norm <= tmp+accel_max )
		{
			if( norm > accel_max )
				scale_v3r( mod, accel_max/norm );
			add_v3r( accel, mod );
		}
	}

	//	compute accel
	norm = norm_v3r( accel);
	tmp = dt;
	if( norm > accel_max )
		tmp *= accel_max/norm;
	scale_v3r( accel, tmp);
	accel[1] = keep*dt;
//	add_scale_v3r( speed, vec, dt);
	//	compute_speed
	add_v3r( speed, accel);
	limit_v3r( speed, speed_max);

	//	compute pos
	add_scale_v3r( pos, speed, dt);
}
