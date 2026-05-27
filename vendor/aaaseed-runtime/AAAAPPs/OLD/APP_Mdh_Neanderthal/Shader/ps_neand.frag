

in VS_out
{
	vec2	uv;
	vec4	color;
	vec3	normal;
} fs_in;

//	output data
out		vec4		fragColor;
//	AAASeed uniform implicit
uniform sampler2D	aaa_tex2d[4];


// --------------------------------------------------------------------------
// GET COLOR FUNCTION

vec4 get_tex_rgb1( in int unit )
{
	return vec4( texture( aaa_tex2d[unit], fs_in.uv.st ).xyz, 1. );
}

// --------------------------------------------------------------------------
// MAIN FUNCTION

void main (void)
{
	// val map ambiant occlusion
	vec4 colorDiffus = aaa_fu_float[0] * get_tex_rgb1( 0 );

	fragColor = colorDiffus;
	//fragColor.xyz += fs_in.normal;
}
