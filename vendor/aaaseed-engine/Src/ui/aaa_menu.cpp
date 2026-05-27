
#include "aaa_def.h"
#include "ui/aaa_menu.h"

#if AAA_NEW_DESIGN()
#	include "System/win32/SystemContextMenu.h"
#else
#	include "draw/aaa_glut.h"
#endif

#include "err.h"

//bool menu::b_on = false;
bool menu::b_allow_ui = true;

namespace {
	bool b_verbose = false;
	INT32 menu_attached[ mouse::BUTTON_END - mouse::BUTTON_BEGIN + 1 ] = { -1, -1, -1, -1, -1, -1, -1, -1, -1 };
}

#if AAA_NEW_DESIGN()
#else
static INT32 convert_mouse_button( menu::MOUSE_BUTTON button )
{
	INT32 ret;
	switch(button)
	{
	case menu::MOUSE_BUTTON_LEFT:	ret = GLUT_LEFT_BUTTON;   break;
	case menu::MOUSE_BUTTON_MIDDLE:	ret = GLUT_MIDDLE_BUTTON; break;
	default:
	case menu::MOUSE_BUTTON_RIGHT:	ret = GLUT_RIGHT_BUTTON;  break;
	}
	return ret;
}
#endif

AAA_ERR	menu::set_cur( INT32 CONST menu_id, C_PCHAR_C signature )
{
#if AAA_DEBUG()
	if( b_verbose )
		DBG_PRINT_STRING( "%s() change menu_id cur %d", signature, menu_id );
#endif
#if AAA_NEW_DESIGN()
	// we deal with only one menu in this case ?
	auto menu_factory = c_system_context_menu_factory::get_instance();
	menu_factory->set_menu_active( menu_id );
#else
	glutSetMenu( menu_id );
#endif
	return AAA_OK;
}

namespace {
	INT32 init_count = 0;
}

AAA_ERR	menu::attach( mouse::BUTTON CONST button, INT32 CONST menu_id )
{
	if( b_allow_ui )
	{
		INT32 button_index = button - mouse::BUTTON_BEGIN;
		if( menu_attached[ button_index ] != menu_id )	
		{
#if AAA_DEBUG()
			if( b_verbose )
				DBG_PRINT_STRING( "attach menu_id %d for %s", menu_id, get_button_name(button) );
#endif
#if AAA_NEW_DESIGN()
			// we deal with only one menu in this case ?
			auto menu_factory = c_system_context_menu_factory::get_instance();
			menu_factory->set_menu_active( menu_id );
			menu_factory->attach_menu( button );
#else
			glutSetMenu( menu_id );	
			glutAttachMenu( convert_mouse_button(button) );
#endif
			// 2023 Jan we have a problem at start probably we attach to early
			//  so we force attachment for a while under 4 no menu
			if( init_count < 4 )
				++init_count;
			else
				menu_attached[ button_index ] = menu_id;
		}
	}
	else
		detach( button );
	return AAA_OK;
}

AAA_ERR	menu::detach( mouse::BUTTON CONST button )
{
	INT32 button_index = button - mouse::BUTTON_BEGIN;
	INT32 CONST menu_id = menu_attached[ button_index ];
	if( menu_id != -1 )	
	{
#if AAA_DEBUG()
		if( b_verbose )
			DBG_PRINT_STRING( "detach menu_id %d for %s", menu_id, get_button_name(button) );
#endif
#if AAA_NEW_DESIGN()
		// we deal with only one menu in this case ?
		auto menu_factory = c_system_context_menu_factory::get_instance();
		menu_factory->set_menu_active( menu_id );
		menu_factory->attach_menu( mouse::BUTTON_NONE );
#else
		glutSetMenu( menu_id );	
		glutAttachMenu( convert_mouse_button(button) );
#endif
		menu_attached[ button_index ] = -1;
	}
	return AAA_OK;
}

// called most of the here to avoid losing menu, making sure it is reattached
void	menu::detach_all()
{
	for( INT32 but = mouse::BUTTON::BUTTON_BEGIN; but <= mouse::BUTTON::BUTTON_END; ++but )
		detach( mouse::BUTTON(but) );
}

INT32 menu::create( PT_MENU_FN menu_fn )
{
#if AAA_NEW_DESIGN()
	return c_system_context_menu_factory::get_instance()->create_menu( menu_fn );
#else
	return glutCreateMenu( menu_fn );
#endif
}

INT32 menu::get_item_nb()
{
#if AAA_NEW_DESIGN()
	return c_system_context_menu_factory::get_instance()->get_item_nb();
#else
	return maaglutGetItemNb();
#endif
}


void	menu::destroy( INT32 CONST menu_id )
{
#if AAA_NEW_DESIGN()	
	c_system_context_menu_factory::get_instance()->release_menu( menu_id );
#else
	glutDestroyMenu( menu_id );
#endif
}


AAA_ERR	menu::insert_item( INT32 CONST item_pos, C_PCHAR_C label, INT32 CONST value, INT32 CONST check )
{
#if AAA_NEW_DESIGN()
	c_system_context_menu_factory::get_instance()->insert_item( item_pos-1, label, value, check );
#else		
	glutAddMenuEntryCheck( label, value, check );
#endif
	return AAA_OK;
}

AAA_ERR	menu::change_item( INT32 CONST item_pos, C_PCHAR label, INT32 CONST value, INT32 CONST check )
{
	if(label)
	{
#if AAA_NEW_DESIGN()
		c_system_context_menu_factory::get_instance()->change_item( item_pos-1, label, value, check );
#else
		glutChangeToMenuEntryCheck( item_pos, label, value, check );
#endif
	}
	else
		DBG_PRINT_STRING( "%s() called with NULL str", __FUNCTION__ );

	return AAA_OK;
}

AAA_ERR	menu::add_item( C_PCHAR_C label, INT32 CONST value, INT32 CONST check )
{
#if AAA_NEW_DESIGN()
	c_system_context_menu_factory::get_instance()->add_item( label, value, check );
#else		
	glutAddMenuEntryCheck( label, value, check );
#endif
	return AAA_OK;
}

AAA_ERR	menu::remove_item( INT32 CONST item_pos )
{
#if AAA_NEW_DESIGN()
	c_system_context_menu_factory::get_instance()->remove_item( item_pos-1 );
#else
	glutRemoveMenuItem( item_pos );
#endif
	return AAA_OK;
}


AAA_ERR	menu::insert_separator( INT32 CONST item_pos )
{
#if AAA_NEW_DESIGN()
	c_system_context_menu_factory::get_instance()->insert_separator( item_pos-1 );
#else
	add_item( "", 0 );
#endif
	return AAA_OK;
}

AAA_ERR	menu::add_separator()
{
#if AAA_NEW_DESIGN()
	c_system_context_menu_factory::get_instance()->add_separator();
#else
	add_item( "", 0 );
#endif
	return AAA_OK;
}



AAA_ERR	menu::insert_menu_sub( INT32 CONST item_pos, C_PCHAR_C label, INT32 CONST menu_sub_id )
{
#if AAA_NEW_DESIGN()
	c_system_context_menu_factory::get_instance()->insert_sub_menu( item_pos-1, label, menu_sub_id );
#else
	glutAddSubMenu( label, menu_sub_id );
#endif
	return AAA_OK;
}

AAA_ERR	menu::add_menu_sub( C_PCHAR_C label, INT32 CONST menu_sub_id )
{
#if AAA_NEW_DESIGN()
	c_system_context_menu_factory::get_instance()->add_sub_menu( label, menu_sub_id );
#else
	glutAddSubMenu( label, menu_sub_id );
#endif
	return AAA_OK;
}

AAA_ERR	menu::change_menu_sub( INT32 CONST item_pos, C_PCHAR_C label, INT32 CONST menu_sub_id )
{
#if AAA_NEW_DESIGN()
	c_system_context_menu_factory::get_instance()->change_sub_menu( item_pos-1, label, menu_sub_id );
#else
	glutChangeToSubMenu( item_pos, label, menu_sub_id );
#endif
	return AAA_OK;
}

void	menu::do_command( INT32 CONST menu_id, INT32 CONST id )
{
#if AAA_NEW_DESIGN()
	c_system_context_menu_factory::get_instance()->do_command( menu_id, id );
#endif
}
