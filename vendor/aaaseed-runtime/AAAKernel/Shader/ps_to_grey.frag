
//	v 0.1		30 Octobre 2008		maa

// Uniform variables for texturing
uniform sampler2D	aaa_tex2d[4];

uniform float		aaa_fu_src;	//	quantity of the fixed path texture
uniform float		aaa_fu_out;	//	quantity of the computed texture from the shader

void main()
{
vec4	src;
vec4	dst;
float	g;

	src = texture2D( aaa_tex2d[0], gl_TexCoord[0].st );

	g = gray( src.xyz );
	g = aaa_fu_float[0] + g * ( aaa_fu_float[1] - aaa_fu_float[0] );
	g *= aaa_fu_out;

	dst.r = dst.g = dst.b = g;
	dst.a = src.a;
	dst.rgb +=  src.rgb * aaa_fu_src;
	gl_FragColor = blend( dst );
}

