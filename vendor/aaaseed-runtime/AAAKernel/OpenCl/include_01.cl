// opencl include v0 : this a test

__constant	float3	__grey_conv = { 0.2989, 0.5870, 0.1140 };
float rgb_to_luma( float3 color )
{
	return dot( color, __grey_conv );
}

/*
float3 hsv_to_rgb( float h, float s, float v )
{
	float c = v * s;
	h = fmod(h * 6.0f, 6.0f);
	float x = c * (1.0 - fabs( fmod(h, 2.0f) - 1.0f) );
	float3 color;

	if		( h < 1.0 )		color = (float3)(c, x, 0.0);
	else if ( h < 2.0 )		color = (float3)(x, c, 0.0);
	else if ( h < 3.0 ) 	color = (float3)(0.0, c, x);
	else if ( h < 4.0 ) 	color = (float3)(0.0, x, c);
	else if ( h < 5.0 ) 	color = (float3)(x, 0.0, c);
	else 					color = (float3)(c, 0.0, x);

	color += v - c;

	return color;
}
*/

#define SWAP(type, a, b) \
  { \
    type sw_ap; \
    sw_ap = (a); \
    (a) = (b); \
    (b) = sw_ap; \
  }

float min_ff( float a, float b )	{	return (a < b) ? a : b;	}

float3 hsv_to_rgb( float h, float s, float v )
{
	float nr, ng, nb;

	nr = fabs(h * 6.0f - 3.0f) - 1.0f;
	ng = 2.0f - fabs(h * 6.0f - 2.0f);
	nb = 2.0f - fabs(h * 6.0f - 4.0f);

	clamp( nr, 0.0f, 1.0f );
	clamp( nb, 0.0f, 1.0f );
	clamp( ng, 0.0f, 1.0f );

	float3 color;
	color.x = ((nr - 1.0f) * s + 1.0f) * v;
	color.y = ((ng - 1.0f) * s + 1.0f) * v;
	color.z = ((nb - 1.0f) * s + 1.0f) * v;
	return color;
}

float3 rgb_to_hsv( float r, float g, float b )
{
	float k = 0.0f;
	float chroma;
	float min_gb;

	if( g < b)
	{
		SWAP(float, g, b);
		k = -1.0f;
	}
	min_gb = b;
	if (r < g) {
		SWAP(float, r, g);
		k = -2.0f / 6.0f - k;
		min_gb = min_ff(g, b);
	}

	chroma = r - min_gb;

	float3 color;
	color.x = fabs(k + (g - b) / (6.0f * chroma + 1e-20f));
	color.y = chroma / (r + 1e-20f);
	color.z = r;

	return color;
}
float3 hsv_to_rgbf3( float3 src )	{	return hsv_to_rgb( src.x, src.y, src.z );	}
float3 rgb_to_hsvf3( float3 src )	{	return rgb_to_hsv( src.x, src.y, src.z );	}
