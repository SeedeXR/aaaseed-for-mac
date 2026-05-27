#if !AAA_IS_AMD

//	Compute the normal
vec3 fnormal( in vec3 normal )
{
	return normalize( gl_NormalMatrix * normal );
//	return (gl_ModelViewMatrix * vec4( normal.xyz, 0.0 )).xyz;
}

float	get_fog( in float z_to_eye )
{
 	//	linear version
	float fog = (gl_Fog.end - z_to_eye) * gl_Fog.scale;
	//	fog = exp2( -gl_Fog.density * gl_FogFragCoord * LOG2E );
		//The equation for fog allows us to move a portion of
		//the calculation (-gl_Fog.density * LOG2E) to the CPU
		//increasing the efficiency of our shader by doing a
		//scalar multiplication on the CPU rather than the GPU.
		//	fog = exp2( gl_FogFragCoord * -0.144270 ); 	//	from 3Dlabs FOG_DENSITY .15
	//	fog = exp2( -gl_Fog.density * gl_Fog.density * gl_FogFragCoord * gl_FogFragCoord * LOG2E );
		//The equation for fog allows us to move a portion of
		//the calculation (-gl_Fog.density * gl_Fog.density * LOG2E)
		//to the CPU increasing the efficiency of our shader by doing a
		//scalar multiplication on the CPU rather than the GPU.
		//	fog = exp2(gl_FogFragCoord * gl_FogFragCoord * -0.014427); 	//	from 3Dlabs FOG_DENSITY .15 bug here
	return clamp_01( fog );
}
vec3	compute_fog3( in vec3 color_in, in float z_to_eye )
{
	return mix( gl_Fog.color.xyz, color_in, get_fog( z_to_eye ) );
	//   texture = vec4( mix( vec3(gl_Fog.color), vec3(texture), fog), texture.a);
}
vec4	compute_fog4( in vec4 color_in, in float z_to_eye )
{
	return mix( gl_Fog.color, color_in, get_fog( z_to_eye ) );
	//   texture = vec4( mix( vec3(gl_Fog.color), vec3(texture), fog), texture.a);
}

#endif //#if !AAA_IS_AMD


float	compute_fall_off( in vec3 eye_coor_pos_3, in vec3 normal )
{
	#if 0
		vec3 VP = vec3 (0.0, 0.0, 1.0) - eye_coor_pos_3;	// direction from surface to eye
		VP = normalize( VP );
		return clamp( dot( VP, normal ), 0.0, 1.0 );
	#else
		return clamp( -	dot( normalize( eye_coor_pos_3 ), normal ), 0.0, 1.0 );
	#endif
}

vec2 SphereMap( in vec3 ecPosition3, in vec3 normal )
{
	vec3 u = normalize( ecPosition3 );
	vec3 r = reflect( u, normal );
	float m = 2. * sqrt( r.x * r.x + r.y * r.y + ( r.z + 1. ) * ( r.z + 1. ) );
	return vec2( r.x / m + 0.5, r.y / m + 0.5 );
}

vec3 ReflectionMap( in vec3 ecPosition3, in vec3 normal)
{
	vec3 u = normalize( ecPosition3 );
	return reflect( u, normal );
}

vec4 ftexgen_0_vec4( in vec4 in_texcoor, in vec4 vertex, in vec3 normal, in vec4 eye_cam_position, in int tex_gen )
{
	vec4 texcoor;
	// Compute texture coordinate
	if( tex_gen==0 )
		texcoor = gl_TextureMatrix[0] * in_texcoor;
	else if( tex_gen == 1 )
	{
		texcoor.s = dot( vertex, gl_ObjectPlaneS[0] );
		texcoor.t = dot( vertex, gl_ObjectPlaneT[0] );
		texcoor.p = dot( vertex, gl_ObjectPlaneR[0] );
		texcoor.q = dot( vertex, gl_ObjectPlaneQ[0] );
	}
	else if( tex_gen == 2 )
	{
		texcoor.s = dot( eye_cam_position, gl_EyePlaneS[0] );
		texcoor.t = dot( eye_cam_position, gl_EyePlaneT[0] );
		texcoor.p = dot( eye_cam_position, gl_EyePlaneR[0] );
		texcoor.q = dot( eye_cam_position, gl_EyePlaneQ[0] );
	}
	else if( tex_gen == 3 )
	{
		vec3 eye_cam_position_v3 = vec3(eye_cam_position) / eye_cam_position.w;
		texcoor = vec4( SphereMap( eye_cam_position_v3, normal ), 0., 1. );
	}
	else if( tex_gen == 4 )
	{
		vec3 eye_cam_position_v3 = vec3(eye_cam_position) / eye_cam_position.w;
		texcoor = vec4( ReflectionMap( eye_cam_position_v3, normal ), 1. );
	}
	else if( tex_gen == 5 )
		texcoor = vec4( normal, 1.0 );
	return texcoor;
}
vec4 ftexgen_0_vec4( in vec4 in_texcoor, in vec4 vertex, in vec3 normal, in vec4 eye_cam_position )
{
	return ftexgen_0_vec4( in_texcoor, vertex, normal, eye_cam_position, aaa_tex_gen[0] );
}

vec2 ftexgen_0_vec2( in vec4 in_texcoor, in vec4 vertex, in vec3 normal, in vec4 eye_cam_position, in int tex_gen )
{
	vec2 texcoor;
	// Compute texture coordinate
	if( tex_gen==0 )
		texcoor = (gl_TextureMatrix[0] * in_texcoor).st;
	else if( tex_gen == 1 )
	{
		texcoor.s = dot( vertex, gl_ObjectPlaneS[0] );
		texcoor.t = dot( vertex, gl_ObjectPlaneT[0] );
	}
	else if( tex_gen == 2 )
	{
		texcoor.s = dot( eye_cam_position, gl_EyePlaneS[0] );
		texcoor.t = dot( eye_cam_position, gl_EyePlaneT[0] );
	}
	else if( tex_gen == 3 )
	{
		vec3 eye_cam_position_v3 = vec3(eye_cam_position) / eye_cam_position.w;
		texcoor = SphereMap( eye_cam_position_v3, normal );
	}
	else if( tex_gen == 4 )
	{
		vec3 eye_cam_position_v3 = vec3(eye_cam_position) / eye_cam_position.w;
		texcoor = ReflectionMap( eye_cam_position_v3, normal ).st;
	}
	else if( tex_gen == 5 )
		texcoor = normal.st;
	return texcoor;
}
vec2 ftexgen_0_vec2( in vec4 in_texcoor, in vec4 vertex, in vec3 normal, in vec4 eye_cam_position )
{
	return ftexgen_0_vec2( in_texcoor, vertex, normal, eye_cam_position, aaa_tex_gen[0] );
}

/* in case some shader will need it but this depreciated
void ftexgen( in vec3 normal, in vec4 ecPosition )
{
	// Compute texture coordinate
	int tex_gen = aaa_tex_gen[0];
	if( tex_gen==0 )
		gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
	else if( tex_gen == 1 )
	{
		gl_TexCoord[0].s = dot( gl_Vertex, gl_ObjectPlaneS[0] );
		gl_TexCoord[0].t = dot( gl_Vertex, gl_ObjectPlaneT[0] );
		gl_TexCoord[0].p = dot( gl_Vertex, gl_ObjectPlaneR[0] );
		gl_TexCoord[0].q = dot( gl_Vertex, gl_ObjectPlaneQ[0] );
	}
	else if( tex_gen == 2 )
	{
		gl_TexCoord[0].s = dot( ecPosition, gl_EyePlaneS[0] );
		gl_TexCoord[0].t = dot( ecPosition, gl_EyePlaneT[0] );
		gl_TexCoord[0].p = dot( ecPosition, gl_EyePlaneR[0] );
		gl_TexCoord[0].q = dot( ecPosition, gl_EyePlaneQ[0] );
	}
	else if( tex_gen == 3 )
	{
		vec3 ecPosition3;
		ecPosition3 = (vec3(ecPosition)) / ecPosition.w;
		vec2 sphereMap;
		sphereMap = SphereMap( ecPosition3, normal );
		gl_TexCoord[0] = vec4( sphereMap, 0.0, 1.0 );
	}
	else if( tex_gen == 4 )
	{
		vec3 ecPosition3;
		ecPosition3 = (vec3(ecPosition)) / ecPosition.w;
		vec3 reflection;
		reflection = ReflectionMap( ecPosition3, normal );
		gl_TexCoord[0] = vec4( reflection, 1.0 );
	}
	else if( tex_gen == 5 )
		gl_TexCoord[0] = vec4( normal, 1.0 );
}
*/






































































































































































































































































































































































































































































































































//end should be x * 100 lines
