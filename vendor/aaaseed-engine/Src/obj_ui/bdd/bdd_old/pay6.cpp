#include "err.h"
#include "math/v_base.h"
#include "math/rand.h"
#include "gol.h"
#include "ui/aaa_menu.h"
#include "draw/seeddraw.h"

CONST	INT32	R	= 0;
CONST	INT32	G	= 1;
CONST	INT32	B	= 2;

//static	REAL col_black[3] = {0.0, 0.0, 0.0};
static	REAL col_light[3] = {1., .8, .8};
static	REAL col_vert[3] = { 0., 1., 0.};
static	REAL col_rouge[3] = { 1., 0., 0.};
static	REAL col_blanc[3] = { 1., 1., 1.};
static	REAL col_mer[3] =  { 0., .4, .55};

static	INT32	ordre = 7;
static	INT32	n_max;
static	INT32	i_max;

struct	pave
{
	REAL	pos[3];
	REAL	nor[3];
	REAL	col[3];
	REAL	z;
};
	
struct	pave*	terre;

FINLINE	struct	pave*	TERRE( INT32 x, INT32 y)
{
	return	terre+x*i_max+y;
}

static	BOOL	b_addition	= FALSE;
static	REAL	h_dim		= .65;		// dimension fractale = 3 - h	
CONST	REAL	TAILLE		= 4.;
CONST	REAL	HAU			= TAILLE;
CONST	REAL	pos_light[XYZ] = {TAILLE/2., TAILLE/2., TAILLE};
CONST	REAL	FACT_LIGHT	= TAILLE;
CONST	REAL	LIGHTVALUE	= FACT_LIGHT*FACT_LIGHT;
CONST	REAL	MY_AMBIENT	= .3;
CONST	REAL	MY_DIFUSE	= 1.;
static	REAL	a_vect[XYZ]	= { TAILLE, 0, 0 };
static	REAL	b_vect[XYZ]	= { 0, TAILLE, 0. };
static	REAL	start_pos[XYZ] = { 0., 0., 0.};
static	REAL	zmin, zmax;

/*opengl
#define	F_LIGHT_FAR	FALSE		
float mat_mer[] = {
	AMBIENT, 0, .4, .55,
	DIFFUSE, .0, .4, .55,
	SPECULAR, .9, .5, .0,
	SHININESS, 34,
	LMNULL,
};

float mat_mon[] = {
	AMBIENT, .1, .85, .7,
	DIFFUSE, .6, .36, .30,
	LMNULL,
};

float mat_ecu[] = {
	AMBIENT, 1, 1, 1,
	DIFFUSE, .8, .8, .8,
	SPECULAR, .9, .5, .0,
	SHININESS, 34,
	LMNULL,
};

static float lm[] = {
	AMBIENT, .2, .2, .2,
	LOCALVIEWER, 1,
	TWOSIDE, 1,
	LMNULL
};

static float atten[] = {
	ATTENUATION, 1, 0,
	ATTENUATION2, 10.,
	LMNULL
};

static float ltr[] = {
	AMBIENT, .5, .6, .6,
	LCOLOR, 1, .8, .7,
#if	F_LIGHT_FAR
	POSITION, TAILLE/2., 10*TAILLE, 5*TAILLE, 1,
#else
	POSITION, TAILLE/2., TAILLE/2., TAILLE, 1,
#endif
	LMNULL
};

static float ltrb[] = {
	AMBIENT, .5, .6, .6,
	LCOLOR, .4, .3, .3,
	POSITION, TAILLE/2., TAILLE/2., TAILLE, 1,
	LMNULL
};

static float ltg[] = {
	LCOLOR, 0, 1, .0,
	POSITION, 0., TAILLE/2., TAILLE, 1,
	LMNULL
};

static float ltb[] = {
	LCOLOR, 0, .0, 1,
	POSITION, TAILLE, TAILLE/2., TAILLE, 1,
	LMNULL
};
*/

//float (*rnd )(void );

static	c_rand	*rand_cur;

struct	wave
{
	REAL	pos[XYZ];
	REAL	hau;
	REAL	lon;
};

static	struct	wave 	wave1	=
	{
	{ -TAILLE*2., -TAILLE*2., 0.},
	TAILLE/400.,
	3.14/(TAILLE/25.)
	};
	
static	struct	wave 	wave2	=
	{
	{ 0., -TAILLE*2., 0.},
	TAILLE/400.,
	3.14/(TAILLE/45.)
	};

static	void	(*the_draw)(INT32);
static	BOOL	b_light_to_do = TRUE;
static	BOOL	b_normale_done;
static	BOOL	b_light_done;
static	BOOL	b_color_done;

static	BOOL	b_psyche = FALSE;
static	BOOL	b_show_cons = FALSE;

//todoqq try to reinsert
void	circ( float x, float y, float r)
{
}

void find_min_max()
{
REAL	z;
//INT32	a,b;
struct pave*	p_pave;

	zmin = 100000.;
	zmax = -100000.;
	for( INT32 a=0; a<i_max; ++a )
		{
		p_pave = TERRE(a,0);
		for( INT32 b=0; b<i_max; ++b )
			{
			z = p_pave->z;
			zmin = MIN(zmin,z);
			zmax = MAX(zmax,z);
			p_pave++;
			}
		}
}

REAL	make_wave( REAL	pos[XYZ] )
{
register	REAL	x,y,c1;
	x = pos[X] - wave1.pos[X];
	y = pos[Y] - wave1.pos[Y];
	c1 = fabs( COS_RAD( SQRT(x*x+y*y) * wave1.lon) );
	x = pos[X] - wave2.pos[X];
	y = pos[Y] - wave2.pos[Y];
	c1 += fabs ( COS_RAD( SQRT(x*x+y*y) * wave2.lon) );
	c1 *=  pos[Z] /10.;
	return( c1 + pos[Z] );
/*	c1 *=  pos[Z];
	return( c1 );
*/
}

void make_color(int f_z)
{
register	struct pave*	p_pave;
register	REAL	z;
//register	INT32	a,b;
register	REAL	c1;

if ( !b_color_done)
	{
#if	AAA_DEBUG()
	DBG_PRINT_STRING("COLOR");
#endif
	find_min_max();
	for( INT32 a=0; a<i_max; ++a )
		{
		p_pave = TERRE(a,0);
		for( INT32 b=0; b<i_max; ++b )
			{
			z = p_pave->z;
			if ( z >= 0. )
				{
				p_pave->pos[Z] = z;
				z /= zmax;
				p_pave->col[R] = .3 + z * .7;
				p_pave->col[G] = .3 + z * .7;
				p_pave->col[B] = z;
				}
			else
				{
				c1 = 1. - z/zmin*.75;
				if (f_z)
					{
					if ( z < zmin/2.)
						z = zmin-z;
					p_pave->pos[Z] = z/(HAU*5);
					p_pave->pos[Z] = make_wave(p_pave->pos);
					}
				p_pave->col[R]= col_mer[R] * c1;
				p_pave->col[G]= col_mer[G] * c1;
				p_pave->col[B]= col_mer[B] * c1;
				}
			p_pave++;
			}
		}
	b_color_done = TRUE;
	b_normale_done = FALSE;
	b_light_done = FALSE;
	}
}

void	make_normal()
{
register	struct pave*	p_pave1;
register	struct pave*	p_pave2;
register	struct pave*	p_pave3;
register	REAL 	xa, ya, za;
register	REAL 	xb, yb, zb;
register	REAL 	xn, yn, zn;
register	REAL 	n;
//INT32	a,b;


if ( !b_normale_done)
	{
#if	AAA_DEBUG()
	DBG_PRINT_STRING("NORMALES");
#endif
	for( INT32 a=i_max-1; a>0; --a )
		{
		p_pave1 = TERRE(a,i_max);
		p_pave3 = TERRE(a+1,i_max-1);
		p_pave2 = TERRE(a-1,i_max-1);
		for( INT32 b=i_max-1; b>0; --b )
			{
			xa = p_pave1->pos[X];
			ya = p_pave1->pos[Y];
			za = p_pave1->pos[Z];
			xb = p_pave2->pos[X] - p_pave3->pos[X];
			yb = p_pave2->pos[Y] - p_pave3->pos[Y];
			zb = p_pave2->pos[Z] - p_pave3->pos[Z];
			p_pave1 -= 2;
			xa -= p_pave1->pos[X];
			ya -= p_pave1->pos[Y];
			za -= p_pave1->pos[Z];
			xn = ya*zb - za*yb;
			yn = za*xb - xa*zb;
			zn = xa*yb - ya*xb;
			n = SQRT( xn*xn + yn*yn + zn*zn);
			++p_pave1;
			p_pave1->nor[X] = xn / n;
			p_pave1->nor[Y] = yn / n;
			p_pave1->nor[Z] = zn / n;
			--p_pave2;
			--p_pave3;
			}
		}
	b_normale_done = TRUE;
	b_light_done = FALSE;
	}
}

void	make_light()
{
register	REAL	x,y,z,n;
//INT32	a,b;
REAL	c1;
struct pave*	p_pave;
BOOL	b_flip = FALSE;

	if ( !b_light_done)
		{
		make_normal();
#if	AAA_DEBUG()
		DBG_PRINT_STRING("LIGHT");
#endif
		for( INT32 a=0; a<i_max; ++a )
			{
			p_pave = TERRE(a,0);
			for( INT32 b=0; b<i_max; ++b )
				{
				x = pos_light[X] - p_pave->pos[X];
				y = pos_light[Y] - p_pave->pos[Y];
				z = pos_light[Z] - p_pave->pos[Z];
				n = SQRT( x*x + y*y+ z*z);
				x /= n;
				y /= n;
				z /= n;
				c1 = x * p_pave->nor[X] + y * p_pave->nor[Y] + z * p_pave->nor[Z];
				if ( c1 > 0.)
						c1 = LIGHTVALUE/n/n * c1 * MY_DIFUSE;
				else
					c1 = 0.;

				x = p_pave->col[R]*col_light[R]*c1 + p_pave->col[R]*MY_AMBIENT;
				p_pave->col[R] = MIN( x, (REAL) 1.0);
				x = p_pave->col[G]*col_light[G]*c1 + p_pave->col[G]*MY_AMBIENT;
				p_pave->col[G] = MIN( x, (REAL) 1.0);
				x = p_pave->col[B]*col_light[B]*c1 + p_pave->col[B]*MY_AMBIENT;
				p_pave->col[B] = MIN( x, (REAL) 1.0);
				if (b_psyche)
					if ( b_flip )
						{
						p_pave->col[R] = 0.;
						p_pave->col[G] = 0.;
						p_pave->col[B] = MIN( p_pave->nor[B]*col_light[B]*1.0, 1.0);
						b_flip = FALSE;
						}
					else 
						{
						p_pave->col[R] = 1.;
						p_pave->col[G] = 1.;
						p_pave->col[B] = MIN( p_pave->nor[B]*col_light[B]*1.0, 1.0);
						b_flip = TRUE;
						}
				p_pave++;
				}
			}
		b_light_done = TRUE;
		}
}


extern	void draw_rand_in_rect(REAL x, REAL y, REAL w, REAL h);

void draw_curves()
{
	if ( draw::b_draw_curve)
		{	//todo these disable are dangerous 
		GOL::set_texture_0D();
		GOL::disable_lighting();
//		GOL::push_matrix();
//		rendering_set_ortho_with_secu_margin( 100, 100);
//		draw_rand_in_rect( 0, 0, 100, 100);
		draw_rand_in_rect( -4, -4, 8, 8);
//		GOL::pop_matrix();
		}
}

void	my_draw()
{
	make_color(1);
	if (b_light_to_do)
		make_light();
	GOL::push_matrix();
		GOL::rotate_x_deg( -90.);
		GOL::translate(-TAILLE/2., -TAILLE/2., 0);
		the_draw(1);
	GOL::pop_matrix();

	draw_curves();
}

void	init_terre()
{
REAL	x,y,z;
REAL 	dx, dy, dz;
//INT32	a,b;
struct pave*	p_pave;

#if	AAA_DEBUG()
	GOOD_PRINT_STRING("init_terre");
#endif
	dx = b_vect[X]/i_max;
	dy = b_vect[Y]/i_max;
	dz = b_vect[Z]/i_max;
	for( INT32 a=0; a<i_max; ++a )
		{
		x = start_pos[X]+ a*a_vect[X]/i_max; 
		y = start_pos[Y]+ a*a_vect[Y]/i_max; 
		z = start_pos[Z]+ a*a_vect[Z]/i_max;
		p_pave = TERRE(a,0);
		for( INT32 b=0; b<i_max; ++b )
			{
			p_pave->pos[X] = x;
			p_pave->pos[Y] = y;
			p_pave->pos[Z] = z;
			p_pave->nor[X] = 0;
			p_pave->nor[Y] = 0;
			p_pave->nor[Z] = 1;
			p_pave->z = z;
			x += dx;
			y += dy;
			z += dz;
			p_pave++;
			}
		}
	b_normale_done = FALSE;
	b_light_done = FALSE;
	b_color_done = FALSE;
}


#define	FACTOR_REP	10
void	make_rnd()
{
//register	INT32	i;
register	INT32	a,b;
register	REAL	f;
	f = TAILLE / n_max / ( FACTOR_REP / 4 );
	for( INT32 i = n_max * n_max * FACTOR_REP; i > 0; --i )
		{
		a = i_max * (rand_cur->get_ufloat());
		if ( a>=0 && a<i_max)
			{
			b = i_max * (rand_cur->get_ufloat());
			if ( b>=0 && b<i_max)
				TERRE(a,b)->z += f;
			}
		}
}

void	make_terre(INT32 min)
{
//register	INT32	a,b;
register	INT32	d;
register	REAL 	h;
register	INT32	dd;
register	REAL	factor;

//int	l;
#if	AAA_DEBUG()
	GOOD_PRINT_STRING("make_terre");
#endif
	d = n_max;
	h = HAU/3;
	factor = POW( .5, .5*h_dim);

	TERRE(0,0)->z		= rand_cur->get_float()*HAU;	
	TERRE(0,n_max)->z	= rand_cur->get_float()*HAU;	
	TERRE(n_max,0)->z	= rand_cur->get_float()*HAU;	
	TERRE(n_max,n_max)->z	= rand_cur->get_float()*HAU;
	
//	printf(" Factor = %f\n", (FLOAT)factor);
	for( INT32 l=ordre; l>min; --l )
		{
		d /= 2;
		dd = 2*d;
		h *= factor + rand_cur->get_float()*(1.-factor)*2.2;
//		printf(" %d delta I -> %f", d, h);
		for ( INT32 a=d; a<=n_max-d; a+=dd )
			{
			for ( INT32 b=d; b<=n_max-d; b+=dd )
				{
				TERRE(a,b)->z =
					( TERRE(a-d,b-d )->z
					+ TERRE(a-d,b+d )->z
					+ TERRE(a+d,b-d )->z
					+ TERRE(a+d,b+d )->z )
					/4.+rand_cur->get_float()*h;
				}
			}
/*opengl		if (b_show_cons)
			{
			GOL::color3v(col_black);
			clear();
			b_color_done = FALSE;
			make_color(1);
			gl_terre_draw(dd );
			sleep(2);
			}
*/		if (b_addition)
			{
			for ( INT32 a=0; a<=n_max; a+=dd )
				for ( INT32 b=0; b<=n_max; b+=dd )
					TERRE(a,b)->z += rand_cur->get_float()*h;
			}
		h *= factor + rand_cur->get_float()*(1.-factor)*2.2 ;
//		printf("\t delta II -> %f\n", (FLOAT)h);
		for ( INT32 a=d; a<=n_max-d; a+=dd )
			{
			TERRE(a,0)->z =
				( TERRE(a+d,0)->z
				+ TERRE(a-d,0)->z
				+ TERRE(a,d )->z )
				/3.+rand_cur->get_float()*h;
			TERRE(a,n_max)->z =
				( TERRE(a+d,n_max)->z
				+ TERRE(a-d,n_max)->z
				+ TERRE(a,n_max-d )->z )
				/3.+rand_cur->get_float()*h;
			TERRE(0,a)->z =
				( TERRE(0,a+d )->z
				+ TERRE(0,a-d )->z
				+ TERRE(d,a)->z )
				/3.+rand_cur->get_float()*h;
			TERRE(n_max,a)->z =
				( TERRE(n_max,a+d )->z
				+ TERRE(n_max,a-d )->z
				+ TERRE(n_max-d,a)->z )
				/3.+rand_cur->get_float()*h;
			}
		for ( INT32 a=d; a<=n_max-d; a+=dd )
			{
			for ( INT32 b=dd; b<=n_max-d; b+=dd )
				{
				TERRE(a,b)->z =
					( TERRE(a,b-d )->z
					+ TERRE(a,b+d )->z
					+ TERRE(a-d,b)->z
					+ TERRE(a+d,b)->z )
					/4.+rand_cur->get_float()*h;
				}
			}
		for ( INT32 a=dd; a<=n_max-d; a+=dd )
			{
			for ( INT32 b=d; b<=n_max-d; b+=dd )
				{
				TERRE(a,b)->z =
					( TERRE(a,b-d )->z
					+ TERRE(a,b+d )->z
					+ TERRE(a-d,b)->z
					+ TERRE(a+d,b)->z )
					/4.+rand_cur->get_float()*h;
				}
			}
/*opengl		if (b_show_cons)
			{
			GOL::color3v(col_black);
			clear();
			b_color_done = FALSE;
			make_color(1);
			gl_terre_draw(dd );
			sleep(10);
			}
*/		if (b_addition)
			{
			for ( INT32 a=0; a<=n_max; a+=dd )
				for ( INT32 b=0; b<=n_max; b+=dd )
					TERRE(a,b)->z += rand_cur->get_float()*h;
			for ( INT32 a=d; a<=n_max-d; a+=dd )
				for ( INT32 b=d; b<=n_max-d; b+=dd )
					TERRE(a,b)->z += rand_cur->get_float()*h;
			}
		}
}

void	moving_terre_draw( INT32 f_clear)
{
register	struct pave*	p_pave0;
register	struct pave*	p_pave1;
register	struct pave*	p_pave2;
//register	INT32	a,b;


	for( INT32 a=f_clear; a<i_max; a+=f_clear)
		{
		p_pave0 = TERRE(a-f_clear,0);
		p_pave1 = TERRE(a,0);
		p_pave2 = TERRE(a-f_clear,f_clear);
		for( INT32 b=f_clear; b<i_max; b+=f_clear)
			{
			GOL::begin( GL_LINE_STRIP);
				GOL::color3v(p_pave2->col);
				GOL::vertex3v(p_pave2->pos);
				GOL::color3v(p_pave0->col);
				GOL::vertex3v(p_pave0->pos);
				GOL::color3v(p_pave1->col);
				GOL::vertex3v(p_pave1->pos);
			GOL::end();
			p_pave0+=f_clear;
			p_pave1+=f_clear;
			p_pave2+=f_clear;
			}
		}
}

//#define	F_MER	1
//#define	F_MON	2
//#define	F_ECU	3

void	gl_line_terre_draw(INT32 f_clear)
{
register	struct pave*	p_pave0;
register	struct pave*	p_pave1;
register	struct pave*	p_pave2;
//register	INT32	a,b;



/*opengl	lmdef(DEFMATERIAL, F_MER, 0, mat_mer);
	lmdef(DEFMATERIAL, F_MON, 0, mat_mon);
	lmdef(DEFMATERIAL, F_ECU, 0, mat_ecu);
	lmdef(DEFLIGHT, 1, 0, ltr);
	lmdef(DEFLIGHT, 4, 0, ltrb);
	lmdef(DEFLIGHT, 2, 0, ltg);
	lmdef(DEFLIGHT, 3, 0, ltb);
	lmdef(DEFLMODEL, 1, 0, lm);
	lmbind(MATERIAL, f_mat = F_MER);
	lmbind(LIGHT0, 1);
	lmbind(LMODEL, 1);
*/

	for( INT32 a=f_clear; a<i_max; a+=f_clear)
		{
		p_pave0 = TERRE(a-f_clear,0);
		p_pave1 = TERRE(a,0);
		p_pave2 = TERRE(a-f_clear,f_clear);
		for( INT32 b=f_clear; b<i_max; b+=f_clear)
			{
/*opengl			if ( p_pave0->pos[Z] > 0. 
				|| p_pave1->pos[Z] > 0.
				|| p_pave2->pos[Z] > 0.)
				{
				if ( p_pave0->pos[Z] < 0. 
					|| p_pave1->pos[Z] < 0.
					|| p_pave2->pos[Z] < 0.)
					{
					if (f_mat != F_ECU )
						lmbind(MATERIAL, f_mat = F_ECU);
					}
				else if (f_mat  != F_MON )
					lmbind(MATERIAL, f_mat = F_MON);
				}
			else  if (f_mat != F_MER)
				lmbind(MATERIAL, f_mat = F_MER);
*/			GOL::begin( GL_LINE_STRIP);
				GOL::color3v(p_pave2->col);
				GOL::normal3v(p_pave2->nor);
				GOL::vertex3v(p_pave2->pos);
				GOL::color3v(p_pave0->col);
				GOL::normal3v(p_pave0->nor);
				GOL::vertex3v(p_pave0->pos);
				GOL::color3v(p_pave1->col);
				GOL::normal3v(p_pave1->nor);
				GOL::vertex3v(p_pave1->pos);
			GOL::end();

			p_pave0+=f_clear;
			p_pave1+=f_clear;
			p_pave2+=f_clear;
			}
		}
//opengl	lmbind(LMODEL, 0);
}

void	gl_terre_draw(INT32 f_clear)
{
register	struct pave*	p_pave0;
register	struct pave*	p_pave1;
register	struct pave*	p_pave2;
//register	INT32	a,b;
/*opengl	register	int	f_mat;

	lmdef(DEFMATERIAL, F_MER, 0, mat_mer);
	lmdef(DEFMATERIAL, F_MON, 0, mat_mon);
	lmdef(DEFMATERIAL, F_ECU, 0, mat_ecu);
	lmdef(DEFLIGHT, 1, 0, ltr);
	lmdef(DEFLIGHT, 4, 0, ltrb);
	lmdef(DEFLIGHT, 2, 0, ltg);
	lmdef(DEFLIGHT, 3, 0, ltb);
	lmdef(DEFLMODEL, 1, 0, lm);
	lmbind(MATERIAL, f_mat = F_MER);
	lmbind(LIGHT0, 1);
	lmbind(LMODEL, 1);
*/

	for( INT32 a=f_clear; a<i_max; a+=f_clear)
		{
		p_pave0 = TERRE(a-f_clear,0);
		p_pave1 = TERRE(a,0);
		p_pave2 = TERRE(a-f_clear,f_clear);
		for( INT32 b=f_clear; b<i_max; b+=f_clear)
			{
/*opengl			if ( p_pave0->pos[Z] > 0. 
				|| p_pave1->pos[Z] > 0.
				|| p_pave2->pos[Z] > 0.)
				{
				if ( p_pave0->pos[Z] < 0. 
					|| p_pave1->pos[Z] < 0.
					|| p_pave2->pos[Z] < 0.)
					{
					if (f_mat != F_ECU )
						lmbind(MATERIAL, f_mat = F_ECU);
					}
				else if (f_mat  != F_MON )
					lmbind(MATERIAL, f_mat = F_MON);
				}
			else  if (f_mat != F_MER)
				lmbind(MATERIAL, f_mat = F_MER);
*/			GOL::begin(GL_TRIANGLES);
				GOL::color3v(p_pave2->col);
				GOL::normal3v(p_pave2->nor);
				GOL::vertex3v(p_pave2->pos);
				GOL::color3v(p_pave0->col);
				GOL::normal3v(p_pave0->nor);
				GOL::vertex3v(p_pave0->pos);
				GOL::color3v(p_pave1->col);
				GOL::normal3v(p_pave1->nor);
				GOL::vertex3v(p_pave1->pos);
			GOL::end();
			p_pave0 += i_max*f_clear+f_clear;
/*opengl			if ( p_pave0->pos[Z] > 0. 
				|| p_pave1->pos[Z] > 0.
				|| p_pave2->pos[Z] > 0.)
				{
				if ( p_pave0->pos[Z] < 0. 
					|| p_pave1->pos[Z] < 0.
					|| p_pave2->pos[Z] < 0.)
					{
					if (f_mat != F_ECU )
						lmbind(MATERIAL, f_mat = F_ECU);
					}
				else if (f_mat  != F_MON )
					lmbind(MATERIAL, f_mat = F_MON);
				}
			else  if (f_mat != F_MER)
				lmbind(MATERIAL, f_mat = F_MER);
*/			GOL::begin(GL_TRIANGLES);
				GOL::color3v(p_pave0->col);
				GOL::normal3v(p_pave0->nor);
				GOL::vertex3v(p_pave0->pos);
				GOL::color3v(p_pave2->col);
				GOL::normal3v(p_pave2->nor);
				GOL::vertex3v(p_pave2->pos);
				GOL::color3v(p_pave1->col);
				GOL::normal3v(p_pave1->nor);
				GOL::vertex3v(p_pave1->pos);
			GOL::end();
			p_pave0 -= i_max* f_clear;
			p_pave1+=f_clear;
			p_pave2+=f_clear;
			}
		}
//opengl	lmbind(LMODEL, 0);
}


REAL make_rayon(
	struct pave*	p1,
	struct pave*	p2,
	struct pave*	p3,
	struct pave*	p4,
	REAL			xa,
	REAL			ya
	)
{
register	REAL	ra,rb;
register	REAL	a,b,c;
//printf ("%f %f\n", p1->pos[X], (FLOAT)p1->pos[Y]);
//printf ("%f %f\n", p3->pos[X], (FLOAT)p3->pos[Y]);
//printf ("%f %f\n", p2->pos[X], (FLOAT)p2->pos[Y]);
//printf ("%f %f\n", p4->pos[X], (FLOAT)p4->pos[Y]);

	b = p3->pos[X] - p1->pos[X];
	a = p1->pos[Y] - p3->pos[Y];
	c = p1->pos[X]*p3->pos[Y] - p3->pos[X]*p1->pos[Y];
	ra = fabs( (a*xa +b*ya +c)/SQRT(a*a+b*b) );
//	printf ("%f ", (FLOAT)ra);

	b = p2->pos[X] - p3->pos[X];
	a = p3->pos[Y] - p2->pos[Y];
	c = p3->pos[X]*p2->pos[Y] - p2->pos[X]*p3->pos[Y];
	rb = fabs( (a*xa +b*ya +c)/SQRT(a*a+b*b) );
//	printf ("%f ", (FLOAT)rb);
	ra = MIN(ra,rb);

	b = p4->pos[X] - p2->pos[X];
	a = p2->pos[Y] - p4->pos[Y];
	c = p2->pos[X]*p4->pos[Y] - p4->pos[X]*p2->pos[Y];
	rb = fabs( (a*xa +b*ya +c)/SQRT(a*a+b*b) );
/*printf ("%f ", (FLOAT)rb);
*/	ra = MIN(ra,rb);

	b = p1->pos[X] - p4->pos[X];
	a = p4->pos[Y] - p1->pos[Y];
	c = p4->pos[X]*p1->pos[Y] - p1->pos[X]*p4->pos[Y];
	rb = fabs( (a*xa +b*ya +c)/SQRT(a*a+b*b) );
/*printf ("%f ", (FLOAT)rb);
*/	ra = MIN(ra,rb);
/*printf ("---- >%f\n", (FLOAT)ra);
sleep(2);
*/
	return( ra );
}

/*
float make_rayon( p1, p2, p3, p4)
struct	pave	*p1,*p2,*p3,*p4;
{
register	float	x,y,ra,rb;
	x = p1->pos[X] - p2->pos[X];
	y = p1->pos[Y] - p2->pos[Y];
	ra = x*x+y*y;
	x = p3->pos[X] - p4->pos[X];
	y = p3->pos[Y] - p4->pos[Y];
	rb = x*x+y*y;
	ra = MIN(ra,rb);
	x = p1->pos[X] - p3->pos[X];
	y = p1->pos[Y] - p3->pos[Y];
	rb = x*x+y*y;
	ra = MIN(ra,rb);
	x = p3->pos[X] - p2->pos[X];
	y = p3->pos[Y] - p2->pos[Y];
	rb = x*x+y*y;
	ra = MIN(ra,rb);
	x = p2->pos[X] - p4->pos[X];
	y = p2->pos[Y] - p4->pos[Y];
	rb = x*x+y*y;
	ra = MIN(ra,rb);
	x = p4->pos[X] - p1->pos[X];
	y = p4->pos[Y] - p1->pos[Y];
	rb = x*x+y*y;
	ra = MIN(ra,rb);
	return( SQRT(ra) /2. );
}
*/

void	terre_move_xy( INT32 min)
{
register	struct pave	*p1, *p2, *p3, *p4;
//register	INT32	a,b;
register	REAL	xa,ya;
register	INT32	d;
register	INT32	dd;
//float x,y,d1,d2,d3,d4,d5;
//float h,h1;
//float	xb,yb;
REAL	hc, r, angle;
//int	l;

	d = n_max/2;
	hc = .5;

	for( INT32 l=ordre; l>min; --l )
		{
		dd = 2*d;
/*opengl		if (b_show_cons)
			{
			GOL::color3v(col_black);
			clear();
			pushmatrix();
			gl_line_terre_draw(dd );
			}
*/		for ( INT32 a=d; a<i_max; a+=dd )
			{
			for ( INT32 b=d; b<i_max; b+=dd )
				{
				p1 = TERRE(a-d,b-d );
				p2 = TERRE(a+d,b+d );
				p3 = TERRE(a-d,b+d );
				p4 = TERRE(a+d,b-d );
/*
	x = p1->pos[X] - p3->pos[X];
	y = p1->pos[Y] - p3->pos[Y];
	d1 = SQRT( x*x+y*y);
	x = p3->pos[X] - p2->pos[X];
	y = p3->pos[Y] - p2->pos[Y];
	d3 = SQRT( x*x+y*y);
	x = p2->pos[X] - p4->pos[X];
	y = p2->pos[Y] - p4->pos[Y];
	d2 = SQRT( x*x+y*y);
	x = p4->pos[X] - p1->pos[X];
	y = p4->pos[Y] - p1->pos[Y];
	d4 = SQRT( x*x+y*y);
	d5 =  d1;
	d1 = (d1+d4)/2.;
	d4 = (d4+d2)/2.;
	d2 = (d2+d3)/2.;
	d3 = (d3+d5)/2.;
*/
				xa = ( p1->pos[X] + p2->pos[X]
					+ p3->pos[X] + p4->pos[X] )
					/4.;
				ya = ( p1->pos[Y] + p2->pos[Y]
					+ p3->pos[Y] + p4->pos[Y] )
					/4.;
				r = make_rayon( p1, p2, p3, p4, xa, ya);
				if (b_show_cons)
					{
					GOL::color3v(col_vert);
					GOL::begin( GL_LINES);
					  GOL::vertex3v(p1->pos);
					  GOL::vertex3v(p2->pos);
					  GOL::vertex3v(p3->pos);
					  GOL::vertex3v(p4->pos);
					GOL::end();
					circ( xa, ya,.001);
					circ( xa, ya, hc*r);
					}

				r = rand_cur->get_float()*hc*r;
				angle = rand_cur->get_float()*3.1416;
				xa +=  r * COS_RAD( angle);
				ya += r * SIN_RAD( angle);
				TERRE(a,b)->pos[X] = xa;
				TERRE(a,b)->pos[Y] = ya;
				if (b_show_cons)
					{
					GOL::color3v(col_rouge);
					circ(xa, ya, .1);
					}
				}
			}
		for ( INT32 a=d; a<n_max; a+=dd )
			{
			for ( INT32 b=dd; b<n_max; b+=dd )
				{
				p1 = TERRE(a,b-d );
				p2 = TERRE(a,b+d );
				p3 = TERRE(a-d,b);
				p4 = TERRE(a+d,b);
				xa = ( p3->pos[X] + p4->pos[X] ) /2.;
				ya = ( p3->pos[Y] + p4->pos[Y] ) /2.;
				r = make_rayon( p1, p2, p3, p4, xa, ya);
				if (b_show_cons)
					{
					GOL::color3v(col_mer);
					GOL::begin( GL_LINES);
					  GOL::vertex3v(p3->pos);
					  GOL::vertex3v(p4->pos);
					GOL::end();
					circ( xa, ya,.001);
					circ( xa, ya, hc*r);
					}
/*				if (r < .1)
					{
					printf("r = %f\n", (FLOAT)r);
					printf("\tx = %f,  y=%f\n",p1->pos[X], (FLOAT)p1->pos[Y]);
					printf("\tx = %f,  y=%f\n",p2->pos[X], (FLOAT)p2->pos[Y]);
					printf("\tx = %f,  y=%f\n",p3->pos[X], (FLOAT)p3->pos[Y]);
					printf("\tx = %f,  y=%f\n",p4->pos[X], (FLOAT)p4->pos[Y]);
					}
*/				r = rand_cur->get_float()*hc*r;
				angle = rand_cur->get_float()*3.1416;
				xa +=  r * COS_RAD( angle);
				ya += r * SIN_RAD( angle);
				TERRE(a,b)->pos[X] = xa;
				TERRE(a,b)->pos[Y] = ya;
				if (b_show_cons)
					{
					GOL::color3v(col_rouge);
					circ(xa, ya, .1);
					}
				}
			}
		for ( INT32 a=dd; a<n_max; a+=dd )
			{
			for ( INT32 b=d; b<n_max; b+=dd )
				{
				p1 = TERRE(a,b-d );
				p2 = TERRE(a,b+d );
				p3 = TERRE(a-d,b);
				p4 = TERRE(a+d,b);
				xa = ( p1->pos[X] + p2->pos[X] ) /2.;
				ya = ( p1->pos[Y] + p2->pos[Y] ) /2.;
				r = make_rayon( p1, p2, p3, p4, xa, ya);
				if (b_show_cons)
					{
					GOL::color3v(col_blanc);
					GOL::begin( GL_LINES);
					  GOL::vertex3v(p1->pos);
					  GOL::vertex3v(p2->pos);
					GOL::end();
					circ( xa, ya,.001);
					circ( xa, ya, hc*r);
					}
				r = rand_cur->get_float()*hc*r;
				angle = rand_cur->get_float()*3.1416;
				xa +=  r * COS_RAD( angle);
				ya += r * SIN_RAD( angle);
				TERRE(a,b)->pos[X] = xa;
				TERRE(a,b)->pos[Y] = ya;
				if (b_show_cons)
					{
					GOL::color3v(col_rouge);
					circ(xa, ya, .1);
					}
				}
			}
/*opengl		if (b_show_cons)
			{
			popmatrix();
			sleep(2);
			}
*/		d /= 2;
		hc *= 1.;
		}
/*opengl	if (b_show_cons)
		{
		GOL::color3v(col_black);
		clear();
		pushmatrix();
		gl_line_terre_draw(1);
		}
*/	hc = .6;
	d=1;
	dd=2;
		for ( INT32 a=d; a<n_max; a+=dd )
			{
			for ( INT32 b=dd; b<n_max; b+=dd )
				{
				p1 = TERRE(a,b-d );
				p2 = TERRE(a,b+d );
				p3 = TERRE(a-d,b);
				p4 = TERRE(a+d,b);
				TERRE(a,b)->pos[X] = ( p1->pos[X] + p2->pos[X]
						+ p3->pos[X] + p4->pos[X] )
						/4.;
				TERRE(a,b)->pos[Y] = ( p1->pos[Y] + p2->pos[Y]
						+ p3->pos[Y] + p4->pos[Y] )
						/4.;
				}
			}
		for ( INT32 a=dd; a<n_max; a+=dd )
			{
			for ( INT32 b=d; b<n_max; b+=dd )
				{
				p1 = TERRE(a,b-d );
				p2 = TERRE(a,b+d );
				p3 = TERRE(a-d,b);
				p4 = TERRE(a+d,b);
				TERRE(a,b)->pos[X] = ( p1->pos[X] + p2->pos[X]
						+ p3->pos[X] + p4->pos[X] )
						/4.;
				TERRE(a,b)->pos[Y] = ( p1->pos[Y] + p2->pos[Y]
						+ p3->pos[Y] + p4->pos[Y] )
						/4.;
				}
			}
/*opengl	if (b_show_cons)
		{
		popmatrix();
		sleep(2);
		}
*/	b_normale_done = FALSE;
}


void	terre_draw(INT32 f_clear)
{
register	struct pave*	p_pave0;
register	struct pave*	p_pave1;
//register	INT32	a,b;


	for( INT32 a=1; a<i_max; ++a )
		{
		p_pave0 = TERRE(a-1,0);
		p_pave1 = TERRE(a,0);
		GOL::begin(GL_TRIANGLE_STRIP);
			GOL::color3v(p_pave0->col);
			GOL::vertex3v(p_pave0->pos);
			GOL::color3v(p_pave1->col);
			GOL::vertex3v(p_pave1->pos);
			for( INT32 b=1; b<i_max; ++b )
				{
				p_pave0++;
				p_pave1++;
				GOL::color3v(p_pave0->col);
				GOL::vertex3v(p_pave0->pos);
				GOL::color3v(p_pave1->col);
				GOL::vertex3v(p_pave1->pos);
				}
		GOL::end();
		}
}

void	terre_deinit()
{
	IF_FREE_AND_NULL( terre );
}

void	terre_initialize()
{
	n_max = POW2(ordre);
	i_max = n_max + 1;
	terre = (struct pave *) MALLOC( sizeof(struct pave) * (i_max * i_max + i_max) );

	if ( terre == NULL )
	{
		ERR_PRINT_STRING( "terre_initialize() can't malloc");
		exit(-1);
	}
		
//	menu_rnd = defpup("Aleatoire%x900|Gauss%x910|Constant%x920|Alterne%x930");
//	menu_rendu = defpup("Simple%x801|Perso%x802|Silicon%x803|Silicon Lines%x804");
//	menu_met = defpup("MidPoint%x610|Avec addition%x620");
//	menu_dim = defpup("2%x500|2.05%x505|2.1%x510|2.15%x515|2.2%x520|2.25%x525|2.3%x530|2.35%x535|2.4%x540");

//	menu = defpup("Paysage%t|Nouveau|Perturber|Voir rnd%l|Point de vue%m|Rendu%m|Psyche%x1100%l|Rnd%m|Methode%m|Dimension%m%l|Visualiation perturbation%x1200%l|Sauver%x1300|Quitter%x1000",
//		 menu_pov, menu_rendu, menu_rnd, menu_met, menu_dim);

//	rnd = rnd_gauss;
	// todofranz dealloc
	rand_cur = new c_rand_gauss;
	the_draw = terre_draw;
}


static	BOOL	b_first = TRUE;
void	terre_reseed()
{
	if( b_first)
	{
		terre_initialize();
		b_first = FALSE;
	}
	rand_cur->set_seed();
}
static	BOOL	b_show_rnd = FALSE;
void	terre_new()
{
	init_terre();
	if ( b_show_rnd )
		make_rnd();
	else
		make_terre(0);
}


void MAACALLBACK	menu_paysage(INT32 menuval)
{
	switch(menuval)
		{
		case 1:
			terre_new();
			break;
		case 2:
			terre_move_xy(0);
			if ( b_light_to_do)
				b_color_done = FALSE;
			b_light_done = FALSE;
			break;
		case 3:
			b_show_rnd = !b_show_rnd;
			terre_new();
			break;
		case 500:
		case 505:
		case 510:
		case 515:
		case 520:
		case 525:
		case 530:
		case 535:
		case 540:
			h_dim = 3. - (((float)(menuval-500)/100.) + 2.);
			VERBOSE_PRINTF( " h -> %f", (FLOAT)h_dim );
			srand(1);
			terre_new();
			break;
		case 610:
		case 620:
			b_addition = (menuval== 620);
			terre_new();
			break;
		case 801:
			if ( b_light_to_do)
				{
				b_color_done = FALSE;
				b_light_to_do = FALSE;
				the_draw = terre_draw;
				}
			break;
/*		case 802:
			if ( !b_light_to_do)
				{
				b_color_done = TRUE;
				b_light_to_do = TRUE;
				}
			break;
		case 803:
			if ( !b_light_to_do)
				{
				b_color_done = TRUE;
				b_light_to_do = TRUE;
				}
			break;
		case 804:
			if ( !b_light_to_do)
				{
				b_color_done = TRUE;
				b_light_to_do = TRUE;
				}
			break;
*/
		case 900:
			delete rand_cur;
			rand_cur = new c_rand_lin;
			terre_new();
			break;
		case 910:
			delete rand_cur;
			rand_cur = new c_rand_gauss_slick;
//clean
//			((c_rand_gauss_slick*)rand_cur)->set_focus();
			terre_new();
			break;
		case 911:
			delete rand_cur;
			rand_cur = new c_rand_gauss;
			terre_new();
			break;
		case 920:
			delete rand_cur;
			rand_cur = new c_rand_max;
			terre_new();
			break;
		case 930:
			delete rand_cur;
			rand_cur = new c_rand_flip;
			terre_new();
			break;
		case 940:
			delete rand_cur;
			rand_cur = new c_rand_exp;
//clean
//			((c_rand_exp*)rand_cur)->set_focus();
			terre_new();
			break;
		case 1100:
			b_psyche = !b_psyche;
			b_color_done = FALSE;
			break;
		case 1200:
			b_show_cons = !b_show_cons;
			break;
		}
}

int	menu_paysage_build()
{
INT32 sub_paysage;
INT32 sub_rnd;
INT32 sub_dim;
INT32 sub_method;

	sub_dim = menu::create( menu_paysage);
	menu::add_item( "2.0", 500);
	menu::add_item( "2.05", 505);
	menu::add_item( "2.10", 510);
	menu::add_item( "2.15", 515);
	menu::add_item( "2.20", 520);
	menu::add_item( "2.25", 525);
	menu::add_item( "2.30", 530);
	menu::add_item( "2.35", 535);
	menu::add_item( "2.40", 540);

	sub_method = menu::create(menu_paysage);
	menu::add_item( "Sans addition", 610);
	menu::add_item( "Avec addition", 620);

	sub_rnd = menu::create(menu_paysage);
	menu::add_item( "Toggle Show random", 3);
	menu::add_item( "Linear", 900);
	menu::add_item( "Gauss Slick", 910);
	menu::add_item( "Gauss", 911);
	menu::add_item( "Constant", 920);
	menu::add_item( "Flip", 930);
	menu::add_item( "Exp", 940);

	sub_paysage = menu::create(menu_paysage);
	menu::add_item( "New", 1);
	menu::add_menu_sub( "Random", sub_rnd );
	menu::add_menu_sub( "Dim", sub_dim);
	menu::add_menu_sub( "Method", sub_method );
	menu::add_item( "Psyche", 1100);
	menu::add_item( "Show construction", 1200);

	return sub_paysage;
 }

void	terre_main()
{
	terre_initialize();
	terre_new();
}

