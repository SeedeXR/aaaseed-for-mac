
#ifdef AAA_EVENT_MIDI_H
#error "EVENT_MIDI_H included more than once."
#endif
#define AAA_EVENT_MIDI_H 1


#ifndef AAA_AAA_EVENT_H
#	include "ui/AAA_event.h"
#endif

class	c_event_midi final : public c_event
{
public:
	static	c_event*	create();

	//			c_event_midi();
	//	virtual	~c_event_midi();

	virtual	C_PCHAR_C	get_type_str()	{	return "midi";	}
	virtual C_PCHAR_C	get_sub_type_str();
	virtual	bool		is_to_send();
private:
	virtual	void		process_low();
};

