
#ifdef AAA_DIALOG_STR_H
#error "DIALOG_STR_H included more than once."
#endif
#define AAA_DIALOG_STR_H 1


#ifndef AAA_DIALOG_WRAPPER_H
#	include "dialog_wrapper.h"
#endif

class	o_str;
class	c_bdd_ft_face;
class	c_bind;


class	c_dialog_wrapper_str final : public c_dialog_wrapper
{
protected:
	c_bind*	_bind;

	INT32	_drag_message;
	INT32	_drag_index_start;
	INT32	_drag_index;

	bool	_b_control_no;
	bool	_b_text_changing;

	INT32	_idc_typein_to_use;
	o_str	_value_initial;		//	used for cancel or revert
	o_str	_o_buf;				//	used as intermediate value
	o_str	_o_tmp;				//	used to detect change after lua callback
	INT32	_sel_begin;
	INT32	_sel_end;
	INT32	_sel_prev_begin;
	INT32	_sel_prev_end;

public:
	c_dialog_wrapper_str( c_obj_ui* obj );
	virtual ~c_dialog_wrapper_str();

	virtual	c_bind*	get_bind_list()	CONST { return _bind; };
			void	set_bind_list(	c_bind* in );
	
			void	init_list();

	virtual bool	do_callback(	UINT message,	WPARAM wParam, LPARAM lParam );
			bool	do_scroll(						WPARAM wParam, LPARAM lParam );
			void	do_text_changed( C_PCHAR_C str );

			void	list_set(		c_bind* bl,		INT32 index );
			void	list_set_all(					INT32 index );
			void	list_move_one(	c_bind* bl,		INT32 dst, INT32 src );
			bool	list_move(		HWND hd_list,	INT32 dst, INT32 src );

			void	set_control_no( bool b );
};
