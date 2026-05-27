/*---------------------------------------------------------\
|                                                          |
| leveldata.c                                              |
|                                                          |
| by Joel Beaudoin                                         |
|                                                          |
|----------------------------------------------------------|
|                                                          |
| This module contains routines for loading the necessary  |
| information from a Quake BSP file. The data loaded has   |
| global scope for now.                                    |
|                                                          |
\---------------------------------------------------------*/


#include "leveldata.h"
#include "bspfile.h"
#include "file/aaa_file.h"
#include "err.h"

float	randUnitScalar();
void	loadPalette();

/*---------------------------------------------------------------*/
/*------------------\
|                   |
| GLOBAL LEVEL DATA |
|                   |
\------------------*/

BSPMODEL			levelModel;
BSPPALETTEENTRY		qpalette[256];

BSPFACE*			faces = nullptr;
UINT32				numFaces = 0;

BSPTEXINFO*			texinfos = nullptr;
short				numTexinfos = 0;

BSPMIPTEXLUMP*		miptexlump = nullptr;
long				sizeOfMiptexLump = 0;

BSPEDGE*			edges = nullptr;
long				numEdges = 0;

long*				edgeIndices = nullptr;
long				numEdgeIndices = 0;

c_point_xyz_real*	vertices = nullptr;
unsigned short		numVertices = 0;

/*----------------------\
|                       |
| END GLOBAL LEVEL DATA |
|                       |
\----------------------*/
/*---------------------------------------------------------------*/
STATUS loadLevelData()
{
	getModel(&levelModel, 0);
	
	numFaces = getNumFaces();
	faces = (BSPFACE*)CALLOC(numFaces, sizeof(BSPFACE));
	if( !faces )
	{
		BOX_ERR("Error allocating faces array.");
		return FAILURE;
	}
	getFaceArray(faces);

	numTexinfos = getNumTexinfos();
	texinfos = (BSPTEXINFO*)CALLOC(numTexinfos, sizeof(BSPTEXINFO));
	if( !texinfos )
	{
		BOX_ERR("Error allocating texinfos array.");
		return FAILURE;
	}
	getTexinfoArray(texinfos);
	
	sizeOfMiptexLump = getSizeOfMiptexLump();
	miptexlump = (BSPMIPTEXLUMP*)CALLOC(1, sizeOfMiptexLump);
	if( !miptexlump )
	{
		BOX_ERR("Error allocating miptex lump.");
		return FAILURE;
	}
	getMiptexLump(miptexlump);

	//loadPalette();
		
	numEdges = getNumEdges();
	edges = (BSPEDGE*)CALLOC(numEdges, sizeof(BSPEDGE));
	if( !edges )
	{
		BOX_ERR("Error allocating edges array.");
		return FAILURE;
	}
	getEdgeArray(edges);

	numEdgeIndices = getNumEdgeIndices();
	edgeIndices = (long*)CALLOC(numEdgeIndices, sizeof(long));
	if( !edgeIndices )
	{
		BOX_ERR("Error allocating edge indices array.");
		return FAILURE;
	}
	getEdgeIndicesArray(edgeIndices);

	numVertices = getNumVertices();
	vertices = (c_point_xyz_real*)CALLOC(numVertices, sizeof(c_point_xyz_real));
	if( !vertices )
	{
		BOX_ERR("Error allocating vertices array.");
		return FAILURE;
	}
	getVerticesArray(vertices);

	return SUCCESS;
}

/*---------------------------------------------------------------*/

float	randUnitScalar()
{
	float	num;

	//srand((unsigned)time( nullptr ));
	num = ((float)rand()) / ((float)(RAND_MAX+1));
	return num;
}

/*---------------------------------------------------------------*/

void	loadPalette()
{
	FILE*	palettefile = c_file::FOPEN("\\QUAKE\\ID1\\GFX\\PALETTE.LMP", "rb");
	if( palettefile )
	{
		c_file::FREAD(qpalette, 3*256, 1, palettefile);
		c_file::FCLOSE(palettefile);
	}
	else
	{
		BOX_ERR("Error loading palette.");
		exit(1);
	}
}

/*---------------------------------------------------------------*/

void	destroyLevelData()
{
	IF_FREE_AND_NULL(faces);
	IF_FREE_AND_NULL(texinfos);
	IF_FREE_AND_NULL(miptexlump);
	IF_FREE_AND_NULL(edges);
	IF_FREE_AND_NULL(edgeIndices);
	IF_FREE_AND_NULL(vertices);
}
