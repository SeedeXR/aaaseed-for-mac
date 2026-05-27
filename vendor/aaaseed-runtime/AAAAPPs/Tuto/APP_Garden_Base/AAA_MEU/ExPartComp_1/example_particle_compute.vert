

layout( std430, binding=0 ) buffer particle_pos_out_buffer	{	vec4 pos_in[];	};
layout( std430, binding=1 ) buffer particle_col_out_buffer	{	vec4 col_in[];	};
layout( std430, binding=2 ) buffer particle_norm_out_buffer	{	vec4 norm_in[];	};
layout( std430, binding=3 ) buffer particle_attr_out_buffer	{	vec4 attr_in[];	};

#define	vol_center			aaa_vu_vec4[0].xyz
#define	vol_size			aaa_vu_vec4[1].xyz

#define	part_size_in		aaa_vu_vec4[2].xy

#define	s_vert_test			aaa_vu_int[0]
#define	s_prim				aaa_vu_int[1]

#define size_min			aaa_vu_float[0]
#define size_factor			aaa_vu_float[1]
#define size_gamma			aaa_vu_float[2]

uniform sampler2D			aaa_samp1;
#define TEX_GRAD			aaa_samp1
uniform sampler2D			aaa_samp2;
#define TEX_COLOR			aaa_samp2

layout(location = 0) out vec3 out_normal;
layout(location = 1) out vec3 out_position_world;
layout(location = 2) out vec2 out_texcoord;
layout(location = 3) out vec4 out_color;
layout(location = 4) out vec4 out_rnd;

#define SU 1
#define SV 1

const vec4 vertex_tri[6] =
{
	vec4(	-SU,-SV,0,  1),		vec4(	SU, SV, 0,	1),	vec4(	-SU, SV, 0, 1),
	vec4(	-SU,-SV,0,	1),		vec4( 	SU,-SV, 0,	1),	vec4(	 SU, SV, 0, 1),	
};
const vec2 uv_tri[6] =
{
	vec2( 0, 0 ),	vec2( 1, 1 ),	vec2( 0, 1 ),			
	vec2( 0, 0 ),	vec2( 1, 0 ),	vec2( 1, 1 ),			
};

const vec4 vertex_strip[4] =
{
	vec4(	-SU, SV, 0, 1 ),	vec4(	-SU,-SV, 0, 1),
	vec4(	SU,  SV, 0, 1 ),	vec4(	SU,	-SV, 0,	1),
};
const vec2 uv_strip[4] =
{
	vec2( 0, 1 ),	vec2( 0, 0 ),		
	vec2( 1, 1 ),	vec2( 1, 0 ),		
};

const vec4 vertex_fan[4] =
{
	vec4(	-SU,-SV,0,1),		vec4(	SU,	-SV,	0,1),
	vec4(	SU,  SV,0,1),		vec4(	-SU, SV,	0,1),
};
const vec2 uv_fan[4] =
{
	vec2( 0, 0 ),	vec2( 1, 0 ),
	vec2( 1, 1 ),	vec2( 0, 1 ),						
};

void main(void)
{
	uint id;
	vec4 vertex;
	vec2 uv;

	switch( s_prim )
	{
	case 1:	// POINTS
		id = gl_VertexID;
		break;
	case 2:	// LINES
	case 3:	// LINE_LOOP
	case 4:	// LINE_STRIP
		id = gl_VertexID;
		break;
	case 5:	// TRIANGLES
		id = gl_VertexID / 6;
		uint vid = gl_VertexID % 6;
		vertex = vertex_tri[vid];
		uv = uv_tri[vid];
		break;
	case 6: // TRIANGLE_STRIP
	case 9: // QUAD_STRIP
		id = gl_InstanceID;
		vertex = vertex_strip[gl_VertexID];
		uv = uv_strip[gl_VertexID];
		break;
	case 7:	// GL_TRIANGLE_FAN
	case 8: // QUADS
	case 10:// POLYGON
		id = gl_InstanceID;
		vertex = vertex_fan[gl_VertexID];
		uv = uv_fan[gl_VertexID];
		break;	
	}

	vec3 pos_cano		= pos_in[id].xyz;
	vec4 grad			= norm_in[id];
	vec4 attr			= attr_in[id];


	//vec2 grad = get_gradient_2d(	TEX_GRAD,  col_in[id];.xy + .5 );
	//we get this from the compyute shader so it is done once by particle
	float d = grad.b;
	

	float part_size_factor;
	if( s_vert_test != 0 )
	{
		//.out_color.g = d2 * 16;
		//out_color.r = 0;
		out_color.rgb = grad.rgb;
//
//		out_color.a = col_in[id].a;
		out_color.a = 1.;
		part_size_factor = .05	;
	}
	else
	{
		out_color = col_in[id];
	//	gl_PointSize = size_grad_0 + attr.y * ( size_grad_1 - size_grad_0 );
	//	rad = a * 32. + pow( dot(grad,grad) * 32., size_gamma ) * size_factor;
		float d_remap = linearstep( aaa_vu_vec4[2].z, aaa_vu_vec4[2].w, d );
		part_size_factor = size_min + size_factor * pow( d_remap, size_gamma );
	}
	
	out_color *= gl_Color;
	//out_color = vec4(1);

//	mat4 mat = aaa_scene.matrix * aaa_model.matrix;
	vec3 position_world = vol_center + pos_cano * vol_size;
	//vec3 position_world = pos_cano;
	//position_world.z += size;
    vec3 normal_world 	= vec3(0,0,1);

	out_rnd				= attr;

	if( s_prim == 1 )
	{
		gl_PointSize = part_size_factor;
		out_texcoord = vec2( 0. );
	}
	else
	{
		gl_PointSize = part_size_factor;
		out_texcoord = uv;

		//vertex.xy *= .01;
		vertex.xy *= part_size_in * part_size_factor;
		// //vertex.xyz *= size;
		//grad.xy = (mat3(aaa_cam.view_inverse) * vec3( grad.xy, 0.) ).xy;
		//if( d > 0. )
		{
			//rotate in the gradient direction
			//vertex.xy *= mat2( grad.x, -grad.y, grad.y, grad.x );
			//rotate in the gradient perpendicular direction
			//vertex.xy *= mat2( grad.y, grad.x, -grad.x, grad.y );
			vertex.xy = vec2(	 vertex.y * grad.x + vertex.x * grad.y,
								 vertex.y * grad.y - vertex.x * grad.x  );
		}
		// following line seems to make sur particle always front the camera but this is a hack
		//vertex.xyz = mat3(aaa_cam.view_inverse) * vertex.xyz;
		position_world += vertex.xyz;
		//position_world.xyz = mat3(aaa_cam.view_inverse) * position_world.xyz;
	}
	
	out_position_world 	= position_world;
    out_normal 			= normalize( normal_world );
	gl_Position = aaa_cam.view_projection * vec4(position_world, 1. );
}
