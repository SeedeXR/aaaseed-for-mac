
#ifdef AAA_RAND_H
#error "RAND_H included more than once."
#endif
#define AAA_RAND_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
//todoq move most of it in cpp
#ifndef AAA_GAINBIAS_H
#	include "math/gainbias.h"
#endif

#define	RAND_I32()		rand()
#define	SRAND(s)		srand(s)
#define	RAND_UF()		( REAL(rand())   / REAL(RAND_MAX) )
#define	RAND_F()		( REAL(rand()*2) / REAL(RAND_MAX) - REAL(1) )

//////
////	BASE CLASS
//
class	c_rand
{
public:
	static	void	test();
	static	void	draw_in_rect(REAL x, REAL y, REAL w, REAL h);
protected:
	UINT32	_seed;
	static	CONST	REAL	factor_for_float;
public:
	c_rand()	{ set_seed(); };
			void	set_seed( UINT32 seed_new = 8 )	{ _seed = seed_new; };
			UINT32	get_seed()						{ return _seed; };

//	these fns should update seed of this object
	// the minimum one method to implement
	virtual UINT32	get_uint32() = 0;
//todoopt	use inline
//todoopt	implement get_floatunit [-1,1.]
	FINLINE FP32	get_fp32_01()				{ return FP32(((DOUBLE)get_uint32()) / aaa::BIGGEST<UINT32>);	};
	FINLINE FP32	get_fp32()					{ return get_fp32_01() - FP32(.5);					};
	template<typename T>
	FINLINE FP32	get_fp32_max( T CONST max )	{ return FP32(get_fp32_01() * max);					};
	template< class T >
	FINLINE void	get_square( T& x, T& y )
		{
			x = get_fp32();
			y = get_fp32();
		}
	template< class T >
	FINLINE void	get_circle( T& x, T& y )
		{
			do
				get_square( x, y );
			while( x*x + y*y > .25 );
		}

	//	these fns should not affect the seed of this object
	FINLINE UINT32	get_uint32_with_seed( UINT32 s )
		{
			UINT32	seed_prev;
			seed_prev = _seed;
			_seed = s;
			s = get_uint32();
			_seed = seed_prev;
			return s;
		};
	FINLINE REAL	get_ufloat_with_seed( UINT32 s )
		{
			UINT32	seed_prev;
			seed_prev = _seed;
			_seed = s;
			s = get_uint32();
			_seed = seed_prev;
			return ((REAL)s)*factor_for_float;
		};
	FINLINE REAL	get_float_with_seed( UINT32 s )
		{
			return ( get_ufloat_with_seed(s)-REAL(.5) );
		};
};

//////
////	WHITE NOISE
//			lin_portable is a clean but slow implementation
//			if lin work the same you should use it because it's faster
class	c_rand_lin_portable final : public c_rand
{
public:
	c_rand_lin_portable()	{};
	UINT32	get_uint32();
};

class	c_rand_lin : public c_rand
{
	UINT32 _count;	//	used to counter a bug

public:
	c_rand_lin() : _count(0)	{};
			UINT32	get_uint32();
	FINLINE	UINT32	get_count()		{	return _count;	}
};

class	c_rand_license final : public c_rand
{
public:
	c_rand_license()	{};
	UINT32	get_uint32();
};


//////
////	GAUSSIAN DONE WITH ADDITIONS
//
class	c_rand_gauss final : public c_rand
{
public:
	c_rand_gauss()	{};
	UINT32	get_uint32();
};

//////
////	GAUSSIAN WITH GAIN THEN BIAS
//
class	c_rand_gauss_slick final : public c_rand_lin
{
	REAL	_bias_factor;
	REAL	_gain_factor;
public:
	c_rand_gauss_slick( UINT32 seed_in = 8, REAL gain_in = .5, REAL bias_in = .5);

	void		set_bias( REAL	bias_in = REAL(.5) )	{ _bias_factor = bias_slick_factor(bias_in); };
	void		set_gain( REAL	gain_in = REAL(.5) )	{ _gain_factor = gain_slick_factor(gain_in); };
	REAL		get_fp32_01();
//	c_param_def*	set_focus();
};


//////
////	EXPERIMENTAL
//
class	c_rand_exp final : public c_rand_lin
{
	REAL	_bias;
	REAL	_gain;
	INT32	_type;
public:
	c_rand_exp();

	void		bias_set(REAL	bias_new) { _bias = bias_new; };
	void		gain_set(REAL	gain_new) { _gain = gain_new; };
	REAL		get_fp32_01();
//	c_param_def*	set_focus();
};

//////
////	MIN MAX and FLIP
//
class	c_rand_min final : public c_rand
{
public:
	c_rand_min()	{};
	UINT32	get_uint32()	{	return 0;	}
};

class	c_rand_max final : public c_rand
{
public:
	c_rand_max()	{};
	UINT32	get_uint32()	{	return aaa::BIGGEST<UINT32>;	}
};

class	c_rand_flip final : public c_rand
{
	bool	b_flip;
public:
	c_rand_flip()			{ b_flip = false; };
	UINT32	get_uint32()	{ b_flip = !b_flip; return b_flip ? 0 : aaa::BIGGEST<UINT32>; };
};

