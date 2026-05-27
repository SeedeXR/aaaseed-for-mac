
#ifdef AAA_SYSTEM_CONTEXT_MENU_H
#error "SYSTEM_CONTEXT_MENU_H included more than once."
#endif
#define AAA_SYSTEM_CONTEXT_MENU_H 1


#ifndef AAA_OBJ_H
#	include "infrastructure/obj/obj.h"
#endif
#ifndef	AAA_AAA_OS_H
#	include "aaa_os.h"
#endif
#ifndef _STRING_
#	include <string>
#endif
#ifndef _MAP_
#	include <map>
#endif
#ifndef AAA_SYSTEMMOUSE_H
#	include "system/shared/SystemMouse.h"
#endif


///////////////////////////////////////////////////////////////////////////////////////////////////
//		c_system_context_menu class
///////////////////////////////////////////////////////////////////////////////////////////////////

/**
* @class c_system_context_menu
* @brief system contextual menu.
*/
class c_system_context_menu final : public c_obj
{
	friend class c_system_context_menu_factory;

protected:
	HMENU				_hd_menu;
	INT32				_id;
	PT_MENU_FN			_callback;
	mouse::BUTTON		_attached_mouse_button;

public:
	/** Create new pointer. */
	static c_system_context_menu* create_ptr( INT32 CONST id, PT_MENU_FN callback );
	/** Release and delete class pointer. */
	static void release_ptr( c_system_context_menu* pt );


protected:
	C_NO_CPY_MOVE( c_system_context_menu )
	/** \! Class constructor. */
	c_system_context_menu( void );
	/** \! Class destructor. */
	virtual ~c_system_context_menu( void );


	/** \! Allocate and init class members. */
	void init( INT32 CONST id, PT_MENU_FN callback );
	/** \! Release and delete class members. */
	void release( void );

	INT32 get_item_nb( void );

	/** \! Remove menu item. */
	bool remove_item(		INT32 CONST item_pos );

	/** \! Insert menu item. */
	bool insert_item(		INT32 CONST item_pos,	C_PCHAR_C label, INT32 CONST value, INT32 CONST check = 0 );
	/** \! Add menu item. */
	bool add_item(									C_PCHAR_C label, INT32 CONST value, INT32 CONST check = 0 );
	/** \! Change menu item in active menu. */
	bool change_item(		INT32 CONST item_pos,	C_PCHAR_C label, INT32 CONST value,	INT32 CONST check = 0 );

	/** \! Insert separator. */
	bool insert_separator(	INT32 CONST item_pos );
	/** \! Insert separator. */
	bool add_separator( void );

	/** \! Insert sub menu. */
	bool insert_sub_menu(	INT32 CONST item_pos,	C_PCHAR_C label, HMENU CONST hd_menu_sub );
	/** \! Add sub menu. */
	bool add_sub_menu(								C_PCHAR_C label, HMENU CONST hd_menu_sub );
	/** \! Change sub menu in active menu. */
	bool change_sub_menu(	INT32 CONST item_pos,	C_PCHAR_C label, HMENU CONST hd_menu_sub );

	/** \! call the associated fn. */
	void do_command( INT32 CONST item_id );


public:
	/** \! Show menu. */
	void show( void );


	///////////////////////////////////////////////////////////////////////////////////////////////
	//		GET / SET
	///////////////////////////////////////////////////////////////////////////////////////////////

public:
	/** \! Get attached mouse button. */ 
	const mouse::BUTTON get_mouse_button_attached( void ) const;
	/** \! Set attached mouse button (default is mouse::BUTTON_RIGHT). */
	void set_mouse_button_attached( const mouse::BUTTON button );
};


//=================================================================================================
inline const mouse::BUTTON c_system_context_menu::get_mouse_button_attached( void ) const
{
	return _attached_mouse_button;
}




///////////////////////////////////////////////////////////////////////////////////////////////////
//		c_system_context_menu_factory class
///////////////////////////////////////////////////////////////////////////////////////////////////

/**
* @class c_system_context_menu_factory
* @brief create new menu and manages menu content.
*/
class c_system_context_menu_factory : public c_obj
{
private:
	std::map< INT32, c_system_context_menu* >	_map_menu;			//<! Menus map
	c_system_context_menu*						_p_menu_active;		//<! Active context menu.
	INT32										_id_generator;		//<! Incremental menu id
	bool										_b_menu_loop_active;

public:
	/** Create unique instance. */
	static c_system_context_menu_factory* create_instance( void );
	/** Get unique instance. */
	static c_system_context_menu_factory* get_instance( void );
	/** Release and delete unique instance. */
	static void release_instance( void );

protected:
	C_NO_CPY_MOVE( c_system_context_menu_factory )
	/** \! Class constructor. */
	c_system_context_menu_factory( void );
	/** \! Class destructor. */
	virtual ~c_system_context_menu_factory( void );

	/** \! Allocate and init class members. */
	void init( void );
	/** \! Release and delete class members. */
	void release( void );
	c_system_context_menu* find( INT32 CONST menu_id );

public:
	///////////////////////////////////////////////////////////////////////////////////////////////
	//		GET / SET
	///////////////////////////////////////////////////////////////////////////////////////////////
	/** \! Get active menu. */
	FINLINE	c_system_context_menu * get_menu_active( void ) CONST	{ return _p_menu_active; }
	/** \! Set active context menu. */
			void set_menu_active(	INT32 CONST menu_id		);

	FINLINE	void set_menu_loop_active( bool b_enter )	{	_b_menu_loop_active = b_enter;	}
			bool is_menu_loop_active() CONST			{	return _b_menu_loop_active;		}

	/** \! Add menu and activate it. 
	* Returns new menu id.
	*/
	INT32 create_menu(		PT_MENU_FN callback );
	/** \! Release menu. */
	void release_menu(		INT32 CONST menu_id );

	INT32 get_item_nb( void );

	/** \! Insert menu item in active menu. */
	bool insert_item(								INT32 CONST item_pos,	C_PCHAR_C label, INT32 CONST value, INT32 CONST check=0 );
	/** \! Insert menu item in target menu. */
	bool insert_item(		INT32 CONST menu_id,	INT32 CONST item_pos,	C_PCHAR_C label, INT32 CONST value, INT32 CONST check=0 );
	/** \! Add menu item in active menu. */
	bool add_item(															C_PCHAR_C label, INT32 CONST value, INT32 CONST check=0 );
	/** \! Add menu item in target menu. */
	bool add_item(			INT32 CONST menu_id,							C_PCHAR_C label, INT32 CONST value, INT32 CONST check=0 );

	/** \! Change menu item in active menu. */
	bool change_item(		INT32 CONST menu_id,							C_PCHAR_C label, INT32 CONST value, INT32 CONST check=0 );
	/** \! Change menu item in target menu. */
	bool change_item(		INT32 CONST menu_id,	INT32 CONST item_pos,	C_PCHAR_C label, INT32 CONST value, INT32 CONST check=0 );

	/** \! Remove menu item in active menu. */
	bool remove_item(								INT32 CONST item_pos );

	/** \! Insert separator in active menu. */
	bool insert_separator(							INT32 CONST item_pos );
	/** \! Insert separator in target menu. */
	bool insert_separator(	INT32 CONST menu_id,	INT32 CONST item_pos );
	/** \! Add separator in active menu. */
	bool add_separator( void );
	/** \! Add separator in target menu. */
	bool add_separator(		INT32 CONST menu_id		);

	/** \! Add sub menu in active menu. */
	bool insert_sub_menu(							INT32 CONST item_pos,	C_PCHAR_C label, INT32 CONST menu_sub_id );
	/** \! Add sub menu in target menu. */
	bool insert_sub_menu(	INT32 CONST menu_id,	INT32 CONST item_pos,	C_PCHAR_C label, INT32 CONST menu_sub_id );
	/** \! Add sub menu in active menu. */
	bool add_sub_menu(														C_PCHAR_C label, INT32 CONST menu_sub_id );
	/** \! Add sub menu in target menu. */
	bool add_sub_menu(		INT32 CONST menu_id,							C_PCHAR_C label, INT32 CONST menu_sub_id );
	/** \! Change sub menu in active menu. */
	bool change_sub_menu(							INT32 CONST item_pos,	C_PCHAR_C label, INT32 CONST menu_sub_id );
	/** \! Change sub menu in target menu. */
	bool change_sub_menu(	INT32 CONST menu_id,	INT32 CONST item_pos,	C_PCHAR_C label, INT32 CONST menu_sub_id );

	/** \! Attach active menu to mouse button. */
	void attach_menu( const mouse::BUTTON button );
	/** \! Attach menu to mouse button. */
	void attach_menu(		INT32 CONST menu_id,	mouse::BUTTON CONST button );

	/** \! call the associated fn. */
	void do_command(		INT32 CONST menu_id,	INT32 CONST id );
};
