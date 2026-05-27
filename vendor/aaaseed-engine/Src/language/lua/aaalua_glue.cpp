#include "aaa_def.h"
#include "aaalua_glue.h"
#include "aaalua_wrap.h"

#include "obj_ui/com/net_lua.h"
#include "obj_ui/com/net.h"
#ifndef AAA_INCLUDED_OSCRECEIVEDELEMENTS_H
#	include "obj_ui/com/osc/OscReceivedElements.h"
#endif

#include "infrastructure/bind/bind.h"

#include "infrastructure/layer/app.h"
#include "infrastructure/layer/modules.h"

#include "draw/multiple.h"

#include "aaalua_master.h"

#include "ui/dialog.h"
#include "infrastructure/aaa_window.h"

#include "action.h"
#include "spy.h"

#include "infrastructure/data/datagrid.h"
#include "draw/mat.h"
#include "media/video/aaa_video.h"

#include "tinyxml/tinyxml.h"

#include "infrastructure/seed_stop.h"
#include "aaa/execute.h"

#include "license/info.h"

#include "infrastructure/layer/layer_lua.h"
#include "infrastructure/layer/layers_lua.h"
#include "infrastructure/layer/module_lua.h"
#include "infrastructure/layer/modules_lua.h"
#include "FBO/fbo_lua.h"
#include "infrastructure/obj/obj_lua.h"
#include "infrastructure/param/param_lua.h"
#include "aaalua_draw.h"
#include "gol/gol_lua.h"
#include "code_utils/file/aaa_file_lua.h"
#include "image/convert/color_space_lua.h"
#include "draw/lights_lua.h"
#include "draw/mat_lua.h"
#include "draw/camera_lua.h"
#include "code_utils/file/aaa_dir_lua.h"
#include "obj_ui/bdd/lua/bdd_lua.h"
#include "shaders/shading_lua.h"
#include "obj_ui/com/midi_lua.h"
#include "math/math_lua.h"
#include "obj_ui/deformer/deformer_lua.h"
#include "aaalua_array.h"
#include "aaalua_exp.h"
#include "image/img_lua.h"
#include "ui/menu_lua.h"
#include "spy_lua.h"
#include "ui/ui_lua.h"
#include "boids/poid.h"
#include "infrastructure/watchdog.h"
#include "infrastructure/tos_lua.h"
#include "ui/flatland_lua.h"
#include "thread/scheduler.h"

#include "infrastructure/param/param_focus.h"

#include "time/aaa_time.h"
#include "truetype/ourtrueType.h"
#include "infrastructure/aaa_env.h"

extern	c_fifo	fifo_def;

namespace aaalua
{
	bool	b_call_debug_ui;
	bool	b_call_store_ui;
	bool	b_call_print_ui;
	FP32	g_fp32[14];


	void	net_send(			INT32 link_dst, INT32 ch,	C_PCHAR_C str )
	{
		net->sendto( link_dst, ch, c_net::BLK_LUA,			(UINT8*)str, (INT32) strlen(str)+1 );
	}
	void	watchdog_net_send(	INT32 link_dst,				C_PCHAR_C str )
	{
		net->sendto( link_dst, 0, c_net::BLK_LUA_WATCHDOG, (UINT8*)str, (INT32) strlen(str)+1 );
	}
	void	net_process(									C_PCHAR_C str )
	{
		g_lua_wrap_net->do_str( str, "net" );
	}

	void	osc_process_message( CONST osc::ReceivedMessage& msg )
	{
		osc::ReceivedMessageArgumentStream		args = msg.ArgumentStream();

		C_PCHAR str;
		bool b_verbose = net->is_osc_lua_verbose();
		while( !args.Eos() )
		{
			args >> str;
			if( b_verbose )
			{
				VERBOSE_PRINT_STRING( "OSC receive lua script and will execute:");
				VERBOSE_PRINT_STRING( "%.480s", str ) ;
			}
			g_lua_wrap_net->do_str( str, "osc" );
		}
		//args >> osc::EndMessage;
	}

	//	table.create( array_elt_nb, hash_elt_nb )
	AAALUACALL(create)
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32 CONST nb_arg = l.get_arg_nb_min_max( 1, 2 );
		lua_createtable( L, l.get_int32(1), (nb_arg>1) ? l.get_int32(2) : 0  );
		return l.return_table();
	}

	//	input obj, class_name
	AAALUACALL(	find_in_bind )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 2 );
		CONST c_bind* CONST	bind	= l.get_aaa_obj_of_class< c_bind > ( -2 );
		C_PCHAR_C			name	= l.get_str();

		INT32 index = bind->find( name );
		if( index>=0 )
			return l.return_int32( index );
	//	if( b_error )
	//		l.error_and_escape( "can't find object up of class %.128s", name );
		return l.return_nothing();
	}

	AAALUACALL(	wipe_focus )
	{
		LUAAAA_START( L, __FUNCTION__ );
		fifo_def.clear();
		g_app->set_focus();
		c_modules::get_ui()->set_focus();
		return l.return_nothing();
	}
		
	//	no input
	AAALUACALL(	get_multiple_cur )
	{
		LUAAAA_START( L, __FUNCTION__ );
		return l.return_obj_ref( c_multiple::cur, "no current multiple" );
	}

	//	no input
	AAALUACALL(	get_caller )
	{
		LUAAAA_START( L, __FUNCTION__ );
		return l.return_obj_ref( c_lua_wrap::get_caller(), "no caller object" );
	}

	//	input	name value
	AAALUACALL(	set )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 2 );
		C_PCHAR_C	name	= l.get_str( -2 );
		REAL CONST	val		= l.get_real();

		aaa::vars_double.set( name, val );
		return l.return_nothing();
	}

	//	input	name
	AAALUACALL(	get )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		C_PCHAR_C name	= l.get_str();

		REAL	val;
		bool	b = aaa::vars_double.get( name, val );
		return	b ? l.return_real( val ) : l.return_nothing();
	}

	typedef	void	a_print_fn( C_PCHAR_C fmt, ... );

	void	print_low( a_print_fn& fn, c_lua_state& l )
	{
		INT32 CONST	nb_arg = l.get_arg_nb();

		//SET_COLOR_NO_MEDIA();
		if( nb_arg==0 )
		{
			fn( "print Nil ?" );
		}
		else
		{
			for( INT32 i = 1; i <= nb_arg; ++i )
			{
				C_PCHAR_C text = l.get_str( i );
				if( text )
				{
					fn( "%s", text );
				}
				else
				{
					INT32 type = l.get_type( i );
					if( c_lua_state::is_type_bool(type) )
					{
						bool b = l.get_bool(i);
						fn( b ? "true" : "false" );
					}
					else
						l.error_message( "aaa.print() don't know how to print %s type", l.get_type_str( type ) );
				}
			}
		}
		//SET_COLOR_NORMAL();
	}

	//	input	name [name...]
	AAALUACALL(	print )
	{
		LUAAAA_START( L, __FUNCTION__ );
		print_low( LUA_GOOD_PRINT_STRING, l );
		if( g_lua_master->is_print_trig_error() )
			l.error_and_escape( "aaa.print() do error for debug" );
		return l.return_nothing();
	}

	AAALUACALL(	print_inverse )
	{
		LUAAAA_START( L, __FUNCTION__ );
		print_low( LUA_GOOD_PRINT_STRING_INV, l );
		if( g_lua_master->is_print_trig_error() )
			l.error_and_escape( "aaa.print_inverse() do error for debug" );
		return l.return_nothing();
	}

	AAALUACALL(	print_debug )
	{
		if( g_lua_master->is_verbose_debug() )
		{
			LUAAAA_START( L, __FUNCTION__ );
			print_low( LUA_DBG_PRINT_STRING, l );
			return l.return_nothing();
		}
		return 0;
	}

	AAALUACALL(	bell )
	{
		BELL();
		return 0;
	}

	//	input	name [name...]
	AAALUACALL(	print_error )
	{
		LUAAAA_START( L, __FUNCTION__ );
		print_low( LUA_ERR_PRINT_STRING, l );
		return l.return_nothing();
	}

	//experimental only
	//	input	name [name...]
	AAALUACALL(	trace_error )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32 CONST	nb_arg = l.get_arg_nb_min( 1 );

		for( INT32 i = 1; i <= nb_arg; ++i )
		{
			C_PCHAR_C text = l.get_str( i );
			l.error_message( text );
		}
		return l.return_nothing();
	}

	//	BOX
	typedef void BOX_FN(	C_PCHAR_C str );
	typedef void PRINT_FN(	C_PCHAR fmt, ... );

	INT32 do_box( c_lua_state& l, BOX_FN* box_fn, PRINT_FN* print_fn, C_PCHAR_C fn_name )
	{
		INT32 CONST	nb_arg = l.get_arg_nb();

		o_str o(512);
		if( nb_arg == 0 )
		{
			o.set( fn_name );
			o.add( " called with no argument." );
		}
		else
		{
			for( INT32 i = 1; i <= nb_arg; ++i )
			{
				C_PCHAR_C str = l.get_str( i );
				(*print_fn)( "%s", str );
				o.add( str );
				if( i!=nb_arg )
					o.add_char( '\n' );
			}
		}
		l.pop_all();	//avoid message on enforcing empty stack when calling EVENT.hook
		(*box_fn)( o.get() );
		return l.return_nothing();
	}

	AAALUACALL(	box_error )		{	LUAAAA_START( L, __FUNCTION__ );	return do_box( l, BOX_ERR_FOR_LUA,	ERR_PRINT_STRING,		__FUNCTION__	);	}
	AAALUACALL(	box_warning )	{	LUAAAA_START( L, __FUNCTION__ );	return do_box( l, BOX_WAR_FOR_LUA,	WARNING_PRINT_STRING,	__FUNCTION__	);	}
	AAALUACALL(	box_good )		{	LUAAAA_START( L, __FUNCTION__ );	return do_box( l, BOX_GOOD_FOR_LUA,	GOOD_PRINT_STRING,		__FUNCTION__	);	}

	INT32 do_box_lua( c_lua_state& l, PRINT_FN* print_fn )
	{
		// find file and line then trig file edit 
		INT32 CONST	nb_arg = l.get_arg_nb();
		INT32 type = l.get_type(1);
		o_str o;
		if( type == LUA_TSTRING )
			o.set( l.get_str(1) );
		else
		{
			o.set( "expected err_str is of type " );
			o.add( l.get_str_from_type(type) );
			if( type != LUA_TNIL )
			{
				o.add( l.get_str(1) );
				o.add( "\n" );
			}
		}
		o.add( "\n" );
		for( INT32 i = 2; i <= nb_arg; ++i )
		{
			type = l.get_type(i);
			if( type != LUA_TNIL )
			{
				C_PCHAR_C str = l.get_str( i );
				(*print_fn)( "%s", str );
				o.add( str );
			}
			o.add_char( '\n' );
		}
		l.pop_all();	//avoid message on enforcing empty stack when calling EVENT.hook
		BOX_LUA( "Lua error", o.get() );
		return l.return_nothing();
	}
	AAALUACALL(	box_error_lua )		{	LUAAAA_START( L, __FUNCTION__ );	return do_box_lua( l, ERR_PRINT_STRING		);	}

	//	input	title, text
	AAALUACALL(	box_ask )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 2 );	

		C_PCHAR_C	title	= l.get_str( -2 );
		C_PCHAR_C	text	= l.get_str();
		bool CONST b = BOX_ASK_WAR( title, text );
		return b ? l.return_real( 1 ) : l.return_nothing();
	}

	//	input	title, text
	AAALUACALL(	box_dev )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32 CONST	nb_arg = l.get_arg_nb_min( 1 );
		C_PCHAR_C	title	= l.get_str( 1 );
		o_str o(512);
		for( INT32 i = 2; i <= nb_arg; ++i )
		{
			C_PCHAR_C str = l.get_str( i );
			ERR_PRINT_STRING( "%s", str );
			o.add( str );
			o.add_char( '\n' );
		}
		INT32 ret = BOX_DEV( title, o.get() );
		l.pop_all();	//avoid message on enforcing empty stack when calling EVENT.hook
		if( ret > 0 )
			return l.return_int32( ret );
		else
			return l.return_nothing();
	}


	//	input	obj param
	//	return	folder name
	AAALUACALL( edit_dialog )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32		CONST nb_arg	= l.get_arg_nb_min_max( 2, 6 );
		INT32		CONST lua_id    = l.get_int32( 1 );
		p_param		CONST param		= l.get_param( 2 );
		c_obj_ui*	CONST obj		= (nb_arg >= 3) ? l.get_aaa_obj_or_nil(3) : nullptr;
		bool		CONST b_simple  = (nb_arg >= 4) ? l.get_bool(4) : false;
		C_PCHAR_C		  title		= (nb_arg >= 5) ? l.get_str_or_nil(5) : nullptr;
		bool		CONST b_integer = (nb_arg >= 6) ? l.get_bool(6) : false;
		if( param )
			th_dialog( obj, param, b_simple, title, true, lua_id, b_integer );
		return l.return_nothing();
	}

	//	input	(obj_ref or obj_name) row col val
	FINLINE int obj_set( c_lua_state& l, bool b_str )
	{
		l.check_arg_nb( 4 );
		c_bdd_datagrid* CONST	bdd		= l.get_aaa_obj_of_class<c_bdd_datagrid>( -4 );
		INT32					col		= l.get_int32( -3 );
		INT32					line	= l.get_int32( -2 );

		if( col >= 1 && line >= 1 )
		{
			--col;
			--line;
			if( b_str )
			{
				C_PCHAR_C str = l.get_str();
				bdd->set_str( col, line, str );
			}
			else
			{
				REAL	val = l.get_real();
				bdd->set_double( col, line, val );
			}
		}
		return l.return_nothing();
	}

	//	input	(obj_ref or obj_name) row col val
	AAALUACALL(	obj_set_real )	{	LUAAAA_START( L, __FUNCTION__ );	return obj_set( l, false );	}
	//	input	(obj_ref or obj_name) row col val
	AAALUACALL(	obj_set_str )	{	LUAAAA_START( L, __FUNCTION__ );	return obj_set( l, true  );	}

	//	input	(obj_ref or obj_name) row col
	AAALUACALL(	obj_get_data )
	{
		LUAAAA_START( L, __FUNCTION__ );
		c_bdd_datagrid*	CONST	bdd		= l.get_aaa_obj_of_class<c_bdd_datagrid>( 1 );
		INT32					col		= l.get_int32( 2 );
		INT32					line	= l.get_int32( 3 );

		if( col >= 1 && line >= 1 )
		{
			--col;
			--line;
			//todoopt to many calls
			if( bdd->has_data( col, line ) )
			{
				if( bdd->is_number( col, line ) )
					return l.return_double( bdd->get_double( col, line ) );
				else
					return l.return_string( bdd->get_str( col, line ) );
			}
		}
		return l.return_nothing();
	}

	//	input	(obj_ref or obj_name) row col
	FINLINE int obj_get_low( c_lua_state& l, bool b_str )
	{
		l.check_arg_nb( 3 );
		c_bdd_datagrid*	CONST	bdd		= l.get_aaa_obj_of_class<c_bdd_datagrid>( 1 );
		INT32					col		= l.get_int32( 2 );
		INT32					line	= l.get_int32( 3 );

		if( col >= 1 && line >= 1 )
		{
			--col;
			--line;
			if( b_str )
				return l.return_string( bdd->get_str( col, line ) );
			else
			{
				DOUBLE	val = bdd->get_double( col, line );
				return l.return_double( val );
			}
		}
		return l.return_nothing();
	}
	//	input	(obj_ref or obj_name) row col
	AAALUACALL(	obj_get_real )	{	LUAAAA_START( L, __FUNCTION__ );	return obj_get_low( l, false );	}
	//	input	(obj_ref or obj_name) row col
	AAALUACALL(	obj_get_str )	{	LUAAAA_START( L, __FUNCTION__ );	return obj_get_low( l, true  );	}

	AAALUACALL(	do_action )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		INT32 CONST	action_index	= l.get_int32( 1 );
		action::doit( (action::ACTION_TYPE)action_index );
		return l.return_nothing();
	}

	AAALUACALL(	material_front_set_alpha )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		REAL CONST	a			= l.get_real( 1 );

		c_materials::get_cur()->front_cur_set_alpha( a );
		return l.return_nothing();
	}


	//	id, x,y,z = get_multiple_id_xyz( obj )
	AAALUACALL(	get_multiple_id_xyz )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		c_obj_ui* CONST	obj		= l.get_aaa_obj( 1 );

		if( c_multiple::is_instance( obj ) )
		{
			l.push_int(		((c_multiple*)obj)->get_index()		);
			l.push_real_v3(	((c_multiple*)obj)->get_pos_draw()	);
			return 4;
		}
		return l.return_nothing();
	}

	AAALUACALL(	sleep )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		INT32 CONST	msec	= l.get_int32(1);
		spy::sleep( msec, "sleep_lua" );
		return l.return_nothing();
	}

	//	here for test
	AAALUACALL(	empty )
	{
		LUAAAA_START( L, __FUNCTION__ );
		return l.return_nothing();
	}

	//	input	(nil)
	AAALUACALL(	save )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb_escape_no( 0 );
		aaa::env::save( nullptr );
		return l.return_nothing();
	}

	//	input	(nil)
	AAALUACALL(	get_time_now )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb_escape_no( 0 );
		return l.return_double( aaa::time::get_real_time() );
	}
//////////////////////////////////////////////////////////////////////////
//	LUA
namespace	n_lua
{
	AAALUACALL(	set_file_force_recheck )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		bool CONST b = l.get_bool(1);
		g_lua_master->set_force_recheck( b );
		return l.return_nothing();
	}

	void	register_lua( lua_State* L )
	{
		LUAAAA_START( L, __FUNCTION__ );

		l.define_table( "lua" );
			ADD_FN( set_file_force_recheck );
		lua_pop( L, 1 );	//pop table "script"
	}
}	//end namespace n_lua
//////////////////////////////////////////////////////////////////////////
//	WINDOW
namespace	n_window
{
	AAALUACALL(	set_main_title_prefix )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		C_PCHAR_C pre = l.get_str();
		set_window_main_title_prefix( pre );
		return l.return_nothing();
	}
	AAALUACALL(	set_main_title )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		C_PCHAR_C title = l.get_str();
		set_window_main_title( title );
		return l.return_nothing();
	}

	void	register_window( lua_State* L )
	{
		LUAAAA_START( L, __FUNCTION__ );

		l.define_table( "window" );
			ADD_FN( set_main_title );
			ADD_FN( set_main_title_prefix );
		lua_pop( L, 1 );	//pop table "script"
	}
}	//end namespace n_window

//////////////////////////////////////////////////////////////////////////
//	TIME
namespace n_time
{
	void	register_time( lua_State* L )
	{
		LUAAAA_START( L, __FUNCTION__ );

		l.define_table( "time" );
			ADD_FN( get_time_now );
		lua_pop( L, 1 );	//pop table "script"
	}
}

	//////////////////////////////////////////////////////////////////////////
	//	VIDEO
namespace n_video
{
	AAALUACALL(	set_bind_filename )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 2 );
		INT32 CONST	index		= l.get_int32( -2 );
		C_PCHAR_C	filename	= l.get_str();

		if( filename )	{	::video_set_bind_name( index, filename );	}

		return l.return_nothing();
	}

	// return filename for a given index
	AAALUACALL(	get_bind_filename )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		INT32 CONST	index		= l.get_int32( 1 );
		//todo will return something even with bad index
		return l.return_string( ::video_get_bind_filename( index ) );
	}

	void	register_video( lua_State* L )
	{
		LUAAAA_START( L, __FUNCTION__ );

		l.define_table( "video" );

		//	GENERAL
		//
		ADD_FN( set_bind_filename );
		ADD_FN( get_bind_filename );

		l.pop( 1 );	//pop new table
	}
}	//end namespace n_video

	//////////////////////////////////////////////////////////////////////////
	//	XML

	void xml_parse_node( lua_State *L, TiXmlNode* pNode )
	{
		if (!pNode) return;
		// resize stack if neccessary
	//	luaL_checkstack(L, 5, "LuaXML_ParseNode : recursion too deep");
	
		TiXmlElement* pElem = pNode->ToElement();
		if (pElem)
		{
			// element name
			lua_pushstring(L,"name");
			lua_pushstring(L,pElem->Value());
			lua_settable(L,-3);
		
			// parse attributes
			TiXmlAttribute* pAttr = pElem->FirstAttribute();
			if (pAttr)
			{
				lua_pushstring(L,"attr");
				lua_newtable(L);
				for (;pAttr;pAttr = pAttr->Next())
				{
					lua_pushstring(L,pAttr->Name());
					lua_pushstring(L,pAttr->Value());
					lua_settable(L,-3);
				}
				lua_settable(L,-3);
			}
		}
	
		// children
		TiXmlNode *pChild = pNode->FirstChild();
		if (pChild)
		{
			int iChildCount = 0;
			for(;pChild;pChild = pChild->NextSibling())
			{
				switch (pChild->Type())
				{
					case TiXmlNode::TINYXML_DOCUMENT: break;
					case TiXmlNode::TINYXML_ELEMENT: 
						// normal element, parse recursive
						lua_newtable(L);
						xml_parse_node(L,pChild);
						lua_rawseti(L,-2,++iChildCount);
						break;
					case TiXmlNode::TINYXML_COMMENT: break;
					case TiXmlNode::TINYXML_TEXT: 
						// plaintext, push raw
						lua_pushstring(L,pChild->Value());
						lua_rawseti(L,-2,++iChildCount);
						break;
					case TiXmlNode::TINYXML_DECLARATION: break;
					case TiXmlNode::TINYXML_UNKNOWN: break;
				};
			}
			lua_pushstring( L, "n" );
			lua_pushnumber( L, iChildCount );
			lua_settable( L, -3 );
		}
	}

	AAALUACALL( xml_load_file )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		const char* sFileName = l.get_str();
		TiXmlDocument doc(sFileName);
		doc.LoadFile();
		lua_newtable( L );
		xml_parse_node( L, &doc );
		return  1;
	}

	//////////////////////////////////////////////////////////////////////////
	//	SCRIPT
	namespace n_script
	{
	
		namespace {
			thread_local o_str o_dir; 
		}
		//	aaa.script.get_dir_name()
		AAALUACALL(	get_dir_name )
		{
			LUAAAA_START( L, __FUNCTION__ );
			//l.check_arg_nb( 0 );
			o_str CONST * filename = c_lua_state::get_script_filename();
			if( filename )
			{
				o_dir.set_dir_name( *filename );
				return l.return_string( o_dir );
			}
			return l.return_nothing();	
		}

		//	aaa.script.get_name()
		AAALUACALL(	get_name )
		{
			LUAAAA_START( L, __FUNCTION__ );
			//l.check_arg_nb( 0 );
			return l.return_string( c_lua_state::get_script_debug_name_str() );
		}

		//	aaa.script.get_name_full()
		AAALUACALL(	get_name_full )
		{
			LUAAAA_START( L, __FUNCTION__ );
			//l.check_arg_nb( 0 );
			o_str CONST * CONST	filename = c_lua_state::get_script_filename();
			return filename ? l.return_string( *filename ) : l.return_nothing();
		}

		void	register_script( lua_State* L )
		{
			LUAAAA_START( L, __FUNCTION__ );

			l.define_table( "script" );
				ADD_FN( get_dir_name );
				ADD_FN( get_name );
				ADD_FN( get_name_full );
			lua_pop( L, 1 );	//pop table "script"
		}
	}	//end namespace n_script

	//////////////////////////////////////////////////////////////////////////
	//	OS
	namespace	n_os
	{
		//	shutdown( b_reboot )
		AAALUACALL(	shutdown )
		{
			LUAAAA_START( L, __FUNCTION__ );
			INT32	CONST	nb_arg = l.get_arg_nb_max( 1 );
			bool	CONST	b_reboot	= (nb_arg<1) ? false : l.get_bool();	
			return l.return_bool( stop::shutdown(b_reboot) );
		}
		//	exit( b_shutdown, b_reboot )
		AAALUACALL(	exit )
		{
			LUAAAA_START( L, __FUNCTION__ );
			INT32	CONST	nb_arg = l.get_arg_nb_max( 2 );
			bool	CONST	b_shutdown	= (nb_arg<1) ? false : l.get_bool( -nb_arg );
			bool	CONST	b_reboot	= (nb_arg<2) ? false : l.get_bool();
			stop::exit( b_shutdown, b_reboot );
			return l.return_nothing();
		}

		//	quit( b_confirm, b_save, b_shutdown, b_reboot )
		AAALUACALL(	quit )
		{
			LUAAAA_START( L, __FUNCTION__ );
			INT32	CONST	nb_arg = l.get_arg_nb_max( 4 );
			bool	CONST	b_confirm	= (nb_arg<1) ? false : l.get_bool( -nb_arg );
			bool	CONST	b_save		= (nb_arg<2) ? false : l.get_bool( -nb_arg+1 );
			bool	CONST	b_shutdown	= (nb_arg<3) ? false : l.get_bool( -nb_arg+2 );
			bool	CONST	b_reboot	= (nb_arg<4) ? false : l.get_bool();
			stop::quit( b_confirm, b_save, b_shutdown, b_reboot );
			return l.return_nothing();
		}

		AAALUACALL(	open_editor )
		{
			LUAAAA_START( L, __FUNCTION__ );
			INT32 CONST	nb_arg = l.get_arg_nb_min_max( 1, 2 );
			o_str CONST	o( l.get_str(1) );
			return l.return_bool( g_lua_master->trig_edit_file( o, (nb_arg==2) ? l.get_int32(2) : 0 ) );
		}

		AAALUACALL(	execute_shell )
		{
			LUAAAA_START( L, __FUNCTION__ );
			INT32 CONST	nb_arg = l.get_arg_nb_min_max( 1, 2 );
			AAA_ERR CONST ret = aaa::execute_shell( l.get_str(1), (nb_arg==2) ? l.get_str(2) : nullptr );
			//todo refine and pass a real error str
			return l.return_string_or_nil( ERR(ret) ? "Error any" : nullptr );
		}

		AAALUACALL(	execute_process )
		{
			LUAAAA_START( L, __FUNCTION__ );
			INT32 CONST	nb_arg = l.get_arg_nb_min_max( 1, 2 );
			AAA_ERR CONST ret = aaa::execute_process( l.get_str(1), (nb_arg==2) ? l.get_str(2) : nullptr );
			//todo refine and pass a real error str
			return l.return_string_or_nil( ERR(ret) ? "Error any" : nullptr );
		}

		AAALUACALL(	get_exe_start_time )
		{
			LUAAAA_START( L, __FUNCTION__ );
			p_param	CONST	param	= g_info->get_param_by_name( "exe_start_time" );
			//todo avoid dynamic allocation here
			o_str po;
			param->get_value_as_str( po );
			return l.return_string( po );
		}

		AAALUACALL(	pop_console )
		{
			LUAAAA_START( L, __FUNCTION__ );
			c_window::pop_console();
			return l.return_nothing();
		}
		AAALUACALL(	push_console )
		{
			LUAAAA_START( L, __FUNCTION__ );
			c_window::push_console();
			return l.return_nothing();
		}

		AAALUACALL(	pop_window )
		{
			LUAAAA_START( L, __FUNCTION__ );
			c_window::pop_window();
			return l.return_nothing();
		}
		AAALUACALL(	set_window_topmost )
		{
			LUAAAA_START( L, __FUNCTION__ );
			c_window::set_window_topmost();
			return l.return_nothing();
		}
		AAALUACALL(	set_window_notopmost )
		{
			LUAAAA_START( L, __FUNCTION__ );
			c_window::set_window_notopmost();
			return l.return_nothing();
		}
		AAALUACALL(	push_window )
		{
			LUAAAA_START( L, __FUNCTION__ );
			c_window::push_window();
			return l.return_nothing();
		}

		void	register_os( lua_State* L )
		{
			LUAAAA_START( L, __FUNCTION__ );

			l.define_table( "os" );
				ADD_FN( shutdown				);
				ADD_FN( exit					);
				ADD_FN( quit					);

				ADD_FN( open_editor				);		
				ADD_FN( execute_shell			);
				ADD_FN( execute_process			);

				ADD_FN( get_exe_start_time		);

				ADD_FN( push_console			);
				ADD_FN( pop_console				);
				ADD_FN( push_window				);
				ADD_FN( pop_window				);
				ADD_FN( set_window_topmost		);
				ADD_FN( set_window_notopmost	);
				lua_pop( L, 1 );	//pop table "os"
		}
	}	//end namespace n_os

	//////////////////////////////////////////////////////////////////////////
	//	DEBUG
	namespace n_debug
	{
		void	register_debug( lua_State* L )
		{
			LUAAAA_START( L, __FUNCTION__ );

			l.define_table( "debug" );
			lua_pop( L, 1 );	//pop table "debug"
		}
	}	//end namespace n_debug

	//////////////////////////////////////////////////////////////////////////
	//	FONT
	namespace n_font
	{
		//	aaa.font.read( bind, filename )
		AAALUACALL(	read )
		{
			LUAAAA_START( L, __FUNCTION__ );
			l.check_arg_nb( 2 );
			INT32 bind = l.get_int32( 1 );
			o_str CONST	o( l.get_str(2) );

			aaa::font::load_from_file( bind, o );
			return l.return_nothing();
		}
		void	register_font( lua_State* L )
		{
			LUAAAA_START( L, __FUNCTION__ );
			l.define_table( "font" );
				ADD_FN( read );
			lua_pop( L, 1 );	//pop table "font"
		}
	}	//end namespace n_script

	//////////////////////////////////////////////////////////////////////////
	//	ALL

	void	register_all_fn( lua_State* L )
	{
#if	AAA_LUA_STORE_CALLS()
		lua_newtable( L );
		lua_setglobal( L, "ccalls" );
#endif
		LUAAAA_START( L, __FUNCTION__ );

		STACK_ENFORCE_STORE( &l );

		l.push_string( "" );
		l.set_global( "aaa_error" );	//todo no idea why this is here

		//	register aaa_fns
		n_param::	register_param(	L );
		n_gol::		register_gol(	L );

		l.get_global( "table" );
			ADD_FN( create	);
		l.pop(1);


		l.define_table_global_get( "aaa" );
			n_draw::	register_draw(	L );
			n_color::	register_color(	L );

			ADD_FN( find_in_bind		);

			ADD_FN( wipe_focus			);

			ADD_FN( get_multiple_cur	);
			ADD_FN( get_caller			);

			ADD_FN( save				);
	
			//todo clean this one day
			n_obj::	register_obj(	L );
			n_file::register_file(	L );
			n_dir::	register_dir(	L );

			ADD_FN( print			);
			ADD_FN( print_inverse	);
	//		ADD_FN( trace_error		);
			ADD_FN( print_error		);
			ADD_FN( print_debug		);
			ADD_FN( bell			);

			ADD_FN( box_error		);
			ADD_FN( box_error_lua	);
			ADD_FN( box_warning		);
			ADD_FN( box_good		);
			ADD_FN( box_ask			);
			ADD_FN( box_dev			);

			ADD_FN( set				);
			ADD_FN( get				);

			ADD_FN( edit_dialog		);

			ADD_FN( obj_set_real	);
			ADD_FN( obj_set_str		);
			ADD_FN( obj_get_real	);
			ADD_FN( obj_get_str		);
			ADD_FN( obj_get_data	);

			ADD_FN( do_action		);

			ADD_FN( material_front_set_alpha );

			n_modules::		register_modules(	L	);
			n_module::		register_module(	L	);
			n_layers::		register_layers(	L	);
			n_layer::		register_layer(		L	);
			n_camera::		register_camera(	L	);
			n_fbo::			register_fbo(		L	);
			n_lights::		register_lights(	L	);
			n_materials::	register_material(	L	);
#if	!AAA_WATCHDOG()
			n_bdd::			register_bdd(		L	);
#endif	//AAA_WATCHDOG
			n_deformer::	register_deformer(	L	);
			n_shading::		register_shading(	L	);
			n_exp::			register_exp(		L	);
			n_array::		register_array(		L	);
			n_video::		register_video(		L	);
			n_time::		register_time(		L	);			
			n_spy::			register_spy(		L	);

			ADD_FN( get_multiple_id_xyz	);

			n_os::			register_os(		L	);

			ADD_FN( xml_load_file		);

			ADD_FN( empty				);
			ADD_FN( sleep				);

			n_script::		register_script(	L	);
			n_debug::		register_debug(		L	);
			n_lua::			register_lua(		L	);
			n_window::		register_window(	L	);
			n_midi::		register_midi(		L	);
			n_menu::		register_menu(		L	);
			n_net::			register_net(		L	);
			n_img::			register_img(		L	);
			n_font::		register_font(		L	);
			n_mouse::		register_mouse(		L	);
			n_clipboard::	register_clipboard(	L	);
			n_math::		register_math(		L	);
			n_flatland::	register_flatland(	L	);

#if	!AAA_WATCHDOG()
			//	creates a in table for traxs
			l.new_table();
			lua_setfield( L, -2, "tin" );
#endif	//AAA_WATCHDOG

	//	l.stack_dump( "register_all_fn()" );
		l.pop(1);	//pop table "aaa"

		//	register watchdog fns
		c_watchdog::register_aaalua( L );

#if	!AAA_WATCHDOG()
		//	register poid fns
		poid::		register_poid( L );
#endif	//AAA_WATCHDOG
		n_process::	register_process( L );

		STACK_ENFORCE_SAME( &l );
	}

	void	unregister_all_fn( lua_State *L )
	{
		LUAAAA_START( L, __FUNCTION__ );

		STACK_ENFORCE_STORE( &l );

			l.define_table_global_get( "aaa" );
			n_net::unregister_net(	L	);
			lua_pop( L, 1 );	//pop table "aaa"

		STACK_ENFORCE_SAME( &l );
	}

}	//end namespace aaalua