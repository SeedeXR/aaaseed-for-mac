
// Uniform variables for texturing
uniform sampler2D	aaa_tex2d[4];



void main()
{
vec4	src;
vec4	dst;
float	y;
float	u;
float	v;

float	r;
float	g;
float	b;

float	c;
float	s;

float	t;

	src = texture2D( aaa_tex2d[0], gl_TexCoord[0].st);

//	g = gray( src.xyz );

	y = 0.299 * src.r + 0.587 * src.g + 0.114 * src.b;
	u = (src.b-y) * 0.565;
	v = (src.r-y) * 0.713;

//	y = src.r;
//	u = src.g;
//	v = src.b;

	c = cos(aaa_fu_float[0]);
	s = sin(aaa_fu_float[0]);

	t = u;
	u =  c*u + s*v;
	v = -s*t + c*v;

	r = y + 1.403 * v;
	g = y - 0.344 * u - 0.714 * v;
	b = y + 1.770 * u;

	dst = vec4( r, g , b, src.a );

	gl_FragColor = dst;
}

