

layout(location = 0) in ST_AAA_BV BV;

CONST int draw_sx = aaa_vu_int[0];
CONST int draw_sy = aaa_vu_int[1];
CONST int draw_ox = aaa_vu_int[2];
CONST int draw_oy = aaa_vu_int[3];

CONST bool b_ndc = draw_sx != 0;

CONST int size_offset_u = int(aaa_fu_float[14]);
CONST int size_offset_v = int(aaa_fu_float[15]);


//Maa july 2012
uniform sampler2D	aaa_tex2d[4];

#define TEX_0 		aaa_tex2d[0]
CONST ivec2 tex_0_size = textureSize( TEX_0, 0 );
// Uniform variables for texturing

//	aaa_fu_vec4[0]
//uniform vec4	aaa_fu_vec4[6];
//	aaa_fu_vec4[0]
	//	0	gamma red
	//	1	gamma green
	//	2	gamma blue
	//	3	gamma fuzzy
CONST vec3	color_gamma = aaa_fu_vec4[0].rgb;
CONST vec4	color_asked = aaa_fu_vec4[1];
//	aaa_fu_vec4[1,2]
	//	0	texture u_offset
	//	1	texture v_offset
	//	2	texture u_size
	//	3	texture v_size

//uniform float	aaa_fu_float[8];
//	0	:	left	( 0 nothing / 1 all texture )
//	1	:	right	idem
//	2	:	bottom	idem
//	3	:	top	idem
//	5	:	multiplication factor to help adjust interactivly

//uniform int		aaa_fu_int[6];
//	0	:	type
CONST int	s_type = aaa_fu_int[0];
CONST bool	b_fuzzy = aaa_fu_int[1]!=0;
//		-1	just texture
//		0	fuzzy edge + texture
//		1	just fuzzy edge
//	1	:	use multiplcation

CONST vec4 fuzzy_lrbt  = aaa_fu_vec4[4];
CONST vec4 fuzzy_gamma = aaa_fu_vec4[5];


#define FUZZY_OPT 1
#if FUZZY_OPT < 1

	float	do_fuzzy_one( float v, vec2 factor, vec2 gamma )
	{
#	if 1
		vec2 tmp2 = vec2( v, 1.-v ) * factor.xy;
		tmp2 = pow( clamp_01(tmp2), gamma.xy );
		return min( tmp2.x, tmp2.y );
#	else	// need more arg: le and ri
		if( v < le )
			v = pow( v * factor.x, gamma.x );	return col.rgb * min( min( vec.x, vec.y ), min( vec.z, vec.w ) );
		else if( ri < v )
			v = pow( (1.-v) * factor.y, gamma.y );
		else
			v = 1.;
		return v;
#	endif
	}

	vec3 do_fuzzy( vec3 col, vec2 uv )
	{
	//	float x = get_att_hack( uv.x, 0 );
	//	float x = get_att( uv.x, 0 );
	//	float y = get_att( uv.y, 2 );

		vec2 v2;
		v2.x = do_fuzzy_one( uv.x, fuzzy_lrbt.xy,  fuzzy_gamma.xy );
		v2.y = do_fuzzy_one( uv.y, fuzzy_lrbt.zw,  fuzzy_gamma.zw );
		return col.rgb * ( v2.x* v2.y );
	}

#else	// FUZZY_OPT

	vec3 get_fuzzy( vec2 uv )
	{
		vec4 vec = vec4( uv.x, 1.-uv.x, uv.y, 1.-uv.y ) * fuzzy_lrbt;
		vec = pow( clamp_01( vec ), fuzzy_gamma );
		return vec3( min( vec.x, vec.y ) * min( vec.z, vec.w ) );
		//return col.rgb * min( min( vec.x, vec.y ), min( vec.z, vec.w ) );
	}

#endif	// FUZZY_OPT

#define UV_FACTOR 4
void main(void)
{
	vec4 dst;
	vec2 uv;
	if( b_ndc )
	{	//todo this far from perfect the logic should be addressed in lua 
		uv = gl_FragCoord.xy;
		vec2 tex_size = tex_0_size + vec2( size_offset_u, size_offset_v );

		uv.y -= (draw_sy-tex_size.y) + draw_oy;
		//uv.y = -uv.y + (draw_sy-tex_size.y) + draw_oy;
		if( uv.y < 0 || tex_size.y < uv.y )
			discard;

		uv.x -= draw_ox;
		if( uv.x < 0 || tex_size.x < uv.x )
			discard;

		uv /= tex_size;
	}
	else
	{
		uv = BV.tex_coor[0].st;
	}
	
//	uv *= UV_FACTOR;
	
	if( s_type == 1 )
	{
		vec2 uvt = mix( aaa_fu_vec4[2].xy, aaa_fu_vec4[2].zw, uv );
		dst = texture2D( TEX_0, uvt ) * color_asked;
	}
	else if( s_type == 0 )
		dst = texture2D( TEX_0, uv ) * color_asked;
	else
	{
		switch( s_type )
		{
		//color
		case 512:
		case 513:	dst = color_asked;	break;
		//white
		case -768:
		case -769:	dst = vec4(1);			break;
		//uv
		case -1024:	dst = vec4( uv,0,	1);	break;
		//uv cropped
		case -1025:	
			{
				vec2 uvt = mix( aaa_fu_vec4[2].xy, aaa_fu_vec4[2].zw, uv );
				dst = vec4( uvt,0,	1);
			}
			break;
		//black
		default: 	dst = vec4(0);	break;
		}
	}

	//gamma
	dst.rgb = pow( dst.rgb, color_gamma );

	if( aaa_fu_int[1]==1 )
		dst.rgb *= get_fuzzy( uv );


//todo switch in the ui between this 2 mode
// #if 1
// 	dst.rgb *= color_asked.a;
// 	dst.a = 1.;
// #else
// 	dst.a *= color_asked.a;
// #endif

	gl_FragColor = dst;

//	gl_FragColor = vec4(0,1,.5, .15 );

//	texture = get_texture( gl_TexCoord[0].st );
//	gl_FragColor = texture;
	//gl_FrontColor = blend( texture );
}
