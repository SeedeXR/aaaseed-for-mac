#include "boulogne.h"
#include "infrastructure/layer/layers.h"
#include "math/rand.h"
#include "obj_ui/tracker/neat.h"
#include "obj_ui/com/midi.h"
#include "time/ourtime.h"
#include "math/aaa_rand.h"
//#include "infrastructure/param/trax.h"

FACTORY_CREATE_V1( c_boul, boulogne, Boulogne, boul );

static	c_rand_lin		rand_boul;

#define	CLICK_CHANNEL	6

c_boul*	boul;

REAL	neat_min[8];
REAL	neat_value[8];
REAL	neat_max[8];
INT32	neat_for_atau[8];
BOOL	b_send_control = FALSE;

namespace n_boulogne
{
	static	CONST	INT32	BASE_PARAM_NB	= 7;
	static	CONST	INT32	PARAM_NEAT_NB	= 8;
	static	CONST	INT32	PARAM_GROUP_NB	= 1;

	static	CONST	INT32	PARAM_NB_MAX	=	BASE_PARAM_NB + PARAM_NEAT_NB * 3
											+	PARAM_GROUP_NB;

	static	ST_PARAM	param[PARAM_NB_MAX] =
	{
		ST_PARAM_BOOL_OFF( active )
		{	(void *) NULL,	PARAM_REAL,	"layers_time_out",		8., 16.,	0., PARAM_INFINI,				NULL, NULL },
		{	(void *) NULL,	PARAM_REAL,	"spiral_layers_time",	8., 4.,		0., PARAM_INFINI,				NULL, NULL },
		{	(void *) NULL,	PARAM_REAL,	"longvue_angle",		.5, 0.,		-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
		{	(void *) NULL,	PARAM_REAL,	"longvue_x",			.5, 0.,		-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
		{	(void *) NULL,	PARAM_REAL,	"longvue_y",			.5, 0.,		-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
		ST_PARAM_GROUP( Neat, PARAM_NEAT_NB * 3 )
			{	NULL,	PARAM_REAL,	"neat_1_min",	.25, 0.,	0, 1,							NULL, NULL },
			{	NULL,	PARAM_REAL,	"neat_1_value",	.5, 0.,		-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,	"neat_1_max",	.75, 1.,	0, 1,							NULL, NULL },
			{	NULL,	PARAM_REAL,	"neat_2_min",	.25, 0.,	0, 1,							NULL, NULL },
			{	NULL,	PARAM_REAL,	"neat_2_value",	.5, 0.,		-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,	"neat_2_max",	.75, 1.,	0, 1,							NULL, NULL },
			{	NULL,	PARAM_REAL,	"neat_3_min",	.25, 0.,	0, 1,							NULL, NULL },
			{	NULL,	PARAM_REAL,	"neat_3_value",	.5, 0.,		-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,	"neat_3_max",	.75, 1.,	0, 1,							NULL, NULL },
			{	NULL,	PARAM_REAL,	"neat_4_min",	.25, 0.,	0, 1,							NULL, NULL },
			{	NULL,	PARAM_REAL,	"neat_4_value",	.5, 0.,		-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,	"neat_4_max",	.75, 1.,	0, 1,							NULL, NULL },
			{	NULL,	PARAM_REAL,	"neat_5_min",	.25, 0.,	0, 1,							NULL, NULL },
			{	NULL,	PARAM_REAL,	"neat_5_value",	.5, 0.,		-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,	"neat_5_max",	.75, 1.,	0, 1,							NULL, NULL },
			{	NULL,	PARAM_REAL,	"neat_6_min",	.25, 0.,	0, 1,							NULL, NULL },
			{	NULL,	PARAM_REAL,	"neat_6_value",	.5, 0.,		-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,	"neat_6_max",	.75, 1.,	0, 1,							NULL, NULL },
			{	NULL,	PARAM_REAL,	"neat_7_min",	.25, 0.,	0, 1,							NULL, NULL },
			{	NULL,	PARAM_REAL,	"neat_7_value",	.5, 0.,		-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,	"neat_7_max",	.75, 1.,	0, 1,							NULL, NULL },
			{	NULL,	PARAM_REAL,	"neat_8_min",	.25, 0.,	0, 1,							NULL, NULL },
			{	NULL,	PARAM_REAL,	"neat_8_value",	.5, 0.,		-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,	"neat_8_max",	.75, 1.,	0, 1,							NULL, NULL },
			ST_PARAM_BOOL_OFF( send_control )
	};
}

void	c_boul::param_init_pt()
{
INT32	h = 0;
	param_set_pt( h, b_active);
	param_set_pt( h, layers_time_out);
	param_set_pt( h, spiral_layers_time);
	param_set_pt( h, longvue_angle);
	param_set_pt( h, longvue_x);
	param_set_pt( h, longvue_y);
	h++;
	for( INT32 i = 0; i < 8; ++i )
		{
		param_set_pt( h, neat_min+i );
		param_set_pt( h, neat_value+i );
		param_set_pt( h, neat_max+i );
		}
	param_set_pt( h, b_send_control);

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE( c_boul )
{
	param_init_with( n_boulogne::param, n_boulogne::PARAM_NB_MAX );
}

c_boul::~c_boul()
{
}

BOOL	click_on = FALSE;
BOOL	fx_on = FALSE;
INT32	count_on;

INT32	nextt = 2;
	
void	c_boul::update()
{
	//INT32	i;
	REAL	value;
	REAL	ival;
	REAL	min;
	REAL	max;
	for( INT32 i = 0; i < 3; ++i )
		{
		value = neat_control_get(1, i);
		min = neat_min[i];
		max = neat_max[i];
		if ( value < min )
			value = 0;
		else if ( value > max )
			value = 1.;
		else
			value = (value-min)/(max-min);
		neat_value[i] = value;

		ival = value * 127;
		if ( neat_for_atau[i] != ival )
			{
			if ( midi && b_send_control )
				midi->send_control_change( i, ival );
			neat_for_atau[i] = ival;
			}
		}
	for( INT32 i = 0; i < 3; ++i )
		{
		value = neat_control_get(2, i);
		min = neat_min[i+4];
		max = neat_max[i+4];
		if ( value < min )
			value = 0;
		else if ( value > max )
			value = 1.;
		else
			value = (value-min)/(max-min);
		neat_value[i+4] = value;

		ival = value * 127;
		if ( neat_for_atau[i+4] != ival)
			{
			if ( midi && b_send_control )
				midi->send_control_change( i+4, ival );
			neat_for_atau[i+4] = ival;
			}

		}
	if( b_active )
		{
		if ( get_neat(CLICK_CHANNEL) >.5)
			{
			if ( click_on )
				{
				}
			else
				{
				if( count_on++>3 )
					{
					time_start();
					click_on = TRUE;
					if( fx_on )
						{
						layers_array[1].set_active(TRUE);
						fx_on = FALSE;
						}
					else
						{
//						next = IMOD(rand_boul.get_uint32()>>4,7) +2;
						nextt = IMOD(nextt-1,12) + 2;
						layers_array[nextt].set_active(TRUE);
						fx_on = TRUE;
						}
					}
				}
			}
		else
			{
			click_on = FALSE;
			count_on = 0;
			}
		}
}

