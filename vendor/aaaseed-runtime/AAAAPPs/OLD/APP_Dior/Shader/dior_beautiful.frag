//#version 330 compatibility

// GLSL fragment shader
// Apply texture

// --------------------------------------------------------------------------
// IN, OUT, INITIALIZATION

//	AAASeed uniform implicit
uniform sampler2D	aaa_tex2d[4];
//		0 image
//		1 blur
//		2 mask
//		3 image anim
//uniform sampler2D	aaa_samp0;
//uniform sampler2D	aaa_samp1;

//	AAAseed uniform pixel shader
//uniform float		aaa_fu_float[8];
//	0	level of hemi
//	1	level of mask/face_on on top
//	2	x y start/stop skin process on the u coordonate
//uniform int		aaa_fu_int[4];
//uniform vec4		aaa_fu_vec4[8];
// 01 xyzw define correction 0 before 1 after
//	the value is interpolated berween the 2
//	then
//		h += x
//		s += y
//		v += z*v + w*(1-v)
// 2 x y : hemi_min hemi_max vertical limit of the after fx
//  hemi_min have to be < hemi_max if we want to have 2 zones
//	hemi face 0 <-> left , 1 <-> right
//	hemi image reverse 1 <-> left , 0 <-> right
// 2 z : border at the hemi_a position 0 <-> none, 1 <-> max
// 2 w : unused

in VS_out
{
	vec2	uv_geo;
	vec2	uv;
	vec4	color;
} fs_in;


// --------------------------------------------------------------------------
// MAIN FUNCTION
#define VERT_FUZ .35

void main( void )
{
	vec2 xy, uv;

	if( aaa_fu_int[1] == 0 )
		uv = xy = fs_in.uv_geo;
	else if ( aaa_fu_int[1] == 1 )
		xy = uv = fs_in.uv;
	else
	{
		xy = fs_in.uv_geo;
		uv = fs_in.uv;
	}

	vec4 back = texture( aaa_tex2d[0], xy );
//	vec4 blur = texture( aaa_tex2d[1], xy );
	vec4 mask = texture( aaa_tex2d[2], uv );	//
	uv.s = 1. - uv.s;
	vec4 img  = texture( aaa_tex2d[3], uv );
//	commented be we use it this way later
//	uv.s = 1.- uv.s;

	int type = aaa_fu_int[0] & 0xff;
	//type = -6;
	if( type < 0 )
	{
		vec4 col = fs_in.color;

		if( type == -1 )	col *= back;
		if( type == -2 )	col = texture( aaa_tex2d[3], xy);
//		if( type == -2 )	col *= texture2D( aaa_tex2d[1], xy );
		if( type == -3 )	col *= mask;
		if( type == -4 )	col *= img;
		if( type == -5 )
		{
			col.rgb = mix( back.rgb, mask.bbb, mask.b );
			col.a = 1.;
		}
		if( type == -6 )
		{
			col.rgb = vec3( 1.- (mask.r - mask.b) );
			col.a = 1.;
		}
		gl_FragColor = col;
		gl_FragColor = col;
		//gl_FragColor.a = 1;
		//gl_FragColor.a = 1;
		return;
	}
	if( type == 3  )
	{
		gl_FragColor = img * fs_in.color;
		//gl_FragColor = vec4( 1,0,0,1);
		gl_FragColor.a *= 1.-(mask.r-mask.a);
		return;
	}

	//we compute a value used to apply hemi face or image
	float hemi_in = 0;
	{
		float u;
		if( aaa_fu_int[1] == 2 )		//hemi image use x
			u =  min( xy.x - aaa_fu_vec4[2].x, aaa_fu_vec4[2].y - xy.x );
		else if( aaa_fu_int[1] == 3 )	//hemi face use v
			u = -min( uv.x - aaa_fu_vec4[2].x, aaa_fu_vec4[2].y - uv.x );
		else
			u = 1.;

		float d = .02;	// hard coded
		if( u < d )
		{
			hemi_in = clamp( (d-u) / d, 0.0, 1.0 );
			//	hemi_in = 1;
		}
	}
	float hemi_fade = hemi_in * aaa_fu_float[0];
	float hemi_blur = hemi_fade * ( aaa_fu_int[2] == 0 ? 1. : mask.b );
	hemi_fade *= mask.r;

	vec4 col;
	if( hemi_blur > 0. )
	{
		//vec4 blur = texture( aaa_tex2d[1], xy );
		vec4 blur = textureLod( aaa_tex2d[1], xy, 0. );
		col = mix( back, blur, hemi_blur );
		//	go closer to detected average color in aaa_fu_vec4[3]
		vec3 hsv_target = rgb2hsv( aaa_fu_vec4[3].rgb );
		vec3 hsv_src = rgb2hsv( col.rgb );
		//this avoid jump in interpolation
		float dh 	= hsv_src.r -  hsv_target.r;
		if( dh >= .5 )
			hsv_target.r += 1.;
		else if( dh <= -.5 )
			hsv_target.r -= 1.;
		hsv_src.rgb = mix( hsv_src, hsv_target, aaa_fu_vec4[4].rgb * hemi_blur );
		col.rgb = hsv2rgb( hsv_src );
	}
	else
		col = back;

	{

		vec4 cor;
		if( (aaa_fu_int[0] & 0x100) == 0 )
			cor = mix( aaa_fu_vec4[0], aaa_fu_vec4[1], hemi_fade );
		else
			cor = mix( aaa_fu_vec4[0], aaa_fu_vec4[1], hemi_blur );
		//float f = 1.- (mask.r - mask.b);
		//if( hemi_fade > 0. )
		//			cor = mix( vec4( 0., 0., 1., 0.), cor, f );
		//cor = mix( aaa_fu_vec4[1], cor, hemi_fade );
		float att_factor = 1. + cor.z - cor.w;
		float att_offset = cor.w;

		vec3 mod = rgb2hsv( col.rgb );
		mod.xy += cor.xy;
		//mod.z *= (1.-cor.z);
		mod.z = clamp( mod.z * att_factor + att_offset, 0.0, 1.0 );
		col.rgb = hsv2rgb( mod );
	}

	if( type == 2 )
	{
		// f = mask.r;
		//img.rgb *= img.a;
		if( aaa_fu_int[3] == 1 )
			col = col 									+ aaa_fu_float[1] * img * .8;
		else
		{
			float v = mix( 1., img.a, aaa_fu_float[2] );
			col = col * ( 1. -  img.a * aaa_fu_float[1] )	+ aaa_fu_float[1] * img * v ;
		}
//		col = mix( back, col , f );
		col.a = 1.;
		//col.rgb = vec3( mask.a );
	}

	//border
	if( aaa_fu_vec4[2].z > 0. )
	{
		float white = clamp( 1.-abs(hemi_in*2.-1.) / aaa_fu_vec4[2].z, 0.0, 1.0 );
		if( aaa_fu_int[2] == 0 )
			col += vec4( white );
		else
			col = mix( col, vec4(1), white );
	}

//	col.xyz = pow( aaa_fu_float[1] * ( col.xyz - aaa_fu_float[0] ), vec3(aaa_fu_float[2], aaa_fu_float[3], aaa_fu_float[4]) );
/*
	if( type ==1 )
	{	//hemi
		u =  u * 100.;
		if( 0.< u && u < aaa_fu_vec4[2].z )
		{
			u = (1. - u/aaa_fu_vec4[2].z) * aaa_fu_vec4[2].w;
			col.rgb += vec3( u );
		}
	}
*/

	//	bottom go to black
	float v = xy.y - (1.-VERT_FUZ);
	if( v > 0. )
	{
		v = (VERT_FUZ-v)/VERT_FUZ;
		col.rgb *= vec3( v );
	}

	gl_FragColor = vec4( col.rgb, 1.) * fs_in.color;
//	gl_FragColor.rgb = vec3( hemi_blur );
}
