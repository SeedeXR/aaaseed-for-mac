// src/text/glyph_atlas.h
//
// CPU-side glyph atlas builder for the .app debug HUD + future text-
// rendering MEUs. Phase 5 third-beachhead consumer (continuation 58).
//
// Pure CPU code -- no GPU upload here. Returns a packed alpha-8 bitmap
// plus per-glyph UV rects + advance metrics. The HUD plumbing (in a
// later continuation) will :
//   1. Call this once at .app boot.
//   2. Upload the alpha-8 bitmap to an MTLTexture (1-channel) via
//      MetalBackend::texture_data_2d.
//   3. Per frame, build a vertex buffer of textured quads (one per
//      character of "FPS=X frame=N") sampling the atlas with the
//      stored UVs.
//
// Why pure CPU here : keeps testing simple (no GPU dependency), keeps
// the helper reusable for headless tools (asset bake, glyph audits),
// and matches the engine's tradition of separating "build the data"
// from "upload + draw it" (`Src/draw/` is full of that pattern).
//
// Doctrine reminder : stb_truetype is the Mac-side font beachhead
// (continuation 57). Windows uses freetype + SdfText ; this helper
// is Mac-only by virtue of being unused on Windows.

#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace aaa { namespace text {

//	ASCII range packed by the helper. Printable characters only :
//	space (32) through tilde (126). 95 glyphs total. Sufficient for
//	a debug HUD ; future MEU work that needs Unicode picks a different
//	atlas builder.
constexpr int kFirstAsciiChar = 32;
constexpr int kLastAsciiChar  = 126;
constexpr int kNumAsciiChars  = kLastAsciiChar - kFirstAsciiChar + 1;

//	Per-glyph layout. Coordinates are in normalized atlas-UV space
//	[0,1] x [0,1] so the shader can sample without knowing the
//	atlas dimensions. Pixel offsets + advance are in EM-scaled pixels
//	at the build-time font size ; the HUD divides by the .app's
//	target font size to scale at draw time.
struct GlyphMetric
{
    float u0, v0, u1, v1;   //  UV rect in the atlas
    float xoff, yoff;       //  pixel offset from cursor to top-left of quad
    float xadvance;         //  pixel advance to the next glyph cursor pos
};

//	The atlas itself. Pixels are alpha-8 (one byte per texel, value =
//	coverage). The HUD uploads this with MTLPixelFormatA8Unorm OR
//	converts to RGBA8 first ; either works.
struct GlyphAtlas
{
    int                          width        = 0;
    int                          height       = 0;
    std::vector< std::uint8_t >  alpha8;             //  width * height bytes
    GlyphMetric                  glyphs[ kNumAsciiChars ] = {};
    //	Font-wide vertical metrics, all in pixels at the build-time size.
    float                        ascent       = 0.0f;
    float                        descent      = 0.0f;
    float                        line_advance = 0.0f;
};

//	Build a printable-ASCII atlas from a TTF font buffer.
//	`pixel_height` is the requested font size in pixels (e.g. 16, 24).
//	`atlas_width` x `atlas_height` is the texture size to pack into
//	(typically 256x256 or 512x512 -- power of two for GPU friendliness).
//	Returns true on success, false if PackBegin / PackFontRange failed
//	(usually too small an atlas for the requested size).
bool build_printable_ascii_atlas(
    std::uint8_t const* font_bytes,
    std::size_t         font_size_bytes,
    float               pixel_height,
    int                 atlas_width,
    int                 atlas_height,
    GlyphAtlas&         out );

//	One vertex of a textured glyph quad. Layout : 2D screen-space
//	position (in PIXELS, relative to the cursor origin) + 2D atlas
//	UV in [0,1]. Consumer's vertex shader applies the framebuffer
//	transform (pixels -> NDC).
struct Vertex2D
{
    float x, y;     //  screen-space pixels
    float u, v;     //  atlas UV
};

//	Build a CPU-side vertex buffer for rendering `text` against `atlas`.
//	`cursor_x, cursor_y` is the screen-space (in pixels) baseline
//	origin -- the cursor advances rightward by each glyph's xadvance.
//	`y` follows the engine's convention : y grows DOWNWARD (top-left
//	origin like CoreGraphics).
//
//	For each printable ASCII character, six vertices are emitted (two
//	triangles forming a textured quad). Non-printable bytes are
//	skipped EXCEPT space (32), which advances the cursor without
//	emitting geometry. Bytes outside [32, 126] are silently skipped.
//
//	Returns the number of glyph quads emitted (>= 0). Each quad adds
//	6 vertices to `out_vertices` ; callers can recover the quad count
//	from `out_vertices.size() / 6` if they don't read the return value.
int layout_text_quads(
    GlyphAtlas const&            atlas,
    char const*                  text,
    float                        cursor_x,
    float                        cursor_y,
    std::vector< Vertex2D >&     out_vertices );

//	Measure the pixel width of a string as if laid out at the atlas's
//	build-time pixel size. Sums each printable ASCII glyph's xadvance ;
//	bytes outside [32,126] contribute 0. Used by the .app HUD to right-
//	justify text without first running layout_text_quads (no vertex
//	allocation needed).
float measure_text_width(
    GlyphAtlas const& atlas,
    char const*       text );

}}  //  namespace aaa::text
