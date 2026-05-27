
#define ATTRIB_NB		4

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

//	AAAseed uniform vertex shader
//uniform float	aaa_gu_float[];
//uniform float	aaa_vu_float[8];
//	0	where the tail start in segment unit
//			this was the len of a segment (2 points of the catmull rom)
//	1	race.ds_over	same over 1 (avoid division)
//	2	nothing
//	3	size_factor	< 1.
//	4	Tail.phase
//	5	Tail.amp
//	7	Tail.begin

//#define LEN_SEG_OVER		(aaa_vu_float[1])
#define TAIL_BEGIN			(aaa_vu_float[7])
//#define TAIL_LEN_OVER		(aaa_vu_float[2])
//#define SIZE_FACTOR			(aaa_vu_float[3])
#define TAIL_PHASE			(aaa_vu_float[4])
#define	TAIL_AMP			(aaa_vu_float[5])

//uniform int		aaa_vu_int[4];
//	0	0 deform on catmull
//		1 fix at origin
//	3	tail_vert
//uniform vec4	aaa_vu_vec4[8];
//	0-5.xyz 6 pt as xyz coordonates for 3 catmull rom segment 0123 1234 2345
//	6 xyz offset
//	7 are free for now
//	0.w	Morph A
//	1.w	Morph B
//	2.w	Morph C
//	3.w	Morph D
//	...

//#include	"aqua_deform.glsl"

void	deform_catmull_v2( inout vec3 pos, inout vec3 nor )
{
	//	deformation, maa's cooking
	float x = pos.x;
	vec3 vx;
	vec3 o;

	//	DEFORMATION sur 6 pts
	if( x <= 0 )
	{
		o = catmull_rom_both(	vx, 0, 0 );
		vx = normalize( vx );
		o += vx * x ;
	}
	else
	{
		x *= LEN_SEG_OVER;
		int index = min( 2, int( floor(x) ) );
		o = catmull_rom_both( vx, index, x - index );
		vx = normalize( vx );
	}

	//	BUILD COOR SYSTEM VECTOR
	vec3 vz = normalize( cross( vx, vec3(0.,1.,0.) ) );
	vec3 vy = cross( vz, vx );

	//	mat3 basis = mat3( vx, vy, vz );

	//	COMPUTE DEFORMED POINT
		// Transform the normal vector and positions:
		//	vNormal = NormalMatrix * (basis * Normal);
		//	pos.xyz += a * pos.x ;
	pos =	o						+ vy * pos.y	+ vz * pos.z;
	//	COMPUTE DEFORMED NORMAL
	nor = 			vx * nor.x		+ vy * nor.y	+ vz * nor.z;
}


void get_point_normal_morph_target_2( out vec4 vertex, out vec3 normal, in float f1, in float f2 )
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
}
void	deform_catmull( inout vec3 pos, inout vec3 nor )
{
	//	deformation, maa's cooking
	float x = pos.x;

	vec3 a;
	vec3 o;
	float inter;
	//	DEFORMATION sur 6 pts
#if 0
	if( x < -1. )
	{
		inter = x + 2.;
		o = mix( aaa_vu_vec4[0].xyz, aaa_vu_vec4[1].xyz, inter );
		a = aaa_vu_vec4[1].xyz - aaa_vu_vec4[0].xyz;
	}
	else if( x < 0. )
	{
		inter = x + 1.;
		o = mix( aaa_vu_vec4[1].xyz, aaa_vu_vec4[2].xyz, inter );
		a = aaa_vu_vec4[2].xyz - aaa_vu_vec4[1].xyz;
	}
	else if( x < 1. )
	{
		inter = x;
		o = mix( aaa_vu_vec4[2].xyz, aaa_vu_vec4[3].xyz, inter );
		a = aaa_vu_vec4[3].xyz - aaa_vu_vec4[2].xyz;
	}
	else if( x < 2. )
	{
		inter = x - 1.;
		o = mix( aaa_vu_vec4[3].xyz, aaa_vu_vec4[4].xyz, inter );
		a = aaa_vu_vec4[4].xyz - aaa_vu_vec4[3].xyz;
	}
	else if( x < 3. )
	{
		inter = x - 2.;
		o = aaa_vu_vec4[4].xyz *(1-inter) + aaa_vu_vec4[5].xyz * inter;
		a = aaa_vu_vec4[5].xyz - aaa_vu_vec4[4].xyz;
	}
	a = normalize( a );
#else
	int	index;
	float dx = aaa_vu_float[0];
	if( x < 0 )
	{
		o = catmull_rom_both(	a, 0, (dx - aaa_vu_float[2]) * aaa_vu_float[1] );
		a = normalize( a );
		o += a * x ;
	}
	else
	{
		x += dx - aaa_vu_float[2];
		if( x < dx )		index = 0;
		else if( x < 2*dx )	index = 1;
		else				index = 2;	// if (x < 1.5)

		inter = (x - index*dx) * aaa_vu_float[1];
		//	if( index >= 0 )
		o = catmull_rom_both(	a, index, inter );
		a = normalize( a );
	}
#endif
	//	BUILD COOR SYSTEM VECTOR a b c
	vec3 b = normalize( cross( a, vec3(0.,1.,0.) ) );
	vec3 c = cross( b, a );
	//	mat3 basis = mat3( a, b, c );

	//	COMPUTE DEFORMED POINT
		// Transform the normal vector and positions:
		//	vNormal = NormalMatrix * (basis * Normal);
		//	pos.xyz += a * pos.x ;
	pos = o + b * pos.z + c * pos.y ;
	//	COMPUTE DEFORMED NORMAL
	nor = a * nor.x + b * nor.z + c * nor.y ;
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
			tex_coor = vec4( r.xy, 1.0, 1.0 );
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
// the head of the fish is turn toward x neg
//	x positif is the body with the tall at the end
void main()
{
	vec4	vertex;
	vec3	normal;

//so we get the data morphed
	get_point_normal_morph_target_2( vertex, normal, aaa_vu_vec4[0].w, aaa_vu_vec4[1].w );

//and deform the tall now
	float x = vertex.x - TAIL_BEGIN;
	if( x > 0 )
	{
		//	if( c  < 0. )	c = - pow(-c,.6);	else	c  = pow(c,.6);
//		float angle = x * sin( TAIL_PHASE + x ) * TAIL_AMP;
		//float angle = x * x * sin( TAIL_PHASE + x ) * TAIL_AMP;
		float angle = x * sin( TAIL_PHASE ) * TAIL_AMP;
		//float angle = TAIL_PHASE ;
		//angle = 3.1416 / 4.;
		//angle = 0 ;
		float c = cos( angle );
		float s = sin( angle );
		mat2 m = mat2( c, -s, s, c );
		if( aaa_vu_int[3] > 0 )
		{	//	tail move verticaly
			vertex.xy = m * vec2( x, vertex.y );
			normal.xy = m * normal.xy;
		}
		else
		{	//	tail move horizontaly
			vertex.xz = m * vec2( x, vertex.z );
			normal.xz = m * normal.xz;
		}
		vertex.x += TAIL_BEGIN;
	}
	if( aaa_vu_vec4[2].w > 0 )
		vertex.xyz = mix( vertex.xyz, normalize( vertex.xyz ) * aaa_vu_vec4[3].w, aaa_vu_vec4[2].w );

//	DEFORM
	if( aaa_vu_int[0] < 1 )
	{
	// scale (should be down only)
		vertex.xyz *= SIZE_FACTOR;
		deform_catmull_v2( vertex.xyz, normal );
	}
	vertex.xyz += aaa_vu_vec4[6].xyz;

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