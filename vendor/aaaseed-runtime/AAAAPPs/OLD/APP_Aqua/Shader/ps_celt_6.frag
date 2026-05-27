
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
	float			z_for_fog;
	vec3			normal;
	float			fall_top;
} fs_in;

//	output
out	vec4 fragColor;

//	AAASeed uniform implicit
uniform	int			aaa_tex_dim[4];
uniform	sampler2D	aaa_tex2d[9];

//#include	"aqua.glsl"

// --------------------------------------------------------------------------
// MAIN  FUNCTION

vec4 l_get_tex_rgba( in int unit, in vec2 coor )
{
	if( aaa_tex_dim[unit] == 2 )
		return texture( aaa_tex2d[unit], coor );
	return vec4( 0, 1, 0, 1 );
}
vec4 l_get_tex_rgb1( in int unit, in vec2 coor )
{
	if( aaa_tex_dim[unit] == 2 )
		return vec4( texture( aaa_tex2d[unit], coor ).xyz, 1. );
	return vec4( 0, 1, 0, 1 );
}
float l_get_tex_caustic_3( in vec2 coor )
{
	if( aaa_tex_dim[3] == 2 )
		//this because grey texture in alpha
		return texture( aaa_tex2d[3], coor ).r;
	return 0.;
}
float	l_get_fall_off( in float f )
{
	if( f < 0. )
		f = 0.;
	#if 0
		f = 1. - f;
		f *= 1.1;
		if( f > 1. )
			f -=  (f - 1.) * 10.;
	#endif
	f = f * f;
	return f;
}

void main (void)
{
	//	discard;
	vec4 tex;

	// --------------------------
	// AJOUT DIFFUS TEXTURE
	vec4 colorDiffus = aaa_fu_float[0] * l_get_tex_rgba( 0, fs_in.tex_impli.st );

	// --------------------------
	// AJOUT LUMIERE
//	tex = (1 - aaa_fu_float[3]) * colorDiffus + aaa_fu_float[3] * blend( colorDiffus );
	tex = (1 - aaa_fu_float[3]) * colorDiffus + aaa_fu_float[3] * colorDiffus * fs_in.color;

	// --------------------------
	// AJOUT REFLEXION
	vec4 colorReflect = l_get_tex_rgb1( 2, fs_in.tex_refle.st );
	vec4 colorMask = l_get_tex_rgba( 0, fs_in.tex_impli.st );
	float f = 1 - l_get_fall_off( fs_in.fall_off );
	tex = 1.0*tex + aaa_fu_float[1]*f*f*f*f*colorReflect*colorMask.r;

	// --------------------------
	// AJOUT CAUSTIC
	float fn = fs_in.fall_top;
	if( fn > 0. )
	{
		float colorCaustic = l_get_tex_caustic_3( fs_in.tex_objec.st );
		fn = clamp( fn, 0.0, 1.0 );
		float r = colorCaustic * fn;
		tex.xyz += aaa_fu_float[2] * vec3( r, r, r*.2) * colorMask.r;
	}

	tex.a = colorDiffus.a;

	// --------------------------
	// AJOUT BROUILLARD
	if( aaa_fu_int[0] > 0 )
	{
		//float fog = get_fog( fs_in.z_for_fog );
		//tex = vec4( fog, fog, fog, 1.);
		tex = compute_fog4( tex, -fs_in.z_for_fog );
	}


//	tex = aqua_compute_fog4( tex, fs_in.z_for_fog );

	fragColor = tex;
//	fragColor = vec4(1);
}
