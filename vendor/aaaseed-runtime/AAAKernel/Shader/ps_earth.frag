// GLSL fragment shader for drawing the earth with three textures
// for Aaaseed
// Author : Franz Hildgen

// Uniform variables for texturing
uniform sampler2D tex0;
uniform sampler2D tex1;
uniform sampler2D tex2;
//uniform sampler2D EarthDay;
//uniform sampler2D EarthNight;
//uniform sampler2D EarthCloudGloss;

in float Diffuse;
in vec3  Specular;

void main (void)
{
    // Monochrome cloud cover value will be in clouds.r
    // Gloss value will be in clouds.g
    // clouds.b will be unused

    vec2 clouds    = texture2D( tex2, gl_TexCoord[0].st ).rg;
    vec3 daytime   = ( texture2D( tex0, gl_TexCoord[0].st ).rgb * Diffuse +
                          Specular * clouds.g ) * ( 1.0 - clouds.r ) +
                          clouds.r * Diffuse;
    vec3 nighttime = texture2D( tex1, gl_TexCoord[0].st ).rgb *
                         (1.0 - clouds.r) * 2.0;

    vec3 color = daytime;

    if (Diffuse < 0.1)
        color = mix( nighttime, daytime, ( Diffuse + 0.1 ) * 5.0 );

    gl_FragColor = blend( vec4 ( color, 1.0 );
}