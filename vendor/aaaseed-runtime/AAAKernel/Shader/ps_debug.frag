// GLSL fragment shader for testing texture swizzle
// for Aaaseed
// Author : Franz Hildgen


// Uniform variables for texturing
uniform sampler2D aaa_samp1;
//uniform sampler2D aaa_samp1;
//uniform sampler2D  aaa_tex2d[4];
uniform int   aaa_fu_int[];

void main (void)
{
	// Monochrome cloud cover value will be in clouds.r
	// Gloss value will be in clouds.g
	// clouds.b will be unused

	vec4 col = texture2D( aaa_samp1, gl_TexCoord[0].st );

	float g;
	if( aaa_fu_int[0] == 0 )        g = col.r;
	else if( aaa_fu_int[0] == 1 )   g = col.g;
	else if( aaa_fu_int[0] == 2 )   g = col.b;
	else if( aaa_fu_int[0] == 3 )   g = col.a;
	else                            g = -1;

	if( g>=0. )
		col = vec4( g, g, g, 1.0 );

	if( aaa_fu_int[1] == 0 )
		col = blend( col );

	gl_FragColor = col;
}