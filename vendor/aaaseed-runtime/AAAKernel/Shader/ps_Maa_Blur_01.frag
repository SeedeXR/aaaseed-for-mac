
// Uniform variables for texturing
uniform sampler2D	aaa_tex2d[4];



float doit()
{
float	g;
float	tmp;

//vec4	c;

float	inv_x = 1./float(aaa_fu_int[0]-1);
float	inv_y = 1./float(aaa_fu_int[1]-1);

ivec2	tex_suv = textureSize( aaa_tex2d[0], 0 );
float	dx = aaa_fu_float[4]/tex_suv.x;
float 	dy = aaa_fu_float[5]/tex_suv.y;
float	sx	= gl_TexCoord[0].x - dx * .5 ;
float	sy	= gl_TexCoord[0].y - dy * .5 ;

		dx = dx * inv_x;
		dy = dy * inv_y;

vec2	pos;
int		i;
int		j;

float	ax;
float	ay;
float	dax = inv_x * 3.1415926535897932384626433832795;
float	day = inv_y * 3.1415926535897932384626433832795;
float	cos_x;

//		c =	vec4(0.,0.,0.,0.);
		g = 0;
		pos.x = sx;
		ax = -1.5707963267948966192313216916398;
		for( i=aaa_fu_int[0]; i>0; i--)
			{
			cos_x = cos( ax) - 1.;
			pos.y = sy;
			ay = -1.5707963267948966192313216916398;
			for( j=aaa_fu_int[1]; j>0; j--)
				{
				tmp = cos_x + cos( ay);
				if( tmp > 0. )
					g += texture2D( aaa_tex2d[0], pos ).r * tmp;
				pos.y += dy;
				ay += day;
				}
			pos.x += dx;
			ax += dax;
			}

		g = g * aaa_fu_float[2] / float(aaa_fu_int[0]*aaa_fu_int[1]) ;

		if( g <  aaa_fu_float[3] )
			tmp = 0.;
		else
			tmp = g - aaa_fu_float[3];
	return tmp;
}

void main()
{
vec4	src;
vec4	dst;
float	s;

	src = texture2D( aaa_tex2d[0], gl_TexCoord[0].st);

	dst.xyz =  src.xyz * aaa_fu_float[0];

	s = doit();

	dst += mix( aaa_fu_vec4[0], aaa_fu_vec4[1], s ) * aaa_fu_float[1];
	gl_FragColor = dst;
}

