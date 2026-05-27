
#ifdef AAA_DEF_BLOB_H
#error "DEF_BLOB_H included more than once."
#endif
#define AAA_DEF_BLOB_H 1


#ifndef AAA_DEFORMER_H
#	include "obj_ui/deformer/deformer.h"
#endif

class	c_def_blob final : public c_deformer
{
public:
	FACTORY_DECLARE(c_def_blob,c_deformer);
private:
	REAL	_bias_factor;
	REAL	_gain_factor;

	REAL	_origin_u;
	REAL	_origin_v;

//	REAL	speed_u;
//	REAL	speed_v;

	REAL	_radius;
	REAL	_strength;
	REAL	_freq_time;

	INT32	_src_axe;
	INT32	_dst_axe;
//	bool	b_abs;

//	REAL	min_u;
//	REAL	max_u;
//	REAL	min_v;
//	REAL	max_v;

	REAL	_bias;
	REAL	_gain;

	DOUBLE	_t;	//	time cached here
	REAL	_the_cos;
	REAL	_u;
	REAL	_v;
	INT32	_src_u;
	INT32	_src_v;
	REAL	_radius_squared;
	REAL	_radius_squared_over_1;

//	used to recompute the phase only when needed

public:

	virtual	void	update() final override;
	virtual	void	apply( REAL CONST * src, REAL* dst, INT32 nb ) final override;
	virtual	void	param_init_pt()  final override;
			void	init();
};

