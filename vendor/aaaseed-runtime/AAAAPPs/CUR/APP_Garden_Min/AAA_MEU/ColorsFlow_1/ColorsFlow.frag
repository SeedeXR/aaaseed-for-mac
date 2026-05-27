
layout(location = 0) out vec4 color_out_0;
layout(location = 1) out vec4 color_out_1;

in ST_AAA_BV BV;

//aaa_fu_vec4[]; // color de 0 a 3, active ds 4.xyzw et 5.xy
uniform sampler2D   aaa_tex2d[4];

#define SEL     aaa_fu_int[0]
#define ALPHA   aaa_fu_float[0]

void main()
{
    vec2    uv  = BV.tex_coor[0].xy;
//	vec2    ts  = textureSize( aaa_tex2d[0], 0 );
    vec4    col;	// = BV.color;

	col = vec4( texture2D( aaa_tex2d[0], uv ).rgb, 1.);

    vec4 target = aaa_fu_vec4[0];
    //float grey = compute_gray( col.rgb );
	float g = compute_gray( col.rgb );
	g = ( ( g - aaa_fu_float[1] ) * aaa_fu_float[2] ) + aaa_fu_float[1];
  	g = clamp( g * aaa_fu_float[2], 0., 1. );

	uv = abs( uv - .5 ) * 2.;
	float v = max( uv.x, uv.y );
	if( v > .975 )
		target.a *= (1.-v) * 40.;

	float grey_pow = pow( g, aaa_fu_float[3] );

//   grey = pow( (1.-grey), aaa_fu_float[3] );
//	col = mix( col, target, grey * aaa_fu_float[2] );
	vec3 tmp = mix( vec3(0), target.rgb, g );
    color_out_0.rgb = mix( tmp, vec3(1), grey_pow );
    color_out_0.a = target.a;

	//color_out_0 = vec4(v);
}
