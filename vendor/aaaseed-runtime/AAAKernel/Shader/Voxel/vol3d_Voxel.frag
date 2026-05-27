//#version 330 compatibility

//#extension GL_ARB_shading_language_include : enable

int LFSR_Rand_Gen(in int n)
{
	n = (n << 13) ^ n;
	return (n * (n*n*15731+789221) + 1376312589) & 0x7fffffff;
}

float noise3f(in vec3 p)
{
	ivec3 ip = ivec3(floor(p));
	int n = ip.x + ip.y*57 + ip.z*113;
	return float(LFSR_Rand_Gen(n))/1073741824.;
}

in VS_out
{
//	mat4			mv_inverse;
	vec4 			pos_world;
	vec4			color;
	vec4			ecPosition;
	vec3			nor_world;
	vec3			uvw;
//	vec3			normal;
	vec3 			x_world;
	vec3 			y_world;
	vec3 			z_world;
	vec3 			ray;
	vec3			light_in;
//	float			fall_off;
//	float			z_to_eye;
} fs_in;

//	output
out vec4	fragColor;

//	AAASeed uniform implicit
uniform sampler3D	aaa_samp0;
uniform sampler2D	aaa_samp1;
uniform sampler3D	aaa_samp2;
//uniform sampler3D	aaa_tex3d[4];
//uniform sampler2D	aaa_tex2d[4];
//uniform sampler3D	aaa_tex3d[4];

//	AAAseed uniform pixel shader
//uniform float		aaa_fu_float[8];
//self.f_color, alpha, 1./s_axe, self.palette_v 
CONST float f_color		= aaa_fu_float[0];
CONST float f_alpha		= aaa_fu_float[1];
//	2	size_factor on w axe
CONST float fz			= aaa_fu_float[2];
CONST float palette_v	= aaa_fu_float[3];

CONST vec4	color_scale	= vec4( vec3(f_color), f_alpha );

//uniform int			aaa_fu_int[4];
CONST int	s_how		= aaa_fu_int[0];
//	1	step_nb
CONST int	step_nb		= aaa_fu_int[1];
//	2 : 0 grey, 1 color using palette
CONST bool	b_map_grey	= aaa_fu_int[2]!=0;
CONST bool	b_use_green	= aaa_fu_int[3]!=0;

//uniform vec4		aaa_fu_vec4[8];
CONST vec3	crop_min	= aaa_fu_vec4[0].xyz;
CONST vec3	crop_max	= aaa_fu_vec4[1].xyz;
//	2	alpha_min,	alpha_max,	alpha_stop
CONST vec3	color_light	= aaa_fu_vec4[7].rgb;
CONST float f_acc		= aaa_fu_vec4[7].a;


const vec4 color_dbg[4] = vec4[4]	(	vec4( 1,1,1, 1 ),	vec4( 1,0,0, 1 ),	vec4( 0,1,0, 1 ),	vec4( 0,0,1, 1 ) );
vec4 get_color_dbg( in int unit )	{	return color_dbg[ aaa_tex_dim[unit] ];	}


vec3 get_gradient_3d( sampler3D samp, vec3 at, vec3 d )
 {	// 42 correct a black bug !!!!	
	// at-delta then at+delta tested in condition (Maa October 2023)
	vec4 d4 = vec4( d, 0 );
 	if( b_use_green )
		return vec3(	texture( samp, at - d4.xww ).r - texture( samp, at + d4.xww ).r + .00042,
						texture( samp, at - d4.wyw ).r - texture( samp, at + d4.wyw ).r,
						texture( samp, at - d4.wwz ).r - texture( samp, at + d4.wwz ).r
					);
	else
		return vec3(	texture( samp, at - d4.xww ).g - texture( samp, at + d4.xww ).g + .00042,
						texture( samp, at - d4.wyw ).g - texture( samp, at + d4.wyw ).g,
						texture( samp, at - d4.wwz ).g - texture( samp, at + d4.wwz ).g
					);
}
vec3 get_normal( sampler3D samp, vec3 at, vec3 d )
 {		
	return normalize( get_gradient_3d( samp, at, d ) );
}

//float	get_red( sampler3D samp, vec3 at )	{	return texture( samp, at ).r;	}
//float	get_red0( vec3 at )					{	return texture( aaa_samp0, at ).r;	}
//float	get_red1( vec3 at )					{	return texture( aaa_samp2, at ).r;	}
vec4	map_grey( float grey )				{	return texture( aaa_samp1, vec2( grey, palette_v ) );	}
vec4	get_color0( vec3 at )
{
	vec4 col = texture( aaa_samp0, at );
	float g = b_use_green ? col.g : col.r;

	//todo should be in the texture
	if( g == clamp( g, aaa_fu_vec4[2].x, aaa_fu_vec4[2].y ) )
		return b_map_grey ? map_grey(g) : vec4(g);
	else
		return vec4(0);
}
vec4	get_color1( vec3 at )				{	return texture( aaa_samp1, vec2( texture( aaa_samp2, at ).r, aaa_fu_float[4] ) );	}

#define TEST_SKIP														\
{																		\
	if( any	( bvec2	(	any( lessThan( uvw, aaa_fu_vec4[0].xyz ) ),		\
						any( greaterThan( uvw, aaa_fu_vec4[1].xyz ) )	\
					)													\
			) )															\
	{																	\
		break;															\
	}																	\
}
#define BEGIN_LOOP		while( true )	{	TEST_SKIP;
#define END_LOOP		uvw += d_uvw;	}

#define	ACCUMULATE						\
	tex.rgb *= tex.a;					\
	acc += (1.0 - acc.a) * tex;			\
	if( acc.a >= aaa_fu_vec4[2].z )		\
		break;

#define	SCALE_COLOR		tex *= color_scale;
//#define BEGIN_LOOP_CH1	BEGIN_LOOP	tex = get_color0( uvw );	if( tex.a > 0. )	{	tex.a = pow( tex.a, aaa_fu_float[4] ); SCALE_COLOR
#define BEGIN_LOOP_CH1	BEGIN_LOOP	tex = get_color0( uvw );	if( tex.a > 0. )	{	SCALE_COLOR
#define END_LOOP_CH1	ACCUMULATE	}	END_LOOP	if( b_map_grey ) { tex.rgb = map_grey(tex.a).rgb; }

void main (void)
{
//	discard;
	vec3 uvw = fs_in.uvw;

	if( s_how <= 0 )
	{
		if( s_how == 0 )
		{
			fragColor.xyz = uvw;
			fragColor.a = 1.;
		}
		else
		{	//color according to dim unit
			fragColor = get_color_dbg( -s_how - 1 );
		}
		return;
	}


	vec3 d_uvw;
#if 0
	vec4 ecPosition = gl_ModelViewMatrix * fs_in.pos_world;
	vec3 ec = ecPosition.xyz;
#else
//	vec3 ec = normalize( fs_in.ecPosition.xyz );
	vec3 ec = fs_in.ecPosition.xyz;
//	vec3 ec = (gl_ModelViewMatrix * fs_in.pos_world).rgb;
//	vec3 ec = ecPosition.xyz;
//	vec3 ec = ecPosition.xyz / ecPosition.z;
#endif
	vec3 ec_nor = -ec / ec.z;
#	if 0
		vec3 nor = normalize( gl_NormalMatrix * fs_in.nor_world );
		d_uvw = refract( ec.xyz, -nor, 1 );
#	else
		d_uvw.x = dot( fs_in.x_world, ec_nor );
		d_uvw.y = dot( fs_in.y_world, ec_nor );
		d_uvw.z = dot( fs_in.z_world, ec_nor );
#	endif
	d_uvw.z *= fz;
//	float m = min( min( d_uvw.x, d_uvw.y ), d_uvw.z );
//	d_uvw /= m;
//	d_uvw = normalize( d_uvw );

//	texture = texture2D( aaa_tex2d[0], fs_in.tex_coor[0].st );
	int nb_it = int( step_nb * fz );

	float alpha = 0.;
	float alpha_acc = 0.;
//	bool b_before = false;
//	bool b_after = false;
	// sqrt(3) is there so in worst case we deal with cube diagonal
	float nb_it_over =  1./ nb_it;

	float f = 1.732 * nb_it_over;	//todo 1.732 ?
//	float f = .75 / nb_it;
	d_uvw *= f;

	float cor = ( -ec.z - floor( -ec.z / f ) * f ) / f;
	uvw += d_uvw * ( 1. - cor );

	uvw += d_uvw * noise3f( uvw*80000. )*.5;

	//	use this to translate and rotate
	//uvw = aaa_fu_vec4[2].xyz + uvw * aaa_fu_vec4[3].xyz;
	//d_uvw *= aaa_fu_vec4[3].xyz;

	bool b_in = false;
	while( true )
	{
		if(			all	( bvec2	(	all( greaterThanEqual(	uvw, crop_min ) ),
									all( lessThanEqual(		uvw, crop_max ) )	)	)	)
		{
			break;
		}
		else if(	any	( bvec2	(	any( lessThan(			uvw, vec3(0) ) ),
									any( greaterThan(		uvw, vec3(1) ) )	)	)	)
		{
			discard;
		}
		uvw += d_uvw;
	}

	vec4 tex;
	vec4 acc = vec4(0.);

	vec3 dn = vec3( 1,1,fz ) * f * aaa_fu_float[7];

	if( s_how == 1 )
	{
		BEGIN_LOOP_CH1
		END_LOOP_CH1
		//tex.rgb = map_grey( acc.r ).rgb;
//		tex.rgb = h2rgb( acc.r );
	}
	else if( s_how == 2 )	// normal -1 1
	{
		BEGIN_LOOP_CH1
			vec3 nor = gl_NormalMatrix * get_normal( aaa_samp0, uvw, dn );
			tex.rgb += nor.rgb * f_acc;
		END_LOOP_CH1
	}
	else if( s_how == 3 )	// normal 0 1
	{
		BEGIN_LOOP_CH1
			vec3 nor = gl_NormalMatrix * get_normal( aaa_samp0, uvw, dn );
			tex.rgb += (nor.rgb * .5 + .5 ) * f_acc;
		END_LOOP_CH1
	}
	else if( s_how == 4 )	// light
	{
		vec3 col_acc = color_light.rgb * f_acc;
		BEGIN_LOOP_CH1
			vec3 nor = gl_NormalMatrix * get_normal( aaa_samp0, uvw, dn );
			float ang = -dot( nor, fs_in.light_in );
			if( ang > 0 )
				tex.rgb += col_acc * ang;
		END_LOOP_CH1
	}
	else if( s_how == 5 )
	{
		BEGIN_LOOP_CH1
			tex.rgb += gl_NormalMatrix * get_gradient_3d( aaa_samp0, uvw, dn ) * f_acc * 100.;
		END_LOOP_CH1
	}
	else if( s_how == 6 )
	{
		BEGIN_LOOP
			tex = get_color0( uvw );
			if( tex.a > 0. )
			{
				SCALE_COLOR
				vec3 nor = normalize( gl_NormalMatrix * get_normal( aaa_samp0, uvw, dn ) );
				//float ang = -dot( nor, fs_in.light_in );
				//if( ang > 0 )
//					tex.rgb += aaa_fu_vec4[7].rgb * f_acc * ang;
					tex.rgb += (nor.rgb * .5 + .5 ) * f_acc;
					//col.rgb = abs( get_gradient_3d( aaa_samp0, uvw, f ) );
					//col.rgb = vec3( max( max( col.r, col.g ), col.b ) );
				ACCUMULATE
					// Accumulate RGB : acc.rgb = voxelColor.rgb*voxelColor.a + (1.0 - voxelColor.a)*acc.rgb;
		        	//acc.rgb = mix(acc.rgb, voxelColor.rgb, voxelColor.a)*LightIntensity;
		 			// Accumulate Opacity: acc.a = acc.a + (1.0 - acc.a)*voxelColor.a;
		 			//acc.a = mix(voxelColor.a, 1.0, acc.a);
			}
		END_LOOP
	}
	else if( s_how == 7 )
	{
		vec4 col = vec4(0.);
		vec4 col_acc = vec4(0.);

		for( int i=0; i<nb_it; i++ )
		{
			TEST_SKIP

			col = get_color0( uvw );
			alpha = col.r * color_scale.r;

			col.rgb = abs( get_gradient_3d( aaa_samp0, uvw, dn ) );
			col.rgb = vec3( max( max( col.r, col.g ), col.b ) );
			col.rgb = h2rgb( palette_v * col.r * .16666 + aaa_fu_float[2] );
			col_acc.rgb += (1.0 - alpha_acc) * col.rgb * alpha * f;
			alpha_acc += alpha * f;

			// Accumulate RGB : acc.rgb = voxelColor.rgb*voxelColor.a + (1.0 - voxelColor.a)*acc.rgb;
        	//acc.rgb = mix(acc.rgb, voxelColor.rgb, voxelColor.a)*LightIntensity;
 			// Accumulate Opacity: acc.a = acc.a + (1.0 - acc.a)*voxelColor.a;
 			//acc.a = mix(voxelColor.a, 1.0, acc.a);

			if( alpha_acc >= 1. )
				break;
			uvw += d_uvw;
		}
		tex.rgb = col_acc.rgb;
		tex.a = alpha_acc;
	}
	else if( s_how == 8 )
	{
		vec4 col = vec4(0.);
		vec4 col_acc = vec4(0.);

		for( int i=0; i<nb_it; i++ )
		{
			TEST_SKIP

			col = get_color0( uvw ) + get_color1( uvw );
			alpha = col.r * color_scale.r;

			col.rgb = abs( get_gradient_3d( aaa_samp0, uvw, dn ) );
			col.r = max( max( col.r, col.g ), col.b );

			tex.rgb = abs( get_gradient_3d( aaa_samp2, uvw, dn ) );
			//col.g = max( max( tex.r, tex.g ), tex.b );
			col.g = tex.b;

			col_acc.rgb += (1.0 - alpha_acc) * col.rgb * alpha;
			alpha_acc += alpha;

			// Accumulate RGB : acc.rgb = voxelColor.rgb*voxelColor.a + (1.0 - voxelColor.a)*acc.rgb;
        	//acc.rgb = mix(acc.rgb, voxelColor.rgb, voxelColor.a)*LightIntensity;
 			// Accumulate Opacity: acc.a = acc.a + (1.0 - acc.a)*voxelColor.a;
 			//acc.a = mix(voxelColor.a, 1.0, acc.a);

			if( alpha_acc >=1. )
				break;
			uvw += d_uvw;
		}
		tex.rgb = col_acc.rgb;
		tex.a = alpha_acc;
	}
	else if( s_how == 9 )	// Tim
	{
		vec4 col = vec4(0.);
		vec4 col_acc = vec4(0.);

		for( int i=0; i<nb_it; i++ )
		{
			TEST_SKIP

			acc.rgb = get_color0( uvw ).rgb; // + get_color1( uvw );
			acc.a = col.r * color_scale.r;

		}
	}
///	tex.xyz = uvw;
//	tex.a = 1.;
//#if 1
	//texture = vec4( 1, 1, 0, 1);

	//gl_FrontColor = blend( texture );
//	texture = texture2D( aaa_tex2d[1], fs_in.tex_coor[1].st );
	//texture *= fs_in.color;
//#endif
#if 0
	texture = compute_fog4( texture, fs_in.z_to_eye );
#endif
//	fragColor.rgb = col_acc;
//	tex.a = alpha_acc;
	fragColor = acc * fs_in.color;
//	fragColor.a = fs_in.color.a;
}
