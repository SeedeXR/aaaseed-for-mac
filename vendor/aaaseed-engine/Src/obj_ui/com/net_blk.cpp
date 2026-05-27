#include "net_link.h"	//because winsock2 need to be first
#include "net_blk.h"

#include "checksum.h"
#include <unordered_map>
#include "time/aaa_time.h"
#include "infrastructure/watchdog.h"
#include "file/asc_parser.h"
#include "obj_ui/tracker/trackers.h"
#include "obj_ui/com/midi.h"
#include "language/lua/aaalua_glue.h"
#include "language/lua/aaalua_wrap.h"
#include "infrastructure/data/datacube.h"
#include "obj_ui/tracker/toaster.h"
#include "infrastructure/obj/command.h"
#include "ui/AAA_event.h"
#include "image/img.h"
#include "image/img_master.h"
#include "obj_ui/bdd/bdd_edit/bdd_line_3d.h"


namespace osc
{
	FINLINE	CONST UINT8*	next_field( UINT8 CONST * buf, INT32 len )
	{
		CONST UINT8* start;
		while( *(++buf)!=0 && --len>0 );
		start = buf;
		while( (reinterpret_cast<uintptr_t>(++buf) & 0x3) != 0 );
		if( (buf-start)<len )
			return buf;
		return nullptr;
	}
	FINLINE	bool	is( CONST UINT8* buf, INT32 len )
	{
		if( *buf=='/' )
		{
			buf = next_field( buf, len );
			if( buf && *buf == ',' )
				return true;
		}
		return false;
	}
}

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
namespace
{
	static	CONSTEXPR C_PCHAR_C	str_net_blk_type_ui[c_net::BLK_TYPE_NB_MAX] =
	{
		"NONE",			"PING",				"HELLO",		"BYE",
		"TRAX",			"MIDI",				"EVENT",		"DATA",
		"ASCII",		"MAX/MSP",			"STR_MIDI",		"STR_DATAGRID",
		"OSC",			"BLK_BUNDLE_OSC",	"COMMAND_V0",	"LUA",
		"LUA_WATCHDOG",	"LINK",				"TEXT_8",		"RAW",
		"PRINT",
//		"MEM",
		"IMAGE",
		"BDD_LINE_3D",
//		"UNIVIEW",
	};
//todonow en fait les block PJLINK sont reconnu ici donc il doivent etre identifi� ici ou pas reconnu ici
}

//C_NET_BLK
c_net_blk::c_net_blk()
	:_link(-1)
	,_channel(-1)
	,_channel_sub(-1)
	,_sender(-1)
	,_blk_id(-1)
	,_type(c_net::BLK_NONE)
	,_len(0)
	,_data_size(0)
	,_data(nullptr)
	,_data_osc(nullptr)
{
}

c_net_blk::~c_net_blk()
{
	dealloc();
}
	
void	c_net_blk::alloc( INT32 CONST byte_nb )
{
	if( byte_nb > _data_size )
	{
		_data = (UINT8*) REALLOC_ALIGNED( _data, byte_nb );
		if( _data )
			_data_size = byte_nb;
		else
			_data_size = 0;
	}
}

void	c_net_blk::dealloc()
{
	if( _data_size != 0 )
	{
		FREE_ALIGNED_AND_NULL( _data );
		_data_size = 0;
	}
}

//todonow get rid of cr at the end of PJLINK blk ?
void	c_net_blk::store_data( UINT8 CONST * CONST data, INT32 CONST len )
{
	alloc( len+1 );
	if( _data )
	{
		MEMCPY( _data, data, len, __FUNCTION__ );
		_len = len;
		*( _data + _len ) = 0;	//to stop printf...
		_b_valid = true;
	}
}

/*
FINLINE	void	c_net_blk::set_data( UINT8 CONST * CONST data, INT32 len )
{
	if( _data_size != 0)
		dealloc();
	_data = data;
	_len = len;
}
*/
/*
class		test_lost_blk_by_sender
{
public:
	INT32	_blk_id;
};
*/

typedef	std::unordered_map< INT32, INT32 >	TEST_LOST_BLK_MAP;
static	TEST_LOST_BLK_MAP					test_lost_blk_map;
aaa::MUTEX									test_lost_lock;

void	c_net_blk::check_and_store( INT32 CONST link, UINT8 CONST * buf, INT32 len )
{
	_b_valid = false;
	_link = link;
	_channel = 0;
	_channel_sub = 0;
	_sender = 0;

	c_net_link* p_link = net->get_link(link);
	if( !p_link )
		return;

	// can't work this way in tcp
	//if( *buf==0x56 && *(buf+1)==0xd5 && *(buf+2)==0x09 )
	//{	//UNIVIEW
	//	_type = c_net::BLK_UNIVIEW;
	//	store_data( buf, len );
	//	_time_received = aaa::time::get_no_origin();
	//}
	//else

	if(		*(UINT32*)buf == *(UINT32*)osc::bundle_header	//	decomposition in 2 tests to optimize
			&&	str_is_equal( (CHAR*)buf+4, osc::bundle_header +4, osc::BUNDLE_HEADER_SIZE -4 )
		)
	{	//case MAX
//		buf += osc::BUNDLE_HEADER_SIZE;
//		len -= osc::BUNDLE_HEADER_SIZE;
		//	 check if it's OSC data_ or an old max blk
		if( osc::is( buf+20, len-20 ) )
		{
			if( p_link->_b_blk_osc_discard )
				return;

			_type = c_net::BLK_BUNDLE_OSC;
			if( c_net::b_verbose_osc_received_ui )
				c_net::osc_print( "OSC Received", buf, len );
			store_data( buf, len );
			_data_osc = _data + 20;
			_time_received = aaa::time::get_no_origin();
		}
		else
		{
			buf += osc::MSG_HEADER_SIZE;
			len -= osc::MSG_HEADER_SIZE;
			_type = c_net::BLK_MAX_BY_OPCODE;
			len = ntohl( *(UINT32*)buf);
			buf += 4;
	 		if( c_net::b_verbose_osc_received_ui )
				c_net::osc_print( "OSC Received", buf, len );
			store_data( buf, len+1 );
			_data_osc = _data;
			_time_received = aaa::time::get_no_origin();
		}
	}
	else if( osc::is( buf, len ) )
	{
		if( p_link->_b_blk_osc_discard )
			return;

		_type = c_net::BLK_OSC;
		if( c_net::b_verbose_osc_received_ui )
			c_net::osc_print( "OSC Received", buf, len );
		store_data( buf, len );
		_data_osc = _data;
		_time_received = aaa::time::get_no_origin();
	}
	else if( *buf=='C' && str_is_equal( (CHAR*)buf, "CTRL", 4 ) )
	{
		if( len > 9 )
		{
			_type = c_net::BLK_STR_MIDI;
			buf += 4;
			len = (INT32)strlen((CHAR*)buf);	//	should we move to len -= 4
			store_data( buf, len+1);
		}
		else
			p_link->err_print( "BLK IN CTRL (MIDI controller) too short" );
	}
	else if( *buf=='D' && str_is_equal( (CHAR*)buf, "DGRI", 4 ) )
	{
		if( len >= 4 )
		{
			_type = c_net::BLK_STR_DATAGRID;
			buf += 4;
			len = (INT32)strlen((CHAR*)buf);	//	should we move to len -= 4
			store_data( buf, len+1);
		}
		else
			p_link->err_print( "BLK IN DGRI (Datagrid) too short" );
	}
	else if ( *buf == 0x12 )
	{
		_type = c_net::BLK_TEXT8;
		buf += 1;
		len = (INT32)strlen((CHAR*)buf);	//	should we move to len -= 4
		store_data( buf, len+1);
	}
	else if( p_link->is_blk_always_valid() )
	{
		_type = c_net::BLK_LINK;
		store_data( buf, len );
	}
	else if( len == -1 || ( len>=0 && len>24 ) )	//24 should be checked
		//rest
	{
		UINT32*		pt;

		//	collect min info
		pt = (UINT32 *)buf;

		_channel		= ntohl(*pt++);
		_channel_sub	= c_net::extract_channel_sub( _channel );
		_channel		= c_net::extract_channel( _channel );

		_sender = ntohl(*pt++);

		if( _channel != 0 )	//	channel 0 always go thru
		{	// if self emitted and we don't loop reception on non 0 channel
			if( _sender == net->_host_id && !net->_b_loop_reception_channel_non_0 )
			{	
				// discard
				//	should not count in received
				--c_net::blk_received_nb;
				--p_link->_blk_received_nb;
				//  but in self received
				++c_net::blk_received_self_nb;
				++p_link->_blk_received_self_nb;

				if( net->is_verbose_in() )
					p_link->print_string( "BLK IN discarded because self emitted" );

				return;
			}
		}

		_blk_id = ntohl(*pt++);

		//	check lost blk
		if( c_net::b_test_blk_lost )
		{
			std::lock_guard<aaa::MUTEX> guard(test_lost_lock);
			if( !c_net::b_test_blk_lost_last )
			{	//reset if if was off
				c_net::b_test_blk_lost_last = true;
				c_net::test_blk_lost_nb = 0;
				c_net::test_blk_lost_receive_nb = 0;	
				test_lost_blk_map.clear();
			}

			++c_net::test_blk_lost_receive_nb;
			p_link->inc_blk_test_lost_received();

			//	find the last blk id by sender
			TEST_LOST_BLK_MAP::iterator it;
			INT32	key = _sender + (_link << 16);	// we check for each link and sender
			it = test_lost_blk_map.find( key );
			if ( it == test_lost_blk_map.end() )
			{	//	if none create
				test_lost_blk_map[ key ] = _blk_id;
			}
			else
			{
				if( _blk_id != 1 )	// sender restarted
				{
					INT32	blk_id_expected = it->second + 1 ;		// blk_id are just incremented
					INT32	lost_nb = _blk_id - blk_id_expected ;
					if( lost_nb != 0 )
					{
						if( lost_nb > 0 )
						{
							c_net::test_blk_lost_nb += lost_nb ;
							p_link->inc_blk_test_lost_err_nb( lost_nb );

							if( c_net::b_test_blk_lost_verbose )
								p_link->err_print( "BLK IN lost %d -> blk_id %d sender %d, channel %d", lost_nb, _blk_id, _sender, _channel );
						}
						else
						{
							p_link->err_print( "BLK IN negative lost %d blk -> blk_id %d sender %d, channel %d", lost_nb, _blk_id, _sender, _channel );
							p_link->err_print( "BLK ON probably another machine restarted" );
						}
					}
				}
				it->second = _blk_id;
			}
		}
		else	//make sure we reset next time
			c_net::b_test_blk_lost_last = false;

		if( _channel != 0 )
		{	//	channel 0 go thru
			if( OUTSIDE( _channel, net->_channel_receive_begin, net->_channel_receive_end ) )
			{	//	filter channel
				if( net->is_verbose_in() )
					p_link->print_string( "BLK IN discarded on channel %d because outside of [%d,%d] general interval",
						_channel, net->_channel_receive_begin, net->_channel_receive_end );
				return;
			}
		}

		_time = ntohd( *(unsigned __int64 *)pt );
		pt += 2;	// because of unsigned __int64
		_type = (c_net::BLK_TYPE)ntohl(*pt++);

		if( c_watchdog::is_on() )
		{
			switch( _type )
			{
			case c_net::BLK_LUA_WATCHDOG:
				_type = c_net::BLK_LUA;	//	now it is a regular LUA BLK
				break;
			default:
				return;
			}
		}
		else
		{
			switch( _type )
			{
			case c_net::BLK_COMMAND_V0:
				if( !net->is_remote_receive() )
					return;
				break;
			case c_net::BLK_LUA_WATCHDOG:
				return;
			default:
				break;
			}
		}

		_len = ntohl(*pt++);
		//	ntohl: the checksum field on the wire is big-endian (see sender in
		//	net_link.cpp). Decode to host byte order before comparing against
		//	the locally-recomputed checksum value.
		UINT32 checksum = ntohl(*pt);
		*pt++ = 0;	//	checksum at 0 to compute it

		//protect against bad or old block
		if(	(_len!=0)
				&& _len <= (c_net::BLK_SIZE_MAX - c_net::BLK_HEADER_SIZE )
				&& (_type >= c_net::BLK_NONE) && (_type < c_net::BLK_TYPE_NB_MAX ) 
			)
		{
			bool	b_valid;
			if( checksum == 0 || p_link->is_blk_checksum_skip() )
				b_valid = true;
			else
			{
				c_checksum	check;
				check.add_buf( buf, len );
				b_valid = ( check.get() == checksum );
			}
			if( b_valid )
			{
				if( !c_net::b_blk_received_always_invalid )
					store_data( (UINT8*)pt, _len );
			}
			else 
				p_link->err_print( "BLK IN Invalid because checksum is bad." );
		}
		else
			p_link->err_print( "BLK IN invalid or old format" );
	}

}

void	c_net_blk::print() CONST
{
	c_net_link* p_link = net->get_link(_link);
	p_link->print_string( "BLK channel %d, from %d, id %d, type %s, len %d", _channel, _sender, _blk_id, str_net_blk_type_ui[_type], _len );
	p_link->print_string( "BLK\tContent : \"%s\"", _data );
}

bool	c_net_blk::do_process( bool b_verbose )
{
	UINT8*	pt = _data;
	bool	b_done = true;
	if( !pt )
		return b_done;	//hack avoid crash

	INT32	nb = _len;
	switch( _type )
	{
	case c_net::BLK_TRAX:
		{
			std::lock_guard<aaa::MUTEX> guard(_do_process_lock);
			nb /= 2 * sizeof(INT32) + sizeof(REAL);
			while( nb-- > 0 )
			{
				INT32	ch;		//	channel
				INT32	ctr;	//	control

				ch = *(INT32*)pt;
				pt += sizeof(INT32);

				ctr = *(INT32*)pt;
				pt += sizeof(INT32);

				c_net::set_matrix_value( ch, ctr, *(REAL *)pt );
				pt += sizeof(REAL);
				if( b_verbose )
					net->get_link(_link)->print_string( "BLK Matrix Channel %d Control %d : value %f", ch, ctr, (DOUBLE)c_net::get_matrix_value( ch, ctr ) );
			}
		}
		break;
	case c_net::BLK_STR_MIDI:
		{
			INT32	ch;
			INT32	index;
			REAL	val;
			c_asc_parser_cstring	parser;
			std::lock_guard<aaa::MUTEX> guard(_do_process_lock);
			if( b_verbose )
				net->get_link(_link)->print_string( "BLK hack b_midi_receive_from_net with %d", ntohl(*(INT32*)pt) );

			parser.set_start( (CHAR*)pt);
			while( 1 )
			{
				if( !parser.get_next_int( ch ) )
					break;
				if( !parser.get_next_int( index ) )
				{
					net->get_link(_link)->err_print( "BLK Error parsing net CTRL block -> skipping" );
					break;
				}
				if( !parser.get_next_word() )
				{
					net->get_link(_link)->err_print( "BLK Error parsing net CTRL block -> skipping" );
					break;
				}
				if( str_is_diff_nocase( parser.get_word(), "invalid") )
				{
					val = REAL( atof( parser.get_word()) );
					c_midi::static_set_control( ch, index, val, 0, false, true );
					if( b_verbose )
						net->get_link(_link)->print_string( "BLK MIDI Channel %d Control %d: value %f", ch, index, val );
				}
			}
		}
		break;
	case c_net::BLK_PRINT:
		if( c_net::b_print_receive )
		{
			//first byte is 0;
			INT32	host	= (UINT8)	pt[1];
			INT32	header	= (UINT8)	pt[2];
			//byte 4 is 0 (alignment/padding)
			CHAR*	pt_buf	= (CHAR*)	&pt[ aaa::mess::BLK_PRINT_HEADER_BYTE_NB ];
			aaa::mess::print_net_received( header, host, pt_buf );
		}
		//GOOD_PRINT_STRING( "Hello" );
		break;
	case c_net::BLK_BUNDLE_OSC:
	case c_net::BLK_OSC:
		//net->osc_process( pt, nb );
		net->get_link(_link)->err_print( "BLK_OSC or BLK_BUNDLE_OSC, AAASeed don't use that path anymore, contact Maa", __FUNCTION__ );
		break;
	case c_net::BLK_STR_DATAGRID:
		{
			INT32	ch;
			INT32	row;
			INT32	col;
			REAL	val;
			c_asc_parser_cstring	parser;
			std::lock_guard<aaa::MUTEX> guard(_do_process_lock);

			if( b_verbose )
				net->get_link(_link)->err_print( "BLK hack b_datagrid_receive_from_net with %d", ntohl(*(INT32*)pt) );

			parser.set_start( (CHAR*)pt);
			while( 1 )
			{
				//	we parse and expect for a succession of
				//		channel_id push
				//		channel_id row col word
				//		channel_id row col value
				//		channel_id row col invalid
				//get channel_
				if( !parser.get_next_int( ch) )
					break;
				//get push or row
				if( !parser.get_next_word() )
				{
					net->get_link(_link)->err_print( "BLK Error parsing row in DGRI block -> skipping" );
					break;
				}
				if( str_is_equal_nocase( parser.get_word(), "push") )
				{
					g_datacube->push_row( ch);
					if( b_verbose )
						net->get_link(_link)->print_string( "BLK DGRI Ok push" );
				}
				else
				{
					row = atol( parser.get_word());
					//get col
					if( !parser.get_next_int( col) )
					{
						net->get_link(_link)->err_print( "BLK parsing col in DGRI block -> skipping" );
						break;
					}
					//get data_
					if( !parser.get_next_word() )
					{
						net->get_link(_link)->err_print( "BLK parsing _data in DGRI block -> skipping" );
						break;
					}
					if( parser.is_word() )
					{
						g_datacube->set_str( ch, row, col, parser.get_word() );
						if( b_verbose )
							net->get_link(_link)->print_string( "BLK CONTROL SET Channel %d Row %d Col %d: value %s", ch, row, col, parser.get_word() );
					}
					else
					{
						if( str_is_diff_nocase( parser.get_word(), "invalid") )
						{
							val = REAL( atof( parser.get_word()) );
							g_datacube->set_double( ch, row, col, val );
							if( b_verbose )
								net->get_link(_link)->print_string( "BLK CONTROL SET Channel %d Row %d Col %d: value %f", ch, row, col, val );
						}
					}
				}
			}
		}
		break;
	case c_net::BLK_MAX_BY_OPCODE:		//todo
#if	AAA_TRACKER_TOASTER()
		if( (g_toaster_cur || c_net::b_midi_receive) && ntohl(*(INT32*)pt) == 0 )
#else
		if( c_net::b_midi_receive && ntohl(*(INT32*)pt) == 0 )
#endif
		{
			if( b_verbose )
				net->get_link(_link)->print_string( "BLK hack b_midi_receive_from_net with %d", ntohl(*(INT32*)pt) );
			pt += 4;
			nb -= 4;
			//hack get rid of leading garbage ??? done in Nov 2003 for Nib in Toulouse
			//pt += 12;
			//nb -= 12;
		}
		else
			break;
	case c_net::BLK_MIDI:
		if( c_net::b_midi_receive )
		{
			std::lock_guard<aaa::MUTEX> guard(_do_process_lock);
			nb /= 5*sizeof(INT32);
			while( nb-- > 0 )
			{
				INT32	what	= ntohl(*(INT32*)pt);
				pt += sizeof(INT32);
				INT32	ch		= ntohl(*(INT32*)pt);
				pt += sizeof(INT32);
				INT32	index	= ntohl(*(INT32*)pt);
				pt += sizeof(INT32);
				INT32	val		= ntohl(*(INT32*)pt);
				pt += sizeof(INT32);
				INT32	when	= ntohl(*(INT32*)pt);
				pt += sizeof(INT32);

				switch(what)
				{
				case MIDI_CODE_CONTROL:
					{
						c_midi::static_set_control( ch, index, REAL(val)/REAL(65536), when, false, true );
						if( b_verbose )
							net->get_link(_link)->print_string( "BLK MIDI Channel %d Control %d: value %d", ch, index, val );
					}
					break;
				case MIDI_CODE_VELOCITY:
					{
						c_midi::static_set_velocity( ch, index, REAL(val)/REAL(65536), when, false );
						if( b_verbose )
							net->get_link(_link)->print_string( "BLK MIDI Channel %d note %d: velocity %d", ch, index, val );
					}
					break;
				default:
					net->get_link(_link)->err_print( "BLK_MIDI have an unknown what code : %d", what );
					break;
				}
			}
		}
		break;
	case c_net::BLK_EVENT:
		if( net->is_remote_receive() )
		{
			//todo deal with it
			net->get_link(_link)->print_string( "BLK_EVENT from sender_id %d", _sender );
			{
				std::lock_guard<aaa::MUTEX> guard(_do_process_lock);
				c_event::do_from_mem( pt );
			}
		}
		break;
	case c_net::BLK_COMMAND_V0:
		if( net->is_remote_receive() )
		{
			//todoqqq use lua to do it
			net->get_link(_link)->print_string( "BLK_COMMAND from sender_id %d : %s", _sender, _data );
			{
				std::lock_guard<aaa::MUTEX> guard(_do_process_lock);
				command_do( (CHAR*)_data );
			}
		}
		break;
	case c_net::BLK_LUA:
		//NET_BLK_PRINT_STRING( _link, "Command from sender_id %d : %s", _sender, _data );
		{
			std::lock_guard<aaa::MUTEX> guard(_do_process_lock);
			aaalua::net_process( (CHAR*)_data );
		}
		break;
	case c_net::BLK_IMG:
		if( b_verbose )
			net->get_link(_link)->print_string( "BLK_IMG id %d channel %d with len %d", _blk_id, _channel, _len );
		if( g_img_master->is_net_in_active() )
		{
			std::lock_guard<aaa::MUTEX> guard(_do_process_lock);
			c_img_2d::process_blk( _data, _len, _link );
		}
		break;
	case c_net::BLK_LINE_3D:
		if( b_verbose )
			net->get_link(_link)->print_string( "BLK_LINE_3D id %d channel %d with len %d", _blk_id, _channel, _len );
		if( c_bdd_line_3d::master->is_net_in_active() )
		{
			std::lock_guard<aaa::MUTEX> guard(_do_process_lock);
			c_bdd_line_3d::process_blk( _data, _len, _link );
		}
		break;
	case c_net::BLK_TEXT8:
		{
			std::lock_guard<aaa::MUTEX> guard(_do_process_lock);
			g_lua_wrap_master->do_fn_pass_s( nullptr, "aaa.net.hook_receive_text8", (CHAR*)_data );
		}
		net->get_link(_link)->print_string( "BLK Command from sender_id %d : %s", _sender, _data );
		break;
	case c_net::BLK_PING:
		net->get_link(_link)->print_string( "BLK Ping from sender_id %d", _sender );
		break;
	case c_net::BLK_HELLO:
		net->get_link(_link)->print_string( "BLK Hello from sender_id %d : %s", _sender, pt);
		break;
	case c_net::BLK_BYE:
		net->get_link(_link)->print_string( "BLK Bye from sender_id %d : %s", _sender, pt);
		break;
	default:
		b_done = false;
		break;
	}
	return b_done;
}
