

#define MAX_BONE_MAT 256

#ifndef ENABLE_TANGENT
#define ENABLE_TANGENT 1
#endif

#ifndef USE_BONE_SSBO
#define USE_BONE_SSBO 1
#endif

#ifndef ENABLE_BLEND_SHAPE
#define ENABLE_BLEND_SHAPE 1
#endif

#ifndef MAX_BLEND_SHAPE
#define MAX_BLEND_SHAPE 8
#endif

// same than pre Monaco aquarium
#define b_catmull_rom 		aaa_vu_int[0]
#define b_maa_orientation	aaa_vu_int[1]



layout(location = 0) in vec3  in_pos_object;
layout(location = 1) in vec3  in_norm_object;
layout(location = 2) in vec2  in_texcoord_object;

layout(location = 3) in vec3  in_tangent_object;
layout(location = 4) in vec3  in_binormal_object;

#if USE_BONE_SSBO == 0
layout(location = 5) in uvec4 in_bone_index_object;
layout(location = 6) in vec4  in_bone_weight_object;
#endif

layout(location = 0) out vec3 out_position_world;
layout(location = 1) out vec3 out_normal;
layout(location = 2) out vec2 out_texcoord;

#if ENABLE_TANGENT == 1
layout(location = 3) out mat3 out_tangent_frame;
#endif

layout(binding = 0) uniform skin_matrix_block					{	mat4 mat[MAX_BONE_MAT];		} 	g_skinning_matrices;
layout(binding = 1) uniform bind_pose_matrix_block				{	mat4 mat[MAX_BONE_MAT];		} 	g_bind_pose_matrices;
layout(binding = 2) uniform bone_to_node_index_block			{	uint index[MAX_BONE_MAT];	} 	g_bone_to_node_index;

//ssbo
#if USE_BONE_SSBO == 1
layout(std430, binding = 0) buffer vertex_bone_offset_buffer	{	uint vertex_bone_offset[];		};
layout(std430, binding = 1) buffer vertex_bone_count_buffer		{	uint vertex_bone_count[];		};
layout(std430, binding = 2) buffer vertex_bone_indices_buffer	{	uint vertex_bone_indices[];		};
layout(std430, binding = 3) buffer vertex_bone_weights_buffer	{	float vertex_bone_weights[];	};
#endif

#if ENABLE_BLEND_SHAPE == 1
layout(std430, binding = 4) buffer blend_shape_position_buffer	{	float blend_positions[];		};
layout(std430, binding = 5) buffer blend_shape_normal_buffer	{	float blend_normals[];			};

struct blend_shape_info
{
	int source_location;
	int target_location;
	float blend_amount;
	float _dummy_;
};

layout(binding = 3)	uniform blend_shape_data_block
{
	blend_shape_info[MAX_BLEND_SHAPE] shapes;
	uint shape_count;
}
g_blend_shape_data;

vec3 fetch_position(int vertex_id)
{
	vertex_id *= 3;
	return vec3( blend_positions[vertex_id], blend_positions[vertex_id + 1], blend_positions[vertex_id + 2] );
}

vec3 fetch_normal(int vertex_id)
{
	vertex_id *= 3;
	return vec3( blend_normals[vertex_id], blend_normals[vertex_id + 1], blend_normals[vertex_id + 2] );
}

vec3 blend_position( int vertex_id, vec3 default_position )
{
	//no blend shape, return vertex
	if( g_blend_shape_data.shape_count == 0 )
		return default_position;

	//apply first shape in a linear fashion
	blend_shape_info shape_info = g_blend_shape_data.shapes[0];
	vec3 source_first = fetch_position( shape_info.source_location + vertex_id );
	vec3 target_first = fetch_position( shape_info.target_location + vertex_id );
	vec3 result = mix( source_first, target_first, shape_info.blend_amount );

	//next shapes are additive
	for( uint i = 1; i < g_blend_shape_data.shape_count; ++i )
	{
		shape_info = g_blend_shape_data.shapes[i];
		vec3 source = fetch_position( shape_info.source_location + vertex_id );
		vec3 target = fetch_position( shape_info.target_location + vertex_id );
		vec3 diff = target - source;
		diff *= shape_info.blend_amount;

		result += diff;
	}

	return result;
}

//todo not called not check
vec3 blend_normal( int vertex_id, vec3 default_normal )
{
	//no blend shape, return vertex
	if( g_blend_shape_data.shape_count == 0 )
		return default_normal;

	//apply first shape in a linear fashion
	blend_shape_info shape_info = g_blend_shape_data.shapes[0];
	vec3 source_first = fetch_normal( shape_info.source_location + vertex_id );
	vec3 target_first = fetch_normal( shape_info.target_location + vertex_id );
	vec3 result = mix( source_first, target_first, shape_info.blend_amount );

	//next shapes are additive
	for( uint i = 1; i < g_blend_shape_data.shape_count; ++i )
	{
		shape_info = g_blend_shape_data.shapes[i];
		vec3 source = fetch_normal(shape_info.source_location + vertex_id);
		vec3 target = fetch_normal(shape_info.target_location + vertex_id);
		vec3 diff = target - source;
		diff *= shape_info.blend_amount;

		result += diff;
	}

	return result;
}
#endif

mat4 get_bone_matrix( uint bone_index )
{
	uint node_index = g_bone_to_node_index.index[bone_index];
	return g_skinning_matrices.mat[node_index] * g_bind_pose_matrices.mat[bone_index];
}

vec4 transform_bone( vec4 input_vector, uint bone_index )
{
	return input_vector * get_bone_matrix(bone_index);
}

vec3 transform_bone4( int vertex_id, vec4 input_vector )
{
#if USE_BONE_SSBO == 1
	uint i = vertex_bone_offset[vertex_id];
	uint i_end = i +	vertex_bone_count[vertex_id];

	//handle case with no bones
	if( i < i_end )
	{
		vec3 v = vec3( 0 );
		for( ; i < i_end; ++i )
			v	+=	vertex_bone_weights[i]	* ( get_bone_matrix(vertex_bone_indices[i]) * input_vector ).xyz ;
		return v;
	}
	else
	{
		vec4 p = vec4(input_vector.xyz, 1.0) * g_bind_pose_matrices.mat[0];
		return p.xyz;
	}
#else
		return		in_bone_weight_object.x	* ( get_bone_matrix(in_bone_index_object.x) * input_vector ).xyz
				+	in_bone_weight_object.y	* ( get_bone_matrix(in_bone_index_object.y) * input_vector ).xyz
				+	in_bone_weight_object.z	* ( get_bone_matrix(in_bone_index_object.z) * input_vector ).xyz
				+	in_bone_weight_object.w	* ( get_bone_matrix(in_bone_index_object.w) * input_vector ).xyz ;
#endif
}

vec3 change_orientation( inout vec3 v )
{
	return v.xyz * vec3( -1.0, 1.0, -1.0 );
}


void main(void)
{
//blend
#if ENABLE_BLEND_SHAPE == 1
	vec3 position	= blend_position( gl_VertexID, in_pos_object );
	vec3 normal		= in_norm_object;	//todomona blend_normal(gl_VertexID);
#else
	vec3 position	= in_pos_object;
	vec3 normal		= in_norm_object;
#endif

// skin
	position		= transform_bone4( gl_VertexID, vec4(position,				1.0 ) );
	normal			= transform_bone4( gl_VertexID, vec4(normal,				0.0 ) );
#if ENABLE_TANGENT == 1
	vec3 tangent	= transform_bone4( gl_VertexID, vec4(in_tangent_object,		0.0 ) );
	vec3 binormal	= transform_bone4( gl_VertexID, vec4(in_binormal_object,	0.0 ) );
#endif

// convert to what fit catmull_rom
	if( b_maa_orientation == 1 )
	{
		position = change_orientation( position	);
		normal   = change_orientation( normal	);
#if ENABLE_TANGENT == 1
		tangent  = change_orientation( tangent	);
		binormal = change_orientation( binormal );
#endif
	}

//todomana unused most of the time
//	skin in world
	//do this before passing it to the shader
	mat4 mat = aaa_scene.matrix * aaa_model.matrix;
	position 	= ( mat * vec4( position,	1.0 ) ).xyz;
    normal 		= ( mat * vec4( normal,		0.0 ) ).xyz;
#if ENABLE_TANGENT == 1
	tangent		= ( mat * vec4( tangent,	0.0 ) ).xyz;
	binormal 	= ( mat * vec4( binormal,	0.0 ) ).xyz;
#endif

//	position	*= 25;

	// catmul rom.
	if( b_catmull_rom == 1 )
	{
		//flip and scale coor
		position.x += aaa_vu_float[2];
		position *=	aaa_vu_float[3] * aaa_vu_float[7];
#if ENABLE_TANGENT == 1
		//deform on catmull
		deform_catmull_rom_monaco(	position, normal, tangent, binormal );
#else
		deform_catmull_rom_monaco(	position, normal );
#endif
	}

	//normalize and output
    normal	= normalize( normal );
#if ENABLE_TANGENT == 1
	out_tangent_frame[0] = normalize( tangent );
	out_tangent_frame[1] = normalize( binormal );
	out_tangent_frame[2] = normal;
#endif
	out_position_world 	= position;
    out_normal 			= normal;
    out_texcoord 		= in_texcoord_object;

//	gl_Position = aaa_cam.projection * aaa_cam.view * vec4(out_position_world, 1.0);
	gl_Position = aaa_cam.view_projection * vec4( position, 1.0 );
}