
#ifdef AAA_SHIP_H
#error "SHIP_H included more than once."
#endif
#define AAA_SHIP_H 1


#ifndef AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif
#ifndef AAA_SEEDCAM_H
#	include "draw/seedcam.h"
#endif

class	c_ship final : public c_obj_active_ui
{
	FACTORY_DECLARE(c_ship,c_obj_active_ui);
private:
	bool	reactor_state[	c_seedcam::RESET - c_seedcam::ROT_RIGHT];
	REAL	reactor_val[	c_seedcam::RESET - c_seedcam::ROT_RIGHT];
	INT32	s_navigation;

	REAL	speed_tra;
	REAL	speed_rot;

public:
	static	c_ship*	def;
	static	c_ship*	cur;

	virtual	void	param_init_pt();
	virtual	void	update();
			void	stop();
			void	move();
			void	idle();
			bool	set_reactor_state( INT32 index, bool state );
	};

/*
class ship
	{
	REAL pos[3];
	REAL rot[3]; 
	REAL vel[3]; 
	REAL acc[3]; 
	REAL mass;
	REAL vel_max; 
	REAL acc_max;
public:
	ship();
//	ST_SHIP *   ship_new(	    void);
	void	    stop();
	void	    accelerate( INT32 acc);
	void	    turn( INT32 acc_x, INT32 acc_y);
	void	    update( INT32 msec);
	};
*/

