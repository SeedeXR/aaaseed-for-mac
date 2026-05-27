/*
MapManager library for the conversion, manipulation and analysis 
of maps used in Mobile Robotics research.
Copyright (C) 2005 Shane O'Sullivan

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

email: shaneosullivan1@gmail.com
*/

//	http://blog.ivank.net/fortunes-algorithm-and-implementation.html

#include "voronoi_diagram_generator.h"
#include "aaa_mem.h"
#include "aaa_util.h"
#include "aaa_math.h"
#include "err.h"
#include "v.h"

bool is_double_ok( double CONST d )
{
	INT32 CONST ret = _fpclass( d );
	C_PCHAR str;
	bool b_ret;
	switch( ret )
	{
	case _FPCLASS_SNAN:	str	=	"Signaling NaN";					b_ret = false;	break;
	case _FPCLASS_QNAN:	str	=	"Quiet NaN";						b_ret = false;	break;
	case _FPCLASS_NINF:	str	=	"Negative infinity ( �INF)";		b_ret = false;	break;
	case _FPCLASS_NN:	str	=	"Negative normalized non-zero";		b_ret = true;	break;
	case _FPCLASS_ND:	str	=	"Negative denormalized";			b_ret = true;	break;
	case _FPCLASS_NZ:	str	=	"Negative zero ( � 0)";				b_ret = true;	break;
	case _FPCLASS_PZ:	str	=	"Positive 0 (+0)";					b_ret = true;	break;
	case _FPCLASS_PD:	str	=	"Positive denormalized";			b_ret = true;	break;
	case _FPCLASS_PN:	str	=	"Positive normalized non-zero";		b_ret = true;	break;
	case _FPCLASS_PINF:	str	=	"Positive infinity (+INF)";			b_ret = false;	break;
	default:			str	=	nullptr;							b_ret = true;	break;
	}
	if( !b_ret )
		ERR_PRINT_STRING( "in Voronoi number fucked up %s", str );

	return b_ret;
}

FINLINE	void dbg_double_ok( double CONST d )
{
#if	AAA_DEBUG()
	is_double_ok(d);
#endif
}

CONSTEXPR	INT32 LE = 0;
CONSTEXPR	INT32 RE = 1;

CONSTEXPR	DOUBLE PRECISION =  1 / (1024. * 1024 * 1024 * 16 );

FINLINE	INT32 	CMP( DOUBLE CONST a , DOUBLE CONST b )
{
	DOUBLE t = a - b;
	if( t > 0 )
		return 1;
	if( t < 0 )
		return -1;
	return 0;
}
FINLINE	INT32 	CMP_PRECISION( DOUBLE CONST a, DOUBLE CONST b )
{
	DOUBLE t = a - b;
	if( t >  PRECISION )
		return 1;
	if( t < -PRECISION )
		return -1;
	return 0;
}
FINLINE	bool	IS_ZERO( DOUBLE CONST a )					{	return (-PRECISION ) < a && a < ( PRECISION ); }
FINLINE	bool	EQUAL( DOUBLE CONST a , DOUBLE CONST b )	{	return	IS_ZERO( a - b );			}
FINLINE	bool 	SUP( DOUBLE CONST a, DOUBLE CONST b )		{	return (a-b) > PRECISION;	}
FINLINE	bool 	INF( DOUBLE CONST a , DOUBLE CONST b )		{	return (a-b) < -PRECISION;	}
FINLINE	bool 	XOR( bool CONST a, bool CONST b )			{	return a ? !b : b;	}

FINLINE	INT64	I64_FLOOR( DOUBLE CONST f )
{
	INT64	i = (INT64)f;
	if ( (f < 0.) && ((f-i)!=0.) )	//todo heavy heavy no
		i -= 1;
	return i;
}

/*
DOUBLE round(DOUBLE num) 
{
	return (num < 0.0) ? ceil(num - 0.5) : floor(num + 0.5);
}
*/
CONSTEXPR DOUBLE PFACTOR = 1024 * 64;	//	still some problem but rare
FINLINE	DOUBLE	VO_ROUND(		DOUBLE CONST a )			{	return I64_FLOOR( a / (PRECISION*PFACTOR ) ) * (PRECISION*PFACTOR);	}

using namespace voronoi;

int MAACALLBACK	site_comp( void CONST * CONST p1, void CONST * CONST p2 )
{
	struct point_2d CONST * CONST a	= reinterpret_cast<point_2d CONST*>(p1);
	struct point_2d CONST * CONST b	= reinterpret_cast<point_2d CONST*>(p2);
#if 0
	INT32	r = CMP( a->y, b->y );
	if( r )
		return r;
	return CMP( a->x, b->x );
#else
	INT32	r = CMP( a->y, b->y );
	if( r )
		return r;
	return CMP( a->x, b->x );

#endif
}

FINLINE	DOUBLE dist( struct Site CONST * CONST s, struct Site CONST * CONST t )
{
	//sum_squared_v2r( s->xyz, t->xyz );
	DOUBLE	dx = s->x - t->x;
	DOUBLE	dy = s->y - t->y;
	return sqrt( dx*dx + dy*dy );
}

/*
FINLINE	DOUBLE dist( struct Site CONST * CONST s, struct Site CONST * CONST t )
{
	DOUBLE	dx = s->x - t->x;
	DOUBLE	dy = s->y - t->y;
	return MAX( ABS(dx), ABS(dy) );
}
*/

//	here the edge storing the 2 the input sites reg[0] and reg[2] (delaunay segment)
//		
FINLINE	void generator::push_segment_voronoi( struct Edge* e, struct Site* s1, struct Site* s2 )
{
	if( !_b_gen_voronoi )
		return;

	st_segment_voronoi * CONST seg = _segments_voronoi.get_next_free();
	seg->a = s1;
	seg->b = s2;

	struct Site * CONST	d1 = e->reg[0];
	struct Site * CONST	d2 = e->reg[1];

	bool			b_side;
	if( e->a==1.0 && e->b < 0 )
		b_side = e->b * (s1->x - s2->x) - (s1->y - s2->y) <  0;
	else
		b_side = e->b * (s1->x - s2->x) - e->a * (s1->y - s2->y) >  0;

	//	infact this is always false
	//	if( d2->y < d1->y || (d2->y == d1->y && d2->x < d1->x ) )
	//		b_side = !b_side;

	if( b_side )
		SWAP( s1, s2 );

	PolygonPoint	pp;

	{
		REAL CONST x = REAL( d1->x );
		REAL CONST y = REAL( d1->y );
//		if( !d1->point_list )
//			d1->point_list = new std::vector<struct PolygonPoint>;
	
		pp.site = s1;
		pp.angle = atan2f( float(x - s1->x), float(y - s1->y) );
		d1->point_list->push_back( pp );
		if( s2->b_boundary )
		{
			pp.site = s2;
			pp.angle = atan2f( float(x - s2->x), float(y - s2->y) );
			d1->point_list->push_back( pp );
			d1->b_boundary = true;
			d2->b_boundary = true;
		}
	}

	{
		REAL CONST x = REAL( d2->x );
		REAL CONST y = REAL( d2->y );
//	if( !d2->point_list )
//		d2->point_list = new std::vector<struct PolygonPoint>;

		pp.site = s2;
		pp.angle = atan2f( float(x - s2->x), float(y - s2->y) );
		d2->point_list->push_back( pp );
		if( s1->b_boundary )
		{
			pp.site = s1;
			pp.angle = atan2f( float(x - s1->x), float(y - s1->y) );
			d2->point_list->push_back( pp );
			d1->b_boundary = true;
			d2->b_boundary = true;
		}
	}
}

/*
FINLINE	void generator::out_ep(struct Edge *e)
{
	//if(!triangulate & plot) 
	//clip_line(e);
	//if(!triangulate & !plot)
	//{	
		//printf("e %d", e->edge_id);
			//printf(" %d ", e->end_point[le] != (Site *)nullptr ? e->end_point[le]->site_id : -1) ;
			//printf("%d\n", e->end_point[re] != (Site *)nullptr ? e->end_point[re]->site_id : -1) ;


		//printf("\n");
	//}
	
}
*/

void generator::insert_vertex_address( INT32 vertexNum, struct Site * CONST address )
{
#if VERTEX_OLD
	//if the site address being entered is past the end of the array, then grow the array
	if( vertexNum >= _vertices_nb_allocated )
	{
		INT32 nb = (( vertexNum / 4096 ) + 1 ) * 4096;
		_vertices = (Site**) REALLOC( _vertices, nb*sizeof(Site*) );
		for( INT32 i = _vertices_nb_allocated; i < nb; ++i )
		{
			_vertices[i] = nullptr;
		}
		_vertices_nb_allocated = nb;
		//LOG<<"Grew _vertices to size "<<sizeOfVertices<<" and vertices = "<<vertices<<endl;
		//HEAP_IS_CORRUPT();
	}
	_vertices[vertexNum] = address;
#else
	Site**	s = _vertices.get_next_free();
	*s = address;
#endif
}

FINLINE	void generator::make_vertex( struct Site * CONST v )
{
	v->site_id = ++_vertices_id_generator;
	insert_vertex_address( _vertices_id_generator, v );
	//	out_vertex(v);
}

void generator::insert_vertex_link( INT32 vertexNum, INT32 vertexLinkedTo )
{
	//if the site address being entered is past the end of the array, then grow the array
	INT32	nb_max	=	MAX( vertexNum, vertexLinkedTo );
	if( nb_max >= _vertex_link_nb_allocated )
	{
		INT32	nb		= ( ( nb_max / 4096 ) + 1 ) * 4096;
		//LOG<<"Resizing the array to "<<_vertex_link_nb_allocated + 4000<<endl;
		//LOG<<endl;
		_vertex_link = (Point3*) REALLOC( _vertex_link, nb * sizeof(Point3) );
		if( ! _vertex_link )
			return;
		//if(vertexLinks == 0)LOG<<"Error - realloc failed, vertexLinks == 0"<<endl;
		Point3*	vertex_link = &_vertex_link[_vertex_link_nb_allocated];
		for( INT32 i = nb-_vertex_link_nb_allocated; i > 0; --i )
		{
			vertex_link->x = vertex_link->y = vertex_link->z = -1; //initialise all elements in the array to -1
			vertex_link->count = 0;
			++vertex_link;
		}
		//HEAP_IS_CORRUPT();
		_vertex_link_nb_allocated = nb;
	}

	_vertex_link_nb = nb_max + 1 ;

	{
		Point3&	vertex_link = _vertex_link[vertexNum];
		if( vertex_link.x == -1 )
		{
			vertex_link.x = vertexLinkedTo;
			vertex_link.count = 1;
		}
		else if( vertex_link.y == -1 )
		{
			vertex_link.y = vertexLinkedTo;
			vertex_link.count = 2;
		}
		else if( vertex_link.z == -1 )
		{
			vertex_link.z = vertexLinkedTo;
			vertex_link.count = 3;
		}
	}

	{
		Point3&	vertex_link = _vertex_link[vertexLinkedTo];
		if( vertex_link.x == -1 )
		{
			vertex_link.x = vertexNum;
			vertex_link.count = 1;
		}
		else if( vertex_link.y == -1 )
		{
			vertex_link.y = vertexNum;
			vertex_link.count = 2;
		}
		else if( vertex_link.z == -1 )
		{
			vertex_link.z = vertexNum;
			vertex_link.count = 3;
		}
	}

	//	LOG<<"insertVertexLink exit"<<endl;
}
FINLINE	void generator::out_triple( struct Site CONST * CONST s1, struct Site CONST * CONST s2, struct Site CONST * CONST s3 )
{
	//printf("circle through left=%d right=%d bottom=%d\n", s1->site_id, s2->site_id, s3->site_id);
}

//
//	FREE
//
FINLINE	bool is_escape( struct Freelist* fl )
{
	if( fl->nb_escape <= 0 )
		return false;
	return fl->nb_escape <= fl->nb_used;
}

FINLINE	void init_free( struct Freelist* fl, INT32 size )
{
	fl->head = (struct Freenode *) nullptr;
	fl->nodesize = size;
	fl->nb_used = 0;
}

FINLINE	void make_free( struct Freelist* fl, struct Freenode* cur )
{
	cur->next_free = fl->head;
	fl->head = cur;
}

FINLINE	char* generator::get_free( struct Freelist* fl )
{
	struct Freenode*	t;
	if( fl->head == (struct Freenode *) nullptr )
	{
		INT32 nb = _nsite_nb_sqrt * _nsite_nb_sqrt;	//was	_nsite_nb_sqrt
		t = (struct Freenode *) MALLOC( nb * fl->nodesize );
		if( t == 0 )
			return 0;

		_total_alloc += nb * fl->nodesize;

		_memory_list_cur->next = new FreeNodeArrayList;
		_memory_list_cur = _memory_list_cur->next;
		_memory_list_cur->memory = t;
		_memory_list_cur->next = nullptr;

		char*	pt = (char *)t;
		for( INT32 i=0; i<nb; ++i )
		{
			make_free( fl, (struct Freenode *)pt );
			pt += fl->nodesize;
		}
	}
	t = fl->head;
	fl->head = fl->head->next_free;
	++(fl->nb_used);

	return (char *)t;
}

//
//	SITE
//
FINLINE	struct	Site*	generator::site_create(		DOUBLE x, DOUBLE y, bool b_boundary	)
{
	struct Site* s = (struct Site *) get_free( &_site_free_list );
	dbg_double_ok( x );
	dbg_double_ok( y );
	s->x			=	x;
	s->y			=	y;
	s->z			=	0;
	s->site_id		=	-42;		//	mark site with no site_id yet
	s->ref			=	0;
	s->b_boundary	=	b_boundary;
	s->point_list	=	nullptr;
	//	s->point_list->clear();
	return	s;
}
FINLINE	void generator::site_dec_ref( struct Site* v )
{
	 if( --(v->ref) == 0 )
	 {
//		SAFE_DELETE( v->point_list );
//todo	make_free( &_site_free_list, (Freenode*)v );
	 }
}
FINLINE	void generator::site_inc_ref( struct Site* v )
{
	++(v->ref);
	//++(	v -> overallRefcnt );	//todo remove or use
}
//
//	EDGE
//
FINLINE	void generator::edge_free( struct Edge* e )
{
	site_dec_ref( e->reg[LE] );
	site_dec_ref( e->reg[RE] );
//todo	make_free( &_edge_free_list, (Freenode*)e );
}

FINLINE	struct Edge* generator::edge_create( struct Site * CONST s1, struct Site * CONST s2,
												DOUBLE CONST a, DOUBLE CONST b, DOUBLE CONST c, DOUBLE CONST ce )
{
	dbg_double_ok( a );
	dbg_double_ok( b );
	dbg_double_ok( c );
	dbg_double_ok( ce );

	struct Edge* CONST e = (struct Edge*) get_free( &_edge_free_list );
	
	//	this the two site generating the edge ( connect them and you have a line of a delaunay triangle )
	e->reg[0] = s1;
		site_inc_ref(s1); 
	e->reg[1] = s2;
		site_inc_ref(s2);
	//	this is the equation of the line bisecting these two site (would became a segment of a voronoi poly)
	//	ax + by = c
	e->a = a;
	e->b = b;
	e->c = c;
	//	-bx + ax = ce is the equation of the orthogonal (Delaunay) the line passing by the two site
	e->ce = ce;

	e->end_point[0] = nullptr; //to begin with, there are no endpoints on the bisector - it goes to infinity
	e->end_point[1] = nullptr;

	e->edge_id = ++_edges_id_generator;

	//printf("\nbisect(%d) ((%f,%f) and (%f,%f)",nedges,s1->x,s1->y,s2->x,s2->y);
	//was but now expandsed : out_bisector( e );
	if( _b_gen_delaunay )
		*(_segments_delaunay.get_next_free()) = e;
	//LOG<<"Pushed Delaunay Edge ("<<e->reg[0]->x<<","<<e->reg[0]->y<<") -> ("<<e->reg[1]->x<<","<<e->reg[1]->y<<")";

	if( _b_verbose )
		VERBOSE_PRINT_STRING( "Edge\t%4d\t%4d\t%4d\t%g %g %g", e->edge_id, s1->site_id, s2->site_id, a, b, c );
  
	return e;
}

//
//	HALF EDGE
//
FINLINE	struct Halfedge* generator::half_edge_create( struct Edge * e, INT32 pm )
{
	struct Halfedge * CONST he = (struct Halfedge*) get_free( &_half_edge_free_list );
	he->EL_edge		=	e;
	he->EL_pm		=	pm;
	he->PQ_next		=	nullptr;
	he->vertex		=	nullptr;
	he->EL_refcnt	=	0;
	return he;
}
FINLINE	void generator::half_edge_dec_ref_no_delete( struct Halfedge* he )
{
	--(he->EL_refcnt);

}
FINLINE	void generator::half_edge_dec_ref( struct Halfedge* he )
{
	if( --(he->EL_refcnt) == 0 )
		make_free( &_half_edge_free_list, (Freenode*)he );
}
FINLINE	void generator::half_edge_inc_ref( struct Halfedge* he )
{
	++(he->EL_refcnt);
}

extern	DOUBLE orient2d( DOUBLE* a, DOUBLE* b, DOUBLE* c );
//extern	DOUBLE orient2dfast( DOUBLE* a, DOUBLE* b, DOUBLE* c );
DOUBLE orient2dfast( DOUBLE* a, DOUBLE* b, DOUBLE* c )
{
	DOUBLE CONST acx = a[0] - c[0];
	DOUBLE CONST bcx = b[0] - c[0];
	DOUBLE CONST acy = a[1] - c[1];
	DOUBLE CONST bcy = b[1] - c[1];
	return acx * bcy - acy * bcx;
}


// returns true if p is to right of halfedge e
FINLINE	bool generator::right_of( struct Halfedge* el, struct Site* p )
{
	struct Edge CONST * CONST e			= el -> EL_edge;
	struct Site CONST * CONST topsite	= e -> reg[1];

	bool CONST right_of_site = p->x > topsite->x ;
	if( right_of_site )
	{
		if( el->EL_pm == LE )
			return true;
	}
	else
	{
		if( el->EL_pm == RE )
			return false;
	}

	bool above;
	if( e->a == 1.0 )
	{
#if 0
		bool	b_eb_negatif = e->b < .0 ;
		above =  p->x + e->b * p->y >= e->c;
		//above = -e->b * p->x + p->y < e->ce;
		if( b_eb_negatif )
			above = !above;
#else
		DOUBLE CONST dxp = p->x - topsite->x;
		DOUBLE CONST dyp = p->y - topsite->y;
		bool CONST b_eb_negatif = e->b < .0 ;
		bool fast = false;
		if( XOR( right_of_site, b_eb_negatif ) )
		{
			DOUBLE CONST tmp = e->b * dxp;
			above = dyp >= tmp;	
			fast = above;
		}
		else 
		{
			DOUBLE CONST left = p->x + e->b * p->y;
			above = left > e->c;
			if( b_eb_negatif )
				above = !above;
			if( !above )
				fast = true;
		}
		if( !fast )
		{
			DOUBLE CONST dxs = topsite->x - (e->reg[0])->x;
			above = e->b * (dxp*dxp - dyp*dyp) < dxs*dyp*(1.0+2.0*dxp/dxs + e->b*e->b) ;

			//above = -e->b * p->x + p->y > e->ce;
			if( b_eb_negatif )
				above = !above;
		}
#endif
	}
	else  //e->b==1.0
	{
		DOUBLE CONST yl = e->c - e->a * p->x;
		DOUBLE CONST t1 = p->y - yl;
		DOUBLE CONST t2 = p->x - topsite->x;
		DOUBLE CONST t3 = yl - topsite->y;
		above = (t1*t1) > (t2*t2 + t3*t3);
	}
	return el->EL_pm == LE ? above : !above;
}



FINLINE	struct Site * generator::leftreg( struct Halfedge CONST * CONST he ) CONST
{
	if( !he->EL_edge ) 
		return _bottom_site;

	return( he->EL_pm == LE ? he->EL_edge->reg[LE] : he->EL_edge->reg[RE] );
}

FINLINE	struct Site * generator::rightreg( struct Halfedge CONST * CONST he ) CONST
{
	if( !he->EL_edge )	//	if this halfedge has no edge,
		return _bottom_site;	//		return the bottom site (whatever that is)

	//if the ELpm field is zero, return the site 0 that this edge bisects, otherwise return site number 1
	return( he->EL_pm == LE ? he->EL_edge->reg[RE] : he->EL_edge->reg[LE]) ;
}

void generator::init_geom()
{	
	init_free( &_edge_free_list, sizeof(Edge) );

	_vertices_id_generator	= -1;
	_edges_id_generator		= -1;

	DOUBLE CONST sn = (DOUBLE) _site_nb + 4;
	_nsite_nb_sqrt = (INT32)sqrt(sn);

	_x_factor_hash	= _EL_hash_size / (_xmax - _xmin);
	_y_factor_hash	= _PQ_hash_nb / (_ymax - _ymin);
}
/*
original
dx = s2->x - s1->x;			//get the difference in x dist between the sites
dy = s2->y - s1->y;
adx = dx>0 ? dx : -dx;					//make sure that the difference in positive
ady = dy>0 ? dy : -dy;
newedge -> c = (float)(s1->x * dx + s1->y * dy + (dx*dx + dy*dy)*0.5);//get the slope of the line

if (adx>ady)
{	
newedge -> a = 1.0; newedge -> b = dy/dx; newedge -> c /= dx;//set formula of line, with x fixed to 1
}
else
{	
newedge -> b = 1.0; newedge -> a = dx/dy; newedge -> c /= dy;//set formula of line, with y fixed to 1
};

}
*/

FINLINE	struct Edge * generator::bisect( struct Site *s1, struct Site *s2 )
{
	//	we have to find the equation of the line passing in the middle of these two site
	//		and orthogonal to the direction the two site
	DOUBLE CONST x1 = s1->x;
	DOUBLE CONST x2 = s2->x;
	DOUBLE CONST y1 = s1->y;
	DOUBLE CONST y2 = s2->y;
	DOUBLE CONST dx = x2 - x1; 
	DOUBLE CONST dy = y2 - y1;

#if 0
#if 0
//was
	DOUBLE	dx2 = dx*dx; 
	DOUBLE	dy2 = dy*dy;
	DOUBLE	c = x1 * dx + y1 * dy + (dx*dx + dy*dy) * 0.5;

	//	formula of line ax + by = c 
	if( ABS( dx ) > ABS( dy ) )
	{
		DOUBLE slope = dy / dx;
		dbg_double_ok( slope );
		//	set formula of line, with x fixed to 1
		return edge_create(	s1, s2, 1.0, slope, c / dx, y2 - slope *x2 );
	}
	else
	{
		DOUBLE slope = dx / dy;
		dbg_double_ok( slope );
		//	set formula of line, with y fixed to 1
		return edge_create(	s1, s2, slope, 1.0, c / dy, slope * y2 - x2 );
	}
#else
	//	formula of line ax + by = c 
	if( ABS( dx ) > ABS( dy ) )
	{
		DOUBLE slope = dy / dx;
		dbg_double_ok( slope );
		//	set formula of line, with x fixed to 1
		return edge_create(	s1, s2, 1.0, slope, x1 + dx*.5 + slope * ( y1 + dy*0.5 ), y2 - slope *x2 );
	}
	else
	{
		DOUBLE slope = dx / dy;
		dbg_double_ok( slope );
		//	set formula of line, with y fixed to 1
		return edge_create(	s1, s2, slope, 1.0, y1 + dy*.5 + slope * ( x1 + dx*0.5 ), slope * y2 - x2 );
	}
#endif
#else
	//	DOUBLE	c = s1->x * dx + s1->y * dy + (dx*dx + dy*dy) * 0.5;
	//	formula of line ax + by = c line between the two site two site (will generate voronoi segment later)
	if( ABS( dx ) > ABS( dy ) )
	{
		if( dy == 0 )
			return edge_create(	s1, s2, 1.0, 0., ( x1 + x2 ) *.5, y2 );
		else
		{
			DOUBLE CONST slope = dy / dx;
			dbg_double_ok( slope );
			//	set formula of line, with x fixed to 1
			return edge_create(	s1, s2, 1.0, slope, ( x1 + x2 + slope * ( y1 + y2 ) ) * 0.5, y2 - slope * x2 );
		//	return edge_create(	s1, s2, 1.0, slope, ( x1 + x2 + (y2*y2 - y1*y1) / dx ) * .5, y2 - slope * x2 );
		}
	}
	else
	{
		if( dx == 0 )
			return edge_create(	s1, s2, 0., 1.0, ( y1 + y2 ) *.5, x1  );
		else
		{
			DOUBLE CONST slope = dx / dy;
			dbg_double_ok( slope );
			//	set formula of line, with y fixed to 1
			return edge_create(	s1, s2, slope, 1.0, ( y1 + y2 + slope * ( x1 + x2 ) ) * .5, slope * y2 - x2 );
		//	return edge_create(	s1, s2, slope, 1.0, ( y1 + y2 + (x2*x2 - x1*x1) / dy ) *.5, slope * y2 - x2 );
		}
	}
#endif
}

//	create a new site where the HalfEdges el1 and el2 intersect - note that the PointVDG in the argument list is not used, don't know why it's there
FINLINE	struct Site* generator::intersect( struct Halfedge CONST * CONST el1, struct Halfedge CONST * CONST el2 )//, struct point_2d* p )
{	
	struct Edge CONST * CONST	e1 = el1 -> EL_edge;
	if( !e1 ) 
		return nullptr;
	struct Edge CONST *	CONST e2 = el2 -> EL_edge;
	if( !e2 ) 
		return nullptr;

	//if the two edges bisect the same parent, return null
	if( e1->reg[1] == e2->reg[1] ) 
		return nullptr;

	//	compute x,y coor of intersection
#if 0
	//	original
	DOUBLE	d = e1->a * e2->b - e1->b * e2->a;
	if( IS_ZERO(d) ) return nullptr;	//	if d is really small no intersection
	DOUBLE	x = ( e1->c * e2->b - e2->c * e1->b ) * d;
	DOUBLE	y = ( e2->c * e1->a - e1->c * e2->a ) * d;
#else
	DOUBLE	d, x, y;
#if	1
	if( e1->a == 1.0 )
	{
		if( e2->a == 1.0 )
		{	// e1->a and e2->a	egal 1
			d = e2->b - e1->b;
			if( IS_ZERO(d) )
				return nullptr;	//	if d is really small no intersection
			x = e1->c * e2->b - e2->c * e1->b;
			y = e2->c - e1->c;
		}
		else
		{	// e1->a and e2->b	egal 1
			d = 1. - e1->b * e2->a;
			if( IS_ZERO(d) )
				return nullptr;	//	if d is really small no intersection
			x = e1->c - e2->c * e1->b;
			y = e2->c - e1->c * e2->a;
		}
	}
	else
	{
		if( e2->a == 1.0 )
		{	// e1->b and e2->a	egal 1
			d = e1->a * e2->b - 1.;
			if( IS_ZERO(d) )
				return nullptr;	//	if d is really small no intersection
			x = e1->c * e2->b - e2->c;
			y = e2->c * e1->a - e1->c;
		}
		else
		{	// e1->b and e2->b	egal 1
			d = e1->a - e2->a;
			if( IS_ZERO(d) )
				return nullptr;	//	if d is really small no intersection
			x = e1->c - e2->c;
			y = e2->c * e1->a - e1->c * e2->a;
		}
	}
	d = 1. / d;	//could add noise
	x *= d;
	y *= d;
#else
	if( e1->a == 1.0 )
	{
		if( e2->a == 1.0 )
		{	// e1->a and e2->a	egal 1
			d = e2->b - e1->b;
			if( IS_ZERO(d) ) return nullptr;	//	if d is really small no intersection
			x = ( e1->c * e2->b ) / d - ( e2->c * e1->b ) / d;
			y = e2->c / d - e1->c / d;
		}
		else
		{	// e1->a and e2->b	egal 1
			d = 1. - e1->b * e2->a;
			if( IS_ZERO(d) ) return nullptr;	//	if d is really small no intersection
			x = e1->c / d - ( e2->c * e1->b ) / d;
			y = e2->c / d - ( e1->c * e2->a ) / d;
		}
	}
	else
	{
		if( e2->a == 1.0 )
		{	// e1->b and e2->a	egal 1
			d = e1->a * e2->b - 1.;
			if( IS_ZERO(d) ) return nullptr;	//	if d is really small no intersection
			x = (e1->c * e2->b) / d - e2->c / d;
			y = (e2->c * e1->a) / d - e1->c / d;
		}
		else
		{	// e1->b and e2->b	egal	 1
			d = e1->a - e2->a;
			if( IS_ZERO(d) ) return nullptr;	//	if d is really small no intersection
			x = e1->c / d - e2->c / d;
			y = (e2->c * e1->a) / d - (e1->c * e2->a) / d;
		}
	}
	//d = 1. / d;	//could add noise
#endif
#endif
//	CLAMP( x, -10, 10 ); 
//	CLAMP( y, -10, 10 ); 
	DOUBLE CONST y1	= e1->reg[1]->y;
	DOUBLE CONST y2	= e2->reg[1]->y;
//	if( INF( y1, y2 ) || ( EQUAL( y1, y2 ) && INF( e1->reg[1]->x, e2->reg[1]->x) ) )
	if( y1< y2 || ( y1 == y2 && e1->reg[1]->x < e2->reg[1]->x ) )
	{	
		//	info : x >= e1->reg[1]->x was named right_of_site
		if( XOR( x >= e1->reg[1]->x, el1->EL_pm == RE ) )
			return nullptr;
	}
	else
	{	
		if( XOR( x >= e2->reg[1]->x, el2->EL_pm == RE ) )
			return nullptr;
	}

//	x = VO_ROUND( x ); 
//	y = VO_ROUND( y ); 

	//create a new site at the point of intersection - this is a new vector event waiting to happen
	return site_create( x,y, false );
}


FINLINE bool	RECOMPUTE_X( DOUBLE& x, DOUBLE& y, DOUBLE val, struct Edge* e )
{
	x = val;
	y = ( e->c - x ) / e->b;
	return true;
}
FINLINE bool	RECOMPUTE_Y( DOUBLE& x, DOUBLE& y, DOUBLE val, struct Edge* e )
{
	y = val;
	x = ( e->c - y ) / e->a;
	return true;
}

void generator::clip_line( struct Edge* CONST e )
{
	//	return;

	DOUBLE x1	= e->reg[0]->x;
	DOUBLE y1	= e->reg[0]->y;
	DOUBLE x2	= e->reg[1]->x;
	DOUBLE y2	= e->reg[1]->y;

	//if the distance between the two points this line was created from is less than the min distance then ignore
	if( is_in_dist_squared_v2d( x2 - x1, y2 - y1, _min_distance_between_sites_squared ) )
		return;

	DOUBLE CONST x_min = _border_x_min;
	DOUBLE CONST x_max = _border_x_max;
	DOUBLE CONST y_min = _border_y_min;
	DOUBLE CONST y_max = _border_y_max;

	//	printf("\nEdge (%d), minX = %f, maxX = %f, minY = %f, maxY = %f",e->edge_id, x_min, x_max, y_min, y_max);

	struct Site * s1 = e->end_point[0];
	struct Site * s2 = e->end_point[1];
	bool b_need_vertex_1 = false;
	bool b_need_vertex_2 = false;

	if( e->a == 1.0 )	//skip one case or the other  => nice bug
	{
		if( e->b >= 0.0 )
			SWAP( s1, s2 );

		if( !s1 )
		{
			y1 = y_min;
			b_need_vertex_1 = true;
		}
		else
		{
			y1 = s1->y;
			if( y1 < y_min )
			{
				y1 = y_min;
				b_need_vertex_1 = true;
			}
			else if( y1 > y_max )
			{
				y1 = y_max;
				b_need_vertex_1 = true;
			}
		}
		x1 = e->c - e->b * y1;

		if( !s2 )
		{
			y2 = y_max;
			b_need_vertex_2 = true;
		}
		else
		{
			y2 = s2->y;
			if( y2 > y_max )
			{
				y2 = y_max;
				b_need_vertex_2 = true;
			}
			else if( y2 < y_min )
			{
				y2 = y_min;
				b_need_vertex_2 = true;
			}
		}
		x2 = e->c - e->b * y2;

		if( x1 > x_max )
		{	
			if( x2 > x_max )
				return;
			b_need_vertex_1 = RECOMPUTE_X( x1, y1, x_max, e );
		}
		else if( x1 < x_min )
		{
			if( x2 < x_min )
				return;
			b_need_vertex_1 = RECOMPUTE_X( x1, y1, x_min, e );
		}
		if( x2 > x_max )
			b_need_vertex_2 = RECOMPUTE_X( x2, y2, x_max, e );
		else if( x2 < x_min )
			b_need_vertex_2 = RECOMPUTE_X( x2, y2, x_min, e );

	}
	else
	{
		if( !s1 )
		{
			x1 = x_min;
			b_need_vertex_1 = true;
		}
		else
		{
			x1 = s1->x;
			if( x1 < x_min )
			{
				x1 = x_min;
				b_need_vertex_1 = true;
			}
			else if( x1 > x_max )
			{
				x1 = x_max;
				b_need_vertex_1 = true;
			}
		}
		y1 = e->c - e->a * x1;

		if( !s2 )
		{
			x2 = x_max;
			b_need_vertex_2 = true;
		}
		else
		{
			x2 = s2->x;
			if( x2 < x_min )
			{
				x2 = x_min;
				b_need_vertex_2 = true;
			}
			else if( x2 > x_max )
			{
				x2 = x_max;
				b_need_vertex_2 = true;
			}
		}
		y2 = e->c - e->a * x2;

		if( y1 > y_max )
		{
			if( y2 > y_max )
				return;
			b_need_vertex_1 = RECOMPUTE_Y( x1, y1, y_max, e );
		}
		else if( y1 < y_min )
		{
			if( y2 < y_min )
				return;
			b_need_vertex_1 = RECOMPUTE_Y( x1, y1, y_min, e );
		}
		if( y2 > y_max )
			b_need_vertex_2 = RECOMPUTE_Y( x2, y2, y_max, e );
		else if( y2 < y_min )
			b_need_vertex_2 = RECOMPUTE_Y( x2, y2, y_min, e );
	}
//	x1 = VO_ROUND( x1 ); 
//	y1 = VO_ROUND( y1 ); 
//	x2 = VO_ROUND( x2 ); 
//	y2 = VO_ROUND( y2 ); 

	//printf("\nPushing line (%f,%f,%f,%f)",x1,y1,x2,y2);
	//was		if( ! ((x1 == x2 && x2== x_min) || (x1 == x2 && x2 == x_max) || (y1 == y2 && y2 == y_min) || (y1 == y2 && y2 == y_max)) )
	//better	if( ! ( ( x1==x2 && ( x2==x_min || x2==x_max ) )|| ( y1==y2 && ( y2==y_min || y2==y_max ) ) ) )
	if( ( x1!=x2 || ( x2!=x_min && x2!=x_max ) ) && ( y1!=y2 || ( y2!=y_min && y2!=y_max ) ) )
	{

		if( b_need_vertex_1 )
		{
			//printf("\nCreate new vertex 1 
			s1 = site_create( x1,y1, true );
			make_vertex( s1 );
		}
		if( b_need_vertex_2 )
		{
			s2 = site_create( x2,y2, true );
			make_vertex( s2 );
		}
		push_segment_voronoi( e, s1, s2 );

		insert_vertex_link( s1->site_id, s2->site_id );
	}
}

FINLINE	void generator::end_point( struct Edge* e, INT32 lr, struct Site* s )
{
	e->end_point[lr] = s;
	site_inc_ref(s);
	if( !e->end_point[RE-lr] ) 
		return;
	clip_line( e );
	edge_free( e );
}

//
//	EL
//

struct Edge *	CONST	EDGE_DELETED = (struct Edge *)-2;

bool	generator::EL_initialize()
{
	init_free( &_half_edge_free_list, sizeof **_EL_hash );
	_EL_hash_size = INT32( 2 * _nsite_nb_sqrt  * _EL_hash_factor);
	if( _EL_hash_nb_allocated < _EL_hash_size )
	{
		_EL_hash = (struct Halfedge **) REALLOC( _EL_hash, sizeof *_EL_hash * _EL_hash_size );
		if( !_EL_hash )
			return false;
		_EL_hash_nb_allocated = _EL_hash_size;
		//HEAP_IS_CORRUPT();
	}

	_EL_left_end	= half_edge_create( nullptr, LE );
	_EL_right_end	= half_edge_create( nullptr, LE );
	_EL_left_end	-> EL_left	= nullptr;
	_EL_left_end	-> EL_right	= _EL_right_end;
	_EL_right_end	-> EL_left	= _EL_left_end;
	_EL_right_end	-> EL_right	= nullptr;

	struct	Halfedge** hash = _EL_hash;
	*hash = _EL_left_end;
	for( INT32 i=_EL_hash_size-2; i>0; --i )
		*++hash = nullptr;
	*++hash = _EL_right_end;

	return true;
}

FINLINE	struct Halfedge* generator::EL_right( struct Halfedge CONST * CONST he )	CONST	{	return he->EL_right;	}
FINLINE	struct Halfedge* generator::EL_left(  struct Halfedge CONST * CONST he )	CONST	{	return he->EL_left;		}

FINLINE	void generator::EL_insert( struct Halfedge* left, struct Halfedge* n )
{
	struct Halfedge* CONST right = left->EL_right;
	n->EL_left	= left;
	n->EL_right	= right;
	right->EL_left = n;
	left->EL_right = n;
}
// This delete routine can't reclaim node, since pointers from hash table may be present.
FINLINE	void generator::EL_delete( struct Halfedge* he )
{
	struct Halfedge* CONST left		= he->EL_left;
	struct Halfedge* CONST right	= he->EL_right;
	left->EL_right = right;
	right->EL_left = left;
	he->EL_edge = EDGE_DELETED;
}

//	Get entry from hash table, pruning any deleted nodes
FINLINE	struct Halfedge*	generator::EL_get_hash( INT32 b )
{
	//	done outside this fns to be faster
	//if( b < 0 || b >= _EL_hash_size ) 
	//	return nullptr;

	struct Halfedge* CONST he = _EL_hash[b]; 
	if( !he )
		return nullptr;
	if( he->EL_edge == EDGE_DELETED ) 
	{
		//	so Hash table points to deleted half edge.  Patch as necessary.
		half_edge_dec_ref( he );
		_EL_hash[b] = nullptr;
		return nullptr;
	}
	return he;
}	

FINLINE	struct Halfedge * generator::EL_leftbnd( struct Site* p )
{
	//	Use hash table to get close to desired halfedge
	//use the hash function to find the place in the hash map that this HalfEdge should be
	DOUBLE CONST d = ( p->x - _xmin ) / (_xmax - _xmin);
	INT32 bucket = (INT32) ( d * _EL_hash_size );
	//make sure that the bucket position in within the range of the hash array
	if( bucket < 0 )
		bucket = 0;					
	else if( bucket >= _EL_hash_size )
		bucket = _EL_hash_size - 1;

	struct Halfedge* he = EL_get_hash( bucket );
	if( !he )			//if the HE isn't found, search backwards and forwards in the hash map for the first non-null entry
	{ 
		INT32 i;
		for( i=1; true; ++i )
		{
			INT32 b = bucket - i;
			if( b >= 0 )
			{
				if( he = EL_get_hash( b ) ) 
					break;
			}
			b = bucket + i;
			if( b < _EL_hash_size )
			{
				if( he = EL_get_hash( b ) ) 
					break;
			}
		}
		_total_search += i;
	}
	++_try_nb;
	//	Now search linear list of halfedges for the correct one
	if( he==_EL_left_end || (he != _EL_right_end && right_of(he,p)) )	//	_EL_left_end have no edge
	{
		do
			he = he -> EL_right;
		while( he!=_EL_right_end && right_of(he,p) );	//keep going right on the list until either the end is reached, or you find the 1st edge which the point
		he = he -> EL_left;				//isn't to the right of
	}
	else
	{	//if the point is to the left of the HalfEdge, then search left for the HE just to the left of the point
		do 
			he = he -> EL_left;
		while( he!=_EL_left_end && !right_of(he,p) );
	}

	// Update hash table and reference counts
	if( bucket > 0 && bucket <_EL_hash_size-1 )
	{
		if( struct	Halfedge* CONST he_old = _EL_hash[bucket] ) 
			half_edge_dec_ref_no_delete( he_old );	//	using only half_edge_dec_ref will crash 
		_EL_hash[bucket] = he;
		half_edge_inc_ref( he );
	}
	return he;
}

//
//	PQ
//
bool generator::PQ_initialize()
{	
	_PQ_count	= 0;
	_PQ_min		= 0;
	_PQ_hash_nb	= INT32( 4 * _nsite_nb_sqrt * _PQ_hash_factor );
	if( _PQ_hash_nb_allocated < _PQ_hash_nb )
	{
		_PQ_hash = (struct Halfedge *) REALLOC( _PQ_hash,  _PQ_hash_nb * sizeof *_PQ_hash );
		_PQ_hash_nb_allocated = _PQ_hash_nb;
		//HEAP_IS_CORRUPT();
	}
	if( !_PQ_hash )
		return false;

	struct	Halfedge*	hash = _PQ_hash - 1;
	for( INT32 i=_PQ_hash_nb; i>0; --i )
		(++hash)->PQ_next = nullptr;
	//HEAP_IS_CORRUPT();
	return true;
}

FINLINE	bool generator::PQ_empty() CONST	{	return _PQ_count == 0;	}

FINLINE	struct Halfedge * generator::PQ_extract_min()
{
	struct Halfedge* cur = _PQ_hash[_PQ_min].PQ_next;
	_PQ_hash[_PQ_min].PQ_next = cur->PQ_next;
	--_PQ_count;
	return(cur);
}

FINLINE	struct point_2d generator::PQ_min()
{
	while( !(_PQ_hash[_PQ_min].PQ_next) )
		++_PQ_min;

	struct	Halfedge CONST * CONST he = _PQ_hash[_PQ_min].PQ_next;
	struct point_2d point;
	point.x = he -> vertex -> x;
	point.y = he -> ystar;
	return point;
}

FINLINE	INT32 generator::PQ_bucket( struct Halfedge* he )
{
	INT32 bucket = INT32( ( ( he->ystar - _ymin ) * _PQ_hash_nb) / (_ymax - _ymin) );
	if( bucket < 0 )
		bucket = 0;
	else if( bucket >=_PQ_hash_nb )
		bucket = _PQ_hash_nb-1 ;

	if( bucket < _PQ_min )
		_PQ_min = bucket;

	return bucket;
}

//push the HalfEdge into the ordered linked list of vertices
FINLINE	void generator::PQ_insert( struct Halfedge* he, struct Site* v, DOUBLE offset )
{
	he->vertex = v;
	site_inc_ref(v);

	DOUBLE CONST ystar = v->y + offset;
	he->ystar = ystar;

	struct Halfedge* last = &_PQ_hash[ PQ_bucket(he) ];
	struct Halfedge* next;
	while	(	( next = last->PQ_next )
				&&	(	ystar > next->ystar
						||	( ystar == next->ystar && v->x > next->vertex->x )
					)
			)
	{	
		last = next;
	}

	he->PQ_next = last->PQ_next; 
	last->PQ_next = he;
	++_PQ_count;
}

//remove the HalfEdge from the list of vertices 
FINLINE	void generator::PQ_delete( struct Halfedge* he )
{
	if( he->vertex )
	{	
		struct Halfedge*	last = &_PQ_hash[ PQ_bucket(he) ];
		while( last->PQ_next != he ) 
			last = last->PQ_next;
		last->PQ_next = he->PQ_next;
		--_PQ_count;
		site_dec_ref( he->vertex );
		he->vertex = nullptr;
	}
}

generator::generator()
:_sites(nullptr)
,_site_nb_allocated(0)
,_site_nb(0)
,_vertex_final(nullptr)
,_vertex_final_nb_allocated(0)
#if VERTEX_OLD
,_vertices(nullptr)
,_vertices_nb_allocated(0)
#endif
,_EL_hash(nullptr)
,_EL_hash_nb_allocated(0)
,_PQ_hash(nullptr)
,_PQ_hash_nb_allocated(0)
,_vertex_link(nullptr)
,_vertex_link_nb(0)
,_vertex_link_nb_allocated(0)
,_final_vertex_links(nullptr)
,_final_vertex_link_nb_allocated(0)
,_vertices_count_1(nullptr)
,_vertices_count_3(nullptr)
,_EL_hash_factor(1)
,_PQ_hash_factor(1)
,_b_escape(false)
,_precision( 1./(1024*64) )
,_site_index_begin(0)
,_site_index_end(1024*1024)
,_b_verbose(false)
,_EL_left_end(nullptr)
,_EL_right_end(nullptr)
,_b_gen_delaunay(false)
,_b_gen_voronoi(false)
//,_close_dist( 1./(1024*64) )
{
	//GET_FILE_LOG
	//LOGGING_OFF
	_site_free_list.head			=	nullptr;
	_edge_free_list.head			=	nullptr;
	_half_edge_free_list.nb_escape	=	0;

	set_escape_nb_site( 0 );
	set_escape_nb_edge( 0 );
	set_escape_nb_half_edge( 0 );

	_memory_list		= nullptr;
	_memory_list_cur	= nullptr;
}

generator::~generator()
{
	//LOG<<"In ~generator()"<<endl;

	dealloc();
}

void generator::dealloc()
{
	//LOG<<"In generator::reset()"<<endl;
	cleanup();
	if( _memory_list )
		delete _memory_list;

	struct	Site*	site = _sites - 1;
	for( INT32 i = 0; i<_site_nb_allocated; ++i )
	{
		++site;
		SAFE_DELETE( site->point_list );
	}
	FREE_AND_NULL( _sites );
	_site_nb_allocated = 0;

	FREE_AND_NULL( _vertex_final );
	_vertex_final_nb_allocated = 0;
	FREE_AND_NULL( _vertex_link );
	_vertex_link_nb_allocated = 0;
#if VERTEX_OLD
	FREE_AND_NULL( _vertices );
	_vertices_nb_allocated = 0;
#endif
	FREE_AND_NULL( _final_vertex_links );
	FREE_AND_NULL( _vertices_count_1 );
	FREE_AND_NULL( _vertices_count_3 );
	_final_vertex_link_nb_allocated = 0;

	FREE_AND_NULL( _EL_hash );
	_EL_hash_nb_allocated = 0;
	FREE_AND_NULL( _PQ_hash );
	_PQ_hash_nb_allocated = 0;

	_segments_delaunay.dealloc();
	_vertices.dealloc();
	_segments_voronoi.dealloc();
	//LOG<<"At end of generator::reset()"<<endl;

}

void generator::set_generate_delaunay( bool b )
{
	_b_gen_delaunay = b;
}


void generator::set_generate_voronoi( bool b )
{
	_b_gen_voronoi = b;
}


void cpy_new_site( struct Site* dst, struct Site* src, INT32 id )
{
	dst->x			=	src->x;
	dst->y			=	src->y;
	dst->z			=	src->z;
	dst->site_id	=	id;
	dst->ref		=	0;
	dst->b_boundary	=	false;
	if( dst->point_list	)
		dst->point_list->clear();
	//todo check why here
	if( src->point_list	)
		src->point_list->clear();
}

bool	generator::generate_voronoi(	REAL* src, INT32 site_nb,
										REAL x_min, REAL x_max, REAL y_min, REAL y_max,
										REAL dist_min, bool genVertexInfo )
{
	if( site_nb <= 0 )
	{
		_site_nb = 0;
		return false;
	}

	//	BBOX
	if( x_min > x_max )
		SWAP( x_min, x_max );
	if( y_min > y_max )
		SWAP( y_min, y_max );

	_border_x_min	=	x_min;
	_border_x_max	=	x_max;
	_border_y_min	=	y_min;
	_border_y_max	=	y_max;

	//	SITE MEMORY
	if( _site_nb_allocated < site_nb )
	{
		struct	Site*	site = _sites - 1;
		for( INT32 i = 0; i<_site_nb_allocated; ++i )
		{
			++site;
			SAFE_DELETE( site->point_list );
		}
		_sites = (struct Site *) REALLOC( _sites, site_nb*sizeof( *_sites ) );
		if( !_sites )
		{
			_site_nb_allocated = 0;
			//LOG<<"generate_voronoi returning false 1";
			return false;
		}
		//HEAP_IS_CORRUPT();
		_site_nb_allocated = site_nb;
		{
			struct	Site*	site = _sites - 1;
			for( INT32 i = 0; i<_site_nb_allocated; ++i )
			{
				++site;
				site->point_list = new std::vector<struct PolygonPoint>;
			}
		}
	}

	//	we keep memory allocated but we need to initialize
#if VERTEX_OLD
	MEMCLEAR( _vertices, vertex_final_nb_allocated*sizeof(*_vertices) );
#else
	_vertices.erase();
#endif

	Point3*	vertex_link = _vertex_link;
	for( INT32 i = _vertex_link_nb_allocated; i > 0; --i )	//todoopt this
	{
		vertex_link->x = vertex_link->y = vertex_link->z = -1; //initialize all elements in the array to -1
		vertex_link->count = 0;
		++vertex_link;
	}
	//HEAP_IS_CORRUPT();
	cleanup();
	//HEAP_IS_CORRUPT();
	_min_distance_between_sites				=	dist_min;
	_min_distance_between_sites_squared		=	_min_distance_between_sites * _min_distance_between_sites;

	init_free( &_site_free_list, sizeof(Site) );
		
//	sizeOfVertices = 0;
//	_vertex_final_nb = 0;
//	_final_vertex_link_nb = 0;

	_xmax = _xmin = *(src);
	_ymax = _ymin = *(src+1);

	--src;
	struct	Site*	site = _sites - 1;

	for( INT32 i = 0; i<site_nb; ++i )
	{
		DOUBLE x = *++src;
		DOUBLE y = *++src;
		DOUBLE z = *++src;
		++site;
		x = VO_ROUND( x );
		y = VO_ROUND( y );
		site->x = x;
		site->y = y;
		site->z = z;
		site->ref = 0;

		if( x < _xmin)
			_xmin = x;
		else if( x > _xmax)
			_xmax = x;

		if( y < _ymin)
			_ymin = y;
		else if( y > _ymax)
			_ymax = y;

		//printf("\n%f %f\n",x,y );
	}
	//HEAP_IS_CORRUPT();

	qsort( _sites, site_nb, sizeof (*_sites), site_comp ); //undo

	//	remove duplicate
	{
		INT32 count = -1;
		struct	Site*	dst = _sites;
		struct	Site*	src = _sites + MIN( _site_index_begin, site_nb );

		//if( dst != src )
		//	*dst = *src;
		cpy_new_site( dst, src, ++count );
		site_nb = MAX0( MIN( site_nb-1, _site_index_end ) - _site_index_begin );
		for( INT32 i = 0; i<site_nb; ++i )
		{
			++src;
#if 0	//debug
			if( i == 0 ) continue;
			if( i == 1 ) continue;
			if( i == 2 ) continue;
//			if( i == 3 ) continue;
			if( i == 4 ) continue;
			if( i == 5 ) continue;
			if( i == 6 ) continue;
//			if( i == 7 ) continue;
			if( i == 8 ) continue;
			if( i == 9 ) continue;
//			if( i == 10 ) continue;
//			if( i == 11 ) continue;
			if( i == 12 ) continue;
			if( i == 13 ) continue;
//			if( i == 14 ) continue;
			if( i == 15 ) continue;
			if( i == 16 ) continue;
//			if( i == 17 ) continue;
			if( i == 18 ) continue;
//			if( i == 19 ) continue;
			if( i == 20 ) continue;
			if( i == 21 ) continue;
			if( i == 22 ) continue;
			if( i == 23 ) continue;
			if( i == 24 ) continue;
			if( i == 25 ) continue;
//			if( i == 26 ) continue;
			if( i == 27 ) continue;
//			if( i == 28 ) continue;
//			if( i == 29 ) continue;
//			if( i == 30 ) continue;

//			if( INSIDE_MIN_MAX( i, 4,  8 ) )
//				continue;
#endif
			//	remove point identical or to close
			if( CMP_PRECISION( src->x, dst->x ) != 0 || CMP_PRECISION( src->y, dst->y) != 0 )
			{
				//++dst;
				//if( dst != src )
				//	*dst = *src;
				//dst->site_id = ++count;
				cpy_new_site( ++dst, src, ++count );
			}
		}
		_site_nb = count + 1;
	}

	_segments_voronoi.erase();
	_segments_delaunay.erase();
	init_geom();
	
	//LOG<<"About to call voronoi("<<triangulate<<")";
	voronoi( genVertexInfo ); //uncomment

	return true;
}



//
//	MEM
//
void generator::cleanup()
{
	//LOG<<"In cleanup"<<endl;

	FreeNodeArrayList* cur = _memory_list;
	while( cur )
	{
		FreeNodeArrayList* next = cur->next;
		FREE( cur->memory );
		delete cur;
		cur = next;
	};

	_memory_list = new FreeNodeArrayList;
	_memory_list->next = nullptr;
	_memory_list->memory = nullptr;
	_memory_list_cur = _memory_list;

	//LOG<<"At the end of cleanup";
}

//	return a single in-storage site
FINLINE		struct Site* generator::next_one()
{
	if( _site_id < _site_nb )
	{	
		struct Site* CONST s = &_sites[_site_id];
		++_site_id;
		return s;
	}
	else	
		return nullptr;
}

/* implicit parameters: _site_nb, sqrt_nsites, xmin, xmax, ymin, ymax,
deltax, deltay (can all be estimates).
Performance suffers if they are wrong; better to make _site_nb,
deltax, and deltay too big than too small.  (?) */

bool generator::voronoi( bool b_gen_vertex_info )
{
	_site_id = 0;

	_total_search = 0;
	_try_nb = 0;

	PQ_initialize();
	_bottom_site = next_one();
	bool retval = EL_initialize();

	if( !retval )
	{
		//LOG<<"voronoi returning false 1";
		return false;
	}
	
	struct point_2d newintstar;
	struct Site* newsite = next_one();
	//LOG<<"About to go into the infinite while loop";
	while(1)
	{
		//LOG<<++counter;
		if( !PQ_empty() ) 
			newintstar = PQ_min();
		
		//if the lowest site has a smaller y value than the lowest vector intersection, process the site
		//otherwise process the vector intersection		

		if(	newsite && ( PQ_empty() || newsite -> y < newintstar.y || (newsite->y == newintstar.y && newsite->x < newintstar.x ) ) )
		{	//	new site is smallest - this is a site event
		//	out_site(newsite);						//output the site
			struct Halfedge		  *	      lbnd	=	EL_leftbnd( newsite );	//	get the first HalfEdge to the LEFT of the new site
			struct Halfedge CONST * CONST rbnd	=	EL_right( lbnd );					//	get the first HalfEdge to the RIGHT of the new site
			struct Site			  * CONST bot	=	rightreg( lbnd );					//if this halfedge has no edge, , bot = bottom site (whatever that is)
			struct Edge			  * CONST e		=	bisect( bot, newsite );				//create a new edge that bisects 
			e->b_created_how = true;	//	keep the info in the edge for rendering			
			struct Halfedge		  *	      bisector =	half_edge_create( e, LE );				//create a new HalfEdge, setting its ELpm field to 0			
			EL_insert( lbnd, bisector );										//insert this new bisector edge between the left and right vectors in a linked list	

			if( struct Site* p = intersect( lbnd, bisector) ) 	//if the new bisector intersects with the left edge, remove the left edge's vertex, and put in the new one
			{	
				PQ_delete( lbnd );
				PQ_insert( lbnd, p, dist(p,newsite) );
			}
			lbnd = bisector;						
			bisector = half_edge_create( e, RE );						//create a new HalfEdge, setting its ELpm field to 1
			EL_insert( lbnd, bisector );							//insert the new HE to the right of the original bisector earlier in the IF stmt

			if( struct Site* p = intersect( bisector, rbnd ) )	//if this new bisector intersects with the
				PQ_insert( bisector, p, dist(p,newsite) );		//push the HE into the ordered linked list of vertices				

			newsite = next_one();
		}
		else if( !PQ_empty() )	// intersection is smallest - this is a vector event		
		{	
			struct Halfedge       *	CONST lbnd	=	PQ_extract_min();		//	pop the HalfEdge with the lowest vector off the ordered list of vectors				
			struct Halfedge       *	CONST llbnd	=	EL_left( lbnd );		//	get the HalfEdge to the left of the above HE
			struct Halfedge       *	CONST rbnd	=	EL_right( lbnd );		//	get the HalfEdge to the right of the above HE
			struct Halfedge CONST * CONST rrbnd	=	EL_right( rbnd );		//	get the HalfEdge to the right of the HE to the right of the lowest HE 
			struct Site		      *       bot	=	leftreg( lbnd );		//	get the Site to the left of the left HE which it bisects
			struct Site		      *       top	=	rightreg( rbnd );		//	get the Site to the right of the right HE which it bisects

			out_triple( bot, top, rightreg(lbnd) );		//	output the triple of sites, stating that a circle goes through them

			struct Site		      * CONST v		=	lbnd->vertex;	//	get the vertex that caused this event
			make_vertex( v );							//	set the vertex number - couldn't do this earlier since we didn't know when it would be processed
			end_point( lbnd->EL_edge, lbnd->EL_pm, v );	//	set the endpoint of the left HalfEdge to be this vector
			end_point( rbnd->EL_edge, rbnd->EL_pm, v );	//	set the endpoint of the right HalfEdge to be this vector
			EL_delete( lbnd );							//	mark the lowest HE for deletion - can't delete yet because there might be pointers to it in Hash Map	
			PQ_delete( rbnd );							//	remove all vertex events to do with the right HE
			EL_delete( rbnd );							//	mark the right HE for deletion - can't delete yet because there might be pointers to it in Hash Map	

			INT32	pm;										
			if( bot->y > top->y )		//	if the site to the left of the event is higher than the Site
			{										//		to the right of it, then swap them and set the 'pm' variable to 1
				SWAP( bot, top );
				pm = RE;
			}
			else
				pm = LE;							//	set the pm variable to zero	

			struct Edge * CONST e = bisect( bot, top );			//	create an Edge (or line) that is between the two Sites. This creates
																		//	the formula of the line, and assigns a line number to it
			e->b_created_how = false;	//	keep the info in the edge for rendering	
			struct Halfedge*	bisector = half_edge_create( e, pm );	//	create a HE from the Edge 'e', and make it point to that edge with its ELedge field
			EL_insert( llbnd, bisector );			//	insert the new bisector to the right of the left HE
			end_point( e, RE-pm, v );				//	set one endpoint to the new edge to be the vector point 'v'.
													//	If the site to the left of this bisector is higher than the right
													//		Site, then this endpoint is put in position 0; otherwise in pos 1
			site_dec_ref(v);						//		delete the vector 'v'

			//	if left HE and the new bisector don't intersect, then delete the left HE, and reinsert it 
			if( struct Site * CONST p = intersect( llbnd, bisector ) )
			{	
				PQ_delete( llbnd );
				PQ_insert( llbnd, p, dist(p,bot) );
			}

			//	if right HE and the new bisector don't intersect, then reinsert it 
			if( struct Site * CONST p = intersect( bisector, rrbnd ) )
				PQ_insert( bisector, p, dist(p,bot) );
		}
		else
			break;
		if( _b_escape )
		{
			if( is_escape( &_half_edge_free_list	)	)
				break;
			if( is_escape( &_edge_free_list			)	)
				break;
			if( is_escape( &_site_free_list			)	)
				break;
		}
	}

	for( struct Halfedge CONST * lbnd = EL_right(_EL_left_end); lbnd != _EL_right_end; lbnd = EL_right(lbnd) )
	{	
		clip_line( lbnd->EL_edge );
	}

	//LOG<<"Voronoi: after finishing the voronoi diagram - about to get the vertices";
	//LOG<<"_vertex_link_nb_allocated = "<<_vertex_link_nb_allocated;
	//count the total number of 
	if( b_gen_vertex_info )
	{
		_vertex_final_nb = 0;
		for( INT32 i = 0; i < _vertex_link_nb_allocated; ++i )
		{
			if( _vertex_link[i].count == 1 || _vertex_link[i].count == 3 )
				++_vertex_final_nb;
		}
		//LOG<<"After counting the size of the final vertices = "<<_vertex_final_nb<<endl;
		if( _vertex_final_nb_allocated < _vertex_final_nb )
		{
			_vertex_final = (point_2d*) REALLOC( _vertex_final, _vertex_final_nb*sizeof(point_2d) );
			_vertex_final_nb_allocated = _vertex_final_nb;
			//HEAP_IS_CORRUPT();
		}

		point_2d* pt = _vertex_final;
		for( INT32 i = 0; i < _vertex_link_nb_allocated; ++i )
		{
			if( (_vertex_link[i].count == 1 || _vertex_link[i].count == 3) && _vertices[i] )
			{
				//deal with 3d
				pt->x = _vertices[i]->x;
				pt->y = _vertices[i]->y;
				++pt;
			}
		}
		//HEAP_IS_CORRUPT();
		//LOG<<"Just before generateVertexLinks()"<<endl;
		generate_final_vertex_link();
		//LOG<<"Just after generateVertexLinks()"<<endl;
	}

	//LOG<<"After cleanup()";

	return true;
}



void generator::generate_final_vertex_link()
{
#if VERTEX_OLD
	if( !_vertices )
#else
	if( _vertices.get_size() <= 0 )
#endif
	{
		//LOG<<"vertices is zero, not doing anything in generateVertexLinks()";
		return;
	}

#if VERTEX_OLD
	_final_vertex_link_nb = MAX( _vertices_nb_allocated, _vertex_link_nb );
#else
	_final_vertex_link_nb = MAX(  _vertices.get_size(), _vertex_link_nb );
#endif
	//LOG<<"sizeOfFinalVertexLinks = "<<_final_vertex_link_nb<<",_vertex_link_nb_allocated = "<<_vertex_link_nb_allocated<<endl; 

	if( _final_vertex_link_nb_allocated < _final_vertex_link_nb )
	{
		_final_vertex_links = (struct VertexLink*)REALLOC( _final_vertex_links, _final_vertex_link_nb * sizeof(VertexLink) );
		_vertices_count_1 = (INT32*)REALLOC( _vertices_count_1, _final_vertex_link_nb * sizeof(INT32) );
		_vertices_count_3 = (INT32*)REALLOC( _vertices_count_3, _final_vertex_link_nb * sizeof(INT32) );
		if( !_final_vertex_links || !_vertices_count_1 || !_vertices_count_3 )
		{
			FREE_AND_NULL( _final_vertex_links );
			FREE_AND_NULL( _vertices_count_1 );
			FREE_AND_NULL( _vertices_count_3 );
			_final_vertex_link_nb_allocated = 0;
			return;
		}
		_final_vertex_link_nb_allocated = _final_vertex_link_nb;
		//HEAP_IS_CORRUPT();
	}
	if( !_final_vertex_links )
		return;

	//LOG<<"Created vertexLinks array of size "<<sizeOfFinalVertexLinks<<endl;
	//LOG<<"_final_vertex_links = "<<finalVertexLinks;
	for( INT32 i = 0; i< _final_vertex_link_nb; ++i )
	{
		_final_vertex_links[i].count = 0;		
		_vertices_count_1[i] = 0;
		_vertices_count_3[i] = 0;
	}

	//LOG<<"sizeOfVertices = "<<sizeOfVertices;
	//LOG<<"_vertex_link_nb_allocated = "<<_vertex_link_nb_allocated;
	//LOG<<"count1vertices and count3vertices are of size "<<_final_vertex_link_nb;
	//LOG<<"finalVertexLinks is of size "<<_final_vertex_link_nb;
	
	INT32	count_1	=	0;
	INT32	count_3	=	0;
	
	//LOG<<"Before copying "<<sizeOfVertices<<" coordinates into _final_vertex_links"<<endl;
#if VERTEX_OLD
	for( INT32 i = 0; i < _vertices_nb_allocated; ++i )
	{		
	//	LOG<<"copying "<<i<<" coords,vertices[i]="<<vertices[i]<<endl;
		if( !_vertices[i] )
			break;	
		_final_vertex_links[i].coord = _vertices[i]->coord;
	}
#else
	reset_vertice_iterator();
	VertexLink* vl = _final_vertex_links - 1;
	while(	Site CONST * s	=	get_vertice_next()	 )
	{
		++vl;
		vl->coord .x = s->x;
		vl->coord .y = s->y;
	}
#endif
	//LOG<<"After copying "<<i+1<<" coordinates into _final_vertex_links"<<endl;

	for( INT32 i = 0; i < _vertex_link_nb; ++i )
	{
		switch( _vertex_link[i].count )
		{
		case 1:_vertices_count_1[count_1] = i;
			++count_1;
			break;
		case 3:_vertices_count_3[count_3] = i;
			++count_3;
			break;		
		}		
	}

	//LOG<<"About to process the "<<count_1<<" count 1 (leaf) _vertices"<<endl;

	//first, we go from all leaf nodes, those with just one edge, to either the next leaf or the
	//next node with 3 edges
	
	for( INT32 i = 0; i < count_1; ++i )
	{
		if( _vertex_link[_vertices_count_1[i]].count != 1 )//if we've already been here, ignore this vertex
			continue;
	//	LOG<<i;
		
		INT32	currentVertex = (long)_vertex_link[_vertices_count_1[i]].x;//use the x variable, since it should be the only one set
		INT32	prevVertex = _vertices_count_1[i];
		_vertex_link[_vertices_count_1[i]].count--;	//don't revisit this site

		while( currentVertex != -1 && currentVertex < _vertex_link_nb_allocated && _vertex_link[currentVertex].count != 1 && _vertex_link[currentVertex].count != 3 )
		{
			if( _vertex_link[currentVertex].count == 2 )
			{
				if( _vertex_link[currentVertex].x == prevVertex )
				{
					_vertex_link[currentVertex].x = _vertex_link[currentVertex].y;
					_vertex_link[currentVertex].y = -1;
					_vertex_link[currentVertex].count--;
				}
				else if( _vertex_link[currentVertex].y == prevVertex )
				{
					_vertex_link[currentVertex].y = -1;
					_vertex_link[currentVertex].count--;
				}
				prevVertex = currentVertex;
				currentVertex = (long)_vertex_link[currentVertex].x;

			}
			else
			{
				break;//this is an error, and shouldn't happen
			}
		}	

		if( currentVertex == -1 || currentVertex >= _vertex_link_nb_allocated )
			continue; //this is an error

		_final_vertex_links[_vertices_count_1[i]].v[_final_vertex_links[_vertices_count_1[i]].count].x = _vertices[currentVertex]->x;
		_final_vertex_links[_vertices_count_1[i]].v[_final_vertex_links[_vertices_count_1[i]].count].y = _vertices[currentVertex]->y;

		_final_vertex_links[_vertices_count_1[i]].count++;
		

		if( _vertex_link[currentVertex].count == 1 )
		{
			_vertex_link[currentVertex].count = 0;
			_vertex_link[currentVertex].x = -1;
		}
		else if( _vertex_link[currentVertex].count == 3 )
		{
			if( _vertex_link[currentVertex].x == prevVertex )
				_vertex_link[currentVertex].x = -1;
			else if( _vertex_link[currentVertex].y == prevVertex )
				_vertex_link[currentVertex].y = -1;
			else if( _vertex_link[currentVertex].z == prevVertex )
				_vertex_link[currentVertex].z = -1;			
		}		
	}

	//LOG<<"Finished processing the "<<count_1<<" count 1 (leaf) _vertices";
	//LOG<<"About to process the "<<count_3<<" count 3 (non-leaf) _vertices"<<endl;

	INT32	count3links[3];

	//at this stage, all the edges that end in leaf nodes are processed, now just do the edges between vertices with 3 connections
	for( INT32 i = 0; i< count_3; i++ )
	{
	//	LOG<<i<<", count3vertices["<<i<<"] = "<<count3vertices[i];

		//get the (possible) three vertices that the vertex at pos count3vertices[i] of vertexLinks is linked to
		count3links[0] = (INT32)_vertex_link[_vertices_count_3[i]].x;
		count3links[1] = (INT32)_vertex_link[_vertices_count_3[i]].y;
		count3links[2] = (INT32)_vertex_link[_vertices_count_3[i]].z;
	//	LOG<<"after count3links, _final_vertex_links[count3vertices[i]].count = "<<_final_vertex_links[count3vertices[i]].count<<endl;

		//LOGCODE		if(_final_vertex_links[count3vertices[i]].count > 2 || _final_vertex_links[count3vertices[i]].count < 0)
		//LOGCODE		{
		//LOGCODE			LOG<<"Error: _final_vertex_links[count3vertices[i]].count = "<<_final_vertex_links[count3vertices[i]].count;
		//LOGCODE			return;
		//LOGCODE		}

		//mark one of the links in this finalVertexLink
		//_final_vertex_links[count3vertices[i]].v[_final_vertex_links[count3vertices[i]].count] = 
		//														_vertices[currentVertex]->coord;
	//	LOG<<"after _final_vertex_links[count3vertices[i]]"<<endl;

		for( INT32 j = 0; j< 3; j++ )//process each of the _vertices that the current one is linked to
		{
			if(count3links[j] == -1) //all links to leaf nodes are marked with -1
				continue;

			INT32	currentVertex = count3links[j];
			INT32	prevVertex  = _vertices_count_3[i];
			while( currentVertex >=0 && currentVertex < _vertex_link_nb_allocated && _vertex_link[currentVertex].count !=  3)
			{
				if(_vertex_link[currentVertex].count == 2)
				{
					if(_vertex_link[currentVertex].x == prevVertex )
					{
						_vertex_link[currentVertex].x = -1;
						prevVertex = currentVertex;
						currentVertex = (long)_vertex_link[currentVertex].y;
					}
					else if(_vertex_link[currentVertex].y == prevVertex)
					{
						_vertex_link[currentVertex].y = -1;
						prevVertex = currentVertex;
						currentVertex = (long)_vertex_link[currentVertex].x;
					}				
					else
					{
						break;//this is an error, prevents infinite recursion in case I make a mistake
					}
				}
			}
			if(currentVertex < 0|| currentVertex >= _vertex_link_nb_allocated)
			{
				//LOG<<"Error, currentVertex = "<<currentVertex<<" and _vertex_link_nb_allocated = "<<_vertex_link_nb_allocated;
				//LOG<<"On element "<<i+1<<" of "<<count_3;
				continue; //this is an error, and shouldn't happen
			}
		
			_final_vertex_links[_vertices_count_3[i]].v[_final_vertex_links[_vertices_count_3[i]].count].x = _vertices[currentVertex]->x;
			_final_vertex_links[_vertices_count_3[i]].v[_final_vertex_links[_vertices_count_3[i]].count].y = _vertices[currentVertex]->y;
			_final_vertex_links[_vertices_count_3[i]].count++;
			if( _vertex_link[currentVertex].count == 3 )
			{
				if( _vertex_link[currentVertex].x == prevVertex )
					_vertex_link[currentVertex].x = -1;
				else if( _vertex_link[currentVertex].y == prevVertex )
					_vertex_link[currentVertex].y = -1;
				else if( _vertex_link[currentVertex].z == prevVertex )
					_vertex_link[currentVertex].z = -1;		
			}	
			else
				continue;
		}		
	}
	//HEAP_IS_CORRUPT();
	//LOG<<"Finished processing the count 3 _vertices"<<endl;
}

bool generator::get_next_vertex_pair_final( REAL& x1, REAL& y1, REAL& x2, REAL& y2 )
{
	if( !_final_vertex_links )
		return false;

	while( _final_vertex_link_cur < _final_vertex_link_nb && _final_vertex_links[_final_vertex_link_cur].count < 1 )
		++_final_vertex_link_cur;
	VertexLink&	vertex_link = _final_vertex_links[_final_vertex_link_cur];
	if( _final_vertex_link_cur >= _final_vertex_link_nb || vertex_link.count < 1 )
		return false;

	INT32 num = vertex_link.count - 1;
	vertex_link.count--;

	x1	=	(REAL)vertex_link.v[num].x;
	y1	=	(REAL)vertex_link.v[num].y;
	x2	=	(REAL)vertex_link.coord.x;
	y2	=	(REAL)vertex_link.coord.y;

	return true;
}

/*
int anglecomp(const void * p1, const void * p2)
{
	PolygonPoint * s1 = (PolygonPoint *)p1 ;
	PolygonPoint * s2 = (PolygonPoint *)p2 ;

	if( s1->angle < s2->angle )
		return -1;
	if( s1->angle > s2->angle)
		return 1;
	return 0;
}


struct angle_less	{ 
public: 
	bool CONST operator() ( CONST struct PolygonPoint a, CONST struct PolygonPoint b ) CONST
	{	return a.angle < b.angle ;	}

};
*/

//void generator::sort_voronoi_site( struct Site* s )
//{
//	sort( s->point_list->begin(), s->point_list->end(), angle_less() );
/*
int i, j, c, any, centrevalue, cornerinpolygon[4];

	if (polygons[sitenbr].numpoints == 0)
	{
		for(c = 0; c < 4; c++)
		{
			pushpoint(sitenbr, corners[c].x, corners[c].y, 0);
		}
	}

	qsort(polygons[sitenbr].pointlist, polygons[sitenbr].numpoints, sizeof(PolygonPoint), anglecomp);

	if (polygons[sitenbr].boundary)
	{
		//		printf("\nsite %d is boundary intersection\n", sitenbr);

		for(c = 0; c < 4; c++) cornerinpolygon[c] = 1;

		for(i = 0; i < polygons[sitenbr].numpoints; i++)
		{
			//			printf("Point (%lf,%lf) %d\n", polygons[sitenbr].pointlist[i].coord.x,polygons[sitenbr].pointlist[i].coord.y,polygons[sitenbr].pointlist[i].boundary);
			j = i > 0?i-1:polygons[sitenbr].numpoints-1;
			if (	(!polygons[sitenbr].pointlist[i].boundary || !polygons[sitenbr].pointlist[j].boundary) &&
				(polygons[sitenbr].pointlist[i].coord.x != polygons[sitenbr].pointlist[j].coord.x ||
				polygons[sitenbr].pointlist[i].coord.y != polygons[sitenbr].pointlist[j].coord.y))
			{
				//				printf("line side test (%lf,%lf) => (%lf,%lf)\n",polygons[sitenbr].pointlist[i].coord.x,polygons[sitenbr].pointlist[i].coord.y,polygons[sitenbr].pointlist[j].coord.x,polygons[sitenbr].pointlist[j].coord.y);
				any = 0;
				centrevalue = ccw(polygons[sitenbr].pointlist[i].coord, polygons[sitenbr].pointlist[j].coord, polygons[sitenbr].coord);
				//printf(" test against centre (%lf,%lf) %d\n", polygons[sitenbr].coord.x, polygons[sitenbr].coord.y, centrevalue);
				for(c = 0; c < 4; c++)
				{
					if (cornerinpolygon[c])
					{

						//printf(" test against corner (%lf,%lf) %d\n", corners[c].x, corners[c].y, ccw(polygons[sitenbr].pointlist[i].coord, polygons[sitenbr].pointlist[j].coord, corners[c]));

						if (centrevalue == ccw(polygons[sitenbr].pointlist[i].coord, polygons[sitenbr].pointlist[j].coord, corners[c]))
						{
							any = 1;
						}
						else
						{
							cornerinpolygon[c] = 0;
						}
					}
				}
				if (!any) break;
			}
		}
		if (any)
		{
			for(c = 0; c < 4; c++)
			{
				if (cornerinpolygon[c])
				{
					//					printf("adding corger (%lf,%lf) to %d\n", corners[c].x, corners[c].y, sitenbr);
					pushpoint(sitenbr, corners[c].x, corners[c].y, 0);
				}
			}
		}
		qsort(polygons[sitenbr].pointlist, polygons[sitenbr].numpoints, sizeof(PolygonPoint), anglecomp);

		polygons[sitenbr].boundary = 0;
	}

	*numpoints = polygons[sitenbr].numpoints;
	*pS = polygons[sitenbr].pointlist;
	*/
//}

