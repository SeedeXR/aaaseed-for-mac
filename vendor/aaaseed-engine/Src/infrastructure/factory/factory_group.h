
#ifdef AAA_FACTORY_GROUP_H
#error "FACTORY_GROUP_H included more than once."
#endif
#define AAA_FACTORY_GROUP_H 1

#ifndef AAA_OBJ_H
#	include "infrastructure/obj/obj.h"
#endif
#ifndef _LIST_
#	include <list>
#endif

class c_factory_base;

class c_factory_group : public c_obj
{
protected:
	std::list<c_factory_base*>	_factories;
public:
	virtual	INT32				get_item_nb()						= 0;
	virtual	INT32				get_index_from_str( C_PCHAR_C str )	= 0;
	virtual C_PCHAR_C			get_str_ui( INT32 index )			= 0;
	virtual	C_PCHAR_C			get_str_storage( INT32 index )		= 0;
	virtual c_factory_base *	get_factory( INT32 index )			= 0;
	virtual	void				init()								= 0;

	virtual	AAA_ERR				build_menu( INT32 menu_id, PT_MENU_FN menu_fn )	= 0;

	std::list<c_factory_base*> CONST &	 get_factories()	{	return _factories;	}
};
