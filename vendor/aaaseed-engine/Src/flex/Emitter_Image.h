#ifdef AAA_FLEX_EMITTER_IMAGE_H
#error "FLEX/EMITTER_IMAGE_H included more than once."
#endif
#define AAA_FLEX_EMITTER_IMAGE_H 1


#if !defined(AAA_GL_UBO_H)
#	include "gl/ubo.h"
#endif
#if !defined(AAA_GL_SSBO_H)
#	include "gl/ssbo.h"
#endif


/*
struct st_emit_data
{
	float _emit_size[3];
	UINT32 _image_w;
	float _emit_offset[3];
	UINT32 _image_h;
	float _emit_velocity[3];
	INT32 _emit_coverage_enable;
	float _emit_jitter[3];
	float _emit_coverage;
	float _emit_min_value[3];
	float _pad;
	float _emit_max_value[3];
	float _pad2;
};
*/

namespace GOL
{
	struct st_img_filter // image filter data (global)
	{
		UINT32	_image_sx;
		UINT32	_image_sy;
		INT32	_coverage_enable;
		FP32	_coverage;
		FP32	_color_min[4];
		FP32	_color_max[4];
		FP32	_color_selector[4];
	};

	struct st_emitter //per emitter data
	{
		FP32	_pos[3];
		UINT32	_rate;
		FP32	_size[4];	//2022 October _size[3] not used
		FP32	_vel[4];
		FP32	_jitter_pos[4];
		FP32	_jitter_vel[4];
	};

	CONSTEXPR INT32 EMITTER_NB_MAX = 2;
	struct st_emit
	{
		st_img_filter	filter;
		st_emitter		emitters[EMITTER_NB_MAX];
	};
}	//namespace GOL



class c_emitter_img final : public c_obj
{
private:
	GOL::st_emit	_emit;

	gl::ssbo*		_buffer_position;
	gl::ssbo*		_buffer_velocity;
	gl::ssbo*		_buffer_accumulation;

	gl::ubo*		_ubo_emit;

	bool			_b_valid;
	
	void release();

public:
	c_emitter_img();
	~c_emitter_img();

	void set_size( UINT32 CONST size_x, UINT32 CONST size_y );

	//takes points from image and filters them into a temp buffer
	void filter();

	//emits into the flex buffers, assumes they are bound for writing
	void emit( UINT32 CONST max_emit_nb, UINT32 CONST begin_slot_idx );

	UINT32 read_counter();

	GOL::st_emit* get_emit()
	{
		return &_emit;
	}
};