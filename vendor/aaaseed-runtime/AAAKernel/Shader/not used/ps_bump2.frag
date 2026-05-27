//
// Fragment shader for procedural bumps
//
// Authors: Randi Rost, John Kessenich
//
// Copyright (c) 2002-2005 3Dlabs Inc. Ltd.
//
// See 3Dlabs-License.txt for license information
//

in vec3 LightDir;
in vec3 EyeDir;

uniform	vec4	aaa_fu_vec4[6];
uniform float	fu_float[6];

//uniform vec3  SurfaceColor;    // = (0.7, 0.6, 0.18)
float BumpDensity = fu_float[0];     // = 16.0
float BumpSize = fu_float[1];        // = 0.15
float SpecularFactor = fu_float[2];  // = 0.5

void main (void)
{
	vec3 SurfaceColor = aaa_fu_vec4[0].xyz;
    vec3 litColor;
    vec2 c = BumpDensity * gl_TexCoord[0].st;
    vec2 p = fract(c) - vec2(0.5);

    float d, f;
    d = p.x * p.x + p.y * p.y;
    f = 1.0 / sqrt(d + 1.0);

    if (d >= BumpSize)
        { p = vec2(0.0); f = 1.0; }

    vec3 normDelta = vec3(p.x, p.y, 1.0) * f;
    litColor = SurfaceColor * max(dot(normDelta, LightDir), 0.0);
    vec3 reflectDir = reflect(LightDir, normDelta);

    float spec = max(dot(EyeDir, reflectDir), 0.0);
    spec *= SpecularFactor;
    litColor = min(litColor + spec, vec3(1.0));

    gl_FragColor = vec4(litColor, 1.0);
}