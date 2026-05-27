
#ifdef AAA_PLACER_H
#error "PLACER_H included more than once."
#endif
#define AAA_PLACER_H 1


#ifndef AAA_OBJ_H
#	include "infrastructure/obj/obj.h"
#endif
#ifndef	AAA_AAA_STR_H
#	include "aaa_str.h"
#endif
#ifndef AAA_AAA_UTIL_H
#	include "aaa_util.h"
#endif
#ifndef AAA_AAA_RECT_H
#	include "aaa/aaa_rect.h"
#endif

class	c_img_2d;
class	c_obj_ui;

class	c_placer final : public c_obj
{
public:
	static	CONST	INT32 PARAM_BASE_NB		= 2;
	static	CONST	INT32 PARAM_IMAGE_NB	= 15;
	static	CONST	INT32 PARAM_TURB_NB		= 3;
	static	CONST	INT32 PARAM_GROUP_NB	= 2;
	static	CONST	INT32 PARAM_ALL_NB		= PARAM_BASE_NB + PARAM_IMAGE_NB + PARAM_TURB_NB + PARAM_GROUP_NB;

	enum MASK_TYPE : INT32
	{
		TYPE_NO = 0,
		TYPE_RND,

		TYPE_RED,	TYPE_GREEN,		TYPE_BLUE,			TYPE_ALPHA,					TYPE_GREY,

		TYPE_TURB,					TYPE_FRACSUM,
		TYPE_TURB_IMPROVED,			TYPE_FRACSUM_IMPROVED,

		TYPE_LUA,

		TYPE_FIELD_TURB,			TYPE_FIELD_FRACSUM,
		TYPE_FIELD_TURB_IMPROVED,	TYPE_FIELD_FRACSUM_IMPROVED,

		TYPE_NB_MAX
	};

	static	C_PCHAR_C	str_type[TYPE_NB_MAX];

protected:
	MASK_TYPE	_s_how_ui;			//i
	o_str		_lua_fn;			//i
	bool		_b_use_min_max;		//i
//	bool		_b_compute_trig;	//i

	FP32		_min;
	FP32		_max;
	REAL		_mask_range_factor;
	bool		_b_outside_ui;		//i
	INT32		_try_max;			//i
	bool		_b_try_always;		//i

	REAL		_offset[3];
	REAL		_scale[3];

	FP32		_min_ui;			//i
	FP32		_max_ui;			//i
	REAL		_offset_ui[3];		//i
	REAL		_scale_ui[4];		//i


	INT32		_turb_harm_nb;		//i
	REAL		_turb_speed;		//i
	bool		_b_sphere;			//i

	INT32		_bind_ui;			//i
	c_img_2d*	_img;
	bool		_b_clamped_ui;			//i
	bool		_b_something;

	aaa::rect::lbrt_real	_rect;

	FINLINE	bool	is_val_break( FP32 val ) CONST;
public:
	c_placer();

	FINLINE	void	pick_point_in_cube(		float* vec ) CONST;
	FINLINE	void	pick_point_in_sphere(	float* vec ) CONST;
	FINLINE	void	build_point(			float* vec ) CONST;

	FINLINE	bool	place_no(		UINT32 nb, float* dst, UINT32 dst_stride  ) CONST;
	FINLINE	bool	place_random(	UINT32 nb, float* dst, UINT32 dst_stride  ) CONST;
	FINLINE	bool	place_lua(		UINT32 nb, float* dst, UINT32 dst_stride  ) CONST;
	FINLINE	bool	place_compo(	UINT32 nb, float* dst, UINT32 dst_stride  ) CONST;
	FINLINE	bool	compute_field(	UINT32 nb, float* dst, UINT32 dst_stride, float* src, UINT32 src_stride  ) CONST;
#if AAA_REAL_IS_DOUBLE()
	FINLINE	bool	compute_field(	UINT32 nb, float* dst, UINT32 dst_stride, REAL* src, UINT32 src_stride  ) CONST;
#endif
			bool	place_noise(	UINT32 nb, float* dst, UINT32 dst_stride  ) CONST;

			bool	place_one(		INT32 id,	float* vec ) CONST;
			bool	place(			UINT32 nb, float* dst, UINT32 dst_stride, float* src, UINT32 src_stride ) CONST;
#if AAA_REAL_IS_DOUBLE()
			bool	place_one(		INT32 id,	REAL* vec ) CONST;
			bool	place(			UINT32 nb, float* dst, UINT32 dst_stride, REAL* src, UINT32 src_stride ) CONST;
#endif				
			void	update();

			void	param_set( c_obj_ui* caller, INT32& h );
			void	build_comment( o_str& o );

	FINLINE	MASK_TYPE	get_type()			CONST	{ return _s_how_ui; }
	FINLINE	bool		is_input_needed()	CONST	{ return INSIDE_MIN_MAX( _s_how_ui, TYPE_FIELD_TURB, TYPE_FIELD_FRACSUM_IMPROVED ); }
//	FINLINE	bool		is_compute_trig()	CONST	{ return _b_compute_trig; }
//	FINLINE	void		clear_compute_trig()		{ _b_compute_trig = false; }
};

//	PARAM_DEF_BOOL_OFF(		name##_compute_trig		)	\

#define PARAM_PLACER( name )\
	PARAM_DEF_SYMBO_PSTR_ONE(	name##_type,			c_placer::str_type )	\
	PARAM_DEF_BOOL_OFF(			name##_sphere			)	\
	\
	PARAM_DEF_GROUP_CLOSED(	name##_Emission_Mask,	c_placer::PARAM_IMAGE_NB	)	\
		/* todo deal with the max limit	*/					\
		PARAM_DEF_BIND_2D_ALONE(	name##_bind			)	\
		PARAM_DEF_FP32_ONE_ZERO(	name##_min				)	\
		PARAM_DEF_FP32_ZERO_ONE(	name##_max				)	\
		PARAM_DEF_BOOL_OFF(			name##_use_min_max		)	\
		PARAM_DEF_BOOL_OFF(			name##_outside			)	\
		PARAM_DEF_INT32(			name##_try_max,			1000, 100,		1, 65536	)	\
		PARAM_DEF_BOOL_OFF(			name##_try_always		)	\
		PARAM_DEF_POINT_UVA(		name##_offset			)	\
		PARAM_DEF_SCALE_UVAF(		name##_scale			)	\
		PARAM_DEF_BOOL_OFF(			name##_uv_clamped		)	\
	\
	PARAM_DEF_GROUP_CLOSED(	name##_Turbulence, c_placer::PARAM_TURB_NB )	\
		PARAM_DEF_INT32(			name##_harmonique_nb,	2, 1,	1, 32 )		\
		PARAM_DEF_REAL_ZERO(		name##_phase_speed		)	\
		PARAM_DEF_REF(				name##_lua_fn			)	\

//	PARAM_DEF_GAIN(	name##"_gain"	)
//	PARAM_DEF_BIAS(	name##"_bias"	)
//	{	nullptr,	PARAM_REAL,	name##"_phase", 1., 0.,	PARAM_MIN_REAL, PARAM_MAX_REAL, nullptr, nullptr },
//	PARAM_DEF_AXE_X( name##"_phase_axe" )

