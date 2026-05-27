#include "factory_group_bdd.h"
#include "obj_ui/bdd/bdd_geo/bdd_gene.h"
#include "ui/aaa_menu.h"

#ifndef _MAP_
#	include <map>
#endif

namespace {
	//this class is a singleton so we keep data here for simplicity of the .h
	struct st_class_info
	{
		C_PCHAR			_str_ui;
		C_PCHAR			_str_storage;
		c_factory_base*	_factory;
		INT32			_index;
	};
	st_class_info class_info_external =  { "EXTERNAL", "EXTERNAL", nullptr, 0 };

	static	c_registry<c_factory_base*>		factories_by_file_ext;
	std::map< INT32, st_class_info >		_map; 
	std::map< c_factory_base*, INT32 >		_map_factory;
}

c_factory_base* c_factory_group_bdd::get_from_file_extension( C_PCHAR_C str )
{
	return factories_by_file_ext.get( str );
}

INT32 c_factory_group_bdd::get_item_nb()
{
	return (INT32)_map.size();
}
bool c_factory_group_bdd::is_valid_index( INT32 CONST index )
{
	return 0<=index && ( ((UINT32)index)<_map.size() );
}

C_PCHAR_C c_factory_group_bdd::get_str_ui( INT32 CONST index )
{
	return C_PCHAR_C( is_valid_index(index) ? _map[ index ]._str_ui : nullptr );
}
C_PCHAR_C c_factory_group_bdd::get_str_storage(	INT32 CONST index )
{
	return C_PCHAR_C( is_valid_index(index) ? _map[ index ]._str_storage : nullptr );
}
c_factory_base * c_factory_group_bdd::get_factory( INT32 CONST index )
{
	return is_valid_index(index) ? _map[index]._factory : nullptr;
}


c_factory_base*	c_factory_group_bdd::find_factory_from_selector( C_PCHAR_C str )
{
	for( auto const & p_factory_base : _factories )
	{
		if( p_factory_base->is_class_match( str ) )
		{
			//	DBG_PRINT_STRING( "%s -> %s", str, (*it)->get_name_human() );
			return p_factory_base;
		}
	}
	DBG_PRINT_STRING( "Can't match %s", str );
	return nullptr;
}

c_factory_group_bdd::c_factory_group_bdd()
{}

void c_factory_group_bdd::init()
{
	st_class_info st;

	_map.clear();
	_map[0] = class_info_external;
	_map_factory.clear();
	factories_by_file_ext.clear();

	_factories.clear();
	c_factory_base::build_derived_concrete( "bdd", _factories );

	for( auto const & pf : _factories )
	{
		C_PCHAR_C ext = pf->get_file_ext().get();
		factories_by_file_ext.sign_in( ext, pf );
	}
	
	//	remove bdd_gene a special case
	c_factory_base* p_empty = &c_bdd_gene::the_factory();
	if( !p_empty )
		debug_break( "compiling without bdd_gene is unimplemented and dangerous" );
	_factories.remove( p_empty );	// we add it back below
	//todo	sort by name

		//	just for a check
		//		we should get no message
		//for( INT32 i=0; i<BDD_NB; ++i )
		//{
		//	find_factory_from_selector( str_bdd[ i ] );
		//}

		INT32	index = 1;
		while( C_PCHAR_C pt = c_bdd_gene::get_sub_name(index-1) )
		{
			st._str_ui = pt;
			st._str_storage = pt;
			st._factory = &c_bdd_gene::the_factory();
			st._index = index;
			_map[index] = st;
			++index;
		}

		for( auto const & pf : _factories )
		{
			st._str_ui = pf->get_name_human().get();	//todo do better with CONST  
			st._str_storage = (CHAR*)pf->get_class_name();		//todo do better with CONST  
			st._factory = pf;
			st._index = index;
			_map_factory[pf] = index;
			_map[index] = st;
			++index;
		}

	if( p_empty )
		_factories.push_back( p_empty );	//	we need all the factories in this list
	//st._factory = find_factory_from_selector( st._str_ui );

}

INT32 c_factory_group_bdd::get_index_from_str( C_PCHAR str )
{
	if( !str )
		debug_break();

	for( auto const & elt : _map )
	{
		const auto pst = &(elt.second);
		if ( str_is_equal_bothnocase( pst->_str_storage, str ) )
			return pst->_index;
	}
	c_factory_base* pf = find_factory_from_selector( str );
	if( pf )
		return _map[_map_factory[pf]]._index;
	return -1;
}


class c_class_info_less
{ 
public: 
	bool operator() ( st_class_info* a, st_class_info* b ) 
	{
		INT32	i;
		//	don't sort bdd_gene
		if( a->_index < 12 || b->_index < 12 )	//todonow make sure if work even if bdd_gene change
			i = (a->_index < b->_index) ? -1 : 1;
		else
			i = strcmp( a->_factory->get_property( "sub_menu" ), b->_factory->get_property( "sub_menu" ) );
		if ( i==0 )
			return strcmp( a->_str_ui, b->_str_ui ) < 0 ;
		else
			return i < 0;
	}
};

AAA_ERR	c_factory_group_bdd::build_menu( INT32 CONST menu_id, PT_MENU_FN CONST menu_fn )
{
	c_registry<INT32>	subs;
	INT32				nb = get_item_nb();
	c_factory_base*		pf;
	//std::list<st_class_info*>	order;
	std::vector<st_class_info*>	order;

	order.resize( nb );
	for( INT32 i = 0; i < nb; ++i )
		order[i] = &_map[i];
	//	order.push_back( &_map[i] );
	//order.sort( c_class_info_less()	);
	std::sort( order.begin(), order.end(), c_class_info_less() );

	auto it = order.begin();
	for( INT32 i=0; i<nb; ++i )
	{
		INT32 index = (*it)->_index;
		++it;
		pf = get_factory( index );
		C_PCHAR pt = get_str_ui( index );
		if( C_PCHAR sub_name = pf ? pf->get_property( "sub_menu" ) : nullptr )
		{
			INT32 sub = subs.get( sub_name );
			if( sub )
			{
				menu::set_cur( sub, __FUNCTION__ );
				menu::add_item( pt, index );
				menu::set_cur( menu_id, __FUNCTION__ );
			}
			else
			{
				sub = menu::create( menu_fn );
				menu::add_item( pt, index );
				menu::set_cur( menu_id, __FUNCTION__ );
				menu::add_menu_sub( sub_name, sub );
				subs.sign_in( sub_name, sub );
			}
		}
		else
			menu::add_item( pt, index );
	}
	return AAA_OK;
}
