// GLSL fragment shader
// Apply texture

uniform sampler2D	 aaa_tex2d[4];
//uniform float	aaa_fu_float[6];
//	0	:	color_factor
in		float		sel_x;		//	this to pass to the fragment shader
in		float		sel_y;		//	this to pass to the fragment shader
in		float	aaa_att_w;		//	this to pass to the fragment shader

void main(void)
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
	tex.s = sel_x + tex.s * .25 ;
	tex.t = sel_y + tex.t * .25;

	vec4 texture = texture2D( aaa_tex2d[0], tex.st );
	//gl_FragColor = gl_Color * blend( texture );
	texture.w *= aaa_att_w * aaa_fu_float[0];
	gl_FragColor = texture;
}
