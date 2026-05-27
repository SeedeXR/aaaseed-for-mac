// GLSL fragment shader
// Slide V

// Uniform variables for texturing
uniform sampler2D tex0;
uniform	sampler2D tex1;

uniform float	fu_float[6];

void main()
{
	float	time = fu_float[0];
	vec4	color;
	vec2	coord_1 = vec2( 0, time );
	if( gl_TexCoord[0].t < time )
		color = texture2D( tex0, gl_TexCoord[0].st - coord_1 );
	else
		color = texture2D( tex1, gl_TexCoord[0].st - coord_1 );
    gl_FragColor = color;
}
