
// --------------------------------------------------------------------------
// IN, OUT, INITIALIZATION

in VS_out
{
	TEX_COOR_VEC	tex_coor[TEX_UNIT_NB];
	vec4			color;
	vec3			normal;
	float			fall_off;
	float			z_to_eye;
} fs_in;
//	output data
out vec4	fragColor;
//	AAASeed uniform implicit
uniform sampler2D	aaa_tex2d[4];


// --------------------------------------------------------------------------
// MAIN FUNCTION

void main (void)
{

	vec4 tex;

	float f = get_fall_off( fs_in.fall_off );
	float f2 = 0;
	if(f<0.4) f2 = 1 - 2.5*f;
	else f2 = 0;

	tex.a = f2;
	tex.xyz =  vec3(1.0, 1.0, 1.0);

	fragColor = tex;
}
