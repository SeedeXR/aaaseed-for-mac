#include "namer.h"
#include "infrastructure/obj/obj_ui.h"
#include "infrastructure/layer/app.h"

INT32	namer_error_count = 0;

void	c_namer::check_one( c_obj_ui* CONST obj )
{
	if( !obj )
	{
		ERR_PRINT_STRING( "%s() can't be passe a nullptr, skipping check. ", __FUNCTION__ );
		return;
	}
	o_str& s = o_str::push_name();

		if( !(obj->get_root()) && !obj->is_the_root() )
		{
			obj->set_root( g_app );
			c_namer::build_name( s, *obj );
			if( *obj->get_name_search_str() )
				DBG_PRINT_STRING( "correcting NULL root %s::%s searchname %s", obj->get_class_name(), s.get(), obj->get_name_search_str() );
			else
				DBG_PRINT_STRING( "correcting NULL root %s::%s with no_searchname", obj->get_class_name(), s.get() );
		}
		else
			c_namer::build_name( s, *obj );
		if( strcmp( s.get(), obj->get_name_search_str() ) )
		{
			++namer_error_count;
			DBG_PRINT_STRING( "namer -> %s \t\tfor class %s", s.get(), obj->get_class_name() );
			if( *obj->get_name_search_str() )
				DBG_PRINT_STRING( "  search %s", obj->get_name_search_str() );
		}

	o_str::pop_name();
}

void	c_namer::check_all()
{
	namer_error_count = 0;

	//todoqqq reestablish by giving a way to parse all the object existing
	//for( auto const & p_obj_ui : c_obj_ui::g_list() )
	//	check_one( p_obj_ui );

	if( namer_error_count )
		DBG_PRINT_STRING( "%s() nb of error : %d", __FUNCTION__, namer_error_count );
}


c_namer*	c_namer::get_namer_always( c_obj_ui* CONST obj )
{
	if( obj )
	{
		c_namer* p = obj->get_namer();
		if( !p )
		{
			p = new c_namer;
			if( !p )
				debug_break();
			else
				obj->set_namer( p );
		}
		return p;
	}
	debug_break();
	return nullptr;
}

void	c_namer::set_dir_and_file( c_obj_ui* CONST obj, C_PCHAR_C dir, C_PCHAR_C file )
{
	c_namer* p = get_namer_always( obj );
	if( p )
	{
		p->_dir.set( dir );	
		p->_file.set( file );	
		p->_b_file_forced = false;
/*		if( dir )
			DBG_PRINT_STRING( "%s\t%s\t%s", dir, file, b_forced?"ON":"Off" );
		else
			DBG_PRINT_STRING( "\t\t%s\t%s", file, b_forced?"ON":"Off" );
*/
	}
}

void	c_namer::set_dir_and_file( c_obj_ui* CONST obj, o_str CONST & dir, o_str CONST & file )
{
	c_namer* p = get_namer_always( obj );
	if( p )
	{
		p->_dir.set( dir );	
		p->_file.set( file );	
		p->_b_file_forced = false;
/*		if( dir )
			DBG_PRINT_STRING( "%s\t%s\t%s", dir, file, b_forced?"ON":"Off" );
		else
			DBG_PRINT_STRING( "\t\t%s\t%s", file, b_forced?"ON":"Off" );
*/
	}
}

void	c_namer::set_dir_and_file( c_obj_ui* CONST obj, o_str CONST & str )
{
	if( obj && !str.is_empty() )
	{
		o_str& dir  = o_str::push_name();
		o_str& file = o_str::push_name();
			dir.set_dir_name( str );
			file.set_fname_pure( str );
			set_dir_and_file( obj, dir, file );
		o_str::pop_name();
		o_str::pop_name();
	}
	else
		debug_break();
}

void	c_namer::set_file_only( c_obj_ui* CONST obj, o_str CONST & str )
{
	if( obj && !str.is_empty() )
	{
		c_namer* p = get_namer_always( obj );
		if( p )
		{
			o_str& file = o_str::push_name();
				file.set_fname_pure( str );
				p->_dir.erase();	
				p->_file.set( file );	
				p->_b_file_forced = false;
			o_str::pop_name();
		}
	}
}
void	c_namer::set_file_forced( c_obj_ui* CONST obj, o_str CONST & filename )
{
	if( obj && !filename.is_empty() )
	{
		c_namer* p = get_namer_always( obj );
		if( p )
		{
			p->_dir.erase();	
			p->_file.set( filename.get_fname() );	
			p->_b_file_forced = false;
		}
	}
}

void	c_namer::set_dir_only( c_obj_ui* CONST obj, o_str CONST & dir )
{
	c_namer* p = get_namer_always( obj );
	if( p )
	{
		p->_dir.set( dir );	
		p->_file.erase();	
		p->_b_file_forced = false;
	}
}
void	c_namer::set_namer_start( c_obj_ui* CONST obj )
{
	if( obj  )
	{
		c_namer*	p = obj->get_namer();
		if( !p )
			debug_break();
		p->set_start();	
//		DBG_PRINT_STRING( "--- this a start---" );
	}
	else
		debug_break();
}



//to while instead of recursive
c_namer*	c_namer::build_dir( o_str& dst, c_obj_ui* CONST obj )
{
	c_namer*	namer = obj->get_namer();
	if( namer )
	{
		//build from root
		if( !namer->is_start() )
		{
			c_obj_ui*	obj_root = obj->get_root();	//todo have a debug version of get_root
			if( obj_root )
				build_dir( dst, obj_root ); 
		}

		//add own contribution
		o_str CONST & s = namer->get_dir();
		if( !s.is_empty() )
		{
			dst.add( s );
			dst.add_slash();
		}
		return namer;
	}
	else
	{
		//build from root
		c_obj_ui*	obj_root = obj->get_root();	//todo have a debug version of get_root
		if( obj_root )
			return build_dir( dst, obj_root );
	}
	return nullptr;
}

void	c_namer::build_name( o_str&	dst, c_obj_ui& obj )
{
	if( obj.is_the_root() )
		return;	// root have no name

	c_namer*	namer;

	if( namer = build_dir( dst, &obj ) )
	{
		dst.add( namer->get_file() );
		if( !namer->_b_file_forced )
			dst.add_ext( obj.get_fname_ext() );
	}
}


c_namer::c_namer()
:_b_file_forced(false)
,_b_start(false)
{
}

c_namer::~c_namer()
{
}


