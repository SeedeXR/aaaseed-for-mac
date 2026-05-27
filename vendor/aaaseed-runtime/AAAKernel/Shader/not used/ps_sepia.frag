// GLSL fragment shader
// Sepia

// Uniform variables for texturing
uniform sampler2D tex0;

// PostProcessing effects (sephia)
uniform float	fu_float[6];
uniform vec4	aaa_fu_vec4[6];

float     desaturate = fu_float[2];
float     toning = fu_float[1];
float	alpha = fu_float[3];
float	orig_per = fu_float[0];

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
	vec4	color = texture2D( tex0, gl_TexCoord[0].st );

	vec4	sepia_color = vec4( sepia( color.rgb ), alpha );

	vec3	black = (0.0, 0.0, 0.0 );
	if( sepia_color.rgb == black )
		sepia_color.a = 1.0;
	if( orig_per > 1.0 )
		orig_per = 1.0;
	gl_FragColor = sepia_color* orig_per + ( 1.0 - orig_per ) * color;

}
