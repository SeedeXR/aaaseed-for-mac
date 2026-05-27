// Uniform variables for texturing
uniform sampler2D 	aaa_tex2d[4];

// 0 = time
// 1 = mouse X
// 2 = mouse Y
//uniform float		aaa_fu_float[];

// 0 = iterations
//uniform int			aaa_fu_int[4];

// balance between texture and effect
uniform float		aaa_fu_src;
uniform float		aaa_fu_out;

// Colors factors
// 0 = src colors
// 1 = out color
//uniform vec4		aaa_fu_vec4[8];


void main(void)
{
	vec2 uv = gl_TexCoord[0].st;
	vec4 src = texture2D( aaa_tex2d[0], uv );

	float u 	= uv.x;
	float v 	= uv.y;

	float angle = atan( u, v ) / (3.14159 * 2.) + .5;
	float a 	= ( angle + aaa_fu_float[1] ) * aaa_fu_float[0];

	float dist	= sqrt( u*u + v*v ) * 2;
	float d		=   dist * aaa_fu_float[2];
	float d_off = aaa_fu_float[3] * aaa_fu_float[2];

	if( aaa_fu_int[0] >= 0 )
	{
		d = log( d ) * pow( aaa_fu_float[4] , 2. ) * 2. - d_off + angle * aaa_fu_int[1];
		if( aaa_fu_int[2]!=0 )
			uv = vec2( d, a );
		else
			uv = vec2( a, d );
		vec4 fx  = texture2D( aaa_tex2d[0], uv* vec2(1,1) );
		gl_FragColor = aaa_fu_src * src * aaa_fu_vec4[0] + aaa_fu_out * fx * aaa_fu_vec4[1];
	//	gl_FragColor = src;
	}
	else if( aaa_fu_int[0] == -6 )
	{
		gl_FragColor = vec4( 0, mod(angle*16,1.), 0., 1.0 );
	}
	else if( aaa_fu_int[0] == -5 )
	{
		gl_FragColor = vec4( mod((u+.5)*16,1.), mod((v+.5)*16,1.)	, 0., 1.0 );
	}
	else if( aaa_fu_int[0] == -4 )
	{
		gl_FragColor = vec4( u+.5, v+.5	, 0, 1.0 );
	}
	else if( aaa_fu_int[0] == -3 )
	{
		gl_FragColor = texture2D( aaa_tex2d[0], uv );
	}
	else if( aaa_fu_int[0] == -2 )
	{
		gl_FragColor = vec4( d + d_off, a, 0, 1 ) * aaa_fu_vec4[1];
	}
	else if( aaa_fu_int[0] == -1 )
	{
		uv.x = d + d_off;
		uv.y = a;
		gl_FragColor	 = texture2D( aaa_tex2d[0], uv );
	}
}







