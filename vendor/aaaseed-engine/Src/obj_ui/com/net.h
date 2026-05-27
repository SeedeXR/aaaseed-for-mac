
#ifdef AAA_NET_H
#error "NET_H included more than once."
#endif
#define AAA_NET_H 1


//todo if commented don't work
#define	AAA_NET()	1

#if	AAA_NET()

#ifndef AAA_SOCKET_H
#	include	"socket.h"
#endif
#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif
#ifndef AAA_AAA_STR_H
#	include "aaa_str.h"
#endif
#ifndef _DEQUE_
#	include <deque>
#endif
#ifndef AAA_AAA_MUTEX_H
#	include "aaa/aaa_mutex.h"
#endif

class c_event;
class c_net_blk;
class c_net_link;

namespace osc
{
	static CONSTEXPR	C_PCHAR_C 	bundle_header	= "#bundle\0";
	//static CONSTEXPR	C_PCHAR_C bundle_header		= "#bundle\0\0\0\0\0\0\0\0\1";
	static CONSTEXPR	INT32 MSG_HEADER_SIZE		= 16;
	static CONSTEXPR	INT32 BUNDLE_HEADER_SIZE	= 8;
	static CONSTEXPR	C_PCHAR	tuio_header			= "/tuio/2Dcur";

						class		OutboundPacketStream;
						class		ReceivedBundle;
						class		ReceivedMessage;
						class		ReceivedPacket;
	//static CONSTEXPR	INT32	BLK_SIZE_MAX	= 1500;		//from jasch
	//static CONSTEXPR	INT32	BLK_SIZE_MAX	= 1472;	//1500 for ethernet - 20 (ip header) - 8 (udp header)
	//													//1472 vient aussi du test de ping sur un PC
	static CONSTEXPR	INT32	BLK_SIZE_MAX	= 1200 - 20 - 8;	//1200 said french experts - 20 (ip header) - 8 (udp header)
														
	static CONSTEXPR	INT32	BUF_NB			= 512;
	static CONSTEXPR	INT32	DST_NB			= 4;

}	


class	c_net final : public c_obj_active_ui
{
public:
	static INT32 CONSTEXPR	BLK_HEADER_SIZE		= 32;
	//todo	make it dynamic
	static INT32 CONSTEXPR	BLK_SIZE_MAX		= c_sock::BLK_SEND_SIZE_MAX;

	//todo see whatis still used
	enum BLK_TYPE : INT32
	{
		BLK_NONE = 0,
		BLK_PING,			//do_process()	immediate	//	trigger ping message at reception	//todo respond with pong
		BLK_HELLO,			//do_process()	immediate	//	trigger hello message at reception
		BLK_BYE,			//do_process()	immediate	//	trigger bye message at reception
		BLK_TRAX,			//do_process()				//	transport trax info
		BLK_MIDI,			//do_process()				//	transport midi info
		BLK_EVENT,			//do_process()				//	transport event info //todo not treated in reception for now (Jan 2004)
		BLK_OBJ_DATA,		//process by obj			//	used by obj to transfer data, Fev 2018 used by blob, boid, mocap, part
		BLK_ASCII,			//process by obj			//	used by obj to transfer ascii data usually com with other software
		BLK_MAX_BY_OPCODE,	//do_process()				//	exchange data with Max from opcode/Cycling 77 
		BLK_STR_MIDI,		//do_process()				//	receive MIDI data as text, easy way to control AAASeed
		BLK_STR_DATAGRID,	//do_process()				//	receive Datagrid data as text, easy way to control AAASeed (done for pulsevision)
		BLK_OSC,			//do_process()				//	transport OSC info (max/msp otudp...) see http://www.cnmat.berkeley.edu/OpenSoundControl/
		BLK_BUNDLE_OSC,		//do_process()				//	when a device send bundle with no encapsulation (e.g. kitchen)
		BLK_COMMAND_V0,		//do_process()				//	use to remotely control AAASeed this a first try implementation
		BLK_LUA,			//do_process()				//	a lua script to execute
		BLK_LUA_WATCHDOG,	//do_process()				//	a lua script to execute only for the watchdog
		BLK_LINK,			//process by obj			//	private data retrieved by link index, blk_always_valid force intercept in this type, so an object can't use a link with a non AAA protocol
		BLK_TEXT8,			//do_process()				//	exchange string (with Medialon)
		BLK_RAW,			//only for sending			//	send raw strings without AAASeed net headers
		BLK_PRINT,			//do_process()				//	transport PRINT_STR console message
//		BLK_MEM,			//do_process()	immediate	//	transfer DATA e.g image
		BLK_IMG,			//do_process()	immediate	//	transfer DATA e.g image
		BLK_LINE_3D,		//do_process()	immediate	//	transfer DATA between bdd_line_3d done for Monaco : generalize ?
//		BLK_UNIVIEW,
		BLK_TYPE_NB_MAX
	};

	enum OSC_TAKING {
		OSC_TAKING_MIDI,
		OSC_TAKING_VARIABLE_SET,
		OSC_TAKING_LUA,
		OSC_TAKING_NONE
		};
	static OSC_TAKING osc_taking;

	friend	class	c_net_link;

	FACTORY_DECLARE( c_net, c_obj_active_ui );
	C_NO_CPY_MOVE(c_net)

public:
	static	CONSTEXPR	INT32	CHANNEL_NB		= 128;
	static	CONSTEXPR	INT32	CHANNEL_SUB_NB	= 128;
//	static	INT32 CONST CONTROL_NB = 128;

	static	CONSTEXPR	INT32	MATRIX_CHANNEL_NB = 128;
	static	CONSTEXPR	INT32	MATRIX_CONTROL_NB = 128;

	static	CONSTEXPR	INT32	LINK_NB		= 17;

	static			INT32	print_dst;

	static			INT32	midi_dst;
	static			bool	b_midi_hack_send_as_maxmsp;
	static			bool	b_midi_send;
	static			bool	b_midi_receive;
//	static			bool	b_midi_receive_from_toaster;

	static			INT32	osc_dst[osc::DST_NB];
	static			bool	b_verbose_osc_received_ui;
	static			bool	b_verbose_osc_processed_ui;
	static			bool	b_verbose_osc_out_ui;
	static			bool	b_verbose_osc_detailed_ui;

	static			bool	b_test_blk_lost;
	static			bool	b_test_blk_lost_verbose;
	static			bool	b_test_blk_lost_last;
	static			INT32	test_blk_lost_nb;
	static			INT32	test_blk_lost_receive_nb;
	static			INT32	blk_received_nb;
	static			INT32	blk_received_self_nb;
	static			INT32	blk_recycled_by_force_nb;
	static			INT32	blk_sent_nb;
	static			INT32	blk_out_error_nb;
	static			bool	b_blk_received_always_invalid;
	static			INT32	img_sent_nb;
	static			INT32	img_received_nb;
	static			INT32	img_err_nb;

	static			bool	b_print_send;
	static			bool	b_print_receive;

private:
	static			INT32	_blk_size_max;
public:	
	static	FINLINE	INT32*	get_blk_size_max_pt()		{	return &_blk_size_max;	}
	static	FINLINE	INT32	get_blk_size_max()			{	return _blk_size_max;	}
	static	FINLINE	INT32	get_blk_obj_data_size_max()	{	return _blk_size_max - c_net::BLK_HEADER_SIZE;	}

protected:
	bool	_b_verbose_in;
	bool	_b_verbose_out;
	bool	_b_active_net_ui;	//we already have an active flag in obj_active_ui

	//	bool	_b_ping_answer;
	bool	_b_master;
	bool	_b_time_slave;

	bool	_b_broadcast_keyboard_ui;
	bool	_b_broadcast_mouse_ui;
	bool	_b_broadcast_window_ui;

	//	INT32	_ping_nb;

	o_str	_str_hello;
	o_str	_str_bye;

	c_net_link*						_link[LINK_NB];

//	osc::OutboundPacketStream*		_osc_buffer[LINK_NB];

	typedef	std::list<c_net_blk*>	cont_pblk;
	typedef	cont_pblk::iterator		iter_pblk;

	INT32							_blk_nb;
	INT32							_blk_osc_nb;

	cont_pblk*						_p_blk_list;
	cont_pblk*						_p_blk_list_bis;

	cont_pblk						_blk_osc;
	cont_pblk						_blk_list;
	cont_pblk						_blk_list_bis;

	std::deque<c_net_blk*>			_blk_free;
	//bool							b_blk_list_lock;

	mutable aaa::MUTEX				_ta_blk_osc;
	mutable aaa::MUTEX				_ta_blk_list;
	mutable aaa::MUTEX				_ta_blk_free;

	CHAR*	_pt_trax_buf_out; 
	UINT8*	_pt_midi_buf_out;
	CHAR	_trax_buf_out[ c_net::BLK_SIZE_MAX*2 ];	// *2 si a security until the length is totally checked
	UINT8	_midi_buf_out[ c_net::BLK_SIZE_MAX*2 ];

	bool	_b_osc_flush_sync	[ osc::DST_NB ];
	UINT8*	_pt_osc_buf_out		[ osc::DST_NB ];
	INT32	_osc_buf_index		[ osc::DST_NB ];
	UINT8	_osc_buf_out		[ osc::DST_NB ]	[ osc::BUF_NB ]	[ osc::BLK_SIZE_MAX ];
	INT32	_osc_buf_out_len	[ osc::DST_NB ]	[ osc::BUF_NB ]	;
//	mutable aaa::MUTEX		osc_lock[ OSC_DST_NB ];
	void	osc_out_init();

//todoqqq
	DOUBLE	_net_time_received;
	bool	_b_net_time_received;

	INT32	_blk_processed_nb;
	INT32	_blk_allocated_nb;
	INT32	_blk_allocated_max;
	bool	_b_remote_send;
	bool	_b_remote_receive;

	o_str	_o_dir;
	bool	_b_save_trig_ui;

	o_str	_machine;
	o_str	_langroup;
	// number of retry to get langroup and machine name, and seconds of sleep in between
	REAL	_retry_time;
	REAL	_sleep_time;

	REAL	_start_sleep_time;		// wait before init net, so the IP address is good

	bool	_b_osc_lua_accept_ui;
	bool	_b_osc_lua_verbose_ui;
	bool	_b_osc_midi_control_accept_ui;
	bool	_b_osc_midi_control_verbose_ui;
	bool	_b_osc_variable_set_accept_ui;
	bool	_b_osc_variable_set_verbose_ui;
	INT32	_osc_packet_size;
	REAL	_osc_keep_time;

	bool	_b_use_ip_as_host_id;
	INT32	_blk_get_failed_nb;
public:
	INT32	_host_id;
	INT32	_host_id_set_master;
	REAL	_frustum_offset_x;
	REAL	_frustum_offset_y;

	bool	_b_max_blk_with_leading_zero;
	bool	_b_loop_reception_channel_non_0;
	INT32	_s_remote_link_dst;

	INT32	_channel_receive_begin;
	INT32	_channel_receive_end;

	INT32	_host_field[4];

	static	CONSTEXPR	INT32	HOST_IP_NB = 4;
	o_str	_host_ip[HOST_IP_NB];

public:
	//	volatile INT32	_index_to_loop;
private:
	FINLINE	void		send_midi_uint32( UINT32 data );
			void		send_midi( INT32 type, INT32 channel_id, INT32 index, REAL value, INT32 when );

public:
	static	REAL		get_matrix_value( INT32 channel_id, INT32 control_index );
	static	void		set_matrix_value( INT32 channel_id, INT32 control_index, REAL in );

			void		init();
	virtual	void		param_init_pt_static();
	virtual	void		param_init_pt();
	virtual	void		update();

	virtual	AAA_ERR		load_do_after(	o_str CONST & filename );
	virtual	AAA_ERR		save_do_before(	o_str CONST & filename );

	FINLINE	bool		check_link_index(	INT32 CONST link_index )		{	return 0 <= link_index && link_index < LINK_NB; }
	FINLINE	c_net_link*	get_link(			INT32 CONST link_index )
						{
							if( check_link_index(link_index) )
								return _link[link_index];
#if AAA_DEBUG()
							debug_break( "link_index %d is invalid" );
#endif
							return nullptr;
						}

	FINLINE	bool		is_master()			CONST { return _b_master; }
	FINLINE	bool		is_slave()			CONST { return !_b_master; }
	FINLINE	bool		is_time_slave()		CONST { return _b_time_slave; }
	FINLINE	bool		is_time_received()	CONST { return _b_net_time_received; }
	FINLINE	DOUBLE		get_time_received()		  { _b_net_time_received = false; return _net_time_received; }

	FINLINE	bool		is_verbose_in()		{ return _b_verbose_in; }
			void		set_verbose_in( bool in );
			void		flip_verbose_in();

	FINLINE	bool		is_verbose_out()	{ return _b_verbose_out; }
			void		set_verbose_out( bool in );
			void		flip_verbose_out();

			void		start();
			void		stop();

	FINLINE	void		osc_blk_lock();
	FINLINE	void		osc_blk_unlock();

		c_net_blk*		osc_blk_take();
		c_net_blk*		osc_blk_take_by_start( C_PCHAR str );
				
	FINLINE	bool		is_osc_lua_verbose()			CONST { return _b_osc_lua_verbose_ui; }
	FINLINE	bool		is_osc_midi_control_verbose()	CONST { return _b_osc_midi_control_verbose_ui; }
	FINLINE	bool		is_osc_variable_set_verbose()	CONST { return _b_osc_variable_set_verbose_ui; }


			void		blk_free( c_net_blk* pblk );
		c_net_blk*		blk_get();

			void		blk_store(c_net_blk* blk_in );
		//	when taken a block should always be freed thru blk_free

			c_net_blk*	blk_take_oldest();												//	not for osc blk
			c_net_blk*	blk_take_by_type( BLK_TYPE type );
			c_net_blk*	blk_take_by_type_channel( BLK_TYPE type, INT32 channel_in );	//	not for osc blk
			c_net_blk*	blk_take_by_link( INT32 index );								//	not for osc blk
			void		blk_receive( INT32 link, UINT8 CONST * CONST buf, INT32 buf_len );		
			void		blk_process();


	//	the slot 0 is the basic common communication link between AAASeed app
	//	then there is channels inside it 


			void		sendto(		INT32 CONST link_index, INT32 CONST channel_id, c_net::BLK_TYPE CONST type, UINT8 CONST * CONST buf, INT32 CONST len );
	FINLINE	void		sendto_link0(						INT32 CONST channel_id, c_net::BLK_TYPE CONST type, UINT8 CONST * CONST buf, INT32 CONST len )
			{
				sendto( 0, channel_id, type, buf, len );
			}			void		send_float( INT32 CONST channel_id, REAL CONST value );
			void		send_ascii( INT32 CONST channel_id, C_PCHAR_C buf );
			void		send_text8( INT32 CONST link_index, C_PCHAR_C buf );
			void		send_raw(	INT32 CONST link_index, C_PCHAR_C buf, INT32 CONST len );


	
			void		trax_flush();
			void		midi_flush();
			void		osc_flush_async(		INT32 s_dst );
			void		osc_flush(				INT32 s_dst );
			void		osc_flush();
			void		flush();

	static	bool		osc_is_dst_valid(		INT32 s_dst );
	static	void		osc_print_bundle(		C_PCHAR_C header,	osc::ReceivedBundle CONST & b );
	static	void		osc_print_message(		C_PCHAR_C header,	osc::ReceivedMessage CONST & msg );
	static	void		osc_print(				C_PCHAR_C header,	osc::ReceivedPacket CONST & p );
	static	void		osc_print(				C_PCHAR_C header,	UINT8 CONST * CONST pt, INT32 size );

	virtual	void		osc_process_message(	osc::ReceivedMessage CONST & msg );
/*
			bool		receive_osc( UINT8* tag, UINT8* arg, UINT8* data );
			void		receive_osc_str( UINT8* str );
			void		receive_osc_bundle( UINT8* buf, INT32 len );
			void		receive_osc( UINT8* pt, INT32 len );
			void		receive_osc_new( UINT8* pt, INT32 len );
*/
			void		send_trax(			INT32 CONST channel_id, INT32 CONST control_id,		REAL CONST value );
			void		send_midi_control(	INT32 CONST channel_id, INT32 CONST control_index,	REAL CONST value, INT32 CONST when=0 );
			void		send_midi_velocity(	INT32 CONST channel_id, INT32 CONST note_index,		REAL CONST value, INT32 CONST when=0 );

			void		send_event(			c_event * CONST pt );
			void		send_osc(			CONST INT32 s_dst, C_PCHAR_C tag, C_PCHAR_C arg, UINT8 CONST * CONST buf );
			void		send_osc_str(		CONST INT32 s_dst, C_PCHAR_C buf );
			void		send_osc(			CONST INT32 s_dst, osc::OutboundPacketStream * CONST p );

			void		send_film_on(		C_PCHAR_C name, REAL time );
			void		send_film_off(		C_PCHAR_C name );

			void		flip_remote_receive();
			void		set_remote_receive(	bool in );
	FINLINE	bool		is_remote_receive()		CONST	{	return _b_remote_receive; }

			void		flip_remote_send();
			void		set_remote_send(	bool in );
	FINLINE	bool		is_remote_send()		CONST	{	return _b_remote_send; }

			INT32		get_osc_packet_size()	CONST	{	return _osc_packet_size; }
	FINLINE	INT32		get_host_id()			CONST	{	return _host_id; };
			void		get_host();
	FINLINE	C_PCHAR_C	get_machine()			CONST	{	return _machine.get(); }
	FINLINE	C_PCHAR_C	get_langroup()			CONST	{	return _langroup.get(); }
			
	FINLINE	bool		is_broadcast_keyboard()	CONST	{	return _b_broadcast_keyboard_ui; }
	FINLINE	bool		is_broadcast_mouse()	CONST	{	return _b_broadcast_mouse_ui;    }
	FINLINE	bool		is_broadcast_window()	CONST	{	return _b_broadcast_window_ui;   }


public:
	template<class T>
	static	FINLINE UINT8* write_value( UINT8* pt, T a )
	{
		T*	p = (T*)pt;
		*p = a;
		return (UINT8*)(++p);
	}
	template<class T>
	static	FINLINE UINT8* write_value( UINT8* pt, T a, T b )
	{
		T*	p = (T*)pt;
		*p = a;
		*++p = b;
		return (UINT8*)(++p);
	}
	template<class T>
	static	FINLINE UINT8* write_value( UINT8* pt, T a, T b, T c )
	{
		T*	p = (T*)pt;
		*p = a;
		*++p = b;
		*++p = c;
		return (UINT8*)(++p);
	}
	template<class T>
	static	FINLINE UINT8* write_value_v2( UINT8* pt, T CONST * a )
	{
		T*	p = (T*)pt;
		*p = *a;
		*++p = *++a;
		return (UINT8*)(++p);
	}
	template<class T>
	static	FINLINE UINT8* write_value_v3( UINT8* pt, T CONST * a )
	{
		T*	p = (T*)pt;
		*p = *a;
		*++p = *++a;
		*++p = *++a;
		return (UINT8*)(++p);
	}
	template<class T>
	static	FINLINE UINT8* write_value_v4( UINT8* pt,  T CONST * a )
	{
		T*	p = (T*)pt;
		*p = *a;
		*++p = *++a;
		*++p = *++a;
		*++p = *++a;
		return (UINT8*)(++p);
	}

	template<class T>
	static	FINLINE UINT8* read_value( UINT8 CONST * pt, T* a )
	{
		T*	p = (T*)pt;
		*a = *p;
		return (UINT8*)(++p);
	}
	template<class T>
	static	FINLINE UINT8* read_value( UINT8 CONST * pt, T* a, T* b )
	{
		T*	p = (T*)pt;
		*a = *p;
		*b = *++p;
		return (UINT8*)(++p);
	}
	template<class T>
	static	FINLINE UINT8* read_value( UINT8 CONST * pt, T* a, T* b, T* c )
	{
		T*	p = (T*)pt;
		*a = *p;
		*b = *++p;
		*c = *++p;
		return (UINT8*)(++p);
	}
	template<class T>
	static	FINLINE UINT8* read_value_v2( UINT8 CONST * pt, T* a )
	{
		T*	p = (T*)pt;
		*a = *p;
		*++a = *++p;
		return (UINT8*)(++p);
	}
	template<class T>
	static	FINLINE UINT8* read_value_v3( UINT8 CONST * pt, T* a )
	{
		T*	p = (T*)pt;
		*a = *p;
		*++a = *++p;
		*++a = *++p;
		return (UINT8*)(++p);
	}
	template<class T>
	static	FINLINE UINT8* read_value_v4( UINT8 CONST * pt, T* a )
	{
		T*	p = (T*)pt;
		*a = *p;
		*++a = *++p;
		*++a = *++p;
		*++a = *++p;
		return (UINT8*)(++p);
	}

	static	FINLINE	INT32	build_channel_id(		INT32 channel, INT32 channel_sub )	{ return channel + (channel_sub << 16);	}
	static	FINLINE	INT32	extract_channel(		INT32 channel_id )					{ return channel_id & 0xffff;	}
	static	FINLINE	INT32	extract_channel_sub(	INT32 channel_id )					{ return (channel_id >> 16)  & 0xffff;	}
};

//todoqqq
/*
void			net_event_add_start();
void			net_event_read_start();
ST_NET_EVENT *	net_event_get_next();
void			net_event_read_stop();
*/

extern	c_net*		net;

#endif	//	AAA_NET()

#define PARAM_DEF_NET_LINK( name, def, ina )		PARAM_DEF_INT32(			name,	def, ina,	0, c_net::LINK_NB-1	)

