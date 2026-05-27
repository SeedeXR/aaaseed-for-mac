
// Uniform variables for texturing
uniform sampler2D	aaa_tex2d[4];



void main()
{
vec4	src;
vec4	dst;
float	g;
float	a;

	src = texture2D( aaa_tex2d[0], gl_TexCoord[0].st);

	g = gray( src.xyz );

	if ( g < aaa_fu_float[2] )
		{
		discard;
		}
	else
		{
		a = (g-aaa_fu_float[2]) / (aaa_fu_float[3]-aaa_fu_float[2]) ;
		dst.a = aaa_fu_float[4] + pow( a, aaa_fu_float[5] );

		dst.xyz += src.xyz * aaa_fu_float[1];

		gl_FragColor = dst;
		}

}

