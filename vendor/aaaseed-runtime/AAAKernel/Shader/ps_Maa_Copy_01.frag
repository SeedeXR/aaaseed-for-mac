//#version 330 compatibility

// Uniform variables for texturing
uniform sampler2D 	aaa_tex2d[];


// 0 = iterations
//uniform int			aaa_fu_int[4];
//uniform float		aaa_fu_float[8];
// Colors factors
// 0 = src colors
//uniform vec4		aaa_fu_vec4[8];

void main(void)
{
	vec4 color;
	if( aaa_tex_dim[0]==2 )
	{
		vec2 uv = gl_FragCoord.xy;
		uv.x += float(-aaa_fu_int[0] + aaa_fu_int[2]);
		uv.y += float(-aaa_fu_int[1] + aaa_fu_int[3]);
		uv /= textureSize( aaa_tex2d[0], 0 ).xy;
		if( aaa_fu_float[2]>0.5 && (uv.x < 0. || 1. < uv.x || uv.y < 0. || 1. < uv.y) )
			discard;
		//color.b = texture2D( aaa_tex2d[0], uv ).b;
		color.rgb = texture2D( aaa_tex2d[0], uv ).rgb;
		//color.b = 0;
		color.a = 1.;
		gl_FragColor = color;
	}
	else
	{
		gl_FragColor = gl_Color;
	}
	//gl_FragColor = gl_Color;
//	gl_FragColor.rgb = texture2D( aaa_tex2d[0], gl_FragCoord.xy ).rgb;
	//gl_FragColor.rg = gl_FragCoord.rg/2024.;
//	gl_FragColor.a = 1.;
//	gl_FragColor = vec4(1.);
}