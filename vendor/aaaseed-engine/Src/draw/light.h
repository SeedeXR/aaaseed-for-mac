
#ifdef AAA_LIGHT_H
#error "LIGHT_H included more than once."
#endif
#define AAA_LIGHT_H 1


#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif
#ifndef AAA_AAA_MATRIX_H
#	include "math/aaa_matrix.h"		//glm included here
#endif

CONSTEXPR	bool LIGHT_WORLD = false;
CONSTEXPR	bool LIGHT_CAMERA = true;


class	c_light final : public c_obj_active_ui
{
	FACTORY_DECLARE( c_light, c_obj_active_ui )

public:
	enum	LIGHT_TYPE : INT32
	{
		SUN = 0,
		BULB,
		SPOT,
		TYPE_MAX_NB
	};

	// AAA struct on cpu side no aligment copnstraints here
	struct st_light_modern
	{
		LIGHT_TYPE	_s_type;
		FP32		_position[4];	//ignore component 4 and reverse in case of sun

		FP32		_color[4];		//ignore alpha in ssbo but old pipe uses it
		FP32		_radius;
		FP32		_ao_factor;
		FP32		_gamma;

		bool		_b_shadow_cast;
		UINT32		_shadow_map_size[2];
		FP32		_shadow_filter_width;
		FP32		_shadow_bias;
		FP32		_shadow_bbox_min[3];
		FP32		_shadow_bbox_max[3];
		glm::mat4	_shadow_view_projection;

		bool		_b_intensity_map;			//	enable_intensity_map;
		INT32		_s_intensity_map;
		FP32		_intensity_map_factor;
		FP32		_intensity_map_scale[2];
		FP32		_intensity_map_offset[2];
	};

	struct st_point_light
	{
		FP32		_position[3];
		FP32		_radius;
		FP32		_color[3];
		FP32		_gamma;

//		FP32		_ao_factor;
	};

	struct st_directional_light
	{
		glm::mat4	_view_projection;			// from the camera used in the shadow pass

		FP32		_direction[3];
		FP32		_gamma;
//		FP32		_ao_factor;
		FP32		_color[3];
		INT32		_b_shadow;					// enable_shadows

		FP32		_shadow_map_size_over[2];
		FP32		_shadow_filter_width;
		FP32		_shadow_bias;

		INT32		_b_intensity_map;			// enable_intensity_map
		INT32		_intensity_map_mode;
		FP32		_intensity_map_factor;
		INT32		_dummy_;					// OpenGl need 16B padding on struct

		FP32		_intensity_map_scale[2];
		FP32		_intensity_map_offset[2];

//		FP32		_pad[12];
	};

	struct st_spot_light
	{
		glm::mat4	_view_projection;	// from the camera used in the shadow pass

		FP32		_position[3];
		FP32		_radius;

		FP32		_direction[3];
		FP32		_gamma;
//		FP32		_ao_factor;

		FP32		_field_of_view;
		FP32		_field_of_view_decay;
		FP32		_dummy_fov_[2];

		FP32		_color[3];
		INT32		_b_shadow;					//	enable_shadows;

		FP32		_shadow_map_size_over[2];
		FP32		_shadow_filter_width;
		FP32		_shadow_bias;

		INT32		_b_intensity_map;			//	enable_intensity_map;
		INT32		_intensity_map_mode;
		FP32		_intensity_map_factor;
		INT32		_dummy_;					//OpenGl need 16B padding on struct

		FP32		_intensity_map_scale[2];
		FP32		_intensity_map_offset[2];
	};


private:
	st_light_modern	_modern;

	FP32			_intensity;
	INT32			_index;
	INT32			_gl_name;
	FP32			_ambient_ui[5];
	FP32			_diffuse_ui[5];
	FP32			_specular_ui[5];

	FP32			_ambient[4];
	FP32			_specular[4];

	FP32			_spot_direction[3];
	FP32			_spot_exponent;
	FP32			_spot_cutoff;
	INT32			_attenuation_type;
	FP32			_attenuation_factor_ui[3];
	bool			_b_camera_lock;

//	glm::mat4		_mat_view_inverse_store;	//todo temp hack to draw light

protected:
	FINLINE void		set_pos_low( glm::mat4 CONST * CONST mat_view_inverse, FP32 CONST * CONST pos, FP32 CONST * CONST dir );
public:
	//helper
	static	c_light*	get_light( INT32 index );

	virtual	void		param_init_pt();

	virtual	bool		is_draw_sum_up()			{ return true; }
	virtual	void		draw_sum_up();

			void		set_index( INT32 index);
	FINLINE	INT32		get_index()	CONST			{ return _index; };
//	FINLINE	INT32		get_gl_name()				{ return _gl_name; };
	FINLINE	LIGHT_TYPE	get_type()	CONST			{ return _modern._s_type; };	//	old shading needs it

	//todo call it is or extend to other lock types
	FINLINE	bool		is_camera_lock()	CONST	{ return _b_camera_lock; };

	st_light_modern CONST *		get_modern()	CONST;

	virtual	void		update();
//	virtual	void		draw();
//			void		force_enable();
//			void		force_disable();
//	FINLINE	void		enable()			{	if( is_active() ) force_enable(); }
//	FINLINE	void		disable()			{	force_disable(); }
			
			void		set_pos(		glm::mat4 CONST * CONST mat_view_inverse );
			void		set_pos_psy(	glm::mat4 CONST * CONST mat_view_inverse );

			//void		set_ambient_4f(		FP32 r, FP32 g, FP32 b, FP32 a );
			//void		set_diffuse_4f(		FP32 r, FP32 g, FP32 b, FP32 a );
			//void		set_specular_4f(	FP32 r, FP32 g, FP32 b, FP32 a );
			//void		set_position_4f(	FP32 x, FP32 y, FP32 z, FP32 w );

			//void		set_spot_direction_3f(	FP32 CONST x, FP32 CONST y, FP32 CONST z );
			void		set_spot( FP32 CONST exponent, FP32 CONST cutoff, INT32 CONST atte_type, FP32 CONST atte_value );
			void		lock_to_camera();
			void		lock_to_world();

			///
			/// Copy light data to graphic buffer (SSBO in OpenGL).
			/// Buffer Map returns void pointer so handle with care.
			/// Returns copied data size.
			///
			INT32		set_light_directional(	st_directional_light * CONST	dst ) CONST NOEXCEPT;
			INT32		set_light_point(		st_point_light * CONST			dst ) CONST NOEXCEPT;
			INT32		set_light_spot (        st_spot_light * CONST			dst ) CONST NOEXCEPT;
			INT32		set_light_for_ssbo(		void * CONST					dst ) CONST NOEXCEPT;

//			void		build_cam_matrix_for_shadow( glm::mat4 * mat );
			void		set_matrix_view_projection( glm::mat4 CONST * CONST mat, glm::vec4 CONST * CONST position, glm::vec4 CONST * CONST direction );
};

