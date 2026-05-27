#include "GLTTGlyphPolygonizerHandler.h"

#if AAA_TRUETYPE1_USE()

#include <stdio.h>
#include "gol/gol.h"

GLTTGlyphPolygonizerHandler::GLTTGlyphPolygonizerHandler( bool b_verbose )
{
	_b_verbose = b_verbose;
	_polygonizer = nullptr;
}

GLTTGlyphPolygonizerHandler::~GLTTGlyphPolygonizerHandler()
{}


void GLTTGlyphPolygonizerHandler::begin( INT32 type )
{
	GOL::begin( GLenum(type) );
}

void GLTTGlyphPolygonizerHandler::vertex( FTGlyphVectorizer::POINT* point )
{
	if( point == 0 )
		return;
	GOL::vertex2f( GLfloat(point->x), GLfloat(point->y) );
}

void GLTTGlyphPolygonizerHandler::end()
{
	GOL::end();
}

void GLTTGlyphPolygonizerHandler::error( INT32 error )
{
	if( ! _b_verbose )
		return;
	C_PCHAR str = (C_PCHAR)gluErrorString(GLenum(error));
	fprintf( stderr, "GLU error #%d (%s)\n", int(error), (str==0) ? "" : str );
}

void GLTTGlyphPolygonizerHandler::combine( DOUBLE coords[3], void *d[4], FP32 w[4], void **dataOut ) 
{
	 fprintf( stderr, "combine not implemented but needed\n" );
}

#endif	//#if AAA_TRUETYPE1_USE()
