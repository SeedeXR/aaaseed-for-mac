//#version 130



uniform sampler2D  aaa_tex2d[4];


#define FULL_PROCEDURAL

#ifdef FULL_PROCEDURAL
    // hash based 3d value noise
	float hash( float n ) 	{	return fract(sin(n)*43758.5453123);		}
	float noise( in vec3 x )
	{
    	vec3 p = floor(x);
    	vec3 f = fract(x);
    	f = f * f* ( 3.0 - 2.0 * f );

    	float n = p.x + p.y*157.0 + 113.0*p.z;
    	return mix(
    				mix(
    						mix( hash(n+0.0),	hash(n+1.0), 	f.x ),
                   			mix( hash(n+157.0),	hash(n+158.0),	f.x ),
                   			f.y
                   		),
              		 mix(	mix( hash(n+113.0), hash(n+114.0),	f.x ),
                   			mix( hash(n+270.0), hash(n+271.0),	f.x ),
                   			f.y
                   		),
                   f.z
                   );
    }

#else

	// LUT based 3d value noise
	float noise( in vec3 x )
	{
	    vec3 p = floor(x);
	    vec3 f = fract(x);
		f = f * f * ( 3.0 - 2.0 * f );

		vec2 uv = p.xy + vec2(37.0,17.0) * p.z + f.xy;
		uv.y = -uv.y;
		vec2 rg = texture2D( aaa_tex2d[0], (uv + 0.5)/256.0, -100.0 ).yx;
		float g = mix( rg.x, rg.y, f.z );
		return g;
	//    return rg.x;
	}
#endif

float maa( in vec3 v )
{
//	v.y = sin( v.xyz *.05 );
	float g = noise( v );
	//g = sin( g * 3.2	) ;
	g = pow( g, aaa_fu_float[2] );
	return g;
}

float map( in vec3 p )
{
	//p *= aaa_fu_float[4];

	//float d = 0.2 - p.y;

	float f;
    f  = 0.5000 * maa( p ); p *= 2.1;
	f += 0.2500 * maa( p ); p *= 2.2;
	f += 0.1250 * maa( p ); p *= 2.3;
	f += 0.0625 * maa( p );

	f = aaa_fu_float[0] + f * aaa_fu_float[1] ;
//	f = clamp( f, 0.0, 1.0 );

	return f;

/*
	d += 3.0 * f;

	d = clamp( d, 0.0, 1.0 );


	vec4 res = vec4( d );

	res.xyz = mix( 1.15*vec3(1.0,0.95,0.8), vec3(0.7,0.7,0.7), res.x );

	return res;
*/
}

vec3 noise33( in vec3 v )
{
	vec3 q = v - aaa_fu_vec4[0].xyz * aaa_fu_float[3];	//time
	vec3 vo;
	vo.x = map( q );
	vo.y = map( q.yzx );
	vo.z = map( q.zxy );
	return vo;
}

void main(void)
{
	vec3 p;
	p.xy = gl_TexCoord[0].st;
 	p.x -= .5;
 	p.y -= .5;
 	p.z = 0.;
  	// 	p.x *= 4;
 	//  p.y *= 4;
 	p += aaa_fu_vec4[1].xyz;
	p *= aaa_fu_vec4[2].xyz;

 	vec3 col;
	//vec3 col = aaa_fu_vec4[6].xyz - rd.y*0.2*aaa_fu_vec4[7].xyz + 0.15*0.5;
	col = noise33( p );

	col += p;
	col = noise33( col );
	col += p;
	col = noise33( col );
	col += p;
	col = noise33( col );
/*	col += p;
	col = noise33( col );
*/
	col = aaa_fu_float[5] * col + aaa_fu_float[6] * texture2D( aaa_tex2d[1], gl_TexCoord[0].st + (col.st - .5) * aaa_fu_float[7] 	).xyz	;
    gl_FragColor = vec4( col, 1.0 );
}
