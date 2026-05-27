
// Uniform variables for texturing
uniform sampler2D	aaa_tex2d[4];

//uniform float	aaa_fu_float[6];
// 0 original image
// 1 line image
// 2 dist in pixel for detection
// 3 threshold
// 4 >.5 do an abs

void main (void)
{
	vec4	c;
	vec4	color;

	float threshold = aaa_fu_float[3];

	ivec2	tex_suv = textureSize( aaa_tex2d[0], 0 );
	float	dx = aaa_fu_float[2]/tex_suv.x;
	float 	dy = aaa_fu_float[2]/tex_suv.y;

	color = texture2D( aaa_tex2d[0], gl_TexCoord[0].xy);
	c = 8. * color;

 //todoopt use swizzle
	c -= texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2( -dx, 0.) );
	c -= texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2( -dx, dy) );
	c -= texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2( dx, 0.) );
	c -= texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2( dx, dy) );
	c -= texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2( 0., dy) );
	c -= texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2( -dx, dy) );
	c -= texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2( 0.,-dy) );
	c -= texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2( -dx,-dy) );

	if ( aaa_fu_float[4] > .5 )
		c = abs( c);

	float g = gray(c);

	if( g < threshold)
		discard;

	color = color * aaa_fu_float[0] + c * aaa_fu_float[1];
	color.a = 1.;
	gl_FragColor = color;
}
