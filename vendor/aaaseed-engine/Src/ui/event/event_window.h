
#ifdef AAA_EVENT_WINDOW_H
#error "EVENT_WINDOW_H included more than once."
#endif
#define AAA_EVENT_WINDOW_H 1


#ifndef AAA_AAA_EVENT_H
#	include "ui/AAA_event.h"
#endif

class	c_event_window final : public c_event
{
private:
    INT32   _x{ 0 };
    INT32   _y{ 0 };

public:
    static	CHAR*	    get_but_name( INT32 button );
    static	CHAR*	    get_state_name( INT32 state );

    static	c_event*	create();

            void        set_event_focus(  bool CONST b_got );
            void        set_event_move(   INT32 CONST  x, INT32 CONST  y );
            void        set_event_resize( INT32 CONST sx, INT32 CONST sy );
            void        set_event_close();

            void        process_focus_got();
            void        process_focus_lost();
            void        process_move();
            void        process_resize();


    virtual	C_PCHAR_C	get_type_str()	{	return "window";	}
    virtual	C_PCHAR_C	get_sub_type_str();
    virtual	bool		is_to_send();
private:
    virtual	void		process_low();
};

