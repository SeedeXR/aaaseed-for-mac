
#define TEX_COOR_DIM 2
#define TEX_COOR_VEC vec2
out VS_out
{
	TEX_COOR_VEC	tex_impli;
	TEX_COOR_VEC	tex_refle;
//	TEX_COOR_VEC	tex_objec;
	vec4			color;
	float			fall_off;
	float			z_to_eye;
	vec3			normal;
//	float			fall_top;
} vs_out;

uniform sampler2D	aaa_tex2d[4];



#include	"aqua_deform.glsl"

// --------------------------------------------------------------------------
// TEXTURE COORD FUNCTION
//	generate texture coordonate

void ftexgen_impli()
{
	TEX_COOR_VEC tex_coor;

	// Compute texture coordinate from uv implicit
	#if (TEX_COOR_DIM == 4)
		tex_coor = gl_TextureMatrix[0] * gl_MultiTexCoord0;	//	gl_TexCoord[i] would be better but dont work for now
	#else
		tex_coor = (gl_TextureMatrix[0] * gl_MultiTexCoord0 ).st;	//	gl_TexCoord[i] would be better but dont work for now
	#endif
	vs_out.tex_impli = tex_coor;

}


void ftexgen_reflec( const in vec3 normal, const in vec4 ecPosition )
{
	TEX_COOR_VEC tex_coor;


	vec3 ecPosition3 = vec3(ecPosition) / ecPosition.w;
	#if 0
		vec3 reflection = reflect( normalize( ecPosition3 ), normal );
		#if (TEX_COOR_DIM == 4)
			tex_coor = vec4( reflection, 1.0 );
		#else
			tex_coor = reflection.xy;
		#endif
	#else
		//sphere
		vec3 r = reflect( normalize( ecPosition3 ), normal );
		float m = 2.0 * sqrt( r.x * r.x + r.y * r.y + ( r.z + 1.0 ) * ( r.z + 1.0 ) );
		r.x = r.x / m + 0.5;
		r.y = r.y / m + 0.5;
		#if (TEX_COOR_DIM == 4)
			tex_coor = vec4( r.x, r,y, 1.0, 1.0 );
		#else
			tex_coor = r.xy;
		#endif
	#endif
	vs_out.tex_refle = tex_coor;
}
/*
void ftexgen_objec( const in vec4 vertex )
{
	TEX_COOR_VEC tex_coor;
	tex_coor.s = dot( vertex, gl_ObjectPlaneS[3] );
	tex_coor.t = dot( vertex, gl_ObjectPlaneT[3] );
	vs_out.tex_objec.st = tex_coor.st;
}
*/

// --------------------------------------------------------------------------
// GET COLOR FUNCTION

vec4 l_get_color_impli( in int unit )
{
	vec4	tex = texture( aaa_tex2d[unit], vs_out.tex_impli.st );
	tex.a = 1;
	return tex;
}


// --------------------------------------------------------------------------
// MAIN FUNCTION

void main()
{
	vec4	vertex;
	vec3	normal;
//	get_point_normal_morph_target_4( vertex, normal, aaa_vu_vec4[0].w, aaa_vu_vec4[1].w, aaa_vu_vec4[2].w, aaa_vu_vec4[3].w );
	get_point_normal_morph_target_2( vertex, normal, aaa_vu_vec4[0].w, aaa_vu_vec4[1].w );

	vec3	transformedNormal;
	vec4	ecPosition;

	ftexgen_impli();

	//	DEFORM CURVE
	if( aaa_vu_int[0] >= 0. )
		deform_catmull( vertex.xyz, normal );

//	vs_out.fall_top = normal.y;

//	ftexgen_objec( vertex );

	//	DEFORM CAUSTIC LEVEL 1

	// vertex
	float qttProv1 = aaa_gu_float[0]*0.01;
	float timingProv1 = aaa_vu_float[4]*aaa_gu_float[1];

	vertex.x += qttProv1*sin(vertex.x*12 +timingProv1);
	vertex.y += qttProv1*cos(vertex.y*12 +timingProv1);
	vertex.z += qttProv1*sin(vertex.z*12 +timingProv1);

	// normal
	vec3 vn = vertex.xyz + 0.1*normal.xyz;
	vn.x += 0.01*sin(vertex.x*12 + aaa_vu_float[4]);
	vn.y += 0.01*cos(vertex.y*12 + aaa_vu_float[4]);
	vn.z += 0.01*sin(vertex.y*12 + aaa_vu_float[4]);
	normal = normalize( vn - vertex.xyz);

	//	DEFORM CAUSTIC LEVEL 2 (mask)

	vec4 valDeformColor = l_get_color_impli(3);
	float valDeform = valDeformColor.r;

	// vertex
	vec3 deformProv;
	float qttProv2 = aaa_gu_float[2]*0.01;
	float timingProv2 = aaa_vu_float[4]*aaa_gu_float[3];

	deformProv.x = vertex.x + qttProv2*sin(vertex.x*50 + timingProv2);
	deformProv.y = vertex.y + qttProv2*cos(vertex.y*50 + timingProv2);
	deformProv.z = vertex.z + qttProv2*sin(vertex.z*50 + timingProv2);
	vertex.xyz = valDeform*deformProv + (1-valDeform)*vertex.xyz;

	// normal
	vec3 vn2 = vertex.xyz + 0.1*normal.xyz;
	vn2.x += 0.01*sin(vertex.x*20 + aaa_vu_float[4]);
	vn2.y += 0.01*cos(vertex.y*20 + aaa_vu_float[4]);
	vn2.z += 0.01*sin(vertex.y*20 + aaa_vu_float[4]);
	normal = normalize( vn2 - vertex.xyz);


	//	TRANSFORM	position
	//	COMPUTE COOR IN DIFFERENT REFERENCE
	// Eye-coordinate position of vertex, needed in various calculations
	ecPosition = gl_ModelViewMatrix * vertex;

	// Do fixed functionality vertex transform
	//	ok but outdated in 4.2
	//		gl_Position = ftransform();
	//	ok but we already have ecPosition
	//		gl_Position = gl_ModelViewProjectionMatrix * vertex;
	gl_Position = gl_ProjectionMatrix * ecPosition;
	//	gl_Position = projection_matrix * modelview_matrix * vec4(vertex, 1.0);
	//	gl_Position = ModelviewProjection * vec4(position, 1);

//	TRANSFORM	normal
	normal = fnormal( normal );

	transformedNormal = normal;
	vs_out.normal = normal;

//	COMPUTE FALL OFF
	//	this is needed also for light computation
	vec3 ecPosition3 = (vec3 (ecPosition)) / ecPosition.w;
	// Compute vector from surface to eye
	#if 0
		vec3 VP = vec3 (0.0, 0.0, 1.0) - ecPosition3;	// direction from surface to eye
		VP = normalize( VP );
		vs_out.fall_off = clamp( dot( VP, normal ), 0.0, 1.0 );
	#else
		vs_out.fall_off = clamp( -	dot( normalize( ecPosition3 ), normal ), 0.0, 1.0 );
	#endif

//	COMPUTE LIGHT
	vec4	color;
	if( aaa_lights!=0u )
		color = flight_nb( transformedNormal, ecPosition3, 3 );
	else
		color = gl_Color;
//	gl_FrontColor = color;
	vs_out.color = color;

#if 0
//	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
#else
	ftexgen_reflec( transformedNormal, ecPosition );
#endif
	//	for fog and other
	vs_out.z_to_eye = abs(ecPosition.z);
}