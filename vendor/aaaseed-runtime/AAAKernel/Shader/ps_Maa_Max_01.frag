
// Uniform variables for texturing
uniform sampler2D	aaa_tex2d[4];


float doit()
{
float	g;
float	tmp;
float	v;

//vec4	c;
int		nb_x = aaa_fu_int[0];
int		nb_y = aaa_fu_int[1];
//	external line contribute for 0 (cos is null)
//	so do it inside
//		so nb+1 interval
//		and nb-1 operation
ivec2	tex_suv = textureSize( aaa_tex2d[0], 0 );
float	dx = aaa_fu_float[4] / tex_suv.x ;
float 	dy = aaa_fu_float[5] / tex_suv.y ;
float	sx	= gl_TexCoord[0].x - dx * .5 ;
float	sy	= gl_TexCoord[0].y - dy * .5 ;
float	dsx = dx * .5;
float	dsy = dy * .5;
		dx = dx / float(nb_x-1) ;
		dy = dy / float(nb_y-1) ;

vec2	pos;
int		i;
int		j;

float	dax = 3.1415926535897932384626433832795 / float(nb_x+1) ;
float	day = 3.1415926535897932384626433832795 / float(nb_y+1) ;
float	ax;
float	ay;

float f = 1. / ( 1. - aaa_fu_float[3]) ;

		pos.x = sx;
		ax = -1.5707963267948966192313216916398 + dax;
		g = 0.;

		if( aaa_fu_int[2] == 0)
		{
		float	cos_x;
			for( i=nb_x; i>0; i--)
			{
				cos_x = cos( ax) - 1.;	//compute factor
				pos.y = sy;
				ay = -1.5707963267948966192313216916398 + day;
				for( j=nb_y; j>0; j--)
				{
					tmp = cos_x + cos( ay);	//compute factor
					if( tmp > 0. )
					{
						v = texture2D( aaa_tex2d[0], pos ).g;
						if( v >  aaa_fu_float[2] )
						{
							v *= tmp;
							if( v > g )
								g = v;
						}
					}
					pos.y += dy;
					ay += day;
				}
				pos.x += dx;
				ax += dax;
			}
		}
		else if( aaa_fu_int[2] == 1)
		{
			for( i=nb_x; i>0; i--)
			{
				pos.y = sy;
				for( j=nb_y; j>0; j--)
				{
					v = texture2D( aaa_tex2d[0], pos ).g;
					if( v >  aaa_fu_float[2] )
					{
						if( v > g )
							g = v;
					}
					pos.y += dy;
				}
				pos.x += dx;
			}
		}
		else if( aaa_fu_int[2] == 2)
		{
			pos.x = sx;
			pos.y = sy;
			for( i=nb_x; i>0; i--)
			{
				v = texture2D( aaa_tex2d[0], pos ).g;
				if( v >  aaa_fu_float[2] )
				{
					if( v > g )
						g = v;
				}
				pos.x += dx;
			}
			pos.x = sx;
			pos.y += 2.*dsy;
			for( i=nb_x; i>0; i--)
			{
				v = texture2D( aaa_tex2d[0], pos ).g;
				if( v >  aaa_fu_float[2] )
				{
					if( v > g )
						g = v;
				}
				pos.x += dx;
			}
			pos.x = sx;
			pos.y = sy;
			for( j=nb_y; j>0; j--)
			{
				v = texture2D( aaa_tex2d[0], pos ).g;
				if( v >  aaa_fu_float[2] )
				{
					if( v > g )
						g = v;
				}
				pos.y += dy;
			}
			pos.x += 2.*dsx;
			pos.y = sy;
			for( j=nb_y; j>0; j--)
			{
				v = texture2D( aaa_tex2d[0], pos ).g;
				if( v >  aaa_fu_float[2] )
				{
					if( v > g )
						g = v;
				}
				pos.y += dy;
			}
		}

	if( g <  aaa_fu_float[3] )
		tmp = 0.;
	else
		tmp = (g - aaa_fu_float[3]) * f;
	return tmp;
}

void main()
{
vec4	src;
vec4	dst;
float	s;

	src = texture2D( aaa_tex2d[0], gl_TexCoord[0].st);


	s = doit();

	dst = mix( aaa_fu_vec4[0], aaa_fu_vec4[1], s ) * aaa_fu_float[1];
	dst.xyz +=  texture2D( aaa_tex2d[0], gl_TexCoord[0].st) * (1.-dst.a);
	dst.a += aaa_fu_float[0];
	gl_FragColor = dst;
}

