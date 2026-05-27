// GLSL fragment shader
// Lattice Shader
//

uniform sampler2D	aaa_tex2d[4];

in vec3  DiffuseColor;
in vec3  SpecularColor;

void main (void)
{
//	float	scale_s = aaa_fu_float[0];
//	float	scale_t = aaa_fu_float[1];
//    float	ss = fract(gl_TexCoord[0].s * scale_s );//Scale.s);
 //   float	tt = fract(gl_TexCoord[0].t * scale_t );//Scale.t);
    float	ss = fract(gl_TexCoord[0].s * aaa_fu_float[2] );
    float	tt = fract(gl_TexCoord[0].t * aaa_fu_float[3] );

//    if ((ss > Threshold.s) && (tt > Threshold.t))
//	float threshold_s = aaa_fu_float[0];
//	float threshold_t = aaa_fu_float[1];

    if( ( ss > aaa_fu_float[0] ) && ( tt > aaa_fu_float[1] ) )
		discard;

	vec4	SurfaceColor;
	if( aaa_fu_float[4] >= 1.0 )
		SurfaceColor = gl_Color;
	else
		SurfaceColor = texture2D( aaa_tex2d[0], gl_TexCoord[0].st );
    vec3	finalColor = SurfaceColor.rgb * DiffuseColor + SpecularColor;

//	gl_FragColor =vec4( finalColor.rgb, SurfaceColor.a );
	gl_FragColor = blend( vec4( finalColor.rgb, SurfaceColor.a ) );
}
