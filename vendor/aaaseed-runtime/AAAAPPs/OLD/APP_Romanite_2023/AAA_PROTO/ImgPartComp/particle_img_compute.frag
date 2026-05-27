
//layout(early_fragment_tests) in;

layout(location = 0) in vec3 in_normal;
layout(location = 1) in vec3 in_position_world;
layout(location = 2) in vec2 in_texcoord;
layout(location = 3) in vec4 in_color;
layout(location = 4) in vec4 in_rnd;

#define hardness				aaa_fu_float[0]
#define roundness				aaa_fu_float[1]
#define	noise					aaa_fu_float[2]
#define noise_freq				aaa_fu_float[3]
CONST float recit_scale_mask =	aaa_fu_float[4];

#define	s_test			aaa_fu_int[0]
CONST bool b_recit	= aaa_fu_int[1]!=0;

#define	s_prim			aaa_vu_int[1]

//outputs
layout(location = 0) out vec4 out_result;

layout(binding = 0) uniform sampler2D TEX;
layout(binding = 1) uniform sampler2D TEX_DEPTH;
layout(binding = 3) uniform sampler2D TEX_FUMEE;


#define NOISE simplex3d
void main()
{
#if 0
//UNUSED
	switch( s_test )
	{
	case 0:
		{
		vec2  uv;
		if( s_prim == 1 )	//GL_POINTS
			uv = gl_TexCoord[0].st;
		else
			uv = in_texcoord.st;

		uv = uv * 2. - 1.;
	//	tex.s *= 4.;
		float d = dot(uv.st, uv.st);
		if( roundness != 1. )
		{
			vec2 uvb = abs(uv);
			d = mix( max(uvb.s,uvb.t), d, roundness );
		}
		// if( noise != 0. )
		// {
		// 	d += ( NOISE( vec3( vec2(uv * noise_freq), 		in_rnd.x * 123.) ) + 1.) * noise;
		// 	d += ( NOISE( vec3( vec2(uv * noise_freq * 2),	in_rnd.x * 829.) ) + 1.) * noise * .5;
		// }

		if( d >= 1. )
			discard;
		// if( d < .8 )
		// 	discard;

		//d = 1. - pow( d, hardness );
		//d = smoothstep( hardness, 1., d );	//this do circle
		d = 1. - smoothstep( hardness, 1., d );	//better than previous when hardness is 1.

		//if( d > .4 )
		//	discard;
	
	//	out_result = vec4( d, d, d, 1 ); // * aaa_fu_vec4[0];
	//	out_result = mix( vec4(1,0,0,1), vec4(0,1,0,1), d );
		out_result = vec4( 1,1,1, d );
		out_result *= mix( aaa_fu_vec4[0], aaa_fu_vec4[1], in_rnd.b );

	//	out_result.a *= in_rnd.a;	//mix( aaa_fu_vec4[0], aaa_fu_vec4[1], in_rnd );
	//	out_result.a *= d;
	//	out_result *= d;
		}
		break;
	case 1:
		out_result = vec4( 1. );
		break;
	case 2:
		{
			vec2  uv;
			if( s_prim == 1 )	//GL_POINTS
				uv = gl_TexCoord[0].st;
			else
				uv = in_texcoord.st;
			out_result = texture( TEX, uv );	//.rgb
			out_result *= mix( aaa_fu_vec4[0], aaa_fu_vec4[1], in_rnd.b );
		}
		break;
	default:
	case 3:
		{
			vec2  uv;
			if( s_prim == 1 )	//GL_POINTS
				uv = gl_TexCoord[0].st;
			else
				uv = in_texcoord.st;
			uv = uv * 2. - 1.;
			float d = dot(uv.st, uv.st);
			if( d > 1. )
				discard;
			d = 1. - pow( d, hardness );
			out_result = vec4( 1,1,1, d );
			out_result *= mix( aaa_fu_vec4[0], aaa_fu_vec4[1], in_rnd.b );
		}
		break;
	}

#else
// WE USE THIS PART OF CODE
	//color.a = 1;
	if( in_color.a <.001 )
		discard;

	vec2 uv;
	if( s_prim == 1 )	//GL_POINTS
		uv = gl_TexCoord[0].st;
	else
		uv = in_texcoord.st;
//	out_result = texture( TEX, vec2(0,1.) + uv * vec2(1.,-2.) );	//.rgb

	vec4 color;
	if( b_recit )
	{
		//uv.x = (uv.x-.5)*recit_scale_mask*recit_scale_mask*recit_scale_mask + .5;
		color = texture( TEX, uv );
		vec2 uvf = uv-.5;
		//rotate( uvf, recit_scale_mask * 2. );
		uvf = uvf*recit_scale_mask;
		uvf += .5;
		uvf = clamp( uvf, 0.,1.);
		float per = texture( TEX_FUMEE, uvf ).r;
		
		color.rgb = mix( vec3(1), color.rgb, color.a) ;
		color.a = per * 2;
		
		//per =  pow( per, .5 ) + .0;
		// if( .01 < per*color.a && per*color.a < .5 )
		// {
		// 	color = vec4(1,1,1, (per*color.a-.01) * 4 );
		// }
		// else
		// {
		// 	color.a *= per*1.2;
		// 	if( color.a <.001 )
		// 		discard;
		// }
		color *= in_color;
	}
else
	{
		color = texture( TEX, uv );
		if( in_rnd.x<=0. && in_rnd.y<=0. )
			color *= in_color;
		else
		{
			// use the 8 points around in this order  to do a kind of blur
			//	432
			//	501
			//	678
			//	point 1
 //todoopt use swizzle
 			uv.x += in_rnd.x;  
			color += texture( TEX, uv );
			//	point 234
			uv.y += in_rnd.y;  
			color += texture( TEX, uv );
			uv.x -= in_rnd.x;
			color += texture( TEX, uv );
			uv.x -= in_rnd.x;
			color += texture( TEX, uv );
			//	point 56
			uv.y -= in_rnd.y;  
			color += texture( TEX, uv );
			uv.y -= in_rnd.y;  
			color += texture( TEX, uv );
			//	point 78
			uv.x += in_rnd.x;
			color += texture( TEX, uv );
			uv.x += in_rnd.x;
			color += texture( TEX, uv );
			// we used 9 points and now take the average
			color *= in_color/9.;
		}
		if( color.a <.001 )
			discard;
	}
	out_result = color;

//	out_result.a *= pow( in_rnd.a, .25 );

//	out_result.a = 1.;

//	out_result = vec4( texture( TEX, uv ).a,0,0, 1 );	//.rgb

//	out_result = vec4(1);
//	out_result = color * mix( aaa_fu_vec4[0], aaa_fu_vec4[1], in_rnd );
#endif
//	out_result *= color;
//	out_result.rgb = mix( vec3(.25), out_result.rgb, color.a );
//	out_result = vec4(1);
}
	
	