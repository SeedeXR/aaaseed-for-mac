
#ifdef AAA_BDD_WATCHDOG_H
#error "BDD_WATCHDOG_H included more than once."
#endif
#define AAA_BDD_WATCHDOG_H 1


#ifndef	AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif

class	c_bdd_watchdog final : public c_bdd
{
	FACTORY_DECLARE( c_bdd_watchdog, c_bdd );
public:
	static	bool	b_dll_loaded;
private:

	static	c_bdd_watchdog*	cur;
//	static	REAL			_startup_time;
public:
	static	void	c_deinit();
	static	void	c_init();

protected:
	REAL			_timeout;
	REAL			_startup_delay_from;

	bool			_b_init;
	bool			_b_verbose;

#if AAA_WIN64()
			void	close()		{}
#else
	unsigned long	_handle;
			bool	open_dll();
			void	init_low();
			void	close();
#endif

public:
#if AAA_WIN64()
			void	init()	{}
#else
			void	init();
#endif
	virtual	void	param_init_pt();
	virtual	void	update();
	virtual	void	draw()		{};
};

