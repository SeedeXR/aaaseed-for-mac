
#ifdef AAA_DEF_DATA_H
#error "DEF_DATA_H included more than once."
#endif
#define AAA_DEF_DATA_H 1


#ifndef AAA_DEFORMER_H
#	include "obj_ui/deformer/deformer.h"
#endif

class	c_def_data final : public c_deformer
{
public:
	FACTORY_DECLARE(c_def_data,c_deformer);
public:
	enum DATA_FILTER_TYPE : INT32
	{
		DATA_FILTER_DIRECT = 0,
		DATA_FILTER_ENVELOPE,
		DATA_FILTER_AVERAGE_2,
		DATA_FILTER_AVERAGE_3,
		DATA_FILTER_AVERAGE_4,
		DATA_FILTER_AVERAGE_5,
		DATA_FILTER_AVERAGE_6,
		DATA_FILTER_AVERAGE_7,
		DATA_FILTER_AVERAGE_8,
		DATA_FILTER_TYPE_MAX_NB
	};
	enum DATA_ABS_CLAMP_TYPE : INT32
	{
		DATA_ABS_CLAMP_NO = 0,
		DATA_ABS,
		DATA_CLAMP,
		DATA_ABS_CLAMP_MAX_NB
	};
	static INT32 CONST SAMPLE_NB_MAX = 44100 * 4;	// 4 secs at 44100
	//todo deal with it dynamicly
	static INT32 CONST BUFFER_NB_MAX = 128;

private:
	REAL				_origin[3];
	INT32				_src_axe;
	INT32				_dst_axe;

	REAL				_src_scale;

	REAL				_data_factor;
	REAL				_data_offset;
	DATA_ABS_CLAMP_TYPE	_s_data_abs_clamp;
	REAL				_abs_clamp_th;

	bool				_b_range;

	INT32				_sample_nb;
	INT32				_channel;

	INT32				_average_count;
	REAL				_data[2][BUFFER_NB_MAX][SAMPLE_NB_MAX];
	REAL				_result[2][SAMPLE_NB_MAX];

	INT32				_count_in;
	INT32				_skip;

//	bool				_b_clamp_0;
	bool				_b_force_0;

	INT32				s_type_visu_;
	DATA_FILTER_TYPE	_s_data_filter_type;
	REAL				_data_filter_value;

//	REAL				snd_exp_;
	REAL				_shape_gain;
	REAL				_shape_bias;
	INT32				_s_process_type;

	bool				_b_lissa;
	INT32				_lissa_channel;
	bool				_b_lissa_fake;
	INT32				_lissa_fake_offset;
	REAL				lissa_src_factor_;
	INT32				_s_data_src;

	INT32				count_a_last_;
	INT32				count_b_last_;
	INT32				interval_last_;

public:
	virtual	void	update() final override;
	virtual	void	apply( REAL CONST * src, REAL* dst, INT32 nb ) final override;
	virtual	void	param_init_pt() final override;

			void	init();
			void	update_channel(REAL* dst, INT32 index, INT32 ch );

			void	filter_envelope( REAL* pt, INT32 nb );
			void	filter_average( REAL* pt, INT32 nb, INT32 nb_ave );
			void	filter( REAL* pt, INT32 nb, DATA_FILTER_TYPE CONST type );
};

