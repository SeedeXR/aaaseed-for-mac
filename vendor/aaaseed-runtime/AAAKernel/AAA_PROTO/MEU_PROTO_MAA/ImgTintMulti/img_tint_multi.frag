
in ST_AAA_BV BV;

uniform sampler2D   aaa_tex2d[4];


// CONST vec2 influence	= aaa_fu_vec4[0].xy;
// CONST vec2 scale		= aaa_fu_vec4[0].zw;
// CONST vec2 scale_over	= 1. / scale;
// CONST vec4 step			= aaa_fu_vec4[1];
// CONST vec2 gain_factor	= aaa_fu_vec4[2].xy;
// CONST vec2 bias_factor	= aaa_fu_vec4[2].zw;
// CONST vec2 factor		= aaa_fu_vec4[3].xy;

CONST int col_nb = aaa_fu_int[0];
CONST int method = aaa_fu_int[1];

CONST float influence		= aaa_fu_float[0];
CONST float divergence_u	= aaa_fu_float[1];

void main()
{
	vec2 uv = BV.tex_coor[0].st;

	vec4 col = texture2D( aaa_tex2d[0], uv );
	if( divergence_u != 0. )
	{
		col.r = texture2D( aaa_tex2d[0], uv + vec2(-divergence_u,0) ).r;
		col.b = texture2D( aaa_tex2d[0], uv + vec2(divergence_u,0) ).b;
	}
	float d2_found = 100000.;
	int i_closer;
	int i;
	if( method > 0 )
	{
		if( method < 5 )
		{
			for( i = 0; i<col_nb; ++i )
			{
				vec3 v = col.rgb - aaa_fu_vec4[i].rgb;
				float d2 = dot(v,v);
				if( d2 < d2_found )
				{
					d2_found = d2;
					i_closer = i;	
				}
			}
			switch(method)
			{
			case 1:
				col.rgb = mix( col.rgb, aaa_fu_vec4[i_closer].rgb, influence );
				break;
			case 2:
				col.rgb = col.rgb - (col.rgb - aaa_fu_vec4[i_closer].rgb) * influence;
				break;
			case 3:
				vec3 hsv = rgb2hsv( aaa_fu_vec4[i_closer].rgb );
				hsv.z = compute_gray(col.rgb);
				col.rgb = mix( col.rgb, hsv2rgb(hsv), influence );
				break;
			case 4:
				col.rgb = (col.rgb - aaa_fu_vec4[i_closer].rgb) * influence + aaa_fu_vec4[i_closer+4].rgb;
				col.a = mix( 1., aaa_fu_vec4[i_closer+4].a, influence );
				break;
			}
		}
		else
		{
			vec3 hsv = rgb2hsv( col.rgb );
			for( i = 0; i<col_nb; ++i )
			{
				float d2 = abs(hsv.x - rgb2hsv( aaa_fu_vec4[i_closer].rgb ).x);
				if( d2 < d2_found )
				{
					d2_found = d2;
					i_closer = i;	
				}
			}
			switch(method)
			{
			case 4:
				col.rgb = mix( col.rgb, aaa_fu_vec4[i_closer].rgb, influence );
				break;
			}
		}
	}


	//Apply slider overlay
	gl_FragColor = col * BV.color; 
}