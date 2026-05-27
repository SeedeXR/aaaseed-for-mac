
// Uniform variables for texturing
uniform sampler2D	aaa_tex2d[4];


//uniform float	aaa_fu_float[6];
//	0	src
//	1	fx
//	2	nb_pixel detect
//	3	threshold
//	4	influence x
//	5	influence y

//	0	: interpolation 0=linear, 1=bezier
uniform int		aaa_fu_int[6];

void main (void)
{
//int	i;
vec4	cx;
vec4	cy;
vec4	color;
float	alpha;

float NPixels = aaa_fu_float[2];
float Threshold = aaa_fu_float[3] * aaa_fu_float[3];

ivec2	tex_suv = textureSize( aaa_tex2d[0], 0 );
float	dx = NPixels/tex_suv.x;
float 	dy = NPixels/tex_suv.y;

//	sx = g22 +2.0*g21 +g20 -g02 -2.0*g01 -g00;
//	sy = g22 +2.0*g12 +g02 -g20 -2.0*g10 -g00;
 //todoopt use swizzle
	cx =		texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2(	dx,		dy) );
	cx -=		texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2(	-dx,	-dy) );
	cy = cx;

	cx += 2.0 *	texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2(	0.0,	dy) );
	cx +=		texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2(	-dx,	dy) );
	cx -=		texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2(	dx,		-dy) );
	cx -= 2.0 *	texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2(	0.0,	-dy) );

	cy += 2.0 *	texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2(	dx,		0.0) );
	cy +=		texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2(	dx,		-dy) );
	cy -=		texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2(	-dx,	dy) );
	cy -= 2.0 *	texture2D( aaa_tex2d[0], gl_TexCoord[0].xy + vec2(	-dx,	0.0) );


	cx.x = abs(cx.x);
	cx.y = abs(cx.y);
	cx.z = abs(cx.z);

float gx = gray( cx );

	cy.x = abs(cy.x);
	cy.y = abs(cy.y);
	cy.z = abs(cy.z);
float gy = gray( cy );
float dist;

	dist = aaa_fu_float[4]*gx*gx + aaa_fu_float[5]*gy*gy;
//	dist = gy*gy;

	if( aaa_fu_int[0] == 0)
		{
		if( dist > Threshold )
			{
			color = texture2D( aaa_tex2d[0], gl_TexCoord[0].st );
	//		discard; //color = vec4(0.0,0.0,0.0,0.25);
			}
		else
			{
			color = aaa_fu_vec4[0]*aaa_fu_float[0] + texture2D( aaa_tex2d[0], gl_TexCoord[0].st )*aaa_fu_float[1];
	//		if( dist > (Threshold * 0.5) )
	//			alpha = 1.0 - smoothstep( Threshold * 0.5, Threshold, dist );
	//		else
	//			alpha = 1.0;
			}
		}
	else
		{
		if( dist > Threshold )
			{
			color = aaa_fu_vec4[0]*aaa_fu_float[1] + texture2D( aaa_tex2d[0], gl_TexCoord[0].st )*aaa_fu_float[0];
			}
		else
			{
			color = texture2D( aaa_tex2d[0], gl_TexCoord[0].st )*aaa_fu_float[0];
			}
		}
	//	color.rgb = texture2D( aaa_tex2d[0], gl_TexCoord[0].st).rgb;
//	color.a = alpha;
	gl_FragColor = color;
}