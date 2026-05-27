
#ifdef AAA_BDD_TOUCHCO_H
#error "BDD_TOUCHCO_H included more than once."
#endif
#define AAA_BDD_TOUCHCO_H 1


#ifndef AAA_WRAP_TOUCHCO_H
//#	include "tracker/TouchCo/TC.h"
#	include "lib_wrappers/wrap_touchco.h"
#endif


#ifndef	AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif
class	c_img_2d;

class	c_bdd_touchco final : public c_bdd
{
	FACTORY_DECLARE( c_bdd_touchco, c_bdd );
public:
	static	bool			b_dll_loaded;
private:
	bool					_b_opened;
//	bool					_b_open_at_start;
	bool					_b_open_trig;
	bool					_b_close_trig;
	bool					_b_first;
	INT32					_device_index;
	REAL					_min;
	REAL					_max;
	REAL					_factor;
	bool					_b_min_max_show;

#if AAA_LIB_USE_TOUCHCO()
	TC_HANDLE				_tc_handle;
	TCSensorInfo			_tc_sensor_info;			// Sensor information
	TCContactFrame			_tc_frame_contact;			// Contact frame
	TCForceFrame			_tc_frame_force;			// Force frame
#endif
public:
	virtual	void	param_init_pt();

#if AAA_LIB_USE_TOUCHCO()
			static	INT32 enumerate();
			void	init();
			AAA_ERR	open();
			void	close();
#endif	//#if AAA_LIB_USE_TOUCHCO()

	virtual	void	update();
	virtual	void	draw();
};
