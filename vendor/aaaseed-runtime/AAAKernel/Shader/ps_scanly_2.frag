// Uniform variables for texturing
uniform sampler2D	aaa_tex2d[4];
//uniform float		aaa_fu_float[6];
//	0	quantize
//	1	tolerance
//	2	texture size x
//	3	texture size y

const vec3 quant = vec3( 0.2125, 0.7154, 0.0721 );
void main()
{
	vec2 st = gl_TexCoord[0].st;
	vec4 color = vec4( 1., 0., 0., 1. );
	vec3 rgb = texture2D( aaa_tex2d[0], st ).rgb;

	vec2 stp0 = vec2( 1. / aaa_fu_float[2],	0. );
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
	if( mag > aaa_fu_float[1] )
		{
		color = vec4( 0., 0., 0., 1. );
		}
	else
		{
		rgb.rgb *= aaa_fu_float[0];
		rgb.rgb += vec3( .5, .5, .5 );
		ivec3 irgb = ivec3( rgb.rgb );
		rgb.rgb = vec3( irgb ) / aaa_fu_float[0];
		color = vec4( rgb, 1. );
		}

	gl_FragColor = color;
}
