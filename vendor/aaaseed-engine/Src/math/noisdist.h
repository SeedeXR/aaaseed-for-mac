
#ifdef AAA_NOISDIST_H
#error "NOISDIST_H included more than once."
#endif
#define AAA_NOISDIST_H 1


#ifndef AAA_FN1D_H
#	include "math/fn1d/fn1d.h"
#endif
#ifndef AAA_AAA_STR_H
#	include "aaa_str.h"
#endif

//////
////	FN 1D PERIODIC DONE WITH AN ARRAY OF SAMPLES
//			AND USING A RANDOM GENERATOR
class	c_fn1d_periodic_turb_dist final : public c_fn1d_periodic_table_rand
{
private:
	INT32	_index_col_out;
	FILE	*_file;
	DOUBLE	_x;
	DOUBLE	_x0;
	DOUBLE	_y0;

protected:
	UINT32	_seed;
	INT32	_index;
	INT32	_prev;
	INT32	_next;
	DOUBLE	_cur;
	DOUBLE	_curve_surface;
	DOUBLE	_curve_surface_step;
	INT32	_target;
	REAL	*_p;
	INT32	_xp;
private:
	void	save_value( REAL val );
public:
	c_fn1d_periodic_turb_dist();
	virtual	~c_fn1d_periodic_turb_dist()	{};
	void	fill();
	void	integrate_start();
	void	integrate_next();
	void	save_cpp_to_file( o_str CONST & filename );
};

