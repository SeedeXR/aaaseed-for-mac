
// Uniform variables for texturing
uniform sampler2D aaa_tex2d[4];

//this the four color for the bezier spline
//uniform vec4	aaa_fu_vec4[6];

//	0	src color
//	1	fx color
//	2	grey offset
//	3	grey factor
//	4	phase factor of vertical sin
//	5	strenght of vertical sin
//uniform float	aaa_fu_float[6];

//	0	: interpolation 0=linear, 1=bezier
//uniform int		aaa_fu_int[6];

// assume "t" ranges from 0 to 1 safely
// brute-force this, it's running on the CPU
//todo
//	isolate for reuse
//	pass only the index
vec4 c_bezier(in vec4 c0, in vec4 c1, in vec4 c2, in vec4 c3, in float t)
{
float t2 = t*t;
float t3 = t2*t;
float nt = 1.0 - t;
float nt2 = nt*nt;
float nt3 = nt2 * nt;
	vec4 b = nt3*c0 + (3.0*t*nt2)*c1 + (3.0*t2*nt)*c2 + t3*c3;
	return b;
}

vec4 c_linear(in vec4 c0, in vec4 c1, in vec4 c2, in vec4 c3, in float t)
{
vec4	b;
	if( t<=0.33 )
		b = (.33-t)*3. * c0 + t*3.*c1;
	else if( t<=0.66 )
		b = (.66-t)*3. * c1 + (t-.33)*3.*c2;
	else
		b = (1.-t)*3. * c2 + (t-.66)*3.*c3;
	return b;
}

vec4 greyGradPS(in sampler2D texture, float inter)
{
vec4	dst_color;

	if( aaa_fu_int[0] == 0)
		dst_color = c_linear( aaa_fu_vec4[0], aaa_fu_vec4[1], aaa_fu_vec4[2], aaa_fu_vec4[3], inter );
	else //if( aaa_fu_int[0] == 0)
		dst_color = c_bezier( aaa_fu_vec4[0], aaa_fu_vec4[1], aaa_fu_vec4[2], aaa_fu_vec4[3], inter );

	return dst_color;
}

void main()
{
vec4	color;
vec2	coor;
vec4	src_color;

	coor = gl_TexCoord[0].st;
	coor.s += aaa_fu_float[5] * sin( aaa_fu_float[4] * coor.t  ) * .5;
	src_color = texture2D( aaa_tex2d[0], coor);

	float  inter = gray( src_color.xyz );
	inter =  aaa_fu_float[3] * ( inter + aaa_fu_float[2] );

	color = greyGradPS(aaa_tex2d[0], inter);
	gl_FragColor = color * aaa_fu_float[1] + src_color * aaa_fu_float[0];
}
