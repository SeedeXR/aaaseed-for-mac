// discontinuous pseudorandom uniformly distributed in [-0.5, +0.5]^3
vec3 random3( in vec3 c )
{
	float j = 4096.0 * sin( dot( c, vec3( 17., 59.4, 15. ) ) );
	return fract( vec3(j) * vec3( 64., 8., 512. ) ) - 0.5;
}
// 3d simplex noise
float simplex3d( in vec3 p )
{
// skew constants for 3d simplex functions
	const float F3 =  0.3333333;
	const float G3 =  0.1666667;
// 1. find current tetrahedron T and it's four vertices
// s, s+i1, s+i2, s+1.0 - absolute skewed (integer) coordinates of T vertices
// x, x1, x2, x3 - unskewed coordinates of p relative to each of T vertices

// calculate s and x
	vec3 s = floor( p + dot(p, vec3(F3)) );
	vec3 x = p - s + dot(s, vec3(G3));
// calculate i1 and i2
	vec3 e = step( vec3(0.), x - x.yzx );
	vec3 i1 = e * ( 1. - e.zxy );
	vec3 i2 = 1. - e.zxy * ( 1. - e );
// x1, x2, x3
	vec3 x1 = x - i1 + G3;
	vec3 x2 = x - i2 + G3 * 2.0;
	vec3 x3 = x - 1. + G3 * 3.0;
// 2. find four surflets and store them in d
	vec4 w, d;
// calculate surflet weights
	w.x = dot( x,	x	);
	w.y = dot( x1,	x1	);
	w.z = dot( x2,	x2	);
	w.w = dot( x3,	x3	);
// w fades from 0.6 at the center of the surflet to 0.0 at the margin
	w = max( .6-w, 0. );
// calculate surflet components
	d.x = dot( random3( s	), x	);
	d.y = dot( random3( s+i1), x1	);
	d.z = dot( random3( s+i2), x2	);
	d.w = dot( random3( s+1.), x3	);
// multiply d by w^4
	w *= w;
	w *= w;
	d *= w;
//3. return the sum of the four surflets
	return dot(d, vec4(52.0));
}

uint rand_wang_hash( inout uint seed )
{
    seed = (seed ^ 61) ^ (seed >> 16);
    seed *= 9;
    seed = seed ^ (seed >> 4);
    seed *= 0x27d4eb2d;
    seed = seed ^ (seed >> 15);
    return seed;
}

float rand_normalized( in uint n )
{
    return float(rand_wang_hash(n)) * (1.0/2147483647.0);
}

// Description : Array and textureless GLSL 2D simplex noise function. imported 2021 July
//      Author : Ian McEwan, Ashima Arts.
//  Maintainer : stegu
//     Lastmod : 20110822 (ijm)
//     License : Copyright (C) 2011 Ashima Arts. All rights reserved.
//               Distributed under the MIT License. See LICENSE file.
//               https://github.com/ashima/webgl-noise
//               https://github.com/stegu/webgl-noise
// 

float mod289(float x)	{	return x - floor(x * (1.0 / 289.0)) * 289.0;	}
vec2  mod289(vec2 x)	{	return x - floor(x * (1.0 / 289.0)) * 289.0;	}
vec3  mod289(vec3 x)	{	return x - floor(x * (1.0 / 289.0)) * 289.0;	}
vec4  mod289(vec4 x)	{	return x - floor(x * (1.0 / 289.0)) * 289.0;	}

float permute(float x)	{	return mod289(((x*34.0)+10.0)*x);				}
vec3  permute(vec3 x)	{	return mod289(((x*34.0)+10.0)*x);				}
vec4  permute(vec4 x)	{	return mod289(((x*34.0)+10.0)*x);				}

float taylorInvSqrt(float r)	{	return 1.79284291400159 - 0.85373472095314 * r;		}
vec4  taylorInvSqrt(vec4 r)		{	return 1.79284291400159 - 0.85373472095314 * r;		}

//original called snoise()
float noise_simplex(vec2 v)
{
	const vec4 C = vec4(0.211324865405187,  // (3.0-sqrt(3.0))/6.0
						0.366025403784439,  // 0.5*(sqrt(3.0)-1.0)
						-0.577350269189626,  // -1.0 + 2.0 * C.x
						0.024390243902439); // 1.0 / 41.0
	// First corner
	vec2 i  = floor(v + dot(v, C.yy) );
	vec2 x0 = v -   i + dot(i, C.xx);

	// Other corners
	vec2 i1;
	//i1.x = step( x0.y, x0.x ); // x0.x > x0.y ? 1.0 : 0.0
	//i1.y = 1.0 - i1.x;
	i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
	// x0 = x0 - 0.0 + 0.0 * C.xx ;
	// x1 = x0 - i1 + 1.0 * C.xx ;
	// x2 = x0 - 1.0 + 2.0 * C.xx ;
	vec4 x12 = x0.xyxy + C.xxzz;
	x12.xy -= i1;

	// Permutations
	i = mod289(i); // Avoid truncation effects in permutation
	vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
		+ i.x + vec3(0.0, i1.x, 1.0 ));

	vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy), dot(x12.zw,x12.zw)), 0.0);
	m = m*m ;
	m = m*m ;

	// Gradients: 41 points uniformly over a line, mapped onto a diamond.
	// The ring size 17*17 = 289 is close to a multiple of 41 (41*7 = 287)

	vec3 x = 2.0 * fract(p * C.www) - 1.0;
	vec3 h = abs(x) - 0.5;
	vec3 ox = floor(x + 0.5);
	vec3 a0 = x - ox;

	// Normalise gradients implicitly by scaling m
	// Approximation of: m *= inversesqrt( a0*a0 + h*h );
	m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );

	// Compute final noise value at P
	vec3 g;
	g.x  = a0.x  * x0.x  + h.x  * x0.y;
	g.yz = a0.yz * x12.xz + h.yz * x12.yw;
	return 130.0 * dot(m, g);
}

float noise_simplex(vec3 v)
{ 
	const vec2  C = vec2(1.0/6.0, 1.0/3.0) ;
	const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);

	// First corner
	vec3 i  = floor(v + dot(v, C.yyy) );
	vec3 x0 =   v - i + dot(i, C.xxx) ;

	// Other corners
	vec3 g = step(x0.yzx, x0.xyz);
	vec3 l = 1.0 - g;
	vec3 i1 = min( g.xyz, l.zxy );
	vec3 i2 = max( g.xyz, l.zxy );

	//   x0 = x0 - 0.0 + 0.0 * C.xxx;
	//   x1 = x0 - i1  + 1.0 * C.xxx;
	//   x2 = x0 - i2  + 2.0 * C.xxx;
	//   x3 = x0 - 1.0 + 3.0 * C.xxx;
	vec3 x1 = x0 - i1 + C.xxx;
	vec3 x2 = x0 - i2 + C.yyy; // 2.0*C.x = 1/3 = C.y
	vec3 x3 = x0 - D.yyy;      // -1.0+3.0*C.x = -0.5 = -D.y

	// Permutations
	i = mod289(i); 
	vec4 p = permute( permute( permute( 
				i.z + vec4(0.0, i1.z, i2.z, 1.0 ))
			+ i.y + vec4(0.0, i1.y, i2.y, 1.0 )) 
			+ i.x + vec4(0.0, i1.x, i2.x, 1.0 ));

	// Gradients: 7x7 points over a square, mapped onto an octahedron.
	// The ring size 17*17 = 289 is close to a multiple of 49 (49*6 = 294)
	float n_ = 0.142857142857; // 1.0/7.0
	vec3  ns = n_ * D.wyz - D.xzx;

	vec4 j = p - 49.0 * floor(p * ns.z * ns.z);  //  mod(p,7*7)

	vec4 x_ = floor(j * ns.z);
	vec4 y_ = floor(j - 7.0 * x_ );    // mod(j,N)

	vec4 x = x_ *ns.x + ns.yyyy;
	vec4 y = y_ *ns.x + ns.yyyy;
	vec4 h = 1.0 - abs(x) - abs(y);

	vec4 b0 = vec4( x.xy, y.xy );
	vec4 b1 = vec4( x.zw, y.zw );

	//vec4 s0 = vec4(lessThan(b0,0.0))*2.0 - 1.0;
	//vec4 s1 = vec4(lessThan(b1,0.0))*2.0 - 1.0;
	vec4 s0 = floor(b0)*2.0 + 1.0;
	vec4 s1 = floor(b1)*2.0 + 1.0;
	vec4 sh = -step(h, vec4(0.0));

	vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
	vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;

	vec3 p0 = vec3(a0.xy,h.x);
	vec3 p1 = vec3(a0.zw,h.y);
	vec3 p2 = vec3(a1.xy,h.z);
	vec3 p3 = vec3(a1.zw,h.w);

	//Normalise gradients
	vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
	p0 *= norm.x;
	p1 *= norm.y;
	p2 *= norm.z;
	p3 *= norm.w;

	// Mix final noise value
	vec4 m = max(0.5 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
	m = m * m;
	return 105.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1), 
								   dot(p2,x2), dot(p3,x3) ) );
}

float noise_simplex( vec3 v, out vec3 gradient )
{
	const vec2  C = vec2(1.0/6.0, 1.0/3.0) ;
	const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);

	// First corner
	vec3 i  = floor(v + dot(v, C.yyy) );
	vec3 x0 =   v - i + dot(i, C.xxx) ;

	// Other corners
	vec3 g = step(x0.yzx, x0.xyz);
	vec3 l = 1.0 - g;
	vec3 i1 = min( g.xyz, l.zxy );
	vec3 i2 = max( g.xyz, l.zxy );

	//   x0 = x0 - 0.0 + 0.0 * C.xxx;
	//   x1 = x0 - i1  + 1.0 * C.xxx;
	//   x2 = x0 - i2  + 2.0 * C.xxx;
	//   x3 = x0 - 1.0 + 3.0 * C.xxx;
	vec3 x1 = x0 - i1 + C.xxx;
	vec3 x2 = x0 - i2 + C.yyy; // 2.0*C.x = 1/3 = C.y
	vec3 x3 = x0 - D.yyy;      // -1.0+3.0*C.x = -0.5 = -D.y

	// Permutations
	i = mod289(i); 
	vec4 p = permute( permute( permute( 
			  i.z + vec4(0.0, i1.z, i2.z, 1.0 ))
			+ i.y + vec4(0.0, i1.y, i2.y, 1.0 )) 
			+ i.x + vec4(0.0, i1.x, i2.x, 1.0 ));

	// Gradients: 7x7 points over a square, mapped onto an octahedron.
	// The ring size 17*17 = 289 is close to a multiple of 49 (49*6 = 294)
	float n_ = 0.142857142857; // 1.0/7.0
	vec3  ns = n_ * D.wyz - D.xzx;

	vec4 j = p - 49.0 * floor(p * ns.z * ns.z);  //  mod(p,7*7)

	vec4 x_ = floor(j * ns.z);
	vec4 y_ = floor(j - 7.0 * x_ );    // mod(j,N)

	vec4 x = x_ *ns.x + ns.yyyy;
	vec4 y = y_ *ns.x + ns.yyyy;
	vec4 h = 1.0 - abs(x) - abs(y);

	vec4 b0 = vec4( x.xy, y.xy );
	vec4 b1 = vec4( x.zw, y.zw );

	//vec4 s0 = vec4(lessThan(b0,0.0))*2.0 - 1.0;
	//vec4 s1 = vec4(lessThan(b1,0.0))*2.0 - 1.0;
	vec4 s0 = floor(b0)*2.0 + 1.0;
	vec4 s1 = floor(b1)*2.0 + 1.0;
	vec4 sh = -step(h, vec4(0.0));

	vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
	vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;

	vec3 p0 = vec3(a0.xy,h.x);
	vec3 p1 = vec3(a0.zw,h.y);
	vec3 p2 = vec3(a1.xy,h.z);
	vec3 p3 = vec3(a1.zw,h.w);

	//Normalise gradients
	vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
	p0 *= norm.x;
	p1 *= norm.y;
	p2 *= norm.z;
	p3 *= norm.w;

	// Mix final noise value
	vec4 m = max(0.5 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
	vec4 m2 = m * m;
	vec4 m4 = m2 * m2;
	vec4 pdotx = vec4(dot(p0,x0), dot(p1,x1), dot(p2,x2), dot(p3,x3));

	// Determine noise gradient
	vec4 temp = m2 * m * pdotx;
	gradient = -8.0 * (temp.x * x0 + temp.y * x1 + temp.z * x2 + temp.w * x3);
	gradient += m4.x * p0 + m4.y * p1 + m4.z * p2 + m4.w * p3;
	gradient *= 105.0;

	return 105.0 * dot(m4, pdotx);
}

vec4 grad4(float j, vec4 ip)
{
	const vec4 ones = vec4(1.0, 1.0, 1.0, -1.0);
	vec4 p,s;

	p.xyz = floor( fract (vec3(j) * ip.xyz) * 7.0) * ip.z - 1.0;
	p.w = 1.5 - dot(abs(p.xyz), ones.xyz);
	s = vec4(lessThan(p, vec4(0.0)));
	p.xyz = p.xyz + (s.xyz*2.0 - 1.0) * s.www; 

	return p;
}

float noise_simplex(vec4 v)
{
	// (sqrt(5) - 1)/4 = F4, used once below
	const float F4 = 0.309016994374947451;
	const vec4  C = vec4( 0.138196601125011,  // (5 - sqrt(5))/20  G4
						0.276393202250021,  // 2 * G4
						0.414589803375032,  // 3 * G4
						-0.447213595499958); // -1 + 4 * G4

	// First corner
	vec4 i  = floor(v + dot(v, vec4(F4)) );
	vec4 x0 = v -   i + dot(i, C.xxxx);

	// Other corners

	// Rank sorting originally contributed by Bill Licea-Kane, AMD (formerly ATI)
	vec4 i0;
	vec3 isX = step( x0.yzw, x0.xxx );
	vec3 isYZ = step( x0.zww, x0.yyz );
	//  i0.x = dot( isX, vec3( 1.0 ) );
	i0.x = isX.x + isX.y + isX.z;
	i0.yzw = 1.0 - isX;
	//  i0.y += dot( isYZ.xy, vec2( 1.0 ) );
	i0.y += isYZ.x + isYZ.y;
	i0.zw += 1.0 - isYZ.xy;
	i0.z += isYZ.z;
	i0.w += 1.0 - isYZ.z;

	// i0 now contains the unique values 0,1,2,3 in each channel
	vec4 i3 = clamp( i0, 0.0, 1.0 );
	vec4 i2 = clamp( i0-1.0, 0.0, 1.0 );
	vec4 i1 = clamp( i0-2.0, 0.0, 1.0 );

	//  x0 = x0 - 0.0 + 0.0 * C.xxxx
	//  x1 = x0 - i1  + 1.0 * C.xxxx
	//  x2 = x0 - i2  + 2.0 * C.xxxx
	//  x3 = x0 - i3  + 3.0 * C.xxxx
	//  x4 = x0 - 1.0 + 4.0 * C.xxxx
	vec4 x1 = x0 - i1 + C.xxxx;
	vec4 x2 = x0 - i2 + C.yyyy;
	vec4 x3 = x0 - i3 + C.zzzz;
	vec4 x4 = x0 + C.wwww;

	// Permutations
	i = mod289(i); 
	float j0 = permute( permute( permute( permute(i.w) + i.z) + i.y) + i.x);
	vec4 j1 = permute	( permute	( permute	( permute( i.w + vec4( i1.w, i2.w, i3.w, 1. ) )
													+ i.z + vec4(i1.z, i2.z, i3.z, 1. )
												) + i.y + vec4(i1.y, i2.y, i3.y, 1. )
									) + i.x + vec4(i1.x, i2.x, i3.x, 1. )
						);

	// Gradients: 7x7x6 points over a cube, mapped onto a 4-cross polytope
	// 7*7*6 = 294, which is close to the ring size 17*17 = 289.
	vec4 ip = vec4(1.0/294.0, 1.0/49.0, 1.0/7.0, 0.0) ;

	vec4 p0 = grad4(j0,   ip);
	vec4 p1 = grad4(j1.x, ip);
	vec4 p2 = grad4(j1.y, ip);
	vec4 p3 = grad4(j1.z, ip);
	vec4 p4 = grad4(j1.w, ip);

	// Normalise gradients
	vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
	p0 *= norm.x;
	p1 *= norm.y;
	p2 *= norm.z;
	p3 *= norm.w;
	p4 *= taylorInvSqrt(dot(p4,p4));

	// Mix contributions from the five corners
	vec3 m0 = max(0.6 - vec3(dot(x0,x0), dot(x1,x1), dot(x2,x2)), 0.0);
	vec2 m1 = max(0.6 - vec2(dot(x3,x3), dot(x4,x4)            ), 0.0);
	m0 = m0 * m0;
	m1 = m1 * m1;
	return 49.0 * ( dot(m0*m0, vec3( dot( p0, x0 ), dot( p1, x1 ), dot( p2, x2 )))
				+ dot(m1*m1, vec2( dot( p3, x3 ), dot( p4, x4 ) ) ) ) ;
}



#ifndef MATH_RANDOM_FXH
#define MATH_RANDOM_FXH

/*uint pcg_hash(uint input)
{
    uint state = input * 747796405u + 2891336453u;
    uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}*/

uint rand_xorshift(inout uint state)
{
    uint x = state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 15;
    state = x;
    return x;
}

float random_float_01_wang(inout uint state)
{
    return (rand_wang_hash(state) & 0xFFFFFF) / 16777216.0f;
}
//todo check
float random_float_01_xorshift(inout uint state)
{
    return (rand_xorshift(state) & 0xFFFFFF) / 16777216.0f;
}

vec3 random_unit_disk(inout uint state)
{
    float a = random_float_01_wang(state) * 2.0f * 3.1415926f;
    vec3 v = vec3( cos(a), sin(a), 0 );
    v.xy *= sqrt(random_float_01_wang(state));
    return v;
}

vec3 random_unit_sphere(inout uint state)
{
    float z = random_float_01_wang(state) * 2.0f - 1.0f;
    float t = random_float_01_wang(state) * 2.0f * 3.1415926f;
    float r = sqrt(max(0.0, 1.0f - z * z));
    float x = r * cos(t);
    float y = r * sin(t);
    vec3 v = vec3( x, y, z );
    v *= pow(random_float_01_wang(state), 1.0 / 3.0);
    return v;
}

vec3 random_unit_vector(inout uint state)
{
    float z = random_float_01_wang(state) * 2.0f - 1.0f;
    float a = random_float_01_wang(state) * 2.0f * 3.1415926f;
    float r = sqrt(1.0f - z * z);
    float x = r * cos(a);
    float y = r * sin(a);
    return vec3( x, y, z );
}

#endif

#ifndef IQNOISE_FXH
#define IQNOISE_FXH

int iqIcoolfFunc3d2( in int n )
{
    n = (n << 13) ^ n;
    return (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff;
}
float iqCoolfFunc3d2( in int n )
{
    return float(iqIcoolfFunc3d2(n));
}

float iqNoise3f( in vec3 p )
{
    ivec3 ip = ivec3(floor(p));
    vec3 u = fract(p);
    u = u * u * (3.0 - 2.0 * u);

    int n = ip.x + ip.y * 57 + ip.z * 113;

    float res = mix(mix(mix(iqCoolfFunc3d2(n + (0 + 57 * 0 + 113 * 0)),
                            iqCoolfFunc3d2(n + (1 + 57 * 0 + 113 * 0)), u.x),
                        mix(iqCoolfFunc3d2(n + (0 + 57 * 1 + 113 * 0)),
                            iqCoolfFunc3d2(n + (1 + 57 * 1 + 113 * 0)), u.x), u.y),
                    mix(mix(iqCoolfFunc3d2(n + (0 + 57 * 0 + 113 * 1)),
                            iqCoolfFunc3d2(n + (1 + 57 * 0 + 113 * 1)), u.x),
                        mix(iqCoolfFunc3d2(n + (0 + 57 * 1 + 113 * 1)),
                            iqCoolfFunc3d2(n + (1 + 57 * 1 + 113 * 1)), u.x), u.y), u.z);

    return 1.0 - res * (1.0 / 1073741824.0);
}

float iqNoise2f( in vec2 p )
{
    ivec2 ip = ivec2(floor(p));
    vec2 u = fract(p);
    u = u * u * (3.0 - 2.0 * u);

    int n = ip.x + ip.y * 57;

    float res = mix(mix(iqCoolfFunc3d2(n + (0 + 57 * 0 + 113 * 0)),
                            iqCoolfFunc3d2(n + (1 + 57 * 0 + 113 * 0)), u.x),
                        mix(iqCoolfFunc3d2(n + (0 + 57 * 1 + 113 * 0)),
                            iqCoolfFunc3d2(n + (1 + 57 * 1 + 113 * 0)), u.x), u.y);
    return 1.0 - res * (1.0 / 1073741824.0);
}

float iqFbm2d( in vec2 p )
{
    float res = 0.5000 * iqNoise2f(p * 1.0) +
           0.2500 * iqNoise2f(p * 2.0) +
           0.1250 * iqNoise2f(p * 4.0) +
           0.0625 * iqNoise2f(p * 8.0);
    return res;
}

float iqFbm3d( in vec3 p )
{
    float res = 0.5000 * iqNoise3f(p * 1.0) +
           0.2500 * iqNoise3f(p * 2.0) +
           0.1250 * iqNoise3f(p * 4.0) +
           0.0625 * iqNoise3f(p * 8.0);		   
    return res;
}


float perlin_iq_3d( in vec3 p, in float frequency, in int octaves, in float lacunarity, in float persistence )
{
	p *= frequency;
	float curPersistence = 0.5f;
	float res = 0.0f;
	
	for( int curOctave = 0; curOctave < octaves; curOctave++ ) 
	{
		res += iqNoise3f(p) * curPersistence ;
		curPersistence *= persistence;
		p *= lacunarity;
	}   
    return res;
}


vec3 perlin_gradient_iq_3d( in vec3 position, in float frequency, in int octaves, in float lacunarity, in float persistence, in float epsilon )
{
	float n1, n2, a, b;
	vec3 curl;

//todo optimize	with use of vec3
	float x = position.x;
	float y = position.y;
	float z = position.z;

	float x1, x2, y1, y2, z1, z2;

	x1 = perlin_iq_3d( vec3( x + epsilon, y, z ), frequency, octaves, lacunarity, persistence );
	x2 = perlin_iq_3d( vec3( x - epsilon, y, z ), frequency, octaves, lacunarity, persistence );
	y1 = perlin_iq_3d( vec3( x, y + epsilon, z ), frequency, octaves, lacunarity, persistence );
	y2 = perlin_iq_3d( vec3( x, y - epsilon, z ), frequency, octaves, lacunarity, persistence );
	z1 = perlin_iq_3d( vec3( x, y, z + epsilon ), frequency, octaves, lacunarity, persistence );
	z2 = perlin_iq_3d( vec3( x, y, z - epsilon ), frequency, octaves, lacunarity, persistence );

	float f = 1. / (2 * epsilon);
	a = (y1 - y2) * f;
	b = (z1 - z2) * f;
	curl.x = a - b;

	a = (z1 - z2) * f;
	b = (x1 - x2) * f;
	curl.y = a - b;

	a = (x1 - x2) * f;
	b = (z1 - z1) * f;
	curl.z = a - b;

	return curl;
}

vec3 CurlNoiseIq( in vec3 position, in float epsilon )
{
	float n1, n2, a, b;
	vec3 curl;

//todo optimize	with use of vec3	
	float x = position.x;
	float y = position.y;
	float z = position.z;

	float x1, x2, y1, y2, z1, z2;

	x1 = iqFbm3d( vec3( x + epsilon, y, z ) );
	x2 = iqFbm3d( vec3( x - epsilon, y, z ) );
	y1 = iqFbm3d( vec3( x, y + epsilon, z ) );
	y2 = iqFbm3d( vec3( x, y - epsilon, z ) );
	z1 = iqFbm3d( vec3( x, y, z + epsilon ) );
	z2 = iqFbm3d( vec3( x, y, z - epsilon ) );
 
	float f = 1. / (2 * epsilon);
	a = (y1 - y2) * f;
	b = (z1 - z2) * f;
	curl.x = a - b;

	a = (z1 - z2) * f;
	b = (x1 - x2) * f;
	curl.y = a - b;

	a = (x1 - x2) * f;
	b = (z1 - z1) * f;
	curl.z = a - b;

	return curl;
}

#endif







































































































































































































































































































//end should be x * 100 lines
