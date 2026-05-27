
#ifdef AAA_IMG_INCRUST_H
#	error "IMG_INCRUST_H included more than once."
#endif
#define AAA_IMG_INCRUST_H 1


#ifndef AAA_OBJ_H
#	include		"infrastructure/obj/obj.h"
#endif

class c_incrust_process final : public c_obj
{
	static CONSTEXPR INT32 LUT_VALUE_NB = 256;

//	INT32	nb;
	FP32	_min;
	FP32	_max;
	FP32	_gain_factor	{-42.};
	FP32	_bias_factor	{-42.};
	bool	_b_inverse;

	alignas(32) UINT8	_lut[LUT_VALUE_NB];

public:

//	INT32	nb_ui;
	FP32	_min_ui;
	FP32	_max_ui;
	FP32	_gain_ui;
	FP32	_bias_ui;

	bool	_b_active_ui;
	bool	_b_inverse_ui;

	void update();
	UINT8 * get_lut()	{ return _lut;	}

};
