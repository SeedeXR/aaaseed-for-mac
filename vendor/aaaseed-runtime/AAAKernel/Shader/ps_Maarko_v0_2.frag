
// Uniform variables for texturing
uniform sampler2D	aaa_tex2d[4];

uniform float	aaa_fu_out;	//	quantity of the computed texture from the shader

void main()
{
vec4	src;
vec4	dst;
float	g;
float	a;

	src = texture2D( aaa_tex2d[0], gl_TexCoord[0].st);

	if( aaa_fu_int[1] == 1 )
	{
		src.x = 1 - src.x;
		src.y = 1 - src.y;
		src.z = 1 - src.z;
	}
	if( aaa_fu_int[0] == 0 )
	{	//	use the max of r g b
		g = src.x;
		if( g < src.y )
			g = src.y;
		if( g < src.z )
			g = src.z;
	}
	else
		g = gray( src.xyz );

	if ( g < aaa_fu_float[2] )
	{
		discard;
	}
	else
	{
		a = (g-aaa_fu_float[0]) / (aaa_fu_float[1]-aaa_fu_float[0]) ;
		dst.a = aaa_fu_float[2] + pow( a, aaa_fu_float[3] );

		dst.xyz += src.xyz * aaa_fu_out;

		gl_FragColor = dst;
	}

}

