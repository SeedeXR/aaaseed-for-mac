#include "SystemContextMenu.h"

#include "err.h"
#include "SystemError.h"
#include "system/shared/SystemUtils.h"
#include "system/shared/SystemCursor.h"
#include "system/win32/SystemEventReader.h"
#include "infrastructure/aaa_window.h"


CONSTEXPR UINT32 ITEM_POS_MAX = 1024;


///////////////////////////////////////////////////////////////////////////////////////////////////
//		c_system_context_menu class
///////////////////////////////////////////////////////////////////////////////////////////////////
c_system_context_menu * c_system_context_menu::create_ptr( INT32 CONST id, PT_MENU_FN callback )
{
	if( !callback )
	{
		debug_break( "%s() null callback pointer", __FUNCTION__ );
		return nullptr;
	}
	else
	{
		c_system_context_menu * ptr = new c_system_context_menu();
		ptr->init( id, callback );
		return ptr;
	}
}

void c_system_context_menu::release_ptr( c_system_context_menu* pt )
{
	if( !pt )
		debug_break( "%s() null pointer", __FUNCTION__ );
	else
	{
		pt->release();
		delete pt;
	}
}

c_system_context_menu::c_system_context_menu( void )
	// Members init
	: _id						( 0 )
	, _callback					( nullptr )
	, _attached_mouse_button	( mouse::BUTTON_NONE )
	, _hd_menu					( nullptr )
{}

c_system_context_menu::~c_system_context_menu( void )
{}

void c_system_context_menu::init( INT32 CONST id, PT_MENU_FN callback )
{
	if( !callback )
		debug_break( "%s() null callback pointer", __FUNCTION__ );
	else
	{
		_id			= id;
		_callback	= callback;
		_hd_menu	= ::CreatePopupMenu();
		if( !_hd_menu )
			debug_break( "%s() could not create menu", __FUNCTION__ );
		else
		{
			MENUINFO menu;
			menu.cbSize				= sizeof(MENUINFO);
			menu.fMask				= MIM_STYLE;
			if( GetMenuInfo( _hd_menu, &menu ) )
			{
				menu.dwStyle |= MNS_NOTIFYBYPOS;
				menu.fMask	= MIM_STYLE;
				if( SetMenuInfo( _hd_menu, &menu ) == 0 )
				{
					WIN_ERR_PRINT( "SetMenuInfo failed" );
				}
			}
		}
/*
	menu.dwStyle			= MFT_SEPARATOR;
	menu.cyMax				= MFS_DISABLED | MFS_UNHILITE;
	menu.hbrBack			= nullptr;
	menu.dwContextHelpID	= nullptr;
	menu.dwMenuData			= nullptr;
*/
	}
}

void c_system_context_menu::release( void )
{
	/////////////////////////////////////////////
	//
	// Before closing, an application must use the DestroyMenu function to destroy a menu not assigned to a window. 
	// A menu that is assigned to a window is automatically destroyed when the application closes.
	// DestroyMenu is recursive, that is, it will destroy the menu and all its sub menus.
	//
	/////////////////////////////////////////////

	if( _hd_menu!=nullptr )
	{
		if( ::DestroyMenu(_hd_menu) == 0 )
			debug_break( "%s() Can not destroy Win32 menu : error was %s", __FUNCTION__, aaa::system::get_err_message().c_str() );
		_hd_menu = nullptr;
	}

	_id						= 0;
	_callback				= nullptr;
	_attached_mouse_button	= mouse::BUTTON_NONE;
}



///////////////////////////////////////////////////////////////////////////////////////////////////
//		UTILITIES
///////////////////////////////////////////////////////////////////////////////////////////////////
INT32 c_system_context_menu::get_item_nb( void )
{
	return ::GetMenuItemCount(_hd_menu);
}

bool c_system_context_menu::remove_item( INT32 CONST item_pos )
{
	if( item_pos >= ITEM_POS_MAX )
	{
		ERR_PRINT_STRING( "%s() AAA limit item_pos to %d, and %d was asked", __FUNCTION__, ITEM_POS_MAX, item_pos );
		return false;
	}
	if( ::RemoveMenu( _hd_menu, item_pos, MF_BYPOSITION ) == 0 )
	{
		ERR_PRINT_STRING( "%s() Can not remove menu item at pos %d : error was %s", __FUNCTION__, item_pos, aaa::system::get_err_message().c_str() );
		return false;
	}
	return true;
}

#define USE_WIDE()	0 
#if USE_WIDE()
#	define AAA_MENU_ITEM_INFO MENUITEMINFOW
#else
#	define AAA_MENU_ITEM_INFO MENUITEMINFOA
#endif

bool c_system_context_menu::insert_item( INT32 CONST item_pos, C_PCHAR_C label, INT32 CONST value, INT32 CONST check )
{
	if( item_pos >= ITEM_POS_MAX )
	{
		ERR_PRINT_STRING( "%s() AAA limit item_pos to %d, and %d was asked", __FUNCTION__, ITEM_POS_MAX, item_pos );
		return false;
	}
	{
		bool b_ret = false;

		if( item_pos < ITEM_POS_MAX )	//todo it cause problem with bing but this anyhow too much
		{
			AAA_MENU_ITEM_INFO item;
			item.cbSize			= sizeof(MENUITEMINFOW);
			item.fMask			= MIIM_STATE | MIIM_STRING | MIIM_ID | MIIM_DATA ;
			item.fType			= MFT_STRING;	// 0
			item.fState			= MFS_ENABLED | MFS_UNHILITE;
			if( check != 0 )
				item.fState	|= MFS_CHECKED;
			item.wID			= value;	//	value to be passed to the fn asociated with the menu
			item.hSubMenu		= nullptr;
			item.hbmpChecked	= nullptr;
			item.hbmpUnchecked	= nullptr;
			item.dwItemData		= _id;		//	to find quicky the menu (MIIM_DATA flag to store it)

			item.hbmpItem		= nullptr;
			//item.cch			= (UINT) strlen( label );
#if USE_WIDE()
			item.dwTypeData		= sysutils::utf8_to_unicode( label );	//leak dwTypeData need to be erased : is it a problem ?
			BOOL ret = ::InsertMenuItemW( _hd_menu, item_pos, TRUE, &item );
#else
			item.dwTypeData		= (LPSTR)label;	//todo dangerous ?
			BOOL ret = ::InsertMenuItemA( _hd_menu, item_pos, TRUE, &item );
#endif	
			if( ret == 0 )
				debug_break( "%s() Can not insert menu item at pos %d : error was %s", __FUNCTION__, item_pos, aaa::system::get_err_message().c_str() );
			else
				b_ret = true;
#if USE_WIDE()
			sysutils::free_str_tmp( item.dwTypeData );
#endif
		}
		else
			ERR_PRINT_STRING( "%s() AAA limit item_pos to %d, and %d was asked", __FUNCTION__, ITEM_POS_MAX, item_pos );
		return b_ret;
	}
}

bool c_system_context_menu::add_item( C_PCHAR_C label, INT32 CONST value, INT32 CONST check )
{
	return insert_item( get_item_nb(), label, value, check );
}

bool c_system_context_menu::change_item( INT32 CONST item_pos, C_PCHAR_C label, INT32 CONST value, INT32 CONST check )
{
	if( remove_item( item_pos ) )
		return insert_item( item_pos, label, value, check );

	ERR_PRINT_STRING( "%s() Can not remove so change menu item at pos %d : error was %s", __FUNCTION__, item_pos, aaa::system::get_err_message().c_str() );
	return false;
}

bool c_system_context_menu::insert_separator( INT32 item_pos )
{
	bool b_ret = false;

	AAA_MENU_ITEM_INFO item;
	item.cbSize			= sizeof(MENUITEMINFOW);
	item.fMask			= MIIM_FTYPE;
	item.fType			= MFT_SEPARATOR;
	item.fState			= MFS_DISABLED | MFS_UNHILITE;
	item.wID			= 0;
	item.hSubMenu		= nullptr;
	item.hbmpChecked	= nullptr;
	item.hbmpUnchecked	= nullptr;
	item.dwItemData		= 0;
	item.dwTypeData		= nullptr;
	item.cch			= 0;
	item.hbmpItem		= nullptr;

	BOOL ret = ::InsertMenuItemA( _hd_menu, item_pos, TRUE, &item );
	if( ret == 0 )
		ERR_PRINT_STRING( "%s() Can not add menu item : error was %s", __FUNCTION__, aaa::system::get_err_message().c_str() );
	else
		b_ret = true;

	return b_ret;
}
bool c_system_context_menu::add_separator( void )
{
	return insert_separator( get_item_nb() );
}
//todo encapsulate InsertMenuW and InsertMenuItemW and deal with limit on ITEM_POS there

bool c_system_context_menu::insert_sub_menu( CONST INT32 item_pos, C_PCHAR_C label, HMENU menu )
{
	bool b_ret = false;
#if USE_WIDE()
	wchar_t* txt = sysutils::utf8_to_unicode( label );
	BOOL ret = ::InsertMenuW( _hd_menu, get_item_nb(), MF_BYPOSITION | MF_POPUP | MF_STRING, (UINT_PTR)menu, txt );
	sysutils::free_str_tmp( txt );
#else
	BOOL ret = ::InsertMenuA( _hd_menu, item_pos, MF_BYPOSITION | MF_POPUP | MF_STRING, (UINT_PTR)menu, label );
#endif
	if( ret == 0 )
		ERR_PRINT_STRING( "%s() Can not add sub menu item : error was %s", __FUNCTION__, aaa::system::get_err_message().c_str() );
	else
		b_ret = true;
	return b_ret;
}
bool c_system_context_menu::add_sub_menu( C_PCHAR_C label, HMENU menu )
{
	return insert_sub_menu( get_item_nb(), label, menu );
}

bool c_system_context_menu::change_sub_menu( INT32 CONST item_pos, C_PCHAR_C label, HMENU menu )
{
	bool b_ret = false;

	if( remove_item( item_pos ) )
	{
#if USE_WIDE()
		wchar_t* txt = sysutils::utf8_to_unicode( label );
		BOOL ret = ::InsertMenuW( _hd_menu, item_pos, MF_BYPOSITION | MF_POPUP | MF_STRING, (UINT_PTR)menu, txt );
		sysutils::free_str_tmp(txt);
#else
		BOOL ret = ::InsertMenuA( _hd_menu, item_pos, MF_BYPOSITION | MF_POPUP | MF_STRING, (UINT_PTR)menu, label );
#endif
		if( ret == 0 )
			ERR_PRINT_STRING( "%s() Can not add sub menu item : error was %s", __FUNCTION__, aaa::system::get_err_message().c_str() );
		else
			b_ret = true;
	}
	else
	{
		ERR_PRINT_STRING( "%s() Can not remove so change sub menu at pos %d : error was %s", __FUNCTION__, item_pos, aaa::system::get_err_message().c_str() );
	}
	return b_ret;
}

void c_system_context_menu::show( void )
{
	/////////////////////////////////////////////
	//
	// TPM_LEFTBUTTON		The user can select menu items with only the left mouse button.
	// TPM_NONOTIFY			The menu and function does not send messages to the window identified by hWnd.
	//
	// Call GetSystemMetrics with SM_MENUDROPALIGNMENT to determine the correct horizontal alignment flag.
	// This is essential for creating an optimal user experience, especially when developing Microsoft Tablet PC applications.
	//
	/////////////////////////////////////////////


	INT32 pos[2];
	n_cursor::get_position_pixel_screen( pos );

	UINT alignFlag	= (UINT)::GetSystemMetrics( SM_MENUDROPALIGNMENT );
	HWND win		= get_window_main_handle();

	BOOL ret = ::TrackPopupMenuEx
		(
			_hd_menu,	alignFlag | TPM_LEFTBUTTON, // | TPM_NONOTIFY, 
			pos[0], pos[1],
			win,	nullptr
			);

	if( ret == 0 )
		DBG_PRINT_STRING( "%s() Can not show menu : error was %s", __FUNCTION__, aaa::system::get_err_message().c_str() );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//		GET / SET
///////////////////////////////////////////////////////////////////////////////////////////////////
void c_system_context_menu::set_mouse_button_attached( mouse::BUTTON button )
{
	_attached_mouse_button = button;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//		DO
///////////////////////////////////////////////////////////////////////////////////////////////////
void c_system_context_menu::do_command( INT32 CONST item_id )
{
	(*_callback)( item_id );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//		c_system_context_menu_factory class
///////////////////////////////////////////////////////////////////////////////////////////////////
namespace
{
	c_system_context_menu_factory*	p_instance_unique = nullptr;
}

c_system_context_menu_factory * c_system_context_menu_factory::create_instance( void )
{
	if( p_instance_unique )
		debug_break( "%s() already an instance pointer", __FUNCTION__ );
	else
	{
		p_instance_unique = new c_system_context_menu_factory();
		p_instance_unique->init();
	}
	return p_instance_unique;
}

c_system_context_menu_factory * c_system_context_menu_factory::get_instance( void )
{
	if( !p_instance_unique )
		debug_break( "%s() null instance", __FUNCTION__ );
	return p_instance_unique;
}

void c_system_context_menu_factory::release_instance( void )
{
	if( !p_instance_unique )
		debug_break( "%s() null instance", __FUNCTION__ );
	else
	{
		p_instance_unique->release();
		delete p_instance_unique;
		p_instance_unique = nullptr;
	}
}

c_system_context_menu_factory::c_system_context_menu_factory( void )
	// Members init
	:_p_menu_active			( nullptr )
	,_id_generator			( 0 )
	,_b_menu_loop_active	( false )
{}

c_system_context_menu_factory::~c_system_context_menu_factory( void )
{}

void c_system_context_menu_factory::init( void )
{
}

void c_system_context_menu_factory::release( void )
{
	auto it = _map_menu.begin();
	c_system_context_menu* menu = nullptr;

	for( ; it!=_map_menu.end(); it++ )
	{
		menu = it->second;
		it->second = nullptr;
		c_system_context_menu::release_ptr( menu );
	}
	_map_menu.clear();

	_p_menu_active	= nullptr;
}

INT32 c_system_context_menu_factory::create_menu( PT_MENU_FN callback )
{
	_p_menu_active = c_system_context_menu::create_ptr( ++_id_generator, callback );

	_map_menu[ _id_generator ] = _p_menu_active;

	return _id_generator;
}

void c_system_context_menu_factory::release_menu( INT32 CONST menu_id )
{
	auto it = _map_menu.find( menu_id );
	if( it != _map_menu.end() )
	{
		c_system_context_menu* p_menu = it->second;
#if AAA_DEBUG()
	if( p_menu == _p_menu_active )
		debug_break( "try to release active menu" );
#endif	
		_map_menu.erase( it );
		c_system_context_menu::release_ptr( p_menu );
	}
	else
		DBG_PRINT_STRING( "%s() No menu at id %d.", __FUNCTION__, menu_id );
}

c_system_context_menu* c_system_context_menu_factory::find( INT32 CONST menu_id )
{
	c_system_context_menu* p_menu;
	if( menu_id <= 0 )
	{
		DBG_PRINT_STRING( "%s() menu_id %d need to be > 0", __FUNCTION__, menu_id );
		p_menu = nullptr;
	}
	else
	{
		auto it = _map_menu.find(menu_id);
		if( it != _map_menu.end() )
			p_menu = it->second;
		else
		{
			DBG_PRINT_STRING( "%s() did not find menu_id %d need to be > 0", __FUNCTION__, menu_id );
			p_menu = nullptr;
		}
	}
	return p_menu;
}

INT32 c_system_context_menu_factory::get_item_nb( void )
{
	if( _p_menu_active == nullptr )
	{
		debug_break( "%s() no active menu", __FUNCTION__ );
		return 0;
	}
	return _p_menu_active->get_item_nb();
}

bool c_system_context_menu_factory::insert_item( INT32 CONST item_pos,	C_PCHAR_C label, INT32 CONST value, INT32 CONST check )
{
	if( _p_menu_active == nullptr )
	{
		debug_break( "%s() no active menu", __FUNCTION__ );
		return false;
	}
	return _p_menu_active->insert_item( item_pos, label, value, check );
}

bool c_system_context_menu_factory::insert_item( INT32 CONST menu_id, INT32 CONST item_pos,	C_PCHAR_C label, INT32 CONST value, INT32 CONST check )
{
	bool b_ret = false;
	auto pt = find( menu_id );
	if( pt )
		b_ret = pt->insert_item( item_pos, label, value, check );
	else
		DBG_PRINT_STRING( "%s() No menu with id %d.", __FUNCTION__, menu_id );
	return b_ret;
}

bool c_system_context_menu_factory::add_item( C_PCHAR_C label, INT32 CONST value, INT32 CONST check )
{
	if( _p_menu_active == nullptr )
	{
		debug_break( "%s() no active menu", __FUNCTION__ );
		return false;
	}
	return _p_menu_active->add_item( label, value, check );
}

bool c_system_context_menu_factory::add_item( INT32 CONST menu_id, C_PCHAR_C label, INT32 CONST value, INT32 CONST check )
{
	bool b_ret = false;
	auto pt = find( menu_id );
	if( pt )
		b_ret = pt->add_item( label, value, check );
	else
		DBG_PRINT_STRING( "%s() No menu with id %d.", __FUNCTION__, menu_id );
	return b_ret;
}


bool c_system_context_menu_factory::change_item( INT32 CONST item_pos, C_PCHAR_C label, INT32 CONST value, INT32 CONST check )
{
	if( _p_menu_active == nullptr )
	{
		debug_break( "%s() no active menu", __FUNCTION__ );
		return false;
	}
	return _p_menu_active->change_item( item_pos, label, value, check );
}

bool c_system_context_menu_factory::change_item( INT32 CONST menu_id, INT32 CONST item_pos, C_PCHAR_C label, INT32 CONST value, INT32 CONST check )
{
	bool b_ret = false;
	auto pt = find( menu_id );
	if( pt )
		b_ret = pt->change_item( item_pos, label, value, check );
	else
		DBG_PRINT_STRING( "%s() No menu at id %d.", __FUNCTION__, menu_id );
	return b_ret;
}

bool c_system_context_menu_factory::remove_item( INT32 CONST item_pos )
{
	if( _p_menu_active == nullptr )
	{
		debug_break( "%s() no active menu", __FUNCTION__ );
		return false;
	}
	return _p_menu_active->remove_item( item_pos );
}

bool c_system_context_menu_factory::add_separator( void )
{
	if( _p_menu_active == nullptr )
	{
		debug_break( "%s() no active menu", __FUNCTION__ );
		return false;
	}
	return _p_menu_active->add_separator();
}
bool c_system_context_menu_factory::add_separator( INT32 CONST menu_id )
{
	bool b_ret = false;
	auto pt = find( menu_id );
	if( pt )
		b_ret = pt->add_separator();
	else
		DBG_PRINT_STRING( "%s() No menu at id %d.", __FUNCTION__, menu_id );
	return b_ret;
}

bool c_system_context_menu_factory::insert_separator( INT32 CONST item_pos )
{
	if( _p_menu_active == nullptr )
	{
		debug_break( "%s() no active menu", __FUNCTION__ );
		return false;
	}
	return _p_menu_active->insert_separator( item_pos );
}
bool c_system_context_menu_factory::insert_separator( INT32 CONST menu_id, INT32 CONST item_pos )
{
	bool b_ret = false;
	auto pt = find( menu_id );
	if( pt )
		b_ret = pt->insert_separator( item_pos );
	else
		DBG_PRINT_STRING( "%s() No menu at id %d.", __FUNCTION__, menu_id );
	return b_ret;
}

bool c_system_context_menu_factory::insert_sub_menu( INT32 CONST item_pos, C_PCHAR_C label, INT32 CONST menu_sub_id )
{
	if( _p_menu_active == nullptr )
	{
		debug_break( "%s() no active menu", __FUNCTION__ );
		return false;
	}
	bool b_ret = false;
	auto pt_sub = find( menu_sub_id );
	if( pt_sub )
		b_ret = _p_menu_active->insert_sub_menu( item_pos, label, pt_sub->_hd_menu );
	else
		DBG_PRINT_STRING( "%s() No menu at id %d.", __FUNCTION__, menu_sub_id );

	return b_ret;
}
bool c_system_context_menu_factory::insert_sub_menu( INT32 CONST menu_id, INT32 CONST item_pos, C_PCHAR_C label, INT32 CONST menu_sub_id )
{
	bool b_ret = false;
	auto pt = find( menu_id );
	if( pt )
	{
		auto pt_sub = find( menu_sub_id );
		if( pt_sub )
			b_ret = pt->insert_sub_menu( item_pos, label, pt_sub->_hd_menu );
		else
			DBG_PRINT_STRING( "%s() No sub menu with id %d.", __FUNCTION__, menu_sub_id );
	}
	else
		DBG_PRINT_STRING( "%s() No menu with id %d.", __FUNCTION__, menu_id );
	return b_ret;
}
bool c_system_context_menu_factory::add_sub_menu( C_PCHAR_C label, INT32 CONST menu_sub_id )
{
	if( _p_menu_active == nullptr )
	{
		debug_break( "%s() no active menu", __FUNCTION__ );
		return false;
	}
	bool b_ret = false;
	auto pt_sub = find( menu_sub_id );
	if( pt_sub )
		b_ret = _p_menu_active->add_sub_menu( label, pt_sub->_hd_menu );
	else
		DBG_PRINT_STRING( "%s() No menu at id %d.", __FUNCTION__, menu_sub_id );

	return b_ret;
}
bool c_system_context_menu_factory::add_sub_menu( INT32 CONST menu_id, C_PCHAR_C label, INT32 CONST menu_sub_id )
{
	bool b_ret = false;
	auto pt = find( menu_id );
	if( pt )
	{
		auto pt_sub = find( menu_sub_id );
		if( pt_sub )
			b_ret = pt->add_sub_menu( label, pt_sub->_hd_menu );
		else
			DBG_PRINT_STRING( "%s() No sub menu with id %d.", __FUNCTION__, menu_sub_id );
	}
	else
		DBG_PRINT_STRING( "%s() No menu with id %d.", __FUNCTION__, menu_id );
	return b_ret;
}
bool c_system_context_menu_factory::change_sub_menu( INT32 CONST item_pos, C_PCHAR_C label, INT32 CONST menu_sub_id )
{
	bool b_ret = false;
	if( _p_menu_active )
	{
		auto pt_sub = find( menu_sub_id );
		if( pt_sub )
			b_ret = _p_menu_active->change_sub_menu( item_pos, label, pt_sub->_hd_menu );
		else
			DBG_PRINT_STRING( "%s() No sub menu with id %d.", __FUNCTION__, menu_sub_id );
	}
	else
		DBG_PRINT_STRING( "%s() No active menu.", __FUNCTION__ );
	return b_ret;
}
bool c_system_context_menu_factory::change_sub_menu( INT32 CONST menu_id, INT32 CONST item_pos, C_PCHAR_C label, INT32 CONST menu_sub_id )
{
	bool b_ret = false;
	auto pt = find( menu_id );
	if( pt )
	{
		auto pt_sub = find( menu_sub_id );
		if( pt_sub )
			b_ret = pt->change_sub_menu( item_pos, label, pt_sub->_hd_menu );
		else
			DBG_PRINT_STRING( "%s() No sub menu with id %d.", __FUNCTION__, menu_sub_id );
	}
	else
		DBG_PRINT_STRING( "%s() No menu with id %d.", __FUNCTION__, menu_id) ;
	return b_ret;
}


void c_system_context_menu_factory::attach_menu( const mouse::BUTTON button )
{
	if( _p_menu_active == nullptr )
		debug_break( "%s() no active menu", __FUNCTION__ );
	else
		_p_menu_active->set_mouse_button_attached( button );
}

void c_system_context_menu_factory::attach_menu( INT32 CONST menu_id, const mouse::BUTTON button )
{
	auto pt = find( menu_id );
	if( pt )
		pt->set_mouse_button_attached( button );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//		GET / SET
///////////////////////////////////////////////////////////////////////////////////////////////////

void c_system_context_menu_factory::set_menu_active( INT32 CONST menu_id )
{
	_p_menu_active = find( menu_id );

	system_event_reader::set_context_menu( _p_menu_active );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//		DO
///////////////////////////////////////////////////////////////////////////////////////////////////

void c_system_context_menu_factory::do_command( INT32 CONST menu_id, INT32 CONST item_id )
{
	auto pt = find( menu_id );
	if( pt )
		pt->do_command( item_id );
	else
		DBG_PRINT_STRING( "%s() No menu with id %d.", __FUNCTION__, menu_id );
}