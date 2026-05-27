// GLSL fragment shader
// Apply texture

//uniform sampler2D	 aaa_tex2d[4];
//uniform float		aaa_fu_float[6];
//	0	:	color_factor
//in		float		sel_x;		//	this to pass to the fragment shader
//in		float		sel_y;		//	this to pass to the fragment shader
//in		float	aaa_att_w;		//	this to pass to the fragment shader

in float aaa_att_w;

void main(void)
{
	//if( aaa_att_w <= 0. )
		//discard;

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
//	tex.s = sel_x + tex.s * .25 ;
//	tex.t = sel_y + tex.t * .25;

	//vec4 texture = texture2D( aaa_tex2d[0], tex.st );
	//gl_FragColor = gl_Color * blend( texture );
	//texture.w = 1.;
	//gl_FragColor = texture;

	tex -= .5;
	float d = dot( tex, tex );
	if( d >.25 )
		discard;

	d = 1. - d * 4. ;	//	we should do a square at some point
	d = pow( d, aaa_fu_float[0] );

	d *= 1. - aaa_att_w;
	d = clamp( d, 0., 1. );

	gl_FragColor = vec4( d,d,d, 1. ) * gl_Color;
}
