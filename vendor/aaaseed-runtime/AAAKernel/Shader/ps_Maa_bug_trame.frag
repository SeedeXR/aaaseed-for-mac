
// Uniform variables for texturing
uniform sampler2D	aaa_tex2d[4];



void main (void)
{
//int	i;
vec4	tmp = vec4(0.0);
vec4	c;
vec4	color;
float	alpha;

float NPixels = aaa_fu_float[2];
float Threshold = aaa_fu_float[3];

ivec2	tex_suv = textureSize( aaa_tex2d[0], 0 );
float	dx = NPixels/tex_suv.x;
float 	dy = NPixels/tex_suv.y;

	color = texture2D( aaa_tex2d[0], gl_TexCoord[0].xy);
	c = 4. * color;

	 //todoopt use swizzle
	c -= texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2( -dx, 0.) );
	c -= texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2( dx, 0.) );
	c -= texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2( 0., dy) );
	c -= texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2( 0.,-dy) );

	c = abs( c);
	c.a = 1.;

float g = gray(c);

	if ( g > Threshold)
		{
//		c = texture2D( aaa_tex2d[0], gl_TexCoord[0].st );
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
	gl_FragColor = color * aaa_fu_float[0] + c * aaa_fu_float[1];
}
