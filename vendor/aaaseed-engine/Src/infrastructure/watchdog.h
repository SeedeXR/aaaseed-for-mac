
#ifdef AAA_WATCHDOG_H
#error "WATCHDOG_H included more than once."
#endif
#define AAA_WATCHDOG_H 1


#ifndef AAA_OBJ_H
#	include "obj.h"
#endif

struct lua_State;


class	c_watchdog final : public c_obj
{
	friend class	c_net;
private:
	static	bool	b_on;
public:
	static			void	trig_exit();
	static			bool	is_exit();
	static	FINLINE bool	is_on()			{ return b_on; }
	static			void	set_on(	bool b_in );
	static			void	main();
	static			void	set_loop_time( REAL loop_time_in );

	static			void	register_aaalua( lua_State* L );
};

