#include "draw/screen_def.h"
#include "obj_ui/com/net.h"
#ifndef	AAA_STR_SYMBO_H
#	include "ui/strsymbo.h"
#endif
#include "infrastructure/param/param.h"

INT32	s_screen_def_type;
REAL	screen_def_cos_freq;
REAL	screen_def_cos_limit;
REAL	screen_def_z_factor;
REAL	screen_def_y_origin;
REAL	screen_def_y_factor;

FACTORY_CREATE_V1( c_screen_def, screen deformation, Screen Deformation, screen_def );


CHAR*	str_screen_def_type[3] =
{
	"No",
	"Transform Deform",
	"Transform Deform Transform"
};

namespace	n_screen_def
{
	static	CONST	INT32	BANANA_PARAM_NB		= 6;
	static	CONST	INT32	NET_PARAM_NB		= 1;
	static	CONST	INT32	GROUP_PARAM_NB		= 2;
	static	CONST	INT32	PARAM_NB_MAX	=	BANANA_PARAM_NB
											+	NET_PARAM_NB
											+	GROUP_PARAM_NB;

	static	ST_PARAM	param[PARAM_NB_MAX] =
	{
		ST_PARAM_GROUP_CLOSED( Banana, BANANA_PARAM_NB )
			{	NULL,	PARAM_SYMBOLIC,	"screen_deformation",	1., 0.,		0., 2.,							NULL, str_screen_def_type },
			ST_PARAM_REAL_ZERO( cos_freq )
			{	NULL,	PARAM_REAL,		"cos_limit",			1., 2.,		-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
			ST_PARAM_REAL_ZERO( z_factor )
			ST_PARAM_REAL_ZERO( y_origin )
			ST_PARAM_REAL_ZERO( y_factor )
		ST_PARAM_GROUP_CLOSED( Net, NET_PARAM_NB )
			ST_PARAM_SYMBO_ZERO( net_channel,	1, 0, 0, c_net::CHANNEL_NB, gstr::no )
			//{	NULL,	PARAM_UINT32_SYMBO_ZERO, "net_channel", 1., 0.,		0, NET_CHANNEL_NB,			NULL, gstr::no },
	};
}

void	c_screen_def::param_init_pt()
{
INT32	h=0;
	h++;
	param_set_pt( h, _s_screen_def);
	param_set_pt( h, _cos_freq);
	param_set_pt( h, _cos_limit);
	param_set_pt( h, _z_factor);
	param_set_pt( h, _y_origin);
	param_set_pt( h, _y_factor);

	h++;
	param_set_pt( h, _net_channel);

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_screen_def)
{
	param_init_with( n_screen_def::param, n_screen_def::PARAM_NB_MAX );// screen_def_param, SCREEN_DEF_PARAM_NB);
}

void	c_screen_def::update()
{
#ifdef	NET
	if( _net_channel && net && net->is_slave() )
		net_receive( _net_channel, SCREEN_DEF_NET_TO_SEND );
#endif
	if( _s_screen_def && _cos_freq != 0. && (_z_factor != 0. || _y_factor != 0.) )
	{
		s_screen_def_type = _s_screen_def;
		screen_def_cos_freq = _cos_freq;
		screen_def_cos_limit = _cos_limit;
		screen_def_z_factor = _z_factor;
		screen_def_y_origin = _y_origin;
		screen_def_y_factor = _y_factor;
	}
	else
		s_screen_def_type = 0;
#ifdef	NET
	if( _net_channel && net && net->is_master() )
		net_send( _net_channel, SCREEN_DEF_NET_TO_SEND );
#endif
}

c_screen_def	screen_def_main;
c_screen_def*	screen_def_cur = &screen_def_main;
