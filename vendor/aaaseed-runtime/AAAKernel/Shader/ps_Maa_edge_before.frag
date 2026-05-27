
// Uniform variables for texturing
uniform sampler2D	aaa_tex2d[4];



/*
vec4	grey = vec4(0.33333);

float getGray(vec4 c)
{
	return( dot(c.rgb,grey.xxx) ); //((0.33333).xxx)));
}
*/

void main (void)
{
//int	i;
vec4	tmp = vec4(0.0);
vec4	color;
float	alpha;

float NPixels = aaa_fu_float[0];
float Threshold = aaa_fu_float[1] * aaa_fu_float[1];

ivec2	tex_suv = textureSize( aaa_tex2d[0], 0 );
float	delta_x = NPixels/tex_suv.x;
float 	delta_y = NPixels/tex_suv.y;

 //todoopt use swizzle
	tmp = texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2(-delta_x, -delta_y));
	float g00 = gray(tmp);

	tmp = texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2(0.0, -delta_y));
	float g01 = gray(tmp);

	tmp = texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2(delta_x, -delta_y));
	float g02 = gray(tmp);

	tmp = texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2(-delta_x, 0.0));
	float g10 = gray(tmp);

	tmp = texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2(delta_x, 0.0));
	float g12 = gray(tmp);

	tmp = texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2(-delta_x, delta_y));
	float g20 = gray(tmp);

	tmp = texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2(0.0, delta_y));
	float g21 = gray(tmp);

	tmp = texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2(delta_x, delta_y));
	float g22 = gray(tmp);


float sx;
	sx = g22 +2.0*g21 +g20 -g02 -2.0*g01 -g00;
float sy;
	sy = g22 +2.0*g12 +g02 -g20 -2.0*g10 -g00;

float dist = sx*sx + sy*sy;

	if (dist > Threshold)
		{

		color = texture2D( aaa_tex2d[0], gl_TexCoord[0].st );
//		discard; //color = vec4(0.0,0.0,0.0,0.25);
		}
	else
		{
		discard;
//		if( dist > (Threshold * 0.5) )
//			alpha = 1.0 - smoothstep( Threshold * 0.5, Threshold, dist );
//		else
//			alpha = 1.0;
		}
//	color.rgb = texture2D( aaa_tex2d[0], gl_TexCoord[0].st).rgb;
//	color.a = alpha;
//	gl_FragColor = blend( color ); //(texture2D( aaa_tex2d[0], gl_TexCoord[0].st).rgb, 1-smoothstep( Threshold/2, Threshold, dist) );
	gl_FragColor = color;
}
