
#ifdef AAA_DEF_TURB_H
#error "DEF_TURB_H included more than once."
#endif
#define AAA_DEF_TURB_H 1


#ifndef AAA_DEFORMER_H
#	include "obj_ui/deformer/deformer.h"
#endif
#ifndef AAA_DELTA_T_H
#	include "time/delta_t.h"
#endif

class	c_def_turb final : public c_deformer
{
public:
	FACTORY_DECLARE( c_def_turb, c_deformer );
private:
	INT32		_fn_type;
	INT32		_harm_nb;
	INT32		_dst_axe;

	REAL		_space_factor[3];
	REAL		_space_period;
	REAL		_strenght;
	REAL		_dst_offset;
	REAL		_bias;
	REAL		_gain;
	REAL		_bias_factor;
	REAL		_gain_factor;

	REAL		_center[3];
//	REAL		phase_offset[3];
	INT32		_phase_index;
	REAL		_phase_speed;
	DOUBLE		_phase;
	c_delta_t	_delta_t;

	REAL		_freq_value;
//	used to recompute the phase only when needed

public:
	virtual	void	update() final override;
	virtual	void	apply( REAL CONST * src, REAL* dst, INT32 nb ) final override;
	virtual	void	param_init_pt() final override;

			void	init();
};

