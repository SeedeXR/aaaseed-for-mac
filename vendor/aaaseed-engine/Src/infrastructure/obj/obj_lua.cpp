#include "obj_lua.h"
#include "obj_ui.h"
#include "language/lua/aaalua_glue.h"
#include "lua/luaconf.h"
#include "infrastructure/param/param_focus.h"
#include "infrastructure/namer.h"
#include "infrastructure/layer/layers.h"
#include "file/aaa_dir.h"


namespace aaalua
{
namespace n_obj
{
	namespace{
		thread_local o_str o_buf;
	}

	//	obj_ref = aaa.obj.create_by_cid( cid [,filename_or_nil [,root_obj_ref] ] )
	AAALUACALL( create_by_cid )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg	= l.get_arg_nb( 1, 2, 3 );
		C_PCHAR_C		cid		= l.get_str( 1 );

#if AAA_DEBUG()
		if( str_is_equal_nocase( cid, "module" ) )
			DBG_PRINT_STRING( "create_obj_by_cid asked for a module" );
#endif

		c_obj_ui* CONST obj	= (c_obj_ui*)c_factory_base::create_obj_by_cid( cid );
		if( !obj )
		{
			l.error_and_escape( "can't create object %.128s", cid );
			return l.return_nothing();
		}

		if( nb_arg >= 2 )
		{
			if( nb_arg >= 3 )
			{
				auto root = l.get_aaa_obj_or_nil( 3 );
				if( root )
					obj->set_root( root );
			}
			C_PCHAR_C filename = l.get_str_or_nil( 2 );
			if( filename )
			{
#if 1
				INT32 err = obj->load_from_file( filename );
#else
				o_str& dir_name = o_str::push_name();
				o_str& file_name = o_str::push_name();
					dir_name.set_dir_name( filename );
					dir_name.set_dir_name( dir_name );
					file_name.set_fname_relative( filename,	dir_name );
					c_dir::push_def( dir_name );
						INT32 err = obj->load_from_file( file_name );
					c_dir::pop_def();
				o_str::pop_name();
				o_str::pop_name();
#endif
				//	DBG_HEAP_IS_CORRUPT();
				if( ERR( err ) )
				{
					delete obj;
					return l.return_nothing();
				}
			}

		}
		return l.return_obj_ref( obj );
	}


	//	obj_ref = aaa.obj.set_root( obj_ref, root_ref )
	AAALUACALL( set_root )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32 CONST	nb_arg	= l.get_arg_nb( 2 );
		auto		obj		= l.get_aaa_obj( 1 );
		auto		root	= l.get_aaa_obj( 2 );
		obj->set_root( root );

		return l.return_nothing();
	}

	AAALUACALL(	delete_obj )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		c_obj_ui* obj = l.get_aaa_obj( 1 );

		SAFE_DELETE( obj );
		//todo delete or invalidate the ref
		return l.return_nothing();
	}

	C_PCHAR	get_help = "( id_or_name )\n  object_ref = aaa.obj.get( id_or_name )";
	C_PCHAR	get_no_error_help = "( id_or_name )\n  object_ref = aaa.obj.get_no_error( id_or_name )";
	//	"\tname is the filename of the object (you can get it thru the clipboard menu.)\n"
	//	"\treturn nil if object not found.";

	static INT32 get_obj_low( lua_State* L, C_PCHAR_C fn_name, bool b_error )
	{
		LUAAAA_START( L, fn_name );
		l.check_arg_nb( 1 );
		c_obj_ui* obj;
		if( l.is_number() )
		{
			c_obj_ui::OBJ_UI_ID CONST	obj_id	= l.get_value_direct<UINT32>();
			if( obj_id == 0 )
				l.error_and_escape( "obj_id should not be 0" );

			obj = c_obj_ui::get_from_id( obj_id );
			if( !obj )
			{
				if( b_error )
					l.error_and_escape( "can't find object number %u", obj_id );
				return l.return_nothing();
			}
		}
		else
		{
			C_PCHAR_C	name	= l.get_str();
			obj = c_obj_ui::find_from_top_by_name( name );
			if( !obj )
			{
				if( b_error )
					l.error_and_escape( "can't find object named \"%.128s\".", name );
				return l.return_nothing();
			}
		}
		return l.return_obj_ref( obj );
	}
	//	input	name_symbo
	AAALUACALL(	get )				{	return get_obj_low( L, __FUNCTION__, true  );	}
	AAALUACALL(	get_no_error )		{	return get_obj_low( L, __FUNCTION__, false );	}	
	
	C_PCHAR	get_by_name_help = 
		"( name )\n"
		"  object_ref = aaa.obj.get_by_name( name )\n"
		"  name is the filename of the object (you can get it thru the clipboard menu.)\n"
		"  return nil if object not found.";
	AAALUACALL(	get_by_name )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		C_PCHAR_C name = l.get_str();

		c_obj_ui* CONST obj = c_obj_ui::find_from_top_by_name( name );
		if( obj )
			return l.return_obj_ref( obj );

		l.error_and_escape( "can't find object \"%.260s\".", name );
		return l.return_nothing();
	}



	C_PCHAR	get_by_name_symbo_help = 
		"( name_symbo )\n"
		"  object_ref = aaa.obj.get_by_name_symbo( name_symbo )\n"
		"  name is the reference name (or symbolic name) of the object. You have to set it explicitly in the object.\n"
		"  return nil if object not found.";
	//	input name
	AAALUACALL(	get_by_name_symbo )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		C_PCHAR_C name = l.get_str();

		c_obj_ui* CONST obj = c_obj_ui::find_from_top_by_name_symbo( name );
		if( obj )
			return l.return_obj_ref( obj );

		l.error_and_escape( "can't find object with symbolic_name \"%.128s\".", name );
		return l.return_nothing();
	}

	C_PCHAR	get_by_class_and_name_symbo_help = 
		"( name_class, name_symbo )\n"
		"  object_ref = aaa.obj.get_by_class_and_name_symbo( name_class, name_symbo )\n"
		"  name is the reference name (or symbolic name) of the object. You have to set it explicitly in the object.\n"
		"  return nil if object not found.";
	//	input name
	AAALUACALL(	get_by_class_and_name_symbo )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 2 );
		C_PCHAR_C class_name	= l.get_str(1);
		C_PCHAR_C name			= l.get_str(2);

		o_buf.set( name );
		c_obj_ui* CONST obj = c_obj_ui::find_from_top_by_class_and_name_symbo( class_name, o_buf );
		
		if( obj )
			return l.return_obj_ref( obj );
		l.error_and_escape( "can't find object of class %.128s with symbolic_name \"%.128s\".", class_name, name );
		return l.return_nothing();
	}

	static INT32 get_down_low( lua_State* L, C_PCHAR_C fn_name, bool b_error )
	{
		LUAAAA_START( L, fn_name );
		l.check_arg_nb( 2 );
		c_obj_ui*	CONST from = l.get_aaa_obj( -2 );

		if( l.is_number() )
		{
			if( b_error )
				l.error_and_escape( "can't find object down by number for now" );
		}
		else
		{
			o_buf.set( l.get_str() );	//todo should we avoid dynamic allocation here ?
			c_obj_ui* found = from->find_down_by_name_symbo( o_buf );
			if( found )
				return l.return_obj_ref( found );
			if( b_error )
				l.error_and_escape( "can't find object down named \"%.128s\".", o_buf.get() );
		}
		return l.return_nothing();
	}
	//	input	obj, name_symbo
	AAALUACALL(	get_down )				{	return get_down_low( L, __FUNCTION__, true  );	}
	AAALUACALL(	get_down_no_error )		{	return get_down_low( L, __FUNCTION__, false );	}

	static INT32 get_up_low( lua_State* L, C_PCHAR_C fn_name, bool b_error )
	{
		LUAAAA_START( L, fn_name );
		l.check_arg_nb( 2 );
		c_obj_ui*	CONST from = l.get_aaa_obj( -2 );

		if( l.is_number() )
		{
			if( b_error )
				{ l.error_and_escape( "can't find object up by number for now" ); }
		}
		else
		{
			o_buf.set( l.get_str() );	//todo should we avoid dynamic allocation here ?
			c_obj_ui* found = from->find_up_by_name_symbo( o_buf );
			if( found )
				return l.return_obj_ref( found );
			if( b_error )
				l.error_and_escape( "can't find object up named \"%.128s\".", o_buf.get() );
		}
		return l.return_nothing();
	}
	//	input	obj, name_symbo
	AAALUACALL(	get_up )				{	return get_up_low( L, __FUNCTION__, true  );	}
	AAALUACALL(	get_up_no_error )		{	return get_up_low( L, __FUNCTION__, false );	}

	C_PCHAR	get_by_id_help = 
		"( id )\n"
		"  object_ref = aaa.get_by_id( id )\n"
		"  id is the id of the object. You have can find it in the interface, it is unique and different from zero.\n"
		"  return nil if object not found.";

	//	input obj_id
	AAALUACALL(	get_by_id )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		c_obj_ui::OBJ_UI_ID CONST	obj_id	= l.get_value_direct<UINT32>(1);
		if( obj_id == 0 )
			l.error_and_escape( "obj_id should not be 0" );

		c_obj_ui* CONST found = c_obj_ui::get_from_id( obj_id );
		if( found )
			return l.return_obj_ref( found );
		l.error_and_escape( "can't find object %u", obj_id );
		return l.return_nothing();
	}

	//	input class_name
	static INT32 get_from_top_by_class_low( lua_State* L, C_PCHAR_C fn_name, bool b_error )
	{
		LUAAAA_START( L, fn_name );
		l.check_arg_nb( 1 );
		C_PCHAR_C class_name = l.get_str();
		if( !class_name )
			 l.error_and_escape( "class_name should not be null" );

		c_obj_ui* CONST found = c_obj_ui::find_first_by_class_name( class_name );
		if( found )
			return l.return_obj_ref( found );
		if( b_error )
			l.error_and_escape( "can't find object of class %.128s", class_name );
		return l.return_nothing();
	}

	//	input class_name
	AAALUACALL(	get_from_top_by_class )				{	return get_from_top_by_class_low( L, __FUNCTION__, true  );	}
	AAALUACALL(	get_from_top_by_class_no_error )	{	return get_from_top_by_class_low( L, __FUNCTION__, false );	}


	//	input obj, class_name
	static INT32 get_by_class_low( lua_State* L, C_PCHAR_C fn_name, bool b_error )
	{
		LUAAAA_START( L, fn_name );
		l.check_arg_nb( 2 );
		c_obj_ui* CONST	obj			= l.get_aaa_obj_or_nil( -2 );
		C_PCHAR_C		class_name	= l.get_str( -1 );

		c_obj_ui* found = obj->find_by_class_name( class_name );
		if( found )
			return l.return_obj_ref( found );
		if( b_error )
			l.error_and_escape( "can't find object of class %.128s", class_name );
		return l.return_nothing();
	}
	//	input	obj, class_name
	AAALUACALL(	get_by_class )			{	return get_by_class_low( L, __FUNCTION__, true  );	}
	AAALUACALL(	get_by_class_no_error )	{	return get_by_class_low( L, __FUNCTION__, false );	}

	//	input obj, class_name
	static INT32 get_down_by_class_low( lua_State* L, C_PCHAR_C fn_name, bool b_error )
	{
		LUAAAA_START( L, fn_name );
		l.check_arg_nb( 2 );
		c_obj_ui* CONST	obj			= l.get_aaa_obj( -2 );
		C_PCHAR_C		class_name	= l.get_str( -1 );

		c_obj_ui* found = obj->find_down_by_class_name( class_name );
		if( found )
			return l.return_obj_ref( found );
		if( b_error )
			l.error_and_escape( "can't find object down of class %.128s", class_name );
		return l.return_nothing();
	}
	//	input	obj, class_name
	AAALUACALL(	get_down_by_class )				{	return get_down_by_class_low( L, __FUNCTION__, true  );	}
	AAALUACALL(	get_down_by_class_no_error )	{	return get_down_by_class_low( L, __FUNCTION__, false );	}

	//	input obj, class_name
	static INT32 get_up_by_class_low( lua_State* L, C_PCHAR_C fn_name, bool b_error )
	{
		LUAAAA_START( L, fn_name );
		l.check_arg_nb( 2 );
		c_obj_ui* CONST	obj			= l.get_aaa_obj( -2 );
		C_PCHAR_C		class_name	= l.get_str( -1 );

		c_obj_ui* found = obj->find_up_by_class_name( class_name );
		if( found )
			return l.return_obj_ref( found );
		if( b_error )
			l.error_and_escape( "can't find object up of class %.128s", class_name );
		return l.return_nothing();
	}
	//	input	obj, class_name
	AAALUACALL(	get_up_by_class )			{	return get_up_by_class_low( L, __FUNCTION__, true  );	}
	AAALUACALL(	get_up_by_class_no_error )	{	return get_up_by_class_low( L, __FUNCTION__, false );	}

	/*


	CHAR*	get_by_id_help = 
		"	object_ref = aaa.obj.get_by_id( id )\n"
		"\tid is the id of the object. You have can find it in the interface, it is unique and different from zero.\n"
		"\treturn nil if object not found.";

	//	input obj_id
	AAALUACALL( get_by_id )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		c_obj_ui::OBJ_UI_ID CONST	obj_id	=  l.get_value_direct<UINT32>( 1 );	//now
		if( obj_id == 0 )
			l.error_and_escape( "obj_id should not be 0" );

		c_obj_ui* obj = c_obj_ui::find_by_id( obj_id );
		if( obj )
			return l.return_obj_ref( obj );

		l.error_message( "can't find object %u", obj_id );
		return l.return_nothing();
	}
	*/
	
	C_PCHAR_C str_meta_obj_name = "__aaa_ud_obj";

	FINLINE INT32 push_obj_ud( lua_State* L, c_obj_ui* obj )
	{
		void* pt; 
		//pt = obj->get_lua_ud();
		//if( !pt )	//hack
		{
			//size_t nbytes = sizeof(c_obj_ui*);
			pt = lua_newuserdata( L, sizeof(c_obj_ui*) );
			luaL_getmetatable( L, str_meta_obj_name );
			lua_setmetatable( L, -2 );
			*(c_obj_ui**)pt = obj;
			obj->set_lua_ud( pt );
		}
		return 1;  //	new userdatum is already on the stack
		//return l.return_obj_ref( obj );
	}			

	C_PCHAR_C	get_with_ud_help = 
		"( name_or_id )\n"
		"	object_ref = aaa.obj.get_with_ud( name_or_id )\n"
		"  name is the name_symbo of the object\n"
		"  if it fail name will be tried then as the filename (you can get it using the clipboard menu.)\n"
		"  if the argument is a number AAASeed try to use it as the id of the object\n"
		"  return nil if object not found.";
	AAALUACALL( get_with_ud )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		c_obj_ui* obj;
		if( l.is_number() )
		{
			c_obj_ui::OBJ_UI_ID CONST obj_id =  l.get_value_direct<UINT32>( 1 );
			if( obj_id == 0 )
				l.error_and_escape( "obj_id should not be 0" );

			obj = c_obj_ui::get_from_id( obj_id );
			if( !obj )
				l.error_message( "can't find object %u", obj_id );
		}
		else
		{
			C_PCHAR_C	name	= l.get_str();
			obj = c_obj_ui::find_from_top_by_name( name );
			if( !obj )
				l.error_message( "can't find object with ref or name %.128s", name );
		}
		if( obj )
		{
			/*
			size_t nbytes = sizeof(c_obj_ui*);
			c_obj_ui**	hd = (c_obj_ui**)lua_newuserdata(L, nbytes);
			luaL_getmetatable( L, str_meta_obj_name );
			lua_setmetatable( L, -2 );
			*hd = obj;
			*/
			push_obj_ud( L, obj );
			return l.return_userdatum();  //	new userdatum is already on the stack
			//return l.return_obj_ref( obj );
		}			
		return l.return_nothing();
	}

	//	input name
	AAALUACALL( find_first_by_class_name )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		C_PCHAR_C name = l.get_str();

		c_obj_ui* obj = c_obj_ui::find_first_by_class_name( name );
		if( obj )
			return l.return_obj_ref( obj );

		l.error_message( "can't find object of class %.128s", name );
		return l.return_nothing();
	}

	//	input	(obj_ref or obj_name)
	AAALUACALL(	get_class )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		CONST c_obj_ui* CONST obj	= l.get_aaa_obj( 1 );

		return l.return_string( obj->get_class_name() );
	}

	//	input	(obj_ref or obj_name)
	AAALUACALL(	get_name_ui )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		CONST c_obj_ui*	CONST obj = l.get_aaa_obj( 1 );
		auto p_o = obj->get_name_ui();
		if( p_o )
			return l.return_string_or_nil( *p_o );
		return l.return_nothing();
	}
	//	input	(obj_ref or obj_name)
	AAALUACALL(	get_name_human )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		CONST c_obj_ui*	CONST obj = l.get_aaa_obj( 1 );
		return l.return_string_or_nil( obj->get_name_human() );
	}


	//	input	(obj_ref or obj_name)
	AAALUACALL(	get_name )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		CONST c_obj_ui*	CONST obj	= l.get_aaa_obj( 1 );

		return l.return_string( obj->get_name_search() );
	}
	//	input	(obj_ref or obj_name), str
	AAALUACALL(	set_name )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32		CONST	nb_arg	= l.get_arg_nb( 1, 2 );
		c_obj_ui*	CONST	obj		= l.get_aaa_obj( 1 );
		C_PCHAR_C			name	= nb_arg == 2 ? l.get_str( 2 ) : nullptr;

		obj->set_name( name );
		return l.return_nothing();
	}

	//	input	(obj_ref or obj_name)
	AAALUACALL(	get_filename )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		CONST c_obj_ui*	CONST obj = l.get_aaa_obj( 1 );

		C_PCHAR_C pt_char = obj->get_my_filename();
		if( *pt_char )
			return l.return_string( obj->get_my_filename() );
		else
			return l.return_nothing();
	}

	//	input	(obj_ref or obj_name, filename )
	AAALUACALL(	set_filename )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 2 );
		c_obj_ui* CONST	obj	= l.get_aaa_obj( 1 );
		o_str& o_filename	= o_str::push_name( l.get_str(2) );
			obj->set_my_filename( o_filename );
		o_str::pop_name();
		return l.return_nothing();
	}

	//	input	(obj_ref or obj_name)
	AAALUACALL(	get_root )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );		
		CONST c_obj_ui* CONST obj = l.get_aaa_obj( 1 );

		c_obj_ui* CONST root = obj->get_root();
		if( root )
			return l.return_obj_ref( root );

		l.error_message( "no root for object %.128s", obj->get_name_search_str() );
		return l.return_nothing();
	}

	//	input	(obj_ref or obj_name)
	AAALUACALL(	get_name_symbo )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		CONST c_obj_ui*	CONST obj = l.get_aaa_obj( 1 );
		return l.return_string_or_nil( obj->get_name_symbo() );
	}
	//	input	(obj_ref or obj_name), str
	AAALUACALL(	set_name_symbo )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32		CONST	nb_arg	= l.get_arg_nb( 1, 2 );
		c_obj_ui*	CONST	obj		= l.get_aaa_obj( 1 );
		C_PCHAR_C			name	= nb_arg == 2 ? l.get_str( 2 ) : nullptr;

		obj->set_name_symbo( name );
		return l.return_nothing();
	}
	
	//	input	(obj_ref or obj_name) class_name
	static INT32 get_branch_by_name_symbo_low( lua_State* L, C_PCHAR_C fn_name, bool b_error )
	{
		LUAAAA_START( L, fn_name );
		l.check_arg_nb( 2 );		
		CONST c_obj_ui* CONST obj = l.get_aaa_obj( -2 );
		o_buf.set( l.get_str() );		//todo should we avoid dynamic allocation here ?

		c_obj_ui* CONST branch = obj->find_branch_by_name_symbo( o_buf );
		if( branch )
			return l.return_obj_ref( branch );
		if( b_error )
			l.error_and_escape( "can't find branch of name symbolic %.64s in object %.128s", o_buf.get(), obj->get_name_search_str() );
		return l.return_nothing();
	}
	//	input	(obj_ref or obj_name) class_name
	AAALUACALL(	get_branch_by_name_symbo )			{	return get_branch_by_name_symbo_low( L, __FUNCTION__, true  );	}
	AAALUACALL(	get_branch_by_name_symbo_no_error )	{	return get_branch_by_name_symbo_low( L, __FUNCTION__, false );	}

	//	input	(obj_ref or obj_name) class_name
	static INT32 get_branch_by_class_low( lua_State* L, C_PCHAR_C fn_name, bool b_error )
	{
		LUAAAA_START( L, fn_name );
		l.check_arg_nb( 2 );		
		CONST c_obj_ui* CONST	obj		= l.get_aaa_obj( -2 );
		C_PCHAR_C				name	= l.get_str();

		c_obj_ui* branch = obj->find_branch_by_class_name( name );
		if( branch )
			return l.return_obj_ref( branch );
		if( b_error )
			l.error_and_escape( "can't find branch of class %.64s in object %.128s", name, obj->get_name_search_str() );
		return l.return_nothing();
	}
	//	input	(obj_ref or obj_name) class_name
	AAALUACALL(	get_branch_by_class )			{	return get_branch_by_class_low( L, __FUNCTION__, true  );	}
	AAALUACALL(	get_branch_by_class_no_error )	{	return get_branch_by_class_low( L, __FUNCTION__, false );	}

	//	input	(obj_ref or obj_name) class_name
	AAALUACALL( get_branchs_by_class )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 2 );		
		CONST c_obj_ui* CONST	obj		= l.get_aaa_obj( -2 );
		C_PCHAR_C			class_name	= l.get_str();

		std::vector< c_obj_ui* > vec;
		obj->find_branchs_by_class_name( class_name, vec );
		if( vec.empty() )
			l.error_and_escape( "can't find any branch of class %.64s in object %.128s", class_name, obj->get_name_search_str() );

		l.new_table();

			INT32 i = 0;
			for( auto const & p_obj_ui : vec )
			{
				l.push_int( ++i );
				l.push_obj_ref( p_obj_ui );
				l.set_table( -3 );
			}

		return l.return_table();
	}

		//	input	(class_name)
	AAALUACALL( get_intances_by_class )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );		
		C_PCHAR_C class_name = l.get_str();

		l.new_table();

		c_factory_base*	pf = c_factory_base::get_by_cid( class_name );
		if( pf )
		{
			INT32 i = 0;
			for( auto const & p_obj_ui : *pf->get_objs_list() )
			{
				l.push_int( ++i );
				l.push_obj_ref( p_obj_ui );
				l.set_table( -3 );
			}
		}

		return l.return_table();
	}


	AAALUACALL( get_branchs )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );		
		CONST c_obj_ui* CONST obj = l.get_aaa_obj( 1 );

		l.new_table();

		INT32 nb = obj->get_branch_nb();
		for( INT32 i=0; i<nb; ++i )
		{
			auto item = obj->get_branch(i);
			l.push_int( i+1 );
			l.push_obj_ref( item );
			l.set_table( -3 );
		}

		return l.return_table();
	}
	//namespace {
	//	thread_local o_str o_dir; 
	//}
	////	input	(obj_ref or obj_name)
	//AAALUACALL(	get_dir )
	//{
	//	LUAAAA_START( L, __FUNCTION__ );
	//	l.check_arg_nb( 1 );
	//	c_obj_ui* CONST obj	= l.get_aaa_obj( 1 );

	//	o_dir.erase();
	//	obj->get_namer()->build_dir( o_dir, obj );
	//	return l.return_string( o_dir );
	//}

	//	input	dst (obj_ref or obj_name), src (obj_ref or obj_name)
	AAALUACALL(	set_param_from )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32		CONST	nb_arg	= l.get_arg_nb_min_max( 2, 4 );
		c_obj_ui*	CONST	dst		= l.get_aaa_obj( 1 );
		c_obj_ui*	CONST	src		= l.get_aaa_obj( 2 );
		if( nb_arg >= 3 )
		{
			INT32 begin = l.get_int32( 3 );
			if( c_obj_ui::is_param_ref( begin ) )
				l.error_message( "%x is a ref not an index", begin );

			if( nb_arg == 4 )
				dst->cpy_params_from( src, begin, l.get_int32( 4 ) );
			else
				dst->cpy_params_from( src, begin );
		}
		else
		{
			dst->cpy_params_from( src );
		}
		return l.return_nothing();
	}

	//	input	dst (obj_ref or obj_name), src (obj_ref or obj_name)
	AAALUACALL( set_param_from_skip )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 4 );
		c_obj_ui* CONST	dst			= l.get_aaa_obj( 1 );
		c_obj_ui* CONST	src			= l.get_aaa_obj( 2 );
		INT32 CONST		begin		= l.get_int32( 3 );
		INT32 CONST		skip_end	= l.get_int32( 4 );
		dst->cpy_params_from_skip( src, begin, skip_end );
		return l.return_nothing();
	}

	//	input	obj_ref or obj_name
	AAALUACALL(	clear_param )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		c_obj_ui* CONST	obj = l.get_aaa_obj( 1 );
		if( obj )
			obj->param_set_to_ina();	//todo one day use the right name default/inactive
		return l.return_nothing();
	}

	static INT32 is_obj_ref_low( lua_State* L, C_PCHAR_C fn_name, bool b_error )
	{
		LUAAAA_START( L, fn_name );
		l.check_arg_nb( 1 );
		if( l.is_number() )
		{
			UINT32 CONST ref = l.get_value_direct<UINT32>();
			if( c_obj_ui::is_obj_ref( ref ) )
				return l.return_bool( true );
			else
			{
				if( b_error )
					l.error_message( "%x is not an obj ref", ref );
			}
		}
		else
		{
			if( b_error )	{ l.error_message( "obj ref should be a number" ); }
		}
		return l.return_bool( false );
	}
	AAALUACALL( is_ref )			{	return is_obj_ref_low( L, __FUNCTION__, true  );	}
	AAALUACALL( is_ref_no_error )	{	return is_obj_ref_low( L, __FUNCTION__, false );	}

	
	//	input	(obj_ref or obj_name)
	AAALUACALL(	update_params )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		c_obj_ui* CONST	obj = l.get_aaa_obj( 1 );
		obj->param_init_pt();
		return l.return_nothing();
	}

	//	input	(obj_ref or obj_name)
	AAALUACALL(	set_focus )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		c_obj_ui* CONST	obj = l.get_aaa_obj( 1 );
		obj->set_focus();
		return l.return_nothing();
	}

	//	input	(obj_ref or obj_name)
	AAALUACALL(	is_focus )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		CONST c_obj_ui* CONST obj = l.get_aaa_obj( 1 );
		return l.return_bool( focus_param::get_obj() == obj );	
	}

	//	input	(obj_ref or obj_name)
	AAALUACALL(	become_ui )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		c_obj_ui* CONST obj	= l.get_aaa_obj( 1 );
		obj->become_ui();
		return l.return_nothing();
	}

	AAALUACALL( set_searchable_by_filename )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		bool CONST b = l.get_bool( 1 );
		c_obj_ui::set_searchable_by_filename( b );
		return l.return_nothing();
	}

	//	input	(obj_ref or obj_name), filename=nil
	AAALUACALL( load )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	  CONST	arg_nb	= l.get_arg_nb( 1,2 );
		c_obj_ui* CONST obj     = l.get_aaa_obj( 1 );
		INT32 err;
		//	reset the object : kind of a hack here
		//hack make a way to clean the obj
		//obj->deinit();
		//obj->init();
		obj->param_set_to_ina();
		if( arg_nb==1 )
		{
			err = obj->load();
		}
		else
		{
			o_buf.set( l.get_str(2) );
			//	reset the object : kind of a hack here
			obj->set_my_filename( "" );
			INT32 CONST err = obj->load_from_file( o_buf );
		//	DBG_HEAP_IS_CORRUPT();	
		}
		return NOERR( err ) ? l.return_nothing() : l.return_int32( err );
	}

	//	input	(obj_ref or obj_name), filename=nil
	AAALUACALL( save )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	  CONST	arg_nb	= l.get_arg_nb( 1,2 );
		c_obj_ui* CONST obj     = l.get_aaa_obj( 1 );
		INT32 err;
		if( arg_nb==1 )
		{
			err = obj->save();
		}
		else
		{
			o_buf.set( l.get_str(2) );
			INT32 CONST err = obj->save_to_file( o_buf );
		//	DBG_HEAP_IS_CORRUPT();	
		}
		return NOERR( err ) ? l.return_nothing() : l.return_int32( err );
	}

	//	input	(obj_ref or obj_name), filename
	AAALUACALL(	save_tree_to_file )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32		CONST	nb_arg	=	l.get_arg_nb( 2 );	//, 3 );
		c_obj_ui*	CONST	obj		=	l.get_aaa_obj( 1 );
		o_str		CONST	fpath(		l.get_str( 2 ) );
		//bool		CONST	b_full	=	(nb_arg==3) ? l.get_bool( 3 ) : false ;
		INT32 err;
		if( obj )
		{
			if( obj->is_class_name( "layers" ) )
			{
				//err = ((c_layers*)obj)->save_tree_to_file( filename, b_full );
				err = obj->save_tree_to_file( fpath );	//unused b_full );
			}
			else if( obj->is_class_name( "module" ) )
			{
				//todo move to module or back to Lua (lua bad for push pop when error in this call
				o_str& dirname = o_str::push_name();
					//dirname.add_slash();
					dirname.set_dir_name( fpath );
					c_dir::push_def( dirname );
						err = obj->save_tree_to_file( fpath.get_fname() );	//unused b_full );
					c_dir::pop_def();
				o_str::pop_name();
			}
			else
				l.error_and_escape( "object is not a layers or a module but of class %.64s", obj->get_class_name() );
		}
		return NOERR( err ) ?  l.return_nothing() : l.return_int32( err );
	}

	//	input	(obj_ref or obj_name), filename
	AAALUACALL(	load_tree_from_file )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 2 );
		c_layers* CONST	obj	= (c_layers*) l.get_aaa_obj_of_class( -2, "layers" );	//only layers for now
		o_buf.set( l.get_str() );

		//INT32 err = ERR_OBJ_NULL;
		//	reset the object : kind of a hack here
		//hack make a way to clean the obj
		obj->set_my_filename( "" );
		obj->deinit();
		obj->init();
		//	then load
		INT32 CONST err = obj->load_from_file( o_buf );
	//	DBG_HEAP_IS_CORRUPT();
		return NOERR( err ) ?  l.return_nothing() : l.return_int32( err );
	}

	//	input	(obj_ref or obj_name)
	AAALUACALL(	update_then_draw )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		c_obj_ui* CONST	obj	= l.get_aaa_obj( 1 );

		l.pop_all();
		if( obj->is_active() )
		{
			if( !obj->update_then_draw_lua() )
				l.error_and_escape( "problem doing update_then_draw() -> %.128s", c_obj_ui::get_ref_error_str() );
		}
		return l.return_nothing();
	}
	//	input	(obj_ref or obj_name)
	AAALUACALL(	update )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		c_obj_ui* CONST	obj = l.get_aaa_obj( 1 );

		l.pop_all();
		obj->update();
		return l.return_nothing();
	}
	//	input	(obj_ref or obj_name)
	AAALUACALL(	draw )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		c_obj_ui* CONST	obj = l.get_aaa_obj( 1 );

		l.pop_all();
		obj->draw();
		return l.return_nothing();
	}

	//	input	(obj_ref or obj_name)
	AAALUACALL(	enum_command )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		c_obj_ui* CONST	obj = l.get_aaa_obj( 1 );

		l.pop_all();
		obj->enum_command( o_buf );
		return l.return_string_or_nil( o_buf );
	}

	//	input	(obj_ref or obj_name), command as string
	AAALUACALL(	do_command )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 2 );
		c_obj_ui* CONST	obj	= l.get_aaa_obj( 1 );
		C_PCHAR_C command	= l.get_str( 2 );

		l.pop_all();		
		return l.return_bool( obj->do_command( command ) );
	}

		/*
	//todo really do it and deal with param_ref directly
	//	input	(obj_ref or obj_name) param_name
	AAALUACALL(	attach )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 2 );
		c_obj_ui* CONST obj = l.get_aaa_obj( 1 );

		INT32 index = l.get_int32( 2 );
		//	insert_before( obj, index );
		return l.return_nothing();
	}

	//todo really do it and deal with param_ref directly
	//	input	(obj_ref or obj_name) param_name
	AAALUACALL(	detach )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 2 );
		c_obj_ui* CONST obj	= l.get_aaa_obj( 1 );
		INT32 index = l.get_int32( 2 );
		//	remove_and_destroy( obj, index );
		return l.return_nothing();
	}
	*/

/*
	//	ADD_FN( attach );
	//	ADD_FN( detach );
	//todo really do it and deal with param_ref directly
	//	input	(obj_ref or obj_name) param_name
	AAALUACALL( attach )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 2 );
		c_obj_ui* obj	= l.get_aaa_obj( 1 );
		if( obj )
		{
			INT32	index = l.get_int32( 2 );
			//	insert_before( obj, index );
		}
		else
			l.error_message( "no object to attach" );
		return l.return_nothing();
	}

	//todo really do it and deal with param_ref directly
	//	input	(obj_ref or obj_name) param_name
	AAALUACALL( detach )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 2 );
		c_obj_ui* obj	= l.get_aaa_obj( 1 );
		if( obj )
		{
			INT32	index = l.get_int32( 2 );
			//	remove_and_destroy( obj, index );
		}
		else
			l.error_message( "no object to detach" );
		return l.return_nothing();
	}
*/
/*
	//	input	(obj_ref or obj_name), slot, (obj_ref or obj_name)
	AAALUACALL( set_target )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 3 );
		bool ret;
		c_obj_ui*	obj		= l.get_aaa_obj( 1 );
		if( obj )
		{
			INT32		slot	= l.get_int32( 2 );
			c_obj_ui*	target	= l.get_aaa_obj_or_nil( 3 );
			ret = obj->set_target( slot, target );
		}
		return l.return_bool( ret );
	}
	//	input	(obj_ref or obj_name), slot
	AAALUACALL( get_target )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 2 );
		c_obj_ui*	obj		= l.get_aaa_obj( 1 );
		c_obj_ui*	target;
		if( obj )
		{
			INT32	slot	= l.get_int32( 2 );
			target = obj->get_target( slot );
		}
		else
			target = nullptr;
		return l.return_obj_ref( target );
	}
*/


	
	/*
	CONST struct luaL_Reg aaa_obj_lib[] =
	{
		{	"set_focus",		set_focus			},
		{	"become_ui",		become_ui			},
		{	"save",				save				},
		{	"update_then_draw",	update_then_draw	},
		{	nullptr,			nullptr				}
	};
	*/

	AAALUACALL( getter ) 
	{
		LUAAAA_START( L, __FUNCTION__ );
		//array self = array_check(L, 1);
		C_PCHAR_C key = l.get_str(2);
		// TODO bounds checking on i
		return l.return_int( 12 );
		//lua_pushnumber(L, self->data[i-1]);
	}

	//	input	(obj_ref or obj_name)
	AAALUACALL( __index )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 2 );	//we should have on the stack the userdata and the field
		//		c_obj_ui*	obj		= l.get_aaa_obj( 1 );
		C_PCHAR_C key = l.get_str();

		lua_getmetatable( L, -2 );
		l.get_field( -1, key );		//	lua_getfield( L, -1, key );
		// Either key is name of a method in the metatable
		if( ! l.is_nil() )
			return l.return_table();

		// ... or its a field access, so recall as self.get(self, value).
		lua_settop(L, 2);
		return getter(L);
	}
	AAALUACALL( __newindex )
	{
		LUAAAA_START( L, __FUNCTION__ );
		//		l.check_arg_nb( 1 );
		//		c_obj_ui*	obj		= l.get_aaa_obj( 1 );
		l.print_stack( "__newindex()" );
		return 0;
	}
	AAALUACALL( __concat )
	{
		LUAAAA_START( L, __FUNCTION__ );
		//		l.check_arg_nb( 1 );	//we should have on the stack the userdata and the field
		//		c_obj_ui*	obj		= l.get_aaa_obj( 1 );
		l.print_stack( "__concat()" );
		return l.return_string( "unimplemented __concat() for AAASeed UserData" );
	}
	AAALUACALL( __tostring )
	{
		LUAAAA_START( L, __FUNCTION__ );
		c_obj_ui* obj = *(c_obj_ui**) l.get_userdata(1);
		CHAR str[128];
		sprintf( str, "userdata for %.64s(%u)", obj->get_class_name(), obj->get_obj_ui_id() );
		return l.return_string( str );
	}
	void register_obj_metable( lua_State* L )
	{
		LUAAAA_START( L, __FUNCTION__ );

		//luaL_newmetatable( L, "aaa.obj );
		aaalua::new_metatable( L, str_meta_obj_name );
//		ADD_FN( __tostring );
//		ADD_FN( __concat );

		lua_pushlstring( L, "__index", 7 );
		lua_pushvalue( L, -2 );	//	pushes the metatable
		lua_settable( L, -3 );	//	metatable.__index = metatable */

		//lua_pushliteral( L, "__metatable" );
		//lua_pushvalue(L, -3);               /* dup methods table*/
		//lua_rawset(L, -3);                  /* hide metatable:

		//luaL_openlib(L, nullptr, arraylib_m, 0);
		//		ADD_FN( attach );
		//		ADD_FN( detach );
		ADD_FN( __index				);
		ADD_FN( __newindex			);
		ADD_FN( __concat			);
		ADD_FN( __tostring			);

		ADD_FN( set_focus			);
		ADD_FN( is_focus			);
		ADD_FN( become_ui			);
		ADD_FN( save				);
		ADD_FN( update_then_draw	);
		ADD_FN( update				);
		ADD_FN( draw				);
		//luaL_register( L, nullptr, aaa_obj_lib );
		l.pop( 1 );	//pop new table
	}

	void	register_obj( lua_State* L )
	{
		LUAAAA_START( L, __FUNCTION__ );

		register_obj_metable( L );

		l.define_table( "obj" );

			ADD_FN( create_by_cid	);
			l.add_fn_to_table( "delete", delete_obj );
			ADD_FN( set_root		);

			ADD_FN_WITH_HELP( get );
			ADD_FN_WITH_HELP( get_no_error );

			ADD_FN_WITH_HELP( get_by_name );
			ADD_FN_WITH_HELP( get_by_name_symbo	);
			ADD_FN_WITH_HELP( get_by_class_and_name_symbo );

			ADD_FN(	get_intances_by_class				);

			ADD_FN(	get_by_id							);

			ADD_FN(	get_down							);
			ADD_FN(	get_down_no_error					);
			ADD_FN( get_up								);
			ADD_FN(	get_up_no_error						);

			ADD_FN( get_from_top_by_class				);
			ADD_FN( get_from_top_by_class_no_error		);
			ADD_FN(	get_by_class						);
			ADD_FN(	get_by_class_no_error				);
			ADD_FN( get_down_by_class					);
			ADD_FN( get_down_by_class_no_error			);
			ADD_FN( get_up_by_class						);
			ADD_FN( get_up_by_class_no_error			);

			ADD_FN( get_class							);

			ADD_FN(	get_name_ui							);
			ADD_FN(	get_name_human						);
			ADD_FN( get_name							);
			ADD_FN( set_name							);
			ADD_FN( get_name_symbo						);
			ADD_FN( set_name_symbo						);
			ADD_FN( get_filename						);

			ADD_FN( set_filename						);

//			ADD_FN( get_dir								);

			ADD_FN( get_root							);

			ADD_FN( get_branch_by_name_symbo			);
			ADD_FN( get_branch_by_name_symbo_no_error	);
			ADD_FN( get_branchs_by_class				);
			ADD_FN( get_branch_by_class					);
			ADD_FN( get_branch_by_class_no_error		);
			ADD_FN( get_branchs							);

			ADD_FN( set_param_from						);
			ADD_FN( set_param_from_skip					);
			ADD_FN( clear_param							);

			ADD_FN( is_ref								);
			ADD_FN( is_ref_no_error						);

			ADD_FN( set_focus							);
			ADD_FN( is_focus							);
			ADD_FN( become_ui							);
			ADD_FN( update_params						);

			ADD_FN( set_searchable_by_filename			);
			ADD_FN( save								);
			ADD_FN( load								);
			ADD_FN( save_tree_to_file					);
			ADD_FN( load_tree_from_file					);


			ADD_FN( update_then_draw					);
			ADD_FN( update								);
			ADD_FN( draw								);

			ADD_FN( enum_command						);
			ADD_FN( do_command							);

			ADD_FN_WITH_HELP( get_with_ud				);
	//		ADD_FN_WITH_HELP( get_by_id );
			ADD_FN( find_first_by_class_name			);

			//ADD_FN( set_target						);
			//ADD_FN( get_target						);
			//ADD_FN( attach							);
			//ADD_FN( detach							);
	
		l.pop( 1 );	//pop new table
	}
}	//end namespace n_obj
}	//end namespace aaalua
