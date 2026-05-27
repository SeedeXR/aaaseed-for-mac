


// --------------------------------------------------------------------------
// VARIABLES

in VS_out
{
	float			grey;
	float			alpha;
} fs_in;

//	output
out		vec4	fragColor;

//	AAASeed uniform implicit
uniform	sampler2D	aaa_tex2d[4];



// --------------------------------------------------------------------------
// MAIN  FUNCTION

void main (void)
{
	float v = fs_in.grey;

	vec2 	tex_coor = mix( aaa_fu_vec4[0].xy, aaa_fu_vec4[0].zw, v );

	vec4	color = texture( aaa_tex2d[0], tex_coor ) * aaa_fu_float[0];
	color.a = fs_in.alpha;

//	vec4 color = vec4( v, v, v, 1. );

	fragColor = color;
}
