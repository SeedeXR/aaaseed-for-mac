
#ifdef AAA_BSPFILE_H
#error "BSPFILE_H included more than once."
#endif
#define AAA_BSPFILE_H 1



//
// BspFile.h
//

#ifndef	AAA_BSPFILESTRUCTS_H
#	include "bspfilestructs.h"
#endif
		
extern	STATUS openBSP( CONST char* CONST filename);
extern	STATUS closeBSP();
extern	STATUS getDirectory(BSPDIRECTORY* pDirectory);
		
extern	STATUS getModel(BSPMODEL* pModel, short idxModel);
		
//
// FACE METHODS
//
extern	STATUS getFace(BSPFACE* pFace, unsigned short idxFace);
extern	UINT32 getNumFaces();
extern	STATUS getFaceArray(BSPFACE* faceArray);
		
//
// EDGE METHODS
//
extern	STATUS getEdge(BSPEDGE* pEdge, long idxEdge);
extern	long getNumEdges();
extern	STATUS getEdgeArray(BSPEDGE* edgeArray);
		
//
// FACE INDEX METHODS
//
extern	STATUS getFaceIndex(unsigned short* pidxFace, unsigned short idxFaceIndex);
		
//
// EDGE INDEX METHODS
//
extern	STATUS getEdgeIndex(long* pidxEdge, long idxEdgeIndex);
extern	long getNumEdgeIndices();
extern	STATUS getEdgeIndicesArray(long* edgeIndicesArray);
		
//
// VERTEX METHODS
//
extern	STATUS getVertex(c_point_xyz_real* pVertex, unsigned short idxVertex);
extern	UINT32 getNumVertices();
extern	STATUS getVerticesArray(c_point_xyz_real* verticesArray);

//
// ENTITY METHODS
//
extern	STATUS getPlayerStartCoords(c_point_xyz_real* pCoords);

//
// MIPTEX METHODS
//
extern long getSizeOfMiptexLump();
extern STATUS getMiptexLump(BSPMIPTEXLUMP* pHeader);

/*
 * TEXINFO METHODS
 */
extern STATUS getTexinfo(BSPTEXINFO* pTexinfo, short idxTexinfo);
extern INT32 getNumTexinfos();
extern STATUS getTexinfoArray(BSPTEXINFO* texinfoArray);
