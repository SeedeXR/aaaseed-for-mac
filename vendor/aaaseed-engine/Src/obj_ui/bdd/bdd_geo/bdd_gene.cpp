#include "aaa_def.h"
		 
#include "bdd_gene.h"
#include "draw/render.h"
#include "draw/model.h"
		 
#include "draw/fracglut.h"
#include "draw/shape.h"
#include "draw/box.h"
#include "draw/aaa_glut.h"
#include "draw/dust.h"
#include "obj_ui/bdd/bdd_geo/bdd_quak.h"
#include "draw/bspfile.h"
#include "draw/mat.h"
#include "infrastructure/layer/layer.h"
#include "gol/gol_matrix.h"
#include "file/aaa_dir.h"


FACTORY_CREATE_PROP_V1( c_bdd_gene, bdd_gene, Primitive Generic, bdd_gene, sub_menu="Primitive Base"; );

enum GENE_TYPE : INT32
{
	BDD_GENE_DUST = 0,
	BDD_GENE_FACES,
	BDD_GENE_FRACTAL,
	BDD_GENE_QUAKE,
	BDD_GENE_TEAPOT,
	BDD_GENE_LIGHT_BOX,
	BDD_GENE_BOX,
	BDD_GENE_CUBE,
	BDD_GENE_TETRAHEDRON,
	BDD_GENE_OCTAHEDRON,
	BDD_GENE_ICOSAHEDRON,
	BDD_GENE_DODECAHEDRON,
	BDD_GENE_NB
};

static C_PCHAR_C	bdd_gene_str[BDD_GENE_NB] =
{
	"Dust",
	"Faces",
	"Fractal",
	"Quake",
	"Teapot",
	"Light Box",
	"Box",
	"Cube",
	"Tetrahedron",
	"Octahedron",
	"Icosahedron",
	"Dodecahedron",
};


C_PCHAR_C	c_bdd_gene::get_sub_name( INT32 index_sub )
{
	if( index_sub<0 || BDD_GENE_NB<=index_sub )
		return nullptr;
	return bdd_gene_str[index_sub];
}

namespace n_bdd_gene
{
	CONSTEXPR INT32 BASE_PARAM_NB	=	3 + c_bdd::GEO_PARAM_NB;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_BASE_PARAMS
		PARAM_DEF_POINT_XYZ( center )
	};
}

void	c_bdd_gene::param_init_pt()
{
	INT32	h = param_init_pt_geo();
	param_set_pt_3( h, _center);

	err_param_init_pt(h);
}

void c_bdd_gene::init()
{
	if( is_obj_first() )
		fracglut_init();
	_b_bsp_loaded = false;
}


void c_bdd_gene::alloc()
{
}

void c_bdd_gene::dealloc()
{
}

CONSTRUCTOR_CREATE(c_bdd_gene)
{
	//todo perhaps iy is time to split bdd_gene in several bdd ????
	param_init_with( n_bdd_gene::param, n_bdd_gene::PARAM_NB_MAX ); //bdd_gene_param, BDD_GENE_PARAM_NB_MAX);
	init();
	alloc();
}

c_bdd_gene::~c_bdd_gene(){
	dealloc();
}

void c_bdd_gene::build()
{
}

void	c_bdd_gene::update()
{
	_s_bdd = c_layer::get_cur()->get_bdd_selector() - 1;	//	-1 because of bdd External
	c_model::cur->get_size_xyz_v3( _size );

	switch( _s_bdd )
	{
	case BDD_GENE_QUAKE:
		if( !_b_bsp_loaded )
		{
			static INT32	rval;
			c_dir::change_to_def();
			//rval = openBSP("Quake/Thisiste.BSP");
//demo
			rval = openBSP("../../Quake/e1m1.BSP");
			if( rval == FAILURE )
			{
				box_err( "Error opening bsp file." );
			}
			else if( bdd_quak_load_level_data() )
			{
				box_err( "Error loading level." );
			}
			closeBSP();
			_b_bsp_loaded = true;
		}
		break;
	}
}

bool	c_bdd_gene::can_implicit()
{
	bool	result;
	switch(_s_bdd)
	{
	case BDD_GENE_FRACTAL:
	case BDD_GENE_LIGHT_BOX:
//	case BDD_GENE_TEAPOT:
	case BDD_GENE_DUST:
//	case BDD_GENE_SIGNATURE:
	case BDD_GENE_DODECAHEDRON:
	case BDD_GENE_OCTAHEDRON:
	case BDD_GENE_TETRAHEDRON:
	case BDD_GENE_ICOSAHEDRON:
		result = false;
		break;
	default:
		result = true;
	}
	return result;
}

void c_bdd_gene::draw()
{
	INT32	axe = c_model::cur->get_axe();
	REAL	resolution = c_model::cur->get_resolution();
	REAL	factor;

	switch( _s_bdd )
	{
	case BDD_GENE_FRACTAL:
		GOL::matrix::push();
			GOL::matrix::translate3v( _center );
			GOL::matrix::scale( _size[0] );
			fracglut_render();
		GOL::matrix::pop();
		break;
	case BDD_GENE_LIGHT_BOX:
		GOL::matrix::push();
			GOL::matrix::translate3v( _center);
			GOL::matrix::scale( _size[0], _size[1], _size[2] );
			draw_light_test();
		GOL::matrix::pop();
		break;
	case BDD_GENE_CUBE:
		draw_cube_at( _size[0], _center );
		break;
	case BDD_GENE_BOX:
		{
			REAL loc_size[3];
			uv_to_xyz_v3r( loc_size, _size, axe );
			draw_box_at( loc_size, _center );
		}
		break;
	case BDD_GENE_DODECAHEDRON:
		GOL::matrix::push();
			GOL::matrix::translate3v( _center);
			factor = REAL(.31);
			GOL::matrix::scale( _size[0] * factor, _size[1] * factor, _size[2] * factor );
			draw_dodecahedron( GL_TRIANGLE_FAN ); // To improve -> simple copy/paste from glut
		GOL::matrix::pop();
		break;
	case BDD_GENE_OCTAHEDRON:
		GOL::matrix::push();
			GOL::matrix::translate3v( _center);
			factor = .5; 
			GOL::matrix::scale( _size[0] * factor, _size[1] * factor, _size[2] * factor );
			draw_octahedron( GL_TRIANGLES ); // To improve -> simple copy/paste from glut
		GOL::matrix::pop();
		break;
	case BDD_GENE_TETRAHEDRON:
		GOL::matrix::push();
			GOL::matrix::translate3v( _center);
			//	factor = .865; 
			//	GOL::scale( _size[0] * factor, _size[1] * factor, _size[2] * factor );
			GOL::matrix::scale( _size[0], _size[1], _size[2] );
			draw_tetrahedron( GL_TRIANGLES );
		GOL::matrix::pop();
		break;
	case BDD_GENE_ICOSAHEDRON:
		GOL::matrix::push();
			factor = REAL(.59); 
			GOL::matrix::scale( _size[0] * factor, _size[1] * factor, _size[2] * factor );
			draw_icosahedron( GL_TRIANGLES ); // To improve -> simple copy/paste from glut
		GOL::matrix::pop();
		break;
	case BDD_GENE_TEAPOT:
		//todo pass an axis
		draw_teapot( _center, _size, INT32(resolution*REAL(10)) );
		break;
	case BDD_GENE_DUST:
		GOL::matrix::push();
			GOL::matrix::scale( _size[0] );
			//todo make Bdd		dust_draw( img_cur, resolution*10., 2.);
			dust_draw( nullptr, INT32(resolution*REAL(10)), REAL(2) );
		GOL::matrix::pop();
		break;
	case BDD_GENE_QUAKE:
		GOL::matrix::push();
			GOL::matrix::translate3v( _center );
			GOL::matrix::scale( _size[0] );
			render_bsp();
		GOL::matrix::pop();
		break;
	case BDD_GENE_FACES:
		//todo make Bdd		faces_draw( _size, img_cur, resolution*10.);
		faces_draw( _size[0], nullptr, INT32(resolution*REAL(10)) );
		break;
	}
}

bool	c_bdd_gene::compute_intersection( REAL CONST* origin, REAL CONST* point, REAL* intersection )
{
	bool	retcode = true;
	INT32	axe = c_model::cur->get_axe();

	switch( _s_bdd )
	{
	case BDD_GENE_BOX:
		{	//this is not optimum but functional
			REAL	tmp[3];
			REAL	p;
			REAL	t;
			REAL	min[3];
			REAL	max[3];
			//INT32	i;

			REAL	inter[3][3];
		
			//compute min/max
			uv_to_xyz_v3r( tmp, _size, axe );
			for( INT32 i = 0; i <= 2; ++i )
			{
				t = ABS( tmp[i]*REAL(.5) );
				min[i] = _center[i] - t;
				max[i] = _center[i] + t;
			}

			//move the origin of the world to the first point
			sub_v3( min, origin );
			sub_v3( max, origin );

			REAL lpoint[3];
			sub_v3( lpoint, point, origin );

			//intersection with the 3 planes
			for( INT32 i = 0; i <= 2; ++i )
			{
				p = lpoint[i];
				if ( p > 0 )
					t = max[i];
				else
					t = min[i]; 
				if(	tmp != 0 )	//hack this buggy
				{
					inter[i][0] = (lpoint[0]*t) / p;
					inter[i][1] = (lpoint[1]*t) / p;
					inter[i][2] = (lpoint[2]*t) / p;
				}
				else
				{
					inter[i][0] = REAL_BIG_VALUE;
					inter[i][1] = REAL_BIG_VALUE;
					inter[i][2] = REAL_BIG_VALUE;
				}
			}
		
			INT32	i;
			//find the closest one
			if( ABS(inter[0][0]) <  ABS(inter[1][0]) )
				if( ABS(inter[0][0]) <  ABS(inter[2][0]) )
					i = 0;
				else
					i = 2;
			else
				if( ABS(inter[1][0]) <  ABS(inter[2][0]) )
					i = 1;
				else
					i = 2;

			//move back to origin
			add_v3( intersection, inter[i], origin );
		}
		break;
	default:
		retcode = false;
		break;
	}
// todoqqq retcode is not initialized by all path!!!
	return retcode;
} 



// Begin : UGLY from GLUT //

namespace
{
	REAL dodec[20][3];

	void init_dodecahedron(void)
	{
		GLfloat alpha, beta;
		alpha = sqrtf( REAL( 2. / (3. + sqrt(5.0)) ) );
		beta = FP32(1) + sqrtf( REAL( 6.0 / (3.0 + sqrt(5.0)) - 2.0 + 2.0 * sqrt(2.0 / (3.0 + sqrt(5.0))) ) );
	
		dodec[0][0] = -alpha; dodec[0][1] = 0; dodec[0][2] = beta;
		dodec[1][0] = alpha; dodec[1][1] = 0; dodec[1][2] = beta;
		dodec[2][0] = -1; dodec[2][1] = -1; dodec[2][2] = -1;
		dodec[3][0] = -1; dodec[3][1] = -1; dodec[3][2] = 1;
		dodec[4][0] = -1; dodec[4][1] = 1; dodec[4][2] = -1;
		dodec[5][0] = -1; dodec[5][1] = 1; dodec[5][2] = 1;
		dodec[6][0] = 1; dodec[6][1] = -1; dodec[6][2] = -1;
		dodec[7][0] = 1; dodec[7][1] = -1; dodec[7][2] = 1;
		dodec[8][0] = 1; dodec[8][1] = 1; dodec[8][2] = -1;
		dodec[9][0] = 1; dodec[9][1] = 1; dodec[9][2] = 1;
		dodec[10][0] = beta; dodec[10][1] = alpha; dodec[10][2] = 0;
		dodec[11][0] = beta; dodec[11][1] = -alpha; dodec[11][2] = 0;
		dodec[12][0] = -beta; dodec[12][1] = alpha; dodec[12][2] = 0;
		dodec[13][0] = -beta; dodec[13][1] = -alpha; dodec[13][2] = 0;
		dodec[14][0] = -alpha; dodec[14][1] = 0; dodec[14][2] = -beta;
		dodec[15][0] = alpha; dodec[15][1] = 0; dodec[15][2] = -beta;
		dodec[16][0] = 0; dodec[16][1] = beta; dodec[16][2] = alpha;
		dodec[17][0] = 0; dodec[17][1] = beta; dodec[17][2] = -alpha;
		dodec[18][0] = 0; dodec[18][1] = -beta; dodec[18][2] = alpha;
		dodec[19][0] = 0; dodec[19][1] = -beta; dodec[19][2] = -alpha;
	}
}


void c_bdd_gene::draw_pentagon( INT32 CONST gl_primitive, int CONST a, int CONST b, int CONST c, int CONST d, int CONST e )
{
	REAL n0[3], d1[3], d2[3];

	sub_v3( d1, dodec[a], dodec[b] );
	sub_v3( d2, dodec[b], dodec[c] );
	cross_v3r( n0, d1, d2 );
	normalize_v3r( n0 );

	GOL::begin( gl_primitive );
		GOL::normal3v( n0 );
		GOL::vertex3v( &dodec[a][0] );
		GOL::vertex3v( &dodec[b][0] );
		GOL::vertex3v( &dodec[c][0] );
		GOL::vertex3v( &dodec[d][0] );
		GOL::vertex3v( &dodec[e][0] );
	GOL::end();
}

void c_bdd_gene::draw_dodecahedron( INT32 CONST gl_primitive )
{
	static int inited = 0;
	if( inited == 0 )
	{
		inited = 1;
		init_dodecahedron();
	}
	draw_pentagon(	gl_primitive,	0,	1,	9,	16,	5	);
	draw_pentagon(	gl_primitive,	1,	0,	3,	18,	7	);
	draw_pentagon(	gl_primitive,	1,	7,	11,	10,	9	);
	draw_pentagon(	gl_primitive,	11, 7,	18,	19,	6	);
	draw_pentagon(	gl_primitive,	8,	17,	16,	9,	10	);
	draw_pentagon(	gl_primitive,	2,	14,	15,	6,	19	);
	draw_pentagon(	gl_primitive,	2,	13,	12,	4,	14	);
	draw_pentagon(	gl_primitive,	2,	19,	18,	3,	13	);
	draw_pentagon(	gl_primitive,	3,	0,	5,	12,	13	);
	draw_pentagon(	gl_primitive,	6,	15,	8,	10,	11	);
	draw_pentagon(	gl_primitive,	4,	17,	8,	15,	14	);
	draw_pentagon(	gl_primitive,	4,	12,	5,	16,	17	);
}				   


void c_bdd_gene::record_item( INT32 CONST gl_primitive, REAL CONST * CONST n1, REAL CONST * CONST n2, REAL CONST * CONST n3 )
{
	REAL q0[3], q1[3];

	sub_v3( q0, n1, n2 );
	sub_v3( q1, n2, n3 );
	cross_v3r( q1, q0, q1 );
	normalize_v3r(q1);

	GOL::begin( gl_primitive );
		GOL::normal3v( q1 );
		GOL::vertex3v( n1 );
		GOL::vertex3v( n2 );
		GOL::vertex3v( n3 );
	GOL::end();
}

void c_bdd_gene::subdivide( INT32 CONST gl_primitive, REAL CONST * CONST v0, REAL CONST * CONST v1, REAL CONST * CONST v2 )
{
	int depth;
	REAL w0[3], w1[3], w2[3];
//	float l;
	int i, j, k, n;

	depth = 1;
	for (i = 0; i < depth; ++i ) {
		for (j = 0; i + j < depth; ++j ) {
			k = depth - i - j;
			for (n = 0; n < 3; ++n ) {
				w0[n] = (i * v0[n] + j * v1[n] + k * v2[n]) / depth;
				w1[n] = ((i + 1) * v0[n] + j * v1[n] + (k - 1) * v2[n])	/ depth;
				w2[n] = (i * v0[n] + (j + 1) * v1[n] + (k - 1) * v2[n]) / depth;
			}
			normalize_v3r( w0 );
			normalize_v3r( w1 );
			normalize_v3r( w2 );
			record_item( gl_primitive, w1, w0, w2 );
		}
	}
}


void c_bdd_gene::draw_triangle( INT32 CONST gl_primitive, int CONST i, REAL CONST data[][3], int CONST ndx[][3] )
{
	subdivide( gl_primitive, data[ndx[i][0]], data[ndx[i][1]], data[ndx[i][2]] );
}

namespace {
// octahedron data: The octahedron produced is centered at the origin and has radius 1.0
	CONSTEXPR REAL odata[6][3] =
	{
		{  1.0,  0.0,  0.0 },
		{ -1.0,  0.0,  0.0 },
		{  0.0,  1.0,  0.0 },
		{  0.0, -1.0,  0.0 },
		{  0.0,  0.0,  1.0 },
		{  0.0,  0.0, -1.0 }
	};

	CONSTEXPR int ondex[8][3] =
	{
		{ 0, 4, 2 },
		{ 1, 2, 4 },
		{ 0, 3, 4 },
		{ 1, 4, 3 },
		{ 0, 2, 5 },
		{ 1, 5, 2 },
		{ 0, 5, 3 },
		{ 1, 3, 5 }
	};
}

void c_bdd_gene::draw_octahedron( INT32 CONST gl_primitive )
{
	for( int i = 7; i >= 0; --i )
		draw_triangle( gl_primitive, i, odata, ondex );
}


namespace {
	REAL CONST X {REAL(0.525731112119133606)};
	REAL CONST Z {REAL(0.850650808352039932)};

	CONSTEXPR REAL idata[12][3] =
	{
		{ -X,  0,  Z },
		{  X,  0,  Z },
		{ -X,  0, -Z },
		{  X,  0, -Z },
		{  0,  Z,  X },
		{  0,  Z, -X },
		{  0, -Z,  X },
		{  0, -Z, -X },
		{  Z,  X,  0 },
		{ -Z,  X,  0 },
		{  Z, -X,  0 },
		{ -Z, -X,  0 }
	};

	CONSTEXPR int index[20][3] =
	{
		{  0,  4,  1 },
		{  0,  9,  4 },
		{  9,  5,  4 },
		{  4,  5,  8 },
		{  4,  8,  1 },
		{  8, 10,  1 },
		{  8,  3, 10 },
		{  5,  3,  8 },
		{  5,  2,  3 },
		{  2,  7,  3 },
		{  7, 10,  3 },
		{  7,  6, 10 },
		{  7, 11,  6 },
		{ 11,  0,  6 },
		{  0,  1,  6 },
		{  6,  1, 10 },
		{  9,  0, 11 },
		{  9, 11,  2 },
		{  9,  2,  5 },
		{  7,  2, 11 },
	};
}

void c_bdd_gene::draw_icosahedron( INT32 CONST gl_primitive )
{
	for( int i = 19; i >= 0; --i )
		draw_triangle( gl_primitive, i, idata, index );
}

namespace {
	CONSTEXPR REAL T {REAL(1.73205080756887729)};

	CONSTEXPR REAL tdata[4][3] =
	{
		{  T,  T,  T },
		{  T, -T, -T },
		{ -T,  T, -T },
		{ -T, -T,  T }
	};

	CONSTEXPR static int tndex[4][3] =
	{
		{ 0, 1, 3 },
		{ 2, 1, 0 },
		{ 3, 2, 0 },
		{ 1, 2, 3 }
	};
}

void c_bdd_gene::draw_tetrahedron( INT32 CONST gl_primitive )
{
	for( int i = 3; i >= 0; --i )
		draw_triangle( gl_primitive, i, tdata, tndex );
}

// End : UGLY from GLUT //
