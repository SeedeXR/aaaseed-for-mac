// GLSL fragment shader
// Apply texture

uniform sampler2D	 aaa_tex2d[4];
//uniform vec4	aaa_fu_vec4[];
//	0	:	color_factor
in		float		sel_x;		//	this to pass to the fragment shader
in		float		sel_y;		//	this to pass to the fragment shader
in		float	aaa_att_w;		//	this to pass to the fragment shader

void main()
{
	if( aaa_att_w <= 0. )
		discard;

	vec2  tex = gl_TexCoord[0].st;
/*
	int sel;
	sel = int(aaa_att_x * 4.);
	sel = 2;
	tex.s = .25 * float(sel) + .01 + (tex.s)*.23 ;
	sel = int(aaa_att_y * 3.);
	sel = 1;
	tex.t = .25 * float(sel) + .01 + (tex.t)*.23;
*/
	vec4 texture;
	if( false )
	{
		tex.s = sel_x + tex.s * .25 ;
		tex.t = sel_y + tex.t * .25;

		texture = texture2D( aaa_tex2d[0], tex.st );
	}
	else
	{
		tex -= .5;
		tex *= 2.;
		float d = tex.s * tex.s + tex.t * tex.t;
		if( d > 1. )
			discard;
		if( aaa_fu_float[1]!=0. )
			d += (1. - cos( atan( tex.t, tex.s ) * aaa_fu_float[2] + sel_x * 6.2432 ) ) * aaa_fu_float[1];
		d = 1. - pow( d, aaa_fu_float[0]*aaa_fu_float[0] );
		texture.w = d;
		//texture.xyz = vec3( 1. );
		texture.xyz = gl_Color.xyz;
	}

	//gl_FragColor = gl_Color * blend( texture );
	texture.xyz *= aaa_fu_vec4[0].xyz;
	texture.w *= aaa_att_w * aaa_fu_vec4[0].w;
	gl_FragColor = texture;
}
