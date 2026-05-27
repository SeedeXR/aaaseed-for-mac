#include "infrastructure/obj/obj_ui.h"
#include "infrastructure/layer/module.h"
#include "infrastructure/param/param_focus.h"
#include "infrastructure/param/param_draw.h"
#include "obj_ui/bdd/bdd_cell_draw.h"
#include "infrastructure/namer.h"	 
#include "obj_ui/com/net_blk.h"
#include "obj_ui/com/osc/OscReceivedElements.h"
#include "spy.h"
#include "infrastructure/obj/root.h"
#include "file/aaa_dir.h"


c_factory_abstract<c_obj_ui>&	c_obj_ui::the_factory()
{
	static	c_factory_abstract<c_obj_ui>	class_factory( "obj_ui", nullptr, "Base Object" );
	return class_factory;
}


bool		c_obj_ui::b_verbose_load		= false;
bool		c_obj_ui::b_verbose_save		= false;

//todo these should be move out of c_obj_ui 
bool		c_obj_ui::b_aaa_exiting			= false;
bool		c_obj_ui::b_aaa_exiting_fast	= false;	//todo true generate error at start
bool		c_obj_ui::b_aaa_exiting_hack	= false;

UINT32		c_obj_ui::obj_ui_nb_created		= 0;
UINT32		c_obj_ui::obj_ui_nb				= 0;

c_obj_ui**	c_obj_ui::id_to_obj				= nullptr;
o_str		c_obj_ui::o_sum_up;

namespace
{
	aaa::MUTEX	lock_io;
	//hack
	bool b_clear_root_on_dealloc = true;
};
c_root*		c_obj_ui::the_global_obj_ui_root = nullptr;



void	c_obj_ui::c_init()
{
	if( the_global_obj_ui_root )
	{
		debug_break( "%s() Called twice remove second call please", __FUNCTION__ );
	}
	else
	{
		size_t size = (1<<c_obj_ui::ID_BIT_NB) * sizeof(c_obj_ui*);
		id_to_obj = (c_obj_ui**) MALLOC( size );
		MEMCLEAR( id_to_obj, size );
		the_global_obj_ui_root = new c_root;
		the_global_obj_ui_root->set_name( "AAASeed_ROOT" );
	}
}

//tod call it
//todo good place to check leaks g_root 
void	c_obj_ui::c_deinit()
{
	SAFE_DELETE( the_global_obj_ui_root );
	SAFE_DELETE( id_to_obj );
}

//define this way to make sure the static obj used is initialized
//singleton
#define AAA_USE_GLIST() 0

#if AAA_USE_GLIST()
	//this keep a list of all existing obj_ui
	FINLINE std::list<c_obj_ui*>&	g_list()
	{
	static	std::list<c_obj_ui*> g_list_trick;
		return g_list_trick;	
	}
#endif

//this provide a search mechanism by name
//todo precise and refine
FINLINE c_registry<c_obj_ui*>&	g_map()
{
static	c_registry<c_obj_ui*> g_map_trick;
	return g_map_trick;
}

#if AAA_USE_GLIST()

c_obj_ui*	c_obj_ui::get_from_id( OBJ_UI_ID CONST id_in )
{
	for( auto const & p_obj : g_list() )
	{
		if( p_obj->get_obj_ui_id() == id_in )
		{
			if( p_obj->is_valid_obj_ui() )
				return p_obj;

			debug_break( "%s() obj found seems to have been deleted", __FUNCTION__ );
		}
	}
	return nullptr;
}
#else	// AAAA_USE_GLIST()

#	if AAA_DEBUG()
//rhe non debug version is in the include and faster
c_obj_ui*	c_obj_ui::get_from_id(	OBJ_UI_ID CONST id_in	)
{
	if( id_in < ID_MIN )
	{
		ERR_PRINT_STRING( "%s() obj_ui_id begin at %u, %u asked : returning nullptr", __FUNCTION__, ID_MIN, id_in );
		return nullptr;
	}
	else if( ID_MAX < id_in )
	{
		ERR_PRINT_STRING( "%s() obj_ui_id max is %u, %u asked : returning nullptr", __FUNCTION__, ID_MAX, id_in );
		return nullptr;
	}

	c_obj_ui* obj = *(id_to_obj+id_in); 
	if( !obj )
	{	//todo refine messages
		ERR_PRINT_STRING( "%s() no obj found, deleted ? ", __FUNCTION__ );
	}	
	return obj;
}
#	endif

#endif	// AAA_USE_GLIST()


//
//	FIND
//
namespace {
	thread_local bool	b_searchable_by_filename = true;
}

void	c_obj_ui::set_searchable_by_filename( bool b )
{
	b_searchable_by_filename = b;
}

void	c_obj_ui::set_cell_pos_size_frame( REAL CONST* CONST pos, REAL CONST size, INT32 CONST frame )
{
	cpy_v3( _cell_pos, pos );
	_cell_size = size;
	_cell_render_frame = frame;
}


//todo this is slow and don't deal with several obj_ui having the same name_symbo
// we should deal with acceleration and same name
// but it imply to have a mecanism in a sub class of o_str to avoid checking changes all the time 
//	this is a static fn
c_obj_ui*	c_obj_ui::find_from_top_by_name_symbo( o_str CONST & name )
{
#if AAA_USE_GLIST()
	for( auto const & p_obj : g_list() )
	{		
#else
	for( UINT32 i=ID_MIN; i<=ID_MAX; i++ )
	{
		c_obj_ui * const p_obj = *(id_to_obj + i);
		if( !p_obj )
			continue;
#endif
		if( p_obj->is_name_symbo( name ) )
			return p_obj;
	}
	return nullptr;
}
//	this is a static fn
c_obj_ui*	c_obj_ui::find_from_top_by_name_symbo( C_PCHAR_C name )
{
	INT32 len = (INT32) strlen(name);
#if AAA_USE_GLIST()
	for( auto const & p_obj : g_list() )
	{	
#else
	for( UINT32 i=ID_MIN; i<=ID_MAX; i++ )
	{
		c_obj_ui * const p_obj = *(id_to_obj + i);
		if( !p_obj )
			continue;
#endif
		if( p_obj->is_name_symbo( name, len ) )
			return p_obj;
	}
	return nullptr;
}

c_obj_ui*	c_obj_ui::find_down_by_name_symbo( o_str CONST & name )
{
	if( is_name_symbo( name ) )
		return this;

	return _branch ? _branch->find_down_by_name_symbo( name ) : nullptr;
}

c_obj_ui*	c_obj_ui::find_by_name_symbo( o_str CONST & name )
{
	c_obj_ui* o = find_down_by_name_symbo( name );
	if( o )
		return o;

	o = get_root();
	//todo we should avoid parsing our own tree again
	return o ? o->find_by_name_symbo( name ) : nullptr;
}

c_obj_ui*	c_obj_ui::find_up_by_name_symbo( o_str CONST & name )
{
	if( is_name_symbo( name ) )
		return this;

	c_obj_ui* o = get_root();
	return o ? o->find_up_by_name_symbo( name ) : nullptr;
}

c_obj_ui*	c_obj_ui::find_by_class_name( C_PCHAR_C class_name )
{
	c_obj_ui* o = find_down_by_class_name( class_name );
	if( o )
		return o;

	//todo we reparse the previous sub tree every time
	o = get_root();
	return o ? o->find_by_class_name( class_name ) : nullptr;
}

c_obj_ui*	c_obj_ui::find_up_by_factory( c_factory_base CONST * CONST factory )
{
	c_obj_ui* o = this;
	while( o )
	{
		if( o->get_factory() == factory )
			return o;
		o = o->get_root();
	}
	return nullptr;
}

c_obj_ui*	c_obj_ui::find_up_by_class_name( C_PCHAR_C class_name )
{
	if( is_class_name(class_name) )
		return this;

	c_obj_ui* o = get_root();
	return o ? o->find_up_by_class_name( class_name ) : nullptr;
}

//	this is a static fn
//todoqqq use the algo part of the stl
c_obj_ui*	c_obj_ui::find_from_top_by_class_and_name_symbo( C_PCHAR_C class_name, o_str CONST & o_sym )
{
	if( !o_sym.is_empty() )
	{
		c_factory_base*	pf = c_factory_base::get_by_cid( class_name );
		if( pf )
		{
			for(  auto const & p_obj : *(pf->get_objs_list()) )
			{
				if( p_obj->is_name_symbo(o_sym) )
					return p_obj;
			}
		}
	}
	return nullptr;
}

c_obj_ui*	c_obj_ui::find_down_by_class_name( C_PCHAR_C class_name )
{
	//todo	find the factory from the class_name and then search by factory should be faster
	if( is_class_name( class_name ) )
		return this;

	return _branch ? _branch->find_down_by_class_name( class_name ) : nullptr;
}

c_obj_ui*	c_obj_ui::find_down_by_class_and_name_symbo_except( C_PCHAR_C class_name, o_str CONST & o_sym, c_obj_ui* o_no )
{
	//todo	find the factory from the class_name and then search by factory should be faster
	if( is_name_symbo( o_sym ) && is_class_name( class_name ) )
		return this;

	return _branch ? _branch->find_down_by_class_and_name_symbo_except( class_name, o_sym, o_no ) : nullptr;
}

c_obj_ui*	c_obj_ui::find_down_by_class_and_name_symbo( C_PCHAR_C class_name, o_str CONST & o_sym )
{
	//todo	find the factory from the class_name and then search by factory should be faster
	if( is_name_symbo( o_sym ) && is_class_name( class_name ) )
		return this;

	return _branch ? _branch->find_down_by_class_and_name_symbo( class_name, o_sym ) : nullptr;
}

c_obj_ui*	c_obj_ui::find_by_class_and_name_symbo_except( C_PCHAR_C class_name, o_str CONST & o_sym, c_obj_ui* o_no )
{
	c_obj_ui* o = find_down_by_class_and_name_symbo_except( class_name, o_sym, o_no );
	if( o )
		return o;

	//todo we should avoid parsing our own tree again
	o = get_root();
	return o ? o->find_by_class_and_name_symbo_except( class_name, o_sym, this ) : nullptr;
}

c_obj_ui*	c_obj_ui::find_by_class_and_name_symbo( C_PCHAR_C class_name, o_str CONST & o_sym )
{
	c_obj_ui* o = find_down_by_class_and_name_symbo( class_name, o_sym );
	if( o )
		return o;

	//todo we should avoid parsing our own tree again
	o = get_root();
	return o ? o->find_by_class_and_name_symbo_except( class_name, o_sym, this ) : nullptr;
}

c_obj_ui*	c_obj_ui::find_up_by_class_and_name_symbo( C_PCHAR_C class_name, o_str CONST & o_sym )
{
	if( is_name_symbo( o_sym ) && is_class_name( class_name ) )
		return this;

	c_obj_ui* o = get_root();
	return o ? o->find_up_by_class_and_name_symbo( class_name, o_sym ) : nullptr;
}

//	this is a static fn
//todoopt extend factory to do it like find_by_class_and_name_symbo above
c_obj_ui*	c_obj_ui::find_from_top_by_class_start_and_name_symbo( C_PCHAR_C class_name, INT32 nb, o_str CONST & o_sym )
{
	if( !o_sym.is_empty() )
	{
#if AAA_USE_GLIST()
		for( auto const & p_obj : g_list() )
		{
#else
		for( UINT32 i=ID_MIN; i<=ID_MAX; i++ )
		{
			c_obj_ui * const p_obj = *(id_to_obj + i);
			if( !p_obj )
				continue;
#endif			
			if( p_obj->is_name_symbo(o_sym) )
			{
				if( str_is_equal( p_obj->get_class_name(), class_name, nb ) )
					return p_obj;
			}
		}
	}
	return nullptr;
}

c_obj_ui*	c_obj_ui::find_down_by_class_start_and_name_symbo(	C_PCHAR_C class_name, INT32 nb, o_str CONST & o_sym )
{
	if( is_name_symbo( o_sym ) && str_is_equal( get_class_name(), class_name, nb ) )
		return this;

	return _branch ? _branch->find_down_by_class_start_and_name_symbo( class_name, nb, o_sym ) : nullptr;
}

c_obj_ui*	c_obj_ui::find_by_class_start_and_name_symbo(			C_PCHAR_C class_name, INT32 nb, o_str CONST & o_sym )
{
	c_obj_ui* o = find_down_by_class_start_and_name_symbo( class_name, nb, o_sym );
	if( o )
		return o;

	//todo we should avoid parsing our own tree again
	o = get_root();
	return o ? o->find_by_class_start_and_name_symbo(  class_name, nb, o_sym ) : nullptr;
}

c_obj_ui*	c_obj_ui::find_up_by_class_start_and_name_symbo(		C_PCHAR_C class_name, INT32 nb, o_str CONST & o_sym )
{
	if( is_name_symbo( o_sym ) && str_is_equal( get_class_name(), class_name, nb ) )
		return this;

	c_obj_ui* o = get_root();
	return o ? o->find_up_by_class_start_and_name_symbo( class_name, nb, o_sym ) : nullptr;
}

/*
c_obj_ui*	c_obj_ui::find_by_filename( C_PCHAR_C filename )
{
list<c_obj_ui*>&			the_list = g_list();
list<c_obj_ui*>::iterator	it;
INT32						len;
	len = strlen( filename);
	for( it=the_list.begin(); it!=the_list.end(); ++it )
		{
		o_str*	str;
		str = &(*it)->my_filename;
		if( str->get_len() == len && strcmp( str->get(), filename) == 0 )
			return *it;
		}
	return nullptr;
}
*/

c_obj_ui*	c_obj_ui::find_from_top_by_name_search( C_PCHAR_C filename )
{
	return g_map().get( filename );
}

c_obj_ui*	c_obj_ui::find_first_by_class_name(		C_PCHAR_C class_name )
{
	c_factory_base*	pf = c_factory_base::get_by_cid( class_name );
	return pf ? pf->get_obj_first() : nullptr;
}

//c_obj_ui*	c_obj_ui::find_from_top_by_class_name_and_param(	C_PCHAR_C class_name, c_param CONST * CONST par )
//{
//	c_factory_base*	pf = c_factory_base::get_by_cid( class_name );
//	return pf ? pf->find_obj_by_param( par ) : nullptr;
//}

c_obj_ui*	c_obj_ui::find_from_top_by_name(		C_PCHAR_C name )
{
	c_obj_ui* obj = find_from_top_by_name_search( name );	//this one first because the fastest for now
	if( obj )
		return obj;
	return find_from_top_by_name_symbo( name );
}

c_bdd* c_obj_ui::find_bdd_by_name_symbo(			o_str CONST & o_sym )
{
	return (c_bdd*)	find_by_class_start_and_name_symbo( "bdd", 3, o_sym );
}

void	c_obj_ui::show_orphan()
{
#if AAA_USE_GLIST()
	for( auto const & p_obj : g_list() )
	{		
#else
	for( UINT32 i=ID_MIN; i<=ID_MAX; i++ )
	{
		c_obj_ui const * p_obj = *(id_to_obj + i);
		if( !p_obj )
			continue;
#endif
		c_obj_ui const * const r = p_obj->get_root();
		if( !r )
			ERR_PRINT_STRING( "NULL root for obj(%u) %s", p_obj->get_obj_ui_id(), p_obj->get_name_str() );
		else if ( r->is_the_root() )
			VERBOSE_PRINT_STRING( "the_root is the root of obj(%u) %s",  p_obj->get_obj_ui_id(), p_obj->get_name_str() );
	}
}

//////
////	REF
//
#define ID_AS_REF() 1

UINT32	c_obj_ui::ref_obj_nb	= 0;
UINT32	c_obj_ui::ref_param_nb	= 0;
o_str	c_obj_ui::ref_error;	

namespace{
#if ID_AS_REF()
	FINLINE CONSTEXPR UINT32	extract_ref_id(		UINT32 ref )					{ return ref & c_obj_ui::MASK_REF; }
#else
	std::vector<c_obj_ui*>						ref_objs
	std::unordered_map< c_obj_ui*, UINT32 >		map_ref_objs;
#endif
	std::vector<c_param *>						ref_params;
	std::unordered_map< c_param *, UINT32 >		map_ref_params;	//todo use it

	FINLINE CONSTEXPR UINT32	extract_ref_index(	UINT32 ref )					{ return (ref & c_obj_ui::MASK_REF) - 1; }
}
//todo check overflow
UINT32	c_obj_ui::get_obj_ref()
{
	if( mem::is_pointer_fucked(this) )
	{
		debug_break( "%s() with a fucked pointer this should never happen, we return 0 but this is a problem", __FUNCTION__ );
		return 0;
	}

	UINT32 ref;
#if ID_AS_REF()
	c_param* CONST header = get_param_header();
	if( !header->is_referenced() )
	{
		header->set_referenced();
		++ref_obj_nb;
	}
	ref = get_obj_ui_id();
	set_ref_obj( ref );
#else
	auto it	= map_ref_objs.find( this );
	if( it == map_ref_objs.end() )
	{
		ref_objs.push_back( this );
		ref_obj_nb = (UINT32)ref_objs.size();//todo add a check on overflow
		ref = ref_obj_nb;
		set_ref_obj( ref );
		map_ref_objs[this] = ref;
		p_param param = get_param_header();
		param->set_referenced();
	}
	else
		ref = it->second;
#endif
	return ref;
}

void	c_obj_ui::remove_obj_ref()
{
	IF_THIS_NULL_RETURN();

	c_param* CONST header = get_param_header();
	if( !header->is_referenced() )
		return;
	header->clear_referenced();

#if ID_AS_REF()
	--ref_obj_nb;
#else
	auto it	= map_ref_objs.find( this );
	if( it == map_ref_objs.end() )
	{
		debug_break( "%s() don't find the obj in map_ref_objs", __FUNCTION__ ); 
		return;
	}

	UINT32 ref_index = extract_ref_index(it->second);
	map_ref_objs.erase( it );
	ref_objs[ref_index] = nullptr;	//	mark as deleted, this pointer is now lost memory but carry the deleted information
#endif
}

//c_obj_ui*	c_obj_ui::get_obj_from_ref_direct( UINT32 ref )
//{
//	return objs_ref[ extract_ref_index(ref) ];
//}

bool		c_obj_ui::is_obj_ref( UINT32 ref )
{
	if( !is_ref_obj(ref) )
		return false;

#if ID_AS_REF()
	return get_from_id( extract_ref_id(ref) ) != nullptr;
#else
	ref = extract_ref_index(ref);
	if( ref >= ref_obj_nb  )
		return false;
	return ref_objs[ref] != nullptr;
#endif
}

c_obj_ui*	c_obj_ui::get_obj_from_ref( UINT32 ref )
{
	if( !is_ref_obj(ref) )
	{
		if( is_ref_param(ref) )
			set_ref_error( "this a param ref not an object ref as expected" );
		//error_and_escape(  "0x%x is a param ref not an object ref as expected" , ref );
		else if( ref == 0 )
			set_ref_error( "object ref should not be 0" );
		else
			set_ref_error( "this is not a valid ref" );
		//error_and_escape(  "0x%x is not a valid ref" , ref );
		return nullptr;
	}

	c_obj_ui* obj;
#if ID_AS_REF()
	ref = extract_ref_id( ref );
	obj = get_from_id( ref );
#else
	ref = extract_ref_index(ref);
	if( ref >= ref_obj_nb )
	{
		set_ref_error( "bigger than number of object ref" );
		//error_and_escape(  "obj_ref 0x%x not valid only %d refs" , ref, objs_ref.size() );
		return nullptr;
	}

	obj = ref_objs[ref];
#endif
	if( !obj )
		set_ref_error( "obj ref is invalid, the object was deleted" );
	return obj;
}


//todo check overflow
UINT32	c_obj_ui::get_param_ref( c_param * CONST param )
{
	if( !param )
		return 0;

	UINT32	ref;
	auto it	= map_ref_params.find( param );
	if( it == map_ref_params.end() )
	{
		ref_params.push_back( param );
		ref_param_nb = (UINT32)ref_params.size();
		ref = ref_param_nb;
		set_ref_param(ref);
		map_ref_params[param] = ref;
		param->set_referenced();
	}
	else
		ref = it->second;
	return ref;
}

void	c_obj_ui::remove_param_ref( c_param * CONST param )
{
	if( !param )
	{
		debug_break( "%s() param is a nullptr", __FUNCTION__ ); 
		return;
	}

	if( !param->is_referenced() )
	{
		debug_break( "%s() param is not referenced", __FUNCTION__ ); 
		return;
	}

	auto it	= map_ref_params.find( param );
	if( it == map_ref_params.end() )
	{
		debug_break( "%s() don't find the param in map_ref_params", __FUNCTION__ ); 
		return;
	}

	param->clear_referenced();
	UINT32	ref_index = extract_ref_index(it->second);
	map_ref_params.erase( it );
	ref_params[ref_index] = nullptr;		//	mark as deleted, this pointer is now lost memory but carry the deleted information
}
//p_param	c_obj_ui::get_param_from_ref_direct( UINT32 ref )
//{
//	return params_ref[ extract_ref_index(ref) ];
//}
bool		c_obj_ui::is_param_ref( UINT32 ref )
{
	if( !is_ref_param(ref) )
		return false;

	ref = extract_ref_index(ref);
	if( ref >= ref_param_nb )
		return false;

	return ref_params[ref] != nullptr;
}

p_param		c_obj_ui::get_param_from_ref( UINT32 ref )
{
	if( !is_ref_param(ref) )
	{
		if( is_ref_obj(ref) )
			set_ref_error( "this a object ref not an param ref as expected" );
		//error_and_escape(  0x%x is an object ref not a param ref as expected" , ref );
		else if( ref == 0 )
			set_ref_error( "param ref should not be 0" );
		else
			set_ref_error( "this is not a valid ref" );
		//error_and_escape(  "0x%x is not a valid ref" , ref );
		return nullptr;
	}

	ref = extract_ref_index(ref);
	if( ref >= ref_param_nb )
	{
		set_ref_error( "bigger than number of param ref" );
		//error_and_escape( "param_ref 0x%x not valid only %d refs", ref, params_ref.size() );
		return nullptr;		
	}

	p_param param = ref_params[ref];
	if( !param )
		set_ref_error( "param ref is invalid, the param was deleted" );

	return param;
}



void	c_obj_ui::class_dealloc()
{
	DBG_PRINT_STRING( "%s() Delete obj left in global list Begin", __FUNCTION__ );
	b_clear_root_on_dealloc = false;
#if AAA_USE_GLIST()
	auto& l = g_list();
	while( !l.empty() )
	{
		auto pt = l.back();
		DBG_PRINT_STRING( "%s() will delete object %.64s of class %.64s", __FUNCTION__, pt->get_name_str(), pt->get_class_name() );
		if( pt == g_root )
			g_root = nullptr;	//avoid crash later
		SAFE_DELETE( pt );
	}
	DBG_PRINT_STRING( "%s() Delete obj left in global list End", __FUNCTION__ );
#else
//todo use g_map() ?
#endif
	/*
	g_map().dealloc();
	//hackqq
	//	c_obj_ui::the_factory().~c_factory_abstract<c_obj_ui>();
	//	c_obj_ui::the_factory().dealloc_dirty();
	map_ref_objs.clear();
	objs_ref.clear();
	map_ref_params.clear();
	params_ref.clear();
	*/
}

#define AAA_DEBUG_MAXIMUM() 0

void	c_obj_ui::obj_new_private( c_obj_ui* obj )
{
#if AAA_DEBUG_MAXIMUM()
	DBG_PRINT_STRING( "Begin obj_new_private %s", obj->get_class_name() );
#endif
	//hack for now
	if( obj->get_root() == c_obj_ui::get_the_root()->get_father() )	//force only if it a default root
	{
#if AAA_DEBUG_MAXIMUM()
		DBG_PRINT_STRING( "Before set_root()" );
#endif
		obj->set_root( this );
#if AAA_DEBUG_MAXIMUM()
			DBG_PRINT_STRING( "after set_root()" );
#endif
	}
#if AAA_DEBUG_MAXIMUM()
	DBG_HEAP_CHECK();
	DBG_PRINT_STRING( "End obj_new_private %s", obj->get_class_name() );
#endif
}

#define AAA_PARAM_HEADER_STORE_CONNEX() 1

c_obj_ui::c_obj_ui( c_factory_base* factory )
	:_factory			(	factory	)
	,_obj_ui_id			{	++obj_ui_nb_created | SIGNATURE_OBJ_EXIST << SIGNATURE_OBJ_EXIST_SHIFT }	//this a way to mark non deleted obj
	,_root				(	nullptr	)
#if !AAA_PARAM_HEADER_STORE_CONNEX() 
	,_outs				(	nullptr	)
	,_ins				(	nullptr	)
#endif
	,_branch			(	nullptr	)
	,_file_version		(	0		)
	,_namer				(	nullptr	)
	,_name_ui			(	nullptr	)
	,_b_obj_active_ui	(	true	)
	,_cell_render_frame	(	0		)
	,_lua_ud			(	nullptr	)
	//,_b_referenced	(	FALSE	)
{
	//todo this test don't work on c_obj_ui but on final derived classes
//	if( is_obj_first() )
//	{
//		DBG_PRINT_STRING( "c_obj_ui size %d",			sizeof(c_obj_ui) );
//		DBG_PRINT_STRING( "offset param is size %d", INT32( (CHAR*)(&_params) - (CHAR*)(this) ) );
//	}

#if AAA_DEBUG_MAXIMUM()
	DBG_PRINT_STRING( "Begin Constructor %s", factory->get_class_name() );
#endif

	++obj_ui_nb;
//c_factory_base*	facto = get_factory();

	auto g_root = c_obj_ui::get_the_root();
	//	root should be initialized first but when we construct root we can't call it yet
	set_root( g_root ? g_root->get_father() : nullptr );

	//todo this is call and changed later always
	//	it could be opt

	if( !get_factory() )
		debug_break( "create obj NULL FACTORY" );
	else
	{
		c_obj_ui::OBJ_UI_ID id = get_obj_ui_id();
		*(id_to_obj+id) = this;
		get_factory()->register_obj( this ); 
#if AAA_USE_GLIST()
		g_list().push_back( this );	
#endif

		C_PCHAR_C str = get_class_name();
		if( !str )
			debug_break( "%s() can't get class_name", __FUNCTION__ );
//		else
//			PRINT_STRING( "create obj %s %d\n", str, get_factory()->get_count() );
	}

	clear_v3( _cell_pos );
	_cell_size = REAL(.2);

#if AAA_DEBUG_MAXIMUM()
	DBG_PRINT_STRING( "End Constructor %s", factory->get_class_name() );
#endif
}


//todo reunite with template<class T> T*	c_obj_ui::obj_new( T*& obj )
c_obj_ui*	c_obj_ui::obj_get( c_obj_ui*& obj, c_factory_base* pf )
{
	if( !obj )
	{
		obj = pf->create_obj();
		if( obj )
			obj_new_private( obj );
		else
			debug_break( "%s() can't allocate object", __FUNCTION__ );
	}
	return obj;
}

c_obj_ui::~c_obj_ui()
{
	if( b_aaa_exiting && b_aaa_exiting_fast )
		return;

	param_focus_remove( this, get_param_header() );
	if( aaa::param::flat::obj_found == this )
		aaa::param::flat::obj_found = nullptr;

	unplug_in_all();
	unplug_out_all();

//todo do header being destroyed is enough ?
#if AAA_PARAM_HEADER_STORE_CONNEX()
#else
	obj_delete( _ins );
	obj_delete( _outs );
#endif

	SAFE_DELETE( _name_ui );

	C_PCHAR_C pt = get_name_search_str();
	if( pt && *pt )
		g_map().sign_out( pt );

//todo should we delete the branches ?
	if( _branch )
	{
		INT32 nb = _branch->get_item_nb();
		if( nb )
			err_print( "destructing object with non empty _branch" );
		obj_delete( _branch );
	}

	if( _namer )
		obj_delete( _namer );

	if( b_clear_root_on_dealloc )
		set_root( nullptr );

	remove_obj_ref();
#if AAA_USE_GLIST()
	g_list().remove( this );
#endif

	get_factory()->unregister_obj( this ); 
	c_obj_ui::OBJ_UI_ID id = get_obj_ui_id();
	*(id_to_obj+id) = nullptr;
	clear_obj_exist(_obj_ui_id);

#if AAA_DEBUG()
	if( obj_ui_nb == 0 )
		debug_break( "the count of obj_ui is already 0 when trying to destruct one" );
	else
#endif
		--obj_ui_nb;
}

c_obj_ui*	c_obj_ui::get_obj_sub_by_index( INT32 CONST index ) CONST
{
	return nullptr;
}

void	c_obj_ui::print_string( C_PCHAR_C fmt, ... ) CONST
{
	va_list args;
	va_start( args, fmt );
	OBJ_PRINT_STRING_VA( get_name_human().get(), fmt, args );
	va_end( args );
}

namespace {
	o_str name_dbg_helper;
}


bool	c_obj_ui::do_dialog_forget( C_PCHAR_C what, INT32 CONST index )
{
	c_obj_ui* obj = get_obj_sub_by_index( index );

	bool b_ok = false;
	o_str& o = o_str::push_name();
		if( !obj )
		{
			o.set( get_class_name() );
			o.add_newline();
			o.add( " sub obj "  );
			o.add_space();
			o.add( what );
			o.add_space();
			o.add( index );
			o.add( " don't exist\nso we can't forget it" );
			BOX_TITLE_ERR( get_name_str(), o.get() );
		}
		else
		{
			//o.set( obj->get_my_filename() );
			//o.set( root->get_name_str() );
			o.set( get_name_str() );
			o.add_newline();
			//o.add( obj->get_name_str() );
			o.add( obj->get_name_str() );
			o.add_newline();
			o.add( "Forget/delete this " );
			o.add( obj->get_class_name() );
			o.add( " ?" );
			b_ok = MESSAGE_BOX( 0, nullptr, o.get(), "Confirmation", MB_OKCANCEL | MB_ICONWARNING | MB_TOPMOST, IDCANCEL ) == IDOK;
		}
	o_str::pop_name();
	return b_ok;
}

o_str CONST & 	c_obj_ui::get_name_dbg() CONST
{
	C_PCHAR name = get_name_str();
	if( name && *name==0 )
		name_dbg_helper.set( name );
	else
		name_dbg_helper.erase();

	UINT32 id = get_obj_ui_id();
	name_dbg_helper.add_char( '(' );
	name_dbg_helper.add( id );
	name_dbg_helper.add_char( ')' );
	
	if( is_my_filename() )
	{
		name_dbg_helper.add( ", filename " );
		name_dbg_helper.add( _my_filename );
	}
	else
	{
		c_namer*	namer = get_namer();
		if( namer )
		{
			name_dbg_helper.add( ", dir " );
			name_dbg_helper.add( namer->get_dir() );
			name_dbg_helper.add( ", file " );
			name_dbg_helper.add( namer->get_file() );
		}
	}

	return name_dbg_helper;
}

void	c_obj_ui::err_print_before() CONST
{
	ERR_PRINT_STRING( "in object %s,", get_name_dbg().get() );
}
void	c_obj_ui::err_print( C_PCHAR_C fmt, ... ) CONST
{
	va_list args;
	va_start( args, fmt );
	err_print_before();
	if( fmt )
		ERR_PRINT_STRING_VA( fmt, args );
	va_end(args);
}

void	c_obj_ui::dbg_print_before() CONST
{
	DBG_PRINT_STRING( "in object %s,", get_name_dbg().get() );
}
void	c_obj_ui::dbg_print( C_PCHAR_C fmt, ... ) CONST
{
	va_list	args;
	va_start( args, fmt );
	dbg_print_before();
	if( fmt )
		DBG_PRINT_STRING_VA( fmt, args );
	va_end(args);
}

void	c_obj_ui::ui_print_before() CONST
{
	UI_PRINT_STRING( "in object %s,", get_name_dbg().get() );
}
void	c_obj_ui::ui_print( C_PCHAR_C fmt, ... ) CONST
{
	va_list	args;
	va_start( args, fmt );
	ui_print_before();
	if( fmt )
		UI_PRINT_STRING_VA( fmt, args );
	va_end(args);
}

void	c_obj_ui::box_err( C_PCHAR_C fmt, ... ) CONST
{
	va_list args;
	va_start( args, fmt );
	err_print_before();
	BOX_TITLE_ERR_VA( get_name_str(), fmt, args );
	va_end(args);
}

void	c_obj_ui::err_print_method_unimplemented(	C_PCHAR_C fn_name ) CONST
{
	err_print( "method %s() not implemented", fn_name );
}
void	c_obj_ui::err_print_unused(	C_PCHAR_C name, C_PCHAR_C fn_name ) CONST
{
	err_print( "method %s() not implemented, %s unused (not included in this executable version)", fn_name, name );
}

/*
void	c_obj_ui::tbuf_add( INT32 channel_id, REAL val_in, CHAR* tex_in )
{
	tbuf_add( channel_id, val_in, tex_in, (void*)this );
}
void	c_obj_ui::tbuf_inc( INT32 channel_id, REAL val_in, CHAR* tex_in )
{
	tbuf_inc( channel_id, val_in, tex_in, (void*)this );
}
void	c_obj_ui::tbuf_dec( INT32 channel_id, REAL val_in, CHAR* tex_in )
{
	tbuf_dec( channel_id, val_in, tex_in, (void*)this );
}
*/
/*
now unuseful with param duplicated by instance 
void	c_obj_ui::param_connex_set()
{
//	return;
INT32		i;
INT32		nb;
p_param	par;
c_connex*	con;
	
	param_header_->remove_out();
	param_header_->remove_in();

	nb = get_param_nb();
	for( i=0; i<nb; ++i )
		{
		par = get_param(i);
		par->remove_out();
		par->remove_in();
		}

	nb = get_in_nb();
	for( i=0; i<nb; ++i )
		{
		con = get_in_connex(i);
		con->get_out_param()->add_in( con );
		}

	nb = get_out_nb();
	for( i=0; i<nb; ++i )
		{
		con = get_out_connex(i);
		con->get_in_param()->add_out( con );
		}
}
*/

INT32	c_obj_ui::add_out_connex( c_connex* con )
{
//parnow
#if AAA_PARAM_HEADER_STORE_CONNEX() 
	get_param_header()->add_out( con );
#else
	if( !_outs )
	{
		try
		{
			//todofranz dealloc
			_outs = new c_connex::LIST;
		}
//		catch( bad_alloc& e)
		catch(...)
		{
			box_err( "c_obj_ui::add_out_connex() : Can't allocate connections_out table." );
			return	-1;
		}
	}
	_outs->insert_at_tail( con );
#endif
	return 0;	//todoq do some try catch at the next levels
}
//todo return err_code
INT32	c_obj_ui::add_in_connex( c_connex* con )
{
//parnow
#if AAA_PARAM_HEADER_STORE_CONNEX() 
	get_param_header()->add_in( con );
#else
	if( !_ins )
	{
		try
		{
			//todonow dealloc
			_ins = new c_connex::LIST;
		}
//		catch( bad_alloc& e)
		catch(...)
		{
			box_err( "c_obj_ui::add_in_connex() : Can't allocate connections_in table.");
			return	-1;
		}
	}
	_ins->insert_at_tail( con );
#endif
	return 0;	//todoq do some try catch at the next levels
}

c_connex*	c_obj_ui::remove_out_connex( c_connex* con )
{
#if AAA_PARAM_HEADER_STORE_CONNEX() 
	get_param_header()->remove_out( con );
#else
	if( is_out() )
	{
		_outs->remove_it( con );
//		if( !is_out() )
//		{
//todonow check param_header dont point to it
//			obj_delete( _outs );
//		}
		return con;
	}
	DBG_PRINT_STRING( "%s : Can't because empty connections table.", __FUNCTION__ );
#endif	
	return nullptr;
}
c_connex*	c_obj_ui::remove_in_connex( c_connex* con )
{
#if AAA_PARAM_HEADER_STORE_CONNEX()
	get_param_header()->remove_in( con );
#else
	if( is_in() )
	{
		_ins->remove_it( con );
//		if( is_in() == 0 )
//todonow check param_header don't point to it
//			obj_delete( _ins );
		return con;
	}

	DBG_PRINT_STRING( "%s : Can't because empty connections table.", __FUNCTION__ );
#endif
	return nullptr;
}

//c_connex*	c_obj_ui::remove_out_index( INT32 index )
//{
//#if AAA_PARAM_HEADER_STORE_CONNEX() 
//	get_param_header()->remove_out( index );
//#else
//	if( is_out() )
//	{
//		c_connex* con = _outs->remove(index);
////		if( !is_out() == 0 )
////		{
////todonow check param_header dont point to it
////			obj_delete( _outs );
////		}
//		return con;
//	}
//#endif
//	box_err( "c_obj_ui::remove_out_index() : Can't because empty connections_out table." );
//	return nullptr;
//}

//hack	dangerous do not use connex don't have a destructor for now 2013 Jan


AAA_ERR		c_obj_ui::unplug_out_all()
{
#if AAA_PARAM_HEADER_STORE_CONNEX()
	if( auto const outs = get_param_header()->get_out() )
		outs->destroy_all_hack_connex();
#else
	if( _outs )
		_outs->destroy_all_hack_connex();
#endif
	return AAA_OK;
}
//hack	dangerous do not use connec don't have a destructor for now 2013 Jan
AAA_ERR		c_obj_ui::unplug_in_all()
{
#if AAA_PARAM_HEADER_STORE_CONNEX()
	if( auto const ins = get_param_header()->get_in() )
		ins->destroy_all_hack_connex();
#else
	if( _ins )
		_ins->destroy_all_hack_connex();	//hack this have a bad name
#endif
	return AAA_OK;
}

//unused
AAA_ERR		c_obj_ui::remove_out_all()
{
	unplug_out_all();
#if AAA_PARAM_HEADER_STORE_CONNEX()
//todo not sure
//	auto const outs = get_param_header()->get_out();
//	delete outs;
#else
	obj_delete( _outs );
#endif
	return AAA_OK;
}
//unused
AAA_ERR		c_obj_ui::remove_in_all()
{
	unplug_in_all();
#if AAA_PARAM_HEADER_STORE_CONNEX()
//todo not sure
//	auto const ins = get_param_header()->get_in();
//	delete ins;
#else
	obj_delete( _ins );
#endif
	return AAA_OK;
}


//c_connex*	c_obj_ui::remove_in_index( INT32 index )
//{
//	if( is_in() )
//	{
//		c_connex* con = _ins->remove(index);
////		if( is_in() == 0 )
////todonow check param_header don't point to it
////			obj_delete( _ins );
//		return con;
//	}
//
//	box_err( "c_obj_ui::remove_in_index() : Can't because empty connections_in table." );
//	return nullptr;
//}




INT32		c_obj_ui::add_branch( c_obj_ui* obj )
{
	if( !_branch )
	{
		try
		{
			//todofranz dealloc
			_branch = new c_list_pt_active<class c_obj_ui>;
		}
//		catch( bad_alloc& e)
		catch(...)
		{
			box_err("Can't allocate branch table in c_obj_ui");
			return	-1;
		}
	}
	_branch->insert_at_tail(obj);
	return 0;	//todoq do some try catch at the next levels
}

c_obj_ui*	c_obj_ui::find_branch_by_name_symbo( o_str& name ) CONST
{
	if( _branch )
		return _branch->find_by_name_symbo( name );

	return nullptr;
}

c_obj_ui*	c_obj_ui::find_branch_by_class_name(	C_PCHAR_C class_name ) CONST
{
	if( _branch )
		return _branch->find_by_class_name( class_name );

	return nullptr;
}

void	c_obj_ui::find_branchs_by_class_name(		C_PCHAR_C class_name, std::vector<c_obj_ui*>& vec ) CONST
{
	if( _branch )
		_branch->find_by_class_name( class_name, vec );
}

c_obj_ui*	c_obj_ui::find_branch_by_my_filename(	C_PCHAR_C name ) CONST
{
	if( _branch )
		return _branch->find_by_my_filename( name );

	return nullptr;
}


c_obj_ui*	c_obj_ui::remove_branch( c_obj_ui* obj )
{
	if( is_branch() )
		return _branch->remove_it(obj);

	box_err("Can't remove branch because empty branch table in c_obj_ui" );
	return nullptr;
}

c_obj_ui*	c_obj_ui::remove_branch_index( INT32 index )
{
	if( is_branch() )
		return _branch->remove(index);
	
	box_err("Can't remove branch because empty branch table in c_obj_ui" );
	return nullptr;
}

AAA_ERR		c_obj_ui::remove_branch_all()
{
	for( INT32 i = get_branch_nb() - 1; i >= 0; --i )
		_branch->remove( i );
	return AAA_OK;
}

/*
AAA_ERR	c_obj_ui::destroy_branch(c_obj_ui* obj)
{
	if( is_branch() )
		{
		INT32	i = branch->found(obj);
		if ( i>=0 )
			return destroy_branch_index(i);
		}
	else
		box_err("Can't destroy branch because empty branch table in c_obj_ui");
	return nullptr;
}


AAA_ERR	c_obj_ui::destroy_branch_index(INT32 index)
{
	if( is_branch() )
		{
		delete branch->remove(index);
		return	AAA_OK;
		}
	else
		box_err("Can't destroy branch because empty branch table in c_obj_ui");
	return ERR_ANY;
}


AAA_ERR		c_obj_ui::destroy_branch_all()
{
	INT32	i;
	for( i=get_branch_nb()-1; i>=0; --i )
		destroy_branch_index(i);
	return AAA_OK;
}
*/

void	c_obj_ui::set_focus()
{
	IF_THIS_NULL_RETURN();
	param_focus_set( this, nullptr );
}
 
namespace {
#if AAA_DEBUG()
	INT32 name_ui_nb = 0;
	FINLINE void inc_name_ui_count()
	{
		++name_ui_nb;
	}
#else
	FINLINE void inc_name_ui_count() {}
#endif
}

void	c_obj_ui::set_name( o_str CONST & name_in )
{
	if( !_name_ui )
	{
		_name_ui = new o_str( name_in );
		inc_name_ui_count();
	}
	else if( !_name_ui->is_equal(name_in) )	//todo useful test ?
		_name_ui->set( name_in );
	else
		debug_break( "Now Maa you can decide what should happen here" );
}

//todo do better ? add id here ? do it inside o_str ?
void	c_obj_ui::set_name( C_PCHAR_C name_in )
{
	if( !_name_ui )
	{
		_name_ui = new o_str( name_in );
		inc_name_ui_count();
	}
	else if( !_name_ui->is_equal(name_in) )	//todo useful test ?
		_name_ui->set( name_in );
	else
		debug_break( "Now Maa you can decide what should happen here" );
}

void	c_obj_ui::set_name_with( C_PCHAR_C start_name, INT32 CONST index )
{
	o_str* o = _name_ui;
	if( !o )
	{
		o = new o_str;
		inc_name_ui_count();
		_name_ui = o;
	}
	o->set( start_name );
	o->add_char( '_' );
	o->add( index );
}

void	c_obj_ui::init_name_with( C_PCHAR_C start_name, INT32 CONST index )
{
	if( _name_ui && !_name_ui->is_empty() )
		debug_break( "%s(%256s,%d) non NULL obj_name at init", __FUNCTION__, start_name, index );

	set_name_with( start_name, index );
}

void	c_obj_ui::init_name_with(	C_PCHAR_C start_name )
{
	init_name_with( start_name, get_factory()->get_obj_created_nb() );
}

void	c_obj_ui::add_to_name(		C_PCHAR_C str )
{
	if( !_name_ui )
		set_name( get_name_str() );
	if( _name_ui )
		_name_ui->add( str );
	else
		debug_break( "%s() try to add to a NULL name", __FUNCTION__ );
}

//hack
C_PCHAR_C	c_obj_ui::get_name_str() CONST
{
	if( _name_ui )
		return _name_ui->get();
	else				
		return get_name_human().get();
}

void	c_obj_ui::prepare_for_ui()	// called by param::draw(), in charge of calling param_init_pt() and do more if needed
{
//	param_init_pt();	//update the pointer of params using non static pointer (pointer not set in optional param_init_pt_static() and not changing )

//	param_connex_set();
}

bool	c_obj_ui::param_do_action( c_param * CONST par, aaa::param::ACTION CONST action )
{
	return false;
}

void	c_obj_ui::make_param_header_name() 
{
	p_param param_header = get_param_header();
	if( param_header )
	{
		CHAR str[256];
		INT32 len = sprintf( str, "%.64s(%u)%.64s", get_class_name(), get_obj_ui_id(), get_name_str() );
		param_header->set_name( str );
	}
	else
		//param make a macro for all this serious problem
		debug_break( "%s() obj_ui have no header, this should not happen.", __FUNCTION__ );
}

//void	c_obj_ui::set_param_header( p_param param ) //par
//{
//	_params.set_param_header( param, this );
//}

//todo split between object and and param  
void	c_obj_ui::set_param_value( C_PCHAR_C param_name, C_PCHAR_C param_value, bool CONST b_str )
{
	IF_THIS_NULL_RETURN();
	{
		//par
		p_param	param = get_param_by_name( param_name );
		if( param )
		{
			param_init_pt();	//todonow 2023 Feb M�a now needed this for the moment (at least for non static param) 
								//	but used only by command_do for now
			//todonow 2023 Feb M�a changed this below not sure it is the best way, do_commande should be checked
			if( b_str )
				param->set_value_from_str( param_value );
			else if( param->is_type_text() )
				param->set_value_str( param_value );
			else
				param->set_value_num_from_double( atof( param_value ) );
		}
	}
}

void	c_obj_ui::param_init_with( c_param_def CONST * CONST pt, INT32 CONST nb )	//par
{
	SPY_PUSH_RANGE( "param_init_with", spy::INFRA );
		//	allocate data the first time for this class
		//		we use c_param_def to feed a structure holdding c_param_data inside the object factory
		//		declaration of param are done thru c_param_def for historical reasons,
		//			infact it is the only use of the c_param_def class
		if( pt )
		{
			auto factory = get_factory();
			INT32 i = factory->get_param_data_size();
			//par to alloc at a upper level
			// if param_data count is below nb we need to add data 
			//		some obj_ui like module or layer could have more of less of an array at the end
			//		so we init first then complete when needed
			// the factory hold a list of pointer on param_data allocated one by one
			//todo should we do better ?
			for( ; i < nb; ++i )
				//todo franz dealloc
				factory->push_param_data( &pt[i] );
		}

		if( _params.get_param_nb_allocated()!=0 )
		{
			if( nb!=0 )	//	needed for example because of c_spaceball
				debug_break( "%s() _params should not exist yet.", __FUNCTION__ );
		}
		else
		{
			_params.init_for_obj( this, nb );	//	we pass nb because of trax
												//		allocation of param is done there
			if( c_param::b_verbose )
				VERBOSE_PRINT_STRING( "param_init_with for %s", get_name_str() );

			make_param_header_name();
			param_init_pt_static();
#if	AAA_DEBUG()
			c_param::push_check();
#endif
				param_init_to_ina();
#if	AAA_DEBUG()
			c_param::pop_check();
#endif
			//	if ( param_init != c_obj_ui::param_init )
			param_init();
		}
	SPY_POP_RANGE();
}

void	c_obj_ui::param_init_to_ina()
{
	SPY_PUSH_RANGE( "param_init_to_ina", spy::INFRA );
		param_init_pt();	//todo call only once
		param_list_init_value_to_ina( get_param_begin(), get_param_nb_allocated() );	//todo perhaps we avoid to pass parameters
	SPY_POP_RANGE();
}

void	c_obj_ui::param_set_to_ina()
{
	SPY_PUSH_RANGE( "param_set_to_ina", spy::INFRA );
		param_init_pt();	//todo it seems it is call twice at init
		param_list_set_value_to_ina( get_param_begin(), get_param_nb_allocated() );	//todo perhaps we avoid to pass parameters
	SPY_POP_RANGE();
}

void	c_obj_ui::param_set_to_def()
{
	SPY_PUSH_RANGE( "param_set_to_def", spy::INFRA );
		param_init_pt();	//todo it seems it is call twice at init
		param_list_set_value_to_def( get_param_begin(), get_param_nb_allocated() );	//todo perhaps we avoid to pass parameters
	SPY_POP_RANGE();
}

//todo have a generic mecanism and err_param_init_pt_static() return h for check 
#if	AAA_DEBUG()
void	c_obj_ui::err_param_init_pt( INT32 nb_in )
{
	if( nb_in != get_param_nb_used() )
	{
		CHAR mess[1024];
		sprintf( mess, "%.128s::param_init_pt() not everything Initialize correctly. %d used for %d params declared", get_class_name(), nb_in, get_param_nb_used() );
		box_err( mess );
		debug_break( mess );
	}
}
#endif


void c_obj_ui::param_init()
{
#if AAA_DEBUG()
//	DBG_PRINT_STRING("c_obj_ui::param_init() called");
#endif
}
void c_obj_ui::param_init_pt_static()
{
}

void	c_obj_ui::update_then_draw()
{
	update();
	draw();
}

bool	c_obj_ui::update_then_draw_lua()
{
	update_then_draw();
	return true;
}
void	c_obj_ui::update_lua()
{
	update();
}

C_PCHAR_C	c_obj_ui::get_my_filename() CONST
{
//CHAR* str;
	if( !is_my_filename() )
	{
#if	AAA_DEBUG() 
//		DBG_PRINT_STRING( "%s() filename is empty", __FUNCTION__ );
#endif
		//todonownow
//		str = get_filename_save_by_value( (TYPE_IO)get_type_io());
//		if( str )
//			set_my_filename(str);
	}
	return _my_filename.get();
}

C_PCHAR_C	c_obj_ui::set_my_filename( o_str CONST & filename )
{
	bool	b_erase = filename.is_empty();
//	if( !filename )
//	{
//		return nullptr;
//	}

	//aqua
	if( is_my_filename() || b_erase )		//leak this should not happen because it invalidate the concept of name_search
											// second test to let erase the filename with a pointer to null
		//problem when we set to "" in case of error
	{	// but when reading states this is normal			
#if AAA_STATE_COMPILE()
		if( c_state_master::is_state_ref() )
#endif //AAA_STATE_COMPILE
		{
			g_map().sign_out( _name_search.get() );
			_name_search.erase();
			_my_filename.erase();
			//	debug_break( "%s() when already set should not happen because of g_map : %s", __FUNCTION__, _my_filename.get() );
		}
	}
	if( !b_erase )
	{
		o_str& str = o_str::push_name();
			str.set_fname_relative( filename );
			//todo clarify this
			_my_filename.set( str );	//	name relative to the current dir
			if( c_module::get_cur() )
			{
				_name_search.set_fname_relative( c_dir::get_def(), c_dir::get_kernel() );
				if( _name_search.is_empty() )
				{
					if( str.is_fname_absolute() )
						_name_search.erase();
					else
						_name_search.set( "./" );
				}
				else
				{
					_name_search.convert_to_slash();
					_name_search.add_slash();
				}
			}
			else
			{
				_name_search.erase();
			}
			_name_search.add( str );	// name relative to the master dir (the app current dir)
//todo a lot of similar names (light ...), incomplete names  and eventually not event called on some obj
			if( b_searchable_by_filename )
				g_map().sign_in( _name_search.get(), _name_search.get_len(), this );
		o_str::pop_name();
	}
	return _my_filename.get();
}

INT32	c_obj_ui::get_file_version_save() CONST
{
	return 0;
}

FINLINE	void	c_obj_ui::check_ext( o_str CONST & o ) CONST
{
	if( !o.have_ext() )
	{
		debug_break( "file %s have no extension ", o.get() );
		//o.replace_ext( get_fname_ext() );
	}
}
bool	c_obj_ui::check_before_file_op( C_PCHAR_C filename ) CONST
{
	IF_THIS_NULL()
	{
		debug_break( "in %s() : can't be load/save with a null object.", __FUNCTION__ );
		return false;
	}
	if( !filename || *filename==0 )
	{
		debug_break( "Object_ui %s can't be loaded with no or Null filename.", get_name_str() );
		return false;
	}
	if( _params.get_param_nb_allocated()==0 )	//	no params we don't save e.g. bdd_empty
	{
		if( !is_class_name( "cameras" ) )
			debug_break( "Object_ui %s don't have params.", get_name_str() );
	}
	return true;
}
bool	c_obj_ui::check_before_file_op( o_str CONST & filename ) CONST
{
	IF_THIS_NULL()
	{
		debug_break( "in %s() : can't be load/save with a null object.", __FUNCTION__ );
		return false;
	}
	if( filename.is_empty() )
	{
		debug_break( "Object_ui %s can't load/save with no filename.", get_name_str() );
		return false;
	}
	if( _params.get_param_nb_allocated()==0 )	//	no params we don't save e.g. bdd_empty
	{
		if( !is_class_name( "cameras" ) )
			debug_break( "Object_ui %s don't have params.", get_name_str() );
	}
	return true;
}

AAA_ERR	c_obj_ui::save_to_this_file( o_str CONST & filename )
{
#if AAA_DEBUG()
	check_ext( filename );
#endif //AAA_DEBUG

	AAA_ERR retcode = ERR_ANY;

	INT32 CONST fname_index_begin = o_str::get_stack_index();
	
	param_init_pt();	//call
	// param_connex_set();

	if( b_verbose_save )
		GOOD_PRINT_STRING( "saving %s", filename.get() );

	//now	where to put it
	//we do this to name new object in case they are builder
//	if( is_not_my_filename() )	//todonow avoid changing name but bas
	set_my_filename( filename );	//todonow
	retcode = save_do_before( filename );
	if( NOERR(retcode) && _params.get_param_nb_allocated()>0 )
	{
		retcode = c_param::list_save_to_file( filename.get(), get_class_name(), get_file_version_save(), get_param_begin(), get_param_nb_used(), is_default_save_always() );
	}
	//we do it always in case of push/pop in before/after
	if( NOERR( retcode ) )
		retcode = save_do_after( filename );
	else
		save_do_after( filename );

	INT32 CONST fname_index_end = o_str::get_stack_index();
	if( fname_index_begin != fname_index_end )
		debug_break( "%s(), saving %s: begin with o_str depth stack %d but end with %d", __FUNCTION__, filename.get(), fname_index_begin, fname_index_end );

	return retcode;
}


AAA_ERR	c_obj_ui::save_to_file( o_str CONST & filename )
{
	if( !check_before_file_op( filename ) )
		return ERR_ANY;
	return save_to_this_file( filename );
}

AAA_ERR	c_obj_ui::save_to_file_replace_ext( o_str CONST & filename_in )
{
	if( !check_before_file_op( filename_in ) )
		return ERR_ANY;
	o_str& filename = o_str::push_name( filename_in );
		filename.replace_ext( get_fname_ext() );
		AAA_ERR retcode = save_to_this_file( filename );
	o_str::pop_name();
	return retcode;
}

AAA_ERR	c_obj_ui::save_to_file_add_ext( o_str CONST & filename_in, C_PCHAR_C ext )
{
	if( !check_before_file_op( filename_in ) )
		return ERR_ANY;
	o_str& filename = o_str::push_name( filename_in );
		filename.add_ext( ext );
		AAA_ERR retcode = save_to_this_file( filename );
	o_str::pop_name();
	return retcode;
}

AAA_ERR	c_obj_ui::save_to_file_add_ext( o_str CONST & filename_in )
{
	return save_to_file_add_ext( filename_in, get_fname_ext() );
}

//todo try/catch ?
AAA_ERR	c_obj_ui::save_tree_to_file( o_str CONST & filename )	//unused, bool b_full )
{
	AAA_ERR err;
	if( !lock_io.try_lock() )
	{
		ERR_PRINT_STRING( "%s() already processing, can't do it until the first one is done,", __FUNCTION__ );
		err = ERR_LOCKED;
	}
	else
	{
		err = save_to_file( filename );
		lock_io.unlock();
	}
	return err;
}

AAA_ERR	c_obj_ui::load_from_existing_file( o_str CONST & filename )
{
	AAA_ERR retcode = ERR_ANY;

	param_init_pt();	//call
//	param_connex_set();	//todonow this is a test

	if( b_verbose_load )
		GOOD_PRINT_STRING( "loading %s", filename.get() );

	INT32 CONST fname_index_begin = o_str::get_stack_index();

	retcode = load_do_before( filename );
	if( NOERR( retcode ) )
	{
		if( is_my_filename() )	// opt avoid to do it again for new object
			param_list_set_value_to_ina( get_param_begin(), get_param_nb_allocated() );

		//	object build by builders 'addfile) need the name
		set_my_filename( filename );
		retcode = c_param::list_load_from_file( filename, this );
		if( ERR( retcode ) )
		{
			box_err( "Error reading\n%s\n%s.", c_dir::get_def().get(), filename.get() );
			set_my_filename( "" );
		}
	}
	//we do it always in case of push/pop in before/after
	if( NOERR( retcode ) )
		retcode = load_do_after( filename );
	else
		load_do_after( filename );

	INT32 CONST fname_index_end = o_str::get_stack_index();
	if( fname_index_begin != fname_index_end )
		debug_break( "%s(), loading %s: begin with o_str depth stack %d but end with %d", __FUNCTION__, filename.get(), fname_index_begin, fname_index_end );

	return retcode;
}

AAA_ERR	c_obj_ui::load_from_file_common( o_str CONST & filename )
{
#if AAA_DEBUG()
	check_ext( filename );
#endif //AAA_DEBUG

	AAA_ERR retcode;
	if( c_file::is_exist( filename ) )
		retcode = load_from_existing_file( filename );
	else
	{
		param_init_pt();//call
		load_do_before( filename );
		set_my_filename( filename );
		load_do_after( filename );
		retcode = AAA_OK;
	}
	DBG_HEAP_CHECK();

	return retcode;
}

AAA_ERR	c_obj_ui::load_from_file( o_str CONST & filename_in )
{
	if( !check_before_file_op( filename_in )  )
		return ERR_ANY;
	o_str& filename = o_str::push_name( filename_in );
		AAA_ERR retcode = load_from_file_common( filename );
	o_str::pop_name();
	return retcode;
}

AAA_ERR	c_obj_ui::load_from_file_add_ext( o_str CONST & filename_in )
{
	if( !check_before_file_op( filename_in ) )
		return ERR_ANY;
	o_str& filename = o_str::push_name( filename_in );
		filename.add_ext( get_fname_ext() );
		AAA_ERR retcode = load_from_file_common( filename );
	o_str::pop_name();
	return retcode;
}

AAA_ERR	c_obj_ui::load_from_file_replace_ext( o_str CONST & filename_in )
{
	if( !check_before_file_op( filename_in ) )
		return ERR_ANY;
	o_str& filename = o_str::push_name( filename_in );
		filename.replace_ext( get_fname_ext() );
		AAA_ERR retcode = load_from_file_common( filename );
	o_str::pop_name();
	return retcode;
}

AAA_ERR	c_obj_ui::load_do_before(	o_str CONST &	filename )	{	return AAA_OK;	}
AAA_ERR	c_obj_ui::load_do_after(	o_str CONST &	filename )	{	return AAA_OK;	}
AAA_ERR	c_obj_ui::save_do_before(	o_str CONST &	filename )	{	return AAA_OK;	}
AAA_ERR	c_obj_ui::save_do_after(	o_str CONST &	filename )	{	return AAA_OK;	}

AAA_ERR	c_obj_ui::load_data_from_filename(	o_str CONST & filename, INT32 type_io )
{
	err_print_method_unimplemented( __FUNCTION__ );
	return ERR_TYPE_UNUSED;
}

void	c_obj_ui::obj_get_load_from_existing_file( c_obj_ui*& obj, c_factory_base * CONST pf, o_str CONST & filename )
{
	obj = obj_get( obj, pf );
	if( obj )
	{
		if( ERR(obj->load_from_existing_file(filename)) )
		{
			ERR_PRINT_STRING( "Error loading %s, deleting object", filename.get() );
			obj_delete( obj );
		}		
	}
}

FINLINE void	c_obj_ui::try_obj_load_with_this_filename( c_obj_ui*& obj, c_factory_base * CONST pf, o_str CONST & filename )
{
	if( c_file::is_exist( filename ) )
		obj_get_load_from_existing_file( obj, pf, filename );
}

void	c_obj_ui::try_obj_load_add_ext( c_obj_ui*& obj, c_factory_base * CONST pf, o_str CONST & filename_in )
{
	C_PCHAR_C ext = pf->get_file_ext().get();
	if( ext )
	{
		o_str& filename = o_str::push_name( filename_in );
			filename.add_ext( ext );
			try_obj_load_with_this_filename( obj, pf, filename );
		o_str::pop_name();
	}
	else
	{
		debug_break( "Houston Maa we got a problem" );
	}
}


o_str*	c_obj_ui::get_comment() CONST				{ return nullptr; }

bool	c_obj_ui::enum_command(	o_str& o )
{
	DBG_PRINT_STRING( "%s() unimplemented for class %s", __FUNCTION__, get_class_name() );
	return false;
}

bool	c_obj_ui::do_command( C_PCHAR_C cmd )
{
	DBG_PRINT_STRING( "%s() unimplemented for class %s", __FUNCTION__, get_class_name() );
	return false;
}

void	c_obj_ui::set_root_direct( c_obj_ui* obj )
{
	if( _root )
		_root->remove_branch( this );
	_root = obj;
}

void	c_obj_ui::set_root( c_obj_ui* root_in )
{
	if( _root != root_in )
	{
		if( _root )
		{
			//control is a hack (don't work 100%) but check signature
			c_obj_ui*	p = get_root();
			if( p && p->is_valid_obj_ui() )
				_root->remove_branch( this );
			else
				debug_break( "%s() we skip _root->remove_branch() because current root is not valid", __FUNCTION__ );
		}
		_root = root_in;
		if( root_in )
		{
			root_in->add_branch( this );	//todo check with deallocation
#if	AAA_DEBUG()
			C_PCHAR_C	str = root_in->get_name_str();
			if( str && strcmp( str, c_node_ui::str_name_def ) == 0 )
			{
				CHAR* sub = (CHAR*) get_name_str();
				if( !sub )
					sub = (CHAR*) get_class_name();
				ERR_PRINT_STRING( "root node for %s need a name", sub );
			}
#endif
		}
	}
}

INT32	c_obj_ui::get_branch_active_nb() CONST
{
	INT32	nb = get_branch_nb();
	if( nb )
	{
		INT32 count = 0;
		for( INT32 i=0; i<nb; ++i )
		{
			if( is_obj_exist_and_active(get_branch(i)) )
				++count;
		}
		return count;
	}
	return 0;
}

void	c_obj_ui::cell_draw_obj( REAL CONST size )
{
	c_bdd_cell_draw::cur->cell_draw_obj_default( size );
}

void	c_obj_ui::mem_print( CHAR* str )
{
	if( is_my_filename() )
		VERBOSE_PRINT_STRING( "%s() by %s named : %s", str, get_name_str(), _my_filename.get() );
	else
		VERBOSE_PRINT_STRING( "%s() by %s", str, get_name_str());
}

/*
void*	c_obj_ui::MALLOC( INT32 s, size_t alignment )
{
	if( mem::is_verbose() )
		mem_print( "MALLOC" );
	return mem::malloc( s, alignment );
}
void	c_obj_ui::FREE( void* memblock, size_t alignment )
{
	if( mem::is_verbose() )
		mem_print( "FREE" );
	mem::free( memblock );
}
void*	c_obj_ui::REALLOC( void* memblock, INT32 size, size_t alignment )
{
	if( mem::is_verbose() )
		mem_print( "REALLOC" );
	return mem::realloc( memblock, size, alignment );
}
*/

void	c_obj_ui::check_all()
{
}

//the dynamic object have a null root
void	c_obj_ui::assign_orphan()
{
#if	AAA_DEBUG()
#if AAA_USE_GLIST()

	DBG_PRINT_STRING( "checking orphan object started" );
	for( auto const & p_obj : g_list() )
	{
		if( !(p_obj->_root) && !p_obj->is_the_root() )
		{
			o_str s;
			p_obj->set_root( c_obj_ui::get_the_root() );
			c_namer::build_name( s, *p_obj );
			if( *p_obj->get_name_search_str() )
				DBG_PRINT_STRING( "NULL root for %s::%s with name_search %s", p_obj->get_class_name(), s.get(), p_obj->get_name_search_str() );
			else
				DBG_PRINT_STRING( "NULL root for %s::%s", p_obj->get_class_name(), s.get() );
		}
	}
	DBG_PRINT_STRING( "checking orphan done" );

#endif
#endif
}

void	c_obj_ui::test_create_delete()
{
	PRINT_STRING( "#\tAAASeed will now create then destroy instance of unused  class\n" );
	//	create and delete one instance of each class 
	std::list<c_factory_base*>	l;
	c_factory_base::build_derived_concrete( "obj_ui", l );

	for( auto const & p_factory : l )
	{
		if( p_factory->get_obj_count()==0 )
			delete p_factory->create_obj();
	}

	PRINT_STRING( "#\tCHECK DONE\n" );
}

#if AAA_STATE_COMPILE()
void		c_obj_ui::state_do_action()

	_params.state_do_action();
	for( INT32 i = get_branch_nb() - 1; i >= 0; --i )
		_branch->get_item(i)->state_do_action();
}
#endif //AAA_STATE_COMPILE

void	c_obj_ui::cpy_params_from( c_obj_ui* src, INT32 index_begin, INT32 nb )
{
	if( is_same_class( src ) )
	{
		if( _params.get_param_nb_allocated() )
			_params.cpy_from( &(src->_params), index_begin, nb );
		else
			debug_break( "Can't %s() from object with no param", __FUNCTION__ );
	}
	else
		debug_break( "Can't %s() with object from different class", __FUNCTION__ );
}

void	c_obj_ui::cpy_params_from_skip( c_obj_ui* src, INT32 nb_skip_at_begin, INT32 nb_skip_at_end )
{
	if( is_same_class(src) )
	{
		if( _params.get_param_nb_allocated() )
			_params.cpy_from_skip( &(src->_params), nb_skip_at_begin, nb_skip_at_end );
		else
			debug_break( "Can't %s() from object with no param", __FUNCTION__ );
	}
	else
		debug_break( "Can't %s() with object from different class", __FUNCTION__ );
}

//FACTORY_ABSTRACT_CREATE_V1( c_obj_active_ui, obj_active_ui, Object Active );
//CONSTRUCTOR_ABSTRACT_CREATE(c_obj_active_ui) {}
//EMPTY_DESTRUCTOR(c_obj_active_ui)

//was	void	c_obj_active_ui::set_active( bool flag )
void	c_obj_ui::set_active( bool CONST flag )
{
	IF_THIS_NULL()
	{
		debug_break( "c_obj_ui::set_active() on Null obj_ui" );
		return;
	}
	if( _b_obj_active_ui != flag )
	{
		_b_obj_active_ui = flag;
		if( !draw::is_rendering() )
			SWITCH_PRINT_STATE( get_name_str(), _b_obj_active_ui );
	}
}

//void	c_obj_ui::param_next_set_list( INT32 h, INT32 nb )
//{
//	p_param p = _params.get(h);
//	p->set_list( _params.get(h+1), nb );
//	p->get_data_unsafe_maa()->or_type_maa( M_MAA_FLAG );
//}

//hack potential trouble when target deleted but emergency FAF prod
//bool		c_obj_ui::set_target( INT32 slot, c_obj_ui* target )		//slot start at 1
//{
//	err_print_method_unimplemented( __FUNCTION__ );
//	return false;
//}
//
//c_obj_ui*	c_obj_ui::get_target( INT32 slot ) CONST					//slot start at 1
//{
//	err_print_method_unimplemented( __FUNCTION__ );
//	return nullptr;
//}


//NET
//todonow	deal with packet limit
void	c_obj_ui::net_send_param( INT32 CONST net_channel, INT32 CONST nb )
{
	if( net )
	{
		//todoqq
		//hack
		CHAR	buf[c_net::BLK_SIZE_MAX];
		param_init_pt();	//call
		CHAR* end = param_write_to_mem( buf, nb );
		net->sendto_link0( net_channel, c_net::BLK_OBJ_DATA, (UINT8*)buf, INT32(end - buf) );
	}
}

void	c_obj_ui::net_receive_param( CHAR CONST * pt, INT32 CONST nb )
{
	if( pt )
	{
		param_init_pt();//call
		param_read_from_mem( pt, nb );
	}
	else
		ERR_PRINT_STRING( "net_receive NULL pointer" );
}

void	c_obj_ui::net_receive_param( INT32 CONST net_channel, INT32 CONST nb )
{
	while( c_net_blk* blk_in = net->blk_take_by_type_channel( c_net::BLK_OBJ_DATA, net_channel ) )
	{
		net_receive_param( (CHAR CONST *)blk_in->get_data_pt(), nb );
		net->blk_free( blk_in );
	}
}

//todonow	these twisted function is the result of a global problem
//	the whole name strategy should be revisited
bool	c_obj_ui::make_script_filename( o_str& dst_filename, o_str CONST & src_name, C_PCHAR_C ext, o_str* CONST name_relative )
{
	//	we want an absolute name so if the cur dir change we are not in troubles
//#if DEBUG
//	if( src_name.is_starting_with_nocase( "gar", 3 ) )
//		DBG_PRINT_STRING( "name could be Garden" );
//#endif
	if( src_name.is_fname_absolute() )
		dst_filename.set( src_name );
	else
	{
		o_str & src = (o_str&)get_name_search();
		if( src.is_empty() )
		{
			c_obj_ui* dad = get_root();
			if( dad )
				src = dad->get_name_search();
			if( src.is_empty() )
			{
				ERR_PRINT_STRING( "%s() we can't make the script name because the object and his root have no name", __FUNCTION__ );
				dst_filename.erase();
				return false;
			}
		}

		o_str& dst = o_str::push_name();
			if( src_name.is_empty() )
			{	//name given by the source
				dst.set( src );
				dst.replace_ext( ext );
			}
			else
			{	//name given by the script_name
				o_str& dir = o_str::push_name();
					{
						C_PCHAR pt = src.get();
						if( *pt=='.' && *(pt+1)=='/' )
							dir.set_dir_name( pt+2 );
						else
							dir.set_dir_name( pt );
					}
					if( dir.is_empty() )
						dst.set( src_name );
					else
					{
						dst.set( dir );
						dst.add_slash();
						dst.add( src_name );
					}
				o_str::pop_name();
			}

			if( name_relative )
				name_relative->set( dst );

			if( dst.is_fname_absolute() )
				dst_filename.set( dst );
			else
			{
				dst_filename.set( c_dir::get_kernel() );	//todo check if we need the GaBuZoMei App dir at some point ? (2024 June Maa)
				dst_filename.add_slash();
				dst_filename.add( dst );
			}
		o_str::pop_name();
	}
	return true;
}	

void	c_obj_ui::osc_process_message( osc::ReceivedMessage CONST & msg )
{
	if( c_net::b_verbose_osc_processed_ui )
		c_net::osc_print_message( "OSC Process :", msg );

	err_print( "%s() this object don't deal with OSC.", __FUNCTION__ );
}

void	c_obj_ui::osc_process( osc::ReceivedPacket CONST & packet )
{
	if( packet.is_bundle() )
		osc_process_bundle(		osc::ReceivedBundle(packet) );
	else
		osc_process_message(	osc::ReceivedMessage(packet) );
}

void	c_obj_ui::osc_process_bundle( osc::ReceivedBundle CONST & b )
{
	// ignore bundle time tag for now
	for( osc::ReceivedBundle::const_iterator it = b.ElementsBegin(); it != b.ElementsEnd(); ++it )
	{
		if( it->is_bundle() )
			osc_process_bundle(		osc::ReceivedBundle(*it) );
		else
			osc_process_message(	osc::ReceivedMessage(*it) );
	}
}

void	c_obj_ui::osc_process( UINT8 CONST * pt, INT32 size )
{
	osc::ReceivedPacket packet( (char*)pt, size );
	osc_process( packet );
}

INT32	c_obj_ui::osc_take_and_process( C_PCHAR start, bool b_verbose )
{
	INT32 count = 0;
	while( c_net_blk* blk = net->osc_blk_take_by_start(start) )
	{
		osc_process( blk->get_data_pt(), blk->get_len() );
		net->blk_free( blk );
		++count;
	}
	return count;
}

