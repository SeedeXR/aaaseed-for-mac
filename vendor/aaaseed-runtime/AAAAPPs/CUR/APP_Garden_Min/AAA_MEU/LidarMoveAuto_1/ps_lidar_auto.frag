////#version 330 compatibility
////#extension GL_EXT_gpu_shader4 : enable
// //#extension GL_ARB_shading_language_include : enable

layout(location = 0) out vec4 frag_color_buf0;
layout(location = 1) out vec4 frag_color_buf1;

//uniform sampler2D	aaa_tex2d[2];
uniform sampler2D	aaa_samp0;
uniform sampler2D	aaa_samp1;
uniform sampler2D	aaa_samp2;
uniform sampler2D	aaa_samp3;
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
	vec4 normal_depth;
	vec2 uv;
} geom_block;

void main()
{
	vec4 col;
	switch( aaa_fu_int[0] )
	{
	case 1:	// White
			col = vec4( 1, 1, 1, 1 );	break;
	case 2:	// TEX : Display input texture.
			//gl_FragColor = texture2D( aaa_tex2d[0], geom_block.uv );
			col = texture( aaa_samp0, geom_block.uv );
			break;
	case 3:	// Grad
			{
				float d;
				d = texture( aaa_samp0, geom_block.uv, 0).r;
				d -= texture( aaa_samp0, geom_block.uv + vec2(1./512.), 0. ).r;
				d = abs( d ) * 500;
				col = vec4( d, d, d, 1 );
			}
			break;
	case 4:	//UV
			col = vec4( geom_block.uv, 0, 1 );
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
			col = vec4( geom_block.normal_depth.xyz, 1. );
			if( aaa_fu_int[0] == 6 )
				col.xyz = col.xyz * .5 + .5;
			break;
	case 8:
	case 9: // Depth Mul
			{
				float d = geom_block.pos_world.z;
				d = clamp( (d+aaa_fu_float[1])  * aaa_fu_float[2], 0., 1. );
				col = vec4( d, d, d, 1 );
				if( aaa_fu_int[0] == 8 )
					col.xyz *= geom_block.normal_depth.xyz;
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
				switch( aaa_fu_int[0] )
				{
				case 12: v = geom_block.pos_world.x; break;
				case 13: v = geom_block.pos_world.y; break;
				case 14: v = geom_block.pos_world.z; break;
				}
				col.rgb = vec3( (v+aaa_fu_float[1]) * aaa_fu_float[2], (v+aaa_fu_float[3]) * aaa_fu_float[4], (v+aaa_fu_float[5]) * aaa_fu_float[6] );
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
	}
	col.a = 1;

	if( aaa_fu_int[1] > 1  )
	{
		vec3 mcol;
		if( aaa_fu_int[1] > 3  )
		{
			vec2 uv = texture( aaa_samp3, vec2( geom_block.uv.r, 1.-geom_block.uv.g) ).rg;	//y flipped in C reception
			uv.r = uv.r / 1920.;
			//uv.g = uv.g / 1080.;	//y flipped in C reception
			uv.g = 1. - uv.g / 1080.;	//y flipped in C reception
			//col.rg = 	uv.rg;
			mcol = texture( aaa_samp2,  uv	).rgb;
		}
		else
			mcol = vec3(1);
		if( aaa_fu_int[1] == 2 || aaa_fu_int[1] == 5  )
			mcol *= geom_block.normal_depth.xyz * .5 + .5;
		else if( aaa_fu_int[1] == 3 || aaa_fu_int[1] == 6 )
			mcol *= geom_block.normal_depth.xyz;
		if( aaa_fu_int[2] == 1 ) // Combine output else output in second buffer
			col.rgb *= mcol;
	}

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
//	gl_FragColor = col;
	//gl_FragColor = vec4(1.);
	frag_color_buf0 = col;
	//	vec3 bcol;
	//		vec2 uv = texture( aaa_samp3, vec2( geom_block.uv.r, geom_block.uv.g) ).rg;	//y flipped in C reception
	//		uv.r = uv.r / 1920;
	//		uv.g = 1. - uv.g / 1080.;
	//		//col.rg = 	uv.rg;
	//		bcol = texture( aaa_samp2,  uv	).rgb;
	if( aaa_fu_int[2] >= 1  )
	{
		vec3 mcol;
		vec2 uv = texture( aaa_samp3, vec2( geom_block.uv.r, geom_block.uv.g) ).rg;	//y flipped in C reception
		uv.r = uv.r / 1920;
		uv.g = 1. - uv.g / 1080.;
		//col.rg = 	uv.rg;
		mcol = texture( aaa_samp2,  uv	).rgb;
		frag_color_buf1 = vec4( mcol, 1. );
	}
}
