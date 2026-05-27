#include "orgia.h"
#include "obj_ui/com/net.h"
#include "math/v_base.h"
#include "infrastructure/param/param_st.h"

FACTORY_CREATE_V1( c_orgia, orgia, Orgia, orgia );

c_orgia*	orgia;

namespace n_orgia
{
	static	CONST	INT32	BASE_PARAM_NB					= 1;
	static	CONST	INT32	ORGIA_PARAM_TEST_NB_MAX			=	2;
	static	CONST	INT32	ORGIA_PARAM_MESS_ABYSSE_NB_MAX	=	9;
	static	CONST	INT32	ORGIA_PARAM_MALE_NB_MAX			=	11;
	static	CONST	INT32	ORGIA_PARAM_FEMALE_NB_MAX		=	11;
	static	CONST	INT32	ORGIA_PARAM_APHARIA_NB_MAX		=	12;
	static	CONST	INT32	ORGIA_PARAM_MAX_NB_MAX			=	22;
	static	CONST	INT32	ORGIA_PARAM_GROUP_NB_MAX		= 6;

	static	CONST	INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
											+	ORGIA_PARAM_TEST_NB_MAX
											+	ORGIA_PARAM_MESS_ABYSSE_NB_MAX
											+	ORGIA_PARAM_MALE_NB_MAX
											+	ORGIA_PARAM_FEMALE_NB_MAX
											+	ORGIA_PARAM_APHARIA_NB_MAX
											+	ORGIA_PARAM_MAX_NB_MAX
											+	ORGIA_PARAM_GROUP_NB_MAX;

	static	ST_PARAM	param[PARAM_NB_MAX] =
	{
		ST_PARAM_BOOL_OFF( active )

		ST_PARAM_GROUP_CLOSED( Test, ORGIA_PARAM_TEST_NB_MAX )
			ST_PARAM_BOOL_OFF( trig_test_send_max_block )
			ST_PARAM_BOOL_OFF( b_trig_test_send_maa_block )

		ST_PARAM_GROUP_CLOSED( from abysse, ORGIA_PARAM_MESS_ABYSSE_NB_MAX )
			ST_PARAM_INT32_LOCKED( who )
			ST_PARAM_INT32_LOCKED( what )
			ST_PARAM_INT32_LOCKED( more )
			ST_PARAM_INT32_LOCKED( speed )
			ST_PARAM_INT32_LOCKED( size )
			ST_PARAM_INT32_LOCKED( color )
			ST_PARAM_REAL_LOCKED( target_x )
			ST_PARAM_REAL_LOCKED( target_y )
			ST_PARAM_REAL_LOCKED( target_z )

		ST_PARAM_GROUP( Hyssard Male, ORGIA_PARAM_MALE_NB_MAX )
			ST_PARAM_REAL_POS_ONE( hyssard_male_nb )
			ST_PARAM_POINT_XYZ( hyssard_male_target )
			ST_PARAM_REAL_ZERO( hyssard_male_target_radius )
			ST_PARAM_POINT_XYZ( hyssard_male )
			ST_PARAM_REAL_POS_ZERO( hyssard_male_speed )
			ST_PARAM_REAL_POS_ZERO( hyssard_male_size )
			ST_PARAM_REAL_POS_ZERO( hyssard_male_color )

		ST_PARAM_GROUP( Hyssard Female, ORGIA_PARAM_FEMALE_NB_MAX )
			ST_PARAM_REAL_POS_ONE( hyssard_female_nb )
			ST_PARAM_POINT_XYZ( hyssard_female_target )
			ST_PARAM_REAL_ZERO( hyssard_female_target_radius )
			ST_PARAM_POINT_XYZ( hyssard_female )
			ST_PARAM_REAL_POS_ZERO( hyssard_female_speed )
			ST_PARAM_REAL_POS_ZERO( hyssard_female_size )
			ST_PARAM_REAL_POS_ZERO( hyssard_female_color )

		ST_PARAM_GROUP( Apharia, ORGIA_PARAM_APHARIA_NB_MAX )
			ST_PARAM_REAL_POS_ONE( apharia_nb )
			ST_PARAM_POINT_XYZ( apharia_target )
			ST_PARAM_REAL_ZERO( apharia_target_radius )
			ST_PARAM_POINT_XYZ( apharia )
			ST_PARAM_REAL_POS_ZERO( apharia_speed )
			ST_PARAM_REAL_POS_ZERO( apharia_size )
			ST_PARAM_REAL_POS_ZERO( apharia_color )
			ST_PARAM_REAL_POS_ZERO( apharia_inter_distance )

		ST_PARAM_GROUP_CLOSED( to Max, ORGIA_PARAM_MAX_NB_MAX )
			{	NULL,	PARAM_REAL,	"max_send_radius",		1., 8.,		0, PARAM_INFINI,				NULL, NULL },
			ST_PARAM_REAL_ZERO( max_send_offset_z )

			{	NULL,	PARAM_REAL,	"projection_factor",	0., 1.69,	-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,	"projection_offset",	0., 4.,		-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
			ST_PARAM_REAL_ZERO( x_normalized_banc )
			ST_PARAM_REAL_ZERO( y_normalized_banc )

			ST_PARAM_REAL_ZERO( val_0 )
			ST_PARAM_REAL_ZERO( val_1 )
			ST_PARAM_REAL_ZERO( val_2 )
			ST_PARAM_REAL_ZERO( val_3 )
			ST_PARAM_REAL_ZERO( val_4 )
			ST_PARAM_REAL_ZERO( val_5 )
			ST_PARAM_REAL_ZERO( val_6 )
			ST_PARAM_REAL_ZERO( val_7 )
			ST_PARAM_REAL_ZERO( val_8 )
			ST_PARAM_REAL_ZERO( val_9 )
			ST_PARAM_REAL_ZERO( val_10 )
			ST_PARAM_REAL_ZERO( val_11 )
			ST_PARAM_REAL_ZERO( val_12 )
			ST_PARAM_REAL_ZERO( val_13 )
			ST_PARAM_REAL_ZERO( val_14 )
			ST_PARAM_REAL_ZERO( val_15 )
	};
}


void	c_orgia::param_init_pt()
{
INT32	h = 0;
//INT32	i;
	param_set_pt( h, get_pt_active() );

	h++;
		param_set_pt( h, b_trig_test_send_max_block);
		param_set_pt( h, b_trig_test_send_maa_block);

	h++;
		param_set_pt( h, mess.who );
		param_set_pt( h, mess.what );
		param_set_pt( h, mess.more );
		param_set_pt( h, mess.speed );
		param_set_pt( h, mess.size );
		param_set_pt( h, mess.color );
		param_set_pt_v3( h, mess.target );

	for( INT32 i=0; i<3; ++i )
		{
		h++;
		param_set_pt( h, group[i].nb );
		param_set_pt_v3( h, group[i].target );
		param_set_pt( h, group[i].target_radius );
		param_set_pt_v3( h, group[i].pos );
		param_set_pt( h, group[i].speed );
		param_set_pt( h, group[i].size );
		param_set_pt( h, group[i].color );
		}

		param_set_pt( h, apharia_inter_dist);

	h++;
		param_set_pt( h, max_send_radius);
		param_set_pt( h, max_send_offset_z);

		param_set_pt( h, projection_factor);
		param_set_pt( h, projection_offset);
		param_set_pt( h, x_normalized_banc);
		param_set_pt( h, y_normalized_banc);

		param_set_pt_v( h, value, 16 );

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_orgia)
{
	param_init_with( n_orgia::param, n_orgia::PARAM_NB_MAX ); // orgia_param, ORGIA_PARAM_NB_MAX);
	for( INT32 i = 0; i < 3; ++i )
		group[i].target_on_last = FALSE;
}

c_orgia::~c_orgia()
{
}

void	c_orgia::update()
{
static INT32	count = 0;
	if( b_trig_test_send_max_block )
		{
		if( net )
			{
			char str[200];
			sprintf( str, "Zobi la mouche %d et oui", count++ );
			net->sendto( 1, 0, BLK_MAX_BY_OPCODE, (UINT8*)str, strlen(str) );
			}
		b_trig_test_send_max_block = FALSE;
		}
	if( b_trig_test_send_maa_block )
		{
		if( net )
			{
			char str[200];
			sprintf( str, "Zobi la mouche %d et oui", count++ );
			net->sendto( 1, 1, BLK_ASCII, (UINT8*)str, strlen(str) );
			}
		b_trig_test_send_maa_block = FALSE;
		}
	if( net )
		{
		net_receive();
		send_hyssard_to_max();

		BOOL	b;
		for( INT32 i=0; i<3; ++i )
			{
			b = dist_v3r( group[i].pos, group[i].target ) <= group[i].target_radius; 
			if( b && !group[i].target_on_last)
				send_on_target(i+1);
			group[i].target_on_last = b;
			}
		}
}



void	c_orgia::send_mess_abysse_to_max()
{
CHAR	str[32];
INT32*	pt;
	pt= (INT32*)str;
	*pt++ = htonl(0);
	*pt++ = htonl(mess.who);
	*pt++ = htonl(mess.what);
	*pt++ = htonl(mess.more);
	*pt++ = htonl(mess.speed);
	*pt++ = htonl(mess.size);
	*pt++ = htonl(mess.color);
	net->sendto( 2, 2, BLK_MAX_BY_OPCODE, (UINT8*)str, 28 );
}

static CONST	INT32	FACTOR_FOR_ONE	= 65536;

void	c_orgia::send_hyssard_to_max()
{
CHAR	buf[512];
INT32*	pt;
REAL	tmp;
//INT32	i;
	pt= (INT32*)buf;
	*pt++ = htonl(0);
	*pt++ = htonl(257);

	tmp = FACTOR_FOR_ONE / max_send_radius;
	for( INT32 i=0; i<3; ++i )
		{
		*pt++ = htonl( INT32( group[i].nb)*FACTOR_FOR_ONE );
		*pt++ = htonl( INT32( group[i].pos[0]*tmp) );
		*pt++ = htonl( INT32( group[i].pos[1]*tmp) );
		*pt++ = htonl( INT32( group[i].pos[2]*tmp + max_send_offset_z*FACTOR_FOR_ONE) );
		}

	for( INT32 i=0; i<3; ++i )
		{
		REAL	x, y;
		tmp = projection_offset - group[i].pos[2];
		x = projection_factor * group[i].pos[0] / tmp;
		y = projection_factor * group[i].pos[1] * REAL(1.33) / tmp;

		*pt++ = htonl( INT32( x*FACTOR_FOR_ONE) );
		*pt++ = htonl( INT32( y*FACTOR_FOR_ONE) );

		if( i==2 )
			{
			x_normalized_banc = x;
			y_normalized_banc = y;
			}
		}

	net->sendto( 2, 2, BLK_MAX_BY_OPCODE, (UINT8*)buf, (3*(4+2)+2)*sizeof(INT32) );
}

void	c_orgia::net_receive_more( INT32* pi)
{
	switch( mess.what )
		{
		case 11:
			{
			}
			break;
		}
}

void	c_orgia::net_receive()
{
c_net_blk*	blk_in;
	while( blk_in = net->blk_take_by_type( BLK_ASCII ) )
	{
		if( blk_in->get_sender() == 1 )
		{
			INT32* pi = (INT32 *)blk_in->get_data_pt();

			mess.who	= ntohl( *pi++);
			mess.what	= ntohl( *pi++);
			mess.more	= ntohl( *pi++);

			mess.speed	= ntohl( *pi++);
			mess.size	= ntohl( *pi++);	
			mess.color	= ntohl( *pi++);

			mess.target[0] = INT32(ntohl( *pi++))/REAL(FACTOR_FOR_ONE);
			mess.target[1] = INT32(ntohl( *pi++))/REAL(FACTOR_FOR_ONE);
			mess.target[2] = INT32(ntohl( *pi++))/REAL(FACTOR_FOR_ONE);

			if( 1<=mess.who && mess.who<=3 )
			{
				st_group*	g;
				
				g = &group[mess.who-1];

				g->speed	=	mess.speed;
				g->size		=	mess.size;	
				g->color	=	mess.color;

				g->target[0]=	mess.target[0];
				g->target[1]=	mess.target[1];	
				g->target[2]=	mess.target[2];
				}

			net_receive_more( pi);

			send_mess_abysse_to_max();
		}
		net->blk_free( blk_in );
	}
}

void	c_orgia::send_on_target( INT32 who)
{
CHAR	str[12];
INT32*	pt;
	pt= (INT32*)str;
	*pt++ = htonl(who);
	*pt++ = htonl(1);
	*pt++ = htonl(0);
	net->sendto( 1, 2, BLK_ASCII, (UINT8*)str, 12 );
}

