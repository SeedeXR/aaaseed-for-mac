
#ifdef AAA_BSPFILESTRUCTS_H
#error "BSPFILESTRUCTS_H included more than once."
#endif
#define AAA_BSPFILESTRUCTS_H 1


//
// BspFileStructs.h
//
#ifndef AAA_V_BASE_H
#	include "math/v_base.h"
#endif


#define MAX_ENTITY_STR 65536

// the return value of CBspFile member functions
enum STATUS : INT32
{
	SUCCESS,
	FAILURE
};

typedef struct BSPBBOXFLOAT
{ 
	c_point_xyz_real   min;		// minimum values of X,Y,Z
	c_point_xyz_real   max;		// maximum values of X,Y,Z
} BSPBBOXFLOAT;					// bounding box

typedef struct BSPBBOXSHORT
{ 
	short   min;				// minimum values of X,Y,Z
	short   max;				// maximum values of X,Y,Z
} BSPBBOXSHORT;					// bounding box

typedef struct BSPPALETTEENTRY
{
	unsigned char red;
	unsigned char green;
	unsigned char blue;
} BSPPALETTEENTRY;

// directory entry structure
typedef struct BSPDIRENTRY
{ 
	long  offset;					// Offset to entry, in bytes, from start of file
	long  size;						// Size of entry in file, in bytes
} BSPDIRENTRY;

// directory - found at the beginning of the file
//
// Note: Edges and faces each seemingly have two entries in the
//       directory; however, one of the entries is just a list
//       of indices into the actual array of edges or faces.
//		 The list of indices is used by a face or a leaf and
//		 is there to avoid storing the same data for edges and
//		 faces that are used by more than one face and leaf.
// 
typedef struct BSPDIRECTORY
{
	long		version;            // Model version, must be 0x17 (23).
	BSPDIRENTRY	entities;           // List of Entities.
	BSPDIRENTRY	planes;             // Map Planes.
									// numplanes = size/sizeof(plane_t)
	BSPDIRENTRY	miptex;             // Wall Textures.
	BSPDIRENTRY	vertices;           // Map Vertices.
									// numvertices = size/sizeof(vertex_t)
	BSPDIRENTRY	visilist;           // Leaves Visibility lists.
	BSPDIRENTRY	nodes;              // BSP Nodes.
									// numnodes = size/sizeof(node_t)
	BSPDIRENTRY	texinfo;            // Texture Info for faces.
									// numtexinfo = size/sizeof(texinfo_t)
	BSPDIRENTRY	faces;              // Faces of each surface.
									// numfaces = size/sizeof(face_t)
	BSPDIRENTRY	lightmaps;          // Wall Light Maps.
	BSPDIRENTRY	clipnodes;          // clip nodes, for Models.
									// numclips = size/sizeof(clipnode_t)
	BSPDIRENTRY	leaves;             // BSP Leaves.
									// numlaves = size/sizeof(leaf_t)
	BSPDIRENTRY	lfaces;             // list of face indices
	BSPDIRENTRY	edges;              // Edges of Faces.
									// numedges = Size/sizeof(edge_t)
	BSPDIRENTRY	ledges;             // list of edge indices
	BSPDIRENTRY	models;             // static portions of the level
									// nummodels = Size/sizeof(model_t)
} BSPDIRECTORY;

// model - a static object
typedef struct BSPMODEL
{
	BSPBBOXFLOAT		bbox;			// The bounding box of the Model
	c_point_xyz_real	origin;         // Origin of model, usually (0,0,0)
	long				idxNode0;		// Index of first BSP node
	long				idxNode1;		// Index of the first Clip node
	long				idxNode2;		// Index of the second Clip node
	long				idxNode3;		// Usually zero
	long				numLeafs;		// Number of BSP leaves
	long				idxFaces;		// Index of the first face for this model
	long				numFaces;		// Number faces following idxFaces that
									// are part of this model.
} BSPMODEL;

// face
//
// Note: The edges for a face are indirectly referenced through
//       the edge list. The idxEdge member is the first index
//       in an array of consecutive indices to edges. Remember
//       that the actual edges in the edge array may not be
//       consecutive.
//.
typedef struct BSPFACE
{ 
	short	idxPlane;				// The plane in which the face lies
									// must be in [0,numPlanes] 
	short	side;					// 0 if in front of the plane, 1 if behind the plane
	long	idxEdges;				// first edge in the List of edges
									// must be in [0,numEdges]
	short	numEdges;				// number of edges in the List of edges
	short	idxTexinfo;				// index of the Texture info the face is part of
									// must be in [0,numtexinfos] 
	unsigned char	typelight;      // type of lighting, for the face
	unsigned char	baselight;      // from 0xFF (dark) to 0 (bright)
	unsigned char	light[2];       // two additional light models  
	long			idxLightmap;	// index inside the general light map, or -1
									// this define the start of the face light map
} BSPFACE;

// edge
typedef struct BSPEDGE
{
	unsigned short idxV0;			// index of the start vertex
									// must be in [0,numvertices]
	unsigned short idxV1;			// index of the end vertex
									// must be in [0,numvertices]
} BSPEDGE;

/*
 * texinfo
 */
typedef struct BSPTEXINFO
{
	c_point_xyz_real	s;
	REAL				offsetS;
	c_point_xyz_real	t;
	REAL				offsetT;
	long				idxTexture;
	long				flags;
} BSPTEXINFO;

typedef struct BSPMIPTEXLUMP
{
	long numMiptex;
	long offsets[4];		// an array of numMiptex offsets from BSPMIPTEXLUMP
							// to each individual BSPMIPTEX
} BSPMIPTEXLUMP;

#define MIPLEVELS 4

typedef struct BSPMIPTEX
{
	char name[16];
	unsigned long width,
				  height;
	unsigned long offsets[MIPLEVELS];	/* immediately following each miptex
										   header is the data for the mipmaps */
} BSPMIPTEX;

