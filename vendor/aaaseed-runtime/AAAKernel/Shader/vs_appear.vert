
out vec4	ecPosition;
//uniform float	aaa_vu_float[];
//	0	:	base size of part
//	1	:	factor size for attrib

in	vec4 aaa_attrib;	//	Vertex Attribute coming from opencl
out	float	sel_x;		//	this to pass to the fragment shader
out	float	sel_y;		//	this to pass to the fragment shader
out	float	aaa_att_z;		//	this to pass to the fragment shader
//out	float	aaa_att_w;		//	this to pass to the fragment shader

#define	AAADEBUG	0

void main(void)
{
	float alpha = gl_Color.a;

	//	Eye-coordinate position of vertex, needed in various calculations
	//		and used in the next transform
#if	AAADEBUG
	//gl_Vertex.x = gl_Vertex.x * .6;
	//gl_Vertex.y = gl_Vertex.y * .6 + gl_Vertex.x * .5;
	//gl_Vertex.z = 0.;
	vec4 v = gl_Vertex;
	v.z = 0.;
	//v.w = 4.;
	ecPosition = gl_ModelViewMatrix * v;
#else
	ecPosition = gl_ModelViewMatrix * gl_Vertex;
#endif
	// Do fixed functionality vertex transform
	gl_Position = gl_ProjectionMatrix * ecPosition;

#if	AAADEBUG
	//gl_Position.x = gl_Position.x - .5;
	//gl_Position.y = gl_Position.y - .1;
	//gl_Position.z = 0.;
#endif
	//was gl_Position = ftransform();

#if	AAADEBUG
	gl_FrontColor.r = 1.;
	gl_FrontColor.g = 1.;
	gl_FrontColor.b = 1.;
	gl_FrontColor.a = 1.;
#else
	gl_FrontColor = gl_Color;
#endif
	//gl_FrontColor.a = 1.0;	//counter bug ?

	//	pass the attrib to the fragment shader
	//aaa_att = aaa_attrib;	//last change
	//	passed use to change texture
	sel_x = float( int( aaa_attrib.x * 4.) ) * .25;
	sel_y = float( int( aaa_attrib.y * 2.) ) * .25;

#if	AAADEBUG
	aaa_att_z = 0.;
#else
	aaa_att_z = gl_Vertex.z;
#endif

	//aaa_att_w = aaa_attrib.w;
	//	try to supress a crash
	//if( aaa_attrib.w == 0. )
	//	return;
	//aaa_att = .5;

	//	deal width point size
	//gl_Vertex.w = 1.;
#if	AAADEBUG
	float d = 0.;
#else
	float d  = gl_Vertex.z;
#endif
	d = 1.-d;
	//d = d*d;
	//d = d*d;
	//		d = d*d;
#if	AAADEBUG
	gl_PointSize = 40.;
#else
	//	set it directly, need to pass a factor to control from the interface
	gl_PointSize = 	d * ( aaa_vu_float[0] + aaa_attrib.x * aaa_vu_float[1] );
#endif
//	gl_PointSize = 	50;
//	gl_PointSize = clamp( d*7., 2., 256.);
	//gl_PointSize = aaa_attrib.x * 128.;
	//gl_PointSize = aaa_att.x * 128.;
}

