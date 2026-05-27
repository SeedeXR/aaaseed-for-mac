//
// Fragment shader for wobbling a texture
//
// Author: Antonio Tejada
//
// Copyright (c) 2002-2004 3Dlabs Inc. Ltd.
//
// See 3Dlabs-License.txt for license information
//

in float LightIntensity;

uniform float	fu_float[6];
float	StartRad = fu_float[0];
vec2	Freq = vec2( fu_float[1], fu_float[2] ) ;
vec2	Amplitude = vec2( fu_float[3],  fu_float[4] );

uniform sampler2D WobbleTex;

void main (void)
{
    vec2  perturb;
    float rad;
    vec3  color;

    // Compute a perturbation factor for the x-direction
    rad = (gl_TexCoord[0].s + gl_TexCoord[0].t - 1.0 + StartRad) * Freq.x;

    // Wrap to -2.0*PI, 2*PI
    rad = rad * C_2PI_I;
    rad = fract(rad);
    rad = rad * C_2PI;

    // Center in -PI, PI
    if (rad >  PI) rad = rad - PI2;
    if (rad < -PI) rad = rad + PI2;

    // Center in -PI/2, PI/2
    if (rad >  PI_HALF) rad =  PI - rad;
    if (rad < -PI_HALF) rad = -PI - rad;

    perturb.x  = (rad - (rad * rad * rad / 6.0)) * Amplitude.x;

    // Now compute a perturbation factor for the y-direction
    rad = (gl_TexCoord[0].s - gl_TexCoord[0].t + StartRad) * Freq.y;

    // Wrap to -2*PI, 2*PI
    rad = rad * C_2PI_I;
    rad = fract(rad);
    rad = rad * C_2PI;

    // Center in -PI, PI
    if (rad >  PI) rad = rad - PI2;
    if (rad < -PI) rad = rad + PI2;

    // Center in -PI/2, PI/2
    if (rad >  PI_HALF) rad =  PI - rad;
    if (rad < -PI_HALF) rad = -PI - rad;

    perturb.y  = (rad - (rad * rad * rad / 6.0)) * Amplitude.y;

    color = vec3 (texture2D(WobbleTex, perturb + gl_TexCoord[0].st));

	vec4 color2 = vec4 (color * LightIntensity, 1.0);

//	if( color2 == vec4(0.0, 0.0, 0.0, 1.0) ) discard;
    gl_FragColor = color2; //vec4 (color * LightIntensity, 1.0);
}