// GLSL vertex shader - fixed functionality done for drop
// for Aaaseed
// Author : Maa


out vec4 ecPosition;
out vec3 transformedNormal;
out float alpha;

void main(void)
{
	alpha = gl_Color.a;
	// Eye-coordinate position of vertex, needed in various calculations
	ecPosition = gl_ModelViewMatrix * gl_Vertex;

	// Do fixed functionality vertex transform
	gl_Position = ftransform();
	transformedNormal = fnormal();

	gl_FrontColor = gl_Color;

	float f = 2.;
	gl_TexCoord[1].s = gl_Vertex.x * f + .5 ;
	gl_TexCoord[1].t = gl_Vertex.y * f + .5 ;
}

