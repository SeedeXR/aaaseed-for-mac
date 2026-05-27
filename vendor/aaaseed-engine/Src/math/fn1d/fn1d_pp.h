
#ifdef AAA_FN1D_PP_H
#error "FN1D_PP_H included more than once."
#endif
#define AAA_FN1D_PP_H 1


#ifndef AAA_FN1D_H
#	include "math/fn1d/fn1d.h"
#endif

class c_param_def;

class	c_fn1d_phase_pertub final : public c_fn1d_periodic_table_rand
{
protected:
	INT32	_phase_nb;
	REAL	_period_out;
public:
	c_fn1d_phase_pertub( INT32 phase_nb_in = 100 );

	void				set_phase_nb( INT32 phase_nb_in = 100 );
	INT32				get_phase_nb()		CONST	{ return _phase_nb; };
	REAL				get_period_out()	CONST	{ return _period_out; };

	void				compute();

	CONST c_param_def*	set_focus();
};

