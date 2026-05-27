/*
 * FTGL - OpenGL font library
 *
 * Copyright (c) 2001-2004 Henry Maddocks <ftgl@opengl.geek.nz>
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

#include <iostream>

#include "FTGL/ftgl.h"

#include "FTInternals.h"
#include "FTExtrudeGlyphImpl.h"
#include "FTVectoriser.h"

#if AAASEED()
#   include "gol/gol_draw.h"
#   include "gol/gol_list.h"
#	include "gol/gol_matrix.h"
#	include "draw/guf.h"
	namespace
	{
		c_guf	guf;
	}
#endif

//
//  FTGLExtrudeGlyph
//


FTExtrudeGlyph::FTExtrudeGlyph(FT_GlyphSlot glyph, float depth,
                               float frontOutset, float backOutset,
                               bool useDisplayList) :
    FTGlyph(new FTExtrudeGlyphImpl(glyph, depth, frontOutset, backOutset,
                                   useDisplayList))
{}


FTExtrudeGlyph::~FTExtrudeGlyph()
{}


const FTPoint& FTExtrudeGlyph::Render(const FTPoint& pen, int renderMode)
{
    FTExtrudeGlyphImpl *myimpl = dynamic_cast<FTExtrudeGlyphImpl *>(impl);
    return myimpl->RenderImpl(pen, renderMode);
}


//
//  FTGLExtrudeGlyphImpl
//


FTExtrudeGlyphImpl::FTExtrudeGlyphImpl(FT_GlyphSlot glyph, float _depth,
                                       float _frontOutset, float _backOutset,
                                       bool useDisplayList)
:   FTGlyphImpl(glyph),
    vectoriser(0),
    glList(0)
{
    bBox.SetDepth(-_depth);

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
    depth = _depth;
    frontOutset = _frontOutset;
    backOutset = _backOutset;

    if(useDisplayList)
    {
        glList = GOL::gen_lists(3);

        /* Front face */
        GOL::new_list( glList + 0, GL_COMPILE );
        RenderFront();
        GOL::end_list();

        /* Back face */
        GOL::new_list( glList + 1, GL_COMPILE );
        RenderBack();
        GOL::end_list();

        /* Side face */
        GOL::new_list( glList + 2, GL_COMPILE );
        RenderSide();
        GOL::end_list();

        delete vectoriser;
        vectoriser = NULL;
    }
}


FTExtrudeGlyphImpl::~FTExtrudeGlyphImpl()
{
    if(glList)
        GOL::delete_lists(glList, 3);
    else if(vectoriser)
    {
        delete vectoriser;
    }
}


const FTPoint& FTExtrudeGlyphImpl::RenderImpl(const FTPoint& pen,
                                              int renderMode)
{
	GOL::matrix::translate( pen.Xf(), pen.Yf(), pen.Zf() );
    if(glList)
    {
        if(renderMode & FTGL::RENDER_FRONT)
            GOL::call_list(glList + 0);
        if(renderMode & FTGL::RENDER_BACK)
            GOL::call_list(glList + 1);
        if(renderMode & FTGL::RENDER_SIDE)
            GOL::call_list(glList + 2);
    }
    else if(vectoriser)
    {
        if(renderMode & FTGL::RENDER_FRONT)
            RenderFront();
        if(renderMode & FTGL::RENDER_BACK)
            RenderBack();
        if(renderMode & FTGL::RENDER_SIDE)
            RenderSide();
    }
	GOL::matrix::translate_neg( pen.Xf(), pen.Yf(), pen.Zf());

    return advance;
}

void FTExtrudeGlyphImpl::RenderFront()
{
    vectoriser->MakeMesh( 1.0, 1, frontOutset );
    GOL::normal3v( unit_z_v4fp32 );

	__render_front_or_back( vectoriser, hscale, vscale, 0 );
}


void FTExtrudeGlyphImpl::RenderBack()
{
    vectoriser->MakeMesh( -1.0, 2, backOutset );
    GOL::normal3v( unit_z_neg_v4fp32 );

	__render_front_or_back( vectoriser, hscale, vscale, -depth );
}


void FTExtrudeGlyphImpl::RenderSide()
{
    int contourFlag = vectoriser->ContourFlag();

    FP32 hscale_over_1 = OVER_ONE_AS_FP32(hscale);
    FP32 vscale_over_1 = OVER_ONE_AS_FP32(vscale);

	auto const contour_nb = vectoriser->ContourCount();
    for( FTGL::VECTOR_SIZE_TYPE c = 0; c < contour_nb; ++c)
    {
        const FTContour* contour = vectoriser->Contour(c);
        auto const point_nb = contour->PointCount();
	
        if( point_nb >= 2 )
		{
			INT32 strip_point_nb = (point_nb+1)*2;
			guf.alloc_point( strip_point_nb, __FUNCTION__ );
			FP32* pt	= guf.get_point();
			FP32* uv	= guf.get_uv();
			FP32* nor	= guf.get_normal();

			for( FTGL::VECTOR_SIZE_TYPE j = 0; j <= point_nb; ++j )
			{
				FTGL::VECTOR_SIZE_TYPE cur = (j == point_nb) ? 0 : j;
				FTGL::VECTOR_SIZE_TYPE next = (cur == point_nb - 1) ? 0 : cur + 1;

				//todofont make it faster
				FTPoint frontPt = contour->FrontPoint(cur);
				FTPoint nextPt = contour->FrontPoint(next);
				FTPoint backPt = contour->BackPoint(cur);

				//toofontd better
				FTPoint normal = FTPoint( 0.f, 0.f, 1.f ) ^ (frontPt - nextPt);
				if( normal != FTPoint(0.0f, 0.0f, 0.0f) )
				{
					FTGL_DOUBLE x = normal.X();
					FTGL_DOUBLE y = normal.Y();
					FTGL_DOUBLE z = normal.Z();
					double norm = sqrt( x*x + y*y + z*z );
					if( norm == 0.0 )
					{
						nor[3] = nor[0] = FP32(0);
						nor[4] = nor[1] = FP32(0);
						nor[5] = nor[2] = FP32(0);
					}
					else
					{
						norm = 1. / norm;
						nor[3] = nor[0] = FP32(x * norm);
						nor[4] = nor[1] = FP32(y * norm);
						nor[5] = nor[2] = FP32(z * norm);
					}
				}
				else
				{
					nor[3] = nor[0] = 0.;
					nor[4] = nor[1] = 0.;
					nor[5] = nor[2] = 1.;
				}
				nor += 6;

				uv[2] = uv[0] = frontPt.Xf() * hscale_over_1;
				uv[3] = uv[1] = frontPt.Yf() * vscale_over_1;
				uv += 4;

				if( contourFlag & ft_outline_reverse_fill )
				{
					*pt++ = backPt.Xf()  * FTGL::size_factor_over;
					*pt++ = backPt.Yf()  * FTGL::size_factor_over;
					*pt++ = 0.;
					*pt++ = frontPt.Xf() * FTGL::size_factor_over;
					*pt++ = frontPt.Yf() * FTGL::size_factor_over;
					*pt++ = -depth;
				}
				else
				{
					*pt++ = backPt.Xf()  * FTGL::size_factor_over;
					*pt++ = backPt.Yf()  * FTGL::size_factor_over;
					*pt++ = -depth;
					*pt++ = frontPt.Xf() * FTGL::size_factor_over;
					*pt++ = frontPt.Yf() * FTGL::size_factor_over;
					*pt++ = 0.;
				}
			}
			guf.draw( GL_TRIANGLE_STRIP, strip_point_nb, true, true );
		}
    }
}

