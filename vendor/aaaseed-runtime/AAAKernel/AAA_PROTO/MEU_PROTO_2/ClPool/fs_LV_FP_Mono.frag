
layout(location = 0) out vec4 color_out_0;
layout(location = 1) out vec4 color_out_1;

//	0	:	color_factor
in		float	sel_x;		//	this to pass to the fragment shader
in		float	sel_y;		//	this to pass to the fragment shader
in		float	aaa_att_w;	//	this to pass to the fragment shader


void main()
{
	//if( aaa_att_w <= 0. )
	//	discard;

	vec2  tex = gl_TexCoord[0].st - .5;
    tex = vec2( tex.x*sel_x + tex.y*sel_y, tex.x*sel_y - tex.y*sel_x ) + .5;

	vec4 texture;
	texture = texture2D( aaa_tex2d[0], tex.st );

	if( texture.a <= .03 )
		discard;
	//texture.xyz = vec3    ( 1. );
	//texture.xyz = gl_Color.xyz;

	texture.a *= aaa_att_w * aaa_fu_vec4[0].a;
	if( texture.a <= .0003 )
		discard;
	//gl_FragColor = gl_Color * blend( texture );
	texture.rgb *= aaa_fu_vec4[0].rgb * gl_Color.rgb;
	color_out_0 = texture;

 //   float id = aaa_fu_int[1];
    //float r = id <<
    //color_out_1 = vec4( vec3( float( aaa_fu_int[1] )  * .09  ), 1.);
 //   if( id != 0. )
    color_out_1 = vec4( vec3( 0 ), 1. );
}
