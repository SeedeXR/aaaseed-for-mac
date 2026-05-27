// GLSL fragment shader
// Foot

// Uniform variables for texturing
uniform sampler2D tex0;

// PostProcessing effects (green->blue)
uniform float	fu_float[6];

float     low_green = fu_float[0];
float     high_rb = fu_float[1];


void main()
{
	vec4	color = texture2D( tex0, gl_TexCoord[0].st );
//	float	high_rb = 1.0 - low_green ;
//	float	low_green =  1.0 - high_rb;
	if( color.g > low_green )
		if( ( color.r < high_rb ) && ( color.b < high_rb ) )
			{
			float temp;
			temp = color.g;
			color.g = color.b;
			color.b = temp;
			}
	gl_FragColor = color;
}
