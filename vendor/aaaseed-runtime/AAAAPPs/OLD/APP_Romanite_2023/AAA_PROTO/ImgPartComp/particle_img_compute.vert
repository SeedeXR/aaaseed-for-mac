// 0 is p0, p0 relexion, ..., pn, pn reflexion
// 1 is p0,....,pn,p0 reflexion,...,pn relexion
// 0 seems to be the fastest
#define B_PASS_WHOLE 0
#if B_PASS_WHOLE
	const int nb = nb_u * nb_v;
#endif

layout( std430, binding=0 ) buffer particle_pos_out_buffer	{	vec4 pos_in[];	};
layout( std430, binding=1 ) buffer particle_col_out_buffer	{	vec4 col_in[];	};
layout( std430, binding=2 ) buffer particle_norm_out_buffer	{	vec4 norm_in[];	};
layout( std430, binding=3 ) buffer particle_attr_out_buffer	{	vec4 attr_in[];	};

CONST vec3 vol_size			= aaa_vu_vec4[0].xyz;
CONST float rot_y			= aaa_vu_vec4[0].w;
CONST vec2 vol_size_over	= aaa_vu_vec4[1].xy;

CONST int s_test			= aaa_vu_int[0];
CONST int s_prim			= aaa_vu_int[1];
CONST int nb_u				= aaa_vu_int[2];
CONST int nb_v				= aaa_vu_int[3];

CONST float du				= aaa_vu_float[0];
CONST float dv				= aaa_vu_float[1];
CONST float displace_z		= aaa_vu_float[2];
CONST float displace_gamma	= aaa_vu_float[3];


layout(binding = 1) uniform sampler2D TEX_DEPTH;


layout(location = 0) out vec3 out_normal;
layout(location = 1) out vec3 out_position_world;
layout(location = 2) out vec2 out_texcoord;
layout(location = 3) out vec4 out_color;
layout(location = 4) out vec4 out_rnd;

const float SU = .5;
const float SV = SU;
// TRIANGLE
const vec3 vertex_tri[6] =
{
	vec3(-SU,-SV,0), vec3(SU, SV,0), vec3(-SU,SV,0),
	vec3(-SU,-SV,0), vec3(SU,-SV,0), vec3( SU,SV,0),	
};
const vec2 uv_tri[6] =
{
	vec2(0,0), vec2(1,1), vec2(0,1 ),			
	vec2(0,0), vec2(1,0), vec2(1,1 ),			
};
// STRIP
// first 4 for regular pass, next 4 for relexion
const vec3 vertex_strip[8] =
{
	vec3(-SU,SV,0),		vec3(-SU,-SV,0),	vec3( SU,SV,0),		vec3( SU,-SV,0),
	vec3( SU,-SV,0),	vec3( SU, SV,0),	vec3(-SU,-SV,0),	vec3(-SU, SV,0)
};
const vec2 uv_strip[8] =
{
	vec2(0,1), vec2(0,0),	vec2(1,1), vec2(1,0),
	vec2(0,0), vec2(0,1),	vec2(1,0), vec2(1,1)
};
//FAN
const vec3 vertex_fan[4] =
{
	vec3(-SU,-SV,0), vec3( SU,-SV,0),
	vec3( SU, SV,0), vec3(-SU, SV,0),
};
const vec2 uv_fan[4] =
{
	vec2(0,0), vec2(1,0),
	vec2(1,1), vec2(0,1),						
};

void main(void)
{
	uint id;
	vec2 uv;
	vec3 vertex;


	
#if B_PASS_WHOLE
//	float s_pass = (gl_InstanceID < nb) ? 0 : 1;	// 0 regular pass, 1 reflexion
	int s_pass = int( gl_InstanceID < nb );	// 0 regular pass, 1 reflexion
#else
	const int pass_selector = 1;	// 0 just one pas: useful to avoid reflexion calculation 
	int s_pass = gl_InstanceID & pass_selector;	// 0 regular pass, 1 reflexion
#endif

	//todo deal with others modes than strip
	// switch( s_prim )
	// {
	// case 1:	// POINTS
	// 	id = gl_VertexID;
	// 	break;
	// case 2:	// LINES
	// case 3:	// LINE_LOOP
	// case 4:	// LINE_STRIP
	// 	id = gl_VertexID;
	// 	break;
	// case 5:	// TRIANGLES
	// 	{
	// 		id = gl_VertexID / 6;
	// 		uint vid = gl_VertexID % 6;
	// 		vertex = vertex_tri[vid];
	// 		uv = uv_tri[vid];
	// 	}
	// 	break;
	// case 6: // TRIANGLE_STRIP
	// case 9: // QUAD_STRIP
		{
#if B_PASS_WHOLE
			id = gl_InstanceID % nb;
#else
			id = gl_InstanceID >> pass_selector;
#endif
			int vertex_id = gl_VertexID + s_pass*4;
			vertex = vertex_strip[gl_VertexID];
			uv = uv_strip[vertex_id];
		}
	// 	break;
	// case 7:	// GL_TRIANGLE_FAN
	// case 8: // QUADS
	// case 10:// POLYGON
		// {
		// 	id = gl_InstanceID >> 1;
		// 	vertex = vertex_fan[gl_VertexID];
		// 	uv = uv_fan[gl_VertexID];
		// }
	// 	break;	
	// }

	vec3 pos			= pos_in[id].xyz;
	vec4 color			= col_in[id];
//	vec4 norm_object	= norm_in[id];
	vec4 attr			= attr_in[id];

	uint iv = id / nb_u;
	uint iu = id % nb_u;	

//	mat4 mat = aaa_scene.matrix * aaa_model.matrix;
	vec4 position_world = vec4(	pos,  1. );
	float coor_factor =  1. - s_pass * 2.;
	float y = coor_factor * position_world.y;
	position_world.y = y;
    //vec4 normal_world 	= vec4(	norm_object.xyz,	0.	);

	// if( s_prim == 1 )
	// {
	// 	float size = 1.;
	// 	gl_PointSize = size;
	// 	out_texcoord = vec2( 0. );
	// }
	// else
	// {
		//gl_PointSize = size;
		// we have to deal here with the same transfo than the one done in compute shader
		vertex.xyz *= vec3( du,dv,1. ) * vol_size; // * vec3( .9, .9, 1 );
	//}

	// compute uv needed for color and depth texture
	uv = (vec2(iu,iv) + uv) * vec2(du,dv) ;
	

	// 
	vec4 depth = texture( TEX_DEPTH, uv );
	float displace = pow( depth.r, displace_gamma ) * displace_z;
	//vertex.z += (pow( depth.r, displace_gamma ) - 1.) * displace * 1. - pos_in[id].w;
	vertex.z += (displace - pos_in[id].w) * attr.w;
	rotate( vertex.xz, rot_y * attr.w );
	position_world.xyz += vertex;

	//if( s_pass > 0 )
	{
	 	y = linearstep( 0,-2.5, y );
	 	//col.xyz = vec3( pow( 1.-y, 1. ) );
#if 0
	 	color.rgb *= vec3( (1. - y * s_pass * 3.) * ( 1- s_pass*.25) );
#else
		color.rgb *= 1. + s_pass * -.2;//+(-.3 + y * 1.);
		color.a *= 1 + s_pass * (-.3- y * 2.);
#endif
		const float blur_factor = .8;
		//y = pow(y, .8	);
		uv.xy += s_pass * y * (attr.xy-.5) * blur_factor * vol_size_over * vec2( 1, .5);

	// 	position_world.xz += s_pass * pow( y, 4. ) * (attr.xz-.5) * 2.;
	// 	out_texcoord.x =  1. - out_texcoord.x;
	}

	out_texcoord = uv;

	out_color = color * gl_Color;
//	out_color.a *= .a;	// * alpha_factor;

	//out_color.rgb *= col_in[id].rgb;
	//out_color.xyz = vec3(1);
	//out_color = vec4( 1,attr.w,1, 1 );

	out_position_world 	= position_world.xyz;
	
    //out_normal 		= normalize( normal_world.xyz );
	//out_normal 		= normal_world.xyz;
	  
	out_rnd				= attr;
	out_rnd.rg = y * vol_size_over * .2;

	gl_Position = aaa_cam.view_projection * vec4(position_world.xyz, 1. );
}
