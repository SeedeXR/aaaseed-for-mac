//we put color stuff here
const vec3 gray_conv = vec3( 0.2989, 0.5870, 0.1140 );
float compute_gray(vec3 a)	{	return  dot( a.xyz, gray_conv );	}
float gray(vec3 a)			{	return  dot( a.xyz, gray_conv );	}

vec2 get_gradient_2d( in sampler2D samp, in vec2 at )
 {
	vec3 ots = vec3( vec2(1./textureSize( samp, 0 )), 0. );
	vec3 hori = texture( samp, at+ots.xz ).rgb - texture( samp, at-ots.xz ).rgb;
	vec3 vert = texture( samp, at+ots.zy ).rgb - texture( samp, at-ots.zy ).rgb;
 	return vec2( compute_gray(hori), compute_gray(vert) );
}
float get_gradient_norm_2d( in sampler2D samp, in vec2 at )	{ return length(  get_gradient_2d(samp,at) ); }

vec2 get_gradient_2d( in sampler2D samp, in vec2 at, in float size )
 {
	vec3 ots = vec3( vec2(size/textureSize( samp, 0 )), 0. );
	vec3 hori = texture( samp, at+ots.xz ).rgb - texture( samp, at-ots.xz ).rgb;
	vec3 vert = texture( samp, at+ots.zy ).rgb - texture( samp, at-ots.zy ).rgb;
 	return vec2( compute_gray(hori), compute_gray(vert) );
}
float get_gradient_norm_2d( in sampler2D samp, in vec2 at, in float size )  { return length(  get_gradient_2d(samp,at,size) ); }

vec2 get_gradient_2d( in sampler2D samp, in vec2 at, in vec3 color )
{
	vec3 ots = vec3( vec2(1./textureSize( samp, 0 )), 0. );
	vec3 hori = texture( samp, at+ots.xz ).rgb - texture( samp, at-ots.xz ).rgb;
	vec3 vert = texture( samp, at+ots.zy ).rgb - texture( samp, at-ots.zy ).rgb;
 	return vec2( compute_gray(hori*color), compute_gray(vert*color) );
}

#define	RGB_MIN .05
vec3 rgb2hsv( vec3 c )
{
	vec4 K = vec4(  0.,  -1./3.,  2./3.,  -1. );
	// 	if( c.r <= RGB_MIN && c.g <= RGB_MIN && c.b <= RGB_MIN )
	// 		return vec3( 0., 0., 0. );
#if 1
    vec4 p = mix( vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g) );
    vec4 q = mix( vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r) );
#else
	vec4 p = c.g < c.b ? vec4(c.bg, K.wz) : vec4(c.gb, K.xy);
	vec4 q = c.r < p.x ? vec4(p.xyw, c.r) : vec4(c.r, p.yzx);
#endif

	float d = q.x - min( q.w, q.y );
	float e = 1.0e-10;
	return vec3(    abs(q.z + (q.w - q.y) / (6.0 * d + e)),    d / (q.x + e),    q.x    );
}
#if 1
	vec3 hsv2rgb( vec3 c )
	{
		vec4 K = vec4( 1., 2./3., 1./3., 3. );
		vec3 p = abs( fract(c.xxx + K.xyz) * 6. - K.www );
		return c.z * mix( K.xxx, clamp(p - K.xxx, .0, 1. ), c.y );
	}
#else
	//  Function from Iñigo Quiles
	//  https://www.shadertoy.com/view/MsS3Wc
	vec3 hsv2rgb( in vec3 c )
	{
		vec3 rgb = clamp(    abs( mod( c.x*6.+vec3(.0,4.,2.), 6. ) -3. ) - 1.,    .0,    1.    );
		rgb = rgb * rgb * (3.-2.*rgb);
		return c.z * mix( vec3(1.), rgb, c.y );
	}
#endif
vec3 h2rgb( float h )	{	return hsv2rgb( vec3( h,1.,1. ) );	}
//-----------------------------------------------------------------------------
// from sRGB to Linear Electro-Optical Transfer Function (EOTF)
// sRGB / 709
float srgb_to_linear( in float v )
{
	if( v < 0.04045f )
	{
		//todo remove if ?
		if( v < 0.0f )	return 0.0f;
		else			return v * (1.0f / 12.92f);
	}
	else
		return pow( (v + 0.055f) * (1.0f / 1.055f), 2.4f );
}
float linear_to_srgb( in float v )
{
	if( v < 0.0031308f )
	{
		//todo remove if ?
		if  (v < 0.0f ) 	return 0.0f;
		else				return v * 12.92f;
	}
	else
		return 1.055f * pow(v, 1.0f / 2.4f) - 0.055f;
}







//end should be x * 100 lines
