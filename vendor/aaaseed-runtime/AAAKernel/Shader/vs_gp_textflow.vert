
//out vec4 ecPosition;
//out vec3 transformedNormal;
//out float alpha;
//out float	aaa_vu_float[6];
//	0	:	base size of part
//	1	:	factor size for attrib

in 	vec4 aaa_attrib;	//	Vertex Attribute coming from opencl
//out	vec4 aaa_att;		//	this to pass to the fragment shader
out	float	aaa_att_w;			//	this to pass to the fragment shader

void main(void)
{
	float alpha = gl_Color.a;
	//	Eye-coordinate position of vertex, needed in various calculations
	//		and used in the next transform
	vec4 ecPosition = gl_ModelViewMatrix * gl_Vertex;

	// Do fixed functionality vertex transform
	gl_Position = gl_ProjectionMatrix * ecPosition;
	//was gl_Position = ftransform();

	gl_FrontColor = gl_Color;

	//	pass the attrib to the fragment shader
	//aaa_att = aaa_attrib;	//last change
	//aaa_att = .5; ;

	//aaa_att_x = aaa_attrib.x;
	//aaa_att_y = aaa_attrib.y;
	//aaa_att.z = 1;
	aaa_att_w = aaa_attrib.w;

	//	deal width point size
	//gl_Vertex.w = 1.;
	float d  = aaa_att_w ;
	//	set it directly, need to pass a factor to control from the interface
	d = aaa_vu_float[0] + d * (aaa_vu_float[1]-aaa_vu_float[0]);
	if( d <= 0.00 )
		d = 0.000;
	gl_PointSize = d;
//	gl_PointSize = clamp( d*7., 2., 256.);
//	gl_PointSize = aaa_attrib.x * 128.;
//	gl_PointSize = aaa_att.x * 128.;
}

