

#ifdef AAA_FACTORY_MENU_H
#error "FACTORY_MENU_H included more than once."
#endif
#define AAA_FACTORY_MENU_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
#ifndef _VECTOR_
#	include <vector>
#endif


class	c_factory_base;
class	c_obj_ui;

class c_factory_menu
{
private:
	std::vector<c_factory_base*>	_factories;
public:
	void		build( INT32 menu_id, PT_MENU_FN menu_fn, C_PCHAR_C pat, C_PCHAR_C to_exclude );
	c_obj_ui*	create_from_index( INT32 index );
};


