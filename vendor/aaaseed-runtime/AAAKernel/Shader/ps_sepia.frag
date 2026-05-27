

// Uniform variables for texturing
uniform sampler2D	aaa_tex2d[4];

// PostProcessing effects (sephia)

float     desaturate = aaa_fu_float[1];
float     toning = aaa_fu_float[0];

vec3		LightColor = aaa_fu_vec4[0].rgb; //= { 1.0, 0.9,  0.5  };
const vec3	DarkColor  = { 0.2, 0.05, 0.0  };
const vec3	grayXfer   = { 0.3, 0.59, 0.11 };

vec3 sepia(in vec3 inColor)
{
	vec3	scnColor = LightColor * inColor;
	float	gray = dot( grayXfer,   scnColor );
	vec3	muted = lerp( scnColor,  gray.xxx,   desaturate );
	vec3	sepia = lerp( DarkColor, LightColor, gray );
	vec3	result = lerp( muted,     sepia,      toning );
	return vec3( result.rgb);
}

void main()
{
	vec4	color = texture2D( aaa_tex2d[0], gl_TexCoord[0].st );
	gl_FragColor = blend( vec4( sepia( color.rgb ), color.a ) );
}
