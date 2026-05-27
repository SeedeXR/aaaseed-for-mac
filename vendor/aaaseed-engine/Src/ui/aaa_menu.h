
#ifdef AAA_AAA_MENU_H
#error "AAA_MENU_H included more than once."
#endif
#define AAA_AAA_MENU_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
#ifndef AAA_SYSTEMMOUSE_H
#	include "system/shared/SystemMouse.h"
#endif


namespace menu
{
//	extern	bool	b_on;	//hack
	extern 	FINLINE	bool	is_on()			{ return true; }

	extern	bool	b_allow_ui;	// 2025 March try to stop menu
	extern 	FINLINE	bool*	get_menu_allow_pt()			{ return &b_allow_ui; }

	extern	AAA_ERR			attach( mouse::BUTTON CONST button, INT32 CONST menu_id );
	extern	AAA_ERR			detach( mouse::BUTTON CONST button );
	extern	void			detach_all();
	
	extern	INT32			create(				PT_MENU_FN menu_fn );
	extern	INT32			get_item_nb();
	extern	AAA_ERR			set_cur(			INT32 CONST menu_id,		C_PCHAR_C signature );
	extern	void			destroy(			INT32 CONST menu_id );
	
	extern	AAA_ERR			insert_item(		INT32 CONST item_pos,	C_PCHAR_C label, INT32 CONST value, INT32 CONST check=0 );
	extern	AAA_ERR			change_item(		INT32 CONST item_pos,	C_PCHAR_C label, INT32 CONST value, INT32 CONST check=0 );
	extern	AAA_ERR			add_item(									C_PCHAR_C label, INT32 CONST value, INT32 CONST check=0 );
	extern	AAA_ERR			remove_item(		INT32 CONST item_pos	);

	extern	AAA_ERR			insert_separator(	INT32 CONST index		);
	extern	AAA_ERR			add_separator();

	extern	AAA_ERR			insert_menu_sub(	INT32 CONST item_pos,	C_PCHAR_C label, INT32 CONST menu_sub_id );
	extern	AAA_ERR			change_menu_sub(	INT32 CONST item_pos,	C_PCHAR_C label, INT32 CONST menu_sub_id );
	extern	AAA_ERR			add_menu_sub(								C_PCHAR_C label, INT32 CONST menu_sub_id );
	
	extern	void			do_command(			INT32 CONST menu_id,	INT32 CONST id );
}

