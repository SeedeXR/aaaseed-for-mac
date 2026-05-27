
// --------------------------------------------------------------------------
// IN, OUT, INITIALIZATION

#define TEX_COOR_DIM 2
#define TEX_COOR_VEC vec2
in VS_out
{
	TEX_COOR_VEC	tex_impli;
	TEX_COOR_VEC	tex_refle;
//	TEX_COOR_VEC	tex_objec;
	vec4			color;
	float			fall_off;
	float			z_to_eye;
	vec3			normal;
//	float			fall_top;
} fs_in;

//	output
out vec4	fragColor;

//	AAASeed uniform implicit
uniform sampler2D	aaa_tex2d[4];

#include	"aqua.glsl"


// --------------------------------------------------------------------------
// MAIN FUNCTION

void main (void)
{
	vec4 tex;
	tex = vec4( 1, 1, 1, 1 );

	float fo = get_fall_off( fs_in.fall_off );
	tex.a = fo;
	tex.xyz = tex.xyz*0.001 + vec3(1.0,1.0,0.8);

	fragColor = tex;
}
