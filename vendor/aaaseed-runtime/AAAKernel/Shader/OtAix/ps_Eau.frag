
// --------------------------------------------------------------------------
// IN, OUT, INITIALIZATION
#define TEX_COOR_DIM 2
#define TEX_COOR_VEC vec2
in VS_out
{
	TEX_COOR_VEC	tex_impli;
	vec4			color;
	vec3			normal;
} fs_in;

//	output data
out vec4	fragColor;
//	AAASeed uniform implicit
uniform sampler2D	aaa_tex2d[4];



// --------------------------------------------------------------------------
// MAIN FUNCTION

void main (void)
{
	vec4	tex;
	vec4	eau;
	vec4	img;

	vec2	coi = fs_in.tex_impli.st;
	vec2	co = vec2( 1., 1. ) - coi;

	if( aaa_fu_int[0] == -4 )
	{	//	see water
		tex = texture( aaa_tex2d[1], co );
		tex.b = 0.5 ;
		tex.a = 1.;
	}
	else 		if( aaa_fu_int[0] == -3 )
	{	//	see water
		tex = texture( aaa_tex2d[1], co );
		tex.r = tex.b ;
		tex.g = tex.b ;
		tex.a = 1.;
	}
	else 	if( aaa_fu_int[0] == -2 )
	{	//	see water
		tex = texture( aaa_tex2d[1], co );
		//tex.b = 0.5;
		tex.a = 1.;
	}
	else if( aaa_fu_int[0] == -1 )
	{	//	see inmge
		tex = texture( aaa_tex2d[0], coi );
		tex.a = 1.;
	}
	else
	{
		vec2	coi = fs_in.tex_impli.st;
		vec2	co = vec2( 1., 1. ) - coi;

		eau = texture( aaa_tex2d[1], co );

		img = texture( aaa_tex2d[0], coi );
		float inf = img.a;
		eau.r -= .50196078;
		eau.g -= .50196078;
		inf *= ((eau.b - .50196078) < 0.) ? (.50196078 - eau.b) : (eau.b - .50196078);
		coi.s += ( eau.r ) * aaa_fu_float[0] * inf;
		coi.t += ( eau.g ) * aaa_fu_float[0] * inf;
		img = texture( aaa_tex2d[0], coi );

		float f = dot( vec2( 0., -1. ), eau.rg );

		img.rgb *= 1. + f * aaa_fu_float[1] * inf;

		img.a = 1.;

		//tex = (eau-.5) + img;
		tex = img;
		tex.a = 1.;
	}

	fragColor = tex;
}

/*
// Uniform variables for texturing
uniform sampler2D	aaa_tex2d[4];

in vec3 transformedNormal;

#define FACTOR	-.1
#define FACTOR_SPHERE	.45

void main()
{
	vec2 	c;
	//c = gl_TexCoord[0].st;
	c = transformedNormal.xy * FACTOR_SPHERE + .5;
	vec4	src1 = texture2D( aaa_tex2d[0], c );

	c = gl_TexCoord[1].st;
	float x = c.x -.5;
	float y = c.y -.5;

	float si = sin( aaa_fu_float[3] );
	float co = cos( aaa_fu_float[3] );

	//rotation
	c.x = x * co + y * si;
	c.y = - x * si + y * co;

	//deform using normal
	c += transformedNormal.xy * aaa_fu_float[2];

	//map like a circle on the texture
	c.x = c.x * aaa_fu_float[0] + .5;
	c.y = c.y * aaa_fu_float[1] + .5;
	vec4	src2 = texture2D( aaa_tex2d[1], c );

//ati	vec3 nor = { 0., 0., 1. };
	float gr = dot( transformedNormal, vec3( 0., 0., 1. ) );
//	src1.r = abs( transformedNormal.x );
//	src1.g = abs( transformedNormal.y );
//	src1.b = gr;


	vec4 dst;
	dst.xyz = mix( src1.xyz*.8, src2.xyz, gr );
//	dst.rgb = mix( src1.rgb*.8, src2.rgb, 1. - (1.-gr) * (1.-gr) );
//	dst.rgb = mix( src1.rgb*.8, src2.rgb, 0. );

	dst.a = max( (1.-gr), src2.a );

	vec3 n = transformedNormal;
	vec3 norb = vec3( .387, .387, .7 );
	gr = dot( n, norb );
	gr = gr * 5.6  - 4.;
	if ( gr > 0. )
	{

		dst.x = max( dst.x, gr );
		dst.y = max( dst.y, gr );
		dst.z = max( dst.z, gr );
		//dst.g += gr;
		//dst.b += gr;
		dst.a = max( dst.a, gr );
	}

	gl_FragColor = dst ;
}
*/