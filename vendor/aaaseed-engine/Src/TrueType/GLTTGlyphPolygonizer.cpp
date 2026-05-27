

/*
WARNING: GLTTGlyphPolygonizer is *NOT* multi-thread safe! /SR
*/


#include "AAA_FTGlyph.h"

#if AAA_TRUETYPE1_USE()

#include <stdio.h>

#include "FTGlyphVectorizer.h"
#include "GLTTGlyphPolygonizer.h"
#include "GLTTGlyphPolygonizerHandler.h"

#include "gol/gol.h"


GLTTGlyphPolygonizer::GLTTGlyphPolygonizer( FTGlyphVectorizer* vectorizer /* = 0 */ )
{
	_glyph = nullptr;
	if( !vectorizer )
	{
		_vectorizer = new FTGlyphVectorizer;
		_b_own_vectorizer = true;
	}
	else
	{
		_vectorizer = vectorizer;
		_b_own_vectorizer = false;
	}
}

GLTTGlyphPolygonizer::~GLTTGlyphPolygonizer()
{
	if( _own_vectorizer )
		delete _vectorizer;
	_vectorizer = nullptr;
	_glyph = nullptr;
}

void GLTTGlyphPolygonizer::setPrecision( DOUBLE precision )
{
	_vectorizer->setPrecision( precision );
}

bool GLTTGlyphPolygonizer::init( AAA_FTGlyph*glyph )
{
	_glyph = glyph;

	if( !_own_vectorizer )
		return true;

	return _vectorizer->init(_glyph);
}

/////////////////////////////////////////////////////////////////////////////
// MMh, this code is *NOT* multi-thread safe!
// (we could synchronize the polygonize() function (in the Java way..)
// /SR
static GLTTGlyphPolygonizerHandler* handler= 0;

// Default OpenGL handler
static GLTTGlyphPolygonizerHandler* default_handler= 0;

// CALLBACK is for windoze users... not SGI ones!
// Thanks to G. Lanois (gerard@msi.com)
#if !defined(CALLBACK) && !defined(__WIN32__ ) && !defined(__WINDOWS__ )
#define CALLBACK
#endif

// IMHO, the (...) vs. (void) warning is due to GNU-C/C++.
#if defined(__GNUC__ ) || defined(_GNUG_ )
#define CALLBACKARG ...
#else
#define CALLBACKARG void
#endif

static void CALLBACK gltt_polygonizer_begin( GLenum type )
{
	if( handler != 0 )
		handler->begin(int(type));
}

static void CALLBACK gltt_polygonizer_vertex( void* data )
{
	if( handler != 0 )
		handler->vertex( (FTGlyphVectorizer::POINT*) data );
}

static void CALLBACK gltt_polygonizer_end()
{
	if( handler != 0 )
		handler->end();
}

static void CALLBACK gltt_polygonizer_error( GLenum error )
{
	if( handler != 0 )
		handler->error(int(error));
}

static void CALLBACK gltt_polygonizer_combine( double coords[3], void  *d[4], float w[4], void  **dataOut )
{
	if( handler != 0 )
		handler->combine( coords, d, w, dataOut );
}

/////////////////////////////////////////////////////////////////////////////

bool GLTTGlyphPolygonizer::polygonize( GLTTGlyphPolygonizerHandler* handle /* = 0 */ )
{
	if( !_glyph )
	{
		if( !_vectorizer )
			return false;

		_glyph = _vectorizer->getGlyph();
		if( !_glyph )
			return false;
	}

	if( !handle )
	{
		if( default_handler == 0 )
			default_handler= new GLTTGlyphPolygonizerHandler();
		handle = ::default_handler; // use the default OpenGL handler
	}

	GLUtriangulatorObj* tobj= gluNewTess();

	if( !tobj )
		return false;

	if( _own_vectorizer )
	{
		if( ! _vectorizer->vectorize() )
		{
			gluDeleteTess(tobj);
			return false;
		}
	}

	::handler= handle;
	handle->_polygonizer= this;

	// this time it should work /srh July 98
#if defined(WIN32) && !defined(__CYGWIN32__ )
	typedef void (CALLBACK *glu_callback)(CALLBACKARG);
#else
	typedef void CALLBACK (*glu_callback)(CALLBACKARG);
#endif

	gluTessCallback( tobj, GLenum(GLU_BEGIN),			(glu_callback) gltt_polygonizer_begin );
	gluTessCallback( tobj, GLenum(GLU_VERTEX),			(glu_callback) gltt_polygonizer_vertex );
	gluTessCallback( tobj, GLenum(GLU_END),				(glu_callback) gltt_polygonizer_end );
	gluTessCallback( tobj, GLenum(GLU_TESS_COMBINE),	(glu_callback) gltt_polygonizer_combine );
	gluTessCallback( tobj, GLenum(GLU_ERROR),			(glu_callback) gltt_polygonizer_error );

	int nContours= _vectorizer->getNContours();

	bool b_in_polygon = false;
	bool b_first_contour = false;

	for( int j= 0; j < nContours; ++j )
	{
		FTGlyphVectorizer::Contour* contour= _vectorizer->getContour(j);
		if( contour == 0 )
			continue;

		if( contour->b_clockwise )
		{
			if( b_in_polygon )
				gluEndPolygon(tobj);

			gluBeginPolygon(tobj);
			b_in_polygon = true;
			b_first_contour = true;
		}

		if( ! in_polygon )
			continue;

		if( b_first_contour )
		{
			gluNextContour( tobj, GLenum(GLU_EXTERIOR) );
			b_first_contour = false;
		}
		else
			gluNextContour( tobj, GLenum(GLU_INTERIOR) );

		int nPoints= contour->nPoints;

		for( int k= nPoints-1; k >= 0; --k )
		{
			FTGlyphVectorizer::POINT& p= contour->points[k];

			GLdouble data[3];
			data[0]= p.x;
			data[1]= p.y;
			data[2]= 0.;
			gluTessVertex( tobj, data, (void*) &(contour->points[k]) );
		}
	}

	if( in_polygon )
		gluEndPolygon(tobj);

	gluDeleteTess(tobj);

	handle->_polygonizer= 0;
	::handler= 0;

	return true;
}

DOUBLE GLTTGlyphPolygonizer::getBearingX() CONST
{
	return _vectorizer->getBearingX();
}

DOUBLE GLTTGlyphPolygonizer::getBearingY() CONST
{
	return _vectorizer->getBearingY();
}

DOUBLE GLTTGlyphPolygonizer::getAdvance() CONST
{
	return _vectorizer->getAdvance();
}

#endif //#if AAA_TRUETYPE1_USE()
