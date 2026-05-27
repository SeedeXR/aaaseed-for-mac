
//END include
#pragma unroll
#pragma optionNV(unroll all)


out VS_out
{
	vec4	color;
	vec2	tex_coor;
//	vec3			normal;
//	float			fall_off;
//	float			z_to_eye;
} vs_out;

void main(void)
{
	vec4	pos = gl_Vertex;

	// Eye-coordinate position of vertex, needed in various calculations
	pos = gl_ModelViewMatrix * pos;

	// Do fixed functionality vertex transform
	gl_Position = gl_ProjectionMatrix * pos ;

	vs_out.tex_coor = gl_MultiTexCoord0.xy;

	vs_out.color = gl_Color;
}

