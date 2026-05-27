
#include "net.h"
#include "net_link.h"	//because winsock2 need to be first
#include "net_blk.h"
#include "infrastructure/param/param_declare.h"
#include <lm.h>
#include "ui/AAA_event.h"
#include "time/aaa_time.h"
#include "obj_ui/tracker/trackers.h"
#include "spy.h"
#include <lib_use.h>
#include "file/aaa_dir.h"


AAA_LIB_USE_MESSAGE( "netapi32" )

CHAR	CONST NET_HEADER[] = "# NET ";
//todo do one by channel
void	NET_PRINT_STRING( C_PCHAR_C fmt, ...)
{
	va_list args;
	va_start( args, fmt );
	trackers::PRINT_STRING_VA( NET_HEADER, fmt, args );
	va_end(args);
}

FACTORY_CREATE_V1( c_net, net, Net, net );

//todo
//#define	VERSION			0

bool	c_net::b_print_send = false;
bool	c_net::b_print_receive = false;

bool	c_net::b_midi_send = false;
bool	c_net::b_midi_receive = false;
INT32	c_net::midi_dst;
INT32	c_net::print_dst;
bool	c_net::b_midi_hack_send_as_maxmsp;


bool	c_net::b_test_blk_lost			= false;
bool	c_net::b_test_blk_lost_last		= false;
bool	c_net::b_test_blk_lost_verbose	= false;
INT32	c_net::test_blk_lost_nb			= 0;
INT32	c_net::test_blk_lost_receive_nb = 0;
bool	c_net::b_blk_received_always_invalid = false;
INT32	c_net::blk_received_nb			= 0;
INT32	c_net::blk_received_self_nb		= 0;
INT32	c_net::blk_recycled_by_force_nb = 0;
INT32	c_net::blk_sent_nb				= 0;
INT32	c_net::blk_out_error_nb			= 0;
INT32	c_net::img_received_nb			= 0;
INT32	c_net::img_sent_nb				= 0;
INT32	c_net::img_err_nb				= 0;

CONSTEXPR	INT32	BLK_SIZE_MAX_DEF = 1472 - c_net::BLK_HEADER_SIZE;	//todobeurk
INT32				c_net::_blk_size_max = BLK_SIZE_MAX_DEF;

c_net::OSC_TAKING	c_net::osc_taking;	//uses to avoidretestingf every osc blk


namespace {
	thread_local UINT8	net_buf_send[c_net::BLK_SIZE_MAX];
}


/*
INT32 CONST	EVENT_MAX_NB = 32;
struct	ST_NET_EVENT	event_net[EVENT_MAX_NB];
static	INT32	event_net_nb;
static	INT32	event_net_index;
static	bool	b_can_add_event = true;
*/

//NET MATRIX
//this is used to transmit the trax thru FN_NET_IN and param net_out
//todo move this outside of the net area ?

static	REAL	net_matrix_value[c_net::MATRIX_CHANNEL_NB][c_net::MATRIX_CONTROL_NB];

FINLINE	bool	net_matrix_check_index( INT32 channel_id, INT32 control_index )
{
	if( OUTSIDE_MIN_MAX( channel_id, 1, c_net::MATRIX_CHANNEL_NB ) )
	{
		DBG_PRINT_STRING( "Net_channel is %d and should be between 1 and %d", channel_id, c_net::MATRIX_CHANNEL_NB );
		return false;
	}
	if( OUTSIDE_MIN_MAX( control_index, 0, c_net::MATRIX_CONTROL_NB-1 ) )
	{
		DBG_PRINT_STRING( "Net_control is %d and should be between 0 and %d", control_index, c_net::MATRIX_CONTROL_NB-1 );
		return false;
	}
	return true;
}

REAL	c_net::get_matrix_value( INT32 channel_id, INT32 control_index )
{
	return net_matrix_check_index( channel_id, control_index ) ? net_matrix_value[channel_id-1][control_index] : REAL(0);		
}

void	c_net::set_matrix_value( INT32 channel_id, INT32 control_index, REAL in )
{
	if( net_matrix_check_index( channel_id, control_index ) )
		net_matrix_value[channel_id-1][control_index] = in;
}

//todonow	reuse blk
//this can be dangerous as it
//memory (data) too should be used smartly
//	this fn just store the blk in the free list for reuse
void	c_net::blk_free( c_net_blk* pblk )
{
	std::lock_guard<aaa::MUTEX> guard(_ta_blk_free);
	_blk_free.push_back( pblk );	
}

c_net_blk*	c_net::blk_get()
{
	c_net_blk* pblk;
	_ta_blk_free.lock();		//	then we will have to unlock for every code path
	if( _blk_free.empty() )
	{	//	no free block available
		_ta_blk_free.unlock();	//	we do it as soon as possible
		if( _blk_allocated_nb < _blk_allocated_max )
		{	//	allocate
			pblk = new c_net_blk;
			++_blk_allocated_nb;
		} 
		else
		{	//	or reuse the oldest one received and not processed
			pblk = blk_take_oldest();
			++blk_recycled_by_force_nb;
		}
	}
	else
	{	//	use free block
		pblk = _blk_free.front();
		_blk_free.pop_front();
		_ta_blk_free.unlock();
	}
	return pblk;
}

void c_net::blk_receive( INT32 link, CONST UINT8* CONST buf, INT32 len )
{
	c_net_blk*	pblk = blk_get();
	if( !pblk )
	{
		++_blk_get_failed_nb;
		return;
	}

	pblk->check_and_store( link, buf, len );	//we use len because non native AAASeed don't give explicitly their len

	if( pblk->is_valid() )
	{
		if( _b_verbose_in )
			pblk->print();

		_net_time_received = pblk->get_time();
		_b_net_time_received = true;

		if( pblk->is_immediate() )
		{
			pblk->do_process( _b_verbose_in );
			if( _b_verbose_in )	
				net->get_link(link)->print_string( "BLK processed" );
			blk_free( pblk );
		}
		else
		{
			net->blk_store( pblk );
			if( _b_verbose_in )
				net->get_link(link)->print_string( "BLK stored" );
			//no delete
		}
	}
	else
	{
		blk_free( pblk );
	}
	/*
	if ( strcmp( pt, "ping")== 0 )
		{
		if( _b_ping_answer )
			send( 0, BLK_PING, "ping", 4);
		if ( (++ping_nb & 0xff) == 0 )
			{
			GOOD_PRINT_STRING( "pinging %d times", ping_nb );
			}
		}
	*/
}


//todoqqq
/*
//todo check everywhere for len_ not too long
void	net_event_store( CHAR *pt, INT32 len )
{
	INT32	i;
	INT32	*pt_i;
	ST_NET_EVENT	*pt_event;

	while( !b_can_add_event )
	{
	}

	if ( event_net_nb < EVENT_MAX_NB)
	{
		len /= sizeof(INT32);
		pt_i = (INT32 *)pt;
		pt_event = &event_net[event_net_nb];

		pt_event->type_ = *pt_i++;
		--len;
		pt_event->value_nb = len;
		for ( i=0; i<len; ++i )
		{
			pt_event->value[i] = *pt_i++;
		}
		++event_net_nb;
	}
	else
		DBG_PRINT_STRING("Too much event in the net buffer");
}


void	net_event_read_start()
{
	b_can_add_event = false;
	event_net_index = 0;
}

ST_NET_EVENT *	net_event_get_next()
{
ST_NET_EVENT * pt;
	if ( event_net_index < event_net_nb )
	{
		pt = &event_net[event_net_index];
		++event_net_index;
	}
	else
	{
		event_net_nb = 0;
		pt = nullptr;
	}
	return pt;
}

void	net_event_read_stop()
{
	b_can_add_event = true;
}
*/

c_net*		net = nullptr;

static	C_PCHAR_C str_same_port[1] =
{
	"Use main port"
};

static	C_PCHAR_C	str_off[1] =
{
	"Off"
};

namespace	n_net
{
	CONSTEXPR INT32	BASE_PARAM_NB		= 17 + c_net::HOST_IP_NB;
	CONSTEXPR INT32	REMOTE_PARAM_NB		= 6;
	CONSTEXPR INT32	CHANNEL_PARAM_NB	= 2;
	CONSTEXPR INT32	LINK_PARAM_NB		= c_net::LINK_NB;
	CONSTEXPR INT32	FRUSTRUM_PARAM_NB	= 2;
	CONSTEXPR INT32	TEK_PARAM_NB		= 19;
	CONSTEXPR INT32	CUSTOM_PARAM_NB		= 1;
	CONSTEXPR INT32	OSC_PARAM_NB		= 13;
	CONSTEXPR INT32	LAN_HOPARAM_DEF_NB	= 4;
	CONSTEXPR INT32	GROUP_PARAM_NB		= 7;

	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	REMOTE_PARAM_NB
									+	CHANNEL_PARAM_NB
									+	LINK_PARAM_NB
									+	FRUSTRUM_PARAM_NB
									+	TEK_PARAM_NB
									+	CUSTOM_PARAM_NB
									+	OSC_PARAM_NB
									+	LAN_HOPARAM_DEF_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_OFF(		active					)
		PARAM_DEF_BOOL_OFF(		verbose_socket			)
		PARAM_DEF_BOOL_OFF(		verbose_in				)
		PARAM_DEF_BOOL_OFF(		verbose_out				)
		
		PARAM_DEF_GROUP_CLOSED(	Osc, OSC_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		osc_verbose_received			)
			PARAM_DEF_BOOL_OFF(		osc_verbose_processed			)
			PARAM_DEF_BOOL_OFF(		osc_verbose_out					)
			PARAM_DEF_BOOL_OFF(		osc_verbose_detailed			)
			PARAM_DEF_BOOL_ON(		osc_lua_accept					)
			PARAM_DEF_BOOL_ON(		osc_lua_verbose					)
			PARAM_DEF_BOOL_ON(		osc_midi_control_accept			)
			PARAM_DEF_BOOL_OFF(		osc_midi_control_verbose		)
			PARAM_DEF_BOOL_ON(		osc_variable_set_accept			)
			PARAM_DEF_BOOL_OFF(		osc_variable_set_verbose		)
			PARAM_DEF_SYMBO_ZERO(	osc_packet_size,				2048, 1500,				24, 32000, nullptr )
			PARAM_DEF_REAL_ONE(		osc_blk_unused_keep_time		)
			PARAM_DEF_INT32_LOCKED(	osc_blk_nb						)

		PARAM_DEF_REAL_POS(		start_sleep_time,		1,0		)
		PARAM_DEF_BOOL_OFF(		master					)
		PARAM_DEF_SYMBO_NEG(	host_id_set_master,		0,-1,		-1,255,	str_off )
		PARAM_DEF_BOOL_OFF(		time_slave				)
		PARAM_DEF_INT32(		host_field_0,			1,0,		0,255 )
		PARAM_DEF_INT32(		host_field_1,			1,0,		0,255 )
		PARAM_DEF_INT32(		host_field_2,			1,0,		0,255 )
		PARAM_DEF_INT32(		host_field_3,			1,0,		0,255 )
		PARAM_DEF_0_3(			host_ip,				PARAM_DEF_STR_LOCKED	)
		PARAM_DEF_BOOL_ON(		use_ip_as_host_id		)
		PARAM_DEF_INT32(		host_id,				1,2,		1,255 )
		PARAM_DEF_STR(			mess_hello				)
		PARAM_DEF_STR(			mess_bye				)
		PARAM_DEF_BOOL_OFF(		save_trig				)

		PARAM_DEF_GROUP_CLOSED(	Lan Host, LAN_HOPARAM_DEF_NB )
			PARAM_DEF_STR_LOCKED(	langroup						)
			PARAM_DEF_STR_LOCKED(	machine							)
			PARAM_DEF_REAL_POS(		service_retry_time,				0,80	)
			PARAM_DEF_REAL_POS(		service_sleep_time,				0,1	)

		PARAM_DEF_GROUP_CLOSED(	Remote, REMOTE_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		remote							)
			PARAM_DEF_BOOL_ON(		remote_receive					)
			PARAM_DEF_INT32(		remote_link,					1,0,					0,c_net::LINK_NB-1 )
			PARAM_DEF_BOOL_OFF(		broadcast_keyboard				)
			PARAM_DEF_BOOL_OFF(		broadcast_mouse					)
			PARAM_DEF_BOOL_OFF(		broadcast_window				)

		PARAM_DEF_GROUP_CLOSED(	Channel, CHANNEL_PARAM_NB )
			PARAM_DEF_INT32(		channel_receive_begin,			2,1,					1,c_net::CHANNEL_NB	)
			PARAM_DEF_INT32(		channel_receive_end,			2,c_net::CHANNEL_NB,	1,c_net::CHANNEL_NB	)
			
		PARAM_DEF_GROUP_CLOSED(	Frustrum, FRUSTRUM_PARAM_NB )
			PARAM_DEF_REAL_ZERO(	frustum_offset_x				)
			PARAM_DEF_REAL_ZERO(	frustum_offset_y				)

		PARAM_DEF_GROUP_CLOSED(	Custom, CUSTOM_PARAM_NB )
			PARAM_DEF_BOOL_ON(		MaxMsp_block_with_leading_zero	)

		PARAM_DEF_GROUP_CLOSED(	TEK, TEK_PARAM_NB )
			PARAM_DEF_INT32(			blk_size_max,					32, BLK_SIZE_MAX_DEF,	256,c_net::BLK_SIZE_MAX )
			PARAM_DEF_INT32(			nb_blk_max,						100, 1000,				10,100000 )
			PARAM_DEF_BOOL_OFF(			loop_reception_channel_non_0	)
			PARAM_DEF_BOOL_OFF(			blk_received_always_invalid		)
			PARAM_DEF_INT32_LOCKED(		blk_nb							)
			PARAM_DEF_INT32_LOCKED(		blk_nb_allocated				)
			PARAM_DEF_INT32_LOCKED(		blk_nb_processed				)
			PARAM_DEF_INT32_SAVE_NOT(	blk_nb_received,				0,0,	0,0 )
			PARAM_DEF_INT32_SAVE_NOT(	blk_nb_received_self,			0,0,	0,0 )
			PARAM_DEF_INT32_SAVE_NOT(	blk_nb_recycled_by_forced,		0,0,	0,0 )
			PARAM_DEF_INT32_SAVE_NOT(	blk_nb_sent,					0,0,	0,0 )
			PARAM_DEF_INT32_SAVE_NOT(	blk_nb_out_error,				0,0,	0,0 )
			PARAM_DEF_BOOL_OFF(			blk_test_lost					)
			PARAM_DEF_BOOL_OFF(			blk_test_lost_verbose			)
			PARAM_DEF_INT32_SAVE_NOT(	blk_test_lost_nb,				0,0,	0,0 )
			PARAM_DEF_INT32_SAVE_NOT(	blk_test_lost_received,			0,0,	0,0 )
			PARAM_DEF_INT32_SAVE_NOT(	img_sent_nb,					0,0,	0,0 )
			PARAM_DEF_INT32_SAVE_NOT(	img_received_nb,				0,0,	0,0 )
			PARAM_DEF_INT32_SAVE_NOT(	img_err_nb,						0,0,	0,0 )

		PARAM_DEF_BOOL_OFF(	link_base_0 )
		PARAM_DEF_16(		link,	PARAM_DEF_BOOL_OFF )
	};

	PARAM_DEF_MAKE_INDEX( link_base_0				);
}

void	c_net::param_init_pt_static()
{
	INT32	h = 0;
	param_set_pt( h, _b_active_net_ui			);
	param_set_pt( h, c_sock::b_verbose			);
	param_set_pt( h, _b_verbose_in				);
	param_set_pt( h, _b_verbose_out				);

	++h;
		param_set_pt( h, b_verbose_osc_received_ui		);
		param_set_pt( h, b_verbose_osc_processed_ui		);
		param_set_pt( h, b_verbose_osc_out_ui			);
		param_set_pt( h, b_verbose_osc_detailed_ui		);
		param_set_pt( h, _b_osc_lua_accept_ui			);
		param_set_pt( h, _b_osc_lua_verbose_ui			);
		param_set_pt( h, _b_osc_midi_control_accept_ui	);
		param_set_pt( h, _b_osc_midi_control_verbose_ui	);
		param_set_pt( h, _b_osc_variable_set_accept_ui	);
		param_set_pt( h, _b_osc_variable_set_verbose_ui	);
		param_set_pt( h, _osc_packet_size				);
		param_set_pt( h, _osc_keep_time					);
		param_set_pt( h, _blk_osc_nb					);

	param_set_pt( h, _start_sleep_time			);
	param_set_pt( h, _b_master					);
	param_set_pt( h, _host_id_set_master		);
	param_set_pt( h, _b_time_slave				);

	param_set_pt_4( h, _host_field				);
	param_set_pt( h, _host_ip[0]				);
	param_set_pt( h, _host_ip[1]				);
	param_set_pt( h, _host_ip[2]				);
	param_set_pt( h, _host_ip[3]				);
	param_set_pt( h, _b_use_ip_as_host_id		);
	param_set_pt( h, _host_id					);
	param_set_pt( h, _str_hello					);
	param_set_pt( h, _str_bye					);
	param_set_pt( h, _b_save_trig_ui			);

	++h;
		param_set_pt( h, _langroup				);
		param_set_pt( h, _machine				);
		param_set_pt( h, _retry_time			);
		param_set_pt( h, _sleep_time			);

	++h;
		param_set_pt( h, _b_remote_send				);
		param_set_pt( h, _b_remote_receive			);
		param_set_pt( h, _s_remote_link_dst			);
		param_set_pt( h, _b_broadcast_keyboard_ui	);
		param_set_pt( h, _b_broadcast_mouse_ui		);
		param_set_pt( h, _b_broadcast_window_ui		);

	++h;
		param_set_pt( h, _channel_receive_begin	);
		param_set_pt( h, _channel_receive_end	);

	++h;
		param_set_pt( h, _frustum_offset_x		);
		param_set_pt( h, _frustum_offset_y		);

	++h;
		param_set_pt( h, _b_max_blk_with_leading_zero );

	++h;
		param_set_pt( h, get_blk_size_max_pt()		);
		param_set_pt( h, _blk_allocated_max			);
		param_set_pt( h, _b_loop_reception_channel_non_0	);
		param_set_pt( h, b_blk_received_always_invalid		);
		param_set_pt( h, _blk_nb					);
		param_set_pt( h, _blk_allocated_nb			);
		param_set_pt( h, _blk_processed_nb			);
		param_set_pt( h, blk_received_nb			);
		param_set_pt( h, blk_received_self_nb		);
		param_set_pt( h, blk_recycled_by_force_nb	);
		param_set_pt( h, blk_sent_nb				);
		param_set_pt( h, blk_out_error_nb			);
		param_set_pt( h, b_test_blk_lost			);
		param_set_pt( h, b_test_blk_lost_verbose	);
		param_set_pt( h, test_blk_lost_nb			);
		param_set_pt( h, test_blk_lost_receive_nb	);
		param_set_pt( h, img_sent_nb				);
		param_set_pt( h, img_received_nb			);	
		param_set_pt( h, img_err_nb					);	

	h += LINK_NB;

	err_param_init_pt( h );
}

void	c_net::param_init_pt()
{
	INT32 h = n_net::PARAM_INDEX_link_base_0;
	for( INT32 i = 0; i < LINK_NB; ++i )
	{
		c_net_link* CONST p_link = _link[i];
		param_attach_obj_no_inc( h, p_link );
		param_set_pt( h, p_link->get_pt_active() );		
	}
}

void	c_net::set_verbose_in( bool in)
{
	_b_verbose_in = in;
	SWITCH_PRINT_STATE("Net In Verbose", _b_verbose_in );
}
void	c_net::flip_verbose_in()
{
	set_verbose_in( !_b_verbose_in );
}

void	c_net::set_verbose_out( bool in)
{
	_b_verbose_out = in;
	SWITCH_PRINT_STATE("Net Out Verbose", _b_verbose_out );
}
void	c_net::flip_verbose_out()
{
	set_verbose_out( !_b_verbose_out );
}

void	c_net::flip_remote_send()
{
	set_remote_send( !_b_remote_send );
}

void	c_net::set_remote_send( bool in )
{
	_b_remote_send = in;
	SWITCH_PRINT_STATE( "Net remote Send", _b_remote_send );
}

void	c_net::flip_remote_receive()
{
	set_remote_receive( !_b_remote_receive );
}

void	c_net::set_remote_receive( bool in )
{
	_b_remote_receive = in;
	SWITCH_PRINT_STATE( "Net remote Receive", _b_remote_receive );
}


CONSTRUCTOR_CREATE(c_net)
,_blk_allocated_nb(0)
,_blk_allocated_max(0)
,_blk_processed_nb(0)
,_blk_nb(0)
,_blk_osc_nb(0)
,_blk_get_failed_nb(0)
,_osc_packet_size(0)
{
	_pt_trax_buf_out = _trax_buf_out;
	_pt_midi_buf_out = _midi_buf_out;
	for( INT32 i = 0; i < osc::DST_NB; ++i )
	{
		_b_osc_flush_sync[i]	= true;
		_osc_buf_index[i]		= 0;
		_pt_osc_buf_out[i]		= _osc_buf_out[i][0];
		_osc_buf_out_len[i][0]	= 0;
	}

	for( INT32 i = 0; i < LINK_NB; ++i )
	{
		_link[i] = nullptr;
		obj_get( _link[i] );
		_link[i]->set_index( i );
	}

	param_init_with( n_net::param, n_net::PARAM_NB_MAX );	//  net_param, NET_PARAM_NB_MAX );

	init();
}

c_net::~c_net()
{
	if( is_active() )
	{
		sendto_link0( 0, BLK_BYE, (UINT8*)_str_bye.get(), _str_bye.get_len()+1 );
		c_sock::stop();
	}

	for( size_t i = 0; i < LINK_NB; ++i )
		obj_delete( _link[i] );

	{
		std::lock_guard<aaa::MUTEX> guard(_ta_blk_free);
		for( auto const & pt : _blk_free )
			delete pt;
		_blk_free.clear();
	}

	{
		std::lock_guard<aaa::MUTEX> guard(_ta_blk_osc);
		for( auto const & pt : _blk_osc )
			delete pt;
		_blk_osc.clear();
	}

	{
		std::lock_guard<aaa::MUTEX> guard(_ta_blk_list);
		for( auto const & pt : _blk_list )
			delete pt;
		_blk_list.clear();
		for( auto const & pt : _blk_list_bis )
			delete pt;
		_blk_list_bis.clear();
	}
}

void	c_net::stop()
{
	set_active( false );
	for( INT32 i = LINK_NB-1; i >= 0; --i )
		_link[i]->stop();
}

void	c_net::start()
{
	if( NOERR(c_sock::start()) )
	{
		for( INT32 i = 0; i < LINK_NB; ++i )
		{
			if( NOERR( _link[i]->start() ) )
			{
				if( i == 0 )
					sendto_link0( 0, BLK_HELLO, (UINT8*)_str_hello.get(), _str_hello.get_len()+1 );
			}
		}
	}
	set_active( true );
}


void	c_net::init()
{
	_p_blk_list = &_blk_list;
	_p_blk_list_bis = &_blk_list_bis;
//	b_blk_list_lock = false;

	set_active( false );
	_b_master = false;
	_b_time_slave = false;
	_b_net_time_received = false;
//	_index_to_loop = -1;

	osc_out_init();
	_blk_osc.clear();

//	_langroup.erase();
//	_machine.erase();
//	get_host();

}

void	c_net::get_host()
{
	if( _langroup.is_empty() )
	{
		//		test();
		WKSTA_INFO_100*	buf = nullptr;
		bool			b_first_pass = true;

		//get network info
#ifdef	WIN98
		_machine.set( "JUAN" );
		_langroup.set( "NO" );
#else
		NET_API_STATUS nStatus;
		for( REAL r = _retry_time; r >= 0.; r -= _sleep_time )
		{
			// Free the allocated memory.
			if( buf )
			{
				NetApiBufferFree( buf );
				buf = nullptr;
			}

			nStatus = NetWkstaGetInfo( nullptr, 100, (UINT8**)&buf );
			if( nStatus == NERR_Success && buf )
			{
				CHAR	str[256];
				WideCharToMultiByte( CP_ACP, 0, (LPCWSTR)buf->wki100_computername, -1, str, 256, nullptr, nullptr );
				_machine.set( str );
				WideCharToMultiByte( CP_ACP, 0, (LPCWSTR)buf->wki100_langroup, -1, str, 256, nullptr, nullptr );	
				_langroup.set( str );
				// Free the allocated memory.
				if( buf )
				{
					NetApiBufferFree( buf );
					buf = nullptr;
				}
				break;
			}
			else
			{
				if( b_first_pass )
				{
					b_first_pass = false;
					WARNING_PRINT_STRING( "NetWkstaGetInfo() don't return infos : error status %d .", nStatus );
					// --- nStatus return code
					// 2102  | The redirector NETWKSTA.200 has not been started.
					// 2138  | The Requester service has not been started.
					// 2141  | The server is not configured for transactions.
					if( nStatus == 2138 )
					{
						WARNING_PRINT_STRING( "Check that the service \"workstation\" or \"station de travail\" is active." );
					}
					WARNING_PRINT_STRING( "AAASeed will retry during %.1f seconds.", _retry_time );
				}
				else
					PRINT_STRING( "." );
				// don't sleep in the last iteration
				if( r >= _sleep_time )
					spy::sleep( INT32(_sleep_time * 1000), "sleep c_net::get_host()" );
			}
		}
		if( !b_first_pass )
			PRINT_STRING( "\n" );
#endif
	}
}

void	c_net::update()
{
	if( _b_save_trig_ui )
	{
		_b_save_trig_ui = false;
		c_dir::push_def( _o_dir );
			save_to_file( get_my_filename() );
		c_dir::pop_def();
	}

	//	start or stop if main change
	if( _b_active_net_ui )
	{
		if( !is_active() )
			start();
	}
	else
	{
		if( is_active() )
			stop();
	}

	if( !is_active() )
		return;

	//todo refine deal with it by link
	if( _blk_get_failed_nb )
	{
		NET_PRINT_STRING( "in reception could not get %d block, allocate more block, the received blocks where simply dropped", _blk_get_failed_nb );
		_blk_get_failed_nb = 0;
	}
	for( INT32 i = 0; i < LINK_NB; ++i )
		_link[i]->update();

	blk_process();

	_blk_nb		= (INT32)_p_blk_list->size();
	_blk_osc_nb	= (INT32)_blk_osc.size();
}

AAA_ERR	c_net::load_do_after( o_str CONST & filename_in )
{
	_o_dir.set( c_dir::get_cur() );	//	o_dir should perhaps vanish

	o_str& filename = o_str::push_name( filename_in );
		filename.drop_ext();
		filename.add( "_" );
		for( INT32 i = 0; i < LINK_NB; ++i )
		{
			filename.add( i );	
	//		c_namer::set_file_only( _link[i], filename);		
			_link[i]->load_from_file_add_ext( filename );
			filename.drop_at( (i<10) ? -1 : -2  );
		}
	o_str::pop_name();

	if( _start_sleep_time > .0f )
	{
		NET_PRINT_STRING( "Net sleeping for %.2f s", _start_sleep_time );
		spy::sleep( INT32(_start_sleep_time * REAL(1000)), "sleep c_net::load_do_after()" );
	}
	if( _b_use_ip_as_host_id )
	{
		_host_id = c_sock::get_ip_field_lowest();
		NET_PRINT_STRING( "host_id from ip : %d", _host_id );
	}
	for( INT32 i=0; i<=3; ++i )
		_host_field[i] = c_sock::get_ip_field(i);

	for( INT32 i=0; i<HOST_IP_NB; ++i )
		_host_ip[i].add( c_sock::get_ip(i) );

	if( _host_id_set_master >= 0 )
	{
		_b_master = (_host_id_set_master == _host_id );
		NET_PRINT_STRING( "host_id set_master -> %s", _b_master?"Master":"Slave" );
	}
	return AAA_OK;	//todo improve
}

AAA_ERR	c_net::save_do_before( o_str CONST & filename_in )
{
	o_str& filename = o_str::push_name( filename_in );
		filename.drop_ext();
		filename.add( "_" );
		for( INT32 i = 0; i < LINK_NB; ++i )
		{
			filename.add( i );
	//		c_namer::set_file_only( _link[i], filename);		
			_link[i]->save_to_file_add_ext( filename );
			filename.drop_at( (i<10) ? -1 : -2  );
		}
	o_str::pop_name();
		
	return AAA_OK;	//todo improve
}

//void	c_net::send_float( INT32 CONST channel_id, REAL CONST value )
//{
//}

void	c_net::send_ascii( INT32 CONST channel_id, C_PCHAR_C buf )
{
	sendto_link0( channel_id, BLK_ASCII, (UINT8*)buf, (INT32)strlen(buf)+1 );
}

void	c_net::send_text8( INT32 CONST link_index, C_PCHAR_C buf )
{
	sendto( link_index, 0, BLK_TEXT8, (UINT8*)buf, (INT32)strlen(buf)+1 );
}

void	c_net::send_raw( INT32 CONST link_index, C_PCHAR_C buf, INT32 CONST len )
{
	sendto( link_index, 0, BLK_RAW, (UINT8*)buf, (INT32)len );
}

//todo send bigger block
void	c_net::sendto( INT32 CONST link_index, INT32 CONST channel_id, BLK_TYPE CONST type, CONST UINT8* CONST buf, INT32 CONST len )
{
	if( is_active() && buf && len )
	{
		c_net_link* p_link = get_link( link_index );
		if( p_link )
			p_link->sendto( channel_id, type, buf, len );
		else
			ERR_PRINT_STRING( "%s() link_index %d is out of range or net_link is null", __FUNCTION__, link_index );
	}
}

//hack deal only with one _channel for now
void	c_net::trax_flush()
{
	if( !is_active() || _pt_trax_buf_out == _trax_buf_out )
		return;

	sendto_link0( 1, BLK_TRAX, (UINT8*)_trax_buf_out, INT32(_pt_trax_buf_out-_trax_buf_out) );
	_pt_trax_buf_out = _trax_buf_out;
}
void	c_net::midi_flush()
{
	if( !is_active() || _pt_midi_buf_out == _midi_buf_out )
		return;

	sendto( midi_dst, 1, b_midi_hack_send_as_maxmsp ? BLK_MAX_BY_OPCODE : BLK_MIDI, _midi_buf_out, INT32(_pt_midi_buf_out - _midi_buf_out) );
	_pt_midi_buf_out = _midi_buf_out;
}

void	c_net::flush()
{
	trax_flush();
	midi_flush();
	osc_flush();
}

//todo use a common fn for everybody
//	and use htonl everywhere
void	c_net::send_trax( INT32 CONST channel_id, INT32 CONST control_id, REAL CONST value )
{
	if( !is_active() )
		return;

	*((INT32*)_pt_trax_buf_out ) = channel_id;
	_pt_trax_buf_out += sizeof(INT32);
	*((INT32*)_pt_trax_buf_out ) = control_id;
	_pt_trax_buf_out += sizeof(INT32);
	*((REAL*)_pt_trax_buf_out ) = value;
	_pt_trax_buf_out += sizeof(REAL);

	//todo do better even generalize
	if( (_pt_trax_buf_out - _trax_buf_out ) > ( c_net::get_blk_obj_data_size_max() - 32 ) )
	{
		trax_flush();
	}
}
FINLINE	void	c_net::send_midi_uint32( UINT32 CONST data )
{
	//htonl
	*((INT32*)_pt_midi_buf_out ) = htonl( data );
	_pt_midi_buf_out += sizeof(UINT32);
}

void	c_net::send_midi( INT32 CONST type, INT32 CONST channel_id, INT32 CONST index, REAL CONST value, INT32 CONST when )
{
	if( !is_active() )
		return;

	send_midi_uint32( type );
	send_midi_uint32( channel_id );
	send_midi_uint32( index );
	send_midi_uint32( (INT32)(value*65536) );
	send_midi_uint32( when );

	//todo do better even generalize
	if( (_pt_midi_buf_out - _midi_buf_out ) > ( c_net::get_blk_obj_data_size_max() - 32 ) )
	{
		midi_flush();
	}
}

void	c_net::send_midi_control( INT32 CONST channel_id, INT32 CONST control_index, REAL CONST value, INT32 CONST when )
{
	send_midi( c_net_blk::MIDI_CODE_CONTROL, channel_id, control_index, value, when );
}

void	c_net::send_midi_velocity( INT32 CONST channel_id, INT32 CONST note_index, REAL CONST value, INT32 CONST when )
{
	send_midi( c_net_blk::MIDI_CODE_VELOCITY, channel_id, note_index, value, when );
}

void	c_net::send_event( c_event* ev )
{
	//todo	for the moment we don't process reception of these
	//		so better not to send it
	if( ev->is_to_send() )
	{
	//todo		this is dangerous		
	//todonow		check on len
		INT32 nb = INT32( ev->write_to_mem( net_buf_send ) - net_buf_send );
		if( nb > 0 )
			sendto_link0( 1, BLK_EVENT, net_buf_send, nb );
	}
}

void	c_net::blk_store( c_net_blk* blk_in )
{
	if( blk_in->is_type_osc() )
	{
		std::lock_guard<aaa::MUTEX> guard(_ta_blk_osc);
		_blk_osc.push_back( blk_in );
	}
	else
	{
		std::lock_guard<aaa::MUTEX> guard(_ta_blk_list);
		_p_blk_list->push_back( blk_in );
	}
}

//	
c_net_blk*	c_net::blk_take_oldest()
{
	c_net_blk*	blk_out = nullptr;
	std::lock_guard<aaa::MUTEX> guard(_ta_blk_list);

	if( !_p_blk_list->empty() )
	{
		blk_out = _p_blk_list->front() ;
		_p_blk_list->pop_front();
	}
	return	blk_out;
}

c_net_blk*	c_net::osc_blk_take()
{
	c_net_blk*	blk_out = nullptr;
	std::lock_guard<aaa::MUTEX> guard(_ta_blk_osc);

	if( !_blk_osc.empty() )
	{
		blk_out = _blk_osc.front() ;
		_blk_osc.pop_front();
	}
	return	blk_out;
}

c_net_blk*	c_net::osc_blk_take_by_start( C_PCHAR pat )
{
//	INT32 len = strlen( pat );
	c_net_blk*	blk_out = nullptr;
	std::lock_guard<aaa::MUTEX> guard(_ta_blk_osc);

	for( auto const & blk : _blk_osc )
	{
		if( !pat || str_start_is_equal_nocase( (CONST CHAR *) blk->_data_osc, pat ) )
		{
			blk_out = blk;
			_blk_osc.remove( blk );	//todo slow
			break;
		}
	}

	return	blk_out;
}

c_net_blk*	c_net::blk_take_by_type( BLK_TYPE type )
{
	if( type == BLK_OSC || type == BLK_BUNDLE_OSC )
		return	osc_blk_take();

	c_net_blk*	blk_out = nullptr;
	{
		std::lock_guard<aaa::MUTEX> guard(_ta_blk_list);

		for( auto const & blk : *_p_blk_list )
		{
			if( blk->is_type( type ) )
			{	
				blk_out = blk;
				_p_blk_list->remove( blk );	//todo slow
				break;
			}
		}
	}
	return	blk_out;
}

c_net_blk*	c_net::blk_take_by_type_channel( BLK_TYPE type, INT32 channel_in )
{
	c_net_blk*	blk_out = nullptr;
	{	//todo a list by channel ?
		std::lock_guard<aaa::MUTEX> guard(_ta_blk_list);	

		for( auto const & blk : *_p_blk_list )
		{
			if( blk->is_type( type ) && blk->get_channel() == channel_in )
			{	
				blk_out = blk;
				_p_blk_list->remove( blk );
				break;
			}
		}
	}
	return	blk_out;
}

c_net_blk*	c_net::blk_take_by_link( INT32 index )
{
	c_net_blk*	blk_out = nullptr;
	{
		std::lock_guard<aaa::MUTEX> guard(_ta_blk_list);
		for( auto const & blk : *_p_blk_list )
		{
			if( blk->get_link() == index )
			{	
				blk_out = blk;
				_p_blk_list->remove( blk );
				break;
			}
		}
	}
	return	blk_out;
}

namespace {
	bool	b_net_first_loop = true;	//mainly to be sure lua script have been run once before receiving something
}

void	c_net::blk_process()
{
	if( b_net_first_loop )
	{
		b_net_first_loop = false;
		return;
	}

	//	Deal with non OSC blk
	std::vector<c_net_blk*>	to_process;

	_ta_blk_list.lock();
		if( _p_blk_list->empty() )
		{
			_blk_processed_nb = 0;
			_ta_blk_list.unlock();
		}
		else
		{
			//	we want to lock as short as possible
			//		so we just dispatch the blks in two structures using is_processed_by_do_process()
			//		1/ to_process which will be processed once unlocked
			//		2/ a second container pointed by _p_blk_list_bis which is swaped with _p_blk_list
			//				the blks keeped there are perhaps never processed
							//todo check into this
			//todostl	find an stl algo which doit

			for( auto const & blk : *_p_blk_list )
			{
				if( blk->is_processed_by_do_process() )
					to_process.push_back( blk );
				else
					_p_blk_list_bis->push_back( blk );
			}
			SWAP( _p_blk_list, _p_blk_list_bis );

			_ta_blk_list.unlock();
			//	clear the old container ( less time critical now )
			_p_blk_list_bis->clear();

			//	now we do the rest
			//	and process the blocks which needed to
			_blk_processed_nb = (INT32)to_process.size();
			if( _blk_processed_nb != 0 )
			{
				for( auto const & blk : to_process )
				{
					blk->do_process( _b_verbose_in );
					blk_free( blk );
				}
			}
		}

	if( _b_osc_midi_control_accept_ui )
	{
		osc_taking = OSC_TAKING_MIDI;
		osc_take_and_process( "/aaa/midi/", false );
	}
	if( _b_osc_variable_set_accept_ui )
	{
		osc_taking = OSC_TAKING_VARIABLE_SET;
		osc_take_and_process( "/aaa/variable/set", false );
	}
	//	Deal with OSC blk
	//	process lua blk
	if( _b_osc_lua_accept_ui )
	{
		osc_taking = OSC_TAKING_LUA;
		osc_take_and_process( "/aaa/lua", false );
	}

	osc_taking = OSC_TAKING_NONE;
	

	//remove old unused osc blk
	to_process.clear();
	{
		std::lock_guard<aaa::MUTEX> guard(_ta_blk_osc);
		REAL now = REAL(aaa::time::get_no_origin());
		iter_pblk it_end = _blk_osc.end();
		while( !_blk_osc.empty() )
		{
			c_net_blk* blk = _blk_osc.front();
			if( now - blk->get_time_received() > _osc_keep_time )
			{
				_blk_osc.pop_front();
				to_process.push_back( blk );
			}
			else
			{
				break;
			}
		}
	}

	for( auto const & blk : to_process )
	{
/*
		CONST char*	addr =  msg.AddressPattern();
		if( *addr != '/' )
		{
			ERR_PRINT_STRING( "%s() osc address don't start with / : %.64s", __FUNCTION__, addr );
			return;
		}

		ERR_PRINT_STRING( "%s() OSC address not implemented : /%.64s", __FUNCTION__, addr );
*/
		//todo	add something to print these blks
		blk_free( blk );
	}

}
