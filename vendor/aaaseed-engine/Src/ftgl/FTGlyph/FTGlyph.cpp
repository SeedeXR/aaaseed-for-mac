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
#include "aaa_util.h"
#include "FTGL/ftgl.h"

#include "FTInternals.h"
#include "FTGlyphImpl.h"
#include "FTVectoriser.h"

#if AAASEED()
#   include "gol/gol_draw.h"
#	include "draw/guf.h"
	namespace
	{
		c_guf	guf;
	}
#endif

//
//  FTGlyph
//


FTGlyph::FTGlyph(FT_GlyphSlot glyph)
{
    impl = new FTGlyphImpl(glyph);
}


FTGlyph::FTGlyph(FTGlyphImpl *pImpl)
{
    impl = pImpl;
}


FTGlyph::~FTGlyph()
{
    delete impl;
}


float FTGlyph::Advance() const
{
    return impl->Advance();
}


const FTBBox& FTGlyph::BBox() const
{
    return impl->BBox();
}


FT_Error FTGlyph::Error() const
{
    return impl->Error();
}


//
//  FTGlyphImpl
//


FTGlyphImpl::FTGlyphImpl(FT_GlyphSlot glyph, bool useList) : err(0)
{
    (void)useList;

    if(glyph)
    {
        bBox = FTBBox(glyph);
        advance = FTPoint(	glyph->advance.x * FTGL::size_factor_over,
							glyph->advance.y * FTGL::size_factor_over );
    }
}


FTGlyphImpl::~FTGlyphImpl()
{}


float FTGlyphImpl::Advance() const
{
    return advance.Xf();
}


const FTBBox& FTGlyphImpl::BBox() const
{
    return bBox;
}


FT_Error FTGlyphImpl::Error() const
{
    return err;
}


void FTGlyphImpl::__render_front_or_back( FTVectoriser * CONST vectoriser, INT32 CONST scale_u, INT32 CONST scale_v, FP32 CONST z )
{
	FP32 scale_u_over_1 = OVER_ONE_AS_FP32(scale_u);
    FP32 scale_v_over_1 = OVER_ONE_AS_FP32(scale_v);

    const FTMesh *mesh = vectoriser->GetMesh();
	auto const tess_nb = mesh->TesselationCount();
    for( UINT32 j = 0; j < tess_nb; ++j )
    {
        const FTTesselation* subMesh = mesh->Tesselation(j);
        unsigned int polygonType = subMesh->PolygonType();
		auto const point_nb = subMesh->PointCount();
		guf.alloc_point( point_nb, __FUNCTION__ );
		FP32* pt = guf.get_point();
		FP32* uv = guf.get_uv();

		for( UINT32 i = 0; i < point_nb; ++i )
		{
			FTPoint point = subMesh->Point(i);
			*uv++ = point.Xf() * scale_u_over_1;
			*uv++ = point.Yf() * scale_v_over_1;
			*pt++ = point.Xf() * FTGL::size_factor_over;
			*pt++ = point.Yf() * FTGL::size_factor_over;
			*pt++ = z;
		}
		guf.draw( polygonType, point_nb, false, true );

        //todofont use modern call like gbuf
/*        GOL::begin( polygonType );

            for( UINT32 i = 0; i < point_nb; ++i )
            {
                FTPoint pointt = subMesh->Point(i);
                GOL::texcoord2( pointt.Xf() * scale_u_over_1,			pointt.Yf() * scale_v_over_1			);
                GOL::vertex3f(  pointt.Xf() * FTGL_size_factor_over,	pointt.Yf() * FTGL_size_factor_over,    z	);
            }
        GOL::end()*/;
    }
}
