
//	#define color_factor		aaa_fu_vec4[0]

//pipeline inputs
layout(location = 0) in vec2 in_texcoord;
layout(location = 1) in vec4 in_color;

//outputs
layout(location = 1) out vec3 out_result;	// flipflop buffer
layout(location = 0) out vec4 out_render;	// where we draw the final output
//texture inputs
layout(binding = 0) uniform sampler2D TEX_PREVIOUS;	// buffer we fliflop to computee the next frame from previous
layout(binding = 1) uniform sampler2D TEX_SRC;		// input used for presence also the image src
layout(binding = 2) uniform sampler2D TEX_COLORMAP;	//

CONST float corner_weight = 0.7071067812;
CONST float sum_factor = 1. / (4. + 4. * corner_weight);

//CONST float feed 				= aaa_fu_float[0];
//CONST float kill				= aaa_fu_float[1];
CONST float kf_nb_over_one		= aaa_fu_float[0];
CONST float grey_gamma 			= aaa_fu_float[1];
CONST float dif_a				= aaa_fu_float[2];
CONST float dif_b				= aaa_fu_float[3];

CONST float lap_size_min		= aaa_fu_float[4];
CONST float lap_size_range 		= aaa_fu_float[5];
CONST float lap_size_gamma 		= aaa_fu_float[6];

CONST float kf_grey_min			= aaa_fu_float[7];
CONST float kf_grey_range 		= aaa_fu_float[8];

CONST float out_min				= aaa_fu_float[9];
CONST float out_factor 			= aaa_fu_float[10];
CONST float out_mul 			= aaa_fu_float[11];
CONST float emboss_dist 		= aaa_fu_float[12];
CONST float emboss_min			= aaa_fu_float[13];
CONST float emboss_factor 		= aaa_fu_float[14];
CONST float erase_radius 		= aaa_fu_float[23];

CONST int	s_how				= aaa_fu_int[0] & 0x0f;
CONST int	s_lap				= (aaa_fu_int[0] & 0xf0) >> 4;
CONST bool	b_clear				= (aaa_fu_int[0] & 0x100) != 0;
CONST int	s_out				= aaa_fu_int[2] & 0x0f;
CONST int	s_out_mul			= (aaa_fu_int[2] & 0xff00) >> 8;
CONST bool	b_out_inv			= (aaa_fu_int[2] & 0x10000) != 0;
CONST bool	b_emboss			= (aaa_fu_int[2] & 0x20000) != 0;
CONST bool	b_grad				= (aaa_fu_int[2] & 0x40000) != 0;
CONST bool	b_final				= aaa_fu_int[3]!=0;

#define		res					(aaa_fu_vec4[0].xy)
#define		dxy					(aaa_fu_vec4[0].zw)
#define	  	kf_in(i)			(aaa_fu_vec4[i+1].xy)

//const float dif_a = .8;
//const float dif_b = .2;

//const float feed = .140;
//const float kill = .052;

// uv Normalized pixel coordinates (from 0 to 1)
vec4 rd_blur_variable( in sampler2D tex, in vec2 uv, in float size )
{   
    // GAUSSIAN BLUR SETTINGS {{{
    const int dir_nb = 32; // BLUR DIRECTIONS (Default 16.0 - More is better but slower)
    const int rad_nb = 32; // BLUR QUALITY (Default 4.0 - More is better but slower)
 //   const float Size = 16.0; // BLUR SIZE (Radius)
    // GAUSSIAN BLUR SETTINGS }}}
   
    vec2 radius = dxy * size / float(rad_nb);
    
    // Pixel colour
	//ok for blur wrong for laplacian
    vec4 color = texture( tex, uv );
    
    // Blur calculations
	float da = PI2 / float(dir_nb);
	float dr = size / float(rad_nb);
    for( int ia=0; ia<dir_nb; ++ia )
    {
		float a = da * ia;
		vec2 dir = vec2( cos(a), sin(a) ) * radius;
		for( int ir=1; ir<=rad_nb; ++ir )
        {
			color += texture( tex, uv + dir*ir  );		
        }
    }
    
    // Output to screen
    return color / float( dir_nb * rad_nb ); 
}

 vec4 do_out( vec2 uv, in vec2 duv )
 {
	vec4 tex = texture( TEX_PREVIOUS, uv );

	float v = (tex.y - out_min) * out_factor;
	if( b_out_inv )
		v = 1. - v;
	
	duv *= emboss_dist;
	vec4 n = texture( TEX_PREVIOUS, uv + duv * vec2(0,1) );
	vec4 e = texture( TEX_PREVIOUS, uv + duv * vec2(1,0) );
    vec4 s = texture( TEX_PREVIOUS, uv - duv * vec2(0,1) );
    vec4 w = texture( TEX_PREVIOUS, uv - duv * vec2(1,0) );

    //vec2 h = vec2(e.z-w.z,n.z-s.z);
   //	vec2 h = vec2(0.);
    vec2 f = vec2( e.x-w.x, n.x-s.x );
	vec2 g = vec2( e.y-w.y, n.y-s.y) ;

 //	vec4 dx = 0.25*(n+e+s+w) - tex;

    //vec3 no = normalize(vec3(5.*h+g,-1));
    vec3 no = vec3( f * 5. + g * 5., .5 );
	no = normalize( no );

    vec3 r = reflect(vec3(0,0,1),no);

    vec3 l = vec3( 2.*res, 3.*res.y );
    vec3 u = vec3( uv*res, 0 );
    vec3 lu = l-u;
    float emboss = length( r - lu * dot(r,lu) / dot(lu,lu) );

	vec4 Q;
	switch(s_out)
	{
	case 1: Q = vec4( v,v,v, 1 );				break;	//grey
	case 2:	Q = texture( TEX_COLORMAP, vec2( uv.x, v ) );	break;
	case 3:	v = clamp_01(v);
	case 4:
		//v = pow( v, .25 );
		Q = sin(  -.2 + v * vec4(8,2,0,0) );
		//Q += sin( .1 + tex.x * vec4(-5,4,0,0) );
		//Q = vec4( -.5 + b.x * 3.);
		//Q = vec4( -.2 + b.y * 3.);
		break;
	case 5: Q.xyz = vec3( (emboss-emboss_min)*emboss_factor ); break;
	case 6: Q.xyz = vec3( 3.5 * exp(-2.*emboss) + 60.*exp(-10.*emboss) ); break;
	// case 7:
	// 	v = (tex.x - out_min) * out_factor;
	// 	if( !b_out_inv )
	// 		v = 1. - v;	
	// 	Q = vec4( v,v,v, 1 );
	// 	break;
	case 7: Q = vec4( vec3(tex.r), 1. );	break;
	case 8: Q = vec4( vec3(tex.g), 1. );	break;
	case 9: Q = vec4( tex.rg, 0., 1. );		break;
	case 10: Q = vec4( tex.rgb, 1. );		break;
	case 11: Q = vec4( vec3( clamp_01(tex.b) ), 1. );	break;
	case 12: Q = vec4( 1 );					break;	//white
	}
	if( b_emboss )
	{
		if( s_out!=5 && s_out!= 6 )
		//o = out_emboss_factor*(1.-emboss) + emboss_min;
			Q.xyz *= (emboss-emboss_min) * emboss_factor;
		//Q.xyz *= emboss_factor * exp(-2.*o) + 60.*exp(-10.*o);
	}

	Q.w = 1.;

	//todo
	//out_render = mix( out_render, src, result.y*2	 );
	switch( s_out_mul )
	{
	case 1:	Q = mix( Q, Q*texture( TEX_SRC, uv ), out_mul ); break;
	case 2:	Q += texture( TEX_SRC, uv ) * out_mul; break;
	case 3:	Q.rgb -= texture( TEX_SRC, uv ).rgb * out_mul; break;
	case 4:	Q.rgb = texture( TEX_SRC, uv ).rgb * out_mul - Q.rgb; break;
	}	//out_render = min( out_render, src );		
	return Q;
}

void main()
{
	vec2 uv = in_texcoord;

	vec4 prev	= texture( TEX_PREVIOUS,	uv );
	vec4 src	= texture( TEX_SRC,			uv );

	if( s_how != 0 )
	{
		switch( s_how )
		{
		case 1: 	out_render = prev;	break;
		case 2: 	out_render = src;	break;
		}
		return;
	}

	if( b_clear )
	{
		//out_result = vec4( 0,fract(uv.x*16)*fract(uv.y*16),0, 1 );
		out_result = vec3( 0, abs(2.2*sin(uv.y*PI2*25)), 0 );
	}
	else if( iMouse.z > 0. && length( (uv-iMouse.xy)*res ) < res.y*erase_radius )
	{
		out_result = vec3( 1,0,0 );
	}
	else
	{
		float g;
		if( b_grad )
			g = get_gradient_norm_2d( TEX_SRC, uv );
		else
			g = gray(src.xyz);

		vec4 lap;

		float fs;
		if( lap_size_range != 0. )
			fs = lap_size_min + lap_size_range * pow( g, lap_size_gamma ); //todo 1-g
		else
			fs = lap_size_min;

		switch( s_lap )
		{
		default:
		case 0:	lap = rd_blur_3x3( TEX_PREVIOUS, uv, fs*dxy );	break;
		case 1: lap = rd_blur_5x5( TEX_PREVIOUS, uv, fs*dxy );	break;
		case 2: lap = rd_blur_variable( TEX_PREVIOUS, uv, fs );	break;	
		case 3:	
				//blur_5x5_size( lap, uv + get_gradient_2d( TEX_PREVIOUS, uv) * .000005, pow(1-g,2.)*4+ .2 );	break;
				lap = rd_blur_5x5( TEX_PREVIOUS, uv + vec2( 0,.0000 * sin(uv.x*6.) ), fs*dxy );	break;

		}
		lap.xy -= prev.xy; 

		vec3 result = prev.xyz;
		float reaction = prev.x * prev.y * prev.y; //pow( prev.y , 2.) ;
		//float reaction = pow( prev.y, 2.) * prev.x;

		//float f = feed * (.40 + (g*4) * .22);
		//g -= result.z * .001 ;
		g = pow( g, grey_gamma );
		g = kf_grey_min + g * kf_grey_range;
		g = g * kf_nb_over_one;

		float	r = fract( g );
		int 	ig = int( g );
		vec2	kf = mix( kf_in(ig), kf_in(ig+1), r );
		
		//kf = vec2(.018,.0) + uv * vec2( .074-.018, .112 );
		//kf = kf_in(0) + uv * (kf_in(1)-kf_in(0));
		//small exploration
		//kf = vec2(.0125,.215) + uv * vec2( .0025, .005 );

		//float k = kill * (.80 + (src.y) * .22);
		//float f = mix( kf.y, kf.w, g );
		//if( result.z > 1. )
		//{
		//	kf.x -= result.z * .0001 ;
		//}
		//r = 1-sqrt(r)*1.5;

		result.x += 1. * ( dif_a * lap.x - reaction +  kf.y         * (1-prev.x) );
		result.y += 1. * ( dif_b * lap.y + reaction - (kf.x + kf.y) *  prev.y    ); // + fract(uv.x*16)*fract(uv.y*16) * .0000;
	//	result.x += dif_a * lap.x - reaction + feed * (1-ab.x) * ( (pres.x>.1) ? 1. : 0.);
	//	result.x += dif_a * lap.x - reaction + feed * (1-ab.x) * ( (pres.x>.1) ? (.80 + pres.x * .22) : 0.);
		
#if 0	//force to move by altering the one product when situation become fix
		result.z = max( result.z + (result.y -.3) * .01, 0. );
		if( result.z > 1. )
		{
			result.x *= (1 - clamp(result.z,1,2) * .05 );
		// 	result.y *= .999;
		}
		out_result = vec3( clamp(result.xy,-8,8), result.z );
#else
		out_result = vec3( clamp(result.xy,-8,8), 1 );
#endif
	}

	if( b_final )
		out_render = do_out( uv, dxy ) * in_color;
//	out_result = vec4( 0,1,0, 1. );
}

// 	out_render = vec3(exp(pow(sh-.75,2.)*-10.),
// 		exp(pow(sh-.50,2.)*-20.),
// 		exp(pow(sh-.25,2.)*-10.));



/*
uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{             
    vec2 tex_offset = 1.0 / textureSize(image, 0); // gets size of single texel
    vec3 result = texture(image, TexCoords).rgb * weight[0]; // current fragment's contribution
    if(horizontal)
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(image, TexCoords + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            result += texture(image, TexCoords - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        }
    }
    else
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(image, TexCoords + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
            result += texture(image, TexCoords - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
        }
    }
    FragColor = vec4(result, 1.0);
}
*/









