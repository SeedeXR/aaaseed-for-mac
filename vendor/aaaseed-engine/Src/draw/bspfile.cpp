//
// BspFile.c
//

//#define BSPFILETEST		// include this definition for testing this module

#include <stdlib.h>
#include "file/aaa_file.h"
#include <string.h>
#include "bspfile.h"

static	FILE*			bspFile = nullptr;
static	long			oldFP = 0;
static	BSPDIRECTORY	dir;
static	char			entityString[MAX_ENTITY_STR+1];

STATUS	openBSP( CONST char* CONST filename )
{
	STATUS result;

	if( bspFile )
		return FAILURE;

	bspFile = c_file::FOPEN(filename, "rb");
	
	if( bspFile )
	{
		result = getDirectory(&dir);
		if( result == SUCCESS )
			return SUCCESS;
		else
			return FAILURE;
	}
	else
		return FAILURE;
}

STATUS	closeBSP()
{
	int	retcode;
	if( bspFile )
	{
		retcode = c_file::FCLOSE(bspFile);
		bspFile = nullptr;
		if( retcode == 0 )
			return SUCCESS;
		else
			return FAILURE;
	}
	else
		return FAILURE;
}

STATUS	getDirectory( BSPDIRECTORY* pDirectory )
{
	// if a bsp file has been opened
	if( bspFile )
	{
		// save the old file pointer
		oldFP = c_file::FTELL(bspFile);
		// move the file pointer to the beginning of the file
		c_file::FSEEK_SET(bspFile, 0 );
		// read the directory into the callers structure
		size_t count = c_file::FREAD( pDirectory, sizeof(BSPDIRECTORY), 1, bspFile );
		// if the full directory was read
		if(count == 1)
		{
			// restore the old file pointer
			c_file::FSEEK_SET(bspFile, oldFP );
			// and return success
			return SUCCESS;
		}
		// else return failure
		else
			return FAILURE;
		}
	// else return failure
	else
		return FAILURE;
}

STATUS	getModel( BSPMODEL* pModel, short idxModel )
{
	// if a bsp file has been opened
	if( bspFile )
	{
		// save the old file pointer
		oldFP = c_file::FTELL(bspFile);
		// move the file pointer to the beginning of the model array
		c_file::FSEEK_SET(bspFile, dir.models.offset + idxModel * sizeof(BSPMODEL) );
		// read the model into the callers structure
		size_t count = c_file::FREAD( pModel, sizeof(BSPMODEL), 1, bspFile );
		// if the full model was read
		if(count == 1)
		{
			// restore the old file pointer
			c_file::FSEEK_SET(bspFile, oldFP );
			// and return success
			return SUCCESS;
		}
		// else return failure
		else
			return FAILURE;
	}
	// else return failure
	else
		return FAILURE;
}

STATUS	getFace( BSPFACE* pFace, unsigned short idxFace )
{
	// if a bsp file has been opened
	if( bspFile )
	{
		// save the old file pointer
		oldFP = c_file::FTELL(bspFile);
		// move the file pointer to the face
		c_file::FSEEK_SET(bspFile, dir.faces.offset + idxFace * sizeof(BSPFACE) );
		// read the face into the callers structure
		size_t count = c_file::FREAD( pFace, sizeof(BSPFACE), 1, bspFile );
		// if the full face was read
		if(count == 1)
		{
			// restore the old file pointer
			c_file::FSEEK_SET(bspFile, oldFP );
			// and return success
			return SUCCESS;
		}
		// else return failure
		else
			return FAILURE;
	}
	// else return failure
	else
		return FAILURE;
}

UINT32	getNumFaces()
{
	return dir.faces.size / sizeof(BSPFACE);
}

STATUS	getFaceArray( BSPFACE* faceArray )
{
	/////////////////////////////////////////////////////
	// NOTE: This function assumes that a suitable amount
	//       of memory has been allocated for the
	//       faceArray parameter.
	/////////////////////////////////////////////////////
	
	UINT32	numFaces;

	// get the number of faces in the bsp file
	numFaces = getNumFaces();
	// save the current file pointer
	oldFP = c_file::FTELL(bspFile);
	// set the file pointer to the beginning of the face array
	c_file::FSEEK_SET(bspFile, dir.faces.offset );
	// read the faces into the callers array
	size_t count = (INT32) c_file::FREAD( faceArray, sizeof(BSPFACE), numFaces, bspFile );
	// verify that all the faces were read
	if(count != numFaces)
		return FAILURE;
	// restore the old file pointer
	c_file::FSEEK_SET(bspFile, oldFP );
	return SUCCESS;
}

STATUS	getEdge( BSPEDGE* pEdge, long idxEdge )
{
	// if a bsp file has been opened
	if( bspFile )
	{
		// save the old file pointer
		oldFP = c_file::FTELL(bspFile);
		// move the file pointer to the edge
		c_file::FSEEK_SET(bspFile, dir.edges.offset + idxEdge * sizeof(BSPEDGE) );
		// read the edge into the callers structure
		size_t count = c_file::FREAD( pEdge, sizeof(BSPEDGE), 1, bspFile );
		// if the full edge was read
		if(count == 1)
		{
			// restore the old file pointer
			c_file::FSEEK_SET(bspFile, oldFP );
			// and return success
			return SUCCESS;
		}
		// else return failure
		else
			return FAILURE;
	}
	// else return failure
	else
		return FAILURE;
}

long	getNumEdges()
{
	return dir.edges.size / sizeof(BSPEDGE);
}

STATUS	getEdgeArray( BSPEDGE* edgeArray )
{
	/////////////////////////////////////////////////////
	// NOTE: This function assumes that a suitable amount
	//       of memory has been allocated for the
	//       edgeArray parameter.
	/////////////////////////////////////////////////////

	long	numEdges;

	// get the number of edges in the bsp file
	numEdges = getNumEdges();
	// save the current file pointer
	oldFP = c_file::FTELL(bspFile);
	// set the file pointer to the beginning of the edge array
	c_file::FSEEK_SET(bspFile, dir.edges.offset );
	// read the edges into the callers array
	size_t count = c_file::FREAD( edgeArray, sizeof(BSPEDGE), numEdges, bspFile );
	// verify that all the edges were read
	if(count != numEdges)
		return FAILURE;
	// restore the old file pointer
	c_file::FSEEK_SET(bspFile, oldFP );
	return SUCCESS;
}

STATUS	getFaceIndex( unsigned short* pidxFace, unsigned short idxFaceIndex )
{
	// if a bsp file has been opened
	if( bspFile )
	{
		// save the old file pointer
		oldFP = c_file::FTELL(bspFile);
		// move the file pointer to the face index
		c_file::FSEEK_SET(bspFile, dir.lfaces.offset + idxFaceIndex * sizeof(short) );
		// read the face index into the callers structure
		size_t count = c_file::FREAD( pidxFace, sizeof(short), 1, bspFile );
		// if the full edge was read
		if(count == 1)
		{
			// restore the old file pointer
			c_file::FSEEK_SET(bspFile, oldFP );
			// and return success
			return SUCCESS;
		}
		// else return failure
		else
			return FAILURE;
	}
	// else return failure
	else
		return FAILURE;
}

STATUS	getEdgeIndex( long* pidxEdge, long idxEdgeIndex )
{
	// if a bsp file has been opened
	if( bspFile )
	{
		// save the old file pointer
		oldFP = c_file::FTELL( bspFile);
		// move the file pointer to the edge index
		c_file::FSEEK_SET( bspFile, dir.ledges.offset + idxEdgeIndex * sizeof(long) );
		// read the edge index into the callers structure
		size_t count = c_file::FREAD( pidxEdge, sizeof(long), 1, bspFile );
		// if the full edge was read
		if(count == 1)
		{
			// restore the old file pointer
			c_file::FSEEK_SET( bspFile, oldFP );
			// and return success
			return SUCCESS;
		}
		// else return failure
		else
			return FAILURE;
	}
	// else return failure
	else
		return FAILURE;
}

long	getNumEdgeIndices()
{
	return dir.ledges.size / sizeof(long);
}

STATUS	getEdgeIndicesArray( long* edgeIndicesArray )
{
	/////////////////////////////////////////////////////
	// NOTE: This function assumes that a suitable amount
	//       of memory has been allocated for the
	//       edgeIndicesArray parameter.
	/////////////////////////////////////////////////////

	long	numEdgeIndices;

	// get the number of edge indices in the bsp file
	numEdgeIndices = getNumEdgeIndices();
	// save the current file pointer
	oldFP = c_file::FTELL(bspFile);
	// set the file pointer to the beginning of the edge indices array
	c_file::FSEEK_SET( bspFile, dir.ledges.offset );
	// read the edge indices into the callers array
	size_t count = c_file::FREAD( edgeIndicesArray, sizeof(long), numEdgeIndices, bspFile );
	// verify that all the edge indices were read
	if(count != numEdgeIndices)
		return FAILURE;
	// restore the old file pointer
	c_file::FSEEK_SET( bspFile, oldFP );
	return SUCCESS;
}

STATUS getVertex( c_point_xyz_real* pVertex, unsigned short idxVertex )
{
	// if a bsp file has been opened
	if( bspFile )
	{
		// save the old file pointer
		oldFP = c_file::FTELL(bspFile);
		// move the file pointer to the vertex
		c_file::FSEEK_SET( bspFile, dir.vertices.offset + idxVertex * sizeof(c_point_xyz_real) );
		// read the vertex into the callers structure
		size_t count = c_file::FREAD( pVertex, sizeof(c_point_xyz_real), 1, bspFile );
		// if the full vertex was read
		if(count == 1)
		{
			// restore the old file pointer
			c_file::FSEEK_SET( bspFile, oldFP );
			// and return success
			return SUCCESS;
		}
		// else return failure
		else
			return FAILURE;
	}
	// else return failure
	else
		return FAILURE;
}

UINT32	getNumVertices()
{
	return dir.vertices.size / sizeof(c_point_xyz_real);
}

STATUS	getVerticesArray( c_point_xyz_real* verticesArray )
{
	/////////////////////////////////////////////////////
	// NOTE: This function assumes that a suitable amount
	//       of memory has been allocated for the
	//       verticesArray parameter.
	/////////////////////////////////////////////////////

	unsigned short	numVertices;

	// get the number of vertices in the bsp file
	numVertices = getNumVertices();
	// save the current file pointer
	oldFP = c_file::FTELL(bspFile);
	// set the file pointer to the beginning of the vertices array
	c_file::FSEEK_SET( bspFile, dir.vertices.offset );
	// read the vertices into the callers array
	size_t count = c_file::FREAD( verticesArray, sizeof(c_point_xyz_real), numVertices, bspFile );
	// verify that all the vertices were read
	if(count != numVertices)
		return FAILURE;
	// restore the old file pointer
	c_file::FSEEK_SET( bspFile, oldFP );

	return SUCCESS;
}

/*
STATUS getPlayerStartCoords(c_point_xyz_real* pCoords)
{	
	char* p;
	int x, y, z;

	if( bspFile )
	{
		// save the old file pointer
		oldFP = c_file::FTELL(bspFile);
		// move the file pointer to the start of the entity lump
		c_file::FSEEK_SET( bspFile, dir.entities.offset );
		// read the entire entity string into the array
		size_t count = c_file::FREAD(entityString, sizeof(char), dir.entities.size, bspFile);
		if(count != (unsigned int)dir.entities.size)
			return FAILURE;
		// add the null character at the end of the array
		entityString[MAX_ENTITY_STR] = '\0';
		// find the coords
		p = strstr(entityString, "info_player_start");
		p = strstr(p, "origin");
		p = strstr(p, "\"");
		p = strstr(p, "\"");
		++p;
		sscanf(p, "%d %d %d", &x, &y, &z);
		pCoords->x = (float)x; pCoords->y = (float)y; pCoords->z = (float)z;
		return SUCCESS;
	}
	else
		return FAILURE;
}
*/

/*STATUS getMiptexLumpHeader(BSPMIPTEXLUMPHEADER* pHeader)
{

}*/

STATUS	getTexinfo(BSPTEXINFO* pTexinfo, short idxTexinfo)
{
	// if a bsp file has been opened
	if( bspFile )
	{
		// save the old file pointer
		oldFP = c_file::FTELL(bspFile);
		// move the file pointer to the texinfo
		c_file::FSEEK_SET( bspFile, dir.texinfo.offset + idxTexinfo * sizeof(BSPTEXINFO) );
		// read the vertex into the callers structure
		size_t count = c_file::FREAD(pTexinfo, sizeof(BSPTEXINFO), 1, bspFile);
		// if the full vertex was read
		if(count == 1)
		{
			// restore the old file pointer
			c_file::FSEEK_SET(bspFile, oldFP );
			// and return success
			return SUCCESS;
		}
		// else return failure
		else
			return FAILURE;
	}
	// else return failure
	else
		return FAILURE;
}

INT32	getNumTexinfos()
{
	return dir.texinfo.size / sizeof(BSPTEXINFO);
}

STATUS	getTexinfoArray( BSPTEXINFO* texinfoArray )
{
	/////////////////////////////////////////////////////
	// NOTE: This function assumes that a suitable amount
	//       of memory has been allocated for the
	//       texinfoArray parameter.
	/////////////////////////////////////////////////////

	short	numTexinfos;

	// get the number of texinfos in the bsp file
	numTexinfos = getNumTexinfos();
	// save the current file pointer
	oldFP = c_file::FTELL(bspFile);
	// set the file pointer to the beginning of the texinfo array
	c_file::FSEEK_SET(bspFile, dir.texinfo.offset );
	// read the texinfos into the callers array
	size_t count = c_file::FREAD( texinfoArray, sizeof(BSPTEXINFO), numTexinfos, bspFile );
	// verify that all the vertices were read
	if(count != numTexinfos)
		return FAILURE;
	// restore the old file pointer
	c_file::FSEEK_SET( bspFile, oldFP );
	return SUCCESS;
}

long	getSizeOfMiptexLump()
{
	return dir.miptex.size;
}

STATUS	getMiptexLump(BSPMIPTEXLUMP* pMiptexLump)
{
	/////////////////////////////////////////////////////
	// NOTE: This function assumes that a suitable amount
	//       of memory has been allocated for the
	//       miptexLump parameter.
	/////////////////////////////////////////////////////

	long	sizeOfMiptexLump;

	// get the size of the miptex lump
	sizeOfMiptexLump = getSizeOfMiptexLump();
	// save the current file pointer
	oldFP = c_file::FTELL(bspFile);
	// set the file pointer to the beginning of the miptex lump
	c_file::FSEEK_SET(bspFile, dir.miptex.offset );
	// read the miptex lump into the callers array
	size_t count = c_file::FREAD( pMiptexLump, sizeOfMiptexLump, 1, bspFile );
	// verify that the lump was read
	if(count != 1)
		return FAILURE;
	// restore the old file pointer
	c_file::FSEEK_SET( bspFile, oldFP );
	return SUCCESS;
}


////////////////////////////////
// TESTING CODE
////////////////////////////////
#define AAA_LOCAL_BSPFILETEST() 0

#if AAA_LOCAL_BSPFILETEST()
void main()
{
	STATUS result;

	result = openBSP("\\QUAKE\\ID1\\MAPS\\E1M1.BSP");
	if(result == FAILURE)
	{
		PRINT_STRING("Error opening bsp file!\n");
		exit(1);
	}

	/*
	{
		BSPMIPTEXLUMP* miptexlump = nullptr;
		long sizeOfMiptexLump = 0;
		sizeOfMiptexLump = getSizeOfMiptexLump();
		miptexlump = (BSPMIPTEXLUMP*)CALLOC(1, sizeOfMiptexLump);
		if( !miptexlump )
			exit(1);
		getMiptexLump(miptexlump);
		closeBSP();
	}*/

	/*{
		BSPTEXINFO texinfo;
		short numTexinfos;
		numTexinfos = getNumTexinfos();
		getTexinfo(&texinfo, 0);
		closeBSP();
	}*/

	/*{BSPTEXINFO* texinfos = nullptr;
	short numTexinfos = 0;
	numTexinfos = getNumTexinfos();
	texinfos = (BSPTEXINFO*)CALLOC(numTexinfos, sizeof(BSPTEXINFO));
	if( !texinfos )
		exit(1);
	getTexinfoArray(texinfos);}*/
	
	/*
	POINT_3R* vertices = nullptr;
	unsigned short numVertices = 0;
	numVertices = getNumVertices();
	vertices = (c_point_xyz_real*)CALLOC(numVertices, sizeof(c_point_xyz_real));
	if( !vertices )
		exit(1);
	getVerticesArray(vertices);
	*/

	/*
	long* edgeIndices = nullptr;
	long numEdgeIndices = 0;
	numEdgeIndices = getNumEdgeIndices();
	edgeIndices = (long*)CALLOC(numEdgeIndices, sizeof(long));
	if( !edgeIndices  )
		exit(1);
	getEdgeIndicesArray(edgeIndices);
	*/

	/*
	BSPEDGE* edges = nullptr;
	long numEdges = 0;
	numEdges = getNumEdges();
	edges = (BSPEDGE*)CALLOC(numEdges, sizeof(BSPEDGE));
	if( !edges )
		exit(1);
	getEdgeArray(edges);
	*/

	/*
	BSPFACE* faces = nullptr;
	unsigned short numFaces = 0;
	numFaces = getNumFaces();
	faces = (BSPFACE*)CALLOC(numFaces, sizeof(BSPFACE));
	if( !faces )
		exit(1);
	getFaceArray(faces);
	*/

	/*
	long numVertices = 0;
	numVertices = getNumVertices();
	*/
	
	/*
	long numEdgeIndices = 0;
	numEdgeIndices = getNumEdgeIndices();
	*/

	/*
	long numEdges = 0;
	numEdges = getNumEdges();
	*/

	/*
	unsigned short numFaces = 0;
	numFaces = getNumFaces();
	*/

	/*
	BSPDIRECTORY directory;
	getDirectory(&directory);
	*/
	
	/*
	BSPMODEL model;
	getModel(&model, 0);
	*/
	
	/*
	BSPFACE face;
	getFace(&face, 0);
	*/

	/*
	unsigned short faceIndex;
	getFaceIndex(&faceIndex, 3);
	*/

	/*
	long edgeIndex;
	getEdgeIndex(&edgeIndex, 6);
	*/

	/*
	c_point_xyz_real start;
	getPlayerStartCoords(&start);
	*/
	
}

#endif	
////////////////////////////////
// END TESTING CODE
////////////////////////////////
