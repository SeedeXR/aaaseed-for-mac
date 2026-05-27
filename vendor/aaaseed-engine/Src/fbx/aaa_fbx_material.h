
#ifdef AAA_FBX_MATERIAL_H
#error "FBX_MATERIAL_H included more than once."
#endif
#define AAA_FBX_MATERIAL_H 1


#if !defined(AAA_AAA_TYPE_H)
#	include "code_utils/aaa_type.h"
#endif

#if !defined(AAA_V_BASE_H)
#	include "math/v_base.h"
#endif

#if !defined(AAA_FBX_STRUCTS_H)
#	include "fbx/internal/fbx_structs.h"
#endif

#if !defined(AAA_FBX_BASE_H)
#	include "fbx/aaa_fbx_base.h"
#endif


//-----------------------------------------------------------------------------

namespace aaa {
//todo 
	class c_fbx_material final : public aaa::c_fbx_base
	{
	private:
		st_fbx_material const *	_mat{ nullptr };
		CHAR CONST *			_name{ nullptr };

	public:
		explicit c_fbx_material( aaa::c_fbx_scene const * p_scene );
		~c_fbx_material() = default;

		C_NO_CPY_MOVE(c_fbx_material);

	public:
		void load(INT32 CONST p_node_idx);

	public:
		FINLINE bool CONST					is_valid() CONST	{ return _mat != nullptr; }
		FINLINE CHAR CONST *				get_name() CONST	{ return _name; }
		FINLINE fbx_material_type_e CONST	get_type() CONST	{ return _mat->_type; }

		FINLINE void						get_ambient_color(FP32 * p_out)	CONST	{ cpy_v3(p_out, _mat->ambient_color);		}
		FINLINE void						get_diffuse_color(FP32 * p_out)	CONST	{ cpy_v3(p_out, _mat->diffuse_color);		}
		FINLINE void						get_specular_color(FP32 * p_out)	CONST	{ cpy_v3(p_out, _mat->specular_color);		}
		FINLINE void						get_emissive_color(FP32 * p_out)	CONST	{ cpy_v3(p_out, _mat->emissive_color);		}

		FINLINE FP32 CONST * CONST		get_ambient_color()					CONST	{ return _mat->ambient_color;				}
		FINLINE FP32 CONST * CONST		get_diffuse_color()					CONST	{ return _mat->diffuse_color;				}
		FINLINE FP32 CONST * CONST		get_specular_color()				CONST	{ return _mat->specular_color;				}
		FINLINE FP32 CONST * CONST		get_emissive_color()				CONST	{ return _mat->emissive_color;				}

		FINLINE FP32 CONST				get_opacity_factor()				CONST	{ return _mat->opacity;						}
		FINLINE FP32 CONST				get_shininess_factor()				CONST	{ return _mat->shininess;					}
		FINLINE FP32 CONST				get_reflectiviy_factor()			CONST	{ return _mat->reflectiviy;					}

		FINLINE INT32 CONST					get_ambient_texture_index()			CONST	{ return _mat->ambient_texture_index;		}
		FINLINE INT32 CONST					get_diffuse_texture_index()			CONST	{ return _mat->diffuse_texture_index;		}
		FINLINE INT32 CONST					get_specular_texture_index()		CONST	{ return _mat->specular_texture_index;		}
		FINLINE INT32 CONST					get_normal_texture_index()			CONST	{ return _mat->normal_texture_index;		}
		FINLINE INT32 CONST					get_emissive_texture_index()		CONST	{ return _mat->emissive_texture_index;		}
		FINLINE INT32 CONST					get_shininess_texture_index()		CONST	{ return _mat->shininess_texture_index;		}
		FINLINE INT32 CONST					get_bump_texture_index()			CONST	{ return _mat->bump_texture_index;			}
		FINLINE INT32 CONST					get_specular_factor_texture_index() CONST	{ return _mat->specular_factor_texture_index;	}
		FINLINE INT32 CONST					get_opacity_texture_index()			CONST	{ return _mat->opacity_texture_index;		}

	}; // class c_fbx_material

} // namespace aaa

//-----------------------------------------------------------------------------
