

// -------------------------------------------------------------------------
// IN, OUT, INITIALIZATION
#define TEX_COOR_DIM 2
#define TEX_COOR_VEC vec2
in VS_out
{
	TEX_COOR_VEC	tex_impli;
	TEX_COOR_VEC	tex_refle;
	vec4			color;
	float			fall_off;
	float			z_to_eye;
	vec3			normal;
} fs_in;

//	output data
out vec4	fragColor;
//	AAASeed uniform implicit
uniform sampler2D	aaa_tex2d[4];



// --------------------------------------------------------------------------
// GET COLOR FUNCTION

vec4 l_get_tex_rgb1( in int unit )
{
	return vec4( texture( aaa_tex2d[unit], fs_in.tex_impli.st ).xyz, 1. );
}



// --------------------------------------------------------------------------
// MAIN FUNCTION

void main (void)
{

	vec4 tex;

	// val map ambiant occlusion
	vec4 colorDiffus = aaa_fu_float[0]*l_get_tex_rgb1( 0 );
	// val map masque
	vec4 colorMasque = l_get_tex_rgb1( 1 );
	// val map reflection
	vec4 colorRefle = l_get_tex_rgb1( 2 );

	// calcul FALLOFF
	float f = get_fall_off( fs_in.fall_off );
	float f2 = 0.0;
	if(f<aaa_fu_float[1]) f2 = aaa_fu_float[2] + (1 - aaa_fu_float[1] - 0.1)*(1-(1/aaa_fu_float[1])*f);
	else f2 = aaa_fu_float[2];

	// texture falloff avec masque
	tex = colorMasque.r * colorDiffus + ( 1 - colorMasque.r) * vec4(colorDiffus.r,colorDiffus.g,colorDiffus.b,f2);

	// ajout reflection douce
	tex *= aaa_fu_float[3]*colorRefle + (1 - aaa_fu_float[3]) * vec4(1,1,1,1);

	tex *= aaa_fu_float[4];


	fragColor = tex;
}
