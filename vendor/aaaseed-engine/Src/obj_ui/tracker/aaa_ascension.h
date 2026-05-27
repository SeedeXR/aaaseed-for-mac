
#ifdef AAA_AAA_ASCENSION_H
#error "AAA_ASCENSION_H included more than once."
#endif
#define AAA_AAA_ASCENSION_H 1


#ifndef AAA_TRACKERS_H
#	include "obj_ui/tracker/trackers.h"
#endif

#if AAA_TRACKER_PCBIRD()

#ifndef	AAA_SENSOR_6DOF_H
#	include "sensor_6dof.h"
#endif

class	c_pcbird final : public c_tracker
{
	FACTORY_DECLARE(c_pcbird,c_tracker);
public:
	static	bool	b_dll_loaded;
protected:
	INT32			angle[3];
	REAL			a_last[3];
	c_sensor_6dof	_sensor;
public:
	virtual	void	param_init_pt();

#if AAA_WIN64()
	virtual	void	update_low()	{}
	virtual	AAA_ERR	open()			{ return ERR_ANY; }
	virtual	void	close()			{}
#else
	virtual	void	update_low();
	virtual	AAA_ERR	open();
	virtual	void	close();
#endif //#if AAA_WIN64()
};

extern	c_pcbird*	pcbird;
extern	void		bird_load_lib();
extern	void		bird_unload_lib();

#endif	//#if AAA_TRACKER_PCBIRD()
