
#ifdef AAA_FBX_UTILS_H
#error "FBX_UTILS_H included more than once."
#endif
#define AAA_FBX_UTILS_H 1


#if !defined(AAA_FBX_SDK_INCLUDE_H)
#	include "fbx/internal/fbx_sdk_include.h"
#endif
#if !defined(AAA_FBX_STRUCTS_H)
#	include "fbx/internal/fbx_structs.h"
#endif

#if !defined(AAA_FBX_STRUCTS_INTERNAL_H)
#	include "fbx/internal/fbx_structs_internal.h"
#endif

#define AAA_USE_VECTOR() 1
#if AAA_USE_VECTOR()
#	ifndef AAA_V_H
#		include "math/v.h"
#	endif
#endif

/*! \brief tells if we need to expand face index buffer, depending on load options. This returns true if at least one optional attribute has a per polygon frequency */
bool is_face_expand_required(
	FbxMesh *						p_mesh, 
	fbx_mesh_load_options const *	p_options );

/*! \brief computes the number of vertices in case we need to expand the positions per polygon */
INT32 compute_vertex_count_expanded(
	FbxMesh *						p_mesh );

/*! \brief returns the number of indices for our geometry */
INT32 compute_index_count(
	FbxMesh *						p_mesh );

/*! \brief triangulates a mesh, writes data at dest_data_index_ptr (assumes this is allocated and has sufficient capacity  */
void triangulate(
	FbxMesh *						p_mesh, 
	INT32 *							p_dst_data_index_ptr, 
	fbx_mesh_load_options const &	p_load_options );

/*! \brief triangulates a mesh, and expands the indices, writes data at dest_data_index_ptr (assumes this is allocated and has sufficient capacity  */
void triangulate_expand(
	FbxMesh *						p_mesh, 
	INT32 *							p_dst_data_index_ptr, 
	fbx_mesh_load_options const &	p_load_options );

/*! \brief get internal info about the scene, this is used in order to calculate memory requirements for data tables */
fbx_scene_info_internal scene_info_internal(
	FbxScene *						p_scene,
	fbx_scene_info *				p_scene_info );

/*! \brief copies node data into internal data structure */
void scene_write_data(
	FbxScene *						p_scene, 
	fbx_scene_data const &			p_scene_result );

/*! \brief retrieves scene info */
fbx_scene_info scene_get_info(
	FbxScene *						p_scene );

/*! \brief count bones for a mesh */
INT32 mesh_count_bones(
	FbxMesh *						p_mesh );

/*! \brief load scen materials */
void load_scene_materials(
	FbxScene *						p_scene_native, 
	fbx_scene_data *				p_scene );

/*! \brief compute bone information */
fbx_mesh_skinning_data_info mesh_compute_bones_info(
	FbxMesh *							p_mesh, 
	fbx_mesh_bones_import_mode const	p_import_mode );

/*! \brief load bones information, please note that result is allocated inside the function, and should be freed by the caller */
void mesh_load_bones(
	FbxMesh *							p_mesh, 
	fbx_mesh_bones_import_mode const	p_import_mode, 
	fbx_scene_data const &				p_scene, 
	fbx_mesh_skinning_data_info const &	p_skinning_info, 
	fbx_mesh_skinning_data &			p_result );

INT32 get_animation_step_count(
	const fbx_animation_info*			animation_info,
	float const							time_step );

/*! \brief load blend shapes informations */
void mesh_load_blend_shapes(FbxMesh* p_mesh_src, fbx_mesh_data* p_mesh_dst);

FINLINE void clear_v3fp32( float * const dst )
{
#if AAA_USE_VECTOR()
	clear_v3( dst );
#else
	*dst		=  0.0f;
	*(dst+1)	=  0.0f;
	*(dst+2)	=  0.0f;
#endif
}
FINLINE void set1_v3fp32( float * const dst )
{
#if AAA_USE_VECTOR()
	set_v3( dst, 1. );
#else
	*dst		=  1.0f;
	*(dst+1)	=  1.0f;
	*(dst+2)	=  1.0f;
#endif
}
/*! \brief writes a double 4 into a vec3 pointer */
FINLINE void cpy_v3fp32( float * const dst, float const * const src )
{
#if AAA_USE_VECTOR()
	cpy_v3( dst, src );
#else
	*dst		=  *src;
	*(dst+1)	=  *(src+1);
	*(dst+2)	=  *(src+2);
#endif
}
FINLINE void cpy_v3_fbx_double_to_fp32( float * const dst, FbxDouble3 const & src )
{
	double const * const pd = src.Buffer();
#if AAA_USE_VECTOR()
	cpy_v3( dst, pd );
#else
	*dst		= (float)*pd;
	*(dst+1)	= (float)*(pd+1);
	*(dst+2)	= (float)*(pd+2);
#endif
}
FINLINE void scale_v3_fbx_double_to_fp32( float * const dst, FbxDouble3 const & src, float const factor )
{
	double const * const pd = src.Buffer();
#if AAA_USE_VECTOR()
	scale_v3( dst, pd, factor );
#else
	*dst		= (float)*pd		* factor;
	*(dst+1)	= (float)*(pd+1)	* factor;
	*(dst+2)	= (float)*(pd+2)	* factor;
#endif
}
FINLINE void write_double4_to_vec3( float* const dst, FbxDouble4 const & src )
{
	cpy_v3_fbx_double_to_fp32( dst, src );
}
FINLINE void write_double4_to_vec3( float* const dst, FbxDouble4 const & src, const float factor )
{
	scale_v3_fbx_double_to_fp32( dst, src, factor );
}
FINLINE void write_double2_to_vec2( float* const dst, FbxDouble2 const & src, bool const b_flip_y )
{
	double const * const pd = src.Buffer();
	*dst = (float)*pd;
	if( b_flip_y )
		*(dst+1) = 1.0f - (float)*(pd+1);
	else
		*(dst+1) = (float)*(pd+1);
}

///*! \brief writes a double 2 into a vec2 pointer */
//void write_double2_to_vec2(
//	float *					p_dest_ptr, 
//	FbxDouble2 const &		p_src_data, 
//	bool const				p_flip_y );

/*! \brief writes a double 4 element channel into a vec3 pointer */
void write_double4_elements_to_vec3_array(
	FbxMesh *									p_mesh,
	float *										p_dst_ptr, 
	FbxLayerElementTemplate<FbxVector4> const *	p_src_data, 
	UINT32 const								p_vertex_count );

/*! \brief writes a double 3 element channel into a vec2 pointer, allow to flip y, since this is generally used by uv */
void write_double2_elements_to_vec2_array(
	FbxMesh *									p_mesh,
	float *										p_dst_ptr, 
	FbxLayerElementTemplate<FbxVector2> const *	p_src_data, 
	UINT32 const								p_vertex_count, 
	bool const									p_flip_y );


/*! \brief write bone data, note that p_polygon_vertex_indices can be null, in case we just use id */
void write_bone_data(
	fbx_mesh_data &						p_mesh_result, 
	fbx_mesh_bones_import_mode const	p_import_mode,
	fbx_mesh_skinning_data_info const &	p_bone_data_info,
	fbx_mesh_skinning_data const &		p_skinning_data,
	INT32 const							p_control_point_count,
	INT32 *								p_polygon_vertex_indices );
