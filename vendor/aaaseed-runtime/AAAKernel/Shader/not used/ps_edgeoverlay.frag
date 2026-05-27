

// Uniform variables for texturing
uniform sampler2D tex0;

// Uniforms for the maximum Texture and Window Size
uniform vec2	winsize;
uniform vec2	texsize;

uniform float	fu_float[6];

vec4	grey = vec4(0.33333);

float getGray(vec4 c)
{
    return(dot(c.rgb,grey.xxx)); //((0.33333).xxx)));
}

void main (void)
{
//		int	i;
	vec4	tmp = vec4(0.0);
	vec4	color;
	float	alpha;

	float NPixels = fu_float[0];
	float Threshold = fu_float[1] * fu_float[1];

	float	delta_x = NPixels/texsize.x;
	float 	delta_y = NPixels/texsize.y;

 //todoopt use swizzle
	tmp = texture2D(tex0, gl_TexCoord[0].xy + vec2(-delta_x, -delta_y));
	float g00 = getGray(tmp);
	tmp = texture2D(tex0, gl_TexCoord[0].xy + vec2(0.0, -delta_y));
	float g01 = getGray(tmp);
	tmp = texture2D(tex0, gl_TexCoord[0].xy + vec2(delta_x, -delta_y));
	float g02 = getGray(tmp);
	tmp = texture2D(tex0, gl_TexCoord[0].xy + vec2(-delta_x, 0.0));
	float g10 = getGray(tmp);
	tmp = texture2D(tex0, gl_TexCoord[0].xy + vec2(delta_x, 0.0));
	float g12 = getGray(tmp);
	tmp = texture2D(tex0, gl_TexCoord[0].xy + vec2(-delta_x, delta_y));
	float g20 = getGray(tmp);
	tmp = texture2D(tex0, gl_TexCoord[0].xy + vec2(0.0, delta_y));
	float g21 = getGray(tmp);
	tmp = texture2D(tex0, gl_TexCoord[0].xy + vec2(delta_x, delta_y));
	float g22 = getGray(tmp);


	float sx = 0.0;
	sx = g22 + 2.0*g21+g20-g02-g01*2.0-g00;
	float sy;
	sy = g22-g20+g12*2.0-2.0*g10+g02-g00;

	float dist = sx*sx + sy*sy;

	if (dist > Threshold)
		{
//			color = ( texture2D( tex0,gl_TexCoord[0].st ).rgb, 1.0 );
		discard; //color = vec4(0.0,0.0,0.0,0.25);
		}
	else
		if( dist > (Threshold * 0.5) )
			alpha = 1.0 - smoothstep( Threshold * 0.5, Threshold, dist );
		else
			alpha = 1.0;
	color.rgb = texture2D(tex0,gl_TexCoord[0].st).rgb;
	color.a = alpha;
	gl_FragColor = blend( color ); //(texture2D( tex0,gl_TexCoord[0].st).rgb, 1-smoothstep( Threshold/2, Threshold, dist) ); ;
}
