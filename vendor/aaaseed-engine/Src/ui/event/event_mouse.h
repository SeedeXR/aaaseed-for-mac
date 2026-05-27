
#ifdef AAA_EVENT_MOUSE_H
#error "EVENT_MOUSE_H included more than once."
#endif
#define AAA_EVENT_MOUSE_H 1


#ifndef AAA_AAA_EVENT_H
#	include "ui/AAA_event.h"
#endif
#ifndef AAA_SYSTEMMOUSE_H
#	include "system/shared/SystemMouse.h"
#endif

class	c_event_mouse final : public c_event
{
public:
	static	CHAR*	get_but_name( INT32 button );
	static	CHAR*	get_state_name( INT32 state );

	static	c_event*	create();

	//			c_event_mouse();
	//	virtual	~c_event_mouse();

			void		get_event_button(	INT32& x,		INT32& y,		mouse::BUTTON& button,	mouse::STATE& state );
			void		set_event_button(	INT32 CONST x,	INT32 CONST y,	mouse::BUTTON button,	mouse::STATE state );
			void		get_event_move(		INT32& x,	INT32& y,	bool& b_passive );
			void		set_event_move(		INT32 CONST x,	INT32 CONST y,	bool b_passive );

	virtual	C_PCHAR_C	get_type_str()	{	return "mouse";	}
	virtual	C_PCHAR_C	get_sub_type_str();
	virtual	bool		is_to_send();
private:
	virtual	void		process_low();
};

