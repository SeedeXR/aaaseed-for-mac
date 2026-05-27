
#ifdef AAA_DEF_REPULSOR_H
#error "DEF_REPULSOR_H included more than once."
#endif
#define AAA_DEF_REPULSOR_H 1


#ifndef AAA_DEFORMER_H
#	include "obj_ui/deformer/deformer.h"
#endif

class	c_def_repulsor final : public c_deformer
{
public:
	FACTORY_DECLARE(c_def_repulsor,c_deformer);
private:
	INT32	s_type;
	INT32	s_effect;

	REAL	spiral_freq;
	REAL	spiral_radius;
	REAL	spiral_inter;

	REAL	origin[3];
//	INT32	dim;
	INT32	i_axe;
	INT32	i_u;
	INT32	i_v;

	REAL	src_ext_ui[4];
	REAL	src_ext[3];
	REAL	src_int_ui[4];
	REAL	src_int[3];
	REAL	src_factor[3];

	REAL	dst_ext_ui[4];
	REAL	dst_ext[3];
	REAL	dst_int_ui[4];
	REAL	dst_int[3];
	REAL	dst_factor[3];

	REAL	bias;
	REAL	gain;

	REAL	bias_fac;
	REAL	gain_fac;

//	FINLINE	REAL	to_local( REAL in, REAL ori, REAL inte, REAL exte );
protected:
	FINLINE	void	apply_3d( REAL* dst, INT32 nb );
	FINLINE	void	apply_2d( REAL* dst, INT32 nb );
	FINLINE	void	apply_1d( REAL* dst, INT32 nb );

public:
	virtual	void	update() final override;
	virtual	void	apply( REAL CONST * src, REAL* dst, INT32 nb ) final override;
	virtual	void	param_init_pt() final override;

			void	init();
};

