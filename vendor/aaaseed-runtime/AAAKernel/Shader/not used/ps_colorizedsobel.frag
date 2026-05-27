// Colorized Bi-directional Sobel edge detection

uniform sampler2D tex0;
//uniform vec4	tex_attribs;
uniform float	fu_float[6];
// Uniforms for the maximum Texture and Window Size
uniform vec2	winsize;
uniform vec2	texsize;

void main (void)
{
		float NPixels = fu_float[0];
//		float	delta_x = NPixels/texsize.x;
//		float 	delta_y = NPixels/texsize.y;
		float	delta_x = NPixels;
		float 	delta_y = NPixels;
 //todoopt use swizzle
        vec4 sample1 = texture2D(tex0, gl_TexCoord[0].xy + vec2(0.0, delta_y));
        vec4 sample2 = texture2D(tex0, gl_TexCoord[0].xy + vec2(-delta_x, delta_y));
        vec4 sample3 = texture2D(tex0, gl_TexCoord[0].xy + vec2(-delta_x, 0.0));
        vec4 sample4 = texture2D(tex0, gl_TexCoord[0].xy + vec2(-delta_x, -delta_y));
        vec4 sample5 = texture2D(tex0, gl_TexCoord[0].xy + vec2(0.0, -delta_y));
        vec4 sample6 = texture2D(tex0, gl_TexCoord[0].xy + vec2(delta_x, -delta_y));
        vec4 sample7 = texture2D(tex0, gl_TexCoord[0].xy + vec2(delta_x, 0.0));


		vec4 tex_attribs;
		tex_attribs.x = 1 / texsize.x;
		tex_attribs.y = 1 / texsize.y;

     	vec4 sample8 = texture2D(tex0, gl_TexCoord[0].xy); // + tex_attribs));

        vec4 accum = sample8 - sample4;
        vec4 accum2 = max(accum - sample2 + sample3 * -2.0, -2.0) + sample6 + sample7 * 2.0;
        accum = min(accum + sample2 + sample1 * 2.0, 2.0) + sample5 * -2.0 - sample6;

        vec3 together = vec3(accum * accum + accum2 * accum2);

//        gl_FragColor =vec4(sqrt(together), 0.0);
        gl_FragColor =vec4(accum2, 0.0);
}
