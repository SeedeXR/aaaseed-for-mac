//#version 330 compatibility

out VS_out
{
	vec4			color;
} vs_out;

//	AAAseed uniform vertex shader
//uniform float	aaa_vu_float[8];	// [0, 2] factors - [3, 7] unused
//uniform vec4	aaa_vu_vec4[8];		// [0, 5] catmul rom control points - [6, 7] unused.

//	but we tried to optimzed this way
const vec3	f1d = vec3( 	-1.,	4.,		-3.	) * .5;
const vec3	f2d = vec3( 	 0.,	-10.,	9.	) * .5;
const vec3	f3d = vec3( 	 1.,	8.,		-9.	) * .5;
const vec3	f4d = vec3( 	 0.,	-2.,	3.	) * .5;

const vec4	f1f = vec4(		0.,		-1.,	2.,		-1.	) * .5;
const vec4	f2f = vec4(		2.,		0.,		-5.,	3.	) * .5;
const vec4	f3f = vec4(		0.,		1.,		4.,		-3.	) * .5;
const vec4	f4f = vec4(		0.,		0.,		-1.,	1.	) * .5;

vec3	cvCatmullRom_both( out vec3 tgn, const in int i, const in float t )
{
#if	CATMULL
	const float	t2 = t * t;
	vec4	vt = vec4( 1, t, t2, t*t2 );
	vec3	pos;
	vec3	v;
	v = aaa_vu_vec4[i].xyz;
	tgn =	dot( vt.xyz,	f1d ) 	* v;
	pos =	dot( vt,		f1f )	* v;
	v = aaa_vu_vec4[i+1].xyz;
	tgn +=	dot( vt.xyz,	f2d )	* v;
	pos +=	dot( vt,		f2f )	* v;
	v = aaa_vu_vec4[i+2].xyz;
	tgn +=	dot( vt.xyz,	f3d )	* v;
	pos +=	dot( vt,		f3f )	* v;
	v = aaa_vu_vec4[i+3].xyz;
	tgn +=	dot( vt.xyz,	f4d )	* v;
	pos +=	dot( vt,		f4f )	* v;
	return pos;
#else
	tgn = aaa_vu_vec4[i+2].xyz - aaa_vu_vec4[i+1].xyz;
	return	(1.-t)*aaa_vu_vec4[i+1].xyz + t*aaa_vu_vec4[i+2].xyz;
#endif
}

void	deform_catmull( inout vec3 pos, inout vec3 nor )
{
	//	deformation, maa's cooking
	float x = pos.x;
	vec3 vx;
	vec3 o;

	//	DEFORMATION sur 6 pts
#if 0
	if( x < -1. )
	{
		float inter = x + 2.;
		o = mix( aaa_vu_vec4[0].xyz, aaa_vu_vec4[1].xyz, inter );
		vx = aaa_vu_vec4[1].xyz - aaa_vu_vec4[0].xyz;
	}
	else if( x < 0. )
	{
		float inter = x + 1.;
		o = mix( aaa_vu_vec4[1].xyz, aaa_vu_vec4[2].xyz, inter );
		vx = aaa_vu_vec4[2].xyz - aaa_vu_vec4[1].xyz;
	}
	else if( x < 1. )
	{
		float inter = x;
		o = mix( aaa_vu_vec4[2].xyz, aaa_vu_vec4[3].xyz, inter );
		vx = aaa_vu_vec4[3].xyz - aaa_vu_vec4[2].xyz;
	}
	else if( x < 2. )
	{
		float inter = x - 1.;
		o = mix( aaa_vu_vec4[3].xyz, aaa_vu_vec4[4].xyz, inter );
		vx = aaa_vu_vec4[4].xyz - aaa_vu_vec4[3].xyz;
	}
	else if( x < 3. )
	{
		float inter = x - 2.;
		o = aaa_vu_vec4[4].xyz *(1-inter) + aaa_vu_vec4[5].xyz * inter;
		vx = aaa_vu_vec4[5].xyz - aaa_vu_vec4[4].xyz;
	}
	vx = normalize( vx );
#else
	int	index;
	float dx = aaa_vu_float[0];
	if( x < 0 )
	{
		o = cvCatmullRom_both(	vx, 0, (dx - aaa_vu_float[2]) * aaa_vu_float[1] );
		vx = normalize( vx );
		o += vx * x ;
	}
	else
	{
		x += dx - aaa_vu_float[2];

		if		( x < dx )		index = 0;
		else if	( x < 2*dx )	index = 1;
		else					index = 2;

		float inter = (x - index*dx) * aaa_vu_float[1];
		//	if( index >= 0 )
		o = cvCatmullRom_both(	vx, index, inter );
		vx = normalize( vx );
	}
#endif
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

// --------------------------------------------------------------------------
// MAIN FUNCTION

void main()
{
	vec4	vertex;
	vec3	normal;

	//get_point_normal_morph_target_4( vertex, normal, aaa_vu_vec4[0].w, aaa_vu_vec4[1].w, aaa_vu_vec4[2].w, aaa_vu_vec4[3].w );
	//get_point_normal_morph_target_1( vertex, normal, aaa_vu_vec4[0].w);
	//get_point_normal_morph_target_2( vertex, normal, aaa_vu_vec4[0].w, aaa_vu_vec4[1].w );
	vertex = gl_Vertex;
	normal = gl_Normal;


//	DEFORM
//	if( aaa_vu_int[0] >= 0. )
		deform_catmull( vertex.xyz, normal );


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
//	normal = fnormal( normal );

//	vec3 ecPosition3 = (vec3 (eye_coor_position)) / eye_coor_position.w;

//	COMPUTE LIGHT
	vec4	color = aaa_vu_vec4[7];
	gl_FrontColor = color;
	vs_out.color = color;
}