
#include "obj_ui/com/net_link.h"
#include "obj_ui/com/net_blk.h"
#include "time/aaa_time.h"
#include "checksum.h"
#include "infrastructure/param/param_declare.h"
#include "thread/scheduler.h"
#include "Thread/aaa_thread.h"
#include "obj_ui/tracker/trackers.h"
#include "spy.h"
#include <infrastructure/watchdog.h>

//todo use this to stop and start correctly threads
//class c_thread_net_link : public c_thread
//{
//public:
//	virtual	void	run()	{	run_it< c_net_link, -1 > ();	}
//	c_thread_net_link() : c_thread( "net_link_loop" )	{}
//};


FACTORY_CREATE_V1( c_net_link, net_link, Net Link, net_link );

//todo get rid of this
static	c_net_link*	p_net_link_base = nullptr;


void	c_net_link::print_string( C_PCHAR_C fmt, ... ) CONST
{
	va_list args;
	va_start( args, fmt );
	trackers::PRINT_STRING_VA( _mess_header.get(), fmt, args );
	va_end(args);
}
void	c_net_link::err_print( C_PCHAR_C fmt, ... ) CONST
{
	va_list args;
	va_start( args, fmt );
	ERR_HEADER_PRINT_STRING_VA( _err_header.get(), fmt, args );
	va_end(args);
}

void	c_net_link::do_received( UINT8 CONST * CONST buf, INT32 len )
{
	if( is_obj_exist_and_active(net) )	// avoid crashes at close
	{
	//	_do_received_lock->lock();
		++c_net::blk_received_nb;
		++_blk_received_nb;
		if( len <= 0 )
		{
			err_print( "received block with negative or null len_, Skipping it." );
			return;
		}
		net->blk_receive( _index, buf, len );
		//_do_received_lock->unlock();
	}
}


#define HIVE_TTL_RESTRICT_TO_HOST         0
#define HIVE_TTL_RESTRICT_TO_SUBNET       1
#define HIVE_TTL_RESTRICT_TO_SITE        31
#define HIVE_TTL_RESTRICT_TO_REGION      63
#define HIVE_TTL_RESTRICT_TO_CONTINENT  127
#define HIVE_TTL_RESTRICT_TO_GLOBAL     254


static	C_PCHAR_C	str_same_port[1]	=	{	"Use main port"	};
static	C_PCHAR_C	str_no[1]			=	{	"NO"			};
static	C_PCHAR_C	str_off[1]			=	{	"Off"			};

namespace	n_net_link
{
	CONSTEXPR INT32 PARAM_BASE_NB	= 21;
	CONSTEXPR INT32 PARAM_TEK_NB	= 9;
	CONSTEXPR INT32 PARAM_GROUP_NB	= 1;
	CONSTEXPR INT32 PARAM_NB_MAX	=	PARAM_BASE_NB
									+	PARAM_TEK_NB
									+	PARAM_GROUP_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_OFF(			active						)
		PARAM_DEF_BOOL_OFF(			enable						)
		PARAM_DEF_BOOL_OFF(			watchdog_enable				)
		PARAM_DEF_BOOL_OFF(			start_trig					)
		PARAM_DEF_BOOL_OFF(			stop_trig					)
		PARAM_DEF_BOOL_LOCKED(		running						)
		PARAM_DEF_INT32_LOCKED(		socket_id					)

		PARAM_DEF_BOOL_OFF(			verbose						)
		PARAM_DEF_BOOL_OFF(			udp_send_only				)
		PARAM_DEF_BOOL_OFF(			tcp							)
		PARAM_DEF_BOOL_ON(			tcp_no_marker				)
		PARAM_DEF_BOOL_OFF(			call						)
		PARAM_DEF_SYMBO_ZERO(		port_nb,					0, 1600, 0, PARAM_MAX_UINT32, str_same_port )
		PARAM_DEF_REF(				dst_ip						)
		PARAM_DEF_SYMBO_ZERO(		port_nb_udp_dst,			16000, 0, 0, PARAM_MAX_UINT32, str_no )
		PARAM_DEF_INT32(			broadcast_time_to_leave,	HIVE_TTL_RESTRICT_TO_SITE, HIVE_TTL_RESTRICT_TO_SUBNET,	0, HIVE_TTL_RESTRICT_TO_GLOBAL )

		PARAM_DEF_BOOL_OFF(			blk_checksum_skip			)
		PARAM_DEF_BOOL_OFF(			blk_always_valid			)
		PARAM_DEF_BOOL_ON(			disable_when_connection_failed	)

		PARAM_DEF_BOOL_OFF(			do_receive_directly_when_sent	)
		PARAM_DEF_BOOL_OFF(			discard_osc_blk					)

		PARAM_DEF_GROUP_CLOSED(	TEK, PARAM_TEK_NB )
			PARAM_DEF_INT32_SAVE_NOT(	blk_received_nb,		0,0, 0, 0 )
			PARAM_DEF_INT32_SAVE_NOT(	blk_received_self_nb,	0,0, 0, 0 )
			PARAM_DEF_INT32_SAVE_NOT(	blk_sent_nb,			0,0, 0, 0 )
			PARAM_DEF_INT32_SAVE_NOT(	blk_out_error_nb,		0,0, 0, 0 )

			PARAM_DEF_INT32_SAVE_NOT(	blk_test_lost_received,	0,0,	0, 0 )
			PARAM_DEF_INT32_SAVE_NOT(	blk_test_lost_err_nb,	0,0,	0, 0 )

			PARAM_DEF_INT32_SAVE_NOT(	img_sent_nb,			0,0,	0, 0 )
			PARAM_DEF_INT32_SAVE_NOT(	img_received_nb,		0,0,	0, 0 )
			PARAM_DEF_INT32_SAVE_NOT(	img_err_nb,				0,0,	0, 0 )
	};
}

void	c_net_link::param_init_pt_static()
{
	INT32	h=0;
	param_set_pt( h, get_pt_active()					);
	param_set_pt( h, _b_enable_ui						);
	param_set_pt( h, _b_watchdog_enable					);
	param_set_pt( h, _b_start_trig						);
	param_set_pt( h, _b_stop_trig						);
	param_set_pt( h, _b_running							);
	param_set_pt( h, _socket_id							);

	param_set_pt( h, _b_verbose							);
	param_set_pt( h, _b_udp_send_only					);
	param_set_pt( h, _b_tcp								);
	param_set_pt( h, _b_tcp_no_marker					);
	param_set_pt( h, _b_call							);
	param_set_pt( h, _port_nb							);
	param_set_pt( h, _ip_str_ui							);
	param_set_pt( h, _port_nb_udp_dst					);
	param_set_pt( h, _broadcast_time_to_leave			);

	param_set_pt( h, _b_blk_checksum_skip				);
	param_set_pt( h, _b_blk_always_valid				);
	param_set_pt( h, _b_disable_when_connection_failed	);

	param_set_pt( h, _b_receive_sent_blk				);
	param_set_pt( h, _b_blk_osc_discard					);

	++h;
		param_set_pt( h, _blk_received_nb			);
		param_set_pt( h, _blk_received_self_nb		);
		param_set_pt( h, _blk_sent_nb				);
		param_set_pt( h, _blk_out_error_nb			);

		param_set_pt( h, _blk_test_lost_received_nb	);
		param_set_pt( h, _blk_test_lost_err_nb		);

		param_set_pt( h, _img_sent_nb				);
		param_set_pt( h, _img_received_nb			);
		param_set_pt( h, _img_err_nb				);		

	err_param_init_pt( h );
}

void	c_net_link::param_init_pt()
{

}

o_str*	c_net_link::get_comment()
{
	_o_comment.erase();
	if( !is_active() )
		_o_comment.add( "off" );
	else
	{
		if( !_b_enable_ui )
			_o_comment.add( " Disabled" );
		else
		{
			if( !_b_running )
				_o_comment.add( " Stopped" );
		}
	}

	_o_comment.add( _b_tcp ?  " Tcp" : " UDP" );
	if( !_ip_str_ui.is_empty() )
	{
		_o_comment.add_space();
		_o_comment.add( _ip_str_ui );
	}
	if( _port_nb )
	{
		_o_comment.add( ":" );
		_o_comment.add( _port_nb );
	}
	if( _blk_received_nb || _blk_sent_nb )
	{
		_o_comment.add( " " );
		_o_comment.add( _blk_received_nb );
		_o_comment.add( " | " );
		_o_comment.add( _blk_sent_nb );
	}
	return &_o_comment;
} 

void	c_net_link::set_index( INT32 index )
{
	_index = index;
	if( index == 0 )
		p_net_link_base = this;	//todonow get rid of this
	_mess_header.set( "# NET(");
	_mess_header.add( _index );
	_mess_header.add( ") ");

	_err_header.set( ERR_HEADER );
	_err_header.add( "NET(");
	_err_header.add( _index );
	_err_header.add( ") ");
}

void	c_net_link::init()
{
	_b_sock_owner = true;
	_psoc = nullptr;
	_b_sock_owner_prev = false;
	_psoc_prev = nullptr;
	_ip_addr = 0;
//	_ip_str_ui.erase();
//	if( !_do_received_lock )
//		_do_received_lock = new aaa::MUTEX;
}

CONSTRUCTOR_CREATE(c_net_link)
,_b_running					(false)
,_blk_received_nb			(0)
,_blk_received_self_nb		(0)
,_blk_sent_nb				(0)
,_blk_out_error_nb			(0)
,_blk_send_index			{0}
,_blk_test_lost_err_nb		{0}
,_blk_test_lost_received_nb	{0}
,_img_sent_nb				{0}
,_img_received_nb			{0}
,_img_err_nb				{0}
//,_do_received_lock(nullptr)
{
	param_init_with( n_net_link::param, n_net_link::PARAM_NB_MAX );

	init();
}

c_net_link::~c_net_link()
{
	stop();	//todo we have to get rid of what left over _psoc _psoc_prev
}

void	c_net_link::stop()
{
	if( _psoc )
	{
		_psoc_prev = _psoc;
		_b_sock_owner_prev = _b_sock_owner;

		_psoc = nullptr;
		_b_running = false;
		_b_sock_owner = true;

		_psoc_prev->ask_close();	//hack this still a hack
		//spy::sleep( 100, "sleep c_net_link::stop()" );
	}
//	SAFE_DELETE( _do_received_lock );
}

//hack
//	Added to ease the process of restarting multiple times
//		avoid ask to stop, call update(), ask to start, call update()
//		also when this was done multiple times it could lead to an error in socket : WSAENOTSOCK
void	c_net_link::restart()
{
	stop();
	_b_start_trig = true;
	update();
}

AAA_ERR	c_net_link::start()
{
	AAA_ERR	ret = ERR_ANY;
	//todo start and stop dynamic
	//	careful : receive run at a high priority and can lock the machine

	if( !is_active() )
		return ret;

	if( c_watchdog::is_on() && !_b_watchdog_enable )
	{
		DBG_PRINT_STRING( "This link is disabled in watchdog mode." );
		if( _b_running )	//in case if was running when _b_watchdog_enable was set to false
			stop();
		return ret;
	}

	if( _b_running )
		ret = AAA_OK;
	else if( _b_enable_ui )
	{
		//todo this should be with c_net
		//build ip address from asked one, if we have a simple number we use the 3 first field from the host for the first fields
		CHAR CONST * ip_str = _ip_str_ui.get();
		if( strlen(ip_str)<=3 && str_is_digit_only(ip_str) )
		{
			_ip_str.erase();
			for( INT32 i=0; i<3; ++i )
			{
				_ip_str.add( net->_host_field[i] );
				_ip_str.add_char( '.' );
			}
			_ip_str.add( ip_str );
		}
		else
			_ip_str.set( _ip_str_ui );
		//todo end

		_ip_addr = c_sock::get_ip_addr_from_str( _ip_str.get() );
		if( _ip_addr == INADDR_ANY )
		{
			ret = ERR_DATA_INCOHERENT;
		}
		else
		{
			ret = ERR_ANY;
			if( _port_nb == 0 )
			{
				_b_sock_owner = false;
				if( p_net_link_base )
				{	
					_psoc = p_net_link_base->_psoc;
					print_string( "Socket reused" );
					ret = AAA_OK;
				}
				else
				{
					_psoc = nullptr;
					err_print( "%s() can't reuse NULL base channel", __FUNCTION__ );
				}
			}
			else
			{
				_b_sock_owner = true;
				_psoc = new c_sock;
				if( _b_tcp )
				{
					if( _b_call )
					{
						print_string( "TCP try to call on port %d", _port_nb );
						ret = _psoc->call( _ip_str.get(), _port_nb );
						if( NOERR( ret ) )
						{
							print_string( "TCP call succeeded on port %d", _port_nb );
							start_loop_receive_tcp( _psoc );
						}
					}
					else
					{
						print_string( "TCP try to establish socket on port %d", _port_nb );
						ret = _psoc->establish( _port_nb );
						if( NOERR( ret ) )
						{
							print_string( "TCP socket established on port %d", _port_nb );
							start_loop_listen();
						}
					}

				}
				else
				{
					print_string( "UDP try to create socket on port %d", _port_nb );
					ret = _psoc->broadcast_create( _port_nb, _port_nb_udp_dst, _broadcast_time_to_leave );
					if( NOERR( ret ) )
					{
						if( !_b_udp_send_only )
						{
							print_string( "UDP socket created on port %d", _port_nb );
							start_loop_receive_broadcast();
						}
					}
				}
				if( ERR(ret) )
					SAFE_DELETE( _psoc );
			}
			_b_running = NOERR(ret);
		}
		if( ERR(ret) )
		{
			err_print( "Can't start." );
			// to avoid lockout
			if( _b_disable_when_connection_failed )
			{
				err_print( "So link is now disabled (param disable_when_connection_failed control this)." );
				_b_enable_ui = false;	//todonow clean up the active/enable.. biz
			}
		}
	}
	return ret;
}

void	c_net_link::update()
{
	if( _b_start_trig )
	{
		_b_start_trig = false;
		start();
	}
	if( _b_stop_trig )
	{
		_b_stop_trig = false;
		stop();
	}
	if( _psoc_prev && !_psoc_prev->is_valid() && _b_sock_owner_prev )
		obj_delete( _psoc_prev );

	_socket_id = _psoc ? _psoc->_s : 0;
}

void	c_net_link::send_raw_no_check( UINT8 CONST * CONST buf, INT32 CONST len )
{	
	if( net->_b_verbose_out )
		print_string( "OUT send block (len %d)", len );

	if( ERR(_psoc->sendto( _ip_addr, buf, len ) ) )
	{
		err_print( "%s() block not send.", __FUNCTION__ );
		++c_net::blk_out_error_nb;
		++_blk_out_error_nb;
	}
	else
	{
		++c_net::blk_sent_nb;
		++_blk_sent_nb;
	}

	if( _b_receive_sent_blk )
		do_received( buf, len );
}

//todo send bigger block
void	c_net_link::send_raw( UINT8 CONST * CONST buf, INT32 CONST len )
{
	if( !is_active() || !buf || !len )
		return;
	if( _psoc && _b_enable_ui )
	{
		{
			//todo this is a double check and that's too much
			INT32	tmp = MIN( ABS(len), c_net::get_blk_size_max() );
			if( tmp != len )
			{
				err_print( "%s() Block to send with data len %d bigger than max %d : Skipping.", __FUNCTION__, len, tmp );
				return;		//could be dangerous to send partial data, use of block should protect from this on the other side 
			}
		}
		
		send_raw_no_check( buf, len );
	}
}

namespace{
	thread_local UINT8 net_link_buf_send[ c_net::BLK_SIZE_MAX+1 ];
}

//todo send bigger block
void	c_net_link::sendto( INT32 CONST channel_id, c_net::BLK_TYPE CONST type, UINT8 CONST * CONST buf, INT32 len )
{
	if( !is_active() || !buf || !len )
		return;

	if( !_psoc || !_b_enable_ui )
		return;

	UINT32 CONST	len_buf = len + 128;
	//beurk
	
	UINT32*			pt;
	INT32			tmp;

	tmp = MIN( ABS(len), INT32(len_buf-c_net::BLK_HEADER_SIZE) );	//hack this should be refined by case
	if( tmp != len )
		err_print( "%s() Huh?  Problem on len of block to send.", __FUNCTION__ );
	len = tmp;
		
	if( type == c_net::BLK_MAX_BY_OPCODE )
	{
		MEMCPY( (CHAR *)net_link_buf_send, (CHAR *)osc::bundle_header, osc::MSG_HEADER_SIZE, __FUNCTION__ );
		pt = (UINT32*) ( net_link_buf_send + osc::MSG_HEADER_SIZE );
		if( net->_b_max_blk_with_leading_zero )
		{
			*pt++ = htonl( len+4 );
			*pt++ = htonl( 0 );
			MEMCPY( (CHAR *)pt, buf, len, __FUNCTION__ );
			len += 8;
		}
		else
		{
			*pt++ = htonl( len );
			MEMCPY( (CHAR *)pt, buf, len, __FUNCTION__ );
			len += 4;
		}
		len += osc::MSG_HEADER_SIZE;
	}
	else if( type == c_net::BLK_OSC )
	{
		MEMCPY( (CHAR *)net_link_buf_send, (CHAR *)osc::bundle_header, osc::MSG_HEADER_SIZE, __FUNCTION__ );
		pt = (UINT32 *)( net_link_buf_send + osc::MSG_HEADER_SIZE);
		MEMCPY( (CHAR *)pt, buf, len, __FUNCTION__ );
		len += osc::MSG_HEADER_SIZE;
	}
	else if( type == c_net::BLK_TEXT8 )
	{
		*net_link_buf_send = c_net::BLK_TEXT8;
		pt = (UINT32 *)(net_link_buf_send+1);
		MEMCPY( (CHAR *)pt, buf, len, __FUNCTION__ );
		len += 1;
	}
	else if( type == c_net::BLK_RAW )
	{
		// send raw data
		MEMCPY( net_link_buf_send, buf, len, __FUNCTION__ );
	}
	else
	{
		pt = (UINT32 *)net_link_buf_send;

		*pt++ = htonl( channel_id );
		*pt++ = htonl( net->_host_id );
		*pt++ = htonl( ++_blk_send_index );
		//hack
		*(unsigned __int64 *)pt = htond( aaa::time::get_no_origin() );
		pt += 2;	// because of unsigned __int64
		*pt++ = htonl( type );
		*pt++ = htonl( len );
		UINT32*	CONST pt_check = pt;
		*pt++ = 0;	// clear for network checksum compute

		MEMCPY( (CHAR *)pt, buf, len, __FUNCTION__ );	//todonow avoid this

		len += c_net::BLK_HEADER_SIZE;

		if( !is_blk_checksum_skip() )
		{
			c_checksum	check;
			check.add_buf( net_link_buf_send, len );
			//	htonl so the checksum field on the wire is big-endian, matching
			//	every other field in the packet (channel_id, host_id, etc).
			//	Without this, an LE sender and BE receiver would disagree on
			//	the stored checksum value and always fail validation.
			*pt_check = htonl( check.get() );
		}
	}
	send_raw_no_check( net_link_buf_send, len );
//	FREE( buf_send );
}

AAA_ERR	c_net_link::load_do_after( o_str CONST & filename )
{
	return AAA_OK;
}

void	c_net_link::loop_receive_broadcast()
{
	print_string( "Looping on broadcast net reception." );

	set_process_priority_class( PRIO_CLASS_REALTIME );
	set_thread_priority( THREAD_PRIO_TIME_CRITICAL );

	c_sock* p_sock = _psoc;
	//p_sock->set_timeout_receive( 500 );

	UINT8	buf[ c_net::BLK_SIZE_MAX+1 ];
//	UINT8*	buf = (UINT8*)MALLOC( c_net::BLK_SIZE_MAX+1 );	//todobeurk //hack //todoq //todolv

	while( p_sock )	//	deal with _psoc deleted
	{
		if( !_b_running )
			break;
		if( is_active() )
		{
			INT32 len = p_sock->recv_broadcast( buf, c_net::BLK_SIZE_MAX );
			if( len == -1 )
			{	//the socket gratefully closed the connection
				_b_running = false;
				break;
			}
			if( !_b_running )
				break;
			if( len > 0 )
				do_received( buf, len );
		}
		else
			spy::sleep( 100, "sleep_loop_receive_broadcast" );	// make sure we don't lock the machine in a high priority endless loop
	}
	print_string( "Exit on broadcast net reception." ); 
	

	if( p_sock )
		p_sock->close();	//close strategy should be refine in particular in case of call
//	FREE( buf );

}

void
#ifdef	WIN32
__cdecl
#endif
th_loop_receive_broadcast( void *dummy )
{
	c_net_link* l = (c_net_link *) dummy;
	l->print_string( "Receive Broadcast thread before loop_receive_broadcast().");
	l->loop_receive_broadcast();
	l->print_string( "Receive Broadcast thread after loop_receive_broadcast()." );
	c_thread::end();
}

void	c_net_link::start_loop_receive_broadcast()
{
//	while( index_to_loop_ != -1 );
//	index_to_loop_ = index;

	print_string( "Receive Broadcast thread will begin." );
	c_thread::begin( "Net receive_broadcast", th_loop_receive_broadcast, 0, (void *)this );
//	print_string( "Receive Broadcast thread begin done." );
//	while( index_to_loop_ != -1 );
	print_string( "Receive Broadcast thread should have Started." );
	//	Sleep( 10);	//hack
}

#define	NET_HEADER_SIZE_MAX	128
void	c_net_link::loop_receive_tcp( c_sock* p_sock )
{
	UINT8*	buf = (UINT8*)MALLOC_ALIGNED( c_net::BLK_SIZE_MAX+1, 0 );	//todobeurk //hack //todoq //todolv

	CHAR	marker[ NET_HEADER_SIZE_MAX+1 ];
	CHAR*	pt_marker;
	pt_marker = marker;

	UINT8*	body = (UINT8*)MALLOC_ALIGNED( c_net::BLK_SIZE_MAX+1, 0 );	//todobeurk //hack //todoq //todolv
	UINT8*	pt_body = body;	//define a null body
	INT32	body_size = 0;

	INT32	state = 0;
	//	0 Wait for start of marker ('<')
	//	1 Wait for stop of marker ('>')
	//  2 Wait for body
	//	3 check start of end marker
	//  4 read body till end marker
	bool	wait_stop = false;

	print_string( "SOCKET %s PORT %d Looping on TCP net reception.", p_sock->get_socket(), p_sock->get_port_nb() );
	while( p_sock )
	{
		if( !_b_running )
			break;
		if( is_active() )
		{
			INT32 nb = p_sock->recv( buf, c_net::BLK_SIZE_MAX );	//todobeurk //hack //todoq //todolv
			if( nb==-1 )
			{	//the socket gratefully closed the connection
				break;
			}
			if( !_b_running )
				break;
			else if( nb > c_net::BLK_SIZE_MAX )
				print_string( "IN TCP got %d bytes.", nb );
			else if( nb > 0 )
			{
				*(buf+nb) = 0;	//in case
				if( !_b_tcp_no_marker )	//<PUB_begin0 size=20>
				{
					UINT8*	pt = buf;
					CHAR	ch;
					do	//loop till we process the buffer
					{
						ch = *pt;
						switch( state )
						{
						case 0:	//Wait for start of marker ('<')
							if( ch == '<' )
							{
								*pt_marker++ = ch;
								state = 1;
							}
							break;
						case 3:	//Check begin of end marker
							if( ch == '<' )
							{
								*pt_marker++ = ch;
								state = 1;
							}
							else	//there is a problem: an end marker should start here
							{
								//reset
								if( net->_b_verbose_in )
									print_string( "IN TCP. can't find end marker where expected." );
								state = 0;
								pt_marker = marker;
								wait_stop = false;
							}
							break;
						case 1:	//Wait for stop of marker ('>')
							*pt_marker++ = ch;
							if( ch == '>' )
							{
								state = 0;
								if( str_is_equal_nocase( marker + 1, "pub_end0>", 9 ) )
								{
									if( wait_stop )
									{
										if( pt_body != body )
										{
											*pt_body++ = 0;
											do_received( body, INT32(pt_body-body) );
										}
									}
									else
										print_string( "IN TCP. end marker not expected." );
									wait_stop = false;
								}
								else if( str_is_equal_nocase( marker + 1, "pub_begin0", 10 ) )
								{
									if( wait_stop )
									{
										print_string( "IN TCP. begin marker when end marker expected." );
										wait_stop = false;
									}

									pt_marker = marker+11;
									if( str_is_equal_nocase( pt_marker, " size=", 6 ) )
									{
										pt_marker += 6;
										body_size = atol( pt_marker );
										if( body_size )
										{
											if( body_size > 4000 )	//hack	use real size
											{	//todo realloc ?
												err_print( "IN TCP. body size more %d than the %d maximum.", body_size, c_net::BLK_SIZE_MAX );
											}
											else
											{
												state = 2;
												wait_stop = true;
												pt_body = body;
											}
										}
									}
									else if( *pt_marker == '>' )
									{
										state = 4;
										wait_stop = true;
										pt_body = body;
									}
								}
								pt_marker = marker;
							}
							else if ( ch == '<' )
							{
								pt_marker = marker;	//error message here ?
								*pt_marker++ = ch;
							}
							break;
						case 2:
							*pt_body++ = ch;
							if( --body_size == 0 )
							{
								state = 3;
							} 
							break;
						case 4:
							if( ch == '<' )
							{
								pt_marker = marker;	//error message here ?
								*pt_marker++ = ch;
								state = 1;
							}
							else
							{
								*pt_body++ = ch;
							}
							break;
						}
						++pt;
					}
					while( --nb);
				}
				else
				{
					//this for eyesweb
					//					if( *buf != 6 && *buf != 13 && *buf != 21)	//eliminate ACK and CR and Nak
					//todo	check if this eye thing below is still necessary
					//					if( *(buf+1)==0 && *(buf+2)==0 && *(buf+3)==0 ) //nb == 4 && 
					//						continue;

					do_received( buf, nb );
					if( net->_b_verbose_in && _b_verbose )
					{	//todo	refine display of incoming blk
						print_string( "TCP received %d bytes: %s", nb, buf );
					}
				}
			}
			else 
			{
				err_print( "%s() Pb: Exiting reception loop.", __FUNCTION__ );
				break;
			}
		}
		else
		{
			spy::sleep( 100, "sleep_loop_receive_tcp" );	// make sure we don't lock the machine in a high priority endless loop
		}
	}
	if( p_sock )
		p_sock->close();	//close strategy should be refine in particular in case of call

	print_string( "TCP Receive thread ended." );
	if( p_sock==_psoc )
	{
		stop();
		print_string( "This was a calling link so it will stop." );
	}
	FREE_ALIGNED( buf );
	FREE_ALIGNED( body );
}

namespace {
	volatile c_sock*	psoc_to_loop = nullptr;

	void
#ifdef	WIN32
	__cdecl
#endif
	th_loop_tcp( void* dummy )
	{
		c_sock*		ps = (c_sock*)psoc_to_loop;
		psoc_to_loop = nullptr;

		c_net_link*	l = (c_net_link *) dummy;
		l->loop_receive_tcp( ps );
		c_thread::end();
	}
}


void	c_net_link::start_loop_receive_tcp( c_sock* ps )
{
	while( psoc_to_loop );
	psoc_to_loop = ps;
	print_string( "TCP Receive thread begin." );
	c_thread::begin( "Net receive_tcp", th_loop_tcp, 0, (void *)this );
//	print_string( "Receive Tcp thread begin done." ;
//	while( index_to_loop_ != -1 );
	while( !psoc_to_loop );
	print_string( "TCP Receive thread Started." );
//	Sleep( 10);	//hack
}

void	c_net_link::loop_listen()
{
	c_sock*	p_received;

/*
	if( index_to_loop_ == -1 )
	{	//todo	better message at min
		NET_PRINT_STRING( "Bug can listen on -1 index" );
		return;
	}
	index = index_to_loop_;
	index_to_loop_ = -1;
*/
	c_sock* p_sock = _psoc;
	print_string( "Port %d TCP Looping on accepting connection.", p_sock->get_port_nb() );

	while( p_sock )	// treat the the case where _psoc is deleted (the link got stopped or deleted)
	{
		if( !_b_running )
			break;
		p_received = p_sock->accept();
		if( !_b_running )
			break;
		if( p_received )
		{
			print_string( "TCP connection just accepted." );
			start_loop_receive_tcp( p_received );
		}
		else
		{	// _psoc can be deleted (the link got stopped or deleted)
			if( p_sock )
				err_print( "%s() TCP Error accepting connection.", __FUNCTION__ );
		}
	}
}

namespace {
	void
#ifdef	WIN32
	__cdecl
#endif
	th_loop_net_listen( void* dummy )
	{
		c_net_link*	l = (c_net_link *) dummy;
		l->loop_listen();
		l->print_string( "TCP Listen thread ended." );
		c_thread::end();
	}
}

void	c_net_link::start_loop_listen()
{
//	while( index_to_loop_ != -1 );
//	index_to_loop_ = index;

	print_string( "TCP Listen thread begin." );
	c_thread::begin( "Net listen", th_loop_net_listen, 0, (void *)this );
//	while( index_to_loop_ != -1 );
//	Sleep( 10);	//hack
	print_string( "TCP Listen thread Started." );
}

