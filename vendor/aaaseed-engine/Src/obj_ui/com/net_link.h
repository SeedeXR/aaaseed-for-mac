
#ifdef AAA_NET_LINK_H
#error "NET_LINK_H included more than once."
#endif
#define AAA_NET_LINK_H 1


//because winsock2 have to be first
#ifndef AAA_SOCKET_H
#	include	"socket.h"
#endif
#ifndef	AAA_NET_H
#	include "net.h"
#endif
#ifndef AAA_AAA_STR_H
#	include "aaa_str.h"
#endif


class	c_net_link final : public c_obj_active_ui
{
	friend	class c_net;
	friend	class c_net_blk;
	friend	class c_img_2d;

	FACTORY_DECLARE( c_net_link, c_obj_active_ui );
	C_NO_CPY_MOVE( c_net_link )

protected:
	bool	_b_enable_ui;
	bool	_b_running;

	bool	_b_verbose;
	bool	_b_udp_send_only;
	bool	_b_tcp;
	bool	_b_tcp_no_marker;
	bool	_b_call;
	INT32	_port_nb;
	o_str	_ip_str;
	o_str	_ip_str_ui;
	INT32	_port_nb_udp_dst;
	INT32	_broadcast_time_to_leave;

	bool	_b_blk_checksum_skip;
	bool	_b_blk_always_valid;
	bool	_b_disable_when_connection_failed;
	bool	_b_blk_osc_discard;
	bool	_b_receive_sent_blk;
//	mutable aaa::MUTEX*	_do_received_lock;

	UINT32	_ip_addr;
	c_sock*	_psoc;
	SOCKET	_socket_id;	//todo should be 64 bits but param not implemented yet

	bool	_b_sock_owner;
	//use to ease the restart
	c_sock* _psoc_prev;
	bool	_b_sock_owner_prev;

	INT32	_index;
	o_str	_mess_header;
	o_str	_err_header;
	o_str	_o_comment;

	INT32	_blk_received_nb;
	INT32	_blk_received_self_nb;
	INT32	_blk_sent_nb;
	INT32	_blk_out_error_nb;

	bool	_b_watchdog_enable;
	bool	_b_start_trig;
	bool	_b_stop_trig;

	INT32	_blk_send_index;
	INT32	_blk_test_lost_err_nb;
	INT32	_blk_test_lost_received_nb;

	INT32	_img_sent_nb;
	INT32	_img_received_nb;
	INT32	_img_err_nb;

public:
//todo should be by channel ?
//	bool	b_loop_reception_;
private:
			//private to force call through c_net
			void	send_raw(													UINT8 CONST * CONST buf, INT32 CONST len );
			void	send_raw_no_check(											UINT8 CONST * CONST buf, INT32 CONST len );
			void	sendto( INT32 CONST channel_id, c_net::BLK_TYPE CONST type,	UINT8 CONST * CONST buf, INT32 CONST len );
public:
			void	init();
	virtual	void	param_init_pt_static();
	virtual	void	param_init_pt();

			void	set_index( INT32 index );
	FINLINE	INT32	get_index()				CONST			{ return _index;			}
//	virtual	void	update();
//	FINLINE			C_PCHAR_C		get_mess_header()		CONST NOEXCEPT	{	return _mess_header.get();	}
//	FINLINE			C_PCHAR_C		get_err_header()		CONST NOEXCEPT	{	return _err_header.get();	}

	virtual	AAA_ERR	load_do_after( o_str CONST & filename );

			void	set_enable( bool b )					{ _b_enable_ui = b;			}
			void	set_port_nb( INT32 port_nb )			{ _port_nb = port_nb;		}
			void	set_tcp( bool b )						{ _b_tcp = b;				}
			void	set_call( bool b )						{ _b_call = b;				}
			void	set_tcp_no_marker( bool b )				{ _b_tcp_no_marker = b;		}
			void	set_blk_always_valid( bool b )			{ _b_blk_always_valid = b;	}

	FINLINE	bool	is_running()			CONST 			{ return _b_running && is_active() && _b_enable_ui;	} 
	FINLINE	bool	is_blk_always_valid()	CONST			{ return _b_blk_always_valid;	} 
	FINLINE	bool	is_blk_checksum_skip()	CONST			{ return _b_blk_checksum_skip;	}

	FINLINE	void	inc_blk_test_lost_err_nb( INT32 nb )	{ _blk_test_lost_err_nb += nb;	}
	FINLINE	void	inc_blk_test_lost_received()			{ ++_blk_test_lost_received_nb;	}
	FINLINE	void	inc_img_sent_nb()
	{
		++_img_sent_nb;
		++c_net::img_sent_nb;
	}
	FINLINE	void	inc_img_received_nb()
	{
		++_img_received_nb;
		++c_net::img_received_nb;
	}
	FINLINE	void	inc_img_err_nb()
	{
		++_img_err_nb;
		++c_net::img_err_nb;
	}

	//todo improve with CONST and/or ref (pause pbs with opt)
	virtual	o_str*	get_comment();

			void	do_received( UINT8 CONST * CONST buf, INT32 len );

			void	loop_receive_broadcast();
			void	start_loop_receive_broadcast();

			void	loop_receive_tcp(		c_sock* ps );
			void	start_loop_receive_tcp(	c_sock* ps );

			void	loop_listen();
			void	start_loop_listen();

			void	restart();
			AAA_ERR	start();
			void	stop();

			void	update();

//	void	send( INT32 channel_id, INT32 type, CHAR* buf, INT32 len );
//	void	send_float( INT32 channel_id, REAL value );
//	void	send_ascii( INT32 channel_id, CHAR * buf );

	virtual	void	print_string(		C_PCHAR_C fmt, ... ) CONST;
	virtual	void	err_print(	C_PCHAR_C fmt, ... ) CONST;
};

