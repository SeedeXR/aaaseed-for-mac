
#ifdef AAA_FACTORY_GROUP_BDD_H
#error "FACTORY_GROUP_BDD_H included more than once."
#endif
#define AAA_FACTORY_GROUP_BDD_H 1


#ifndef AAA_FACTORY_GROUP_H
#	include "infrastructure/factory/factory_group.h"
#endif


class c_factory_group_bdd final : public c_factory_group
{
private:
	c_factory_base*	find_factory_from_selector( C_PCHAR_C str );

public:
	c_factory_group_bdd();

	c_factory_base*				get_from_file_extension(	C_PCHAR_C str );

	virtual	void				init();
	virtual INT32				get_index_from_str(			C_PCHAR_C str );
	virtual INT32				get_item_nb();
			bool				is_valid_index(		INT32 CONST index );																		  
																			  
	virtual C_PCHAR_C			get_str_ui(			INT32 CONST index );
	virtual C_PCHAR_C			get_str_storage(	INT32 CONST index );
	virtual	c_factory_base *	get_factory(		INT32 CONST index );

	virtual	AAA_ERR				build_menu(			INT32 CONST menu_id, PT_MENU_FN CONST menu_fn );
};