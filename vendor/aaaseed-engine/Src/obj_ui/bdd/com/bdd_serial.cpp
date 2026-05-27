#include "bdd_serial.h"
#include "obj_ui/com/serial.h"
#include "time/aaa_time.h"
#include "obj_ui/com/net.h"
#include "obj_ui/com/net_blk.h"
#include "spy.h"
#include <charconv>


FACTORY_CREATE_PROP_V1( c_bdd_serial, bdd_serial, Serial link, bdd_serial, sub_menu="Com"; );

namespace	n_bdd_serial
{
	CONSTEXPR INT32	BASE_PARAM_NB		=	9 + c_bdd::NO_GEO_PARAM_NB;
	CONSTEXPR INT32	HOW_PARAM_NB		=	2;
	CONSTEXPR INT32	SERIAL_PARAM_NB		=	6;
	CONSTEXPR INT32	RECEIVED_PARAM_NB	=	5;
	CONSTEXPR INT32	LINE_PARAM_NB		=	3 + c_bdd_serial::LINE_NB_MAX;	

	CONSTEXPR INT32	GROUP_PARAM_NB	=	4;	

	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
									+	HOW_PARAM_NB
									+	SERIAL_PARAM_NB
									+	RECEIVED_PARAM_NB
									+	LINE_PARAM_NB
									+	GROUP_PARAM_NB;
			
	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_NO_GEO_BASE_PARAMS
		PARAM_DEF_BOOL_OFF(		active	)

		PARAM_DEF_GROUP_CLOSED( How, HOW_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		network						)
			PARAM_DEF_NET_LINK(		net_link,					2, 1	)
	
		PARAM_DEF_GROUP_CLOSED( Serial, SERIAL_PARAM_NB )
			PARAM_DEF_SYMBO_PSTR(	com_port_baudrate,			c_serial::BAUDRATE_19200, c_serial::BAUDRATE_115200, c_serial::str_baudrate	)
			PARAM_DEF_INT32(		com_port_nb,				2, 1,		1, 32			)
			PARAM_DEF_BOOL_OFF(		wait_answer					)
			PARAM_DEF_INT32(		wait_loop_nb_max,			1000, 100,	1, 1000000		)
			PARAM_DEF_SYMBO_NEG(	sleep_second_micro,			-1, 100,	-1, 1000000,	gstr::no	)
			PARAM_DEF_SYMBO_NEG(	sleep_ex_second_milli,		1, -1,		-1, 1000,		gstr::no	)

//			PARAM_DEF_INT32(			wait_answer_timeout_millisec,	2, 1,	1, 60000	) 

		PARAM_DEF_BOOL_OFF(		open_asked				)
		PARAM_DEF_BOOL_LOCKED(	open_state				)
		PARAM_DEF_BOOL_OFF(		verbose					)
		PARAM_DEF_BOOL_ON(		remove_newline_last		)

		PARAM_DEF_BOOL_ON(		send_newline_at_the_end	)
		PARAM_DEF_BOOL_ON(		send_newline_as_0x0d	)

		PARAM_DEF_BOOL_OFF(		send_trig				)
		PARAM_DEF_STR(			send_string				)

		PARAM_DEF_GROUP_CLOSED( Received, RECEIVED_PARAM_NB )
			PARAM_DEF_STR_LOCKED(		received_last			)
			PARAM_DEF_STR_LOCKED(		received_last_non_empty	)
			PARAM_DEF_INT32_LOCKED(		received_number_last	)
			PARAM_DEF_DOUBLE_LOCKED(	received_last_time		)
			PARAM_DEF_STR_LOCKED(		received				)

		PARAM_DEF_GROUP_CLOSED( Line Received, LINE_PARAM_NB )
			PARAM_DEF_BOOL_OFF(			line_received_split		)
			PARAM_DEF_INT32_POS_ZERO(	line_received_total		)
			PARAM_DEF_INT32_LOCKED(		line_received_nb		)
			PARAM_DEF_STR_LOCKED(		line_received_1			)
			PARAM_DEF_STR_LOCKED(		line_received_2			)
			PARAM_DEF_STR_LOCKED(		line_received_3			)
			PARAM_DEF_STR_LOCKED(		line_received_4			)
			PARAM_DEF_STR_LOCKED(		line_received_5			)
			PARAM_DEF_STR_LOCKED(		line_received_6			)
			PARAM_DEF_STR_LOCKED(		line_received_7			)
			PARAM_DEF_STR_LOCKED(		line_received_8			)
	};
}
	
void	c_bdd_serial::param_init_pt()
{
	INT32	h = param_init_pt_no_geo();

	param_set_pt( h, get_pt_active() );
	
	++h;
		param_set_pt( h, _b_network					);
		param_attach_obj_no_inc(	h, (c_obj_ui*)_net_link	);
		param_set_pt( h, _net_link_index_ui			);

	++h;
		param_set_pt( h, _s_baudrate			);
		param_set_pt( h, _com_port_id_ui		);
		param_set_pt( h, _b_wait_answer_ui		);
//		param_set_pt( h, _wait_answer_time_out	);
		param_set_pt( h, _wait_loop_nb_max_ui	);
		param_set_pt( h, _sleep_second_micro_ui	);
		param_set_pt( h, _sleep_second_milli_ui	);

	param_set_pt( h, _b_open_asked				);
	param_set_pt( h, _b_open					);
	param_set_pt( h, _b_verbose_ui				);
	param_set_pt( h, _b_remove_newline_last		);

	param_set_pt( h, _b_send_newline_at_the_end	);
	param_set_pt( h, _b_send_newline_as_0x0d	);

	param_set_pt( h, _b_send_trig_ui			);
	param_set_pt( h, _o_to_send					);
	++h;
		param_set_pt( h, _o_received_last			);
		param_set_pt( h, _o_received_last_empty_not	);
		param_set_pt( h, _o_received_number_last	);
		param_set_pt( h, _received_last_time		);
		param_set_pt( h, _o_received				);

	++h;
		param_set_pt( h, _b_line_received_split		);
		param_set_pt( h, _line_received_nb_total	);
		param_set_pt( h, _line_received_nb			);
		for( INT32 i=0; i < LINE_NB_MAX; ++i )
			param_set_pt( h, _o_line_received[i]	);

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_bdd_serial)
	,_b_open(false)
	,_com_port_id_opened(-42)
	,_o_received_number_last(0)
	,_received_last_time(0.)
	,_net_link(nullptr)
	,_line_received_nb(0)
{
	param_init_with( n_bdd_serial::param, n_bdd_serial::PARAM_NB_MAX );
	_serial = nullptr;
}

c_bdd_serial::~c_bdd_serial()
{
	close_serial();
}

/*
void	c_bdd_serial::set_port( INT32 port_id )
{
	_com_port_id_ui = port_id;
}
*/

void	c_bdd_serial::do_received( CHAR CONST * buf, INT32 len )
{
	if( len > 0 ) 
	{
		INT32 line_last = _line_received_nb;
		_received_last_time = aaa::time::get_real_time();

		_o_received_last.set( buf, len );
		if( _b_remove_newline_last )
			_o_received_last.remove_control_at_end();
		_o_received.set( _o_received_last );

		if( !_o_received_last.is_empty() )
			_o_received_last_empty_not.set( _o_received_last );

		std::from_chars( buf, buf + len - 1, _o_received_number_last );

		if( _b_verbose_ui )
			GOOD_PRINT_STRING( "%s() len %d str %s", __FUNCTION__, _o_received_last.get_len(), _o_received_last.get() );

		if( _b_line_received_split )
		{
			_line_received_nb = 0;
			INT32 len_total = _buf_keep_len + len;
			if( len_total > BUFFER_SIZE - 1 )
			{
				DBG_PRINT_STRING( "serial buffer overflow, discarding some chars" );
				_buf_keep_len = 0; 
			}
			MEMCPY( _buf_keep + _buf_keep_len, buf, len, __FUNCTION__ );
			len += _buf_keep_len;
			UINT8* pt_base = _buf_keep;
			UINT8* pt = pt_base;
			while( len )
			{
				if( *pt == 0x0d && len >= 2 && *(pt + 1) == 0x0a )
				{
					_o_line_received[_line_received_nb].set( (CHAR*)pt_base, INT32(pt-pt_base) );
					++_line_received_nb;
					pt += 2;
					len -= 2;
					pt_base = pt;
					if( _line_received_nb >= LINE_NB_MAX )
						break;
				}
				else
				{
					++pt;
					--len;
				}
			}
			_line_received_nb_total += _line_received_nb;
			MEMMOVE( _buf_keep, pt_base, pt-pt_base );
		}
		while( line_last > _line_received_nb )
			_o_line_received[--line_last].erase();
	}

}

void c_bdd_serial::update_serial()
{
	update_send();

	// receive
	_o_received_tmp.erase();
	INT32 loop_cnt = 0;
	do
	{
		INT32 nb = _serial->read_get_nb_ready();
		if( nb != 0 )
		{
			if( _b_verbose_ui )
				GOOD_PRINT_STRING( "read %d bytes from com port on loop %d", nb, loop_cnt );
			if( nb > BUFFER_SIZE-1 )
			{
				ERR_PRINT_STRING( "c_bdd_ir_touch::update_low(), buffer too small(%d) when receiving %d from COM port", BUFFER_SIZE, nb );
				nb = MIN( BUFFER_SIZE-1, nb );
			}
			if( NOERR( _serial->read( _buf_read, nb ) ) )
			{
				if( _b_wait_answer )	// we wait an answwer ending by a control char
				{ 
					for( INT32 i=0; i<nb; ++i )
					{
						if( _buf_read[i] < 32 )
						{
							_b_wait_answer = false;
							break;
						}
					}
				}
				_buf_read[nb] = 0;
				_o_received_tmp.add( (CHAR*)_buf_read );
			}
			loop_cnt = 0;
		}
		else
		{
			if(	_sleep_second_micro_ui >= 0 )
				spy::sleep_micro( _sleep_second_micro_ui, "sleep serial" );
			if(	_sleep_second_milli_ui >= 0 )
				spy::sleep_ex( _sleep_second_milli_ui, true, "sleep serial" );
		}
		if( !_b_wait_answer )
			break;
		if( ++loop_cnt > _wait_loop_nb_max_ui )
		{
			DBG_PRINT_STRING( "Serial exiting from receiving loop" );
			break;
		}
	}
	while( true );

	auto CONST len = _o_received_tmp.get_len();
	if( len > 0 )
		do_received( _o_received_tmp.get(), len );

}


o_str& c_bdd_serial::get_str_to_send()
{
	if( _b_send_newline_at_the_end )
	{
		C_PCHAR_C pt = _o_to_send.get(-1);
		if( pt && *pt!=0xa && *pt!=0x0d )
			_o_to_send.add_char( _b_send_newline_as_0x0d ? 0x0d : 0x0a );	
	}
	if( _b_send_newline_as_0x0d )
		_o_to_send.convert_line_feed_to_carriage_return();
	return _o_to_send;
}

AAA_ERR	c_bdd_serial::send( o_str& o )
{
	if( _b_verbose_ui )
		GOOD_PRINT_STRING( "%s() len %d str %s", __FUNCTION__, o.get_len(), o.get() );

	if( _b_network )
	{
		net->send_raw( _net_link_index_ui, o.get(), o.get_len() );
		return AAA_OK;
	}
	else
		return _serial->write( o.get(), o.get_len() );
}

void	c_bdd_serial::update_network()
{
	//this should be cleaned
	_net_link = net->get_link( _net_link_index_ui );
	update_send();
	c_net_blk* blk_in = net->blk_take_by_link( _net_link_index_ui );
	if( blk_in )
	{
		do_received( (CHAR*)blk_in->get_data_pt(), blk_in->get_len() );
		net->blk_free( blk_in );
	}
}

void	c_bdd_serial::update_send()
{
	if( _b_send_trig_ui )
	{
		send( get_str_to_send() );
		_b_send_trig_ui = false;
		_b_wait_answer = _b_wait_answer_ui;
	}
	else
		_b_wait_answer = false;
}

void	c_bdd_serial::update()
{
	if( is_active() )
	{
		_o_received_last.erase();
		_o_received_number_last = 0;
		if( _b_network )
		{
			update_network();
		}
		else
		{
			if( _com_port_id_opened != _com_port_id_ui )
				close_serial();
			if( _b_open != _b_open_asked )
			{
				if( _b_open_asked )
					open_serial();
				else
					close_serial();
			}
			if( _b_open )
				update_serial();
		}
	}
}

/*
void	c_bdd_serial::draw()
{
}
*/

void	c_bdd_serial::close_serial()
{
	if( _serial )
	{
		_serial->close();
		obj_delete( _serial );	
	}
	_b_open = false;
}

void	c_bdd_serial::open_serial()
{
	if( !_serial )
	{
		_serial = new c_serial;
		if( _serial )
		{
			_serial->set_baudrate( c_serial::BAUDRATE(_s_baudrate) );
			if( NOERR(_serial->open( _com_port_id_ui )) )
			{
				_b_open = true;
				_com_port_id_opened = _com_port_id_ui;
				_buf_keep_len = 0;
				//_command_time_last = aaa::time::get_real_time();
			}
			else
			{
				err_print( "Can't start serial on COM%d:", _com_port_id_ui );
				close_serial();
			}
		}
		else
			err_print( "Can't create serial" );
	}
}
