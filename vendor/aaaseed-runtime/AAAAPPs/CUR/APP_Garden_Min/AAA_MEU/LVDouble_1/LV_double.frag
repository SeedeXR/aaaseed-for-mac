//#version 330 compatibility
//#extension GL_ARB_shading_language_include : enable

// GLSL fragment shader
// Apply texture

in VS_out
{
	vec4	color;
	vec2	tex_coor;
} fs_in;

//	AAASeed uniform implicit
uniform sampler2D	aaa_tex2d[4];


void main (void)
{
//	discard;
	vec2 uv = fs_in.tex_coor;
	if( aaa_fu_int[1] == 2 )
		//uv *= 4;
		uv.x = mod( uv.x, .5 )  + 	.25;
	else if( aaa_fu_int[1] == 3 )
		uv.x = mod( uv.x, .33333333333333333333333333 )  + .333333333333333333333333;
	vec4 texture_b = texture2D( aaa_tex2d[0], uv ) * aaa_fu_vec4[0].rgba;
	vec4 texture_f = texture2D( aaa_tex2d[1], uv ) * aaa_fu_vec4[1].rgba;

	vec4 texture;
	if( aaa_fu_int[0] > 0 )
		texture = mix( texture_f, texture_b, texture_b.a );
	else
		texture = mix( texture_b, texture_f, texture_f.a );
	gl_FragColor = vec4( texture.rgb, 1. ) * fs_in.color;
//	gl_FragColor = fs_in.color;
//	gl_FragColor.a = 1;
//	gl_FragColor = vec4(0,1,0,.5);

}
