
// --------------------------------------------------------------------------
// IN, OUT, INITIALIZATION
#define TEX_COOR_DIM 2
#define TEX_COOR_VEC vec2
in VS_out
{
	TEX_COOR_VEC	tex_impli;
	vec4			color;
	vec4 			vertex;
	vec3			normal;
	vec4			tex_coord;
	float			fall_off;
} fs_in;

//	output data
out vec4	fragColor;
//	AAASeed uniform implicit
uniform sampler2D	aaa_tex2d[4];



// --------------------------------------------------------------------------
// MAIN FUNCTION

void main (void)
{
	//vec4	tex;

	//vec2	coi = fs_in.tex_impli.st;

	fragColor = texture( aaa_tex2d[0], fs_in.tex_impli.st );
	//fragColor = vec4( tex.r, tex.g, tex.b , tex.a );


}
