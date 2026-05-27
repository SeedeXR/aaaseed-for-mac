//#version 330 compatibility
////#extension GL_EXT_gpu_shader4 : enable
// //#extension GL_ARB_shading_language_include : enable
precision highp float;

layout(location = 0) out vec4 frag_color_buf0;
layout(location = 1) out vec4 frag_color_buf1;

// 1-v1/2-v2/3-azure deal
//todo deal with lidar too ?
CONST	int		s_cam_type			= aaa_vu_int[3];

CONST	int		s_render			= aaa_fu_int[0];
CONST	int		s_render_combined	= aaa_fu_int[1];
CONST 	bool	b_combine			= aaa_fu_int[2] == 1;

CONST	float	alpha_asked = aaa_fu_float[7];


//uniform sampler2D	aaa_tex2d[2];
uniform sampler2D	aaa_samp0;
#define TEX_DEPTH 			aaa_samp0
uniform sampler2D	aaa_samp1;
#define TEX_DEPTH_CORRECT	aaa_samp1
uniform sampler2D	aaa_samp2;
#define TEX_RGB				aaa_samp2
uniform sampler2D	aaa_samp3;
#define TEX_RGB_CORRECT		aaa_samp3

#define	depth_tex_size_over	aaa_vu_vec4[1].zw
#define	rgb_tex_size_over	aaa_fu_vec4[0].zw

//uniform int 		aaa_fu_int[4];			//[] don't work on intel (Nov 2017)
//uniform float 	aaa_fu_float[8];		//[] don't work on intel (Nov 2017)
// 0 calage
// 12 for Red dynamic : r = (r+f1)*f2
// 34 for Green
// 56 for blue
// Input.
in GEOM_BLOCK
{
	vec4 pos_kin;
	vec4 pos_world;
	vec4 normal;
	vec4 uv_depth_rgb;
} geom_block;

void main()
{
	vec4 col;
	switch( s_render )
	{
	case 1:	// White
			col = vec4( 1, 1, 1, 1 );	break;
	case 2:	// TEX : Display input texture.
			col = texture( TEX_DEPTH_CORRECT, geom_block.uv_depth_rgb.xy );
			break;
	case 3:	// Grad
			{
				//todo better use get_gradient
				float d;
				d = texture( TEX_DEPTH_CORRECT, geom_block.uv_depth_rgb.xy, 0).r;
				d -= texture( TEX_DEPTH_CORRECT, geom_block.uv_depth_rgb.xy + depth_tex_size_over, 0. ).r;
				d = abs( d ) * 500;
				col = vec4( d, d, d, 1 );
			}
			break;
	case 4:	//UV
			col = vec4( geom_block.uv_depth_rgb.xy, 0, 1 );
			break;
	case 5:	//Calage
			col = geom_block.pos_world;
			col.r = (col.x <= 0.) ? .5 : 1.;
			col.g = (col.y <= 0.) ? .5 : 1.;
			col.b = (col.z <= 0.) ? .5 : 1.;
			col.a = 1;
			break;
	case 6:
	case 7:	// 6,7 Normal
			col = vec4( geom_block.normal.xyz, 1. );
			if( s_render == 6 )
				col.xyz = col.xyz * .5 + .5;
			break;
	case 8:
	case 9: // Depth Mul
			{
				float d = geom_block.pos_world.z;
				d = clamp( (d+aaa_fu_float[1])  * aaa_fu_float[2], 0., 1. );
				col = vec4( d, d, d, 1 );
				if( s_render == 8 )
					col.xyz *= geom_block.normal.xyz;
			}
			break;
	case 10: //World XYZ
		{
			col = geom_block.pos_world;
		}
		break;
	case 11:
			{
				col.rgb = geom_block.pos_world.xyz;
				col.rgb = vec3( (col.r+aaa_fu_float[1]) * aaa_fu_float[2], (col.g+aaa_fu_float[3]) * aaa_fu_float[4], (col.b+aaa_fu_float[5]) * aaa_fu_float[6] );
				if( col.r > 1. )	col.r = 0;
				if( col.g > 1. )	col.g = 0;
				if( col.b > 1. )	col.b = 0;
				if( all( lessThan( col.rgb, vec3(0.001) ) )	)
					discard;
				col.a = 1;
			}
			break;
	case 12:	//X
	case 13:	//Y
	case 14:	//Z
			{
				float v;
				switch( s_render )
				{
				case 12: v = geom_block.pos_world.x; break;
				case 13: v = geom_block.pos_world.y; break;
				case 14: v = geom_block.pos_world.z; break;
				}
				col.rgb = vec3( 1.-((v+aaa_fu_float[1]) * aaa_fu_float[2]), (v+aaa_fu_float[3]) * aaa_fu_float[4], (v+aaa_fu_float[5]) * aaa_fu_float[6] );
				if( col.r > 1. )	col.r = 0;
				if( col.g > 1. )	col.g = 0;
				if( col.b > 1. )	col.b = 0;
				if( all( lessThan( col.rgb, vec3(0.001) ) )	)
					discard;
				col.a = 1;
			}
			break;
	case 15:	//kin XYZ
			col = geom_block.pos_kin;
			col.rgb = vec3( col.xy, -col.z );
			break;

		//vec3( pos.xy*.5+.5, -pos.z );
		//if( all( lessThan( col.rgb, vec3(0.0001) ) ) )
		//{
		//	col.a = 0;
		//	return;
		//}
	case 17:	//Red X
	case 18:	//Red Y
	case 19:	//Red Z
			{
				int index = s_render - 17;
				float v = geom_block.pos_world[index];
				index = index * 2 + 1;
				col.rgb = vec3( (v + aaa_fu_float[ index ]) * aaa_fu_float[ index + 1 ] );
				// if( col.r > 1. || col.r < 0.001 )
				//  	discard;
				col.a = 1;
			}
			break;
	}
	col.a = 1;

	vec3 mcol;
	if( s_render_combined > 1  )
	{
		if( s_render_combined > 3  )
		{
			vec2 uv = geom_block.uv_depth_rgb.xy;
			if( s_cam_type==2 )
			{	//we used this with Kinect 2
				uv = texture( TEX_RGB_CORRECT, vec2( uv.r, 1.-uv.g) ).rg;	//y flipped in C reception (HLSL ?)
				//todo this kinect 2 code should be change for azure when rgb aligned done
				uv.r = uv.r / 1920.;
				uv.g = 1 - uv.g / 1080.;	//y flipped in C reception //y flipped in C reception (HLSL ?)ww
				//uv = vec2(.5);
			}
			else if( s_cam_type==3 )
			{
				uv = geom_block.uv_depth_rgb.zw;
			}
			//col.rg = 	uv.rg;--
			mcol = texture( TEX_RGB,  uv	).rgb;
		}
		else
			mcol = vec3(1);

		if( s_render_combined == 2 || s_render_combined == 5  )	// normal [0,1]
			mcol *= geom_block.normal.xyz * .5 + .5;
		else if( s_render_combined == 3 || s_render_combined == 6 )	// norma; [-1,1]
			mcol *= geom_block.normal.xyz;
	}
	else
		mcol = vec3(1);

	if( b_combine ) // Combine output else output in second buffer
		col.rgb *= mcol;

// coloriage de calage
	if( aaa_fu_float[0] > .0 )
	{
		vec4 pos = abs( geom_block.pos_world );
		float cal = aaa_fu_float[0];
		if( 		pos.x < cal )	col = vec4( 1,0,0,1 );
		else if(	pos.y < cal )	col = vec4( 0,1,0,1 );
		else if(	pos.z < cal )	col = vec4( 0,0,1,1 );
	}
	// Color used for debuging.
	//gl_FragColor = geom_block.color;
	//gl_FragColor = col;
	//gl_FragColor = vec4(1.);

	col.a *= alpha_asked;
	frag_color_buf0 = col;
//	frag_color_buf0 = vec4( 1,0,0, 1 );

	if( !b_combine  )
	{
		frag_color_buf1 = vec4( mcol, 1. );
	}
	//frag_color_buf1 *= gl_Color;
}
