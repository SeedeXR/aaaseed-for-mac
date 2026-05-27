/*
 * To compile: cc -o fractals fractals.c -lGL -lGLU -lX11 -lglut -lXmu -lm
 *
 * Usage: fractals
 *
 * Homework 6, Part 2: fractal mountains and fractal trees 
 * (Pretty Late)
 *
 * Draws fractal mountains and trees -- and an island of mountains in water 
 * (I tried having trees on the island but it didn't work too well.)
 *
 * Two viewer modes: polar and flying (both restrained to y>0 for up vector).
 * Keyboard 0->9 and +/- control speed when flying.
 *
 * Only keyboard commands are 0-9 and +/- for speed in flying mode.
 *
 * Fog would make the island look much better, but I couldn't get it to work
 * correctly.  Would line up on -z axis not from eye.
 *
 * Philip Winston - 3/4/95
 * pwinston@hmc.edu
 * http://www.cs.hmc.edu/people/pwinston
 *
 */

#include "gol/gol.h"
#include "gol/gol_draw.h"
#include "gol/gol_matrix.h"
		 
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>           // ULONG_MAX is defined here
#include <float.h>            // FLT_MAX is at least defined here
		 
#include <time.h>  // for random seed
#include "ui/aaa_menu.h"
#include "gol/gol_light.h"
#include "gol/gol_list.h"

enum {
	CHOICE_MOUNTAIN	=	0,
	CHOICE_TREE,
	CHOICE_ISLAND
};

static	INT32	MOUNTAIN		;
static	INT32	MOUNTAIN_MAT	;
static	INT32	TREE			;
static	INT32	ISLAND			;
static	INT32	STEM			;
static	INT32	LEAF			;
static	INT32	WATER_MAT		;
static	INT32	LEAF_MAT		;
static	INT32	TREE_MAT		;
static	INT32	STEMANDLEAVES	;

#define MAXLEVEL 8

int Rebuild	=	1,				//	Rebuild display list in next display ?
	Fract	=	CHOICE_TREE,	//	What fractal are we building
	Level	=	5;				//	levels of recursion for fractals

int DrawAxes = 0;       

/***************************************************************/
/*************************   EB   JUNK *************************/
/***************************************************************/

#ifdef	WIN32
void	srand48(unsigned long seed )
{
	srand(seed );
}

REAL	drand48()
{
	return ( ((REAL)rand())/(REAL)RAND_MAX );
}
#endif

//************************* VECTOR JUNK *************************

// calculates a normalized crossproduct to v1, v2
void ncrossprod(REAL v1[3], REAL v2[3], REAL cp[3] )
{
	cp[0] = v1[1]*v2[2] - v1[2]*v2[1];
	cp[1] = v1[2]*v2[0] - v1[0]*v2[2];
	cp[2] = v1[0]*v2[1] - v1[1]*v2[0];
	normalize_v3r(cp);
}

// calculates normal to the triangle designated by v1, v2, v3
void triagnormal(REAL v1[3], REAL v2[3], REAL v3[3], REAL norm[3] )
{
	REAL vec1[3], vec2[3];

	vec1[0] = v3[0] - v1[0];  vec2[0] = v2[0] - v1[0];
	vec1[1] = v3[1] - v1[1];  vec2[1] = v2[1] - v1[1];
	vec1[2] = v3[2] - v1[2];  vec2[2] = v2[2] - v1[2];

	ncrossprod(vec2, vec1, norm);
}

REAL xzlength(REAL v1[3], REAL v2[3])
{
	return SQRT( (v1[0] - v2[0])*(v1[0] - v2[0]) +
				 (v1[2] - v2[2])*(v1[2] - v2[2]) );
}

REAL xzslope(REAL v1[3], REAL v2[3])
{
	return (v1[0] != v2[0]) ? ((v1[2] - v2[2]) / (v1[0] - v2[0])) : FLT_MAX;
}


//************************ MOUNTAIN STUFF ***********************

REAL DispFactor[MAXLEVEL];  /* Array of what to multiply random number
				  by for a given level to get midpoint
				  displacement  */
REAL DispBias[MAXLEVEL];  /* Array of what to add to random number
				before multiplying it by DispFactor */

#define NUMRANDS 191
float RandTable[NUMRANDS];  /* hash table of random numbers so we can
				   raise the same midpoints by the same amount */ 

		 /* The following are for permitting an edge of a mountain to be   */
		 /* pegged so it won't be displaced up or down.  This makes it    */
		 /* easier to setup scenes and makes a single mountain look better */

REAL Verts[3][3],    /* Vertices of outside edges of mountain */
		Slopes[3];      /* Slopes between these outside edges */
int     Pegged[3];      /* Is this edge pegged or not */           


// Comes up with a new table of random numbers [0,1)
void InitRandTable( UINT32 seed )
{
	srand48( seed );
	for( INT32 i = 0; i < NUMRANDS; ++i )
		RandTable[i] = float(drand48() - 0.5);
}

// calculate midpoint and displace it if required
void Midpoint( REAL mid[3], REAL v1[3], REAL v2[3], int edge, int level )
{
	UINT32 hash;

	mid[0] = (v1[0] + v2[0]) / 2;
	mid[1] = (v1[1] + v2[1]) / 2;
	mid[2] = (v1[2] + v2[2]) / 2;
	if( !Pegged[edge] || (fabs(xzslope(Verts[edge], mid ) 
						- Slopes[edge]) > 0.00001) )
	{
		srand48((int)((v1[0]+v2[0])*23344));
		hash = UINT32(drand48() * 7334334);
		srand48((int)((v2[2]+v1[2])*43433));
		hash = (UINT32 )(drand48() * 634344 + hash) % NUMRANDS;
		mid[1] += ((RandTable[hash] + DispBias[level]) * DispFactor[level]);
	}
}

// Recursive mountain drawing routine -- from lecture with addition of 
//	allowing an edge to be pegged.  This function requires the above
//	globals to be set, as well as the Level global for fractal level
void FMR(REAL v1[3], REAL v2[3], REAL v3[3], int level)
{
	if( level == Level )
	{
		REAL norm[3];

		triagnormal(v1, v2, v3, norm);
		GOL::normal3v(norm);
		GOL::vertex3v(v1);
		GOL::vertex3v(v2);
		GOL::vertex3v(v3);
	}
	else
	{
		REAL m1[3], m2[3], m3[3];

		Midpoint(m1, v1, v2, 0, level);
		Midpoint(m2, v2, v3, 1, level);
		Midpoint(m3, v3, v1, 2, level);

		FMR(v1, m1, m3, level + 1);
		FMR(m1, v2, m2, level + 1);
		FMR(m3, m2, v3, level + 1);
		FMR(m1, m2, m3, level + 1);
	}
}


#ifdef _MSC_VER
#	pragma warning( push )  
#	pragma warning (4 : 4305)	// disable truncation warnings
#	pragma warning (4 : 4838)	// disable conversion requires a narrowing conversion warnings
#endif	//_MSC_VER

// sets up lookup tables and calls recursive mountain function

void FractalMountain(REAL v1[3], REAL v2[3], REAL v3[3], int pegged[3])
{
	REAL lengths[MAXLEVEL];

	REAL fraction[8]	= { 0.3, 0.3, 0.4, 0.2, 0.3, 0.2, 0.4, 0.4  };
	REAL bias[8]		= { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1  };
	REAL avglen			= REAL( ( xzlength(v1, v2) + xzlength(v2, v3) + xzlength(v3, v1) ) / 3. );

	for( INT32 i = 0; i < 3; ++i )
	{
		Verts[0][i] = v1[i];      // set mountain vertex globals
		Verts[1][i] = v2[i];
		Verts[2][i] = v3[i];
		Pegged[i] = pegged[i];
	}

	Slopes[0] = xzslope( Verts[0], Verts[1] );   // set edge slope globals
	Slopes[1] = xzslope( Verts[1], Verts[2] );
	Slopes[2] = xzslope( Verts[2], Verts[0] );

	lengths[0] = avglen;          
	for( INT32 i = 1; i < Level; ++i )
	{   
		lengths[i] = lengths[i - 1] / 2;     // compute edge length for each level
	}

	for( INT32 i = 0; i < Level; ++i )
	{   // DispFactor and DispBias arrays    
		DispFactor[i] = ( lengths[i] * ( ( i <= 7 ) ? fraction[i] : fraction[7] ) );
		DispBias[i]   = ( ( i <= 7 ) ? bias[i] : bias[7] );
	} 

	GOL::begin(GL_TRIANGLES);
	FMR( v1, v2, v3, 0 );    // issues no GL but vertex calls
	GOL::end();
}


// draw a mountain and build the display list

void CreateMountain()
{
	REAL v1[3] = { 0, 0, -1 }, v2[3] = { -1, 0, 1 }, v3[3] = { 1, 0, 1 };
	int pegged[3] = { 1, 1, 1 };

	GOL::compile_list( MOUNTAIN );
		GOL::call_list( MOUNTAIN_MAT );
		FractalMountain( v1, v2, v3, pegged );
	GOL::end_list();
}


// new random numbers to make a different mountain

void NewMountain()
{
	InitRandTable( UINT32(time(nullptr)) );
}


//**************************** TREE ****************************

UINT32 tree_seed;   /* for srand48 - remember so we can build "same tree" at a different level */


// recursive tree drawing thing, fleshed out from class notes pseudocode 
  
#define	ROT_RANGE_Y	40
#define	ROT_RANGE_Z	20
void FractalTree(int level)
{
	UINT32 savedseed;  // need to save seeds while building tree too

	if( level == Level )
	{
		GOL::matrix::push();
			GOL::matrix::rotate_y_deg( drand48()*180);
			GOL::call_list(STEMANDLEAVES);
		GOL::matrix::pop();
	}
	else
		{
		GOL::call_list(STEM);
		GOL::matrix::push();

			GOL::matrix::rotate_y_deg( drand48()*180);
			GOL::matrix::translate(0, 1, 0);
			GOL::matrix::scale( 0.7);

			savedseed = UINT32(drand48()*ULONG_MAX);    // recurse on a 3-way branching
			GOL::matrix::push();
				GOL::matrix::rotate_y_deg( 110 + drand48()*ROT_RANGE_Y);
				GOL::matrix::rotate_z_deg(  30 + drand48()*ROT_RANGE_Z);
				FractalTree( level + 1);
			GOL::matrix::pop();

			srand48(savedseed );
			savedseed = UINT32(drand48()*ULONG_MAX);
			GOL::matrix::push();
				GOL::matrix::rotate_y_deg(-130 + drand48()*ROT_RANGE_Y);
				GOL::matrix::rotate_z_deg(  30 + drand48()*ROT_RANGE_Z);
				FractalTree(level + 1);
			GOL::matrix::pop();

			srand48(savedseed );
			GOL::matrix::push();
				GOL::matrix::rotate_y_deg(-20 + drand48()*ROT_RANGE_Y);
				GOL::matrix::rotate_z_deg( 30 + drand48()*ROT_RANGE_Z);
				FractalTree(level + 1);
			GOL::matrix::pop();

		GOL::matrix::pop();
		}
}

 
// Create display lists for a leaf, a set of leaves, and a stem

void CreateTreeLists()
{
	GLUquadricObj *cylquad = gluNewQuadric();

	GOL::compile_list( STEM );
		GOL::matrix::push();
			GOL::matrix::rotate_x_deg( -90);
			gluCylinder(cylquad, 0.1, 0.08, 1, 6, 1 );
			/*
			GOL::begin(GL_LINES);
				GOL::vertex3( 0, 0, 0);
				GOL::vertex3( 0, 0, 1);
			GOL::end();
			*/
		GOL::matrix::pop();
	GOL::end_list();

	GOL::compile_list( LEAF );  // I think this was jeff allen's leaf idea
		GOL::begin(GL_TRIANGLES);
			GOL::normal3(-0.1, 0, 0.25);  // not normalized
			GOL::vertex3( 0, 0, 0);
			GOL::vertex3( 0.25, 0.25, 0.1);
			GOL::vertex3( 0, 0.5, 0);

			GOL::normal3(0.1, 0, 0.25);
			GOL::vertex3( 0, 0, 0);
			GOL::vertex3( 0, 0.5, 0);
			GOL::vertex3( -0.25, 0.25, 0.1);
		GOL::end();
	GOL::end_list();

	GOL::compile_list( STEMANDLEAVES );
		GOL::matrix::push();
			GOL::call_list(STEM);
			GOL::call_list(LEAF_MAT);
			for( INT32 i = 0; i < 3; ++i )
				{
				GOL::matrix::translate(0, 0.333, 0);
				GOL::matrix::rotate_y_deg( 90);
				GOL::matrix::push();
					//GOL::rotate_y_deg( 0);
					GOL::matrix::rotate_x_deg( 50);
					GOL::call_list(LEAF);
				GOL::matrix::pop();
				GOL::matrix::push();
					GOL::matrix::rotate_y_deg( 180);
					GOL::matrix::rotate_x_deg( 60);
					GOL::call_list(LEAF);
				GOL::matrix::pop();
				}
		GOL::matrix::pop();
	GOL::end_list();
}


// draw and build display list for tree

void CreateTree()
{
	srand48( tree_seed );

	GOL::compile_list( TREE );
		GOL::matrix::push();
			GOL::call_list( TREE_MAT );
			GOL::matrix::translate( 0, -1, 0 );
			FractalTree(0);
		GOL::matrix::pop();
	GOL::end_list();  
}

 /*
  * new seed for a new tree (groan)
  */
void NewTree()
{
	tree_seed = UINT32(time(nullptr));
}


//********************** FRACTAL PLANET ************************

void CreateIsland()
{
	CreateMountain();
	GOL::compile_list( ISLAND );
		GOL::matrix::set_modelview();
		GOL::matrix::push();
			GOL::call_list(WATER_MAT);

			GOL::begin(GL_TRIANGLE_STRIP);
				GOL::normal3v( unit_y_v4fp32);
				GOL::vertex3( 1000, 0.01, 1000);
				GOL::vertex3( 1000, 0.01, -1000);
				GOL::vertex3( -1000, 0.01, 1000);
				GOL::vertex3( -1000, 0.01, -1000);
			GOL::end();

			GOL::matrix::push();
				GOL::matrix::translate(0, -0.1, 0);
				GOL::call_list(MOUNTAIN);
			GOL::matrix::pop();

			GOL::matrix::push();
				GOL::matrix::rotate_y_deg( 135);
				GOL::matrix::translate(0.2, -0.15, -0.4);
				GOL::call_list(MOUNTAIN);
			GOL::matrix::pop();

			GOL::matrix::push();
				GOL::matrix::rotate_y_deg( -60);
				GOL::matrix::translate(0.7, -0.07, 0.5);
				GOL::call_list(MOUNTAIN);
			GOL::matrix::pop();

			GOL::matrix::push();
				GOL::matrix::rotate_y_deg( -175);
				GOL::matrix::translate(-0.7, -0.05, -0.5);
				GOL::call_list(MOUNTAIN);
			GOL::matrix::pop();

			GOL::matrix::push();
				GOL::matrix::rotate_y_deg( 165);
				GOL::matrix::translate(-0.9, -0.12, 0.0);
				GOL::call_list(MOUNTAIN);
			GOL::matrix::pop();

		GOL::matrix::pop();
	GOL::end_list();  
}

void NewFractals()
{
	NewMountain();
	NewTree();
}

void Create( int fract )
{
	switch( fract )
	{
	case CHOICE_MOUNTAIN:	CreateMountain();	break;
	case CHOICE_TREE:		CreateTree();		break;
	case CHOICE_ISLAND:		CreateIsland();		break;
	}
}



//**************************** OPENGL ***************************

void SetupMaterials()
{
	GLfloat mtn_ambuse[]		=	{ 0.426, 0.256, 0.108, 1.0 };
	GLfloat mtn_specular[]		=	{ 0.394, 0.272, 0.167, 1.0 };
	GLfloat mtn_shininess[]		=	{ 10 };

	GLfloat water_ambuse[]		=	{ 0.0, 0.1, 0.5, 1.0 };
	GLfloat water_specular[]	=	{ 0.0, 0.1, 0.5, 1.0 };
	GLfloat water_shininess[]	=	{ 10 };

	GLfloat tree_ambuse[]		=	{ 0.4, 0.25, 0.1, 1.0 };
	GLfloat tree_specular[]		=	{ 0.0, 0.0, 0.0, 1.0 };
	GLfloat tree_shininess[]	=	{ 0 };

	GLfloat leaf_ambuse[]		=	{ 0.0, 0.8, 0.0, 1.0 };
	GLfloat leaf_specular[]		=	{ 0.0, 0.8, 0.0, 1.0 };
	GLfloat leaf_shininess[]	=	{ 10 };

	GOL::compile_list( MOUNTAIN_MAT );
		GOL::materialv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE,			mtn_ambuse		);
		GOL::materialv( GL_FRONT, GL_SPECULAR,						mtn_specular	);
		GOL::materialv( GL_FRONT, GL_SHININESS,						mtn_shininess	);
	GOL::end_list();

	GOL::compile_list( WATER_MAT );
		GOL::materialv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE,			water_ambuse	);
		GOL::materialv( GL_FRONT, GL_SPECULAR,						water_specular	);
		GOL::materialv( GL_FRONT, GL_SHININESS,						water_shininess	);
	GOL::end_list();

	GOL::compile_list( TREE_MAT );
		GOL::materialv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE,			tree_ambuse		);
		GOL::materialv( GL_FRONT, GL_SPECULAR,						tree_specular	);
		GOL::materialv( GL_FRONT, GL_SHININESS,						tree_shininess	);
	GOL::end_list();

	GOL::compile_list( LEAF_MAT );
		GOL::materialv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE,	leaf_ambuse		);
		GOL::materialv( GL_FRONT_AND_BACK, GL_SPECULAR,				leaf_specular	);
		GOL::materialv( GL_FRONT_AND_BACK, GL_SHININESS,			leaf_shininess	);
	GOL::end_list();
}

#ifdef _MSC_VER
#	pragma warning( pop )  
#endif	//_MSC_VER

//************************ GLUT STUFF ***************************

void fracglut_render()
{ 
	if( Rebuild )
	{
		Create( Fract );
		Rebuild = 0;
	}
	INT32 list = 0;
	switch( Fract )
	{
	case CHOICE_MOUNTAIN:	list = MOUNTAIN;	break;
	case CHOICE_TREE:		list = TREE;		break;
	case CHOICE_ISLAND:		list = ISLAND;		break;
	}
	GOL::call_list( Fract );
}


//******************* MENU SETUP & HANDLING *********************

#define	MENU_RAND 200

void MAACALLBACK setlevel(int value)
{
	Level = value-100;
	Rebuild = 1;
}

void MAACALLBACK choosefract(int value)
{
	Fract = value;
	Rebuild = 1;
}

void MAACALLBACK handlemenu(int value)
{
	switch (value)
		{
		case MENU_RAND:
			NewFractals();
			Rebuild = 1;
			break;
		}
}

INT32 fracglut_init_menu()
{
	INT32 submenu1 = menu::create( setlevel );
	menu::add_item( "0", 100 );
	menu::add_item( "1", 101 );
	menu::add_item( "2", 102 );
	menu::add_item( "3", 103 );
	menu::add_item( "4", 104 );
	menu::add_item( "5", 105 );
	menu::add_item( "6", 106 );
	menu::add_item( "7", 107 );
	menu::add_item( "8", 108 );

	INT32 submenu2 = menu::create( choosefract );
	menu::add_item( "Mountain", CHOICE_MOUNTAIN	);
	menu::add_item( "Tree",		CHOICE_TREE		);
	menu::add_item( "Island",	CHOICE_ISLAND	);

	INT32 fracglut_menu = menu::create( handlemenu );
	menu::add_menu_sub( "Level",	submenu1 );
	menu::add_menu_sub( "Fractal",	submenu2 );
	menu::add_item( "New Fractal", MENU_RAND );

	return fracglut_menu;
}


//**************************** MAIN *****************************

void	fracglut_init()
{
	if( MOUNTAIN == 0 )
	{
		MOUNTAIN		=	GOL::gen_lists(	10 );
		if( MOUNTAIN==0 )
			return;

		MOUNTAIN_MAT	=	MOUNTAIN + 1;
		TREE			=	MOUNTAIN + 2;
		ISLAND			=	MOUNTAIN + 3;
		STEM			=	MOUNTAIN + 4;
		LEAF			=	MOUNTAIN + 5;
		WATER_MAT		=	MOUNTAIN + 6;
		LEAF_MAT		=	MOUNTAIN + 7;
		TREE_MAT		=	MOUNTAIN + 8;
		STEMANDLEAVES	=	MOUNTAIN + 9;
	}
	NewFractals();
	SetupMaterials();
	CreateTreeLists();
}


