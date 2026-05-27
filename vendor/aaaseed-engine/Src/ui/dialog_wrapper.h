
#ifdef AAA_DIALOG_WRAPPER_H
#error "DIALOG_WRAPPER_H included more than once."
#endif
#define AAA_DIALOG_WRAPPER_H 1


#ifndef AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif
#ifndef AAA_AAA_MUTEX_H
#	include "aaa/aaa_mutex.h"
#endif
#ifndef _VECTOR_
#	include <vector>
#endif

class c_obj_ui;
extern	bool	gb_dlg_verbose;

void	DLG_PRINT_STRING( C_PCHAR fmt, ... );
extern	void	print_callback_event( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

class	c_dialog_wrapper : public c_obj
{
protected:
	static	std::vector<c_dialog_wrapper*>	wrappers;	//todo should be private

	static	aaa::MUTEX_RECURSIVE			access;

			bool			_b_lua_callback;
			INT32			_lua_id;

private:
	static	c_dialog_wrapper*	wrapping;
	static	c_dialog_wrapper*	find_from_hd(	HWND hd);
	static	void				attach(			HWND hd);

public:
	static	void				add(			c_dialog_wrapper* pdw);
	static	void				cleanup();
	static	bool				is_dialog_on_same_data( c_param CONST * param );

	static	bool				do_callback(	HWND hd_dlg, UINT message, WPARAM wParam, LPARAM lParam );
	static	void				update_all();
private:
			HWND				_hd_dlg;
			bool				_b_to_destroy;
		
			c_obj_ui*			_obj;
			p_param				_param;
			o_str				_title;
			bool				_b_title_need_set;

			void CONST *		_pt_ref;

public:
	c_dialog_wrapper( c_obj_ui* obj );
	virtual ~c_dialog_wrapper();

	virtual	void				update() {} 
	virtual bool				do_callback( UINT message, WPARAM wParam, LPARAM lParam ) = 0;

			void				set_lua_callback( bool CONST b_on, INT32 CONST lua_id );

			void				set_hd( HWND hd );
			void				do_begin( C_PCHAR_C type );
			void				do_end( INT32 CONST return_value_int, C_PCHAR_C return_value );

	FINLINE HWND				get_hd()			CONST			{	 return _hd_dlg;		}
	FINLINE bool				is_hd( HWND hd )	CONST			{	 return _hd_dlg == hd;	}
	FINLINE bool				is_to_destroy()		CONST			{	 return _b_to_destroy;	}
	FINLINE p_param				get_param()			CONST			{	 return _param;			}
	FINLINE c_obj_ui*			get_obj()			CONST			{	 return _obj;			}

			void				set_param( p_param CONST param );
//todo 2023 Fev decided if we need this
//			void				set_obj( c_obj_ui* obj )			{	_obj = obj;				}
			void				set_title( C_PCHAR_C title );				

#ifdef WIN32
	FINLINE	HWND				get_dlg_item(		INT32 item )	{	return GetDlgItem( get_hd(), item ); }
	FINLINE	bool				is_dlg_but_checked(	INT32 item )	{	HWND hd = get_dlg_item(item); return hd && IsDlgButtonChecked( get_hd(), item ) == BST_CHECKED; }
#endif
};


extern void	
#ifdef	WIN32
__cdecl
#endif
do_dialog( void* res_id );

extern void	
#ifdef	WIN32
__cdecl
#endif
do_dialog_modeless( void* res_id );
