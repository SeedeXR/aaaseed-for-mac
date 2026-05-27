
#ifdef AAA_FBX_MESH_H
#error "FBX_MESH_H included more than once."
#endif
#define AAA_FBX_MESH_H 1


#if !defined(_MEMORY_)
#	include <memory>	//for unique_ptr
#endif
#if !defined(AAA_AAA_TYPE_H)
#	include "code_utils/aaa_type.h"
#endif
#if !defined(AAA_FBX_STRUCTS_H)
#	include "fbx/internal/fbx_structs.h"
#endif
#if !defined(AAA_FBX_POSE_H)
#	include "fbx/aaa_fbx_pose.h"
#endif


//-----------------------------------------------------------------------------

namespace aaa {

	class c_fbx_material;

//todo should be better
//	CONSTEXPR INT32 MAX_BLEND_SHAPE_CHANNELS = 8;	//should be in a fbx space
#define MAX_BLEND_SHAPE_CHANNELS 8

	struct st_blend_shape_ubo_data
	{
		fbx_blend_shape_interpolation_info shapes[MAX_BLEND_SHAPE_CHANNELS];
		UINT32 shape_nb;
	};

	class c_fbx_mesh final : public aaa::c_fbx_base
	{

		//---------------------------------------------------------------------

	public:
		typedef std::unique_ptr<aaa::c_fbx_pose>		pt_uni_pose;
		typedef std::unique_ptr<aaa::c_fbx_material>	pt_uni_material;

		//---------------------------------------------------------------------

	private:
		st_fbx_mesh_gl			_mesh_gl;
		pt_uni_pose				_pose;
		pt_uni_material			_material;
		gl::ubo *				_ubo_model_matrix;
		gl::ubo *				_ubo_blend_shape;
//todo  remove or set an option to use it in shader
//		glm::mat4				_model_matrix{ aaa::matrix::identity };
		FP32					_bbox_min[3];
		FP32					_bbox_max[3];

		fbx_mesh_load_result	_mesh_load_result;
		fbx_mesh_load_options	_mesh_load_options;
		INT32					_mesh_index;
		CHAR CONST *			_name;

		bool					_use_ssbo;

		st_blend_shape_ubo_data	_blend_shape_ubo_data;

		//---------------------------------------------------------------------

	public:
		explicit c_fbx_mesh( aaa::c_fbx_scene const * p_scene );
		virtual ~c_fbx_mesh();

		C_NO_CPY_MOVE(c_fbx_mesh);

		//---------------------------------------------------------------------

	public:
		c_fbx_mesh & set_use_ssbo( bool const p_value );

		//---------------------------------------------------------------------

	private:
		void setup_mesh_gl();
		void setup_pose(INT32 CONST p_node_idx);
		void setup_material(INT32 CONST p_node_idx);
		void setup_size();

		//---------------------------------------------------------------------

	public:
		bool load(INT32 const p_mesh_index);
 //		void update(FP32 const p_time);
		void set_blend_shape_factors( UINT32 const shape_index, FP32 const * factors );
		void draw() const;
		void draw_static() const;

		//---------------------------------------------------------------------

	public:
		FINLINE CHAR CONST *			get_name()				CONST	{ return _name; }
			
		FINLINE INT32 CONST				get_index_nb()			CONST	{ return _mesh_load_result.mesh.index_count; }

		FINLINE INT32 CONST				get_mesh_index()		CONST	{ return _mesh_index; }
		FINLINE INT32 CONST				get_blend_shape_nb()	CONST	{ return _mesh_load_result.mesh.blend_shapes_data.shape_count; }
		INT32 CONST						get_blend_shape_channel_nb(UINT32 const shape_index) CONST;
		FINLINE bool  CONST				has_bones()				CONST	{ return _pose->has_bones(); }

		FINLINE FP32 CONST *			get_bbox_min()			CONST	{ return _bbox_min; }
		FINLINE FP32 CONST *			get_bbox_max()			CONST	{ return _bbox_max; }
		
		FINLINE c_fbx_pose CONST *		get_pose()				CONST	{ return _pose.get();		}
		FINLINE c_fbx_material CONST *	get_material()			CONST	{ return _material.get();	}

		//---------------------------------------------------------------------

	public:
		FINLINE INT32 CONST				get_vertex_nb()					CONST { return _mesh_load_result.mesh.vertex_count; }
				REAL *					get_vertex_positions()			CONST;
				REAL *					get_vertex_normals()			CONST;

		//---------------------------------------------------------------------

	}; // class c_fbx_mesh

} // namespace aaa

//-----------------------------------------------------------------------------
