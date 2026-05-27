layout(location = 0) in vec3 in_position;
//layout(location = 1) in vec2 in_texcoord;

layout(location = 0) out ST_AAA_BV BV;

CONST bool b_ndc = aaa_vu_int[0] != 0;

void main(void)
{
 	vec4	pos = vec4(in_position, 1.);

	BV.pos_world = pos;
	
	if(  aaa_vu_int[0] != 0 ) 
	{
		//no camera tramsformation... we just ouput what's in      
		gl_Position = pos;
		return;
	}
	else
	{   
		//Maa is still old school 
		BV.tex_coor[0].st = gl_MultiTexCoord0.xy;
	}

	// Eye-coordinate position of vertex, needed in various calculations
	//	pos = gl_ModelViewMatrix * pos;
	// Do fixed functionality vertex transform
//	gl_Position = gl_ProjectionMatrix * pos ;

	gl_Position = gl_ModelViewProjectionMatrix * pos; 
}

