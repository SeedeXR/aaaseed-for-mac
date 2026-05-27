
#include "gol/gol_draw.h"

//	pour memoire
#if	AAA_SCREEN_DEFORMATION()

#include "bdd_def_vertex.h"
#include "obj_ui/com/net.h"


FACTORY_CREATE_PROP_V1( c_bdd_def_vertex, bdd_deformer_vertex, Projection deformation, bdd_def_vertex, sub_menu="Special"; );

static	C_PCHAR_C	str_def_vertex_type[4] =
{
	"No",
	"Vertex_Shader",
	"Transform Deform",
	"Transform Deform Transform",
};

namespace	n_bdd_def_vertex
{
	CONSTEXPR INT32	BASE_PARAM_NB	= 1;
	CONSTEXPR INT32	BANANA_PARAM_NB	= 5;
	CONSTEXPR INT32	NET_PARAM_NB	= 1;
	CONSTEXPR INT32	GROUP_PARAM_NB	= 2;
	CONSTEXPR INT32	PARAM_NB	=	BASE_PARAM_NB
								+	BANANA_PARAM_NB
								+	NET_PARAM_NB
								+	GROUP_PARAM_NB;


	CONSTEXPR INT32	NET_TO_SEND = PARAM_NB-1-NET_PARAM_NB;

	CONST c_param_def paramPARAM_NB] =
	{
		PARAM_DEF_SYMBO_PSTR_ZERO( method,	str_def_vertex_type )
		PARAM_DEF_GROUP_CLOSED( Banana, BANANA_PARAM_NB )
			PARAM_DEF_REAL_ZERO(	cos_freq )
			PARAM_DEF_REAL_INF(		cos_limit, 1, 2 )
			PARAM_DEF_REAL_ZERO(	z_factor )
			PARAM_DEF_REAL_ZERO(	y_origin )
			PARAM_DEF_REAL_ZERO(	y_factor )
		PARAM_DEF_GROUP_CLOSED( Net, NET_PARAM_NB )
			PARAM_DEF_SYMBO_ZERO(	net_channel,	1, 0,	0, c_net::CHANNEL_NB,	gstr::no	)
	};
}

void	c_bdd_def_vertex::param_init_pt()
{
	INT32	h=0;
	param_set_pt( h, _s_screen_def);

	++h;
	param_set_pt(h, _cos_freq);
	param_set_pt(h, _cos_limit);
	param_set_pt(h, _z_factor);
	param_set_pt(h, _y_origin);
	param_set_pt(h, _y_factor);

	++h;
		param_set_pt( h, _net_channel );

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_bdd_def_vertex)
{
	param_init_with( n_bdd_def_vertex::param, n_bdd_def_vertex::PARAM_NB );
}
EMPTY_DESTRUCTOR(c_bdd_def_vertex)

void	c_bdd_def_vertex::update()
{
//#if	AAA_NET()
	if( _net_channel && net && net->is_slave() )
		net_receive_param( _net_channel, n_bdd_def_vertex::NET_TO_SEND );
//#endif
	if( _s_screen_def && _cos_freq != 0. && ( _z_factor != 0. || _y_factor != 0.) )
	{
		GOL::s_screen_def_type = _s_screen_def;
		GOL::screen_def_cos_freq = _cos_freq;
		GOL::screen_def_cos_limit = _cos_limit;
		GOL::screen_def_z_factor = _z_factor;
		GOL::screen_def_y_origin = _y_origin;
		GOL::screen_def_y_factor = _y_factor;
	}
	else
		GOL::s_screen_def_type = 0;
//#if	AAA_NET()
	if( _net_channel && net && net->is_master() )
		net_send_param( _net_channel, n_bdd_def_vertex::NET_TO_SEND );
//#endif
}

void	c_bdd_def_vertex::draw()
{
}

c_bdd_def_vertex*	bdd_def_vertex_cur = nullptr;

#endif // #if	AAA_SCREEN_DEFORMATION()