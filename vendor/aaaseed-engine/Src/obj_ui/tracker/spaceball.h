
#ifdef AAA_SPACEBALL_H
#error "SPACEBALL_H included more than once."
#endif
#define AAA_SPACEBALL_H 1


#ifndef AAA_TRACKERS_H
#	include "obj_ui/tracker/trackers.h"
#endif

#if AAA_TRACKER_SPACEBALL()
#	ifndef	AAA_SENSOR_6DOF_H
#		include "sensor_6dof.h"
#	endif

class	c_spaceball final : public c_sensor_6dof
{
	FACTORY_DECLARE( c_spaceball, c_sensor_6dof );
protected:
	INT32	_port_nb;
	bool	_b_open;
public:

	virtual	void	update();
//			void	set_port( INT32 port_id );
	virtual	AAA_ERR	open();
	virtual	void	close();
};

extern	c_spaceball*	spaceball;

#endif	//#if AAA_TRACKER_SPACEBALL()
