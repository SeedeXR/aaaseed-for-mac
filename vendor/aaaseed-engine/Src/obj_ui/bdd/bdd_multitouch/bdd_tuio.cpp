
#include "bdd_tuio.h"

#include "image/bind_img.h"
#include "image/bind_img_2d.h"
#include "obj_ui/com/net.h"
#include "obj_ui/com/osc/OscPrintReceivedElements.h"
#include "obj_ui/com/osc/OscOutboundPacketStream.h"
#include "draw/model.h"
#include "draw/tex.h"



FACTORY_CREATE_PROP_V1(c_bdd_tuio, bdd_tuio, TUIO, bdd_tuio, sub_menu="Multitouch"; );

//c_bdd_tuio*				c_bdd_tuio::cur = nullptr;
//CONST	INT32	MOUSE_ID_TRACKING = -1;


static INT32 CONST	EVENT_NB_UI = 32;
namespace n_bdd_tuio
{
	CONSTEXPR INT32	BASE_PARAM_NB	= 6 + c_bdd::GEO_PARAM_NB;
	CONSTEXPR INT32	IMG_PARAM_NB	= 7;
	CONSTEXPR INT32	NET_IN_PARAM_NB	= 12;
	CONSTEXPR INT32 GROUP_PARAM_NB	= 2;

	CONSTEXPR INT32 PARAM_NB	=	BASE_PARAM_NB
								+	IMG_PARAM_NB
								+	NET_IN_PARAM_NB
								+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB] =
	{
		BDD_BASE_PARAMS

	//	PARAM_DEF_BOOL_OFF( active )	
		PARAM_DEF_BOOL_OFF(		verbose					)
		PARAM_DEF_BOOL_ON(		image_update			)
		
		PARAM_DEF_GROUP( Image, IMG_PARAM_NB )
			PARAM_DEF_BOOL_ON(				image_clear			)
			PARAM_DEF_BIND_2D_CURRENT_SEL(	image_bind_dst		)
			PARAM_DEF_BANK_BIND_2D_OUT(		image_bind_out		)
		//todo regroup with PARAM_DEF_IMG_SIZE_FORMAT
			PARAM_DEF_IMG_SIZE(				image_size_x,		256, 512	)
			PARAM_DEF_IMG_SIZE(				image_size_y,		256, 512	)
			PARAM_DEF_INT32(				image_channel_nb,	3, 2,		1, 4	)
			PARAM_DEF_SYMBO(				image_channel_type,	GOL::INTERNAL_TYPE::UINT_16, GOL::INTERNAL_TYPE::UINT_8,		GOL::INTERNAL_TYPE::FLOAT_32,	GOL::internal_type_str	)

	//	PARAM_DEF_INT32(			dot_size, 8, 4, 1, 1024)
	//	PARAM_DEF_INT32(			point_max_nb, 16, 32, 1, PARAM_MAX_INT32)
		PARAM_DEF_POINT_XYZ(		center					)
		PARAM_DEF_INT32_LOCKED(	point_nb ) 

		PARAM_DEF_GROUP( In, NET_IN_PARAM_NB )
			//PARAM_DEF_BOOL_OFF(	net_in )
			PARAM_DEF_BOOL_ON(		tuio_in							)
			PARAM_DEF_INT32(		tuio_in_verbose,				1, 0,		0, 2 )
			PARAM_DEF_BOOL_OFF(		tuio_in_skip_alive				)
			PARAM_DEF_BOOL_OFF(		tuio_in_clear_each_frame		)
			PARAM_DEF_INT32_POS(	tuio_in_silent_frame_max_nb,	60,32 )
			PARAM_DEF_BOOL_OFF(		tuio_in_flip_x					)
			PARAM_DEF_BOOL_OFF(		tuio_in_flip_y					)
			PARAM_DEF_SCALE_XY(		tuio_in_scale					)
			PARAM_DEF_POINT_XY(		tuio_in_offset					)
		//	PARAM_DEF_REAL_ONE(		tuio_in_blob_size_u )
		//	PARAM_DEF_REAL_ONE(		tuio_in_blob_size_v )
		//	PARAM_DEF_REAL_POS(		tuio_in_blob_size_factor, 1./64., .01 )
	//		PARAM_DEF_INT32_LOCKED(	tuio_in_frame_nb )
			PARAM_DEF_INT32_LOCKED(	tuio_in_blk_received_nb )

	};
}

using namespace std;


void	c_bdd_tuio::param_init_pt()
{
	INT32	h = param_init_pt_geo();

	param_set_pt( h, _b_verbose_ui );
	param_set_pt( h, _b_img_update_ui );
	++h;
		param_set_pt( h, _b_img_clear_ui);
		param_set_pt( h, _bind_dst_ui );
		param_set_pt( h, _bind_dst );
		param_set_pt( h, _size_x_ui );
		param_set_pt( h, _size_y_ui );
		param_set_pt( h, _channel_nb_ui );
		param_set_pt( h, _s_channel_type_ui );

	//param_set_pt( h, _dot_size_ui );
//	param_set_pt( h, _blob_max_nb_ui );
	param_set_pt_3(	h, _center_ui );
	param_set_pt(		h, _blob_nb );
	++h;
	//	param_set_pt( h, _b_net_in );
		param_set_pt( h, _b_tuio_in );
		param_set_pt( h, _s_net_verbose_in );
		param_set_pt( h, _b_tuio_in_skip_alive );
		param_set_pt( h, _b_tuio_clear_each_frame_ui);
		param_set_pt( h, _tuio_silent_frame_max_nb );
		param_set_pt( h, _b_tuio_in_flip_x );
		param_set_pt( h, _b_tuio_in_flip_y );
		param_set_pt( h, _tuio_in_scale_x );
		param_set_pt( h, _tuio_in_scale_y );
		param_set_pt( h, _tuio_in_offset_x );
		param_set_pt( h, _tuio_in_offset_y );
		//param_set_pt_3( h, _tuio_in_blob_size_ui );
//		param_set_pt( h, _frame_in );
		param_set_pt( h, _tuio_in_message_received_nb );
	err_param_init_pt( h );
}


CONSTRUCTOR_CREATE( c_bdd_tuio )
//,_frame_in(0)
,_bind_dst(-1)
,_img_dst(nullptr)
,_size_x(0)
,_size_y(0)
//,_b_net_received(false)
,_net_no_frame(0)
,_tuio_in_message_received_nb(0)
{

	param_init_with( n_bdd_tuio::param, n_bdd_tuio::PARAM_NB );

//	clear_v3( m_origin );
}

c_bdd_tuio::~c_bdd_tuio()
{
	if( cur == this )
		cur = nullptr;
}


void c_bdd_tuio::update()
{
	_bind_dst = g_bind_img_2d->build_index_from_param( _bind_dst_ui );
	_img_dst = g_bind_img_2d->get_always( _bind_dst );
	//c_model::cur->get_size_v3( m_size );

	if( _b_tuio_clear_each_frame_ui )
		_receiving.clear();
	if( _b_tuio_in )
	{
		osc_take_and_process( osc::tuio_header, is_net_in_verbose() );
		++_net_no_frame;	// set to 0 when tuio process osc
		if( _net_no_frame > _tuio_silent_frame_max_nb )
		{
			_receiving.clear();
			_net_no_frame = 0;
			//DBG_PRINT_STRING("clearing events");
		}
		_blob_nb = static_cast<INT32>(_receiving.size());
	}
}

void c_bdd_tuio::render_blobs_in_img()
{
	if( _img_dst )
	{
		aaa::PIXEL_FORMAT	pixel_format	= aaa::c_pixel_format::make_format_from_channel_gltype( _channel_nb_ui, _s_channel_type_ui );
		//	GLenum	internal_format = GOL::make_internal_format_gl( _channel_nb_ui, _s_channel_type_ui );
		if( !_img_dst->is_size_format( _size_x_ui,_size_y_ui, pixel_format ) )
		{
			_img_dst->init_with_size( _size_x_ui,_size_y_ui, pixel_format, "TUIO Image" );
			_size_x = _img_dst->get_size_x();
			_size_y = _img_dst->get_size_y();
			_img_dst->fill_rgba( zero_v4fp32 );

			tex2d.bind( _bind_dst );
			tex2d.image_level( 0, _size_x_ui,_size_x_ui, _channel_nb_ui, _s_channel_type_ui ); 
		}
		if( _img_dst->is_valid() )
		{
			if( _receiving.empty() )
			{
				// Nothing received, clear image
				if( !_b_img_cleared )
				{
					if(_b_img_clear_ui )
						_img_dst->fill_compo( 0, 0. );
				//	src_img->move_to_gpu("TUIO");
					_b_img_cleared = true;
				}
			}
			else
			{
				static FP32		color_draw[4];

				if( _b_img_clear_ui )
					_img_dst->fill_compo( 0, 0. );

				for( const auto& dot : _receiving )
				{
					write_img( dot.x, dot.y, 0, 1. );
					//if (_b_draw_circle)
					//{
					//	for (INT32 v = MAX(0, y_pos - _dot_size_ui); v < MIN(_size_y, y_pos + _dot_size_ui); ++v)
					//	{
					//		for (INT32 u = MAX(0, x_pos - _dot_size_ui); u < MIN(_size_x, x_pos + _dot_size_ui); ++u)
					//		{
					//			if ((x_pos - u) * (x_pos - u) + (y_pos - v) * (y_pos - v) < _dot_size_ui * _dot_size_ui)
					//			{
					//				// inside the circle
					//				_img_dst->set_xy_color4r(u, v, color_draw);
					//			}
					//		}
					//	}
					//}
					//else
					//{
					//	for (INT32 v = MAX(0, y_pos - _dot_size_ui); v < MIN(_size_y, y_pos + _dot_size_ui); ++v)
					//	{
					//		for (INT32 u = MAX(0, x_pos - _dot_size_ui); u < MIN(_size_x, x_pos + _dot_size_ui); ++u)
					//		{
					//			//if ((x_pos - u) * (x_pos - u) + (y_pos - v) * (y_pos - v) < m_dot_size_ui * m_dot_size_ui)
					//			{
					//				// inside the circle
					//				_img_dst->set_xy_color4r(u, v, color_draw);
					//			}
					//		}
					//	}
					//}
				}
			//	src_img->move_to_gpu("TUIO");
				_b_img_cleared = false;
			}
		}
	}
}


void c_bdd_tuio::draw_single()
{
	if( _b_img_update_ui )
		render_blobs_in_img();
}
void c_bdd_tuio::draw_multiple()
{
	FP32 size[3];
	c_model::cur->get_size_v3( size );

	c_multiple::cur->set_nb( (INT32)_receiving.size() );

	REAL	p[3];
	p[2] = 0;
	for( const auto& dot : _receiving )
	{
		FP32 const x = dot.x - REAL(.5);
		FP32 const y = dot.y - REAL(.5);
		c_multiple::cur->set_index( dot.id );
		p[0] = x;
		p[1] = y;
		mul_add_v3( p, size, _center_ui );
		c_multiple::cur->align_then_draw( p );
	}

}


static	CONST	CHAR	TUIO_HEADER[] = "### TUIO -> ";
void	c_bdd_tuio::TUIO_PRINTF( C_PCHAR fmt, ... )
{
	va_list args;
	va_start( args, fmt );
	HEADER_PRINT_STRING_VA( TUIO_HEADER, fmt, args );
	va_end(args);
}


template <class T, class BoolFunction>
inline std::size_t get_index(const std::vector<T>& values, const BoolFunction& target)
{
	return std::distance(values.begin(), std::find_if(values.begin(), values.end(), target));
}
template<class T, class BoolFunction>
inline void remove(std::vector<T>& values, BoolFunction shouldErase)
{
	values.erase(std::remove_if(values.begin(), values.end(), shouldErase), values.end());
}

void	c_bdd_tuio::write_img( FP32 CONST x_in, FP32 CONST y_in, INT32 CONST channel_index, FP32 CONST value )
{
	INT32 const x = INT32(x_in * _size_x);
	INT32 const y = INT32(y_in * _size_y);
	//DBG_PRINT_STRING( "bdd_tuio %d %d ", x_pos, y_pos );
	if( _img_dst->is_valid_xy( x,y ) )
	{
		static FP32	color_draw[4];
		_img_dst->get_color4r_from_xy_low( color_draw, x, y );
		color_draw[channel_index] = value;
		_img_dst->set_xy_color4r_low( x, y, color_draw );
	}
}
//	documented at http://www.tuio.org/?specification
void	c_bdd_tuio::osc_receive_tuio_2Dcur( CONST osc::ReceivedMessage& msg )
{
	//	at least consume the block
	if( !_b_tuio_in )
		return;

	osc::ReceivedMessageArgumentStream		args	= msg.ArgumentStream();
	osc::ReceivedMessage::const_iterator	arg		= msg.ArgumentsBegin();

	_net_no_frame = 0;
	CONST char* cmd;
	args >> cmd;

	if( strcmp( cmd, "set" ) == 0 )
	{
		osc::int32	s_id;
		FP32		x, y, dx, dy;
		FP32		m_acc;
		REAL		a = 0.;
		REAL		da;

		if( strcmp( msg.TypeTags(), "sifffffff" )==0 )	//	TUIO 1.1 tbeta ???
		{
			args >> s_id >> x >> y >> a >> dx >> dy >> da >> m_acc >> osc::EndMessage;
		}
		else	//	TUIO 1.0 and 1.1 
		{
			args >> s_id >> x >> y >> dx >> dy >> m_acc >> osc::EndMessage;
		}

		//listener->updateTuioCur((int)s_id,x,y,X,Y,m);
		INT32	id = s_id;
		if( is_net_in_verbose_detailed() )
		{
			TUIO_PRINTF( "2Dcur set id %d, x,y : %f,%f, xspeed,yspeed : %f,%f, accel %f", id , x, y, dx, dy, m_acc );
		}

		if(  _b_tuio_in_flip_x )
			x = REAL(1) - x;
		//	it seem that tuio is flipped by default
		if( !_b_tuio_in_flip_y )
			y = REAL(1) - y;
		
		x = ((x - REAL(.5)) * _tuio_in_scale_x) + REAL(.5) + _tuio_in_offset_x; 
		y = ((y - REAL(.5)) * _tuio_in_scale_y) + REAL(.5) + _tuio_in_offset_y;

		// new blob
		c_tuio_item	blob( s_id, x,y, dx,dy, m_acc );


		if( _channel_nb_ui >= 2 && _img_dst->is_valid() )
			write_img( x,y, 1,1. );

		auto const index = get_index(_receiving, [s_id](const c_tuio_item& cur) { return cur.id == s_id; });
		if( index == _receiving.size() )
		{
			// New id
			_receiving.push_back(blob);
			//DBG_PRINT_STRING("new id %d", s_id);
		}
		else
		{
			_receiving[index] = blob;
			//DBG_PRINT_STRING("existing id %d", s_id);
		}
	}
	else if( strcmp( cmd, "alive" ) == 0 )
	{	//	list of alive blobs
		//	infact if blob don't move they don't need to be send with a set, so if alive we have to get the data from the last frame
		//DBG_PRINT_STRING("osc alive");
		if( is_net_in_verbose_detailed() )
		{
			TUIO_PRINTF( "2Dcur alive" );
		}
		if( _b_tuio_in_skip_alive )
		{
			return;
		}
		std::set<INT32>		alive_ids;

		osc::int32 s_id;
		while( !args.Eos() )
		{
			args >> s_id;
			if( is_net_in_verbose_detailed() )
				TUIO_PRINTF( "%d is alive", s_id );

			alive_ids.insert( s_id );

		}
		if( alive_ids.empty() )
		{
			_receiving.clear();
			//DBG_PRINT_STRING("Alive empty clearing events");
		}
		else
		{
			_receiving.erase(
				std::remove_if(
					_receiving.begin(),
					_receiving.end(),
					[&](c_tuio_item const& p) { return alive_ids.find( p.id ) == alive_ids.end(); }
				),
				_receiving.end()
			);
			//DBG_PRINT_STRING("Removing events, new size is %d", _receiving.size());
		}
	}
	else if( strcmp( cmd, "fseq" ) == 0 )
	{	//	start a frame
		osc::int32 currentFrame;
		args >> currentFrame  >> osc::EndMessage;
		if( currentFrame == -1 )
		{
			// no signification only redundant bundle
		}
		else
		{
			if( is_net_in_verbose() )
				TUIO_PRINTF( "2Dcur fseq %d with %d blobs", static_cast<INT32>(currentFrame), static_cast<INT32>(_receiving.size()) );
		}
	}
	else if( strcmp( cmd, "source" ) == 0 )
	{
		CONST char* str;
		args >> str  >> osc::EndMessage;
			// new bundle
		if( is_net_in_verbose_detailed() )
		{
			TUIO_PRINTF( "2Dcur source %s", str );
		}
	}
	else
	{
		ERR_PRINT_STRING( "TUIO %s() unknown cmd : %s", __FUNCTION__, cmd );
	}
}

void	c_bdd_tuio::osc_process_message( CONST osc::ReceivedMessage& msg )
{
	if( c_net::b_verbose_osc_processed_ui )
	{
		c_net::osc_print_message( "OSC Process :", msg );
	}
	CONST char*	addr =  msg.AddressPattern();
	if( str_is_equal( addr, "/tuio/", 6 ) )
	{
		++_tuio_in_message_received_nb;
		addr += 6;
		//	documented at http://www.tuio.org/?specification
		if( strcmp( addr, "2Dcur" ) == 0 )
		{
			osc_receive_tuio_2Dcur( msg ); 
		}
		else
		{
			ERR_PRINT_STRING( "%s() TUIO address not implemented : %.64s", __FUNCTION__, addr );
		}
	}
	else
	{
		ERR_PRINT_STRING( "%s() OSC address not implemented : %.64s", __FUNCTION__, addr );
	}
}
 