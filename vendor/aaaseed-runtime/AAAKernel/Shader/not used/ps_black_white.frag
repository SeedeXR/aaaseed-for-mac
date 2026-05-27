
// Uniform variables for texturing
uniform sampler2D tex0;

uniform float	fu_float[6];

vec4 specExp = { 0.5, 0, 0, 0 };



vec3 bw1()
{
vec4	tex = texture2D( tex0, gl_TexCoord[0].st );

	tex *= 0.3333333333;

	float sat = tex.x + tex.y + tex.z;
	vec3 color;
	if( sat >= .1 )
		color = vec3( 1.0 );
	else
		color = vec3( 0.0 );

	return color;

}

vec3 bw2()
{
	vec4	tex = texture2D( tex0, gl_TexCoord[0].st );

	vec3 d2 = vec3( fu_float[0], -fu_float[0], 0.0 );

	vec4	texture_sum;
	texture_sum = texture2D( tex0, gl_TexCoord[0].st + d2.yz )
				+ texture2D( tex0, gl_TexCoord[0].st + d2.yx )
				+ texture2D( tex0, gl_TexCoord[0].st + d2.xy )
				+ texture2D( tex0, gl_TexCoord[0].st + d2.xz )
				+ texture2D( tex0, gl_TexCoord[0].st + d2.xx )
				+ texture2D( tex0, gl_TexCoord[0].st + d2.zy )
				+ texture2D( tex0, gl_TexCoord[0].st + d2.zx )
				+ texture2D( tex0, gl_TexCoord[0].st + d2.yy );

	texture_sum *= 0.125;


//# Moyenne Grey
	float border;
	border = texture_sum.x + texture_sum.y + texture_sum.z;
	float actual;
	actual = tex.x + tex.y + tex.z;
	border *= 0.3333333;
	actual *= 0.3333333;


//# Black or White

if( actual < .01 )
	actual = 1.0;
else
	actual = 0.0;

if( border >= .01 )
	border = 1.0;
else
	border = 0.0;

	vec3 color;
//	color = max( 0.0,  border * actual );
	color = max( tex.xyz, vec3( border * actual ) );
	return color;
//# Compare

}

void main()
{
	if( fu_float[1] >= 1.0 )
		gl_FragColor.xyz = bw2();
	else
		gl_FragColor.xyz = bw1();
}