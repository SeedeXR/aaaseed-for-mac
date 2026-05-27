

//uniform float	aaa_vu_float[6];
//	0	:	base size of part
//	1	:	factor size for attrib

//uniform vec4	aaa_vu_vec4[];

in 	vec4 aaa_attrib;	//	Vertex Attribute coming from opencl
//out	vec4 aaa_att;		//	this to pass to the fragment shader
out	float	sel_x;				//	this to pass to the fragment shader
out	float	sel_y;				//	this to pass to the fragment shader
out	float	aaa_att_w;			//	this to pass to the fragment shader

void main()
{
	float alpha = gl_Color.a;
	//	store world position
	vec4 pos = gl_Vertex;
	float z = pos.w;
	pos.w = 1.;

	//	Eye-coordinate position of vertex, needed in various calculations
	//		and used in the next transform
	vec4 ecPosition = gl_ModelViewMatrix * pos;

	// Do fixed functionality vertex transform
	gl_Position = gl_ProjectionMatrix * ecPosition;
	//was gl_Position = ftransform();

	gl_FrontColor = gl_Color;

	//aaa_att_x = aaa_attrib.x;
	//aaa_att_y = aaa_attrib.y;
	//aaa_att.z = 1;

	//	pass the attrib to the fragment shader
	//aaa_att = aaa_attrib;	//last change
	//aaa_att = .5;
	//	passed use to change texture
	//sel_x = floor(aaa_attrib.x * 3.99999) * .25;
	//sel_y = floor(aaa_attrib.y * 3.99999) * .25;
	sel_x = aaa_attrib.x;
	sel_y = aaa_attrib.y;
	aaa_att_w = aaa_attrib.w;

	//	deal width point size
	//gl_Vertex.w = 1.;
	z = pow( z, aaa_vu_float[0] );

	//	set it directly, need to pass a factor to control from the interface
	gl_PointSize = ((aaa_vu_float[1] * (1-z) + z * aaa_vu_float[2]) * aaa_attrib.x) * ( 1. + (aaa_attrib.z * .0000002) * (aaa_attrib.z * .0000002) )	 ;
}

