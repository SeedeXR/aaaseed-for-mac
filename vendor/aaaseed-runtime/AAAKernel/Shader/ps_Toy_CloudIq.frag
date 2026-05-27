////#version 330 compatibility
//#extension GL_ARB_shading_language_include : enable

// Created by inigo quilez - iq/2013
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.

//uniform float screenWidth;
//uniform float screenHeight;
#define	factor_u	aaa_fu_float[1]
#define	factor_v	aaa_fu_float[2]
#define time		aaa_fu_float[3]

#define phase		(aaa_fu_vec4[0].xyz)
#define pos1_stuff	(aaa_fu_vec4[1].xyz)
#define pos2_stuff	(aaa_fu_vec4[2].xyz)

uniform sampler2D  aaa_tex2d[4];

in ST_AAA_BV BV;

#if 0

#define FULL_PROCEDURAL
#ifdef FULL_PROCEDURAL
    // hash based 3d value noise
	float hash( float n ) 	{	return fract(sin(n)*43758.5453123);		}
	float noise( in vec3 x )
	{
#if 0	
		return iqNoise3f(x);
#else
    	vec3 p = floor(x);
    	vec3 f = fract(x);
    	f = f*f*(3.0-2.0*f);

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
#endif
    }

#else


	// LUT based 3d value noise
	float noise( in vec3 x )
	{
	    vec3 p = floor(x);
	    vec3 f = fract(x);
		f = f * f * ( 3.0-2.0*f );

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
//		v.y = sin( v.xyz *.05 );
	float g = noise( v ) ;
//	g = sin( g * 12. 		);
	g = pow( g, aaa_fu_float[5] );
	return g;
}

vec4 map( in vec3 p )
{
	p *= aaa_fu_float[4];

	float d = 0.2 - p.y;

	vec3 q = p - phase;
	float f;
    f  = 0.5000 * maa( q );
	q = q*2.02;
    f += 0.2500 * maa( q );
	q = q*2.03;
    f += 0.1250 * maa( q );
	q = q*2.01;
    f += 0.0625 * maa( q );

	d += 3.0 * f;

	d = clamp( d, 0.0, 1.0 );

	vec4 res = vec4( d );

	res.xyz = mix( 1.15*vec3(1.0,0.95,0.8), vec3(0.7,0.7,0.7), res.x );

	return res;
}


vec3 sundir = vec3( -1,0,0 );

vec4 raymarch( in vec3 ro, in vec3 rd )
{
	vec4 sum = vec4( 0,0,0, 0 );

	float t = 0.1;
	for(int i=0; i<aaa_fu_int[0]; i++)
	{
		if( sum.a > aaa_fu_float[0] ) continue;

		vec3 pos = ro + t*rd;
		vec4 col = map( pos );

#	if 1
		float dif =  clamp((col.w - map(pos+0.3*sundir).w)/0.6, 0.0, 1.0 );

        vec3 lin = aaa_fu_vec4[4].xyz*aaa_fu_vec4[4].w + aaa_fu_vec4[5].xyz*aaa_fu_vec4[5].w*dif;
		col.xyz *= lin;
#	endif

		col.a *= aaa_fu_float[6];
		col.rgb *= col.a;

		sum = sum + col * (1.0 - sum.a);

#	if 0
		t += 0.1;
#	else
		t += max( .1, .025*t );
#	endif
	}

	sum.xyz /= (0.001+sum.w);

	return clamp( sum, 0.0, 1.0 );
}

#if 0

vec3 fade( vec3 t )		{ return t*t*t*(t*(t*6.0-15.0)+10.0);	}

// Classic Perlin noise
float cnoise(vec3 P)
{
	vec3 Pi0 = floor(P); // Integer part for indexing
	vec3 Pi1 = Pi0 + vec3(1.0); // Integer part + 1
	Pi0 = mod289(Pi0);
	Pi1 = mod289(Pi1);
	vec3 Pf0 = fract(P); // Fractional part for interpolation
	vec3 Pf1 = Pf0 - vec3(1.0); // Fractional part - 1.0
	vec4 ix = vec4(Pi0.x, Pi1.x, Pi0.x, Pi1.x);
	vec4 iy = vec4(Pi0.yy, Pi1.yy);
	vec4 iz0 = Pi0.zzzz;
	vec4 iz1 = Pi1.zzzz;

	vec4 ixy = permute(permute(ix) + iy);
	vec4 ixy0 = permute(ixy + iz0);
	vec4 ixy1 = permute(ixy + iz1);

	vec4 gx0 = ixy0 * (1.0 / 7.0);
	vec4 gy0 = fract(floor(gx0) * (1.0 / 7.0)) - 0.5;
	gx0 = fract(gx0);
	vec4 gz0 = vec4(0.5) - abs(gx0) - abs(gy0);
	vec4 sz0 = step(gz0, vec4(0.0));
	gx0 -= sz0 * (step(0.0, gx0) - 0.5);
	gy0 -= sz0 * (step(0.0, gy0) - 0.5);

	vec4 gx1 = ixy1 * (1.0 / 7.0);
	vec4 gy1 = fract(floor(gx1) * (1.0 / 7.0)) - 0.5;
	gx1 = fract(gx1);
	vec4 gz1 = vec4(0.5) - abs(gx1) - abs(gy1);
	vec4 sz1 = step(gz1, vec4(0.0));
	gx1 -= sz1 * (step(0.0, gx1) - 0.5);
	gy1 -= sz1 * (step(0.0, gy1) - 0.5);

	vec3 g000 = vec3(gx0.x,gy0.x,gz0.x);
	vec3 g100 = vec3(gx0.y,gy0.y,gz0.y);
	vec3 g010 = vec3(gx0.z,gy0.z,gz0.z);
	vec3 g110 = vec3(gx0.w,gy0.w,gz0.w);
	vec3 g001 = vec3(gx1.x,gy1.x,gz1.x);
	vec3 g101 = vec3(gx1.y,gy1.y,gz1.y);
	vec3 g011 = vec3(gx1.z,gy1.z,gz1.z);
	vec3 g111 = vec3(gx1.w,gy1.w,gz1.w);

	vec4 norm0 = taylorInvSqrt(vec4(dot(g000, g000), dot(g010, g010), dot(g100, g100), dot(g110, g110)));
	g000 *= norm0.x;
	g010 *= norm0.y;
	g100 *= norm0.z;
	g110 *= norm0.w;
	vec4 norm1 = taylorInvSqrt(vec4(dot(g001, g001), dot(g011, g011), dot(g101, g101), dot(g111, g111)));
	g001 *= norm1.x;
	g011 *= norm1.y;
	g101 *= norm1.z;
	g111 *= norm1.w;

	float n000 = dot(g000, Pf0);
	float n100 = dot(g100, vec3(Pf1.x, Pf0.yz));
	float n010 = dot(g010, vec3(Pf0.x, Pf1.y, Pf0.z));
	float n110 = dot(g110, vec3(Pf1.xy, Pf0.z));
	float n001 = dot(g001, vec3(Pf0.xy, Pf1.z));
	float n101 = dot(g101, vec3(Pf1.x, Pf0.y, Pf1.z));
	float n011 = dot(g011, vec3(Pf0.x, Pf1.yz));
	float n111 = dot(g111, Pf1);

	vec3 fade_xyz = fade(Pf0);
	vec4 n_z = mix(vec4(n000, n100, n010, n110), vec4(n001, n101, n011, n111), fade_xyz.z);
	vec2 n_yz = mix(n_z.xy, n_z.zw, fade_xyz.y);
	float n_xyz = mix(n_yz.x, n_yz.y, fade_xyz.x);
	return 2.2 * n_xyz;
}

//	<https://www.shadertoy.com/view/Xd23Dh>
//	by inigo quilez <http://iquilezles.org/www/articles/voronoise/voronoise.htm>
//

vec3 hash3( vec2 p )
{
    vec3 q = vec3( dot(p,vec2(127.1,311.7)), 
				   dot(p,vec2(269.5,183.3)), 
				   dot(p,vec2(419.2,371.9)) );
	return fract(sin(q)*43758.5453);
}

float iqnoise( in vec2 x, float u, float v ){
    vec2 p = floor(x);
    vec2 f = fract(x);
		
	float k = 1.0+63.0*pow(1.0-v,4.0);
	
	float va = 0.0;
	float wt = 0.0;
    for( int j=-2; j<=2; j++ )
    for( int i=-2; i<=2; i++ )
    {
        vec2 g = vec2( float(i),float(j) );
		vec3 o = hash3( p + g )*vec3(u,u,1.0);
		vec2 r = g - f + o.xy;
		float d = dot(r,r);
		float ww = pow( 1.0-smoothstep(0.0,1.414,sqrt(d)), k );
		va += o.z*ww;
		wt += ww;
    }
	
    return va/wt;
}

float noiseFractal( vec3 m )
{
	return    0.5333333 * cnoise( m )
			+ 0.2666667 * cnoise( m * 2. )
			+ 0.1333333 * cnoise( m * 4. )
			+ 0.0666667 * cnoise( m * 8. );
}

void main(void)
{
//speed are unclear
    vec3 vec = vec3( gl_TexCoord[0].st * 50., time*0.2 );
	//	float g = abs(noise( vec )); // procedural 37fps // texture 47fps
	//float g = abs(simplex3d( vec )); //25fps
	//float g = abs(noise_simplex( vec )); //16fps
	float g = abs( cnoise( vec ) ); //13fps
	//float g =abs(iqnoise( vec.xy, .75, .75 )); //5fps
	//float g =abs(noiseFractal( vec ));
	gl_FragColor = vec4( g,g,g, 1 ) * BV.color;
}
#else
void main(void)
{
//	vec2 q = gl_FragCoord.xy / iResolution.xy;
//   vec2 p = -1.0 + 2.0*q;
//    p.x *= iResolution.x/ iResolution.y;
 //   vec2 mo = -1.0 + 2.0*iMouse.xy / iResolution.xy;
 //   vec2 mo;
 //   mo.x = factor_u;
 //   mo.y = factor_v;
	vec2 p = gl_TexCoord[0].st;
	p.xy -= .5;
 	p.xy *= vec2( factor_u, factor_v );

    // camera
    //vec3 ro = 4.0*normalize(vec3(cos(2.75-3.0*mo.x), 0.7+(mo.y+1.0), sin(2.75-3.0*mo.x)));
	vec3 ta = pos1_stuff;
    vec3 ro = pos2_stuff;
    float angle = 6.14159 * ro.z;
    ro.z = ro.x * cos(angle);
    ro.x = ro.x * sin(angle);
    ro.y += ta.y;

    vec3 ww = normalize( ta - ro );
    vec3 uu = normalize( cross( vec3(0.,1.,0.), ww ) );
    vec3 vv = cross( ww, uu );
    vec3 rd = normalize( p.x*uu + p.y*vv + 1.*ww );

    vec4 res = raymarch( ro, rd );

	float sun = clamp( dot(sundir,rd), 0., 1. );
//	vec3 col = vec3(0.6,0.71,0.75) - rd.y*0.2*vec3(1.0,0.5,1.0) + 0.15*0.5;
	vec3 col = aaa_fu_vec4[6].xyz - rd.y*0.2*aaa_fu_vec4[7].xyz + 0.15*0.5;

	col += 0.2 * vec3(1.,.6,.1) * pow( sun, 8. );
	col *= 0.95;
	col = mix( col, res.xyz, res.w );
	col += 0.1 * vec3(1.,.4,.2) * pow( sun, 3. );

//	gl_FragColor = res;
	gl_FragColor = vec4( col, res.w ) * BV.color;
//	gl_FragColor = vec4( 1. );
/*
 	vec3 posb;
    posb = vec3( p.xy, 0 );
	posb *= 10.;
 	posb -= vec3( 0.0, 0.0, .1 )*aaa_fu_float[3];
    float g = noise( posb );
    gl_FragColor = vec4( g,g,g, 1.0 );
*/
}
#endif

#else

// Copyright Inigo Quilez, 2013 - https://iquilezles.org/
// I am the sole copyright owner of this Work.
// You cannot host, display, distribute or share this Work neither
// as it is or altered, here on Shadertoy or anywhere else, in any
// form including physical and digital. You cannot use this Work in any
// commercial or non-commercial product, website or project. You cannot
// sell this Work and you cannot mint an NFTs of it or train a neural
// network with it without permission. I share this Work for educational
// purposes, and you can link to it, through an URL, proper attribution
// and unmodified screenshot, as part of your educational material. If
// these conditions are too restrictive please contact me and we'll
// definitely work it out.


// Volumetric clouds. Not physically correct in any way - 
// it does the wrong extintion computations and also
// works in sRGB instead of linear RGB color space. No
// shadows are computed, no scattering is computed. It is
// a volumetric raymarcher than samples an fBM and tweaks
// the colors to make it look good.
//
// Lighting is done with only one extra sample per raymarch
// step instead of using 3 to compute a density gradient,
// by using this directional derivative technique:
//
// https://iquilezles.org/articles/derivative



// 0: sunset look
// 1: bright look
#define LOOK 1

// 0: one 3d texture lookup
// 1: two 2d texture lookups with hardware interpolation
// 2: two 2d texture lookups with software interpolation
#define NOISE_METHOD 1

// 0: no LOD
// 1: yes LOD
#define USE_LOD 1


mat3 setCamera( in vec3 ro, in vec3 ta, float cr )
{
	vec3 cw = normalize(ta-ro);
	vec3 cp = vec3(sin(cr), cos(cr),0.0);
	vec3 cu = normalize( cross(cw,cp) );
	vec3 cv = normalize( cross(cu,cw) );
    return mat3( cu, cv, cw );
}

float noise( in vec3 x )
{
    vec3 p = floor(x);
    vec3 f = fract(x);
	f = f*f*(3.0-2.0*f);

#if NOISE_METHOD==0
    x = p + f;
    return textureLod(aaa_tex2d[2],(x+0.5)/32.0,0.0).x*2.0-1.0;
#endif
#if NOISE_METHOD==1
	vec2 uv = (p.xy+vec2(37.0,239.0)*p.z) + f.xy;
    vec2 rg = textureLod(aaa_tex2d[0],(uv+0.5)/256.0,0.0).yx;
	return mix( rg.x, rg.y, f.z )*2.0-1.0;
#endif    
#if NOISE_METHOD==2
    ivec3 q = ivec3(p);
	ivec2 uv = q.xy + ivec2(37,239)*q.z;
	vec2 rg = mix(mix(texelFetch(aaa_tex2d[0],(uv           )&255,0),
				      texelFetch(aaa_tex2d[0],(uv+ivec2(1,0))&255,0),f.x),
				  mix(texelFetch(aaa_tex2d[0],(uv+ivec2(0,1))&255,0),
				      texelFetch(aaa_tex2d[0],(uv+ivec2(1,1))&255,0),f.x),f.y).yx;
	return mix( rg.x, rg.y, f.z )*2.0-1.0;
#endif    
}

#if LOOK==0
float map( in vec3 p, int oct )
{
	vec3 q = p - vec3(0.0,0.1,1.0)*time;
    float g = 0.5+0.5*noise( q*0.3 );
    
	float f;
    f  = 0.50000*noise( q ); q = q*2.02;
    #if USE_LOD==1
    if( oct>=2 ) 
    #endif
    f += 0.25000*noise( q ); q = q*2.23;
    #if USE_LOD==1
    if( oct>=3 )
    #endif
    f += 0.12500*noise( q ); q = q*2.41;
    #if USE_LOD==1
    if( oct>=4 )
    #endif
    f += 0.06250*noise( q ); q = q*2.62;
    #if USE_LOD==1
    if( oct>=5 )
    #endif
    f += 0.03125*noise( q ); 
    
    f = mix( f*0.1-0.5, f, g*g );
        
    return 1.5*f - 0.5 - p.y;
}

const int kDiv = 1; // make bigger for higher quality
const vec3 sundir = normalize( vec3(1.0,0.0,-1.0) );

vec4 raymarch( in vec3 ro, in vec3 rd, in vec3 bgcol, in ivec2 px )
{
    // bounding planes	
    const float yb = -3.0;
    const float yt =  0.6;
    float tb = (yb-ro.y)/rd.y;
    float tt = (yt-ro.y)/rd.t;

    // find tigthest possible raymarching segment
    float tmin, tmax;
    if( ro.y>yt )
    {
        // above top plane
        if( tt<0.0 ) return vec4(0.0); // early exit
        tmin = tt;
        tmax = tb;
    }
    else
    {
        // inside clouds slabs
        tmin = 0.0;
        tmax = 60.0;
        if( tt>0.0 ) tmax = min( tmax, tt );
        if( tb>0.0 ) tmax = min( tmax, tb );
    }
    
    // dithered near distance
    float t = tmin;
	t += 0.1*texelFetch( aaa_tex2d[1], px&1023, 0 ).x;
    
    // raymarch loop
	vec4 sum = vec4(0.0);
    for( int i=0; i<190*kDiv; i++ )
    {
       // step size
       float dt = max(0.05,0.02*t/float(kDiv));

       // lod
       #if USE_LOD==0
       const int oct = 5;
       #else
       int oct = 5 - int( log2(1.0+t*0.5) );
       #endif
       
       // sample cloud
       vec3 pos = ro + t*rd;
       float den = map( pos,oct );
       if( den>0.01 ) // if inside
       {
           // do lighting
           float dif = clamp((den - map(pos+0.3*sundir,oct))/0.25, 0.0, 1.0 );
           vec3  lin = vec3(0.65,0.65,0.75)*1.1 + 0.8*vec3(1.0,0.6,0.3)*dif;
           vec4  col = vec4( mix( vec3(1.0,0.93,0.84), vec3(0.25,0.3,0.4), den ), den );
           col.xyz *= lin;
           // fog
           col.xyz = mix(col.xyz,bgcol, 1.0-exp2(-0.1*t));
           // composite front to back
           col.w    = min(col.w*8.0*dt,1.0);
           col.rgb *= col.a;
           sum += col*(1.0-sum.a);
       }
       // advance ray
       t += dt;
       // until far clip or full opacity
       if( t>tmax || sum.a>0.99 ) break;
    }

    return clamp( sum, 0.0, 1.0 );
}

vec4 render( in vec3 ro, in vec3 rd, in ivec2 px )
{
	float sun = clamp( dot(sundir,rd), 0.0, 1.0 );

    // background sky
    vec3 col = vec3(0.76,0.75,0.95);
    col -= 0.6*vec3(0.90,0.75,0.95)*rd.y;
	col += 0.2*vec3(1.00,0.60,0.10)*pow( sun, 8.0 );

    // clouds    
    vec4 res = raymarch( ro, rd, col, px );
    col = col*(1.0-res.w) + res.xyz;
    
    // sun glare    
	col += 0.2*vec3(1.0,0.4,0.2)*pow( sun, 3.0 );

    // tonemap
    col = smoothstep(0.15,1.1,col);
 
    return vec4( col, 1.0 );
}

#else


float map5( in vec3 p )
{    
    vec3 q = p - vec3(0.0,0.1,1.0)*time;    
    float f;
    f  = 0.50000*noise( q ); q = q*2.02;    
    f += 0.25000*noise( q ); q = q*2.03;    
    f += 0.12500*noise( q ); q = q*2.01;    
    f += 0.06250*noise( q ); q = q*2.02;    
    f += 0.03125*noise( q );    
    return clamp( 1.5 - p.y - 2.0 + 1.75*f, 0.0, 1.0 );
}
float map4( in vec3 p )
{    
    vec3 q = p - vec3(0.0,0.1,1.0)*time;    
    float f;
    f  = 0.50000*noise( q ); q = q*2.02;    
    f += 0.25000*noise( q ); q = q*2.03;    
    f += 0.12500*noise( q ); q = q*2.01;   
    f += 0.06250*noise( q );    
    return clamp( 1.5 - p.y - 2.0 + 1.75*f, 0.0, 1.0 );
}
float map3( in vec3 p )
{
    vec3 q = p - vec3(0.0,0.1,1.0)*time;    
    float f;
    f  = 0.50000*noise( q ); q = q*2.02;    
    f += 0.25000*noise( q ); q = q*2.03;    f += 0.12500*noise( q );    
    return clamp( 1.5 - p.y - 2.0 + 1.75*f, 0.0, 1.0 );
}
float map2( in vec3 p )
{    
    vec3 q = p - vec3(0.0,0.1,1.0)*time;    
    float f;
    f  = 0.50000*noise( q ); 
    q = q*2.02;    f += 0.25000*noise( q );
    return clamp( 1.5 - p.y - 2.0 + 1.75*f, 0.0, 1.0 );
}

const vec3 sundir = vec3(-0.7071,0.0,-0.7071);

#define MARCH(STEPS,MAPLOD) for(int i=0; i<STEPS; i++) { vec3 pos = ro + t*rd; if( pos.y<-3.0 || pos.y>2.0 || sum.a>0.99 ) break; float den = MAPLOD( pos ); if( den>0.01 ) { float dif = clamp((den - MAPLOD(pos+0.3*sundir))/0.6, 0.0, 1.0 ); vec3  lin = vec3(1.0,0.6,0.3)*dif+vec3(0.91,0.98,1.05); vec4  col = vec4( mix( vec3(1.0,0.95,0.8), vec3(0.25,0.3,0.35), den ), den ); col.xyz *= lin; col.xyz = mix( col.xyz, bgcol, 1.0-exp(-0.003*t*t) ); col.w *= 0.4; col.rgb *= col.a; sum += col*(1.0-sum.a); } t += max(0.06,0.05*t); }

vec4 raymarch( in vec3 ro, in vec3 rd, in vec3 bgcol, in ivec2 px )
{    
    vec4 sum = vec4(0.0);    
    float t = 0.05 * texelFetch( aaa_tex2d[1], px&255, 0 ).x;    
    MARCH(40,map5);    
    MARCH(40,map4);    
    MARCH(30,map3);    
    MARCH(30,map2);    
    return clamp( sum, 0.0, 1.0 );
}

vec4 render( in vec3 ro, in vec3 rd, in ivec2 px )
{
    // background sky         
    float sun = clamp( dot(sundir,rd), 0.0, 1.0 );    
    vec3 col = vec3(0.6,0.71,0.75) - rd.y*0.2*vec3(1.0,0.5,1.0) + 0.15*0.5;    
    col += 0.2*vec3(1.0,.6,0.1)*pow( sun, 8.0 );    
    // clouds        
    vec4 res = raymarch( ro, rd, col, px );    
    col = col*(1.0-res.w) + res.xyz;        
    // sun glare        
    col += vec3(0.2,0.08,0.04)*pow( sun, 3.0 );    
    return vec4( col, 1.0 );
}

#endif

/*
void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 p = (2.0*fragCoord-iResolution.xy)/iResolution.y;
    vec2 m =                iMouse.xy      /iResolution.xy;

    // camera
    vec3 ro = 4.0*normalize(vec3(sin(3.0*m.x), 0.8*m.y, cos(3.0*m.x))) - vec3(0.0,0.1,0.0);
	vec3 ta = vec3(0.0, -1.0, 0.0);
    mat3 ca = setCamera( ro, ta, 0.07*cos(0.25*iTime) );
    // ray
    vec3 rd = ca * normalize( vec3(p.xy,1.5));
    
    fragColor = render( ro, rd, ivec2(fragCoord-0.5) );
}
*/

void main(void)
{
	vec2 p = gl_TexCoord[0].st;
	p.xy -= .5;
 	p.xy *= vec2( factor_u, factor_v ) * 3.;

    // camera
	// vec3 ta = pos1_stuff;
    // vec3 ro = pos2_stuff;
    // float angle = 6.14159 * ro.z;
    // ro.z = ro.x * cos(angle);
    // ro.x = ro.x * sin(angle);
    // ro.y += ta.y;

	// camera
	vec2 m = pos1_stuff.xy;
    vec3 ro = 4.0 * normalize(vec3( sin(3.0*m.x), 0.8*m.y, cos(3.0*m.x) )) - vec3( .0,.1,.0);
	vec3 ta = pos2_stuff;

    mat3 ca = setCamera( ro, ta, 0.07 * cos(0.25*time) );
    // ray
    vec3 rd = ca * normalize( vec3( p.xy, 1.5 ) );
    
    gl_FragColor = render( ro, rd, ivec2(p*2048.) ) * BV.color;
	//gl_FragColor = vec4( p, 0, 1 );
//	gl_FragColor = vec4( 1. );
}
#endif