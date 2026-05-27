#include "net_lua.h"
#include "net_blk.h"
#include "net_link.h"
#include "language/lua/aaalua_glue.h"
#include "math.h"

#include "osc/OscOutboundPacketStream.h"
#ifndef AAA_INCLUDED_OSCRECEIVEDELEMENTS_H
#	include "obj_ui/com/osc/OscReceivedElements.h"
#endif
#ifndef AAA_INCLUDED_OSCTYPES_H
#	include "obj_ui/com/osc/Osctypes.h"
#endif
#include "net_requests/net_requests_lua.h"

namespace aaalua
{
	CONSTEXPR	INT32	OSC_BLK_SIZE_CUSTOM = 8192;		// are we in the standard ? Should match udp packet size we sent
//	char						buffer_for_size[ OSC_BLK_SIZE_MAX ];
//	osc::OutboundPacketStream	osc_pkt_for_size( buffer_for_size, OSC_BLK_SIZE_MAX );

	// OSC out buffer for each destination
	CHAR*						osc_buffer[   osc::DST_NB ];
	osc::OutboundPacketStream*	osc_out_pkt[  osc::DST_NB ];
	o_str						osc_tag_last[ osc::DST_NB ];
	//aaa::MUTEX				osc_lua_lock;

	void osc_pkt_flush( INT32 osc_index )
	{
	//	osc_lua_lock.lock();
		osc::OutboundPacketStream * pkt = osc_out_pkt[ osc_index ];
		if( pkt->is_bundle_in_progress() )
		{
			*pkt << osc::EndBundle;
			if( pkt->is_ready() )
				net->send_osc( osc_index, pkt );
			pkt->clear();
		}
	//	osc_lua_lock.unlock();
	}

namespace n_net
{
/*
	//send_osc( s_dst, "/", ",s", buf );
	//	input	dst tag arg str
	static	INT32	osc_send_old( lua_State* L )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 4 );
		INT32			osc_index		= l.get_int32(1);
		C_PCHAR_C	tag		= l.get_str(2);
		C_PCHAR_C	arg		= l.get_str(3); 
		C_PCHAR_C	str		= l.get_str(4);
		net->send_osc( osc_index, (CHAR*)tag, (CHAR*)arg, (UINT8*)str );
		return l.return_nothing();
	}
*/
	void	osc_lua_deinit()
	{
		for( size_t i = 0; i < osc::DST_NB; i ++ )
		{
			SAFE_DELETE( osc_out_pkt[i] );
			SAFE_DELETE( osc_buffer[i] );
		}
	}

	void	osc_lua_init()
	{
		for( size_t i = 0; i < osc::DST_NB; ++i )
		{
			osc_buffer[i] = new CHAR[ OSC_BLK_SIZE_CUSTOM ];
			osc_out_pkt[i] = new osc::OutboundPacketStream( osc_buffer[i], OSC_BLK_SIZE_CUSTOM );
		}
	}

	// avoid dynamic allocation
	thread_local	CHAR                      buffer[ OSC_BLK_SIZE_CUSTOM ];
	thread_local	osc::OutboundPacketStream packet( buffer, OSC_BLK_SIZE_CUSTOM );
	//send_osc( s_dst, "/", ... );
	//	input	dst tag args
	INT32 osc_send_common( c_lua_state& l, bool b_bundle )
	{
		INT32       nb_arg    = l.get_arg_nb_min( 2 );
		INT32 CONST	osc_index = l.get_int32(1);
		C_PCHAR_C   tag       = l.get_str(2);

		if( net->osc_is_dst_valid(osc_index) )
		{
			// this is dynamic memory allocation AAASeed avoid this
			//	CHAR                      buffer[ OSC_BLK_SIZE_CUSTOM ];
			//	osc::OutboundPacketStream packet( buffer, OSC_BLK_SIZE_CUSTOM );

			
			//= *osc_out_pkt[ osc_index ];
			osc::OutboundPacketStream * pt_pkt;

			INT32 index = 3;
			nb_arg -= 2;
			if( b_bundle )
			{
				INT32 message_size = osc::OutboundPacketStream::get_message_space( tag );
				INT32 nb_arg_b = nb_arg;
				while( nb_arg_b-- )
				{
					INT32	type = l.get_type( index );
					if( l.is_type_number(type) )
					{
						REAL r = l.get_real(index);
						//signed	long	i = floor(r);
						//if( REAL(i)==r )
						if( floor(r)==r )
							message_size += osc::OutboundPacketStream::get_int_space();
						else
							message_size += osc::OutboundPacketStream::get_real_space();
					}
					else if( l.is_type_string(type) )
						message_size += osc::OutboundPacketStream::get_string_space( l.get_str(index) );
					else if( l.is_type_bool(type) )
						message_size += osc::OutboundPacketStream::get_bool_space();
					else if( l.is_type_nil(type) )
						message_size += osc::OutboundPacketStream::get_nil_space();
					else
						message_size += osc::OutboundPacketStream::get_nil_space();
					++index;
				}
				pt_pkt = osc_out_pkt[ osc_index ];
				if( pt_pkt->is_bundle_in_progress() )
				{
					if( !osc_tag_last[ osc_index ].is_str_equal( tag ) )
					{
						// tag not equal, flush existing packet, and create a new one
						aaalua::osc_pkt_flush( osc_index );
					}
					else
					{
						INT32 capacity_left = pt_pkt->get_capacity() - pt_pkt->get_size() - 20;	// make sure there is enough space in buffer to end Bundle.
						message_size += osc::OutboundPacketStream::get_argument_space_size( nb_arg );
						if( message_size > capacity_left )
						{
							aaalua::osc_pkt_flush( osc_index );
						}
					}
				}
				if( !pt_pkt->is_bundle_in_progress() )
				{
					*pt_pkt << osc::BeginBundle();
					osc_tag_last[ osc_index ].set( tag );
				}
			}
			else
			{
				packet.clear();
				pt_pkt = &packet;
			}
			*pt_pkt << osc::BeginMessage( tag );
			index = 3;
			while( nb_arg-- )
			{
				//todo this an imperfect test
				// this depend on the argument
				if( !pt_pkt->check_for_available_argument_space(8) )
				{
					LUA_ERR_PRINT_STRING( "osc_send() no more space in OutboundPacketStream at argument index %d", index );
					break;
				}
				INT32	type = l.get_type( index );
				if( l.is_type_number( type ) )
				{
					REAL		r = l.get_real(index);
					//signed	long	i = floor(r);
					//if( REAL(i)==r )
					REAL		i = floor(r);
					if( i==r )
						*pt_pkt << INT32(i);
					else
						*pt_pkt << r;
				}
				else if( l.is_type_string( type ) )
					*pt_pkt << l.get_str(index);
				else if( l.is_type_bool(type) )
					*pt_pkt << l.get_bool(index);
				else if( l.is_type_nil( type ) )
					*pt_pkt << osc::Nil;
				else
				{
					*pt_pkt << osc::Nil;
					LUA_ERR_PRINT_STRING( "osc_send() don't deal with argument of type %s, pushing nil instead", c_lua_state::get_type_str(type) );
				}
				++index;
			}
			*pt_pkt << osc::EndMessage;

			if( !b_bundle )
			{
				if( pt_pkt->is_ready() )
					net->send_osc( osc_index, pt_pkt );
			}
		}
		return l.return_nothing();
	}
	//send_osc( s_dst, "/", ... );
	//	input	dst tag args

	AAALUACALL( osc_send )				{	LUAAAA_START( L, __FUNCTION__ );	return osc_send_common( l, true );	}
	AAALUACALL( osc_send_no_bundle )	{	LUAAAA_START( L, __FUNCTION__ );	return osc_send_common( l, false );		}


	AAALUACALL( osc_flush )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		INT32	osc_index	= l.get_int32(1);
		net->osc_flush( osc_index );
		return l.return_nothing();
	}

	INT32	process_osc_message( c_lua_state& l, osc::ReceivedMessage msg, INT32 index )
	{
		if( !msg.is_valid() )
			return 0;

		o_str	addr( msg.AddressPattern() );
		//	l.push_string( addr );

		INT32	i = 0;
		l.push_int( index );

		l.new_table();
	//	l.push_int( 1 );

		// tags parse and accumulate 
		l.push_string( "tags", 4 );
		l.new_table();
		CHAR*	pt = (CHAR*) addr.get() + 1;	//need cast we change content
		CHAR*	mark = pt;
		for( ; ; )
		{
			if( *pt=='/' || *pt == 0 )
			{
				bool b_done = (*pt == 0);
				*pt = 0;
				if( *mark )
					l.set_field( ++i, mark );
				if( b_done )
					break;
				++pt;
				mark = pt;
			}
			else
				++pt;
		}
		l.set_table( -3 );

		// args parse and accumulate
		//osc::ReceivedMessageArgumentStream		args	= msg.ArgumentStream();
		i = 0;
		l.push_string( "args", 4 );
		//l.push_int( 2 );
		l.new_table();
		osc::ReceivedMessageArgumentIterator	it		= msg.ArgumentsBegin();
		osc::ReceivedMessageArgumentIterator	it_end	= msg.ArgumentsEnd();
		//while( !args.Eos() )

		while( it != it_end )
		{
			CONST osc::ReceivedMessageArgument* arg = &(*it);
			switch( arg->get_type_tag() )
			{
			case osc::TRUE_TYPE_TAG:			l.set_field_bool(	++i, true );					break;
			case osc::FALSE_TYPE_TAG:			l.set_field_bool(	++i, false );					break;
			case osc::NIL_TYPE_TAG:				l.set_field_nil(	++i );							break;
			case osc::INFINITUM_TYPE_TAG:		l.set_field(		++i, aaa::BIGGEST<DOUBLE> );	break;	//todo check it in lua
			case osc::INT32_TYPE_TAG:			l.set_field(		++i, arg->AsInt32() );			break;
			case osc::FLOAT_TYPE_TAG:			l.set_field(		++i, arg->AsFloat() );			break;
			case osc::CHAR_TYPE_TAG:			l.set_field_char(	++i, arg->AsChar() );			break;
			case osc::RGBA_COLOR_TYPE_TAG:		l.set_field(		++i, arg->AsRgbaColor() );		break;	//todo extend	return a table ?
			case osc::MIDI_MESSAGE_TYPE_TAG:	l.set_field(		++i, arg->AsMidiMessage() );	break;	//todo extend	return a table ?
			case osc::INT64_TYPE_TAG:			l.set_field(		++i, arg->AsInt64() );			break;
			case osc::TIME_TAG_TYPE_TAG:		l.set_field(		++i, arg->AsTimeTag() );		break;	//todo send
			case osc::DOUBLE_TYPE_TAG:			l.set_field(		++i, arg->AsDouble() );			break;
			case osc::STRING_TYPE_TAG:			l.set_field(		++i, arg->AsString() );			break;
			case osc::SYMBOL_TYPE_TAG:			l.set_field(		++i, arg->AsSymbol() );			break;
			case osc::BLOB_TYPE_TAG:			l.set_field_nil(	++i );							break;	//todo extend
			default:							l.set_field_nil(	++i );							break;
			}
			++it;
		}

		if( i == 0 )	// if no args replace table by a nil
		{
			l.pop( 1 );
			l.push_nil();
		}	
		l.set_table( -3 );
		l.set_table( -3 );

		return 1;
	}

	AAALUACALL( osc_take_by_start )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg = l.get_arg_nb( 0, 1 );

		C_PCHAR_C selector = nb_arg==1 ? l.get_str() : nullptr;
		c_net_blk*	blk = net->osc_blk_take_by_start( selector );
		if( !blk )
			return l.return_nothing();
	
		l.new_table();
		INT32	index = 1;
		do
		{
			osc::ReceivedPacket packet( (CONST CHAR *) blk->get_data_pt(), blk->get_len() );			
			if( packet.is_bundle() )
			{
				osc::ReceivedBundle bundle = osc::ReceivedBundle(packet);
				for( osc::ReceivedBundle::const_iterator it = bundle.ElementsBegin(); it != bundle.ElementsEnd(); ++it )
				{
					if( it->is_bundle() )
						LUA_ERR_PRINT_STRING( "this is bundle in Bundle Osc, can't treat this for now" );
					else
						index += process_osc_message( l, osc::ReceivedMessage(*it), index );
				}
			}
			else
				index += process_osc_message( l, osc::ReceivedMessage(packet), index );
	
			net->blk_free( blk );
		}
		while( blk = net->osc_blk_take_by_start( selector ) );

		return l.return_table();
	}

	//	input	destination channel name [name...]
	AAALUACALL( lua_send )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg		= l.get_arg_nb_min( 3 );
		INT32	CONST	link_index	= l.get_int32( 1 );
		INT32	CONST	ch			= l.get_int32( 2 );

		for( INT32 i = 1; i <= ( nb_arg - 2 ); ++i )
		{
			C_PCHAR_C text = l.get_str( i + 2 );
			aaalua::net_send( link_index, ch, text );
		}
		return l.return_nothing();
	}
	//	input	destination channel name [name...]
	AAALUACALL( send_text8 )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg		= l.get_arg_nb_min( 2 );
		INT32	CONST	osc_index	= l.get_int32( 1 );

		for( INT32 i = 1; i <= ( nb_arg - 1 ); ++i )
		{
			C_PCHAR_C text = l.get_str( i + 1 );
			net->send_text8( osc_index, text );
		}
		return l.return_nothing();
	}
	//	aaa.net.send_magic_packet(	link_index, mac_raw )
	AAALUACALL( send_magic_packet )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 2 );
		INT32	CONST	link_index	= l.get_int32( -2 );
		C_PCHAR_C		mac_raw		= l.get_str();
//		CHAR*	mac_stop;
		UINT8	magic_packet[ 17 * 6 ];
		UINT8	mac_address[ 6 ];
		C_PCHAR	mac_start = mac_raw;

		for( UINT32 i = 0; i < 6; ++i )
		{
			mac_address[ i ] = 0;
			if ( (*mac_start) != 0 )
			{
				mac_address[ i ] = INT8( strtol( mac_start, nullptr, 16 ) );
				mac_start += 3;
			}
		}
		for( UINT32 i = 0; i < 6; ++i )
		{
			magic_packet[ i ] = 0xFF;
		}
		for( UINT32 i = 1; i < 17; ++i )
		{
			for( UINT32 j = 0; j < 6; ++j )
			{
				magic_packet[ i * 6 + j ] = mac_address[ j ];
			}
		}
		net->send_raw( link_index, (CHAR*)magic_packet, 17 * 6 );
		//net->sendto( link_index, 0, BLK_ASCII, magic_packet, 17 * 6 );
		return l.return_nothing();
	}

	AAALUACALL( restart )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		INT32	link_index = l.get_int32( 1 );
		c_net_link* p_link = net->get_link( link_index );
		p_link->restart();
		return l.return_nothing();
	}

	void	register_net( lua_State* L )
	{
		LUAAAA_START( L, __FUNCTION__ );

		l.define_table( "net" );
			ADD_FN( osc_send );
			ADD_FN( osc_send_no_bundle );
			ADD_FN( osc_flush );
			ADD_FN( osc_take_by_start );
			ADD_FN( lua_send );
			ADD_FN( send_text8 );
			ADD_FN( send_magic_packet );
			ADD_FN( restart );

			n_net_requests::register_net_requests(L);
		lua_pop( L, 1 );	//pop table "net"

		osc_lua_init();
	}
	void	unregister_net( lua_State* L )
	{
		//LUAAAA_START( L, __FUNCTION__ );
		//todo remove table
		osc_lua_deinit();
	}
}	//end namespace n_net
}	//end namespace aaalua
