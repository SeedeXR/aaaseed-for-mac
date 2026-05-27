
#ifdef AAA_POLHEMUS_H
#error "POLHEMUS_H included more than once."
#endif
#define AAA_POLHEMUS_H 1


#ifndef AAA_TRACKERS_H
#	include "obj_ui/tracker/trackers.h"
#endif
#ifndef	AAA_SENSOR_6DOF_H
#	include "sensor_6dof.h"
#endif

#if AAA_TRACKER_POLHEMUS()
class CPDIdev;
class	c_polhemus final : public c_tracker
{
	FACTORY_DECLARE( c_polhemus, c_tracker );
	c_sensor_6dof	_sensors[2];
public:
protected:
//	INT32		angle[3];
//	REAL		a_last[3];

	CPDIdev*	pdi;
public:
	virtual	void	param_init_pt();
#if AAA_WIN64()
			void	DisplayFrame( INT8* pBuf, INT32 dwSize )	{}
	virtual	void	update_low()	{}
	virtual	AAA_ERR	open()			{ return ERR_ANY; }
	virtual	void	close()			{}
#else
			void	DisplayFrame( INT8* pBuf, INT32 dwSize );
	virtual	void	update_low();
	virtual	AAA_ERR	open();
	virtual	void	close();
#endif //#if AAA_WIN64()
	virtual	AAA_ERR	load_do_after( o_str CONST & filename );
	virtual	AAA_ERR	save_do_after( o_str CONST & filename );

//	virtual	void	calibrate_default() {};
};

extern	c_polhemus*		polhemus;

#endif	//AAA_TRACKER_POLHEMUS
