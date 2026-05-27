
// Uniform variables for texturing
uniform sampler2D tex0;

// assume "t" ranges from 0 to 1 safely
// brute-force this, it's running on the CPU
vec3 c_bezier(in vec3 c0, in vec3 c1, in vec3 c2, in vec3 c3, in float t)
{
	float t2 = t*t;
	float t3 = t2*t;
	float nt = 1.0 - t;
	float nt2 = nt*nt;
	float nt3 = nt2 * nt;
	vec3 b = nt3*c0 + (3.0*t*nt2)*c1 + (3.0*t2*nt)*c2 + t3*c3;
	return b;
}

// function used to fill the volume noise texture
vec4 color_curve(in vec2 Pos)
{
    vec3 color0 = vec3(-0.1,0.0,0.3);
    vec3 color1 = vec3(1.1,0.3,-0.1);
    vec3 color2 = vec3(0.1,1.4,0.0);
    vec3 color3 = vec3(0.3,0.3,1.1);
	vec3 sp = c_bezier(color0,color1,color2,color3,Pos.x);
    return vec4(sp,1);
}

const vec3 GrayConv = { 0.25, 0.65, 0.10 };

vec4 gryGradPS(in sampler2D texture, in vec2 texcoord)
{
	vec4 scnColor;
	scnColor = texture2D(texture, texcoord.st);
	float  n = dot( scnColor.xyz, GrayConv );
	return vec4( color_curve( vec2(n,0) ).xyz,scnColor.w );
}

void main()
{
	vec4 color;

	color.xyz = gryGradPS(tex0, gl_TexCoord[0].st).xyz;
	color.a = 1.0;

	gl_FragColor = blend( color );
}
