
#ifdef AAA_EVENT_KEYBOARD_H
#error "EVENT_KEYBOARD_H included more than once."
#endif
#define AAA_EVENT_KEYBOARD_H 1


#ifndef AAA_AAA_EVENT_H
#	include "ui/AAA_event.h"
#endif

class	c_event_keyboard final : public c_event
{
public:
	static	c_event*	create();

//			c_event_keyboard();
//	virtual	~c_event_keyboard();

			void		get_event_keyboard(			INT32& key_code,	INT32& modifiers,	INT32& x,	INT32& y );
			void		set_event_keyboard(			INT32 key_code,		INT32 modifiers,	INT32 x,	INT32 y );
			void		set_event_keyboard_special(	INT32 key_code,		INT32 modifiers,	INT32 x,	INT32 y );

	virtual	C_PCHAR_C	get_type_str()	{	return "keyboard";	}
	virtual	C_PCHAR_C	get_sub_type_str();
	virtual	bool		is_to_send();
private:
	virtual	void		process_low();
};
