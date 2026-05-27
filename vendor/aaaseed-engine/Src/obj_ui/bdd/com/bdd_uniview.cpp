#include "bdd_uniview.h"
#include "gol/gol.h"
#include "draw/model.h"
#include "draw/axe.h"
#include "math/billboard.h"
#include "draw/box.h"
#include "obj_ui/com/net_blk.h"


FACTORY_CREATE_PROP_V1( c_bdd_uniview, bdd_uniview, bdd for uniview interactive floor, bdd_uniview, sub_menu="Hardware"; );

namespace	n_bdd_uniview
{
	CONSTEXPR UINT32 PARAM_NB_MAX = 4 + c_bdd::GEO_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_BASE_PARAMS
		PARAM_DEF_POINT_XYZ( origin )
		PARAM_DEF_INT32_POS( blk_received, 0, 0 )
	};
}

void	c_bdd_uniview::param_init_pt()
{
	INT32	h = param_init_pt_geo();

	param_set_pt_3( h, _origin );
	param_set_pt( h, _blk_received );

	err_param_init_pt(h);
}

void c_bdd_uniview::init()
{
	param_init_with( n_bdd_uniview::param, n_bdd_uniview::PARAM_NB_MAX);
}

CONSTRUCTOR_CREATE(c_bdd_uniview)
{
	init();
}
EMPTY_DESTRUCTOR(c_bdd_uniview)

void c_bdd_uniview::update()
{
	c_model::cur->get_size_v3( _size );

//	while( c_net_blk* blk_in = net->blk_take_by_type( c_net::BLK_UNIVIEW ) )
//	{
//		CONST UINT8*	pt		=	blk_in->get_data_pt();
//		CONST UINT8*	pt_max	=	pt + blk_in->get_len();
//		DBG_PRINT_STRING( "uniview len %d", blk_in->get_len() );
//		++_blk_received;
		//while( pt < pt_max )
		//{	
			//INT32	index;
			//INT32	what;
			//pt = c_net::read_value( pt, &what );
			//switch( what )
			//{
			//case NET_CODE_CREATE:	pt = c_net::read_value( pt, &index );
			//	pt = _poids.get_pt_from_index(index)->read_from_mem( pt, t );
			//	break;
			//case NET_CODE_KILL:		pt = c_net::read_value( pt, &index );
			//	_poids.get_pt_from_index(index)->set_dead();
			//	break;
			//default:				debug_break( "bad net code" );
			//	break;
			//}
		//}
//		net->blk_free( blk_in );
//	}
}

void c_bdd_uniview::draw_single()
{
	::draw_box_at( _size, _origin );
}

void c_bdd_uniview::draw_multiple()
{
	c_multiple::cur->set_nb( 1 );
	c_multiple::cur->align_then_draw( _origin );
}

INT32	c_bdd_uniview::get_point_nb()
{
	return 1;
}

REAL*	c_bdd_uniview::get_point_pt( INT32 CONST index )
{
	return _origin;
}
