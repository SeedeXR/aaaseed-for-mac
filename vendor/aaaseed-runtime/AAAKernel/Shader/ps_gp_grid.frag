
// Uniform variables for texturing
uniform sampler2D	aaa_tex2d[4];


void main (void)
{
	vec2 uv = gl_TexCoord[0].st;
	uv.s *= aaa_fu_float[0];
	uv.t *= aaa_fu_float[1];

	float rep = float(aaa_fu_int[0]);
	float d = aaa_fu_float[3] * .5;
	vec2 uvb = fract( (uv + d) / rep ) * rep;

	if( any( lessThan( uvb.rg, vec2(aaa_fu_float[3]) ) ) )
		gl_FragColor = aaa_fu_vec4[2];
	else
	{
		uv = abs( fract( uv - .5 ) - .5 );
		float f = min( uv.r, uv.g );
		gl_FragColor = aaa_fu_vec4[ ( f < (aaa_fu_float[2] * .5) ) ? 1 : 0 ];
	}
}
