
// Uniform variables for texturing
uniform sampler2D  aaa_tex2d[0];




void main (void)
{
//int	i;
vec4	tmp = vec4(0.0);
vec4	c;
float	alpha;

float NPixels = aaa_fu_float[0];
float Threshold = aaa_fu_float[1];

ivec2	tex_suv = textureSize( aaa_tex2d[0], 0 );
float	dx = NPixels/tex_suv.x;
float 	dy = NPixels/tex_suv.y;
float	d2x = 2.0*dx;
float 	d2y = 2.0*dy;

	c = 24. * texture2D( aaa_tex2d[0], gl_TexCoord[0].xy);
 //todoopt use swizzle
	c -= texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2( -d2x, -d2y) );
	c -= texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2( -dx, -d2y) );
	c -= texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2( 0, -d2y) );
	c -= texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2( dx, -d2y) );
	c -= texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2( d2x, -d2y) );

	c -= texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2( -d2x, -dy) );
	c -= texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2( -dx, -dy) );
	c -= texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2( 0, -dy) );
	c -= texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2( dx, -dy) );
	c -= texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2( d2x, -dy) );

	c -= texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2( -d2x, 0.) );
	c -= texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2( -dx, -0.) );
	c -= texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2( dx, 0.) );
	c -= texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2( d2x, 0.) );

	c -= texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2( -d2x, dy) );
	c -= texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2( -dx, dy) );
	c -= texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2( 0, dy) );
	c -= texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2( dx, dy) );
	c -= texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2( d2x, dy) );

	c -= texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2( -d2x, d2y) );
	c -= texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2( -dx, d2y) );
	c -= texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2( 0, d2y) );
	c -= texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2( dx, d2y) );
	c -= texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2( d2x, d2y) );


	c.xyz = abs(c.xyz);

float g = gray(c);

	if ( g > Threshold)
	{
		c = texture2D( aaa_tex2d[0], gl_TexCoord[0].st );
//		discard; //color = vec4(0.0,0.0,0.0,0.25);
	}
	else
	{
		discard;
//		if( dist > (Threshold * 0.5) )
//			alpha = 1.0 - smoothstep( Threshold * 0.5, Threshold, dist );
//		else
//			alpha = 1.0;
	}
//	color.rgb = texture2D( aaa_tex2d[0], gl_TexCoord[0].st).rgb;
//	color.a = alpha;
//	gl_FragColor = blend( color ); //(texture2D( aaa_tex2d[0], gl_TexCoord[0].st).rgb, 1-smoothstep( Threshold/2, Threshold, dist) );
	gl_FragColor = c;
}
