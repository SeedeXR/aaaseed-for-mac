
// Uniform variables for texturing
uniform sampler2D	aaa_tex2d[4];

//uniform float	aaa_fu_float[6];
//	0	display edge
//	1	nb_pixel detect
//	2	texture size x
//	3	texture size y
//	4	edge threshold

vec4 specExp = { 0.5, .0, .0, .0 };
const vec3 quant = vec3( 0.2125, 0.7154, 0.0721 );


vec3 sobel()
{
//vec4	tex = texture2D( aaa_tex2d[0], gl_TexCoord[0].st );

vec4	cx;
vec4	cy;
float NPixels = aaa_fu_float[1];
//float Threshold = aaa_fu_float[2] * aaa_fu_float[3];

float	dx = NPixels/aaa_fu_float[3];
float 	dy = NPixels/aaa_fu_float[4];
 //todoopt use swizzle
	cx =		texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2(	dx,		dy) );
	cx -=		texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2(	-dx,	-dy) );
	cy = cx;

	cx += 2.0 *	texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2(	0.0,	dy) );
	cx +=		texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2(	-dx,	dy) );
	cx -=		texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2(	dx,		-dy) );
	cx -= 2.0 *	texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2(	0.0,	-dy) );

	cy += 2.0 *	texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2(	dx,		0.0) );
	cy +=		texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2(	dx,		-dy) );
	cy -=		texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2(	-dx,	dy) );
	cy -= 2.0 *	texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2(	-dx,	0.0) );


	vec4 r1;
	r1 = cx * cx + cy * cy;

	r1.x = max( r1.x, r1.y );
	r1.x = max( r1.x, r1.z );

	vec3 color;
	color = pow( r1, specExp );


	return color;

}

vec3 sobel2()
{
	vec2 st = gl_TexCoord[0].st;
	vec3	color;

	vec2 stp0 = vec2( 1. / aaa_fu_float[0],	0. );
	vec2 st0p = vec2( 0.,  				1. / aaa_fu_float[3] );
	vec2 stpp = vec2( 1. / aaa_fu_float[2],	1. / aaa_fu_float[3] );
	vec2 stpm = vec2( 1. / aaa_fu_float[2],	-1. / aaa_fu_float[3] );
 //todoopt use swizzle
	float i00 =   dot( texture2D( aaa_tex2d[0], st ).rgb, quant );
	float im1m1 = dot( texture2D( aaa_tex2d[0], st-stpp ).rgb, quant );
	float ip1p1 = dot( texture2D( aaa_tex2d[0], st+stpp ).rgb, quant );
	float im1p1 = dot( texture2D( aaa_tex2d[0], st-stpm ).rgb, quant );
	float ip1m1 = dot( texture2D( aaa_tex2d[0], st+stpm ).rgb, quant );
	float im10 =  dot( texture2D( aaa_tex2d[0], st-stp0 ).rgb, quant );
	float ip10 =  dot( texture2D( aaa_tex2d[0], st+stp0 ).rgb, quant );
	float i0m1 =  dot( texture2D( aaa_tex2d[0], st-st0p ).rgb, quant );
	float i0p1 =  dot( texture2D( aaa_tex2d[0], st+st0p ).rgb, quant );
	float h = -1. * im1p1 - 2. * i0p1 - 1. * ip1p1  +  1. * im1m1 + 2. * i0m1 + 1. * ip1m1;
	float v = -1. * im1m1 - 2. * im10 - 1. * im1p1  +  1. * ip1m1 + 2. * ip10 + 1. * ip1p1;

	float mag = pow( h * h + v * v, 0.5 );
	if( mag > aaa_fu_float[4] )
		{
		color = vec3( 1, 1., 1 );
		}
	else
		{
		color = vec3( 0, 0.0,0.0 );
		}
	return color;
}

vec3 scanly( in vec3 sobel )
{
	vec4	tex = texture2D( aaa_tex2d[0], gl_TexCoord[0].st );


//normalize the color hue

//calculate [sum RGB]
	vec4	hue;
	hue.x = tex.x + tex.y + tex.z;

//calculate 1/[sum RGB]
	hue.y = 1 / hue.x;

//multiply pixel color with 1/[sum RGB]
	vec3 color;
	color = tex.xyz * hue.y;

//calculate the pixel intensity

// make everything with [sum RGB] <.25 black

// define darktones ([sum RGB] >.2) and store in channel z
	if( hue.x >= .25 )
		hue.z = 1.0;
	else
		hue.z = 0.0;

// darktones will be normalized HUE * 0.5
	hue.z *= 0.5;

// define midtones ([sum RGB] >.8) and store in channel y
	if( hue.x >= 0.8 )
		hue.y = 1.0;
	else
		hue.y = 0.0;

// midtones will be normalized HUE * (0.5+0.5)
	hue.y *= 0.5;

// define brighttones ([sum RGB] >1.5) and store in channel x
	if( hue.x >= 1.5 )
		hue.x = 1.0;
	else
		hue.x = 0.0;

// brighttones will be normalized HUE * (0.5+0.5+1.5)
	hue.x *= 1.5;

//# sum darktones+midtones+brighttones to calculate final pixel intensity
	hue.x = hue.x + hue.y + hue.z;

//multiply pixel color with rotoscope intensity
	color *= hue.x;

//generate edge mask -> make 1 for all regions not belonging to an edge
//use 0.8 as threshold for edge detection
	if( sobel.x < 0.8 )
		sobel.x = 1.0;
	else
		sobel.x = 0.0;

//blend white into edge artifacts (occuring in bright areas)
//find pixels with intensity>2.5
	if( hue.x >= 2.5 )
		sobel.y = 1.0;
	else
		sobel.y = 0.0;

//set the edge mask to 1 in such areas
//	sobel.x = max( sobel.x, sobel.y );

//	sobel.x = 1.0;
//multiply cleaned up edge mask with final color image
	color *= sobel.x;

//transfer image to output
//	color = vec3( 1.0, .0, .0 );
//	color = tex.xyz;
	return color;

}

void main()
{
	if( aaa_fu_float[0] >= 1 )
		{
		vec3 sob = sobel2();
		gl_FragColor.xyz = scanly( sob );
		}
	else
		{
		gl_FragColor.xyz = scanly( vec3( 0.0 ) );
		}
}