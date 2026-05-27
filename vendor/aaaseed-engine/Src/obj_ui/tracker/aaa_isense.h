
#ifdef AAA_AAA_ISENSE_H
#error "AAA_ISENSE_H included more than once."
#endif
#define AAA_AAA_ISENSE_H 1



#ifndef AAA_TRACKERS_H
#	include "obj_ui/tracker/trackers.h"
#endif

#if AAA_TRACKER_ISENSE()

#ifndef	AAA_SENSOR_6DOF_H
#	include "sensor_6dof.h"
#endif
#ifndef _ISD_isenseh
#	include "isense.h"
#endif

class	c_isense final : public c_tracker
{
	FACTORY_DECLARE(c_isense,c_tracker);
public:
	static	bool	b_dll_loaded;
protected:
	INT32			_handle;
	ISD_DATA_TYPE*	_data;
	INT32			_com_port_nb;
	c_sensor_6dof	_sensor;

private:
			void	data_alloc();
			void	data_dealloc();
//	ISD_STATION_CONFIG_TYPE  station;
public:
	virtual	void	param_init_pt();
			void	set_port( INT32 port_id );

	virtual	void	update_low();
	virtual	AAA_ERR	open();
	virtual	void	close();
};

extern	c_isense*		isense;
extern	void	isense_load_lib();
extern	void	isense_unload_lib();

#endif //#if AAA_TRACKER_ISENSE()
