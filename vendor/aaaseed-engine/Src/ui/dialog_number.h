
#ifdef AAA_DIALOG_NUMBER_H
#error "DIALOG_NUMBER_H included more than once."
#endif
#define AAA_DIALOG_NUMBER_H 1


#ifndef AAA_DIALOG_WRAPPER_H
#	include "dialog_wrapper.h"
#endif

class	c_dialog_wrapper_number final : public c_dialog_wrapper
{
protected:
	DOUBLE			_value_initial;
    DOUBLE			_out_value;
 // INT32			_out_value_int;
    bool			_b_integer;

public:
	c_dialog_wrapper_number( c_obj_ui* obj, p_param param );
	virtual ~c_dialog_wrapper_number();

	virtual	void	update();

	virtual bool	do_callback( UINT message, WPARAM wParam, LPARAM lParam );

			void	set_edit( DOUBLE r );
			void	output_edit();

			void	set_type_integer( bool CONST b_integer )	{	_b_integer = b_integer;	}
};
