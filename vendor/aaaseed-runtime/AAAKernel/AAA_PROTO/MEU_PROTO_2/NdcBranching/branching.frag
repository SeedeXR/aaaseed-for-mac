
//	#define color_factor		aaa_fu_vec4[0]

//pipeline inputs
layout(location = 0) in vec2 in_texcoord;
layout(location = 1) in vec4 in_color;

//outputs
layout(location = 1) out vec4 out_result;
layout(location = 0) out vec4 out_render;

//texture inputs
layout(binding = 0) uniform sampler2D TEX_PREVIOUS;	// buffer we fliflop to computee the next frame from previous
layout(binding = 1) uniform sampler2D TEX_SRC;		// input used for presence also the image src
layout(binding = 2) uniform sampler2D TEX_COLORMAP;	//

CONST float corner_weight = 0.7071067812;
CONST float sum_factor = 1. / (4. + 4. * corner_weight);

//CONST float grey_gamma 		= aaa_fu_float[1];
CONST float img_amount 			= aaa_fu_float[1];
CONST float img_normal 			= aaa_fu_float[2];
CONST float maa_factor 			= aaa_fu_float[3];

CONST float particle_gamma		= aaa_fu_float[8];

CONST float out_min				= aaa_fu_float[9];
CONST float out_factor 			= aaa_fu_float[10];

CONST float out_mul 			= aaa_fu_float[11];

CONST float emboss_dist 		= aaa_fu_float[12];
CONST float emboss_min			= aaa_fu_float[13];
CONST float emboss_factor 		= aaa_fu_float[14];
CONST float emboss_gamma		= aaa_fu_float[15];

CONST float erase_radius 		= aaa_fu_float[23];

CONST int	s_how				= aaa_fu_int[0] & 0x0f;
CONST bool	b_clear				= (aaa_fu_int[0] & 0x100) != 0;
CONST int	s_frame				= aaa_fu_int[1];	//todo
CONST int	s_out				= aaa_fu_int[2] & 0x0f;
CONST bool	b_out_mul			= (aaa_fu_int[2] & 0x100) != 0;
CONST bool	b_out_inv			= (aaa_fu_int[2] & 0x200) != 0;
CONST bool	b_emboss			= (aaa_fu_int[2] & 0x400) != 0;
CONST bool	b_final				= aaa_fu_int[3] != 0;	// 1 mean the last frame and shader should write to second attachment

CONST vec2	res					= aaa_fu_vec4[0].xy;
CONST vec2	dxy					= aaa_fu_vec4[0].zw;
CONST vec3	emboss_dir			= aaa_fu_vec4[1].xyz;
CONST vec4	emboss_col_beg		= aaa_fu_vec4[2];
CONST vec4	emboss_col_end		= aaa_fu_vec4[3];
CONST vec4	part_col			= aaa_fu_vec4[4];

#define GET_TEX_01(UV) texture( TEX_PREVIOUS, UV );
#define GET_TEX_XY(UV) GET_TEX_01( UV*dxy );
//texture(iChannel0,(U)/R)

vec4 compose_emboss( in vec4 col, in float amount, in vec3 g )
{ 
	vec4 c;
	if( b_emboss )
	{
		float d = dot( g, emboss_dir ) *.5 + .5;
		d = pow( d, emboss_gamma );
		float emboss = emboss_min + d * emboss_factor;
		//emboss = pow( emboss, emboss_gamma );
		//float emboss = (ve + emboss_min) * emboss_factor;
//		Q.xyz *= emboss;
		c = mix( emboss_col_beg, emboss_col_end, emboss );
		c = mix( c, col, amount );
	}
	else
		c = col * amount;
	return c;
}

// last iteration callm this to displau result
vec4 do_out( vec2 uv )
{
	vec3 d3 = vec3( dxy * emboss_dist, 0. );

	vec4 n = GET_TEX_01( uv + d3.zy );
	vec4 e = GET_TEX_01( uv + d3.xz );
    vec4 s = GET_TEX_01( uv - d3.zy );
    vec4 w = GET_TEX_01( uv - d3.xz );

	vec4 m = 0.25 * (n+e+s+w) ;

	vec3 g = normalize( vec3( e.z-w.z , n.z-s.z, -.3 ) );
	//g = reflect( g, vec3(0,0,1) );
	

	vec4 tex = GET_TEX_01( uv );

	vec4 Q = vec4( 0,0,0, 1);
	switch( s_out )
	{
	case 1:	//wz
		{
			float d = dot( g, normalize( vec3(0,1.,-.5) ) );
			Q = (exp(-4.*d*d)) * m.w * abs( sin(2.+vec4(1,2,3,4)*(1.+2.*m.z)) ) * 2.;
		}
		break;
	case 2:	//Particle0
		{
			vec3 b = normalize( vec3( e.w-w.w, n.w-s.w, 1 ) );
			Q = -.8 * (1.+0.5*(b.x+b.y)) * pow(tex.w,particle_gamma) * sin(2.+0.5*(g.z) * vec4(1,2,3,4));
		}
		break;
	case 3:
		{
			//float v = pow( tex.a, 2 ) ;
			//return part_col * v;
			float amount = tex.a;
			amount = pow( amount, particle_gamma );
			return compose_emboss( part_col, amount, g );
		}
		break;
	case 4:
		{
			// vec3 b = normalize( vec3( e.w-w.w, n.w-s.w, 1 ) );
			// //Q = vec4( 1.+g.z );
			// //Q = vec4( vec3(abs(b.x))*5.,1. );
			// float v = max( abs(b.y), abs(b.x) );
			// v = smoothstep( .0, .15, v );
			// return compose_emboss( part_col, 1.-v, g.x );
			float amount = tex.a;
			amount = 1. - pow( amount, particle_gamma );
			return compose_emboss( part_col, amount, g );
		}
		break;
	
	case 5:
		{
			//Q = vec4( UV*dxy, 0, 0 );
			//Q = vec4( clamp( floor(uv/2.)*2., 0.5*res-2., 0.5*res+2. ), 0, 0 );
			//Q = vec4( vec2( ( Q.xy - .5*res ) * .5) , 0. ,1. );
			float amount = tex.a;
			//amount = smoothstep( .75, 0., amount );
			amount = pow( amount, particle_gamma );
			Q = texture( TEX_COLORMAP, vec2( uv.x, amount ) );
			return compose_emboss( Q, amount, g );
			//return Q;
		}
		break;
	case 12: //Emboss other
		g = normalize( vec3( e.w-w.w , n.w-s.w, -.3 ) );
	case 6:	//Emboss
		{
			float d = dot( g, emboss_dir ) *.5 + .5;
			d = pow( d, emboss_gamma );
			float emboss = emboss_min + d * emboss_factor;
			Q = mix( emboss_col_beg, emboss_col_end, emboss );
			//return vec4( emboss, emboss, emboss, 1 );
			return vec4( emboss, emboss, emboss, 1 );
			//return Q;
		}
		break;
	case 7:  {	float v = (tex.r-out_min)*out_factor;  Q = vec4( vec3(v), 1. );	}	break;
	case 8:  {	float v = (tex.g-out_min)*out_factor;  Q = vec4( vec3(v), 1. );	}	break;
	case 9:  {	float v = (tex.b-out_min)*out_factor;  Q = vec4( vec3(v), 1. );	}	break;
	case 10: {	float v = (tex.a-out_min)*out_factor;  Q = vec4( vec3(v), 1. );	}	break;
	case 11: Q = (tex-out_min) * out_factor;	break;
//	case 11: Q = vec4( vec3( clamp_01(tex.b) ), 1. );	break	;

	}
	if( b_emboss )
	{
//		if( /*s_out!=5 &&*/ s_out != 6 )
//		{
			float d = dot( g, emboss_dir ) *.5 + .5;
			float emboss = emboss_min + d * emboss_factor;
//			Q.xyz *= emboss;
			Q += mix( emboss_col_beg, emboss_col_end, emboss );
//		}
	}
	//Q = pow( Q, vec4(3) );
	//Q = smoothstep( vec4(.5), vec4(1.), Q );
	return Q;
}

vec4 X( inout vec2 c, inout float m, in vec2 uv, in vec2 duv )
{
	vec2 uvb = uv + duv;
	//uv = (uv - .5) * .99 + .5;
	//uvb -= res * .5;
	//rotate( uvb, .0004 );
	//uvb *= 1.00001;
	//uvb += res * .5;
	//uvb.x += sin(uvb.x*.01)*.1;
	//uvb.y += sin(uvb.y*.01)*.1;


    vec4 tex = GET_TEX_XY( uvb );
    m += tex.z;

	switch( s_how )
	{
	case 1:
		// original wyatt stuff
		if( length( uvb - tex.xy ) < length( uv - c ) * 3.) //* (1.-n.z * .0045) )	//( .1 / texture( TEX_SRC, uvb*dxy ).x + .0 ) )
		{
			//if( texture( TEX_SRC, uv*dxy ).y > n.z * .005 )
			c = tex.xy; 	//* (texture( TEX_SRC, uv*dxy ).xy*.000+1.) * 1.;
		//	m += (texture( TEX_SRC, uv*dxy ).x -.2) * .1;
		}
		break;
	case 2:
	//	if( length( uvb - tex.xy ) < length( uv - c ) ) //* (1.-tex.z * .0045) )	//( .1 / texture( TEX_SRC, uvb*dxy ).x + .0 ) )

	//	vec2 grad = get_gradient_2d( TEX_SRC, uvb, 1. );
		if( s_frame%32 <= 1 && length( uvb - tex.xy ) < length( uv - c ) * maa_factor  ) //* (1.-tex.z * .0045) )	//( .1 / texture( TEX_SRC, uvb*dxy ).x + .0 ) )
	//	if( grad.x*1. < 0 && (length( uvb - tex.xy ) < length( uv - c ) * 1.) ) //* (1.-tex.z * .0045) )	//( .1 / texture( TEX_SRC, uvb*dxy ).x + .0 ) )
		{
			//if( texture( TEX_SRC, uv*dxy ).y > n.z * .005
			c = tex.xy;
			//float f = ( tex.x > 0 ) ? -.0001 : .0001;
			//c += f * tex.yx * vec2(-1,1); 	//* (texture( TEX_SRC, uv*dxy ).xy*.000+1.) * 1.;

			//	m += (texture( TEX_SRC, uv*dxy ).x -.2) * .1;
		}
		break;
	case 3:
		if( length( uvb - tex.xy ) < length( uv - c ) * maa_factor ) //* (1.-n.z * .0045) )	//( .1 / texture( TEX_SRC, uvb*dxy ).x + .0 ) )
		{
			//if( texture( TEX_SRC, uv*dxy ).y > n.z * .005 )
			c = tex.xy; 	//* (texture( TEX_SRC, uv*dxy ).xy*.000+1.) * 1.;
		//	m += (texture( TEX_SRC, uv*dxy ).x -.2) * .1;
		}
		break;
	case 4:
		break;
	}

	return tex;
}


const vec3 d3 = vec3( 1., -1., 0. );
void main() //out vec4 Q, vec2 U )
{
	vec2 uv_01 = in_texcoord;	// in [0,1]
	vec2 uv_xy = uv_01*res;	// in [0, size_in_pixel]

    vec4 Q = GET_TEX_01(uv_01);

	vec2 c = Q.xy;

	float m = 0.;	// appart of the influence on we accumulate here z
	vec4 n = X( c, m, uv_xy, d3.zx );	//	north
	vec4 e = X( c, m, uv_xy, d3.xz );	//	east
	vec4 s = X( c, m, uv_xy, d3.zy );	//	south
	vec4 w = X( c, m, uv_xy, d3.yz );	//	west

	float mb = 0.;	// and now the diagonals
	X( c, mb, uv_xy, d3.xx );
	X( c, mb, uv_xy, d3.xy );
	X( c, mb, uv_xy, d3.yx );
	X( c, mb, uv_xy, d3.yy );

	// direction of gradient on z
	//vec2 grad_z = vec2( e.z-w.z, n.z-s.z );
	vec2 grad_z = vec2( e.z-w.z, n.z-s.z );
	 
	vec2 grad = get_gradient_2d( TEX_SRC, uv_01, 1. ) * 25.;
	//grad_z -= dot(grad, grad) * 5;

	Q.xy = c;
	//maa
	Q.xy += grad * img_amount; //* Q.w; //max(Q.w,Q.z);
	Q.xy += grad.yx * vec2(-1,1) * img_normal;	// * max(Q.w,Q.z);

	// original value is .001
	const float dec = .001;	// * (.0+Q.w);
	Q.z += (m*.2 + mb*.05 - Q.z) + .05*Q.w - .0001 * Q.z;
	Q.w -= .001 * Q.w;
	Q.zw = max( Q.zw, vec2(2,1) * smoothstep( 4., .0, length(uv_xy-c) ) );
	Q.xy -= 0.25 * grad_z;

    if( iMouse.z > 0. && length(uv_xy-iMouse.xy*res) < erase_radius*res.y )
		Q = vec4( -res, Q.z, 0 );
		//Q = vec4( .5,.5,0,.001);

	//if( texture( TEX_SRC, uv ).x > .6 )
	//Q.z -= texture( TEX_SRC, uv ).x * .005 ;
	//Q.w -= texture( TEX_SRC, uv ).y * .005 ;

    if( b_clear )
		Q = vec4( clamp( floor(uv_xy/2.)*2., 0.5*res-2., 0.5*res+2. ), 0, 0 );
		//Q = vec4( 1,1,0, .001 );

	//Q.z += texture( TEX_SRC, uv ).y * .005 ;
	//Q.z = mix( Q.z, texture( TEX_SRC, uv ).y*16., 0.001 );
	// infinite boid ?
	//Q.w = mix( Q.w, (texture( TEX_SRC, uv ).x-.0)*1., 0.0025 );

	out_result = Q;

	if( b_final )
	{
		//out_render = vec4( grad.xy * vec2(1,1) * 100., 0, 1 );
		//return;

		out_render = do_out( uv_01 ) * in_color;
	// 	//todo
	// 	//out_render = mix( out_render, src, result.y*2	 );
	 	if( b_out_mul )
	 	{
	 		vec4 src	= texture( TEX_SRC, uv_01 );
	 	 	out_render	= mix( out_render, out_render*src, out_mul );
	// 		//out_render = min( out_render, src );
	 	}
	}  
}

