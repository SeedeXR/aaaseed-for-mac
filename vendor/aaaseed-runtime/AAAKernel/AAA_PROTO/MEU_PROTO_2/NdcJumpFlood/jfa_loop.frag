

#define USE_MAA_VERSION 1

#define dist2_max	aaa_fu_float[2]		//we limit the size of distance that interest us

#if USE_MAA_VERSION
#else
#	define max_level	aaa_fu_float[2]
#	define iter			aaa_fu_int[0]
#endif


//pipeline inputs
layout(location = 0) in vec2 in_texcoord;
//outputs
layout(location = 0) out vec4 out_render;
layout(location = 1) out vec4 out_result;
//texture inputs
layout(binding = 0) uniform sampler2D TEX_PREVIOUS;
//layout(binding = 1) uniform sampler2D TEX_INPUT;

// done in lua now
//  const vec2 offsets[8] = {
//    vec2(	 1,	 0	),
// 	vec2(	-1,	 0	),
// 	vec2(	 0,	 1	),
// 	vec2(	 0,	-1	),
//  vec2(	 1,	 1	),
// 	vec2(	-1,	 1	),
// 	vec2(	 1,	-1	),
// 	vec2(	-1,	-1	),
// };

#define vec2_far_away vec2(-1000000,0)

#if 0//Julien
	#define USE_VECTOR_BOOL 1
	vec2 load_sample( vec2 uv )
	{
		//prevent out of bounds due to sampler
	#if USE_VECTOR_BOOL
		if( all(lessThanEqual( vec2(0.), uv )) && all(lessThanEqual( uv, vec2(1.) )) )
	#else
		if( 0. <= uv.x && 0. <= uv.y && uv.x <= 1. && uv.y <= 1.)
	#endif
			return texture( TEX_PREVIOUS,		uv ).xy;
		else
			return vec2_far_away;
	}
#else
	vec2 load_sample( vec2 uv )
	{
		return texture( TEX_PREVIOUS, uv ).xy;
	}
#endif

float distance2( in vec2 a, in const vec2 b )
{
	a -= b;
	return dot(a,a);
}

void main()
{
	const vec2 uv_nor = in_texcoord.xy;
	const vec2 uv_xy = gl_FragCoord.xy;

#if USE_MAA_VERSION==0
	//@Julien si je met iter+2 cidessous ca marche encore et en 2 ite de moins @?@?@?@?@
	float level = clamp( float(iter), 0., max_level );
	//step in pixels
    int step_size = int(exp2(max_level - level)-1);
	//int stepwidth = int(exp2(max_level - level)+0.5);
#endif

	vec2 xy = texture( TEX_PREVIOUS, uv_nor ).xy;
   
    vec2 xy_best;
	float dist2_best;
    if( xy.x < 0. )
	{
    	xy_best = vec2_far_away;
		dist2_best = dist2_max;
	}	
	else
	{
        xy_best = xy;
		dist2_best = distance2( xy, uv_xy );
	}

    for( int i=0; i<8; ++i )
	{
		xy = load_sample( uv_nor + aaa_fu_vec4[i].xy );
		if( xy.x >= 0. ) 
		{
			float dist2 = distance2( xy, uv_xy );
			if( dist2 < dist2_best )
			{
               	dist2_best = dist2;
                xy_best = xy;
            }
    	}
    }

	out_result.rg = xy_best;
}

