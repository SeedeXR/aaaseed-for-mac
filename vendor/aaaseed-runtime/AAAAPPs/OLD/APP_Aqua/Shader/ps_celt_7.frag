
// --------------------------------------------------------------------------
// VARIABLES

#define TEX_COOR_DIM 2
#define TEX_COOR_VEC vec2
in VS_out
{
	TEX_COOR_VEC	tex_impli;
	TEX_COOR_VEC	tex_refle;
	TEX_COOR_VEC	tex_objec;
	vec4			color;
	float			fall_off;
	float			z_to_eye;
	vec3			normal;
} fs_in;

//	output
out vec4	fragColor;

//	AAASeed uniform implicit
uniform sampler2D	aaa_tex2d[4];


// --------------------------------------------------------------------------
// MAIN  FUNCTION

void main (void)
{
	//	discard;
	vec4 tex;

	// --------------------------
	// AJOUT DIFFUS TEXTURE
	vec4 colorDiffus = aaa_fu_float[0]*get_tex_rgba( 0, fs_in.tex_impli.st );
	//tex.a = 1;

	// --------------------------
	// AJOUT LUMIERE
	tex = (1 - aaa_fu_float[3])*colorDiffus + aaa_fu_float[3]*blend_fs( colorDiffus );

	// --------------------------
	// AJOUT REFLEXION
	vec4 colorMask = get_tex_rgba( 1, fs_in.tex_impli.st );
	vec4 colorReflect = get_tex_rgb1( 2, fs_in.tex_refle.st );

	float f = 1 - get_fall_off( fs_in.fall_off );
	tex = 1.0*tex + aaa_fu_float[1]*f*f*f*f*colorReflect*colorMask.r;

	// --------------------------
	// AJOUT CAUSTIC
	float colorCaustic = get_tex_caustic_3( fs_in.tex_objec.st );
	float fn = fs_in.normal.y;
	fn = clamp( fn, 0.0, 1.0 );
	float r = colorCaustic * fn;
	tex.xyz += aaa_fu_float[2] * vec3( r, r, r*.2)*colorMask.r;

	// --------------------------
	// AJOUT BROUILLARD
#if 0
	tex = compute_fog4( tex, fs_in.z_to_eye );
#endif

	tex.a = colorDiffus.a;
	fragColor = tex;
}
