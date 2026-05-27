
#define ATTRIB_NB		4

#define TEX_COOR_DIM 2
#define TEX_COOR_VEC vec2
out VS_out
{
	TEX_COOR_VEC	tex_impli;
	TEX_COOR_VEC	tex_refle;
	TEX_COOR_VEC	tex_objec;
	vec4			color;
	float			fall_off;
	float			z_for_fog;
	vec3			normal;
	float			fall_top;
} vs_out;

//	AAAseed uniform vertex shader
//uniform float	aaa_gu_float[];
//uniform float	aaa_vu_float[];
//	0	ds
//	1	ds_over
//	2	s_inter
//	3	size_factor	< 1.
//	4	Tail.angle
//	5	Tail.amp
//	7	Tail.begin
//uniform int		aaa_vu_int[];
//	0	0 deform on catmull
//		1 fix at origin
//	3	Tail.vert
//uniform vec4	aaa_vu_vec4[8];
//	0.w	Morph A
//	1.w	Morph B

#include	"aqua_deform.glsl"


// --------------------------------------------------------------------------
// TEXTURE COORD FUNCTION
//	generate texture coordonate
void ftexgen( const in vec3 normal, const in vec4 ecPosition )
{
	TEX_COOR_VEC tex_coor;

	// Compute texture coordinate from uv implicit
	#if (TEX_COOR_DIM == 4)
		tex_coor = gl_TextureMatrix[0] * gl_MultiTexCoord0;	//	gl_TexCoord[i] would be better but dont work for now
	#else
		tex_coor = (gl_TextureMatrix[0] * gl_MultiTexCoord0 ).st;	//	gl_TexCoord[i] would be better but dont work for now
	#endif
	vs_out.tex_impli = tex_coor;

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
			tex_coor = vec4( r, 1.0, 1.0 );
		#else
			tex_coor = r.xy;
		#endif
	#endif
	vs_out.tex_refle = tex_coor;
}

void ftexgen_objec( const in vec4 vertex )
{
	TEX_COOR_VEC tex_coor;
	tex_coor.s = dot( vertex, gl_ObjectPlaneS[3] );
	tex_coor.t = dot( vertex, gl_ObjectPlaneT[3] );
	vs_out.tex_objec.st = tex_coor.st;
}

// --------------------------------------------------------------------------
// MAIN FUNCTION

void main()
{
	vec4	vertex;
	vec3	normal;

	get_point_normal_morph_target_2( vertex, normal, aaa_vu_vec4[0].w, aaa_vu_vec4[1].w );

	float xs = aaa_vu_float[0] * 2. * aaa_vu_float[7];	// last number 0 => all, 1 => no
	float x = vertex.x - xs;
	if( x > 0 )
	{
		//	if( c  < 0. )	c = - pow(-c,.6);	else	c  = pow(c,.6);
		float angle = x * aaa_vu_float[1] * sin( aaa_vu_float[4] + (x * aaa_vu_float[1]) ) * aaa_vu_float[5];
		float c = cos( angle );
		float s = sin( angle );
		if( aaa_vu_int[3] > 0 )
		{	//	tail move verticaly
			float y;
			y = x * s + vertex.y * c;
			x = x * c - vertex.y * s;
			vertex.x = x + xs;
			vertex.y = y;
			x = normal.x;
			y = x * s + normal.y * c;
			x = x * c - normal.y * s;
			normal.x = x;
			normal.y = y;
		}
		else
		{	//	tail move horitaly
			float z;
			z = x * s + vertex.z * c;
			x = x * c - vertex.z * s;
			vertex.x = x + xs;
			vertex.z = z;
			x = normal.x;
			z = x * s + normal.z * c;
			x = x * c - normal.z * s;
			normal.x = x;
			normal.z = z;
		}
	}
	vertex.xyz *= aaa_vu_float[3];

//	DEFORM
	if( aaa_vu_int[0] >= 0. )
		deform_catmull( vertex.xyz, normal );

	vs_out.fall_top		=	normal.y;
	vs_out.z_for_fog	=	-vertex.z;

	ftexgen_objec( vertex );

//	TRANSFORM	position
	//	COMPUTE COOR IN DIFFERENT REFERENCE
	// Eye-coordinate position of vertex, needed in various calculations
	vec4	eye_coor_position = gl_ModelViewMatrix * vertex;

	// Do fixed functionality vertex transform
	//	ok but outdated in 4.2
	//		gl_Position = ftransform();
	//	ok but we already have ecPosition
	//		gl_Position = gl_ModelViewProjectionMatrix * vertex;
	gl_Position = gl_ProjectionMatrix * eye_coor_position;
	//	gl_Position = projection_matrix * modelview_matrix * vec4(vertex, 1.0);
	//	gl_Position = ModelviewProjection * vec4(position, 1);
//	TRANSFORM	normal
	normal = fnormal( normal );
	vs_out.normal = normal;

//	COMPUTE FALL OFF
	//	this is needed also for light computation
	vec3 ecPosition3 = (vec3 (eye_coor_position)) / eye_coor_position.w;
	vs_out.fall_off = compute_fall_off( ecPosition3 , normal );

//	COMPUTE LIGHT
	vec4	color;
	if( aaa_lights != 0u )
		color = flight_nb( normal, ecPosition3, 3 );
	else
		color = gl_Color;
	//gl_FrontColor = color;
	vs_out.color = color;

#if 0
//	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
#else
	ftexgen( normal, eye_coor_position );
#endif
	//	for fog and other
//	vs_out.z_for_fog = -eye_coor_position.z;
}