/*
 * FTGL - OpenGL font library
 *
 * Copyright (c) 2001-2004 Henry Maddocks <ftgl@opengl.geek.nz>
 * Copyright (c) 2008 Éric Beets <ericbeets@free.fr>
 * Copyright (c) 2008 Sam Hocevar <sam@hocevar.net>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef AAA_FTGL_CONFIG_H
#   include "ftgl_config.h"
#endif

#include "FTGL/ftgl.h"

#include "FTInternals.h"
#include "FTPolygonGlyphImpl.h"
#include "FTVectoriser.h"

#include "gol/gol_list.h"
#include "gol/gol_matrix.h"


//
//  FTGLPolyGlyph
//


FTPolygonGlyph::FTPolygonGlyph(FT_GlyphSlot glyph, float outset,
                               bool useDisplayList) :
    FTGlyph(new FTPolygonGlyphImpl(glyph, outset, useDisplayList))
{}


FTPolygonGlyph::~FTPolygonGlyph()
{}


const FTPoint& FTPolygonGlyph::Render(const FTPoint& pen, int renderMode)
{
    FTPolygonGlyphImpl *myimpl = dynamic_cast<FTPolygonGlyphImpl *>(impl);
    return myimpl->RenderImpl(pen, renderMode);
}


//
//  FTGLPolyGlyphImpl
//


FTPolygonGlyphImpl::FTPolygonGlyphImpl(FT_GlyphSlot glyph, float _outset,
                                       bool useDisplayList)
:   FTGlyphImpl(glyph),
    vectoriser(0),
    glList(0)
{
    if(ft_glyph_format_outline != glyph->format)
    {
        err = 0x14; // Invalid_Outline
        return;
    }

    vectoriser = new FTVectoriser(glyph);

    if((vectoriser->ContourCount() < 1) || (vectoriser->PointCount() < 3))
    {
        delete vectoriser;
        vectoriser = NULL;
        return;
    }

	hscale = INT32(glyph->face->size->metrics.x_ppem * FTGL::size_factor);
    vscale = INT32(glyph->face->size->metrics.y_ppem * FTGL::size_factor);
    outset = _outset;

    if(useDisplayList)
    {
        glList = GOL::gen_lists(1);
        GOL::new_list(glList, GL_COMPILE);

        DoRender();

        GOL::end_list();

        delete vectoriser;
        vectoriser = NULL;
    }
}


FTPolygonGlyphImpl::~FTPolygonGlyphImpl()
{
    if(glList)
        GOL::delete_lists(glList, 1);
    else if(vectoriser)
    {
        delete vectoriser;
    }
}


const FTPoint& FTPolygonGlyphImpl::RenderImpl(const FTPoint& pen,
                                              int renderMode)
{
    (void)renderMode;

    GOL::matrix::translate( pen.Xf(), pen.Yf(), pen.Zf() );
    if(glList)
    {
        GOL::call_list(glList);
    }
    else if(vectoriser)
    {
        DoRender();
    }
     GOL::matrix::translate( -pen.Xf(), -pen.Yf(), -pen.Zf() );

    return advance;
}


void FTPolygonGlyphImpl::DoRender()
{
    vectoriser->MakeMesh( 1.0, 1, outset );
	__render_front_or_back( vectoriser, hscale, vscale, 0. );
}
