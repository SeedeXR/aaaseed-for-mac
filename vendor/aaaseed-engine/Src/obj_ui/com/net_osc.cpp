
#include "obj_ui/com/net_link.h"	//because winsock2 need to be first

#ifndef AAA_INCLUDED_OSCRECEIVEDELEMENTS_H
#	include "osc/OscReceivedElements.h"
#endif
#include "osc/OscOutboundPacketStream.h"
#include "osc/OscPrintReceivedElements.h"

#include "obj_ui/bdd/bdd_multitouch/bdd_blob_tracking.h"
#include "language/lua/aaalua_glue.h"
#include "obj_ui/com/net_lua.h"

#include "obj_ui/tracker/trackers.h"
#include "obj_ui/com/midi.h"
#include "infrastructure/Data/aaa_global.h"


INT32	c_net::osc_dst[osc::DST_NB];
bool	c_net::b_verbose_osc_received_ui;
bool	c_net::b_verbose_osc_processed_ui;
bool	c_net::b_verbose_osc_out_ui;
bool	c_net::b_verbose_osc_detailed_ui;


void	c_net::osc_out_init()
{
	//for( INT32 i = 0; i < LINK_NB; ++i )
	//{
	//	_osc_buffer[i] = nullptr;
	//
	//	char*	buffer;
	//	buffer = new char[_osc_packet_size];
	//	_osc_buffer[i] = new osc::OutboundPacketStream( buffer, _osc_packet_size );
	//	_osc_buffer[i]->Clear();
	//}
}

/*
// kroonde is a special case they forgot the #bundle encapsulation
INT32 CONST	OSC_HEADERS_NB = 5;
static	C_PCHAR_C	osc_headers[OSC_HEADERS_NB] =
{
	"/kroonde\0\0\0\0",
	"/toaster\0\0\0\0",
	"/warhol\0",
	"/warhol/midi_in\0",
	"/erasme\0"
};

static	INT32	osc_headers_size[OSC_HEADERS_NB] =
{
	12,
	12,
	8,
	16,
	8
};

static	char	osc_8_int_header[] =	",iiiiiiii\0\0\0";
static	char	osc_16_int_header[] =	",iiiiiiiiiiiiiiii\0\0\0";
static	char	osc_32_int_header[] =	",iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii\0\0\0";
static	char	osc_1_str_header[] =	",s\0\0";

INT32	osc_find_header( UINT8* buf)
{
	INT32 i;
	for( i=0; i<OSC_HEADERS_NB; ++i )
	{
		if( str_is_equal( (CHAR*)buf, osc_headers[i], osc_headers_size[i] ) )
		{
			buf += osc_headers_size[i];
			if(  str_is_equal( (CHAR*)buf, osc_8_int_header, 12 )
				|| str_is_equal( (CHAR*)buf, osc_16_int_header, 20 )
				|| str_is_equal( (CHAR*)buf, osc_32_int_header, 36 )
				|| str_is_equal( (CHAR*)buf, osc_1_str_header, 8 ) )
			{
				return i;
			}
			return -1;
		}
	}
	return -1;
}
*/

/*
INT32	osc_print( UINT8* str, UINT8* buf, INT32 len)
{
UINT8*	src;
UINT8*	dst;
INT32	count;
UINT8	d;

	src = buf-1;
	dst = str-1;
	count = 0;
	for( INT32 i = len; i > 0; --i )
	{
		d = *++src;
		if( d < 32 || d > 127)
			*++dst = '.';
		else
			*++dst = d;
		++count;
	}
	*++dst = 0;
	return count;
}
*/

bool	c_net::osc_is_dst_valid( INT32 s_dst )
{
	INT32 index_max = osc::DST_NB-1;
	if( INSIDE_MIN_MAX( s_dst, 0, index_max ) )
		return true;

	ERR_PRINT_STRING( "%d is not a valid OSC destination should be between 0 and %d.", s_dst, index_max );
	return false;
}

void	c_net::osc_flush( INT32 s_dst )
{
	if( !osc_is_dst_valid( s_dst ) )
		return;

	// flush pkt buffer for OSC dst
	aaalua::osc_pkt_flush( s_dst );

	INT32	index_max;
	INT32	size;

//	osc_lock[s_dst].lock();
	index_max = _osc_buf_index[s_dst];
	index_max = MIN( index_max+1, osc::BUF_NB);
	for( INT32 index = 0; index < index_max; ++index )
	{
		size = _osc_buf_out_len[s_dst][index];
		if( size > 0 )
		{
//			osc_print( _osc_buf_out[s_dst][index], size);
			sendto( osc_dst[s_dst], 1, BLK_OSC, _osc_buf_out[s_dst][index], size );
		}
	}
	_osc_buf_index[s_dst] = 0;
	_pt_osc_buf_out[s_dst] = _osc_buf_out[s_dst][0];
	_osc_buf_out_len[s_dst][0] = 0;			
//	osc_lock[s_dst].unlock();
}

//	when the flush is called async or with a lock  //bdd_poinbt dot_... fns called async
void	c_net::osc_flush_async( INT32 s_dst )
{
	if( !osc_is_dst_valid( s_dst ) )
		return;

	_b_osc_flush_sync[s_dst] = false;	
	osc_flush( s_dst );	
}

void	c_net::osc_flush()
{
	if( !is_active() )
		return;

	for( INT32 i = 0; i < osc::DST_NB; ++i )
	{
		if( _b_osc_flush_sync[i] )
			osc_flush( i );
		else
			aaalua::osc_pkt_flush( i );
	}
}

static UINT8*	strcpy_and_align32_with_zero( UINT8* dst, UINT8 CONST * src )
{
	CHAR c;
	--src;
	--dst;
	while( (c=*++src) != 0 )
		*++dst = c;

	do
		*++dst = 0;
	while( ( (reinterpret_cast<uintptr_t>(dst)) & 0x3) != 3 );

	return ++dst;
}

//	there is now two path to send osc one direct here
//		and one using 
void	c_net::send_osc( INT32 s_dst, osc::OutboundPacketStream* pt_pkt )
{
	if( !is_active() )
		return;
	if( !osc_is_dst_valid( s_dst ) )
		return;

	c_net_link* p_link = get_link(osc_dst[s_dst]);
	if( p_link )
	{
		auto size = pt_pkt->get_size();
		if( b_verbose_osc_out_ui )
		{
			if( p_link->is_active() && p_link->is_running() )
				p_link->print_string( "OSC Packet OUT of size %d :", size );
		}
		p_link->send_raw( (UINT8*)pt_pkt->get_data(), size );
	}
	else
		ERR_PRINT_STRING( "%s() link_index %d is out of range or net_link is null", __FUNCTION__, osc_dst[s_dst] );
}

void	c_net::send_osc( INT32 s_dst, C_PCHAR_C tag, C_PCHAR_C arg, UINT8 CONST * CONST buf )
{
	if( !is_active() )
		return;	
	if( !osc_is_dst_valid( s_dst ) )
		return;

//		osc_lock[s_dst].lock();
	INT32	index = _osc_buf_index[s_dst];

	if( index < osc::BUF_NB )
	{
		UINT8*	pt_start;
		UINT8*	pt_dst;
		INT32	size;

		pt_start = _pt_osc_buf_out[s_dst];

		pt_dst = pt_start + 4;	// let space for _len of bundle
		pt_dst = strcpy_and_align32_with_zero( pt_dst, (UINT8*)tag );
		pt_dst = strcpy_and_align32_with_zero( pt_dst, (UINT8*)arg );
		pt_dst = strcpy_and_align32_with_zero( pt_dst, buf );
		*(INT32*)pt_start = htonl( INT32(pt_dst - pt_start - 4) );	//bundle start with its _len

		_pt_osc_buf_out[s_dst] = pt_dst;	//store the new position
//		osc_lock[s_dst].unlock();
		if( b_verbose_osc_out_ui )
			get_link(s_dst)->print_string( "BLK_OSC OUT STORED: %s %s %s", tag, arg, buf );

		//todo do better even generalize
		size = INT32( pt_dst - _osc_buf_out[s_dst][index] );
		_osc_buf_out_len[s_dst][index] = size;		//store the _len to send
		if( size > ( osc::BLK_SIZE_MAX - 256 ) )	//todo if we what to send a tag/arg/buf bigger than 256 we could be in trouble
		{
			_osc_buf_index[s_dst] = ++index;	//go to next buffer
			if( index < osc::BUF_NB )
			{
				_pt_osc_buf_out[s_dst] = _osc_buf_out[s_dst][index];	//have pointer ready
				_osc_buf_out_len[s_dst][index] = 0;
			}
		}
	}
	else
	{
		get_link(s_dst)->err_print( "OSC_OUT(%d) buffer overload", s_dst );
	}
}

void	c_net::send_osc_str( INT32 s_dst, C_PCHAR_C buf )
{
	send_osc( s_dst, "/", ",s", (UINT8*)buf );
}

void	c_net::send_film_on( C_PCHAR_C name, REAL time )
{
	CHAR	buf[256];
	sprintf( buf, "FILM_ON %s %f", name, time );
	send_osc_str( 0, buf );
}
void	c_net::send_film_off( C_PCHAR_C name )
{
	CHAR	buf[256];
	sprintf( buf, "FILM_OFF %s", name );
	send_osc_str( 0, buf );
}

using namespace osc;

void	print_osc_arg( C_PCHAR_C header, osc::ReceivedMessageArgument CONST & arg )
{
	CHAR	buf[512];
	C_PCHAR	str;

	switch( arg.get_type_tag() )
	{
	case TRUE_TYPE_TAG:			str = "bool	true";			break;
	case FALSE_TYPE_TAG:		str = "bool false";			break;
	case NIL_TYPE_TAG:			str = "nil";				break;
	case INFINITUM_TYPE_TAG:	str = "infinitum";			break;
	case INT32_TYPE_TAG:		str = buf;	snprintf( buf, sizeof(buf)-1, "INT32 %d",	arg.AsInt32() );		break;
	case FLOAT_TYPE_TAG:		str = buf;	snprintf( buf, sizeof(buf)-1, "FLOAT %f",	arg.AsFloat() );		break;
	case CHAR_TYPE_TAG:			str = buf;	snprintf( buf, sizeof(buf)-1, "CHAR %c",	arg.AsChar() );	break;
	case RGBA_COLOR_TYPE_TAG:	str = "RGBA";				break;
	case MIDI_MESSAGE_TYPE_TAG:	str = "MIDI";				break;
	case INT64_TYPE_TAG:		str = "INT64";				break;	//AsInt64
	case TIME_TAG_TYPE_TAG:		str = "TIME_TAG";			break;
	case DOUBLE_TYPE_TAG:		str = "DOUBLE";				break;
	case STRING_TYPE_TAG:		str = buf;	snprintf( buf, sizeof(buf)-1, "STRING %.480s", arg.AsString() );	break;
	case SYMBOL_TYPE_TAG:		str = buf;	snprintf( buf, sizeof(buf)-1, "SYMBOL %.480s", arg.AsSymbol() );	break;
	case BLOB_TYPE_TAG:			str = "BLOB";				break;
	default:					str = "OSC_UNKNOWN_TAG";	break;
	}
	VERBOSE_PRINT_STRING( "%s %s", header, str );
}

CHAR* sprint_osc_arg( CHAR* dst, osc::ReceivedMessageArgument CONST & arg )
{
	C_PCHAR	str = nullptr;
	switch( arg.get_type_tag() )
	{
		case TRUE_TYPE_TAG:			str = "TRUE";				break;
		case FALSE_TYPE_TAG:		str = "FALSE";				break;
		case NIL_TYPE_TAG:			str = "nil";				break;
		case INFINITUM_TYPE_TAG:	str = "infinitum";			break;
		case INT32_TYPE_TAG:		dst += sprintf( dst, " %d", arg.AsInt32() );	break;
		case FLOAT_TYPE_TAG:		dst += sprintf( dst, " %f", arg.AsFloat() );	break;
		case CHAR_TYPE_TAG:			dst += sprintf( dst, " %c", arg.AsChar() );		break;
		case RGBA_COLOR_TYPE_TAG:	str = "RGBA";				break;
		case MIDI_MESSAGE_TYPE_TAG:	str = "MIDI";				break;
		case INT64_TYPE_TAG:		str = "INT64";				break;	//AsInt64
		case TIME_TAG_TYPE_TAG:		str = "TIME_TAG";			break;
		case DOUBLE_TYPE_TAG:		str = "DOUBLE";				break;
		case STRING_TYPE_TAG:		dst += sprintf( dst, " %.480s", arg.AsString() );	break;
		case SYMBOL_TYPE_TAG:		dst += sprintf( dst, " %.480s", arg.AsSymbol() );	break;
		case BLOB_TYPE_TAG:			str = "BLOB";				break;
		default:					str = "OSC_UNKNOWN_TAG";	break;
	}
	if( str )
		dst += sprintf( dst, " %.480s", str );
	return dst;
}

CONST	INT32	PRINT_LEN = 4 * 1024;
static	CHAR	print_buf[ PRINT_LEN ];
static	CHAR*	print_buf_end = print_buf + PRINT_LEN - 512;	//secu

void	c_net::osc_print_message( C_PCHAR_C header, osc::ReceivedMessage CONST & msg )
{
	ReceivedMessageArgumentIterator it = msg.ArgumentsBegin();
	ReceivedMessageArgumentIterator it_end = msg.ArgumentsEnd();
	if( b_verbose_osc_detailed_ui )
	{
		VERBOSE_PRINT_STRING( "%s %s", header, msg.AddressPattern() );
		while( it != it_end )
		{
			print_osc_arg( header, *it );
			++it;
		}
	}
	else
	{
		CHAR* pt = print_buf + sprintf( print_buf, "%s", msg.AddressPattern() );
		while( it != it_end && pt < print_buf_end )
		{
			pt = sprint_osc_arg( pt, *it );
			++it;
		}
		VERBOSE_PRINT_STRING( "%s %s", header, print_buf );
	}
}

void	c_net::osc_print_bundle( C_PCHAR_C header, osc::ReceivedBundle CONST & b )
{
	// ignore bundle time tag for now
	for( ReceivedBundle::const_iterator it = b.ElementsBegin(); it != b.ElementsEnd(); ++it )
	{
		if( it->is_bundle() )
			osc_print_bundle( header, ReceivedBundle(*it) );
		else					
			osc_print_message( header, ReceivedMessage(*it) );
	}
}

void	c_net::osc_print( C_PCHAR_C header, UINT8 CONST * CONST pt, INT32 size )
{
	osc::ReceivedPacket p( (char*)pt, size );
	if( p.is_bundle() )
		osc_print_bundle( header, ReceivedBundle(p) );	//ProcessBundle( ReceivedBundle(p) );
	else
		osc_print_message( header, ReceivedMessage(p) );//ProcessMessage( ReceivedMessage(p) );
}

void	c_net::osc_process_message( osc::ReceivedMessage CONST & msg )
{
	if( b_verbose_osc_processed_ui )
		osc_print_message( "OSC Process :", msg );

	switch( osc_taking )
	{
	case OSC_TAKING_MIDI:
		c_midi::osc_process_message( msg );
		break;
	case OSC_TAKING_VARIABLE_SET:
		aaa::osc_process_message_variable_set( msg );
		break;
	case OSC_TAKING_LUA:
		aaalua::osc_process_message( msg );
		break;
	default:
		break;
	}

	//} catch( Exception& e ){
	//	std::cout << "error while parsing message: "<< msg.AddressPattern() << ": " << e.what() << "\n";
	//}
}

/*
void	c_net::osc_print_bundle( CONST osc::ReceivedBundle& b )
{
	for( osc::ReceivedBundle::const_iterator it = b.ElementsBegin(); it != b.ElementsEnd(); ++it )
		osc_print_message( (CONST osc::ReceivedMessage&)(*it) );
}

void	c_net::osc_process_bundle( CONST osc::ReceivedBundle& b )
{
	for( osc::ReceivedBundle::const_iterator it = b.ElementsBegin(); it != b.ElementsEnd(); ++it )
		osc_process_message( (CONST osc::ReceivedMessage&)(*it) );
//		osc_process( (CONST osc::ReceivedPacket&)(*it) );
}
*/

/*
void	c_net::receive_osc( UINT8* pt, INT32 len )
{
INT32	i;
	if( b_verbose_osc_in )
		{
		UINT8	str[128];
		UINT8*	p;
		INT32	l;

		NET_OSC_PRINT_STRING( "IN BLK_OSC %d : ", len );
		p = pt;
		l = len;
		while( l > 64 )
			{
			osc_print( str, p, 64 );
			NET_OSC_PRINT_STRING( "--\t%s", str );
			l -= 64;
			p += 64;
			}
		osc_print( str, p, l);
		NET_OSC_PRINT_STRING( "end\t%s", str );
		}

	i = osc_find_header(pt);
	if( i>=0 )
		{
		if( g_toaster_cur )
			{
			INT32 nb;

			pt += osc_headers_size[i];
			nb = 0;
			if( str_is_equal( (CHAR*)pt, osc_16_int_header, 20) )
				{
				nb = 16;
				pt += 20;
				}
			else if( str_is_equal( (CHAR*)pt, osc_8_int_header, 12)  )
				{
				nb = 8;
				pt += 12;
				}
			i = 0;
			while( ++i <= nb )
				{
				INT32	val;

				val = ntohl(*(INT32*)pt);
				pt += sizeof(INT32);

				g_toaster_cur->set_data_in( i-1, REAL(val) );
//				if( b_verbose_ )
//					NET_OSC_PRINT_STRING( "Toaster Control %d: value %d", i, val);
				}
			}
		}
}


REAL	osc_get_float( UINT8* data)
{
	UINT32	i = ntohl(*(INT32*)data);
	float f = *(float*)&i;
	return REAL(f);
}

static INT32 osc_slice_nb = 0;
static UINT8 slice_float[2][8];

void	osc_change_slice()
{
CHAR	str[256];
	sprintf( str, "Slice%d", osc_slice_nb+1 );
	net->send_osc( 2, "/aaa/lemur/slice/x", ",f", slice_float[0] );
	net->send_osc( 2, "/aaa/lemur/slice/y", ",f", slice_float[1] );
	net->send_osc( 2, "/interface", ",s", (UINT8*)str );
	net->osc_flush( 2);
	net->send_osc( 2, "/aaa/lemur/slice/x", ",f", slice_float[0] );
	net->send_osc( 2, "/aaa/lemur/slice/y", ",f", slice_float[1] );
	net->osc_flush( 2 );
}

bool	c_net::receive_osc( UINT8* tag_in, UINT8* arg_in, UINT8* data )
{
	CHAR*	tag = (CHAR*)tag_in;
	CHAR*	arg = (CHAR*)arg_in;
	bool	b_used = false;
	if( str_is_equal( tag, "/aaa/", 5 ) )
	{
		tag += 5;
		if( str_is_equal( tag, "lemur/", 6 ) )
		{
			tag += 6;
			if( str_is_equal( tag, "inter/", 6 ) )
			{
				tag += 6;
				send_osc( 2, "/interface", ",s", (UINT8*)tag );
				osc_flush( 2);
				b_used = true;
			}
			else if( str_is_equal( tag, "slice/", 6 ) )
			{
				tag += 6;
				if( str_is_equal( tag, "x", 1 ) )
				{
					REAL f = osc_get_float(data)*3.0000003;
					INT32 i = f;
					osc_slice_nb = osc_slice_nb& ~0x3 | i;
					MEMCPY( slice_float[0], data, 8);
					osc_change_slice();
					b_used = true;
				}
				else if( str_is_equal( tag, "y", 1 ) )
				{
					REAL f = osc_get_float(data)*3.0000003;
					INT32 i = f;
					osc_slice_nb = osc_slice_nb& ~0xc | ((3-i)<<2);
					MEMCPY( slice_float[1], data, 8);
					osc_change_slice();
					b_used = true;
				}
				else if( str_is_equal( tag, "z", 1 ) )
				{
//					REAL f = osc_get_float(data);
//					{
//						CHAR	str[256];
//						sprintf( str, "Slice%d", osc_slice_nb+1 );
//						send_osc( 2, "/interface", ",s", (UINT8*)str );
//						osc_flush( 2);
//						if( f==0. )
//						{
//						}
//					}
					b_used = true;					}
			}
			else if( str_is_equal( tag, "midi/", 5 ) )
			{
				tag += 5;
				INT32 ch;
				INT32 ctl;
				INT32 nb = sscanf( tag, "%d/%d", &ch, &ctl );
				if( nb==2 && *arg==',')
				{
					while( *++arg=='f' )
					{
						REAL val = osc_get_float(data);
						c_midi::set_control( ch, ctl, val );
						++ctl;
						data += 4;
					}
				}
			}
		}
	}
	else if( str_is_equal( tag, osc::tuio_header, 12 ) )
	{
		// TUIO message
		tag += 12;
	}
	return b_used;
}

void	c_net::receive_osc_str( UINT8* str )
{
	if( b_verbose_osc_in)
		NET_OSC_PRINT_STRING( "IN OSC_STR : %s", str );
	if( bdd_point_cur && str_is_equal( (CHAR*)str, "DOT", 3 ) )
	{
		bdd_point_cur->receive_osc( (CHAR*)str );
	}
}

void	c_net::receive_osc_bundle( UINT8* buf, INT32 len )
{
	UINT8*	tag = buf;
	UINT8*	arg;
	UINT8*	data;
	// get tag
	buf = osc_next_field( buf, len);
	len -= buf-tag;
	if( len >0 )	// cheap check
	{
		arg = buf;
		//get arg
		buf = osc_next_field( buf, len);
		len -= buf-arg;
		if( len >0 )	// cheap check
		{
			data = buf;
			if( b_verbose_osc_in)
				{
				if( strcmp((CHAR*)arg,",s") == 0)
					NET_OSC_PRINT_STRING( "IN OSC : %s %s %s", tag, arg, data );
				else
					NET_OSC_PRINT_STRING( "IN OSC : %s %s", tag, arg );
				}
			if( !receive_osc( tag, arg, data ) )
				if( strcmp( (CHAR*) arg, osc_1_str_header )==0 )
					receive_osc_str( data );
		}
	}
}

void	c_net::receive_osc_new( UINT8* buf, INT32 len )
{
	UINT8*	end = buf+len;
	while( buf < end )
	{
	INT32	len_bundle = ntohl( *(UINT32*)buf );
		buf += 4;
		net->receive_osc_bundle( buf, len_bundle );
		buf += len_bundle;
	}
}
*/

