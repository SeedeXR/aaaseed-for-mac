// Uniform variables for texturing
uniform sampler2D	aaa_tex2d[4];



void main (void)
{
vec4	cx;
vec4	cy;
vec4	color;
vec4	colorb;
float	alpha;

vec2 location = gl_TexCoord[0].st;

	color = texture2D( aaa_tex2d[0], location.st );
	location.s += (color.r - aaa_fu_vec4[0][0]) * aaa_fu_vec4[0][0];
	location.s += (color.g - aaa_fu_vec4[1][1]) * aaa_fu_vec4[0][1];
	location.s += (color.b - aaa_fu_vec4[1][2]) * aaa_fu_vec4[0][2];

	location.t += (color.r - aaa_fu_vec4[3][0]) * aaa_fu_vec4[2][0];
	location.t += (color.g - aaa_fu_vec4[3][1]) * aaa_fu_vec4[2][1];
	location.t += (color.b - aaa_fu_vec4[3][2]) * aaa_fu_vec4[2][2];

	colorb = texture2D( aaa_tex2d[0], location.st );
	color = color * aaa_fu_float[0] +  colorb * aaa_fu_float[1];
	color.a = 1.;

	gl_FragColor = color;
}