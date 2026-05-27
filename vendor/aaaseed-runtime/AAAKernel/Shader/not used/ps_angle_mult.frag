// GLSL fragment shader
// Blend to image, using an angle and offset

// Uniform variables for texturing
uniform sampler2D tex0;
uniform sampler2D tex1;

uniform	int		fu_int[6];
uniform float	fu_float[6];
uniform vec4	aaa_fu_vec4[6];

CONST	float	weight_tex0 = fu_float[0];
CONST	float	weight_tex1 = fu_float[1];
//CONST	float	offset_x = fu_float[2];
//CONST	float	offset_y = fu_float[3];
//CONST	float   angle = fu_float[4];
CONST	vec4	color = aaa_fu_vec4[0];
//CONST	int		b_repeat = fu_float[0];

vec4 angle_mult( float angle, float offset_x, float offset_y )
{
	vec4	color2;
	vec2	coord;

	// texture coord are in the range 0-1.0, so renter to center, and add offset before rotation
	// be careful gl_TexCoord need to be generated in vertex shader
	float	x = gl_TexCoord[1].s - 0.5 + offset_x;
	float	y = gl_TexCoord[1].t - 0.5 + offset_y;
	coord = vec2( x * cos( angle ) - y * sin( angle ), x * sin( angle ) + y * cos( angle ) );
//	coord += vec2( offset_x + 0.5, offset_y + 0.5 );
	// recenter texture
	coord += vec2( 0.5, 0.5 );

	color2 = texture2D( tex1, coord );
//	if( coord.x > 1 || coord.x < 0 || coord.y > 1.0 || coord.y < 0 )
//		{
//		if( b_repeat == 0 )
//			{
//			color2 = vec4( 0.0, 0.0, 0.0, color2.a );
//			}
//		}
	return color2;
}


void main()
{
	vec4	color_1 = texture2D( tex0, gl_TexCoord[0].st );
	vec4	color_2;

	color_2 = angle_mult( fu_float[4], fu_float[3], fu_float[2] );
	color_2 *= color;
	gl_FragColor = weight_tex0 * color_1 + weight_tex1 * color_1 * color_2;
}
