#include "factory.h"
#include "infrastructure/obj/obj_ui.h"

/*	trick to make sure object is created
//singleton
c_registry<c_factory_base*>&	the_registry_1()
{
	static	c_registry<c_factory_base*>	registry;
	return registry;
}
*/

//	could be in the class static but complexifies includes
static	c_registry<c_factory_base*>*	registry_by_cid		= nullptr;
static	c_registry<c_factory_base*>*	registry_by_file_ext= nullptr;

bool	c_factory_base::b_verbose		= false;
INT32	c_factory_base::count			= 0;
INT32	c_factory_base::count_abstract	= 0;
INT32	c_factory_base::count_concrete	= 0;
bool	c_factory_base::b_do_dealloc	= true;

UINT32 factory_param_data_nb_max = 0;
o_str factory_param_data_nb_max_class_name;

#if AAA_DEBUG()
#	define AAA_LOCAL_VERBOSE() 1
#else
#	define AAA_LOCAL_VERBOSE() 0
#endif
c_factory_base::c_factory_base( C_PCHAR_C cid, c_factory_base* super_factory, C_PCHAR_C human, C_PCHAR_C ext, C_PCHAR_C props )
	:_obj_count			(	0						)
	,_obj_created_nb	(	0						)
	,_class_name		(	cid						)
	,_super_factory		(	super_factory			)
	,_name_human		(	human ? human : cid		)
	,_file_ext			(	ext ? ext : cid			)
{	
	++count;
#if AAA_LOCAL_VERBOSE()
	DBG_PRINT_STRING( "Create Factory %d : %.128s", count, cid );
#endif
//	std::cout << "# create_obj factory " << count << " " << cid << "\n";
	if( super_factory )
	{
		_super_factory->add_derived( this );

		if( b_verbose )
		{
			o_str CONST & o = super_factory->get_class_name();
			PRINT_STRING( "create_obj factory %s:%s\n", cid, o.is_empty() ? "super not initialized" : o.get() );
		}
	}
	else
	{
		if( strcmp( cid, "obj_ui" ) != 0 )
			DBG_PRINT_STRING( "factory<%s>have no super", cid );
	}
		
	set_properties( props );

	//	make sure we have the 2 registries for the factories
	if( !registry_by_cid )
		registry_by_cid = new c_registry<c_factory_base*>;
	if( !registry_by_file_ext )
		registry_by_file_ext = new c_registry<c_factory_base*>;

	registry_by_cid->sign_in( _class_name.get(), _class_name.get_len(), this );
	if( ext )
	{
		if( strcmp( ext, "def" ) != 0 )	//hack def is the extension for all type of deformer
			registry_by_file_ext->sign_in( ext, this );
		//else
		//	debug_break( "ext not def" );
	}

#if AAA_LOCAL_VERBOSE()
	DBG_PRINT_STRING( "\tFactory %.128s Created", cid );
#endif
}

void	c_factory_base::delete_all_objs()
{
//	c_obj_ui* root = c_obj_ui::get_the_root()->get_father();
	C_PCHAR_C cname = get_class_name();
	if( _objs.empty() )
		DBG_PRINT_STRING( "delete_all_objs() no %s", cname );
	else
	{
		DBG_PRINT_STRING( "delete_all_objs() %s Begin", cname );
		while( !_objs.empty() )
		{
			c_obj_ui* obj = _objs.front();
			//if( obj->get_root() != root )
				SAFE_DELETE( obj );
		}
		DBG_PRINT_STRING( "delete_all_objs() %s End", cname );
	}
}

void	c_factory_base::before_exit_all_objs()
{
	//	c_obj_ui* root = c_obj_ui::get_the_root()->get_father();
	C_PCHAR cname = get_class_name();
	if( _objs.empty() )
		DBG_PRINT_STRING( "prepare_quit_all_objs() no %s", cname );
	else
	{
		DBG_PRINT_STRING( "prepare_quit_all_objs() %s Begin", cname );
		for( auto const & obj : _objs )
			obj->before_exit();
		DBG_PRINT_STRING( "prepare_quit_all_objs() %s End", cname );
	}
}

void c_factory_base::dealloc()
{
	if( !b_do_dealloc )
		return;

	DBG_PRINT_STRING( "destroying factory data %.64s", _class_name.get() );
	_derived_factories.clear();

	if( registry_by_file_ext )
	{
		registry_by_file_ext->sign_out( _file_ext.get() );
		if( registry_by_file_ext->size()==0 )
			SAFE_DELETE( registry_by_file_ext );
	}
	if( registry_by_cid )
	{
		registry_by_cid->sign_out( _class_name.get() );
		if( registry_by_cid->size()==0 )
			SAFE_DELETE( registry_by_cid );
	}

	//check and refine
	_objs.clear();	//todo check at least, delete what left (if not static alloc)
	_obj_count = 0;

	for( auto const & pf : _param_data )
		delete pf;
	_param_data.clear();

	//_properties; 

	//std::set<c_factory_base*>		_derived_factories;	//	derived class factories

}

c_factory_base::~c_factory_base()
{
	DBG_PRINT_STRING( "destroying factory %.64s", _class_name.get() );
	//factories are static for now
	//	so we let exit free the instance
	//for( auto pf : _derived_factories )
	//{
	//	obj_delete( pf );
	//}
	dealloc();
	--count;
}
void c_factory_base::dealloc_dirty()
{
	DBG_PRINT_STRING( "destroying _derived_factories data %.64s", _class_name.get() );
	for( auto const & pf : _derived_factories )
		pf->dealloc_dirty();
	dealloc();
	//amusant mais pas tres efficace
	//std::vector<c_param_data*>		dum;
	//std::swap( dum, _param_data );
}

void	c_factory_base::add_derived( c_factory_base* derived )
{
	if( derived )
		_derived_factories.insert( derived );
	else
		debug_break( "in %s() derived variable can't be NULL", __FUNCTION__ );
}
INT32	c_factory_base::get_derived_nb()
{
	return (INT32)_derived_factories.size();
}

c_factory_base*	c_factory_base::get_derived( INT32 index )
{
	auto it = _derived_factories.begin();
	advance( it, index );
	return *it;
}

C_PCHAR_C			c_factory_base::get_cid(			INT32 index )		{	return registry_by_cid->get_key( index );		}
C_PCHAR_C			c_factory_base::get_file_ext(		INT32 index )		{	return registry_by_file_ext->get_key( index );	}
c_factory_base*		c_factory_base::get_by_cid(			C_PCHAR_C cid )		{	return registry_by_cid->get( cid );				}
c_factory_base*		c_factory_base::get_by_file_ext(	C_PCHAR_C ext )		{	return registry_by_file_ext->get( ext );		}


c_obj_ui*	c_factory_base::create_obj_by_cid( C_PCHAR_C cid )
{
	if( cid )
	{
		c_factory_base*	pf = get_by_cid( cid );
		if( pf )
			return pf->create_obj();
		ERR_PRINT_STRING( "%s(\"%s\") can't find factory", __FUNCTION__, cid );
	}
	else
		ERR_PRINT_STRING( "%s() can't find factory from a null pointer", __FUNCTION__ );
	return nullptr;
}

c_obj_ui*	c_factory_base::create_obj_by_file_ext( C_PCHAR_C ext )
{
	if( ext )
	{
		c_factory_base*	pf = get_by_file_ext( ext );
		if( pf )
			return pf->create_obj();
		ERR_PRINT_STRING( "%s(\"%s\") can't find factory", __FUNCTION__, ext );
	}
	else
		ERR_PRINT_STRING( "%s() can't find factory from a null pointer", __FUNCTION__ );
	return nullptr;
}

void	c_factory_base::register_obj( c_obj_ui* obj )
{
	_objs.push_back( obj ); 
	++_obj_count;
	++_obj_created_nb;
	if( _super_factory )
		_super_factory->register_obj( obj );
}

void	c_factory_base::unregister_obj( c_obj_ui* obj )
{
	_objs.remove( obj );
	if( _obj_count > 0 )
		--_obj_count;
	else
		debug_break( "%s() unregister called with factory obj count already at zero", __FUNCTION__ );
	if( _super_factory )
		_super_factory->unregister_obj( obj );
}

c_obj_ui*	c_factory_base::create_obj()
{
	c_obj_ui* po = new_hidden();
	if( !po )
		ERR_PRINT_STRING( "%s() for class %s can't create", __FUNCTION__, _class_name.get() );
	return po;
}

//	is_class_match is because this is a loose mechanism
bool	c_factory_base::is_class_match( C_PCHAR_C str )
{
	INT32	str_len = (INT32)strlen( str );

	C_PCHAR match_str = get_class_name();
	INT32	match_len = (INT32)strlen( match_str );
	if( str_len == (match_len-4) && str_is_similar( str, match_str + 4 ) )
		return true;
	if( str_len == match_len && str_is_similar( str, match_str ) )
		return true;

	match_str = get_name_human().get();
	match_len = get_name_human().get_len();
	if( str_len == match_len && str_is_similar( str, match_str ) )
		return true;
	match_str = get_property( "sel0" );	//todo say why
	if( match_str )
	{
		if( str_is_similar( str, match_str ) )
			return true;
	}
	//	try to match the end, but does wrong match
	//	if( str_len < match_len && str_is_similar( str, match_str+match_len-str_len ) )
	//		return true;
	return false;
}

void	c_factory_base::get_hierarchic_name( o_str& dst )
{
	dst.set( get_class_name() );

	c_factory_base*	pf = this;
	while( pf = pf->get_super() )
	{
		dst.add_char( ':' );
		dst.add( pf->get_class_name() );
	}
}

bool	c_factory_base::is_in_get_hierarchic_name( C_PCHAR_C str )
{
	c_factory_base*	pf = this;
	while( pf = pf->get_super() )
	{
		if( str_is_equal_bothnocase( pf->get_class_name(), str ) )
			return true;
	}
	return false;
}

void	c_factory_base::add_property( C_PCHAR_C name, C_PCHAR_C value )
{
	_properties.sign_in( name, value );
}

namespace
{
	//	infact don't alloc for now
	bool str_isolate( o_str& dst, CHAR const * & cur, CHAR deli )
	{
		CHAR const *	pdeli = strchr( cur, deli );
		if( pdeli )
		{	
			CHAR const *	start = cur;
			CHAR const *	stop = pdeli - 1;
			//	set cur position after deli
			cur = pdeli + 1;
			// get rid of delimiter and trailing space
			while( stop > start && *stop==' ' )
				--stop;
			++stop;
			// get rid of space at beginning
			while( *start==' ' )
				++start;
			if( *start == '"' )
			{
				++start;
				--stop;
			}
			dst.set( start, (INT32)(stop-start), -1 );
			return true;
		}
		dst.erase();
		return false;
	}
}


void	c_factory_base::set_properties( CHAR const * command )
{	//todo do real parsing of string as with file
//we used these 2 object for all c_factory_base::set_properties process
static o_str o_prop_buf;
static o_str o_val_buf;

	if( command )
	{
		CHAR const *	pt = command;
		while( *pt )
		{
			//	if(	sscanf( pt, "%64[^=]=%256[^;]", key, prop ) == 2)
			//	pt is incremented by str_isolate
			if( !str_isolate( o_prop_buf, pt,'=' ) )
				break;
			if( !str_isolate( o_val_buf, pt, ';' ) )
			{
				BOX_ERR( "in c_factory<%s> can't get value of property %s", _class_name.get(), o_prop_buf.get() );
				break;
			}
			add_property( o_prop_buf.get(), o_val_buf.get() );
		}
	}
}

C_PCHAR_C	c_factory_base::get_property( C_PCHAR_C name )
{
	return _properties.get( name );
}
C_PCHAR_C	c_factory_base::get_file_filter()
{
	return _properties.get( "file_filter" );
}
C_PCHAR_C	c_factory_base::get_menu_sub()
{
	return _properties.get( "menu_sub" );
}

void	c_factory_base::push_param_data( c_param_def CONST * CONST def )
{
	c_param_data* pd = new c_param_data( def );
	_param_data.push_back( pd );
	UINT32 nb = (UINT32) _param_data.size();
	if( nb > factory_param_data_nb_max && !is_class_name("bind") )	// bind image is 65536 ()
	{
		factory_param_data_nb_max = nb;
		factory_param_data_nb_max_class_name.set( get_class_name() );
	}
}

//todo this cost a lot and go up with the obj number
//	we need to go from param to obj in a local way (something in c_param and c_params should address this)
//c_obj_ui*	c_factory_base::find_obj_by_param( c_param CONST * CONST param ) CONST
//{
//	for( auto const & p_obj : _objs )
//	{
//		if( p_obj->is_param_belong(param) )
//			return p_obj;
//	}
//	return nullptr;
//}

/*
void	c_factory_base::print_by_registry( c_registry* reg)
{
C_PCHAR		key;
INT32			count;
	PRINT_STRING( "# this factory have %d items\n", reg->size() );
	count = 0;
	while( key = reg.get_key( count) )
	{
		c_factory_base*	pf = reg->get( key);
		if( pf )
		{
			PRINT_STRING( "# item %d-%s\t:\t %s\t->%s\n", ++count, key , pf->get_cid(), pf->get_name_human().get() );
			if( pt->get_file_filter() )
				if( strcmp( cid, pf->get_file_ext() ) != 0 )
					PRINT_STRING( "#\t\t.%s\t%s\n", pf->get_file_ext(), pf->get_file_filter() );
				else
					PRINT_STRING( "#\t\tfile_filter %s\n", pf->get_file_filter() );
			else
			{
				if( strcmp( cid, pf->get_file_ext() ) != 0 )
					PRINT_STRING( "#\t\tfile_ext %s\n", pf->get_file_ext() );
			}
			pf->properties_.print();
			o_str	tmp;
			pf->get_hierarchic_name( tmp);
			PRINT_STRING("\t%s\n", tmp.get() );
			//test
			//		c_obj_ui* obj;
			//		obj = pt->create_obj();
			//		if( strcmp( cid, obj->get_class_name()) )
			//			ERR_PRINT_STRING( "# bad match for : factory %s <-> virtual fn %s", cid , obj->get_class_name() );		
			//		delete obj;
		}
	}
}
*/

void	c_factory_base::print()
{
	c_registry<c_factory_base*>& reg = *registry_by_cid;

	PRINT_STRING( "# this factory by class name have %d items\n", reg.size() );

	C_PCHAR	cid;
	INT32	count = 0;
	while( cid = reg.get_key(count) )
	{
		c_factory_base*	pt = reg.get(cid);
		if(pt)
		{
			if( strcmp( pt->get_class_name(), cid ) != 0 )
				debug_break( "factory:: pb class_name different than key.");	
			PRINT_STRING( "# item %d:\t %s\t->%s\n", ++count, cid , pt->get_name_human().get() );
			if( pt->get_file_filter() )
				if( !pt->get_file_ext().is_equal(cid) )
					PRINT_STRING( "#\t\t.%s\t%s\n", pt->get_file_ext().get(), pt->get_file_filter() );
				else
					PRINT_STRING( "#\t\tfile_filter %s\n", pt->get_file_filter() );
			else
			{
				if( pt->get_file_ext().is_equal(cid) )
					PRINT_STRING( "#\t\tfile_ext %s\n", pt->get_file_ext().get() );
			}
			pt->_properties.print();
			o_str	tmp;
			pt->get_hierarchic_name( tmp);
			PRINT_STRING("\t%s\n", tmp.get() );
			//test
	//		c_obj_ui* obj;
	//		obj = pt->create_obj();
	//		if( strcmp( cid, obj->get_class_name()) )
	//			ERR_PRINT_STRING( "# bad match for : factory %s <-> virtual fn %s", cid , obj->get_class_name() );		
	//		delete obj;
		}
		else
			debug_break( "%s() got a nullptr factory.", __FUNCTION__ );
	}
}

//	check all the sub hierarchy of a class
void	c_factory_base::check_hierarchy()
{
	PRINT_STRING( "# check hierarchy of %s class\n", get_class_name() );

	c_registry<c_factory_base*>&	reg = *registry_by_cid;
	INT32							index = 0;
	INT32							sub_nb = 0;
	c_factory_base*					pt;
	//	parse all the factories
	while( pt = reg.get_item( index ) )
	{
		C_PCHAR	pstr = pt->get_super_class_name();
		if( pstr && strcmp( pstr, get_class_name() ) == 0 )
		{	//check the one having this one (name) as super (derived from super)
			++sub_nb;
			pt->check_hierarchy();
		}
		++index;
	} 
	//	check the numbers are ok
	if( sub_nb != get_derived_nb() )
		debug_break();

	PRINT_STRING( "#\tCHECK DONE\n" );
}

//	check all the sub hierarchy of a class
void	c_factory_base::check_hierarchy( C_PCHAR_C name )
{
	c_factory_base*	pt = registry_by_cid->get( name );
	pt->check_hierarchy();
}

//	print all the sub hierarchy of a class
void	c_factory_base::print_hierarchy( bool b_detail, INT32 level )
{
	PRINT_STRING( "#" );
	for( INT32 i=level; i>0; --i )
		PRINT_STRING( "  " );
	if( b_detail )
	{
		PRINT_STRING( " %s(%d) %s %s\n", get_class_name(), get_obj_count(), get_name_human().get(), get_file_ext().get() );
		//_properties.print( true );
	}
	else
		PRINT_STRING( " %s(%d)\n", get_class_name(), get_obj_count() );

	INT32			index = 0;
	INT32			sub_nb = get_derived_nb();
	while( --sub_nb >= 0 )
	{
		c_factory_base* pt = get_derived( index );
		++index;
		pt->print_hierarchy( b_detail, level+1 );
	}
}

//	print all the sub hierarchy of a class
void	c_factory_base::print_hierarchy( C_PCHAR_C name, bool b_detail, INT32 level )
{
	c_factory_base*	pt = registry_by_cid->get( name );
	pt->print_hierarchy( b_detail, level+1 );
}

void	c_factory_base::build_derived_concrete( std::list<c_factory_base*>& list_dc )
{
	if( is_concrete() )
		list_dc.push_back( this );

	INT32			index = 0;
	INT32			sub_nb = get_derived_nb();
	while( --sub_nb >= 0 )
	{
		c_factory_base*	pt = get_derived( index );
		++index;
		pt->build_derived_concrete( list_dc );
	}
}

//	build a list of all derived concrete class
void	c_factory_base::build_derived_concrete( C_PCHAR_C name, std::list<c_factory_base*>& list_dc )
{
	c_factory_base*	pt = registry_by_cid->get( name );
	pt->build_derived_concrete( list_dc );
}

void	c_factory_base::check_all_obj()
{
	c_registry<c_factory_base*>&	reg = *registry_by_cid;

	PRINT_STRING( "# there is %d object factories\n", reg.size() );
	PRINT_STRING( "#\tAAASeed will now check an object in each class\n" );

	C_PCHAR	cid;
	INT32 count = 0;
	while( cid = reg.get_key(count) )
	{
		o_str tmp;
		c_factory_base* pt = reg.get( cid );
		if(pt)
		{
			pt->get_hierarchic_name( tmp );
			PRINT_STRING("\t%s\n", tmp.get() );

			if( strcmp( pt->get_class_name(), cid ) != 0 )
				debug_break( "factory:: pb class_name different than key.");

			if( !(pt->_objs.empty()) )
			{
				c_obj_ui* obj = pt->_objs.front();
				C_PCHAR	a = pt->get_class_name();
				C_PCHAR	b = obj->get_class_name();
				if( a && b )
				{
					if( strcmp( a, b ) != 0 && pt->is_concrete() )
						debug_break( "%s() pb class_name .", __FUNCTION__ );
				}
				else
				{
					if( !a )
						ERR_PRINT_STRING( "%s() factory empty info .", __FUNCTION__ );
					if( !b )
						ERR_PRINT_STRING( "%s() obj empty info .", __FUNCTION__ );
				}
			}
			++count;
			//test
			//		c_obj_ui* obj;
			//		obj = pt->create_obj();
			//		if( strcmp( cid, obj->get_class_name()) )
			//			ERR_PRINT_STRING( "# bad match for : factory %s <-> virtual fn %s", cid , obj->get_class_name() );		
			//		delete obj;
		}
		else
			debug_break( "%s() got a nullptr factory.", __FUNCTION__ );
	}
	PRINT_STRING( "#\tCHECK DONE\n" );
}

//TEST
class c_toto : public c_obj_ui
{
	FACTORY_DECLARE(c_toto,c_obj_ui)
private:
//	c_factory<c_toto>( "toto", "exemple de toto", "toto_ext", "toto_filer");
public:
	virtual	void param_init_pt() {};
};

FACTORY_CREATE_V1( c_toto, exemple de toto, toto_ext, toto_filter );
CONSTRUCTOR_CREATE(c_toto)	{}
EMPTY_DESTRUCTOR(c_toto)

class c_titi : public c_obj_ui
{
	FACTORY_DECLARE(c_titi,c_obj_ui)
private:
public:
	virtual	void param_init_pt() {};
};

FACTORY_CREATE_V1( c_titi, exemple de titi, , );
CONSTRUCTOR_CREATE(c_titi)	{}
EMPTY_DESTRUCTOR(c_titi)


void	c_factory_base::test()
{
	c_factory_base::print();
}


