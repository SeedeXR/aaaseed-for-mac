#include "bdd_pjlink.h"
#include "obj_ui/com/net_link.h"	//because winsock2 need to be first
#include "obj_ui/com/net_blk.h"
#include "time/aaa_time.h"
#include "file/asc_parser.h"

FACTORY_CREATE_PROP_V1( c_bdd_pjlink, bdd_pjlink, PJLink protocol , bdd_pjlink, sub_menu="Hardware"; );

static	C_PCHAR_C str_cmd[c_bdd_pjlink::CMD_NB]=
{
	"POWR",
	"INPT",
	"AVMT",
	"ERST",
	"LAMP",
	"INST",
	"NAME",
	"INF1",
	"INF2",
	"INFO",
	"CLSS",
};

static	C_PCHAR_C str_state[c_bdd_pjlink::STATE_NB]=
{
	"Closed",
	"Calling",
	"Waiting for Id",
	"Ready",
	"Waiting for answer",
	"Closing"
};

static	C_PCHAR_C str_power[c_bdd_pjlink::STATE_NB]=
{
	"OFF",
	"ON",
	"Cooling",
	"Warming-up",
};

static	INT32 CONST INPUT_TYPE_NB = 5;
static	C_PCHAR_C str_input_type[INPUT_TYPE_NB]=
{
	"RGB",
	"VIDEO",
	"DIGITAL",
	"STORAGE",
	"NETWORK",
};


namespace n_bdd_pjlink
{
	CONSTEXPR INT32 BASE_PARAM_NB	=	7 + 4 + ( 1 + 2 * c_bdd_pjlink::LAMP_NB ) + c_bdd::NO_GEO_PARAM_NB;
	CONSTEXPR INT32 DEBUG_PARAM_NB	=	4;
	CONSTEXPR INT32 RESULT_PARAM_NB	=	c_bdd_pjlink::CMD_NB + 1;
	CONSTEXPR INT32 GROUP_PARAM_NB	=	2;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	RESULT_PARAM_NB
									+	DEBUG_PARAM_NB
									+	GROUP_PARAM_NB;
			
	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_NO_GEO_BASE_PARAMS
		PARAM_DEF_BOOL_OFF(		active			)
		PARAM_DEF_BOOL_OFF(		open			)
		PARAM_DEF_BOOL_OFF(		verbose			)

		PARAM_DEF_NET_LINK(		net_link, 2, 1	)

		PARAM_DEF_REAL_POS_ONE(	check_interval	)
		PARAM_DEF_BOOL_OFF(		power_asked		)
		PARAM_DEF_SYMBO_PSTR(	power_state,	1, 0,	str_power )

		PARAM_DEF_SYMBO_PSTR_ZERO(		s_input_type_asked,		str_input_type	)
		PARAM_DEF_INT32(				s_input_selector_asked,	2, 1,	1, 9	)
		PARAM_DEF_SYMBO_LOCKED_PSTR(	s_input_type_state,		str_input_type	)
		PARAM_DEF_INT32_LOCKED(			s_input_selector_state	)

		PARAM_DEF_INT32_LOCKED(	lamp_nb )
		PARAM_DEF_BOOL_LOCKED(	lamp_1_state )
		PARAM_DEF_INT32_LOCKED(	lamp_1_hour )
		PARAM_DEF_BOOL_LOCKED(	lamp_2_state )
		PARAM_DEF_INT32_LOCKED(	lamp_2_hour )
		PARAM_DEF_BOOL_LOCKED(	lamp_3_state )
		PARAM_DEF_INT32_LOCKED(	lamp_3_hour )
		PARAM_DEF_BOOL_LOCKED(	lamp_4_state )
		PARAM_DEF_INT32_LOCKED(	lamp_4_hour )

		PARAM_DEF_GROUP( Result, RESULT_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		result_trig )
			PARAM_DEF_STR_LOCKED(	Power )
			PARAM_DEF_STR_LOCKED(	Input )
			PARAM_DEF_STR_LOCKED(	Mute )
			PARAM_DEF_STR_LOCKED(	Error )
			PARAM_DEF_STR_LOCKED(	Lamp )			
			PARAM_DEF_STR_LOCKED(	Input_available )
			PARAM_DEF_STR_LOCKED(	name )
			PARAM_DEF_STR_LOCKED(	inf1 )
			PARAM_DEF_STR_LOCKED(	inf2 )
			PARAM_DEF_STR_LOCKED(	info )
			PARAM_DEF_STR_LOCKED(	class )

		PARAM_DEF_GROUP( Debug, DEBUG_PARAM_NB )
			PARAM_DEF_STR(					send )	
			PARAM_DEF_BOOL_OFF(				send_trig )	
			PARAM_DEF_STR_LOCKED(			received )	
			PARAM_DEF_SYMBO_LOCKED_PSTR(	state,		str_state	)
	};
}

	

void	c_bdd_pjlink::param_init_pt()
{
	INT32	h = param_init_pt_no_geo();

	param_set_pt( h, get_pt_active() );
	param_set_pt( h, _b_open_ui );
	param_set_pt( h, _b_verbose );
	
	param_attach_obj_no_inc( h, _link );
	param_set_pt( h, _link_index_ui );

	param_set_pt( h, _check_interval );
	param_set_pt( h, _b_power_asked );
	param_set_pt( h, _s_power_state );

	param_set_pt( h, _s_input_type_asked );
	param_set_pt( h, _s_input_selector_asked );
	param_set_pt( h, _s_input_type_state );
	param_set_pt( h, _s_input_selector_state );

	param_set_pt( h, _lamp_nb );
	for( INT32 i=0; i<LAMP_NB; ++i )
	{
		param_set_pt( h, _lamp_state[i] );
		param_set_pt( h, _lamp_time[i] );	
	}

	++h;
		param_set_pt( h, _b_result_trig_ui );
		for( INT32 i=0; i<CMD_NB; ++i )
			param_set_pt( h, _o_result[i]);

	++h;
		param_set_pt( h, _o_send );
		param_set_pt( h, _b_send_trig_ui );
		param_set_pt( h, _o_received );
		param_set_pt( h, _state );

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_bdd_pjlink)
	,_link(nullptr)
//	,_link_index(-42)
	,_b_open(false)
{
	set_state( CLOSED );

	_s_power_state = 2;	//	so the state will be asked

	_s_input_type_state = 0;
	_s_input_selector_state = 1;

	_lamp_nb = 0;
	for( INT32 i=0; i<LAMP_NB; ++i )
	{
		_lamp_state[i] = false;
		_lamp_time[i] = 0;	
	}
	_check_time = 0;

	param_init_with( n_bdd_pjlink::param, n_bdd_pjlink::PARAM_NB_MAX );
}

c_bdd_pjlink::~c_bdd_pjlink()
{
	close();
}

//todo check the '=' in the reply

bool	c_bdd_pjlink::receive_cmd( INT32 cmd, CHAR* buf, INT32 len )
{
	bool	b_ok = false;
	bool	b_ret = false;
	if( *buf=='E' && *(buf+1)=='R' && *(buf+2)=='R' )
	{
		//todo print and deal with that
		//todo deal also in each command
		b_ret = true;
	}
	else if( *buf=='O' && *(buf+1)=='K' )
	{
		b_ok = true;
		b_ret = true;
	}
	if( !b_ok )
		_o_result[cmd].set( buf, len, 1);	// done first because parser can destroy it
	switch( cmd )
	{
	case CMD_POWER:
		if( b_ok)
			_b_result_trig_ui = true;
		else if( len== 1)
		{
			_s_power_state = *buf-'0';
			b_ret = true;
		}
		break;
	case CMD_INPUT:
		if( b_ok)
			_b_result_trig_ui = true;
		else if( len==2 )
		{
			_s_input_type_state = CLAMP( INT32((*buf)-'1'), 0, 4 );
			_s_input_selector_state = CLAMP( INT32((*++buf) - '0'), 1, 9 );
			b_ret = true;
		}
		break;
	case CMD_LAMP:
		_lamp_nb = 0;
		{
			c_asc_parser	parser;
			INT32	i;
			parser.set_start( buf );
			while( parser.get_next_int(_lamp_time[_lamp_nb]) && parser.get_next_int(i) )
			{
				_lamp_state[_lamp_nb] = (i==1);
				++_lamp_nb;
				b_ret = true;
			}
		}
		break;
	}
	return b_ret;
}

void	c_bdd_pjlink::set_state( STATE in )
{
	_state = in;
	if( _b_verbose )
		VERBOSE_PRINT_STRING( "pjlink state set to %s", str_state[in] );
	switch( _state )
	{
	case WAITING_ANSWER:
		_waiting_count = 30;
		break;
	}
}
//todonow do a print_string in the object
//todonow do a verbose
//todo add the timeout and tryout

void	c_bdd_pjlink::update_low()	
{
	if( !_link || !_b_open )
		return;

	c_net_blk*	blk_in;
	while( blk_in = net->blk_take_by_link( _link->get_index() ) )
	{
		CHAR*	buf = (CHAR*)blk_in->get_data_pt();
		INT32	len = blk_in->get_len();
		//todoqqq	
		switch( _state )
		{
		case WAITING_ID:
			if( str_is_equal( (CHAR*)buf, "PJLINK ", 6 ) )
			{
				_link->print_string( "%s() received %s", __FUNCTION__, buf );
				set_state( READY );	//todonow deal with no crypting version
			}
			else
				_link->print_string( "%s() don't received PJLINK so discard %s", __FUNCTION__, buf );					
			break;
		case WAITING_ANSWER:
			{
				INT32	cmd;
				_o_received.set( buf+1, len-2, 1 );
				if( _b_verbose )
					_link->print_string( "%s() received %s", __FUNCTION__, _o_received.get() );

				cmd = find_cmd( buf+2, len-2);
				if( cmd>=0 )
				{
					receive_cmd( cmd, buf+7, len-8);					
					if( _result_index == cmd)
						++_result_index;
				}
			}
			set_state( READY );
			break;
		default:
			_link->err_print( "%s() don't wait for block : Discarding (%s)", __FUNCTION__, buf );
			break;
		}
		net->blk_free( blk_in );
	}
	if( _delta_t.update() )
		_check_time = 0. ;
	_check_time += REAL(_delta_t.get_dt());
	if( _check_time > _check_interval )
	{
		_check_time = 0.;
		_b_result_trig_ui = true;
	}
	if( _state==WAITING_ANSWER && _waiting_count > 0)
	{
		if( --_waiting_count == 0)
			set_state( READY );	
	} 
	if( _state != CLOSED && !_link->is_running() )
	{
		set_state( CLOSED );
	}
	if( _state!=WAITING_ANSWER && !_b_send_trig_ui )
	{
		if( _b_power_asked != (_s_power_state==1) )
		{	//	we need to turn it ON or OFF
			make_cmd( &_o_send, CMD_POWER, _b_power_asked ? "1" : "0" );
			_b_send_trig_ui = true;
		}
//	check_interval_ do it now
//		else if( s_power_state_ >= 2 )	//	not ON or OFF cooling down or starting
//		{	//	we keep asking
//			make_cmd( &o_send_, CMD_POWER, "?" );
//			b_send_trig_ = true;
//		}
		else if( _s_input_type_asked != _s_input_type_state || _s_input_selector_asked != _s_input_selector_state )
		{	//	force the right input if needed
			CHAR str[] = "--";
			str[0] = '1'+_s_input_type_asked;
			str[1] = '0'+_s_input_selector_asked;
			make_cmd( &_o_send, CMD_INPUT, str);
			_b_send_trig_ui = true;
		}
	}
	switch( _state )
	{
	case CLOSED:
		if( _b_send_trig_ui || _b_result_trig_ui  )	//	open please we need to send
		{
			_link->set_port_nb(4352);
			_link->set_tcp(true);
			_link->set_call(true);
			_link->set_tcp_no_marker(true);
			_link->set_blk_always_valid(true);
//					link_->set_enable(true);
			if( NOERR(_link->start()) )
				set_state( WAITING_ID );
		}
		break;
	case CALLING:
		//todo should not happen
		break;
	case WAITING_ID:
		break;
	case READY:
		if( _b_result_trig_ui )
		{
			if( _result_index<0 )
				_result_index = 0;
			if( _result_index<CMD_NB)
			{
				make_cmd( &_o_send, _result_index, "?" );
				add_cr_then_send( &_o_send );
			}
			else
			{
				_result_index = -1;
				_b_result_trig_ui = false;
			}
		}
		else if( _b_send_trig_ui )
		{
			add_cr_then_send( &_o_send );
			_b_send_trig_ui = false;
		}
		break;		
	}
}

void	c_bdd_pjlink::update()
{
	if( is_active() )
	{
		if( _b_open != _b_open_ui )
		{
			if( _b_open_ui )
				open();
			else
				close();
		}
		if( _b_open )
		{
			if( _link && (_link_index_ui != _link->get_index()) )
			{
				close();
				_link = net->get_link( _link_index_ui );
				open();
			}
			update_low();
		}
		else if( !_link || (_link_index_ui != _link->get_index()) )
		{
			_link = net->get_link( _link_index_ui );
		}
	}
}

void	c_bdd_pjlink::draw()
{
}

void	c_bdd_pjlink::close()
{	//todo	make sure we interup a running protocol
	_b_open = false;
}

void	c_bdd_pjlink::open()
{
	_b_open = true;
}

INT32	c_bdd_pjlink::find_cmd( CHAR* str, INT32 len)
{
	if( *(str+4) != '=' )
		_link->err_print( "%s() received block don't have a = after the command: Skipping blk", __FUNCTION__ );
	else if( *(str+len-1) != 0x0d )
		_link->err_print( "%s() received block don't have a CR at the end: Skipping blk", __FUNCTION__ );
	else
	{
		for( INT32 i=0; i<CMD_NB; ++i )
		{
			if( str_is_equal( str, str_cmd[i], 4) )
				return i;
		}
	}
	return -1;
}

AAA_ERR	c_bdd_pjlink::make_cmd( o_str* o, INT32 cmd, C_PCHAR_C str )
{
	o->set( "%1" );
	o->add( str_cmd[cmd] );
	o->add_space();
	o->add( str );
	return AAA_OK;
//	o->add_char( 0x0d );	done later
}


AAA_ERR	c_bdd_pjlink::add_cr_then_send( o_str* o )
{
	o_str l_o;		//todo dynamic allocation beurk
	l_o.set( o->get() );
	l_o.add_char( 0x0d );
	set_state( WAITING_ANSWER );
	net->send_raw( _link_index_ui, l_o.get(), l_o.get_len() );
	if( _b_verbose )
		_link->print_string( "PJLINK sent %s", o->get()+2 );
	return AAA_OK;
}
