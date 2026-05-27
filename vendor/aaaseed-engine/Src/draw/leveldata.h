

#ifdef AAA_LEVELDATA_H
#error "LEVELDATA_H included more than once."
#endif
#define AAA_LEVELDATA_H 1


/*---------------------------------------------------------\
|                                                          |
| leveldata.h                                              |
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
#ifndef AAA_BSPFILESTRUCTS_H
#   include "bspfilestructs.h"
#endif
#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

extern BSPPALETTEENTRY		qpalette[256];

extern BSPMODEL				levelModel;

extern BSPFACE*				faces;
extern UINT32				numFaces;

extern BSPTEXINFO*			texinfos;
extern short				numTexinfos;

extern BSPMIPTEXLUMP*		miptexlump;
extern long					sizeOfMiptexLump;


extern BSPEDGE*				edges;
extern long					numEdges;

extern long*				edgeIndices;
extern long					numEdgeIndices;

extern c_point_xyz_real*	vertices;
extern unsigned short		numVertices;


extern STATUS loadLevelData();
extern void destroyLevelData();
