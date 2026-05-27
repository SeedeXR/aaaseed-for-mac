
const float PI = acos(-1);		//#define	PI 3.141592653589793238462643383279502884197169399375105820974944592307816406286f
const float PI2 = PI*2.;		//#define PI 3.14159265358979323846264338327950288419716
const float PI2_OVER = 1. / (PI2);
const float PI_HALF =  PI / 2.;

float saturate( in float v )  { return clamp( v, 0.,		1.			); }
vec2  saturate( in vec2  v )  { return clamp( v, vec2(0.), 	vec2(1.)	); }
vec3  saturate( in vec3  v )  { return clamp( v, vec3(0.), 	vec3(1.)	); }
vec4  saturate( in vec4  v )  { return clamp( v, vec4(0.), 	vec4(1.)	); }
#define clamp_01 saturate

float linearstep( float begin, float end, float t )	{ return clamp_01( (t-begin) / (end-begin) ); }
//float linearstep( vec2 begin,  vec2 end,  vec2 t  )	{ return clamp_01( (t-begin) / (end-begin) ); }
// void make_coor_sys_using_y( in vec3 a, out vec3 b, out vec c )
// {
// 	b = normalize( cross( a, vec3( 0.,1.,0.) ) );
// 	c = cross( b, a ) ;
// }

const uint disc_sample_count = 16;
#if 0
	//version Julien
	const vec2 disc_kernel[disc_sample_count] = vec2[disc_sample_count]
	(
		vec2( 0.2069841f, 	 0.2245269f), 	vec2( 0.525808f, 	-0.2310817f),	vec2( 0.1383943f, 	 0.9056122f),	vec2( 0.6633009f, 	 0.5129843f),
		vec2(-0.4002792f, 	 0.3727058f),	vec2( 0.07912822f,  -0.6512921f),	vec2(-0.7726067f, 	-0.5151217f),	vec2(-0.3843193f, 	-0.1494132f),
		vec2(-0.9107782f, 	 0.2500633f),	vec2( 0.6940153f, 	-0.7098927f),	vec2(-0.1964669f, 	-0.379389f),	vec2(-0.4769286f, 	 0.1840863f),
		vec2(-0.247328f,  	-0.8798459f),	vec2( 0.4206541f, 	-0.714772f),	vec2( 0.5829348f, 	-0.09794202f),	vec2( 0.3691838f, 	 0.4140642f)
	);
#else
	// poissonDisk from http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-16-shadow-mapping/
	// same in nvidia paper: http://developer.download.nvidia.com/whitepapers/2008/PCSS_Integration.pdf
	const vec2 disc_kernel[disc_sample_count] = vec2[disc_sample_count]
	(
		vec2( -0.94201624,	-0.39906216 ),	vec2(  0.94558609,	-0.76890725 ),	vec2( -0.094184101,	-0.92938870 ),	vec2(  0.34495938,   0.29387760 ),
		vec2( -0.91588581,	 0.45771432 ),	vec2( -0.81544232,	-0.87912464 ),	vec2( -0.38277543, 	 0.27676845 ),	vec2(  0.97484398, 	 0.75648379 ),
		vec2(  0.44323325,	-0.97511554 ),	vec2(  0.53742981,	-0.47373420 ),	vec2( -0.26496911,	-0.41893023 ),	vec2(  0.79197514,	 0.19090188 ),
		vec2( -0.24188840,	 0.99706507 ),	vec2( -0.81409955,	 0.91437590 ),	vec2(  0.19984126,	 0.78641367 ),	vec2(  0.14383161,	-0.14100790 )
	);
#endif

void rotate( inout vec2 xy, float angle )
{
	float si = sin( angle );
	float co = cos( angle );
	xy = mat2( co, -si, si, co ) * xy;
}
void rotate2_cos_sin( inout vec2 vec, in vec2 sc )
{
//	mat2 m = mat2( sc.x, sc.y, -sc.y, sc.x );
	vec = mat2( sc.x, sc.y, -sc.y, sc.x ) * vec;
}
void rotate2_cos_sin_neg( inout vec2 vec, in vec2 sc )
{
//	mat2 m = mat2( sc.x, -sc.y, sc.y, sc.x );
	vec = mat2( sc.x, -sc.y, sc.y, sc.x ) * vec;
}

// by iq. http://iquilezles.org/www/articles/smin/smin.htm
float smax(float a, float b, float k) {
	float h = clamp(0.5 + 0.5 * (b - a) / k, 0.0, 1.0);
	return mix(a, b, h) + k * h * (1.0 - h);
}
float smin( float a, float b, float k )
{
    float h = max( k-abs(a-b), 0.0 )/k;
    return min( a, b ) - h*h*k*(1.0/4.0);
}
float sminCubic( float a, float b, float k )
{
    float h = max( k-abs(a-b), 0.0 )/k;
    return min( a, b ) - h*h*h*k*(1.0/6.0);
}
//done by Mâa testing Deepseek. These version are signed and function with input in [-1,1]
float bias( in const float v, in const float b ) { const float abs_v = abs(v); return v * b / (abs_v * b + (1. - abs_v) * (1. - b)); }
vec2  bias( in const vec2  v, in const vec2  b ) { const vec2  abs_v = abs(v); return v * b / (abs_v * b + (1. - abs_v) * (1. - b)); }
vec3  bias( in const vec3  v, in const vec3  b ) { const vec3  abs_v = abs(v); return v * b / (abs_v * b + (1. - abs_v) * (1. - b)); }
vec4  bias( in const vec4  v, in const vec4  b ) { const vec4  abs_v = abs(v); return v * b / (abs_v * b + (1. - abs_v) * (1. - b)); }
float gain( in const float v, in const float g ) { return (bias( v*2.-1., 1.-g ) + 1.) * .5; }
vec2  gain( in const vec2  v, in const vec2  g ) { return (bias( v*2.-1., 1.-g ) + 1.) * .5; }
vec3  gain( in const vec3  v, in const vec3  g ) { return (bias( v*2.-1., 1.-g ) + 1.) * .5; }
vec4  gain( in const vec4  v, in const vec4  g ) { return (bias( v*2.-1., 1.-g ) + 1.) * .5; }
















//end should be x * 100 lines
