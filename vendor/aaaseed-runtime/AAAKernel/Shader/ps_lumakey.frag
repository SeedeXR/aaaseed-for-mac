// GLSL fragment shader
// Lumakeying


uniform sampler2D	aaa_tex2d[4];

//// CCIR 601 standard
//const vec3 std601R = {  1.164 ,  0    ,  1.4022   };
//const vec3 std601G = {  1.164 , -0.391, -0.813    };
//const vec3 std601B = {  1.164 ,  2.018,  0        };
//const vec4 stdbias = { -0.0625, -0.5  , -0.5, 0.0   };


const vec3 std_Y = {  0.257,  0.504,  0.098   };
const vec3 std_G = {  -0.1482, -0.291, 0.4391    };
const vec3 std_B = {  0.440,  -0.368,  -0.0718   };
const vec3 stdbias_x = { 0.0625, 0.5  , 0.5  };

//	float smooth_step( float edge2, float edge1, float value)
//	{
//		float t;
//		t = clamp( (  value - edge1 ) / ( edge2 - edge1 ), 0.0, 1.0 );
//		return t * t * ( 3.0 - 2.0 * t);
//	}

void main (void)
{
	vec3	luma_key = aaa_fu_vec4[0].rgb;
	float	threshold_high = aaa_fu_float[0];
	float	threshold_low = aaa_fu_float[1];
	vec4	color_tex = texture2D( aaa_tex2d[0], gl_TexCoord[0].st );
	// convert to luma
	vec3    luma_color = vec3( dot( color_tex.rgb, std_Y ),  dot( color_tex.rgb, std_G ), dot( color_tex.rgb, std_B ) ) + stdbias_x;

	float	dist = distance( luma_color, luma_key );
	float	alpha = 1.0 - smoothstep( threshold_low, threshold_high, dist );
//		if( dist > threshold_high )
//			discard;

	gl_FragColor = blend( vec4(color_tex.rgb,  alpha ) );//color_tex.a);
}
