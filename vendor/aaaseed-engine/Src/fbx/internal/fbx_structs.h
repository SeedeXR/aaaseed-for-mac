
#ifdef AAA_FBX_STRUCTS_H
#error "FBX_STRUCTS_H included more than once."
#endif
#define AAA_FBX_STRUCTS_H 1


#if !defined(AAA_AAA_TYPE_H)
#	include "code_utils/aaa_type.h"
#endif


#define FBX_NAME_MAX_LENGTH				256
#define FBX_BONE_PER_VERTEX_LIMIT_FOUR	4
#define FBX_FILE_NAME_MAX_LENGTH		1024

////////// Result Codes

/*! \brief Result code sent when we are loading a scene */
enum class fbx_scene_load_result_code
{
	/*! \brief Scene was loaded successfully */
	success = 0,
	/*! \brief fbx sdk manager failed to load */
	fail_manager_create = -1,
	/*! \brief plugins were not loaded */
	fail_load_plugins = -2,
	/*! \brief Importer could not be created */
	fail_importer_create = -3,
	/*! \brief Scene could not be loaded */
	fail_scene_load = -4,
	/*! \brief Scene result just got initialized */
	fail_init = -5
};

/*! \brief Result code trying to load a mesh */
enum class fbx_mesh_load_result_code
{
	/*! \brief Mesh was loaded successfully */
	success = 0,
	/*! \brief Scene is invalid */
	fail_invalid_scene = -1,
	/*! \brief Index is out of bounds */
	fail_invalid_index = -2,
	/*! \brief No normals were found in the mesh */
	fail_normals_not_present = -3,
	/*! \brief No tangent frame was found in the mesh */
	fail_tangent_frame_not_present = -4,
	/*! \brief No texture coordinates were found in the mesh */
	fail_texture_coordinates_not_present = -5,
	/*! \brief No bones were found in the mesh */
	fail_bones_not_present = -6,
	/*! \brief No normals were found in the mesh, and normals generation failed */
	fail_normals_not_generated = -7,
	/*! \brief No tangents were found in the mesh, and normals generation failed */
	fail_tangents_not_generated = -8
};


////////// Enumerations

/*! \brief Mesh vertex layout */
enum class fbx_mesh_layout
{
	/*! \brief Vertex is packed as a single structure in an array */
	packed,
	/*! \brief Each attribute is packed in its own array */
	array_per_attribute
};

/*! \brief Mesh attribute options, this is used for optional attributes, to indicate if we want to load those or nor */
enum class fbx_mesh_attribute_options
{
	/*! \brief Load attributes if they are present in the mesh */
	enable_if_present,
	/*! \brief Generate attribute if not present in the scene */
	generate_if_missing,
	/*! \brief Do not load this attribute */
	ignore
};

/*! \brief Gives info to the caller, tells if an otpional attribute has been loaded from the file or auto generated */
enum class fbx_mesh_optional_attribute_load_result
{
	/*! \brief Attribute was in data */
	success_data_present = 0,
	/*! \brief Attribute was auto generated */
	success_generated = 1,
	/*! \brief Attribute was set to be ignored */
	success_ignored = 2,
	/*! \brief Attribute was not generated, generate function failed */
	fail_generate_generic = -1,
	/*! \brief Specific to tangent, attribute was not generated because uv set is missing */
	fail_generate_tangent_no_texcoord = -2
};

/*! \brief Import mode for mesh bones */
enum class fbx_mesh_bones_import_mode
{
	/*! \brief Takes the highest influence bones */
	limit_four_highest_influence = 0,
	/*! \brief Takes the highest influence bones and normalizes them */
	limit_four_highest_influence_normalized = 1,
	/*! \brief Takes all the bones and put them in a flat table size (max_bones * vertex count) */
	all_bones_flat = 2,
	/*! \brief Takes all the bones and put them in a compacted table */
	all_bones_compact = 3,
};

/*! \brief Tells if an attribute is required or optional*/
enum class fbx_attribute_requirement
{
	/*! \brief attribute is optional, loader with still return if absent or could not be generated */
	optional,
	/*! \brief attribute is required, loader will fail if attribute is nnot here or not generated */
	required,
};

/*! \brief fbx axis */
enum class fbx_axis
{
	/*! \brief x */
	x,
	/*! \brief y */
	y,
	/*! \brief z */
	z
};

/*! \brief fbx rotation order. Note that default is xyz, but if we perform deep conversion, this order can be modified */
enum class fbx_rotation_order
{
	XYZ,
	XZY,
	YZX,
	YXZ,
	ZXY,
	ZYX,
	SphericalXYZ
};

/*! \brief fbx axis direction */
enum class fbx_axis_direction
{
	positive = 1,
	negative = -1
};


/*! \brief This is a kind of animation curve for a node*/
enum class fbx_curve_attribute_kind
{
	/*! \brief position (x axis) */
	position_x,
	/*! \brief position (y axis) */
	position_y,
	/*! \brief position (z axis) */
	position_z,
	/*! \brief scale (x axis) */
	scale_x,
	/*! \brief scale (y axis) */
	scale_y,
	/*! \brief scale (z axis) */
	scale_z,
	/*! \brief rotation (x axis) */
	rotation_x,
	/*! \brief rotation (y axis) */
	rotation_y,
	/*! \brief rotation (z axis) */
	rotation_z,
};

/*! \brief Interpolation mode for a keyframe */
enum class fbx_animation_import_mode
{
	/*! \brief imports keyframes as they are stored */
	keyframes_default,
	/*! \brief flattens keyframes to a target framerate */
	keyframes_fixed_timestep,
};

/*! \brief Interpolation mode for a keyframe */
enum class fbx_keyframe_interpolation
{
	/*! \brief linear Interpolation */
	linear,
	/*! \brief no Interpolation */
	no_interpolation,
	/*! \brief cubic Interpolation */
	cubic
};

/*! \brief Indicates which mode we prefer in order to import an attribute that can appear several time (uv/vertex colors) */
enum class fbx_multiple_attribute_import_mode
{
	/*! \brief do not import */
	none,
	/*! \brief import first attribute only */
	first,
	/*! \brief import all attributes */
	all
};

/*! \brief Indicates how we play an animation */
enum class animation_sampler_mode
{
	/*! \brief we clamp the animation, we play only once and stay ast first/last frame if out of bounds */
	clamp,
	/*! \brief we loop the animation */
	wrap
};

enum class fbx_file_mode
{
	absolute,
	relative
};

enum class fbx_unit
{
	centimeters,
	meters
};

enum class fbx_target_coord_system
{
	opengl,
	directx,
	gol
};

enum class fbx_target_coord_system_convert_mode
{
	none,
	simple,
	deep
};

/*! \brief A bounding box structure */
struct fbx_bounding_box
{
	/*! \brief Minimum */
	float min[3];
	/*! \brief Maximum X */
	float max[3];
};


////////// Load options

/*! \brief Additional options when we want to load a scene */
struct fbx_scene_load_options
{
	/*! \brief Load plugins from application directory */
	bool load_plugins;
};


/*! \brief Options to load a mesh */
struct fbx_mesh_load_options
{
	/*! \brief Preferred vertex layout */
	fbx_mesh_layout data_layout;
	/*! \brief Chooses how we want to load normals */
	fbx_mesh_attribute_options normals;
	/*! \brief Tells if we want normals as required or optional */
	fbx_attribute_requirement normals_requirement;
	/*! \brief Chooses how we want to load tangent frame, note that generate option wil only work if at least one set of uv is present */
	fbx_mesh_attribute_options tangent_frame;
	/*! \brief Tells if we want normals as required or optional */
	fbx_attribute_requirement tangent_frame_requirement;
	/*! \brief Tells if we want to load texture coordinates. This is an all or nothing option in case of multiple sets */
	fbx_multiple_attribute_import_mode texture_coordinates;
	/*! \brief Tells if we want normals as required or optional */
	fbx_attribute_requirement texture_coordinates_requirement;

	/*! \brief Allows to scale the mesh */
	float scaling;

	/*! \brief Reverses triangle winding */
	INT32 reverse_winding;
	/*! \brief Flips y axis for texture coordinates */
	INT32 flip_texture_coordinates_y;

	/*! \brief bones import mode */
	fbx_mesh_bones_import_mode bones_import_mode;
	/*! \brief bones */
	INT32 enable_bones;
};

/*! \brief Options to load a scene */
struct fbx_scene_data_load_options
{
	/*! \brief animation import mode (default or fixed timestep) */
	fbx_animation_import_mode animation_mode;
	/*! \brief if animation is in fixed time step mode, use this, otherwise this is ignored */
	float animation_time_step;
	/*! \brief converts scene to target unit */
	INT32 convert_to_target_unit;
	/*! \brief target unit */
	fbx_unit target_unit;
	/*! \brief coordinate syste, conversion mode */
	fbx_target_coord_system_convert_mode coord_system_convert_mode;
	/*! \brief target coordinate system */
	fbx_target_coord_system target_coord_system;
	/*! \brief tells if we load fil path in relative or absolute */
	fbx_file_mode file_texture_load_mode;
};

////////// General data

/*! \brief scene animation info */
struct fbx_animation_info
{
	/*! \brief location of ths first track in the track table */
	INT32 first_track_location;
	/*! \brief number of tracks for this animation */
	INT32 track_count;
	/*! \brief location of the first keyframe in the table */
	INT32 first_keyframe_location;
	/*! \brief number of keyframes for this animations */
	INT32 keyframe_count;
	/*! \brief number of animation layers */
	INT32 layer_count;
	/*! \brief start time for the animation */
	float start_time;
	/*! \brief end time for the animation */
	float end_time;
	/*! \brief start time for the animation for nodes */
	float node_start_time;
	/*! \brief end time for the animation for nodes */
	float node_end_time;
	/*! \brief start time for the animation for nodes */
	float blend_shape_start_time;
	/*! \brief end time for the animation for nodes */
	float blend_shape_end_time;
};

/*! \brief Scene information */
struct fbx_scene_info
{
	/*! \brief Number of meshes in the scene */
	UINT32				mesh_count;
	/*! \brief Total number of materials in the scene */
	UINT32				material_count;
	/*! \brief Total number of file textures in the scene */
	UINT32				file_texture_count;
	/*! \brief Number of animation stacks in the scene */
	UINT32				animation_stack_nb;
	/*! \brief Total number of nodes in the scene */
	UINT32				total_node_count;
	/*! \brief Total number of animation tracks in the scene */
	UINT32				animation_track_count;
	/*! \brief Total number of animation keyframes in the scene */
	UINT32				animation_keyframe_total_count;
	/*! \brief Number of meshes blend shape animations in the scene */
	UINT32				mesh_blend_shape_animations_count;
	/*! \brief up axis vector */
	fbx_axis			up_axis_vector;
	/*! \brief up axis vector direction */
	fbx_axis_direction	up_axis_vector_direction;

	/*! \brief front axis vector */
	fbx_axis			front_axis_vector;
	/*! \brief front axis vector direction */
	fbx_axis_direction	front_axis_vector_direction;

	/*! \brief right axis vector */
	fbx_axis			right_axis_vector;
	/*! \brief right axis vector direction */
	fbx_axis_direction	right_axis_vector_direction;

	/*! \brief animation info */
	fbx_animation_info*	animations;
};

/*! \brief Scene data, contains pointer to access scene and release memory,opaque, do not use directly */
struct fbx_scene_native
{
	void* context_pointer;
	void* scene_pointer;
	fbx_scene_info scene_info;
};

/*! \brief this is the type of material assigned to a suurface */
enum class fbx_material_type_e
{
	/*! \brief phong surface */
	phong_surface,
	/*! \brief lambert (diffuse only) */
	lambert_surface,
	/*! \brief unknown */
	unknown
};

/*! \brief represents a fbx material */
typedef struct st_fbx_material
{
	/*! \brief material kind */
	fbx_material_type_e	_type;

	/*! \brief ambient color */
	float	ambient_color[3];
	/*! \brief diffuse color */
	float	diffuse_color[3];
	/*! \brief specular color */
	float	specular_color[3];
	/*! \brief emissive color */
	float	emissive_color[3];

	/*! \brief opacity */
	float	opacity;
	/*! \brief shininess */
	float	shininess;
	/*! \brief reflectivity */
	float	reflectiviy;

	/*! \brief index of ambient texture, or -1 if no texture associated */
	INT32	ambient_texture_index;
	/*! \brief index of diffuse texture, or -1 if no texture associated */
	INT32	diffuse_texture_index;
	/*! \brief index of specular texture, or -1 if no texture associated */
	INT32	specular_texture_index;
	/*! \brief index of normal map texture, or -1 if no texture associated */
	INT32	normal_texture_index;
	/*! \brief index of emissive texture, or -1 if no texture associated */
	INT32	emissive_texture_index;
	/*! \brief index of shininess texture, or -1 if no texture associated */
	INT32	shininess_texture_index;
	/*! \brief index of specular factor texture, or -1 if no texture associated */
	INT32	specular_factor_texture_index;
	/*! \brief index of bump map texture, or -1 if no texture associated */
	INT32	bump_texture_index;
	/*! \brief index of transparent map texture, or -1 if no texture associated */
	INT32	opacity_texture_index;
} st_fbx_material;


/*! \brief fbx mesh bones vertex data structure */
struct fbx_mesh_bones_vertex_data
{
	/*! \brief Bone count per vertex data pointer (stride 4), can be NULL */
	void* bone_count_ptr;

	/*! \brief Start location of bone indices and weights data pointer (stride 4), can be NULL */
	void* bone_start_location_ptr;

	/*! \brief Bone index data pointer can be NULL */
	void* bone_index_ptr;

	/*! \brief Bone weights data pointer, can be NULL */
	void* bone_weight_ptr;
};

/*! \brief If a mesh is loaded using fbx_mesh_layout::array_per_attribute, this contains the location of each attribute buffer */
struct fbx_mesh_array_location_info
{
	/*! \brief Position pointer (stride 12) */
	void* position_ptr;
	/*! \brief Normals pointer (stride 12), can be NULL */
	void* normal_ptr;
	/*! \brief Normals pointer (stride 8), can be NULL */
	void* texture_coordinates_ptr;
	/*! \brief Tangents pointer (stride 12), can be NULL */
	void* tangent_ptr;
	/*! \brief Bi-Tangents pointer (stride 12), can be NULL */
	void* bitangent_ptr;

	/*! \brief bone related data */
	fbx_mesh_bones_vertex_data vertex_bones_data;
};

/*! \brief fbx mesh bones data structure */
struct fbx_mesh_bones_data
{
	/*! \ number of bones for this mesh (or 0) if no bones */
	UINT32 bones_nb;

	/*! \ bind pose location, or NULL if no skinning is present */
	void* bones_bind_pose_ptr;

	/*! \ bone index location for each bone or NULL if no skinning is present */
	void* bones_node_index_table_ptr;
};

/*! \brief fbx blend shape target */
struct fbx_blend_shape_target_data
{
	/*! \brief weight, normalized between 0 and 1, this is the location of the target inside the morph */
	float weight;
	/*! \brief location in the buffer, since data is per vertex this is uniform */
	UINT32 buffer_start_location;
};

/*! \brief fbx blend shape channel, a channel can contain several targets (similar to animation keyframes) */
struct fbx_blend_shape_channel_data
{
	/*! \brief amount of targets */
	UINT32 target_count;
	/*! \brief targets */
	UINT32 targets_start_index;
	/*! \brief curve index for this blend shape */
	INT32 curve_index;
};

/*! \brief fbx blend shape */
struct fbx_blend_shape_data
{
	/*! \brief amount of channels */
	UINT32 channel_count;
	/*! \brief fbx channels */
	UINT32 channels_start_index;
};

/*! \brief fbx blend shape deformer data structure */
struct fbx_mesh_blend_shape_data
{
	/*! \brief amount of shapes */
	UINT32 shape_count;
	/*! \brief size of buffers */
	UINT32 buffer_element_count;
	/*! \brief position data pointer (total amount of targets * vec3) */
	void* position_data_ptr{ nullptr };
	/*! \brief normals data pointer (total amount of targets * vec3) */
	void* normal_data_ptr{ nullptr };
	
	/*! \brief blend shapes info */
	fbx_blend_shape_data* shapes_info_ptr{ nullptr };
	/*! \brief blend shapes info */
	fbx_blend_shape_channel_data* shapes_channel_info_ptr{ nullptr };
	/*! \brief blend shapes target info */
	fbx_blend_shape_target_data* shapes_target_ptr{ nullptr };
	
	/*! \brief tells if we have normals or not */
	bool has_normals;
};

/*! \brief fbx blend shape deformer, interpolation info, we get this information by querying 
a channel, it gives the buffer location for vertices and the interpolation amount to be used
*/
struct fbx_blend_shape_interpolation_info
{
	/*! \brief this is the source start location in the data buffer */
	UINT32 source_vertex_location;
	/*! \brief this is the target start location in the data buffer */
	UINT32 target_vertex_location;
	/*! \brief this is the interpolation amount */
	float interpolation_amount;
	/*! \brief padding so we keep struct aligned to 16 bytes */
	float __dummy;
};

/*! \brief fbx mesh data structure */
struct fbx_mesh_data
{
	/*! \brief fbx mesh data layout */
	fbx_mesh_layout layout;
	/*! \brief mesh uses a single memory space, this is the global one. If layout is packed, this is the first vertex location */
	void* data_ptr{nullptr};
	/*! \ data size */
	UINT32 data_length;

	/*! \ vertex count */
	UINT32 vertex_count;

	/*! \ index count, this can be 0 if no index buffer is present */
	UINT32 index_count;

	/*! \ vertex weights count (used for the vertex weights array size */
	UINT32 vertex_weights_count;

	/*! \ number of texture coordinates channels */
	UINT32 texture_coordinates_channel_count;

	/*! \ bounding box (in object space) for the mesh, this does not take deformers into account. */
	fbx_bounding_box bounding_box;

	/*! \ attributes locations, if layout is array_per_attribute, can be ignored otherwise */
	fbx_mesh_array_location_info array_location_info;

	/*! \ index buffer location, or NULL if not index buffer is present */
	void* index_buffer_ptr;

	/*! \ skinning related data */
	fbx_mesh_bones_data bones_data;

	/*! \ blend shapes */
	fbx_mesh_blend_shape_data blend_shapes_data;

	/*! \ tells if our mesh has normals */
	bool has_normals;

	/*! \ tells if our mesh has tangents and bitangents */
	bool has_tangent_frame;

	/*! \ tells if our mesh has tangents and bitangents */
	bool has_bones;
};

/*! \brief a mesh blend shape animation */
struct fbx_mesh_blend_shape_animation
{
	/*! \brief if node has animated properies, this is the location of the first curve in the animation table, or -1 if node has no animations */
	INT32 animation_curve_index;
	/*! \brief this is the mesh index */
	UINT32 mesh_index;
	/*! \brief this is animation stack index */
	UINT32 animation_index;
		/*! \brief blend shape index */
	UINT32 blend_shape_index;
	/*! \brief blend shape channel index */
	UINT32 blend_shape_channel_index;
};

/*! \brief a node animation info in the node hierarchy */
struct fbx_node_animation
{
	/*! \brief if node has animated properties, this is the location of the first curve in the animation table, or -1 if node has no animations */
	INT32 animation_table_location;

	/*! \brief number of animated properties for this node */
	UINT32 animation_count;
};

/*! \brief a node in the scene hierarchy, we use a single array of nodes so all references are indices in the array */
struct fbx_node
{
	/*! \brief parent index in the array, of -1 if this is the root node */
	INT32 parent_index;

	/*! \brief node mesh index, -1 if no mesh present */
	INT32 mesh_index;

	/*! \brief node material count */
	UINT32 material_count;

	/*! \brief node material index */
	INT32 first_material_index;

	/*! \brief first child index in the array, of -1 if this is a leaf node */
	INT32 child_first_index_location;

	/*! \brief number of child nodes */
	UINT32 child_count;

	/*! \brief tells if node is root of armature */
	bool is_armature;

	/*! \brief node position */
	float position[3];

	/*! \brief node scaling */
	float scaling[3];

	/*! \brief node rotation */
	float rotation[3];

	/*! \brief node rotation offset */
	float rotation_offset[3];

	/*! \brief node rotation pivote */
	float rotation_pivot[3];

	/*! \brief node pre rotation */
	float pre_rotation[3];

	/*! \brief node pre rotation */
	float post_rotation[3];

	/*! \brief node scaling offset */
	float scaling_offset[3];

	/*! \brief node scaling pivot*/
	float scaling_pivot[3];

	/*! \brief geometric trnalstion, applied in global space after traversal */
	float geometric_translation[3];

	/*! \brief geometric scaling , applied in global space after traversal */
	float geometric_scaling[3];

	/*! \brief geometric rotation , applied in global space after traversal */
	float geometric_rotation[3];

	/*! \brief order of rotations */
	fbx_rotation_order rotation_order;
};

/*! \brief animation curve */
struct c_fbx_animation_curve
{
	/*! \brief tells which attribute this curve is animating */
	fbx_curve_attribute_kind curve_attribute;

	/*! \brief index of first keyframe in the keyframe table */
	UINT32 index_in_keyframe_table;

	/*! \brief number of keyframes for that curve */
	UINT32 keyframe_count;
};

/*! \brief animation curve keyframe */
struct fbx_animation_keyframe
{
	/*! \brief time */
	float time;

	/*! \brief value */
	float value;

	/*! \brief interpolation */
	fbx_keyframe_interpolation interpolation;

	/*! \brief left tangent weight */
	float left_tangent_weight;

	/*! \brief left tangent velocity */
	float left_tangent_velocity;

	/*! \brief right tangent weight */
	float right_tangent_weight;

	/*! \brief right tangent velocity */
	float right_tangent_velocity;
};

/*! \brief this struct contains some object mapping data (references from newly created structure in reference to sdk objects.
* This is required by some parts, for example building rigging information
*/
struct fbx_scene_objects_mapping
{
	/*! \brief contiguous array of native sdk fbx nodes, in same order as our flat hierarchy */
	void * native_node_table;
};


/*! \brief represents an fbx scene data*/
struct fbx_scene_data
{
	/*! \brief scene information */
	fbx_scene_info					scene_info;
	/*! \brief load options that have been used to get this data */
	fbx_scene_data_load_options		load_options;
	/*! \brief contiguous array of nodes, root node is at zero location */
	fbx_node *						nodes;
	/*! \brief contiguous array of nodes animations, size is n animation stack * node count */
	fbx_node_animation*				nodes_animations;
	/*! \brief contiguous array of materials */
	st_fbx_material *				materials;
	/*! \brief animation curves */
	char *							node_names_table;
	/*! \brief list of texture files */
	char *							texture_files_name_table;
	/*! \brief list of animation curve names*/
	char*							animation_names_table;
	/*! \brief list of material names*/
	char*							material_names_table;
	/*! \brief animation curves */
	c_fbx_animation_curve *			animation_curves;
	/*! \brief animation keyframes */
	fbx_animation_keyframe *		animation_keyframes;
	/*! \brief animation info for mesh blend shapes */
	fbx_mesh_blend_shape_animation*	mesh_blend_shape_animations;
	/*! \brief If code is success, contains scene mapping data, if fail, this is undefined */
	fbx_scene_objects_mapping		object_mapping;
};

///////////////  Result data structures

/*! \brief result struct when trying to load a mesh */
struct fbx_mesh_load_result
{
	/*! \brief generic result code */
	fbx_mesh_load_result_code result_code;

	/*! \brief generic result code that indicates how normals were loaded */
	fbx_mesh_optional_attribute_load_result normal_result_code;
	/*! \brief generic result code that indicates how tangents were loaded */
	fbx_mesh_optional_attribute_load_result tangents_result_code;

	/*! \brief maximum number of bones per vertex */
	UINT32 max_bone_per_vertex;
	/*! \brief maximum lost influence per vertex in case bone count is limited */
	float max_vertex_lost_influence;

	/*! \brief mesh structure, if load failed, this is undefined (api does zero memory on that structure, so if guarantees that pointers will be set to NULL */
	fbx_mesh_data mesh;
};

/*! \brief Scene load result */
struct fbx_scene_load_result
{
	/*! \brief Result code */
	fbx_scene_load_result_code result_code;
	/*! \brief If code is success, contains scene data, if fail, this is undefined */
	fbx_scene_native scene;
};

/*! \brief result of mesh validation process */
struct fbx_scene_validate_result
{
	/*! \brief we found that some nodes carry more than just position / scale / rotation data (like offsets or pivots), this is reported as standard information so that caller knows about it */
	bool non_srt_info_in_nodes;
	/*! \brief we found that some animation stacks are multi layered */
	bool multi_layer_animation;
	/*! \brief we found that some nodes rotation order are non consistent (they should all be in the same order) */
	bool non_consistent_rotation_order;
};


/*! \brief result of mesh validation process */
struct fbx_mesh_validate_result
{
	/*! \brief we found that index buffer contains the same index for a triangle in a mesh */
	bool same_index_found_in_triangle;
	/*! \brief we found that index buffer contains indices ouside of vertex count */
	bool overload_index_found_in_triangle;
	/*! \brief we found that bone index buffer contains indices ouside of bone count */
	bool overload_bone_found_in_bone_index_buffer;
	/*! \brief we found a vertex with no bone attached */
	bool null_vertex_to_bone_index;
};
