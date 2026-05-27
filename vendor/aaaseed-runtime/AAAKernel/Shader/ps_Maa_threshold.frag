
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
	g = aaa_fu_float[2] + g * (aaa_fu_float[3] - aaa_fu_float[2]);

	if ( g < aaa_fu_float[4] )
		a = 0.;
	else if ( g > aaa_fu_float[5] )
		a = 1.;
	else
		a = (g-aaa_fu_float[4])/(aaa_fu_float[5]-aaa_fu_float[4]);
	dst.xyz +=  src.xyz * aaa_fu_float[1];
	dst.a = a;

	gl_FragColor = dst;
}

