
#ifdef AAA_FN1D_FBM_H
#error "FN1D_FBM_H included more than once."
#endif
#define AAA_FN1D_FBM_H 1


#ifndef AAA_FN1D_H
#	include "math/fn1d/fn1d.h"
#endif

//class c_param_def;

class	c_fn1d_fm : public c_fn1d_periodic_table_rand
{
protected:
	//	params
	REAL	_h;

public:
	c_fn1d_fm();
	virtual	~c_fn1d_fm() {};

		void		set_h( REAL h_in = .98 );
		void		set_fractal_dim( REAL dim_in )	{ set_h( REAL(2.) - dim_in ); }
		
//		c_param_def*	set_focus();
};

class	c_fn1d_fm_add final : public c_fn1d_fm
{
public:
	virtual		~c_fn1d_fm_add() {};
		void	compute();
};

