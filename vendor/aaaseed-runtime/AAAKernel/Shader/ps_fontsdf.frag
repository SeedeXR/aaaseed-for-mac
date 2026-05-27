

layout(location = 0) in vec2 out_uv;

uniform sampler2D  aaa_samp0;

// 0 = Color
//uniform float		aaa_fu_float[8];


float median( float r, float g, float b )
{
	return max( min( r, g ), min( max( r, g ), b ) );
}

void main()
{
	vec4 uColor = aaa_fu_vec4[ 0 ];
	//vec4 uColor = vec4( 1.0, 1.0, 1.0, 1.0 );
	vec2 pos = vec2( out_uv.x, out_uv.y);
	vec3 samp = texture( aaa_samp0, pos ).rgb;
	ivec2 sz = textureSize( aaa_samp0, 0).xy;

	float dx = dFdx(pos.x) * sz.x;
	float dy = dFdy(pos.y) * sz.y;
	float toPixels = 8.0 * inversesqrt(dx * dx + dy * dy);
	float sigDist = median( samp.r, samp.g, samp.b );
	float w = fwidth(sigDist);
	float opacity = smoothstep(0.5 - w, 0.5 + w, sigDist);

	gl_FragColor = vec4( uColor.rgb, opacity * uColor.a );
}