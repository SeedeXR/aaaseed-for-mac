
// Uniform variables for texturing
uniform sampler2D	aaa_tex2d[4];



vec4 maadiff( vec3 a, vec3 b )
{
vec3	d;
float	g;
vec4	ret;

	d = a-b;

	d = abs( d);

	g = gray(d) - aaa_fu_float[2];

	if( g<=0. )
		g = 0.;
	else
		g *= aaa_fu_float[3];

	ret.x = g;
	ret.y = g;
	ret.z = g;
	ret.w = 1.;

	return ret;
}

void main()
{
vec4	src1_color;
vec4	src2_color;

	src1_color = texture2D( aaa_tex2d[0], gl_TexCoord[0].st);
	src2_color = texture2D( aaa_tex2d[1], gl_TexCoord[0].st);

	gl_FragColor =  (src1_color * aaa_fu_float[0]) + (maadiff( src1_color.xyz, src2_color.xyz ) * aaa_fu_float[1]);
}

