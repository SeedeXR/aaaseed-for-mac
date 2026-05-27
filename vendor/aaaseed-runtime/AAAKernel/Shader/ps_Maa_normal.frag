
// Uniform variables for texturing
uniform sampler2D	aaa_tex2d[4];


in vec3 transformedNormal;

void main()
{
	vec4	src;
	src = texture2D( aaa_tex2d[0], gl_TexCoord[0].st);

	vec3 nor = { 0., 0., 1. };
	float gr = dot( transformedNormal, nor );
	src.r = abs( transformedNormal.x );
	src.g = abs( transformedNormal.y );
	src.b = gr;

	gl_FragColor = blend( src );
	//gl_FragColor = src;
}

