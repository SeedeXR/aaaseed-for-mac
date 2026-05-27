
//#version 330 compatibility
//#extension GL_EXT_gpu_shader4 : enable

// Uniform.
//uniform sampler2D	aaa_tex2d[2];
uniform sampler2D	aaa_samp0;
uniform sampler2D	aaa_samp1;


// Output.
out VERT_BLOCK
{
	vec4 	pos;
	vec2 	uv;
	vec3 	nor;
	float	depth;
} vert_block;

/*
vec2 camera_projected_uv(in mat4 p_mvp, in vec3 vec )
{
	vec4 v4 = p_mvp * vec4( vec, 1.0 );
	return v4.xy / v4.w;
}
*/
#define OUT 100.
#define OUT_LOW (OUT - 1.)
void compute_pos( in vec2 uv, out vec3 pos )
{
#if 1
	//	DEPTH TO KINECT
	//	depth is positive for now
	float depth = texture( aaa_samp0, uv ).r * 8.;
	if( depth <= aaa_vu_float[1] || aaa_vu_float[2] <= depth )
	{
		pos.z = OUT;
	}
	else
	{
		//	depth is now negative like a good opengl camera
		//pos.xy =  texture2D( aaa_tex2d[1], pos.xy, 0 ).rg * depth;
		pos.xy = -texture( aaa_samp1, vec2( uv.x, uv.y ) ).rg * depth;
		pos.z = -depth;
		//pos.xy = uv;
	}
//	pos.z = sin( pos.x *5. ) * ;
//	pos.z = -depth;
//	pos.xy = uv;
//	pos.z = -1;
#else
	pos.xy = texture( aaa_samp1, uv ).rg - (2.*uv-1.) * vec2(0.685,-0.632);
	pos.xy *= 10.;
	//pos.xy = (uv-.5) * 2. * vec2(0.685,-0.632);
	pos.z = -1.;
#endif
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

	vec3 pos;
	compute_pos( uv, pos );
	if( abs(pos.z) >= OUT_LOW )
	{
		vert_block.depth = OUT;
		return;
	}

	vert_block.depth = 	-pos.z;	//store before we change it

//	if( true )
	{
	// rotation on z
		rotate2_cos_sin( pos.xy, aaa_vu_vec4[0].xy );
	//	first translation (center)
		pos.xyz	+=  aaa_vu_vec4[5].xyz;
	// rotation on x
		rotate2_cos_sin( pos.yz, aaa_vu_vec4[0].zw );
		//pos.yz	=	aaa_vu_vec4[0].xy * pos.yz	+	aaa_vu_vec4[0].zw * pos.zy;
	// rotation on y
		pos.xz	=	aaa_vu_vec4[1].xy * pos.xz	+	aaa_vu_vec4[1].zw * pos.zx;
	// swap xy
		if( aaa_vu_int[0] == 1 )
		{
			pos.yz = pos.zy;	pos.x = -pos.x;
		}
	// translation xyz
		pos.xyz	+=  aaa_vu_vec4[2].xyz;
	// rotation on z
		pos.xy	=	aaa_vu_vec4[3].xy * pos.xy	+	aaa_vu_vec4[3].zw * pos.yx;
	// scaling on xy
		pos.xy	*=	aaa_vu_vec4[4].xy;
	}

	vec3 tmp = clamp( pos.xyz, aaa_vu_vec4[6].xyz, aaa_vu_vec4[7].xyz );
	if( tmp != pos.xyz )
	{
		vert_block.depth = OUT;
		return;
	}

//compute normal only now
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
	vec3 a1,a2;
	compute_pos( uv + vec2(aaa_vu_float[0]/512.,0.), a1 );
//	if( abs(a1.z) >= OUT_LOW )		{	vert_block.depth = OUT;		return; 	}
	compute_pos( uv - vec2(aaa_vu_float[0]/512.,0.), a2 );
//	if( abs(a2.z) >= OUT_LOW )		{	vert_block.depth = OUT;		return; 	}

	vec3 b1,b2;
	compute_pos( uv + vec2(0.,aaa_vu_float[0]/424.), b1 );
//	if( abs(b1.z) >= OUT_LOW )		{	vert_block.depth = OUT;		return; 	}
	compute_pos( uv - vec2(0.,aaa_vu_float[0]/424.), b2 );
//	if( abs(b2.z) >= OUT_LOW )		{	vert_block.depth = OUT;		return;		}

	vec3 nor = normalize( cross( b1-b2, a1-a2) );
#endif

//	if( true )
	{
	// rotation on z
		rotate2_cos_sin( nor.xy, aaa_vu_vec4[0].xy );
	// rotation on x
		rotate2_cos_sin( nor.yz, aaa_vu_vec4[0].zw );
		//	nor.yz	=	aaa_vu_vec4[0].xy * nor.yz	+	aaa_vu_vec4[0].zw * nor.zy;
	// rotation on y
		nor.xz	=	aaa_vu_vec4[1].xy * nor.xz	+	aaa_vu_vec4[1].zw * nor.zx;
	// swap xy
		if( aaa_vu_int[0] == 1 )
		{
			nor.yz = nor.zy;	nor.x = -nor.x;
		}
	// translation xyz
	// rotation on z
		nor.xy	=	aaa_vu_vec4[3].xy * nor.xy	+	aaa_vu_vec4[3].zw * nor.yx;
	// scaling on xy
		nor.xy	*=	aaa_vu_vec4[4].xy;
	}
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
	vert_block.pos = vec4( pos.xyz, 1. );

	// Built-in.
//	gl_Position = vert_block.pos;
}


