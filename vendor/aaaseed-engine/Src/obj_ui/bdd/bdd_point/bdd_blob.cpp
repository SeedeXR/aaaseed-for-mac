#include "bdd_blob.h"
#include "obj_ui/bdd/bdd_ui_pref.h"
#include "gol/gol.h"
#include "draw/model.h"
#include "math/rand.h"

FACTORY_ABSTRACT_CREATE_V1( c_bdd_blob_base, bdd_blob_base, Bdd Blob Base );
CONSTRUCTOR_ABSTRACT_CREATE(c_bdd_blob_base)
{
}
EMPTY_DESTRUCTOR(c_bdd_blob_base)

FACTORY_CREATE_PROP_V1( c_bdd_blob, bdd_blob, Blob, bdd_blob, sub_menu="Point"; );

c_bdd_blob*	c_bdd_blob::cur = nullptr;
namespace n_bdd_blob
{
	CONSTEXPR INT32 BASE_PARAM_NB		= 6 + c_bdd::GEO_PARAM_NB;
	CONSTEXPR INT32 MULTIPLY_PARAM_NB	= 5;
	CONSTEXPR INT32 DRAW_PARAM_NB		= 7;
	CONSTEXPR INT32 FIDUCIAL_PARAM_NB	= 2;
	CONSTEXPR INT32 NET_PARAM_NB		= 5;
	CONSTEXPR INT32 GROUP_PARAM_NB		= 4;

	CONSTEXPR INT32 PARAM_NB	=	BASE_PARAM_NB
								+	MULTIPLY_PARAM_NB
								+	DRAW_PARAM_NB
								+	FIDUCIAL_PARAM_NB
								+	NET_PARAM_NB
								+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB] =
	{
		BDD_BASE_PARAMS
		PARAM_DEF_NONE( Bdd_ui_pref )

//		PARAM_DEF_INT32_LOCKED( contour_discarded_nb )
		PARAM_DEF_BOOL_OFF(		blob_reset_trig	)
		PARAM_DEF_INT32_LOCKED(	blob_nb )

//		{	nullptr,	PARAM_INT32,	"blob_point_nb_min",		2, 1,	1, PARAM_MAX_INT32,	nullptr, nullptr },		
//		{	nullptr,	PARAM_INT32,	"blob_point_nb_max",		12, 10,	1, PARAM_MAX_INT32,	nullptr, nullptr },		
//		PARAM_DEF_REAL_POS_ZERO( blob_area_min )
//		PARAM_DEF_REAL_POS_ONE( blob_area_max )
//		PARAM_DEF_BOOL_OFF( polygon_approximation )
//		{	nullptr,	PARAM_REAL,	"polygon_approximation_factor",	1, .02,	0, PARAM_MAX_REAL,	nullptr, nullptr },

		PARAM_DEF_POINT_XYZ(	center	)

		PARAM_DEF_GROUP_CLOSED( Multiply, MULTIPLY_PARAM_NB )
			PARAM_DEF_BOOL_OFF(	multiply )
			PARAM_DEF_INT32(	multiply_nb,		2, 1,		1, 1024	)
			PARAM_DEF_REAL(		multiply_range_x,	1, .125,	0, 4	)
			PARAM_DEF_REAL(		multiply_range_y,	1, .125,	0, 4	)
			PARAM_DEF_BOOL_ON(	multiply_unstable	)

		PARAM_DEF_GROUP_CLOSED( Draw, DRAW_PARAM_NB )
//			PARAM_DEF_BOOL_OFF(		contour_draw_discarded		)
			PARAM_DEF_BOOL_OFF(		contour_draw				)
//			PARAM_DEF_REAL_ONE(		contour_draw_alpha			)
			PARAM_DEF_BOOL_OFF(		contour_draw_as_quad		)
			PARAM_DEF_BOOL_OFF(		bounding_draw				)
//			PARAM_DEF_REAL_ONE(		bounding_draw_alpha			)
			PARAM_DEF_REAL_ONE(		bounding_draw_size_factor	)
			PARAM_DEF_REAL_ZERO(	bounding_draw_size_min		)
			PARAM_DEF_BOOL_OFF(		bounding_draw_ellipse		)
			PARAM_DEF_INT32(		bounding_draw_ellipse,		24, 12,	1, 2048	)

		PARAM_DEF_GROUP_CLOSED( Fiducial, FIDUCIAL_PARAM_NB )
//			PARAM_DEF_BOOL_OFF(		fiducial_active )
//			{	nullptr,	PARAM_REAL,	"fiducial_size_min",	1000, 200, 0, PARAM_MAX_REAL,	nullptr, nullptr },		
//			PARAM_DEF_INT32_LOCKED(	fiducial_discarded_nb )
			PARAM_DEF_INT32_LOCKED(	fiducial_nb		)
			PARAM_DEF_BOOL_OFF(		fiducial_draw	)

		PARAM_DEF_GROUP_CLOSED( Net, NET_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		net_received		)
			PARAM_DEF_BOOL_OFF(		net_send			)
			PARAM_DEF_NET_LINK(		net_link,			1, 0	)
			PARAM_DEF_INT32(		net_channel,		2, 1,	1, c_net::CHANNEL_NB		)
			PARAM_DEF_INT32(		net_channel_sub,	2, 1,	1, c_net::CHANNEL_SUB_NB	)
	};
}

void	c_bdd_blob::param_init_pt()
{
	INT32	h = param_init_pt_geo();

	param_attach_obj( h, c_bdd_ui_pref::cur );

//	param_set_pt(		h, _contour_discarded_nb	);
	param_set_pt(		h, _blob_reset_trig_ui		);
	param_set_pt(		h, _blob_nb					);
//	param_set_pt(		h, _blob_point_nb_min		);
//	param_set_pt(		h, _blob_point_nb_max		);
//	param_set_pt(		h, _blob_area_min			);
//	param_set_pt(		h, _blob_area_max			);
	param_set_pt_3(	h, _center_ui				);

	++h;
		param_set_pt( h, _b_multiply_blob			);
		param_set_pt( h, _multiply_nb				);
		param_set_pt( h, _multiply_range_x			);
		param_set_pt( h, _multiply_range_y			);
		param_set_pt( h, _b_multiply_unstable		);

	++h;
	//	param_set_pt( h, _b_contour_draw_discarded_ui	);
		param_set_pt( h, _b_contour_draw_ui				);
	//	param_set_pt( h, _contour_draw_alpha_ui			);
		param_set_pt( h, _b_contour_draw_as_quad_ui		);
		param_set_pt( h, _b_bounding_draw_ui			);
	//	param_set_pt( h, _bounding_draw_alpha_ui		);
		param_set_pt( h, _bounding_draw_size_factor		);
		param_set_pt( h, _bounding_draw_size_min		);
		param_set_pt( h, _b_bounding_draw_ellipse_ui	);
		param_set_pt( h, _ellipse_seg_nb				);

	++h;
	//	param_set_pt( h, _b_fiducial				);
	//	param_set_pt( h, _fiducial_size_min			);
	//	param_set_pt( h, _fiducial_discarded_nb		);
		param_set_pt( h, _fiducial_nb				);
		param_set_pt( h, _b_fiducial_draw			);

	++h;
		param_set_pt( h, _net_buf.get_receive_ui_pt()		);
		param_set_pt( h, _net_buf.get_send_ui_pt()			);
		param_set_pt( h, _net_buf.get_net_link_pt()			);
		param_set_pt( h, _net_buf.get_net_channel_pt()		);
		param_set_pt( h, _net_buf.get_net_channel_sub_pt()	);

	err_param_init_pt( h );
}

CONSTRUCTOR_CREATE( c_bdd_blob )
,_blob_nb(0)
,_fiducial_nb(0)
,_b_blob_inserted(false)
//_contour_discarded_nb(0),
//_fiducial_kept_nb(0)
{
	param_init_with( n_bdd_blob::param, n_bdd_blob::PARAM_NB );
}

c_bdd_blob::~c_bdd_blob()
{
	if( cur == this )
		cur = nullptr;
}

void	c_bdd_blob::clear_blob()
{
	_blob.clear();
}

void	c_bdd_blob::add_blob( INT32 id, REAL x, REAL y, REAL sx, REAL sy  )
{
	_b_blob_inserted = true;

	c_blob	blob;
	blob.set_quick( id, x, y, sx, sy );
	_blob.push_back( blob );
}

static	c_rand_lin	rnd;

void	c_bdd_blob::update()
{
	c_model::cur->get_size_v3( _size );
	cpy_v3( _center, _center_ui );
//	_center[2] = _center_ui[2];
	if( _blob_reset_trig_ui )
	{
		_blob_reset_trig_ui = false;
		_blob.clear();
	}
	if( _b_blob_inserted )
		_b_blob_inserted = false;
	else
		_blob.clear();
	if( !_bdd_src.empty() )
	{
		for( auto const & p_bdd : _bdd_src )
		{
			p_bdd->transfer_blobs_to( _blob );
		}
		_bdd_src.clear();
		//c_blob::scale_translate( _blob, _size, _center );
		//do_process();
	}
	//hack this should be change to take in account the received stuff
	//get_point_nb() .. should also be redone

	_net_buf.update();
	if( _net_buf.is_send() )
	{
		_net_buf.send_cont<BLOBS_CONT,c_blob>( _blob );
	}
	if( _net_buf.is_receive() )
	{
		c_net_blk*	blk_in;
		while( blk_in = net->blk_take_by_type_channel( c_net::BLK_OBJ_DATA, _net_buf.get_net_channel() ) )
		{
			TYPE_MAP_RECEIVER::receiver_type*	receiver = _map_receiver.get_receiver( blk_in->get_sender(),  blk_in->get_channel_sub() );
			receiver->do_receive( blk_in );
			net->blk_free( blk_in );
		}
		for( auto const & p : _map_receiver.get_map_ref() )
		{
			for( auto const & blob : p.second->get_objs() )
			{
				_blob.push_back( blob );
			}
			//_blob.insert( _blob.end(), it->second->get_objs().begin(), it->second->get_objs().end() );
		}
	}
	if( _b_multiply_blob )
	{
		if( !_b_multiply_unstable )
			rnd.set_seed(8888);
		BLOBS_CONT	blob_added;
		blob_added.reserve( _blob.size() * _multiply_nb );
		c_blob		blob;
		for( auto const & b : _blob )
		{
			for( INT32 i=_multiply_nb; i>0; --i )
			{
				REAL x = rnd.get_fp32() * _multiply_range_x * REAL(.5);
				REAL y = rnd.get_fp32() * _multiply_range_y * REAL(.5);
				blob.set( b );
				blob.offset_pos( x, y );
				blob_added.push_back(blob);
			}
		}
		_blob.insert( _blob.end(), blob_added.begin(), blob_added.end() );
	}
	_blob_nb = (UINT32)_blob.size();

	cur = this;
}

/*
void	c_bdd_blob::do_process()
{
	c_blob::scale_translate( _blob, _size, _center );
}
*/

void	c_bdd_blob::register_as_src( c_bdd* src )
{
	_bdd_src.insert( src );
}

void	c_bdd_blob::draw_blobs( BLOBS_CONT& blobs )
{	
	//	if( _b_contour_draw_discarded_ui )
	//	{
	//		GOL::color4( 0,.5,.5, _contour_draw_alpha_ui );
	//		c_blob::draw_contour( _blob_discarded );
	//	}
	//todo propagate change to center and size only for draw here
	if( _b_contour_draw_ui )
		c_blob::draw_contour( blobs, _b_contour_draw_as_quad_ui, _center[2] );
	if( _b_bounding_draw_ui ) //&& _bounding_draw_alpha_ui!=0. )
	{
		c_blob::ellipse_seg_nb = _ellipse_seg_nb;
		c_blob::draw_bounding( blobs, _center, _size, _bounding_draw_size_factor, _bounding_draw_size_min, _b_bounding_draw_ellipse_ui );
	}
	if( _b_fiducial_draw )
		c_blob::draw_fiducial( blobs );
}

void	c_bdd_blob::draw_single()
{	
	draw_blobs( _blob ); 
}

void	c_bdd_blob::draw_multiple()
{
	//todo	c_multiple::cur->set_nb( nb_to_draw );

	c_blob::sta_size		= _bounding_draw_size_factor;	//hack now clean and generalize to all this object draw path
	c_blob::sta_size_min	= _bounding_draw_size_min;
	//c_blob::draw_multiple( _blob, _center[2] );
	c_blob::draw_multiple( _blob, _center, _size );
/*
	TYPE_MAP_RECEIVER::map_type	map_rec = _map_receiver.get_map_ref();
	TYPE_MAP_RECEIVER::map_type::const_iterator it_end = map_rec.end();
	for( TYPE_MAP_RECEIVER::map_type::iterator it=map_rec.begin(); it!=it_end; ++it )
		c_blob::draw_multiple( it->second->get_objs(), _center[2] );
*/
}

REAL	c_bdd_blob::get_field_at( REAL* pos )
{
	REAL	value;
	INT32	nb;
	c_blob::get_field_at( _blob, pos, value, nb );
/*
	TYPE_MAP_RECEIVER::map_type	map_rec = _map_receiver.get_map_ref();
	TYPE_MAP_RECEIVER::map_type::const_iterator it_end = map_rec.end();
	for( TYPE_MAP_RECEIVER::map_type::iterator it=map_rec.begin(); it!=it_end; ++it )
		c_blob::get_field_at( it->second->get_objs(), pos, value, nb );
*/
	if( nb == 0 )
		return 0.;
	return value / REAL(nb);
}

void	c_bdd_blob::get_field_gradient_at( REAL* grad,  REAL* pos )
{
	clear_v3( grad );

	REAL	value = 0;
	INT32	nb = 0;
	c_blob::add_field_gradient_at( grad, _blob, pos );

/*
	TYPE_MAP_RECEIVER::map_type	map_rec = _map_receiver.get_map_ref();
	TYPE_MAP_RECEIVER::map_type::const_iterator it_end = map_rec.end();
	for( TYPE_MAP_RECEIVER::map_type::iterator it=map_rec.begin(); it!=it_end; ++it )
		c_blob::add_field_gradient_at( grad, it->second->get_objs(), pos );
*/
}

INT32	c_bdd_blob::get_point_nb()										{	return get_blob_nb();	}
INT32	c_bdd_blob::get_point_and_id( REAL* dst, INT32 CONST index )	{	_blob[index].get_center(dst); return _blob[index].get_id();	}
