
out ST_AAA_BV BV;

void main(void)
{
	// Eye-coordinate position of vertex, needed in various calculations
	BV.pos_ec = gl_ModelViewMatrix * gl_Vertex;

	// Do fixed functionality vertex transform
	//gl_Position = ftransform();
	gl_Position = gl_ProjectionMatrix * BV.pos_ec;

	//transformedNormal = normalize( gl_NormalMatrix * normal );

	BV.color = gl_Color;
//	gl_BackColor = gl_Color;
	BV.color.a = 1.0;	//counter bug ?
	
	BV.tex_coor[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
}

