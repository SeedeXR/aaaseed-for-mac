
#define ATTRIB_NB		6
in	vec3	attrib[];

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



//#include	"aqua_deform.glsl"
void get_point_normal_morph_target_3( out vec4 vertex, out vec3 normal, in float f1, in float f2, in float f3 )
{
	vertex = gl_Vertex;
	normal = gl_Normal;
	if( f1 != 0. )
	{
		vertex.xyz	+=	attrib[0] * f1;
		normal		+=	attrib[1] * f1;
	}
	if( f2 != 0. )
	{
		vertex.xyz	+=	attrib[2] * f2;
		normal		+=	attrib[3] * f2;
	}
	if( f3 != 0. )
	{
		vertex.xyz	+=	attrib[4] * f3;
		normal		+=	attrib[5] * f3;
	}
}

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

#if	ATTRIB_NB==6
	get_point_normal_morph_target_3( vertex, normal, aaa_vu_vec4[0].w, aaa_vu_vec4[1].w, aaa_vu_vec4[2].w );
#else
	get_point_normal_morph_target_4( vertex, normal, aaa_vu_vec4[0].w, aaa_vu_vec4[1].w, aaa_vu_vec4[2].w, aaa_vu_vec4[3].w );
#endif

	float xs = aaa_vu_float[0] * 2. * aaa_vu_float[7];	// last number 0 => all, 1 => no
	float x = vertex.x - xs;
	if( x > 0 )
	{
		//	if( c  < 0. )	c = - pow(-c,.6);	else	c  = pow(c,.6);
		float angle = x * aaa_vu_float[1] * sin( aaa_vu_float[4] + (x * aaa_vu_float[1]) ) * aaa_vu_float[5];
		float c = cos( angle );
		float s = sin( angle );
		if( aaa_vu_int[3] > 0 )
		{
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
		{
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
		deform_catmull_rom( vertex.xyz, normal );

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
		color = flight_nb( normal, ecPosition3, 2 );
	else
		color = gl_Color;
	//	gl_FrontColor = color;
	vs_out.color = color;

#if 0
//	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
#else
	ftexgen( normal, eye_coor_position );
#endif
	//	for fog and other
//	vs_out.z_for_fog = -eye_coor_position.z;
}
