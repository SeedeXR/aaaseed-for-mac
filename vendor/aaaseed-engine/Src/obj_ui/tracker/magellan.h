
#ifdef AAA_MAGELLAN_H
#error "MAGELLAN_H included more than once."
#endif
#define AAA_MAGELLAN_H 1


#ifndef AAA_TRACKERS_H
#	include "obj_ui/tracker/trackers.h"
#endif

#if	AAA_TRACKER_MAGELLAN()

#ifndef	AAA_SENSOR_6DOF_H
#	include "sensor_6dof.h"
#endif

class	c_magellan final : public c_tracker
{
	FACTORY_DECLARE(c_magellan,c_tracker);
//	friend LRESULT __stdcall
//		magellan_win_proc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
//	static	bool	b_verbose_low;
protected:
public:
	c_sensor_6dof	_sensor;
public:
	virtual	void	param_init_pt();

	virtual void	update_low();
	virtual AAA_ERR	open();
	virtual	void	close();

	virtual	AAA_ERR	load_do_after( o_str CONST & filename );
	virtual	AAA_ERR	save_do_after( o_str CONST & filename );
};

extern	c_magellan*	magellan;

#endif	//#if AAA_TRACKER_MAGELLAN()


