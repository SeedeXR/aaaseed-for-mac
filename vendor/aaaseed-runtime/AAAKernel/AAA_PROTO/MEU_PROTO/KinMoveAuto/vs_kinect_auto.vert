////#version 330 compatibility
////#extension GL_EXT_gpu_shader4 : enable

precision highp float;

// Uniform.
//uniform sampler2D	aaa_tex2d[2];
uniform sampler2D			aaa_samp0;
#define TEX_DEPTH 			aaa_samp0
uniform sampler2D			aaa_samp1;
#define TEX_DEPTH_CORRECT	aaa_samp1
uniform sampler2D			aaa_samp4;
#define TEX_DEFORM			aaa_samp4
//uniform sampler2D	aaa_samp2;
//#define TEX_RGB				aaa_samp2
//uniform sampler2D	aaa_samp3;
//#define TEX_RGB_CORRECT		aaa_samp3


// Output.
out VERT_BLOCK
{
	vec4 	pos_kin;
	vec4 	pos_world;
	vec2 	uv;
	vec3 	nor;
	float	depth;
} vert_block;

CONST 	float pixel_factor  	= aaa_vu_float[0];
CONST 	float depth_clip_min	= aaa_vu_float[1];
CONST 	float depth_clip_max	= aaa_vu_float[2];

// cs mean cos_sin
#define	plan_center		aaa_vu_vec4[8].xyz
#define plan_rot_y		aaa_vu_vec4[9].zw
#define plan_rot_x		aaa_vu_vec4[9].xy
#define plan_rot_z		aaa_vu_vec4[10].xy

#define	trs_center		aaa_vu_vec4[5].xyz
#define	trs_rot_z_cs	aaa_vu_vec4[0].xy
#define	trs_rot_x_cs	aaa_vu_vec4[0].zw
#define	trs_rot_y_cs	aaa_vu_vec4[3].xy

#define b_trs_swap_xy	((aaa_vu_int[0]&0x1)!=0)
#define b_trs_flip_x	((aaa_vu_int[0]&0x100)!=0)

#define	trs_trans		aaa_vu_vec4[2].xyz;
#define	trs_rot_xy_cs	aaa_vu_vec4[3].zw
#define	trs_sca_xy_cs	aaa_vu_vec4[4].xy

#define clamp_min_xyz	aaa_vu_vec4[6].xyz
#define clamp_max_xyz	aaa_vu_vec4[7].xyz

#define	depth_tex_size		aaa_vu_vec4[1].xy
#define	depth_tex_size_over	aaa_vu_vec4[1].zw

CONST 	int		s_remap			= aaa_vu_int[1];
CONST 	bool	b_unity			= aaa_vu_int[2] != 0 ;

// 1-v1/2-v2/3-azure deal
//todo deal with lidar too ?
CONST int	s_cam_type			= aaa_vu_int[3];

CONST	float	remap_u_min		= aaa_vu_float[3];
CONST 	float	remap_u_factor	= aaa_vu_float[4];
CONST 	float	remap_v_min 	= aaa_vu_float[5];
CONST 	float	remap_v_factor 	= aaa_vu_float[6];


void do_plan( inout vec3 vec, bool b_tra )
{
// first translation (center)
	if( b_tra )
		vec.xyz	+= plan_center;
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
// flip on x
	if( b_trs_flip_x )
		vec.x = -vec.x;
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
CONST float OUT = 100.;
CONST float OUT_TEST = OUT - 1.;
CONST float F_Z = 8.;

bool compute_pos( in vec2 uv, out vec3 pos )
{
#define TEXEL_USE	0
	//	DEPTH TO KINECT
	//	depth is positive for now

	//todo read on this gl textue/texel coor
	ivec2 i_uv = ivec2( floor( uv * depth_tex_size ) );
		
#if TEXEL_USE==1
	float depth = texelFetch( TEX_DEPTH, i_uv, 0 ).r * F_Z;
	if( depth <= depth_clip_min || depth_clip_max <= depth )
		return false;
#else
	float depth;
	if( b_unity )
	{
		vec4 col = texture( TEX_DEPTH, uv+.5 );
		if( col.a < .5 )
			return false;
		//depth = pow( texture( TEX_DEPTH, uv+.5 ).b, 2.42 ) * 10.;
		depth = pow( col.r, 2.2 ) * 3.5 + .5;

		if( depth <= depth_clip_min || depth_clip_max <= depth )
			return false;
	}
	else
	{		
		float depth_fetched = texelFetch( TEX_DEPTH, i_uv, 0 ).r * F_Z;
		if( depth_fetched <= depth_clip_min || depth_clip_max <= depth_fetched )
			return false;

		depth = texture( TEX_DEPTH, uv ).r * F_Z;
		if( abs( depth_fetched - depth ) > .05 )
			return false;

		// already done for the fetched one
		// if( depth <= depth_clip_min || depth_clip_max <= depth )
		// 	return false;
	}

#endif

	//	depth is now negative like a good opengl camera
	depth = -depth;
	//pos.xy =  texture2D( aaa_tex2d[1], pos.xy, 0 ).rg * depth;	
#if TEXEL_USE==1
	pos.xy = texelFetch( TEX_DEPTH_CORRECT, i_uv, 0 ).rg * depth;
#else
	if( b_unity )
	{
		pos.xy = texture( TEX_DEPTH_CORRECT, uv+.5 ).rg * depth;
		// if( fract(pos.x*10) < 0.5 && fract(pos.y*10) < 0.5 )
		// 	return false;
		//pos.x = sin( uv.x * 3.) * depth;
	}
	else
		pos.xy = texture( TEX_DEPTH_CORRECT, uv ).rg * depth;
#endif
	pos.z = depth;
	return true;
//	pos.z = sin( pos.x *5. ) * ;
//	pos.z = -depth;
//	pos.xy = uv;
//	pos.z = -1;
}

void main()
{
	vec2 uv = gl_Vertex.xy;

	// KINECT MATRIX is the gl_TextureMatrix[0]gol.set_teee
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
	vert_block.pos_kin = vec4( pos_kinect, 1. );
	vert_block.depth = 	-pos_kinect.z;	//store before we change it

	vec3 pos = pos_kinect;
// PLAN us the first transformation used to place the cloud of point
//	in reference to a plan or floor
	do_plan( pos, true );
// Then is more easy to move where we want to be
	do_transfo( pos, true );
// then we crop with a box
	vec3 tmp = clamp( pos.xyz, clamp_min_xyz, clamp_max_xyz );
	if( tmp != pos.xyz )
	{
		vert_block.depth = OUT;
		return;
	}
// then we can remap in world space
	if( s_remap!=0 )
	{
		// get remap space
		int u = s_remap & 0x3;
		int v = (s_remap >> 8) & 0x3;
		// get remap data
		vec2 def_uv = vec2( (pos[u] - remap_u_min) * remap_u_factor, (pos[v] - remap_v_min) * remap_v_factor );
		vec2 def = texture( TEX_DEFORM, def_uv ).rg;
		// if outside invalidate
		if( def == vec2(0) )
		{
			vert_block.depth = OUT;
			return;
		}
		// remap the values
		if( (s_remap & 0x4) != 0 )
		{
			if( (s_remap & 0x8) != 0 )		//  deal with inversion 
				def.r = 1. - def.r;
			pos[u] = def.r / remap_u_factor + remap_u_min;
		}
		if( (s_remap & 0x400) != 0 )
		{
			if( (s_remap & 0x800) != 0 )	//  deal with inversion 
				def.g = 1. - def.g;
			pos[v] = def.g / remap_v_factor + remap_v_min;
		}
		// if( s_remap & 0x4 )
		// {
		// }
	}

// COMPUTE NORMAL ONLY NOW
//todo deal with remap here too
#define ZMIN -.5

	vec3 nor;
#if 0
#	define D 8
	vec3 a1;
	compute_pos( uv + vec2(D/512.,0.), a1 );
	if( a1.z >= ZMIN )
		pos.z = 0.;

	vec3 b1;
	compute_pos( uv + vec2(0.,D/424.), b1 );
	if( b1.z >= ZMIN )
		pos.z = 0.;

	nor = normalize( cross( a1-pos, b1-pos) );
#else
	//todo the normal computation is shitty when there is no neighboor
	//todo revive eliminate point with no neighboor as an option: was 	if( abs(a1.z) >= OUT_LOW )		{	vert_block.depth = OUT;		return; 	}
	vec3 a1,a2;
	vec2 duv = vec2( pixel_factor, 0. ) * depth_tex_size_over;
	if( !compute_pos( uv+duv, a1 ) )
	{
		if( !compute_pos( uv-duv, a2 ) )
		{
			//todo make this an option
			vert_block.depth = OUT;	//mark the point as invalid
			return;
		}
		else
			a1 = pos - a2;
	}
	else
	{
		if( !compute_pos( uv-duv, a2 ) )
			a1 = a1 - pos;
		else
			a1 = a1 - a2;
	}

	vec3 b1,b2;
	duv = vec2( 0., pixel_factor ) * depth_tex_size_over;
	if( !compute_pos( uv+duv, b1 ) )
	{
		if( !compute_pos( uv-duv, b2 ) )
		{
			//todo make this an option
			vert_block.depth = OUT;	//mark the point as invalid
			return;
		}
		else
			b1 = pos - b2;
	}
	else
	{
		if( !compute_pos( uv-duv, b2 ) )
			b1 = b1 - pos;
		else
			b1 = b1 - b2 ;
	}

	nor = normalize( cross( b1, a1) );
#endif
	do_plan( nor, false );
	do_transfo( nor, false );
	vert_block.nor = nor;

//	pos.w = 1;
//	float d;

#if 0
	//eliminate where this invalid pixel nearby
	{
#	define STEP 2
#	define MINZ (.01*.125)

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
#	define STEP 2
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

// here we can do the curvature for circular screen like Nimes
#if 0
	float DM = 4.85; // distance center to curve wall
	float R = 6.;
	float f = 4.2;
	float z = R - DM - pos.z;
	float rot = atan( pos.x, z );
	float r = z / cos( rot );
	pos.x = f * rot;
	pos.z = R - DM - r;
#endif
	vert_block.pos_world = vec4( pos.xyz, 1. );

	// Built-in.
//	gl_Position = vert_block.pos;
}


