
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
	vec4	tex;
//	vec4	eau;
//	vec4	img;


//aaa_fu_vec4[0]
	vec2	coi = fs_in.tex_impli.st;

	vec2 coi2 = vec2( ( aaa_fu_vec4[0].z - aaa_fu_vec4[0].x ) * coi.s +  aaa_fu_vec4[0].x, ( aaa_fu_vec4[0].w - aaa_fu_vec4[0].y ) * coi.t +  aaa_fu_vec4[0].y );

	tex = texture( aaa_tex2d[0], coi2 );

	fragColor = tex;
	fragColor.a = 1.0;
	//fragColor = vec4( 1.0, 0.5, 0.5, 0.5 );
	//fragColor = tex + tex2; //mix(tex, tex2, 1.0);
	//fragColor =  vec4( 1.0, 1.0, 1.0, 1.0 ); //x;
//	fragColor.a = 0.0;
//	if( tex.r > 0.5 )
//		fragColor = vec4( 1.0, .0, .0, 1.0 );

	fragColor.a = smoothstep(	0.45, 0.5, tex.r );
//	fragColor.rgb = smoothstep(	0.45, 0.5, tex.rgb );
	// if( (tex.r > 0.5) && (tex.r < 0.55) )
	// 	fragColor.rgb = vec3( 1.0,.0, .0);
	// else if( tex.r > 0.55 )
	// 	fragColor.rgb = vec3( .0,.0, 1.0);




			//fragColor.a = 1.0; //tex.r;
//	fragColor = vec4( 1.0, .0, .0, 1.0 );

}
