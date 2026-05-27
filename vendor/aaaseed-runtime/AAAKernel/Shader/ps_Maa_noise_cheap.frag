
// Uniform variables for texturing
uniform sampler2D	aaa_tex2d[4];



void main (void)
{
//int	i;
vec4	cx;
vec4	cy;
vec4	color;
float	alpha;

    //
    // cheap noise
    //
    vec3 location = gl_TexCoord[0].xyz;
    vec3 floorvec = vec3( floor(aaa_fu_float[0] * location.x), floor(aaa_fu_float[0] * location.y), 0. );
    vec3 noise = location * aaa_fu_float[0] - floorvec - 0.5;
    noise *= noise;
    location += noise * aaa_fu_float[2] + aaa_fu_float[1];

	color = texture2D( aaa_tex2d[0], location.st );
	color.a = 1.0;
	gl_FragColor = color;
}

