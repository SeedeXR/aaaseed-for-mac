
#ifdef AAA_FBX_SCENE_H
#error "FBX_SCENE_H included more than once."
#endif
#define AAA_FBX_SCENE_H 1


#if !defined(_MEMORY_)
#	include <memory>	//for unique_ptr
#endif
#ifndef _VECTOR_
#	include <vector>
#endif
#ifndef AAA_OBJ_H
#	include "infrastructure/obj/obj.h"
#endif
#if !defined(AAA_AAA_MATRIX_H)
#	include "math/aaa_matrix.h"
#endif
#if !defined(AAA_FBX_STRUCTS_H)
#	include "fbx/internal/fbx_structs.h"
#endif


namespace aaa
{
	class c_fbx_graph;
	class c_fbx_mesh;
	class c_fbx_animation_curve;
	class c_fbx_texture;
}
namespace gl
{
	class ubo;
}


//-----------------------------------------------------------------------------

namespace aaa {

	class c_fbx_scene final : public c_obj
	{
	public:
		typedef std::unique_ptr<aaa::c_fbx_graph>			pt_uni_graph;
		typedef std::unique_ptr<aaa::c_fbx_mesh>			pt_uni_mesh;
		typedef std::unique_ptr<aaa::c_fbx_animation_curve>	pt_uni_curve;
		typedef std::unique_ptr<aaa::c_fbx_texture>			pt_uni_texture;

	private:
		pt_uni_graph					_graph;
		std::vector<pt_uni_mesh>		_meshes;
		std::vector<pt_uni_curve>		_curves;
		std::vector<pt_uni_texture>		_textures;

		bool							_b_loaded;

		gl::ubo *						_ubo_matrix;
		glm::mat4						_matrix;
		//todo transfo in layer should do it
		// or we should use a transfo object here
		FP32							_translate[3];
		FP32							_rotate[3];		//we store in radians
		FP32							_scale[3];

		fbx_scene_load_options			_scene_load_options;
		fbx_scene_data_load_options		_scene_data_load_options;
		fbx_scene_load_result			_scene_result;
		fbx_scene_data					_scene_data;

		glm::mat4 CONST *				_root_armature_global_transform;

		INT32							_node_nb;
		INT32							_mesh_nb;
		INT32							_texture_nb;
		INT32							_stack_nb;
		INT32							_track_nb;
		INT32							_keyframe_nb;

		bool							_b_verbose;
		bool							_b_do_validate;
		bool							_b_use_ssbo_mesh;
		bool							_b_use_sdk_evaluator;
		bool							_b_apply_geometric_transform;
		bool							_b_use_absolute_path;

	public:
		c_fbx_scene();
		virtual ~c_fbx_scene();

		C_NO_CPY_MOVE(c_fbx_scene);

		private:
		void init_fbx_data();
		void release_fbx_data();
		void do_bind() CONST;
 
	public:
		void				set_use_sdk_evaluator(	bool CONST b )			{ _b_use_sdk_evaluator = b;		}
		FINLINE bool CONST  is_use_sdk_evaluator()	CONST					{ return _b_use_sdk_evaluator;	}

		void				set_apply_geometric_transform(	bool CONST b )	{ _b_apply_geometric_transform = b;	}
		FINLINE bool CONST  is_use_geometric_transform()  CONST				{ return _b_apply_geometric_transform;	}

		void				set_verbose(			bool CONST b )			{ _b_verbose = b;				}
		FINLINE bool CONST  is_verbose()			CONST					{ return _b_verbose;			}

		void				set_do_validation(		bool CONST b )			{ _b_do_validate = b;			}
		FINLINE bool CONST	is_do_validate()		CONST					{ return _b_do_validate;		}

		void				set_use_ssbo_mesh(		bool CONST b )			{ _b_use_ssbo_mesh = b;			}
		void				set_use_absolute_path(	bool CONST b )			{ _b_use_absolute_path = b;		}

		void set_animation_time_step(FP32 CONST val)	{ _scene_data_load_options.animation_time_step = val;	}

		void set_mesh_blend_shape_factors(UINT32 CONST mesh_index, UINT32 CONST shape_index, FP32 CONST * factors );

	public:
		bool load( C_PCHAR_C path );
		void clear();

	public:
		void update(
			FP32 CONST		p_time,
			UINT32 CONST		p_animation_index,
			bool CONST			p_linear,
			glm::mat4 CONST*	p_post_local_transforms,
			UINT32 CONST*		p_post_local_transforms_indices,
			UINT32 CONST		p_post_local_transforms_count);

		void draw_meshes() CONST;
		void draw_mesh(			INT32 CONST mesh_index ) CONST;

		void draw_meshes_static() CONST;
		void draw_mesh_static(	INT32 CONST mesh_index ) CONST;

		void draw_curves() CONST;
//todo revive
//		void draw_curve(		INT32 CONST curve_index ) CONST;

	public:
		void update_matrix( bool CONST b_update_ubo, bool CONST b_ubo_immediate );

	public:
		//FINLINE FP32 CONST *	get_scene_position() CONST { return _translation;	}
		//FINLINE FP32 CONST *	get_scene_rotation() CONST { return _rotation;		}
		//FINLINE FP32 CONST *	get_scene_scale()	 CONST { return _scale;			}

		//void store_translate(	FP32 CONST * CONST	tra		);
		//void set_translate(	glm::vec3 CONST &		tra,	bool CONST b_update_ubo, bool CONST b_update_immediate );
		//void set_translate(	FP32 CONST * CONST	tra,	bool CONST b_update_ubo, bool CONST b_update_immediate );
		//void set_translate_x(FP32 CONST tra, bool CONST b_update_ubo, bool CONST b_update_immediate);
		//void set_translate_y(FP32 CONST tra, bool CONST b_update_ubo, bool CONST b_update_immediate);
		//void set_translate_z(FP32 CONST tra, bool CONST b_update_ubo, bool CONST b_update_immediate);

		//void store_rotate(		FP32 CONST * CONST	rot_rad	);
		//void set_rotate(		glm::vec3 CONST &		rot_rad,	bool CONST b_update_ubo, bool CONST b_update_immediate );
		//void set_rotate(		FP32 CONST * CONST	rot_rad,	bool CONST b_update_ubo, bool CONST b_update_immediate );
		//void set_rotate_x(FP32 CONST rot, bool CONST b_update_ubo, bool CONST b_update_immediate);
		//void set_rotate_y(FP32 CONST rot, bool CONST b_update_ubo, bool CONST b_update_immediate);
		//void set_rotate_z(FP32 CONST rot, bool CONST b_update_ubo, bool CONST b_update_immediate);

		//void store_scale(		FP32 CONST * CONST	scale		);
		//void set_scale(			FP32 CONST * CONST	scale,		bool CONST b_update_ubo, bool CONST b_update_immediate );

		void set_trs(			FP32 CONST * CONST	pos,		FP32 CONST * CONST	rot_rad,	FP32 CONST * CONST scale,
								bool CONST b_update_ubo, bool CONST b_update_immediate
					);
		void set_trs(			glm::mat4 CONST & mat, bool CONST b_update_ubo, bool CONST b_update_immediate );

		void set_identity(		bool CONST b_update_ubo, bool CONST b_update_immediate );
		//---------------------------------------------------------------------

		FP32 get_animation_time_begin(				UINT32 CONST p_animation_index);
		FP32 get_animation_time_end(					UINT32 CONST p_animation_index);
		FP32 get_animation_node_time_begin(			UINT32 CONST p_animation_index);
		FP32 get_animation_node_time_end(			UINT32 CONST p_animation_index);
		FP32 get_animation_blend_shape_time_begin(	UINT32 CONST p_animation_index);
		FP32 get_animation_blend_shape_time_end(		UINT32 CONST p_animation_index);

		FP32 get_animation_play_time(				UINT32 CONST p_animation_index, float CONST p_time, animation_sampler_mode CONST p_sampler);
		char const* get_animation_name(					UINT32 CONST p_animation_index );

		glm::mat4 CONST *	get_node_global_transform( UINT32 CONST node_index ) CONST;
		bool				get_root_armature_global_position( FP32 * CONST pt_fp32 ) CONST;
		bool				get_root_armature_global_rotation( FP32 * CONST pt_fp32 ) CONST;

	public:
		FINLINE fbx_scene_data CONST *		get_fbx_scene_data() CONST				{ return &_scene_data;				}
		FINLINE fbx_scene_native CONST *	get_fbx_scene_native() CONST			{ return &(_scene_result.scene);	}
				c_fbx_mesh CONST *			get_mesh(	INT32 CONST index) CONST;
				c_fbx_texture CONST *		get_texture(INT32 CONST index) CONST;

		FINLINE bool CONST					is_loaded()			CONST				{ return _b_loaded;		}
																		
		FINLINE INT32 CONST					get_node_nb()		CONST				{ return _node_nb;		}
		FINLINE INT32 CONST					get_mesh_nb()		CONST				{ return _mesh_nb;		}
		FINLINE INT32 CONST					get_texture_nb()	CONST				{ return _texture_nb;	}
		FINLINE INT32 CONST					get_stack_nb()		CONST				{ return _stack_nb;		}
		FINLINE INT32 CONST					get_track_nb()		CONST				{ return _track_nb;		}
		FINLINE INT32 CONST					get_keyframe_nb()	CONST				{ return _keyframe_nb;	}
 
		FINLINE FP32 CONST				get_animation_time_step() CONST			{ return _scene_data_load_options.animation_time_step; }

		//---------------------------------------------------------------------

	}; // class c_fbx_scene

} // namespace aaa

//-----------------------------------------------------------------------------
