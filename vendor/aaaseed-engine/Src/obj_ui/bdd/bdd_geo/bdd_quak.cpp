/*******************************************************************************
**
**  Module: bdd_quak.c
**
**  DESCRIPTION: 
**
**  CREATION: Emmanuel BERRIET					date : 23/03/97
**
\******************************************************************************/

#include "obj_ui/bdd/bdd_geo/bdd_quak.h"
#include "obj_ui/deformer/def_node.h"
#include "draw/render.h"
#include "draw/map.h"
#include "draw/model.h"
#include "draw/colorrnd.h"
#include "draw/leveldata.h"
#include "draw/bspfile.h"
#include "infrastructure/layer/layer.h"
#include "gol/gol_draw.h"

c_point_xyz_real*	normal = nullptr;

void render_bsp()
{
	BSPFACE*			pCurFace = nullptr;
	BSPEDGE*			pCurEdge = nullptr;
	c_point_xyz_real*	pCurVertex = nullptr;
	c_point_xyz_real*	loc_normal;
	long edgeIndex;
	//int i, j;
	REAL	angle;
	REAL	one_point[3];
	INT32	max;

	INT32	color_mat;

	if ( !faces || !edges || !vertices )
		return;

	//todo	this should be one level above (rendering)
	//		but the rendering of object should go thru bdd_tri (bdd_poly in fact)?
	if ( color_mat = c_render::get_cur()->get_random_on_color() )
		c_color_random::begin();

	bool use_normal = c_layer::get_cur()->is_normal_draw();

	max = c_model::cur->compute_from_resolution( levelModel.numFaces );

	loc_normal = normal;
	
	auto def = c_def_node::get_cur();
	if( !def->is_deforming() )
		def = nullptr;

	if( c_map::get_cur()->is_implicit() )
	{
		REAL	tu = c_map::get_cur()->get_u();
		REAL	tv = c_map::get_cur()->get_v();
		for( INT32 i = 0; i < max; ++i )
		{
			//maa	tex_2d_bind( IMOD(i/100,4) + 19);
			pCurFace = &faces[levelModel.idxFaces + i];
			GOL::begin(GL_TRIANGLE_FAN);
			if ( color_mat )
				c_color_random::set();
			if ( use_normal)
				GOL::normal3v( &loc_normal->x);

			if ( pCurFace->numEdges == 4 )
			{
				for( INT32 j =3; j >= 0; --j )
				{
					edgeIndex = edgeIndices[pCurFace->idxEdges + j];
					if(edgeIndex > 0)
					{
						pCurEdge = &edges[edgeIndex];
						pCurVertex = &vertices[pCurEdge->idxV0];
					}
					else
					{
						pCurEdge = &edges[-edgeIndex];
						pCurVertex = &vertices[pCurEdge->idxV1];
					}
					*one_point = ( (j&0x1) != ((j&0x2)>>1) )? tu : REAL(0);
					*(one_point+1) = ((j&0x2) == 0) ? tv : REAL(0);
					if( def )
						def->apply( one_point, 1 );
					GOL::texcoord2v( one_point );

					*one_point = pCurVertex->x;
					*(one_point+1) = pCurVertex->y;
					*(one_point+2) = pCurVertex->z;
					if( def )
						def->apply( one_point, 1 );
					GOL::vertex3v( one_point );
				}
			}
			else
			{
				angle = OVER_ONE_AS_REAL(pCurFace->numEdges);
				for( INT32 j = pCurFace->numEdges - 1; j >= 0; --j )
				{
					edgeIndex = edgeIndices[pCurFace->idxEdges + j];
					if(edgeIndex > 0)
					{
						pCurEdge = &edges[edgeIndex];
						pCurVertex = &vertices[pCurEdge->idxV0];
					}
					else
					{
						pCurEdge = &edges[-edgeIndex];
						pCurVertex = &vertices[pCurEdge->idxV1];
					}
					*one_point     = REAL(.5) - COS_TURN(angle*j-.375) * REAL(.5) * tu;
					*(one_point+1) = REAL(.5) - SIN_TURN(angle*j-.375) * REAL(.5) * tv;
					if( def )
						def->apply( one_point, 1 );
					GOL::texcoord2v( one_point );

					*one_point = pCurVertex->x;
					*(one_point+1) = pCurVertex->y;
					*(one_point+2) = pCurVertex->z;
					if( def )
						def->apply( one_point, 1 );
					GOL::vertex3v( one_point);
				}
			}
			GOL::end();
			loc_normal += 1;
		}
	}
	else
	{
		for( INT32 i = 0; i < max; ++i )
		{
			pCurFace = &faces[levelModel.idxFaces + i];
			GOL::begin(GL_TRIANGLE_FAN);
			if ( color_mat )
				c_color_random::set();
			if ( use_normal )
				GOL::normal3v( &loc_normal->x);
			for( INT32 j = pCurFace->numEdges - 1; j >= 0; --j )
			{
				edgeIndex = edgeIndices[pCurFace->idxEdges + j];
				if(edgeIndex > 0)
				{
					pCurEdge = &edges[edgeIndex];
					pCurVertex = &vertices[pCurEdge->idxV0];
				}
				else
				{
					pCurEdge = &edges[-edgeIndex];
					pCurVertex = &vertices[pCurEdge->idxV1];
				}
				*one_point = pCurVertex->x;
				*(one_point+1) = pCurVertex->y;
				*(one_point+2) = pCurVertex->z;
				if( def )
					def->apply( one_point, 1 );
				GOL::vertex3v( one_point);
			}
			GOL::end();
			loc_normal += 1;
		}
	}
	if ( color_mat )
		c_color_random::end();
}

bool	quad_do_normal_std( c_point_xyz_real const * const a, c_point_xyz_real const * const b, c_point_xyz_real const * const c, c_point_xyz_real * n )
{
	REAL		x1,y1,z1;
	REAL		x2,y2,z2;
	REAL		x,y,z;
	REAL		size;

	x1 = b->x - a->x;
	y1 = b->y - a->y;
	z1 = b->z - a->z;

	x2 = c->x - a->x;
	y2 = c->y - a->y;
	z2 = c->z - a->z;

	x = z1*y2 - y1*z2;
	y = x1*z2 - z1*x2;
	z = y1*x2 - x1*y2;

	size = SQRT( x*x + y*y + z*z );

	n->x = x/size;
	n->y = y/size;
	n->z = z/size;

	return( size < .001 );	//todo this a dirty hack
}

STATUS bdd_quak_make_normals()
{
	BSPFACE* pCurFace = nullptr;
	BSPEDGE* pCurEdge = nullptr;
	long edgeIndex;
	int j;
	c_point_xyz_real *loc_normal;
	long	nb;
	c_point_xyz_real *a, *b, *c;
	bool	b_goon;

	IF_FREE_AND_NULL(normal);
	normal = ( c_point_xyz_real *)CALLOC( levelModel.numFaces, sizeof(c_point_xyz_real) );
	if( !normal )
	{
		BOX_ERR( "Error allocating face normal array." );
		return FAILURE;
	}

	loc_normal = normal;
	for( INT32 i = 0; i < levelModel.numFaces; ++i )
	{
		pCurFace = &faces[levelModel.idxFaces + i];
		nb = pCurFace->numEdges;

		edgeIndex = edgeIndices[pCurFace->idxEdges + 0];
		if( edgeIndex > 0 )
		{
			pCurEdge = &edges[edgeIndex];
			a = &vertices[pCurEdge->idxV0];
		}
		else
		{
			pCurEdge = &edges[-edgeIndex];
			a = &vertices[pCurEdge->idxV1];
		}
			
		j = 2;
		do
		{
			//if ( j != 2 )
				//	ERR_PRINT_STRING( "saloperie de poly" );
			edgeIndex = edgeIndices[pCurFace->idxEdges + j-1];
			if(edgeIndex > 0)
			{
				pCurEdge = &edges[edgeIndex];
				b = &vertices[pCurEdge->idxV0];
			}
			else
			{
				pCurEdge = &edges[-edgeIndex];
				b = &vertices[pCurEdge->idxV1];
			}

			edgeIndex = edgeIndices[pCurFace->idxEdges + j];
			if(edgeIndex > 0)
			{
				pCurEdge = &edges[edgeIndex];
				c = &vertices[pCurEdge->idxV0];
			}
			else
			{
				pCurEdge = &edges[-edgeIndex];
				c = &vertices[pCurEdge->idxV1];
			}
			b_goon = quad_do_normal_std( a, b, c, loc_normal);
			++j;
			if( j >= nb)
				break;
		}
		while( b_goon );

		++loc_normal;
	}
	return SUCCESS;

}


void	bdd_quak_reorganize()
{
	INT32		i;
	REAL		f;
	c_point_xyz_real* pvertex;

	REAL d250 = OVER_ONE_AS_REAL(250);
	pvertex = vertices;
	i = getNumVertices();
	for( ; i>0; --i )
	{
		pvertex->x = pvertex->x * d250 - REAL(3);
		f = -pvertex->y * d250 + REAL(5);
		pvertex->y = pvertex->z * d250;
		pvertex->z = f;
		++pvertex;
	}
}

void	bdd_quak_place()
{
	INT32	nb = getNumVertices();
	REAL	min[3];
	REAL	max[3];
		
	// find box
	min_max_v3r( min, max, (REAL*)vertices, nb );

	// translate and scale
	REAL scale = aaa::MAX( max[0]-min[0], max[1]-min[1], max[2]-min[2] );
	scale = REAL(10) / scale;

	center_v3r( min, min, max );

	c_point_xyz_real* pvertex = vertices;
	for( ; nb>0; --nb )
	{
		pvertex->x = (pvertex->x + min[0]) * scale;
		REAL y = pvertex->y;
		pvertex->y = (pvertex->z + min[2]) * scale;
		pvertex->z = -(y + min[1]) * scale;
		++pvertex;
	}
}


STATUS bdd_quak_load_level_data()
{
	STATUS status = loadLevelData();
	if ( status == SUCCESS)
	{
		bdd_quak_place();
		bdd_quak_make_normals();
		return SUCCESS;
	}
	else
		return FAILURE;
}
