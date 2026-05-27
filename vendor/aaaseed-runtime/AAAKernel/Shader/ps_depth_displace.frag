
//#version 330 compatibility
//#extension GL_EXT_gpu_shader4 : enable
// //#extension GL_ARB_shading_language_include : enable

//uniform sampler2D	aaa_tex2d[2];
uniform sampler2D	aaa_samp0;

// 0 calage
// 12 for Red dynamic : r = (r+f1)*f2
// 34 for Green
// 56 for blue
// Input.
in GEOM_BLOCK
{
	vec4 pos_world;
	vec4 normal_depth;
	vec2 uv;
} geom_block;

void main()
{
	vec4 col;
	if( aaa_fu_int[0] < 4 )
	{
		if( aaa_fu_int[0] == 1 )
		{	// Display input texture.
			//gl_FragColor = texture2D( aaa_tex2d[0], geom_block.uv );
			col = texture( aaa_samp0, geom_block.uv );
		}
		else if( aaa_fu_int[0] == 0 )
		{		// Display WHITE
			col = vec4( 1, 1, 1, 1 );
		}
		else if( aaa_fu_int[0] == 2 )
		{
			float d;
			d = texture( aaa_samp0, geom_block.uv, 0).r;
			d -= texture( aaa_samp0, geom_block.uv + vec2(1./512.), 0. ).r;
			d = abs( d ) * 500;
			col = vec4( d, d, d, 1 );
		}
		else
		{
			col = vec4( geom_block.uv, 0, 1 );
		}
	}
	else if( aaa_fu_int[0] < 8 )
	{
		if( aaa_fu_int[0] < 6 )
		{	// 4,5 Normal
			col = vec4( geom_block.normal_depth.xyz, 1. );
			if( aaa_fu_int[0] == 5 )
				col.xyz = col.xyz * .5 + .5;
		}
		else
		{	// 6,7 Depth Mul
			float d = geom_block.pos_world.z;
			d = clamp( (d+aaa_fu_float[1])  * aaa_fu_float[2], 0., 1. );
			col = vec4( d, d, d, 1 );
			if( aaa_fu_int[0] == 7 )
				col.xyz *= geom_block.normal_depth.xyz;
		}
	}
	else
	{

#define HS .01
		if( aaa_fu_int[0] == 8 )
		{
			float z = geom_block.pos_world.z;
			col.rgb = vec3( (z+aaa_fu_float[1]) * aaa_fu_float[2], (z+aaa_fu_float[3]) * aaa_fu_float[4], (z+aaa_fu_float[5]) * aaa_fu_float[6] );
			if( col.r > 1. )	col.r = 0;
			if( col.g > 1. )	col.g = 0;
			if( col.b > 1. )	col.b = 0;
			if( all( lessThan( col.rgb, vec3(0.001) ) )	)
				discard;
		}
		else if( aaa_fu_int[0] == 9 )
		{
			vec4 pos = geom_block.pos_world;
			col.rgb = vec3( pos.y * 10. );
		}
		else if( aaa_fu_int[0] == 10 )
		{
			vec4 pos = geom_block.pos_world;
			col.r = (pos.x <= 0.) ? .5 : 1.;
			col.g = (pos.y <= 0.) ? .5 : 1.;
			col.b = (pos.z <= 0.) ? .5 : 1.;
		}
		else
		{
			vec4 pos = geom_block.pos_world;
			col.rgb = vec3( 1+pos.z );
		}
		col.a = 1.;
	}

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
	gl_FragColor = col;
	//gl_FragColor = vec4(1.);
}
