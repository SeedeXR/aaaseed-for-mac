// GLSL fragment shader
// Sepia

// Uniform variables for texturing
uniform sampler2D tex0;

void main()
{
	vec4	color = texture2D( tex0, gl_TexCoord[0].st );

	gl_FragData[0] = vec4( color.rgb,1.0);
	gl_FragData[1] = vec4( color.r, .0, .0 ,1.0);
	gl_FragData[2] = vec4( .0, color.g, .0 ,1.0);
	gl_FragData[3] = vec4( .0, .0, color.b ,1.0);
}
