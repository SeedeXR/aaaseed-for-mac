////#version 330 compatibility
////#extension GL_EXT_gpu_shader4 : enable

// Uniform.
//uniform sampler2D	aaa_tex2d[2];
uniform sampler2D	aaa_samp0;
uniform sampler2D	aaa_samp1;


// Output.
out VERT_BLOCK
{
	vec4 	pos_kin;
	vec4 	pos_world;
	vec2 	uv;
	vec3 	nor;
	float	depth;
} vert_block;


// cs mean cos_sin
#define	plan_center		aaa_vu_vec4[8].xyz
#define plan_rot_y		aaa_vu_vec4[9].zw
#define plan_rot_x		aaa_vu_vec4[9].xy
#define plan_rot_z		aaa_vu_vec4[10].xy

#define	trs_center		aaa_vu_vec4[5].xyz
#define	trs_rot_z_cs	aaa_vu_vec4[0].xy
#define	trs_rot_x_cs	aaa_vu_vec4[0].zw
#define	trs_rot_y_cs	aaa_vu_vec4[3].xy
#define b_trs_swap_xy	(aaa_vu_int[0]==1)
#define	trs_trans		aaa_vu_vec4[2].xyz;
#define	trs_rot_xy_cs	aaa_vu_vec4[3].zw
#define	trs_sca_xy_cs	aaa_vu_vec4[4].xy

#define clamp_min_xyz	aaa_vu_vec4[6].xyz
#define clamp_max_xyz	aaa_vu_vec4[7].xyz

void do_plan( inout vec3 vec, bool b_tra )
{
// first translation (center)
	if( b_tra )
		vec.xyz	+=  plan_center;
// rotation on y
	rotate2_cos_sin_neg(	vec.xz, plan_rot_y );
// rotation on x
	rotate2_cos_sin(		vec.yz, plan_rot_x );
// rotation on z
	rotate2_cos_sin(		vec.xy, plan_rot_z );
}

void do_transfo( inout vec3 vec, bool b_translate )
{
// first translation (center)
	if( b_translate )
		vec.xyz	+= trs_center;
// rotation on z
	rotate2_cos_sin(		vec.xy, trs_rot_z_cs );
// rotation on x
	rotate2_cos_sin(		vec.yz, trs_rot_x_cs );
// rotation on y
	rotate2_cos_sin_neg(	vec.xz, trs_rot_y_cs );
// swap xy
	if( b_trs_swap_xy )
		vec = vec3( -vec.x, vec.zy );
// translation xyz
	if( b_translate )
		vec.xyz	+= trs_trans;
// rotation on z
	rotate2_cos_sin(		vec.xy, trs_rot_xy_cs );
// scaling on xy
	vec.xy	*= trs_sca_xy_cs;
}

/*
vec2 camera_projected_uv(in mat4 p_mvp, in vec3 vec )
{
	vec4 v4 = p_mvp * vec4( vec, 1.0 );
	return v4.xy / v4.w;
}
*/
const float OUT = 100.;
const float OUT_TEST = OUT - 1.;

bool compute_pos( in vec2 uv, out vec3 pos )
{
	//	DEPTH TO KINECT
	//	depth is positive for now
	float depth = texture( aaa_samp0, uv ).r * 8.;
	if( depth <= aaa_vu_float[1] || aaa_vu_float[2] <= depth )
		return false;

	//	depth is now negative like a good opengl camera
	//pos.xy =  texture2D( aaa_tex2d[1], pos.xy, 0 ).rg * depth;
	pos.xy = -texture( aaa_samp1, vec2( uv.x, uv.y ) ).rg * depth;
	pos.z = -depth;
	return true;
//	pos.z = sin( pos.x *5. ) * ;
//	pos.z = -depth;
//	pos.xy = uv;
//	pos.z = -1;
}

void main()
{
	//vec2 uv = gl_Vertex.xy * vec2( 511, 423 ) + .5;
	//uv *= vec2( 1./512., 1/424. );

	vec2 uv = gl_Vertex.xy;

	// KINECT MATRIX is the gl_TextureMatrix[0]
	// UV PROJECTED TO GET DEPTH
//	vec2 uv = camera_projected_uv( gl_TextureMatrix[0], pos.xyz );
	// we pass uv to display texture later
	vert_block.uv = uv;	// just pass

	vec3 pos_kinect;
	if( !compute_pos( uv, pos_kinect ) )
	{
		vert_block.depth = OUT;	//mark the point as invalid
		return;
	}
	vert_block.pos_kin.xyz = pos_kinect;
	vert_block.depth = 	-pos_kinect.z;	//store before we change it

	vec3 pos = pos_kinect;
	do_plan( pos, true );
	do_transfo( pos, true );

	vec3 tmp = clamp( pos.xyz, clamp_min_xyz, clamp_max_xyz );
	if( tmp != pos.xyz )
	{
		vert_block.depth = OUT;
		return;
	}

// COMPUTE NORMAL ONLY NOW
#define ZMIN -.5
#if 0
#define D 8
	vec3 a1;
	compute_pos( uv + vec2(D/512.,0.), a1 );
	if( a1.z >= ZMIN )	pos.z = 0.;

	vec3 b1;
	compute_pos( uv + vec2(0.,D/424.), b1 );
	if( b1.z >= ZMIN )	pos.z = 0.;

	vec3 n = normalize( cross( b1-pos, a1-pos) );
#else
	//todo the normal computation is shitty whebn there is no neighboor
	//todo revive eliminate point with no neighboor as an option: was 	if( abs(a1.z) >= OUT_LOW )		{	vert_block.depth = OUT;		return; 	}
	vec3 a1,a2;
	if( !compute_pos( uv + vec2(aaa_vu_float[0]/512.,0.), a1 ) )
	{
		if( !compute_pos( uv - vec2(aaa_vu_float[0]/512.,0.), a2 ) )
			a1 = vec3( 1,0,0 );
		else
			a1 = pos - a2;
	}
	else
	{
		if( !compute_pos( uv - vec2(aaa_vu_float[0]/512.,0.), a2 ) )
			a1 = a1 - pos;
		else
			a1 = a1 - a2;
	}

	vec3 b1,b2;
	if( !compute_pos( uv + vec2(0.,aaa_vu_float[0]/424.), b1 ) )
	{
		if( !compute_pos( uv - vec2(0.,aaa_vu_float[0]/424.), b2 ) )
			b1 = vec3( 0,1,0 );
		else
			b1 = pos - b2;
	}
	else
	{
		if( !compute_pos( uv - vec2(0.,aaa_vu_float[0]/424.), b2 ) )
			b1 = b1 - pos;
		else
			b1 = b1 - b2 ;
	}

	vec3 nor = normalize( cross( b1, a1) );
#endif
	do_plan( nor, false );
	do_transfo( nor, false );
	vert_block.nor = nor;

//	pos.w = 1;
//	float d;

#if 0
	//eliminate where this invalid pixel nearby
	{
#define STEP 2
#define MINZ (.01*.125)

		d = 0.;
		vec4 dep;
		dep.x = texture2D(aaa_tex2d[0], uv).r;
		uv.x += STEP/512.;
		dep.y = texture2D(aaa_tex2d[0], uv).r;
		uv.y -= STEP/424.;
		dep.z = texture2D(aaa_tex2d[0], uv).r;
		uv.x -= STEP/512.;
		dep.w = texture2D(aaa_tex2d[0], uv).r;
		uv.y += STEP/424.;
		if( any( lessThan( dep, vec4(MINZ,MINZ,MINZ,MINZ) ) ) )
			d = 0;
		else
			d = texture2D(aaa_tex2d[0], uv, 2 ).r;
	}
#else
//	d = texture2D(aaa_tex2d[0], uv, 1 ).r;
#endif

#if 0
	{
#define STEP 2
		uv.x += STEP/512.;

		float f  = texture2D(aaa_tex2d[0], uv).r;
		if( f != 0. )
		{
			d += f;
			uv.y -= STEP/424.;
			f = texture2D(aaa_tex2d[0], uv).r;
			if( f != 0. )
			{
				d += f;
				uv.x -= STEP/512.;
				f = texture2D(aaa_tex2d[0], uv).r;
				if( f != 0. )
				{
					d += f;
					d *= .25;
				}
			}
		}
	}
#endif


	//depth -= 1;
	//vec3 dir 		= normalize( pos.xyz - aaa_gu_vec4[0].xyz );
	//we doin't normalize because we divide by the dit product
//	vec3 dir 		= pos.xyz - aaa_gu_vec4[0].xyz;
//	vec3 kinect_dir	= normalize( aaa_gu_vec4[1].xyz - aaa_gu_vec4[0].xyz );
	//vec2 uv = vert_block[i].tex_uv - .5;
	//displace.xyz = kinect_pos.xyz - (vx * uv.x * CONE_FX + vy * uv.y * CONE_FY - vz) * depth ;

//	ver_block.pos_depth.xyz = aaa_gu_vec4[0].xyz + dir * depth / dot( kinect_dir, dir );
//	pos.xyz += aaa_vu_vec4[5].xyz;
	vert_block.pos_world = vec4( pos.xyz, 1. );

	// Built-in.
//	gl_Position = vert_block.pos;
}


