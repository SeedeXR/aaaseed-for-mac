
#ifdef AAA_BDD_YOCTO_H
#error "BDD_YOCTO_H included more than once."
#endif
#define AAA_BDD_YOCTO_H 1


#ifndef	AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif
#ifndef AAA_DELTA_T_H
#	include "time/delta_t.h"
#endif


class YDigitalIO;

class	c_bdd_yocto final : public c_bdd
{
	FACTORY_DECLARE( c_bdd_yocto, c_bdd );
public:


protected:
	bool		_b_init				{false};
	bool		_b_open_ui;
	bool		_b_verbose_ui;
	bool		_b_open;
	REAL		_polling_interval_ui;
	REAL		_check_time;
	INT32		_input				{};
	c_delta_t	_delta_t;

private:
	YDigitalIO* io					{nullptr};
public:
	virtual	void	param_init_pt();

	void			open();
	void			close();

	virtual	void	update();
	virtual	void	draw();
};
