#include "factory_menu.h"
#include "factory.h"
#include "ui/aaa_menu.h"

//	to_remove is just for the start of the name to be removed 
void	c_factory_menu::build( INT32 menu_id, PT_MENU_FN menu_fn, C_PCHAR_C pat, C_PCHAR_C to_exclude )
{
	if( !_factories.empty() )
		_factories.clear();

	INT32 len = to_exclude ? (INT32)strlen( to_exclude ) : 0;

	C_PCHAR				cid;
	c_registry<INT32>	subs;
	INT32				count = 0;
	while( cid = c_factory_base::get_cid(count) )	//	this is not a good way to parse : ok but inefficient
	{
		if( str_start_is_equal_bothnocase( cid, pat ) )
		{	//	beginning of name match
			c_factory_base*	CONST pf = c_factory_base::get_by_cid( cid );
			if( pf->is_concrete() )
			{
				CHAR* pt = (CHAR*) pf->get_name_human().get();
				if( to_exclude && str_start_is_equal_bothnocase( pt, to_exclude ) )
					pt += len;

				if( C_PCHAR sub_name = pf->get_property( "sub_menu" ) )
				{
					INT32 sub = subs.get( sub_name );
					if( sub )
					{
						menu::set_cur( sub, __FUNCTION__ );
						menu::add_item( pt, (INT32)_factories.size() );
						menu::set_cur( menu_id, __FUNCTION__ );
					}
					else
					{
						sub = menu::create( menu_fn );
						menu::add_item( pt, (INT32)_factories.size() );
						menu::set_cur( menu_id, __FUNCTION__ );
						menu::add_menu_sub( sub_name, sub );
						subs.sign_in( sub_name, sub );
					}
				}
				else
					menu::add_item( pt, (INT32)_factories.size() );
				_factories.push_back( pf );		/*! \todo keep the buffer */
			}
		}
		++count;
	}
}

c_obj_ui*	c_factory_menu::create_from_index( INT32 index )
{
	return _factories[index]->create_obj();
}
