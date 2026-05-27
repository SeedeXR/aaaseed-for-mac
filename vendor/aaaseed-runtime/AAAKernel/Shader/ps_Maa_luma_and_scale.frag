
// Uniform variables for texturing
uniform sampler2D  aaa_tex2d[4];



vec4 doit( vec4 a )
{
float	g;
vec4	ret;
float	factor;

	g = gray(a.xyz);

	if( g <= aaa_fu_float[2] )
		ret.x = ret.y = ret.z = 0.;
	else
		{
		g -= aaa_fu_float[2];
		g /= 1. - aaa_fu_float[2];
		g *= aaa_fu_float[3] ;
		ret.x = ret.y = ret.z = g ;
		}

	ret.a = a.a;
	return ret;
}

void main()
{
vec4	src;
vec4	dst;

	src = texture2D( aaa_tex2d[0], gl_TexCoord[0].st);

	dst.xyz =  src.xyz * aaa_fu_float[0];
	dst.xyz += doit( src ).xyz * aaa_fu_float[1];
	dst.a = src.a;
	gl_FragColor = dst;
}

