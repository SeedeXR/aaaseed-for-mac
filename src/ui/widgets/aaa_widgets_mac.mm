// src/ui/widgets/aaa_widgets_mac.mm
//
// c147-A : Mac-native immediate-mode widget UI. Implementation file ;
// see header for design + hermetic doctrine notes.
//
// Render architecture (matches the c61 HUD pipeline) :
//   - One ProgramId, two MSL programs : solid-colored quad + (later)
//     textured glyph. For Phase 1 we draw EVERYTHING as solid-colored
//     quads ; widget labels render as a string of glyph quads via the
//     c87 layout_text_quads helper sharing the host's atlas-textured
//     glyph pipeline (driven externally by the host -- see "label
//     deferral" below).
//
//   - Per-frame quad buffer : std::vector< QuadVertex > accumulated by
//     widget calls + uploaded as one GPU buffer at end_frame. One
//     draw_arrays call covers all the colored quads. Keeps the encoder
//     overhead at O(1) regardless of widget count.
//
// Label-text deferral (intentional Phase 1 simplification) :
//   - Glyph rendering requires the host's font atlas + text MSL +
//     atlas-textured pipeline (c61). To keep this sub-lib hermetic and
//     under the 600-LOC budget, the widget system currently renders
//     widget CHROME (the boxes + thumbs + colored backgrounds) and
//     LEAVES LABEL TEXT to the host's existing MEU HUD path. Authors
//     who want a label on a widget can call aaa.draw_hud_text() with
//     a position hint in their Lua script. Real label-rendering inside
//     this sub-lib is queued for v2 Phase 2.
//
// Retained state :
//   - One `WidgetState` per widget key (= hash of label string). Each
//     entry remembers slider drag state + button armed state + color-
//     well preset index across frames.
//
// Hit-testing :
//   - begin_frame stores mouse_x/y + click edges. Each widget's
//     hit-test runs at widget-call time, BEFORE the draw is emitted,
//     so the same call returns the post-interaction value.

#import "aaa_widgets_mac.h"

#import <Foundation/Foundation.h>

#include "src/gol/gol_backend.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace aaa
{
namespace ui
{
namespace widgets
{

namespace
{
    //	Solid-color quad MSL. Pixel-space xy -> NDC (top-left origin,
    //	y grows downward). Per-vertex RGBA tint via the second attribute.
    //	Mirrors the c68 HUD backdrop shader BUT uses a per-vertex color
    //	instead of a uniform tint so a single draw_arrays call can paint
    //	all widget rects with their own colors.
    constexpr char const* kWidgetMsl = R"MSL(
#include <metal_stdlib>
using namespace metal;

struct VIn {
    float2 pos   [[attribute(0)]];
    float4 color [[attribute(1)]];
};

struct VOut {
    float4 position [[position]];
    float4 color;
};

struct ScreenSize { float2 size; float2 pad; };

vertex VOut vs_main(
    VIn in                       [[stage_in]],
    constant ScreenSize& screen  [[buffer(1)]] )
{
    float2 ndc;
    ndc.x =  (in.pos.x / screen.size.x) * 2.0 - 1.0;
    ndc.y = -((in.pos.y / screen.size.y) * 2.0 - 1.0);
    VOut o;
    o.position = float4( ndc, 0.0, 1.0 );
    o.color    = in.color;
    return o;
}

fragment float4 fs_main( VOut in [[stage_in]] )
{
    return in.color;
}
)MSL";

    struct ScreenSize { float w, h, pad0, pad1; };
    static_assert( sizeof( ScreenSize ) == 16, "ScreenSize alignment" );

    struct QuadVertex
    {
        float x, y;        //	pixel-space position (top-left origin)
        float r, g, b, a;  //	RGBA tint per vertex
    };

    //	8 preset colors for the color-well cycle. Bright + distinguishable
    //	so the .app user sees the cycle land on a different color each click.
    constexpr int kColorPresetCount = 8;
    constexpr Color4f kColorPresets[ kColorPresetCount ] = {
        { 0.90f, 0.20f, 0.20f, 1.0f },   //  red
        { 0.95f, 0.55f, 0.10f, 1.0f },   //  orange
        { 0.95f, 0.90f, 0.20f, 1.0f },   //  yellow
        { 0.25f, 0.85f, 0.30f, 1.0f },   //  green
        { 0.20f, 0.80f, 0.85f, 1.0f },   //  cyan
        { 0.20f, 0.40f, 0.90f, 1.0f },   //  blue
        { 0.80f, 0.30f, 0.85f, 1.0f },   //  magenta
        { 0.95f, 0.95f, 0.95f, 1.0f },   //  white
    };

    //	String hash : FNV-1a 32. Same key any frame for the same label.
    std::uint32_t hash_label( std::string const& s )
    {
        std::uint32_t h = 2166136261u;
        for( char c : s )
        {
            h ^= (std::uint32_t)( (unsigned char) c );
            h *= 16777619u;
        }
        return h;
    }

    bool point_in_rect( double mx, double my,
                        float x, float y, float w, float h )
    {
        return ( mx >= x ) && ( mx < ( x + w ) )
            && ( my >= y ) && ( my < ( y + h ) );
    }

    //	Append 6 vertices (two triangles) forming a quad with `color`.
    void push_quad( std::vector< QuadVertex >& out,
                    float x, float y, float w, float h,
                    Color4f const& c )
    {
        float const x0 = x;
        float const y0 = y;
        float const x1 = x + w;
        float const y1 = y + h;
        QuadVertex const v[ 6 ] = {
            { x0, y0, c.r, c.g, c.b, c.a },
            { x0, y1, c.r, c.g, c.b, c.a },
            { x1, y0, c.r, c.g, c.b, c.a },
            { x1, y0, c.r, c.g, c.b, c.a },
            { x0, y1, c.r, c.g, c.b, c.a },
            { x1, y1, c.r, c.g, c.b, c.a },
        };
        out.insert( out.end(), v, v + 6 );
    }

    //	Layout : a panel reserves vertical strip on the right side of
    //	the host viewport unless overridden by begin_panel. Each widget
    //	flows top-down inside the active panel ; widget rects are
    //	deterministic for hit-testing (no relayout pass needed).
    constexpr float kWidgetHeight  = 22.0f;
    constexpr float kWidgetSpacing = 6.0f;
    constexpr float kPanelPad      = 8.0f;
    constexpr float kSliderTrackH  = 4.0f;
    constexpr float kSliderThumbW  = 10.0f;
    constexpr float kColorWellSide = kWidgetHeight;

    //	c148-A HSV picker layout constants.
    constexpr float kHsvSquareSide   = 128.0f;   //  SV square side
    constexpr float kHsvHueBarWidth  = 16.0f;    //  hue bar width
    constexpr float kHsvAlphaBarH    = 12.0f;    //  alpha strip height
    constexpr float kHsvReadoutH     = 14.0f;    //  RGB readout band

    //	c148-A modal layout constants.
    constexpr float kModalTitleH    = 24.0f;
    constexpr float kModalFooterH   = 36.0f;
    constexpr float kModalButtonW   = 70.0f;
    constexpr float kModalButtonH   = 24.0f;

    //	c148-A collapsing panel : chevron rect is square (= title bar
    //	height) on the left of the title bar.
    constexpr float kChevronSide    = kWidgetHeight;

    //	Smith 1978 "Color Gamut Transform Pairs" (SIGGRAPH '78 vol 12.3)
    //	algorithm B (HSV -> RGB). h/s/v all in [0,1]. Returns RGB in
    //	[0,1]. Reference : Alvy Ray Smith, "Color Gamut Transform Pairs",
    //	Computer Graphics 12(3), Aug 1978, pp. 12-19.
    void hsv_to_rgb( float h, float s, float v,
                     float& r, float& g, float& b )
    {
        if( s <= 0.0f )
        {
            r = g = b = v;
            return;
        }
        if( h >= 1.0f ) h -= std::floor( h );      //  wrap
        if( h <  0.0f ) h += 1.0f;
        float const H  = h * 6.0f;
        int   const I  = (int) std::floor( H );
        float const F  = H - (float) I;
        float const P  = v * ( 1.0f - s );
        float const Q  = v * ( 1.0f - s * F );
        float const T  = v * ( 1.0f - s * ( 1.0f - F ) );
        switch( I )
        {
            case 0:  r = v; g = T; b = P; break;
            case 1:  r = Q; g = v; b = P; break;
            case 2:  r = P; g = v; b = T; break;
            case 3:  r = P; g = Q; b = v; break;
            case 4:  r = T; g = P; b = v; break;
            default: r = v; g = P; b = Q; break;   //  case 5 / wrap
        }
    }

    //	Smith 1978 algorithm A (RGB -> HSV). Inputs + outputs in [0,1].
    //	When the input is achromatic (max==min) the returned hue is 0.
    void rgb_to_hsv( float r, float g, float b,
                     float& h, float& s, float& v )
    {
        float const mx = std::max( r, std::max( g, b ) );
        float const mn = std::min( r, std::min( g, b ) );
        v = mx;
        float const d = mx - mn;
        s = ( mx > 0.0f ) ? ( d / mx ) : 0.0f;
        if( d <= 0.0f )
        {
            h = 0.0f;
            return;
        }
        float hh = 0.0f;
        if( mx == r ) hh = ( g - b ) / d + ( g < b ? 6.0f : 0.0f );
        else if( mx == g ) hh = ( b - r ) / d + 2.0f;
        else               hh = ( r - g ) / d + 4.0f;
        h = hh / 6.0f;
    }

    //	Append a quad with PER-CORNER colors (4-corner bilinear gradient).
    //	Order : top-left, bottom-left, top-right, bottom-right. The two
    //	triangles repeat corners so the GPU bilerps the four colors.
    void push_quad_4c( std::vector< QuadVertex >& out,
                       float x, float y, float w, float h,
                       Color4f const& tl, Color4f const& bl,
                       Color4f const& tr, Color4f const& br )
    {
        float const x0 = x;
        float const y0 = y;
        float const x1 = x + w;
        float const y1 = y + h;
        QuadVertex const v[ 6 ] = {
            { x0, y0, tl.r, tl.g, tl.b, tl.a },
            { x0, y1, bl.r, bl.g, bl.b, bl.a },
            { x1, y0, tr.r, tr.g, tr.b, tr.a },
            { x1, y0, tr.r, tr.g, tr.b, tr.a },
            { x0, y1, bl.r, bl.g, bl.b, bl.a },
            { x1, y1, br.r, br.g, br.b, br.a },
        };
        out.insert( out.end(), v, v + 6 );
    }

    //	Clamp a float into [a,b].
    inline float clamp01( float v )
    {
        return v < 0.0f ? 0.0f : ( v > 1.0f ? 1.0f : v );
    }

}   //	anonymous

//	Per-widget retained state. Keyed by hash(label).
struct WidgetState
{
    //	Slider drag state.
    bool   dragging      = false;
    double drag_start_x  = 0.0;
    float  drag_start_v  = 0.0f;
    float  last_v        = 0.0f;
    float  drag_delta_px = 0.0f;

    //	Button armed (pressed inside, awaiting release).
    bool   armed         = false;
    bool   clicked_this_frame = false;

    //	Color well : current preset index.
    int    preset_ix     = 0;
};

//	c148-A : HSV picker retained state. One per labeled hsv_color_picker.
//	hue / sat / val all in [0,1]. alpha kept separate so callers can
//	round-trip RGBA without losing precision in the HSV conversion.
struct HsvPickerState
{
    float h    = 0.0f;   //  hue        [0,1]
    float s    = 1.0f;   //  saturation [0,1]
    float v    = 1.0f;   //  value      [0,1]
    float a    = 1.0f;   //  alpha      [0,1]
    int   drag = 0;      //  0 = none, 1 = SV square, 2 = hue bar, 3 = alpha
    bool  inited = false;
};

//	c148-A : per-label collapsing-panel expanded state. Defaults to
//	`true` (panels start expanded) ; toggled on chevron-rect click.
struct PanelExpandedState
{
    bool expanded = true;
    bool armed    = false;   //  chevron pressed-inside, awaiting release
};

//	c148-A : single-line text-input retained state. The buffer is the
//	user's edited string (only meaningful when this label is focused).
struct TextInputState
{
    std::string buffer;
    std::string original;   //  pre-edit value, restored on Escape
    bool        primed = false;
};

//	c150-A v4 : multi-line text_area retained state. Lines is the
//	logical (newline-separated) row split of the buffer ; cursor_row /
//	cursor_col is the insertion point ; scroll_top is the topmost
//	visible row when the buffer exceeds `visible_lines`. width_chars
//	is the visual column count for soft-wrap rendering ; the buffer
//	itself keeps hard newlines but rendering wraps long rows.
struct TextAreaState
{
    std::vector< std::string > lines;        //  one entry per hard line
    int  cursor_row   = 0;
    int  cursor_col   = 0;
    int  scroll_top   = 0;
    bool primed       = false;
    std::string original_joined;   //  pre-edit joined buffer for Escape revert
};

//	c150-A v4 : marked-text (IME composition) retained state. Single
//	global ; the macOS Text Input System composes one IME session at
//	a time. The buffer is UTF-8 ; selection_* are character offsets
//	into the composing string (used by the renderer to draw IME's
//	internal caret). When commit fires, the buffer is appended into
//	the focused text_input or text_area at cursor position.
struct MarkedTextState
{
    std::string composing;          //  current pending composition
    int         selection_start  = 0;
    int         selection_length = 0;
    bool        active            = false;
};

//	c148-A : modal-dialog state. One global (the spec only supports a
//	single active modal at a time) ; survives across frames until OK/
//	Cancel fires.
struct ModalGlobalState
{
    bool        is_open       = false;
    std::string title;
    bool        ok_fired      = false;  //  edge for the one frame
    bool        cancel_fired  = false;
    bool        ok_armed      = false;  //  button armed states
    bool        cancel_armed  = false;
};

//	Pimpl.
class WidgetSystemImpl
{
public:
    explicit WidgetSystemImpl( GOL::Backend* backend )
        : _backend( backend )
    {}

    ~WidgetSystemImpl()
    {
        teardown_gpu();
    }

    void begin_frame( std::uint32_t width, std::uint32_t height,
                      double mouse_x, double mouse_y,
                      bool mouse_pressed_now, bool mouse_released_now )
    {
        _w = width;
        _h = height;
        _mouse_x = mouse_x;
        _mouse_y = mouse_y;

        //	Track held state from edge events. The host owns the source-
        //	of-truth ; we mirror it here so widget calls inside this
        //	frame can ask "is the button held?" without re-reading the
        //	host on every call.
        if( mouse_pressed_now )  _mouse_held = true;
        if( mouse_released_now ) _mouse_held = false;
        _mouse_pressed_edge   = mouse_pressed_now;
        _mouse_released_edge  = mouse_released_now;

        _quads.clear();
        _widget_count = 0;
        _had_interaction = false;

        //	Default panel rect : right column, 200px wide, full height.
        _active_panel    = false;
        _panel_x         = float( width ) - 220.0f;
        _panel_y         = 40.0f;
        _panel_w         = 200.0f;
        _panel_h         = float( height ) - 80.0f;
        _cursor_y        = _panel_y + kPanelPad;
        _next_panel_id   = 1;

        //	Reset per-frame click latches on every widget state.
        for( auto& kv : _state ) kv.second.clicked_this_frame = false;

        //	c148-A : bump frame counter for cursor-blink + clear modal
        //	one-frame edge flags.
        ++_frame_count;
        _modal_state.ok_fired     = false;
        _modal_state.cancel_fired = false;
        _suppress_widgets = false;

        //	c149-A v3 Feature 5 : reset nested-panel tracking on every
        //	frame. begin_collapsing_panel rebuilds the stack each frame
        //	based on the Lua script's calls ; leftover state from a
        //	previous frame would corrupt the new draw.
        _collapsing_nest_depth = 0;
        _collapsing_nest_stack.clear();
    }

    PanelHandle begin_panel( std::string const& title,
                             float x, float y, float w, float h )
    {
        (void) title;   //  label deferral : title text not rendered v2P1
        _active_panel = true;
        _panel_x = x;
        _panel_y = y;
        _panel_w = w;
        _panel_h = h;
        _cursor_y = _panel_y + kPanelPad;

        //	Panel chrome : a dark translucent backdrop + a brighter
        //	1-row "header bar" so the user sees the panel exists.
        Color4f const bg{ 0.10f, 0.10f, 0.12f, 0.80f };
        Color4f const hdr{ 0.20f, 0.22f, 0.30f, 0.95f };
        push_quad( _quads, x, y, w, h, bg );
        push_quad( _quads, x, y, w, kWidgetHeight, hdr );

        _cursor_y = y + kWidgetHeight + kWidgetSpacing;

        PanelHandle handle;
        handle.id    = _next_panel_id++;
        handle.valid = true;
        return handle;
    }

    void end_panel()
    {
        _active_panel = false;
    }

    float slider( std::string const& label,
                  float value, float min, float max )
    {
        float const w = effective_widget_width();
        float const x = effective_widget_x();
        float const y = next_widget_y();

        std::uint32_t const key = hash_label( label );
        WidgetState& st = _state[ key ];

        //	Clamp input value to range immediately so callers that pass
        //	out-of-range values see deterministic clamp behaviour.
        if( max < min ) { float t = min; min = max; max = t; }
        if( value < min ) value = min;
        if( value > max ) value = max;

        float const range = ( max > min ) ? ( max - min ) : 1.0f;

        //	Hit-testing : track rectangle covers the slider's full width
        //	minus pad. Thumb is a slim vertical bar centered on the
        //	value's fractional position along the track.
        float const track_x = x;
        float const track_y = y + ( kWidgetHeight - kSliderTrackH ) * 0.5f;
        float const track_w = w;
        float const track_h = kSliderTrackH;
        float const t = ( value - min ) / range;
        float thumb_x = track_x + t * track_w - kSliderThumbW * 0.5f;

        bool const hovering = point_in_rect(
            _mouse_x, _mouse_y, x, y, w, kWidgetHeight );

        if( _mouse_pressed_edge && hovering && !st.dragging )
        {
            st.dragging     = true;
            st.drag_start_x = _mouse_x;
            st.drag_start_v = value;
            _had_interaction = true;
        }
        if( _mouse_released_edge ) st.dragging = false;

        if( st.dragging )
        {
            double const dx = _mouse_x - st.drag_start_x;
            st.drag_delta_px = (float) dx;
            float const dv = float( dx ) / ( track_w > 0.0f ? track_w : 1.0f ) * range;
            value = st.drag_start_v + dv;
            if( value < min ) value = min;
            if( value > max ) value = max;
            //	Recompute thumb after drag-updated value.
            float const t2 = ( value - min ) / range;
            thumb_x = track_x + t2 * track_w - kSliderThumbW * 0.5f;
            _had_interaction = true;
        }
        else
        {
            st.drag_delta_px = 0.0f;
        }
        st.last_v = value;

        //	Draw : track + thumb. Hover tints the thumb brighter.
        Color4f const track_col{ 0.35f, 0.36f, 0.40f, 0.95f };
        Color4f const thumb_col = hovering || st.dragging
            ? Color4f{ 0.95f, 0.95f, 1.00f, 1.00f }
            : Color4f{ 0.70f, 0.72f, 0.78f, 1.00f };

        //	Widget background (faint) so the label area is visible.
        Color4f const bg{ 0.15f, 0.16f, 0.20f, 0.80f };
        push_quad( _quads, x, y, w, kWidgetHeight, bg );

        push_quad( _quads, track_x, track_y, track_w, track_h, track_col );
        push_quad( _quads, thumb_x, track_y - 6.0f, kSliderThumbW,
                   track_h + 12.0f, thumb_col );

        ++_widget_count;
        return value;
    }

    bool button( std::string const& label )
    {
        float const w = effective_widget_width();
        float const x = effective_widget_x();
        float const y = next_widget_y();

        std::uint32_t const key = hash_label( label );
        WidgetState& st = _state[ key ];

        bool const hovering = point_in_rect(
            _mouse_x, _mouse_y, x, y, w, kWidgetHeight );

        bool clicked = false;
        if( _mouse_pressed_edge && hovering )
        {
            st.armed = true;
            _had_interaction = true;
        }
        if( _mouse_released_edge )
        {
            if( st.armed && hovering )
            {
                clicked = true;
                _had_interaction = true;
            }
            st.armed = false;
        }
        st.clicked_this_frame = clicked;

        //	Visual state : armed (pressed) -> darker ; hovering -> brighter.
        Color4f color;
        if( st.armed && hovering )
            color = { 0.25f, 0.45f, 0.65f, 0.95f };
        else if( hovering )
            color = { 0.45f, 0.55f, 0.75f, 0.95f };
        else
            color = { 0.30f, 0.36f, 0.50f, 0.95f };

        push_quad( _quads, x, y, w, kWidgetHeight, color );

        //	Bottom shadow line for depth.
        Color4f const shadow{ 0.0f, 0.0f, 0.0f, 0.35f };
        push_quad( _quads, x, y + kWidgetHeight - 2.0f, w, 2.0f, shadow );

        ++_widget_count;
        return clicked;
    }

    Color4f color_well( std::string const& label, Color4f rgba )
    {
        float const w = effective_widget_width();
        float const x = effective_widget_x();
        float const y = next_widget_y();

        std::uint32_t const key = hash_label( label );
        WidgetState& st = _state[ key ];

        //	If this is the first time the widget is seen, sync the
        //	preset index to the closest preset (or zero). Otherwise the
        //	retained preset wins on subsequent frames -- the caller's
        //	`rgba` argument is treated as the INITIAL value only.
        if( !st.preset_ix && rgba_matches_preset_zero( rgba ) )
        {
            //	First-frame init : keep preset 0.
        }

        //	Color well rect : a square swatch on the right side of the
        //	widget's row.
        float const well_x = x + w - kColorWellSide;
        float const well_y = y;
        bool const hovering = point_in_rect(
            _mouse_x, _mouse_y, well_x, well_y, kColorWellSide, kColorWellSide );

        //	Click cycles to next preset on press-edge inside the well.
        if( _mouse_pressed_edge && hovering )
        {
            st.preset_ix = ( st.preset_ix + 1 ) % kColorPresetCount;
            _had_interaction = true;
        }

        Color4f const current = kColorPresets[ st.preset_ix ];

        //	Background row (so the label area is visible).
        Color4f const bg{ 0.15f, 0.16f, 0.20f, 0.80f };
        push_quad( _quads, x, y, w, kWidgetHeight, bg );

        //	The color swatch.
        push_quad( _quads, well_x + 2.0f, well_y + 2.0f,
                   kColorWellSide - 4.0f, kColorWellSide - 4.0f,
                   current );

        //	Hover ring (thin bright outline on top + bottom).
        if( hovering )
        {
            Color4f const ring{ 1.0f, 1.0f, 1.0f, 0.90f };
            push_quad( _quads, well_x + 2.0f, well_y + 1.0f,
                       kColorWellSide - 4.0f, 1.0f, ring );
            push_quad( _quads, well_x + 2.0f, well_y + kColorWellSide - 2.0f,
                       kColorWellSide - 4.0f, 1.0f, ring );
        }

        ++_widget_count;
        return current;
    }

    //	---- c148-A v2 Phase 2 : HSV color picker --------------------------
    //	Layout block (top-down within the active panel) :
    //	  Row 1 : SV square (left) + hue bar (right).
    //	  Row 2 : alpha strip.
    //	  Row 3 : RGB readout band (color preview).
    //	Total height : kHsvSquareSide + spacing + kHsvAlphaBarH +
    //	spacing + kHsvReadoutH. Mouse interaction routes by which
    //	sub-rect the press lands in ; drag state lives in
    //	_hsv_picker_state[hash(label)].
    Color4f hsv_color_picker( std::string const& label, Color4f rgba )
    {
        if( _suppress_widgets ) return rgba;

        float const w = effective_widget_width();
        float const x = effective_widget_x();
        float const y = next_widget_y();

        std::uint32_t const key = hash_label( label );
        HsvPickerState& st = _hsv_picker_state[ key ];

        //	First-frame init : convert caller's RGB to HSV (Smith 1978 A).
        if( !st.inited )
        {
            rgb_to_hsv( rgba.r, rgba.g, rgba.b, st.h, st.s, st.v );
            st.a      = rgba.a;
            st.inited = true;
        }

        //	Sub-rects.
        float const sq_x  = x;
        float const sq_y  = y;
        float const sq_s  = kHsvSquareSide;
        float const hue_x = sq_x + sq_s + 4.0f;
        float const hue_y = sq_y;
        float const hue_w = kHsvHueBarWidth;
        float const hue_h = sq_s;
        float const alpha_y = sq_y + sq_s + 4.0f;
        float const alpha_w = sq_s + hue_w + 4.0f;
        float const readout_y = alpha_y + kHsvAlphaBarH + 4.0f;
        float const readout_w = alpha_w;

        //	Hit-testing : route press to the appropriate sub-region.
        bool const in_sq    = point_in_rect( _mouse_x, _mouse_y, sq_x,  sq_y,  sq_s,    sq_s   );
        bool const in_hue   = point_in_rect( _mouse_x, _mouse_y, hue_x, hue_y, hue_w,   hue_h  );
        bool const in_alpha = point_in_rect( _mouse_x, _mouse_y, sq_x,  alpha_y, alpha_w, kHsvAlphaBarH );

        if( _mouse_pressed_edge )
        {
            if( in_sq )    { st.drag = 1; _had_interaction = true; }
            else if( in_hue )   { st.drag = 2; _had_interaction = true; }
            else if( in_alpha ) { st.drag = 3; _had_interaction = true; }
        }
        if( _mouse_released_edge ) st.drag = 0;

        if( st.drag == 1 )
        {
            float const fx = float( _mouse_x - sq_x ) / sq_s;
            float const fy = float( _mouse_y - sq_y ) / sq_s;
            st.s = clamp01( fx );
            st.v = clamp01( 1.0f - fy );
            _had_interaction = true;
        }
        else if( st.drag == 2 )
        {
            float const fy = float( _mouse_y - hue_y ) / hue_h;
            st.h = clamp01( fy );
            _had_interaction = true;
        }
        else if( st.drag == 3 )
        {
            float const fx = float( _mouse_x - sq_x ) / alpha_w;
            st.a = clamp01( fx );
            _had_interaction = true;
        }

        //	Compute current RGB for both preview + readout.
        float rr, gg, bb;
        hsv_to_rgb( st.h, st.s, st.v, rr, gg, bb );

        //	SV square : 4-corner bilinear gradient. Top-left=white,
        //	top-right=pure-hue, bottom-left=black, bottom-right=black.
        float h_rr, h_gg, h_bb;
        hsv_to_rgb( st.h, 1.0f, 1.0f, h_rr, h_gg, h_bb );
        Color4f const c_tl{ 1.0f, 1.0f, 1.0f, 1.0f };
        Color4f const c_tr{ h_rr, h_gg, h_bb, 1.0f };
        Color4f const c_bl{ 0.0f, 0.0f, 0.0f, 1.0f };
        Color4f const c_br{ 0.0f, 0.0f, 0.0f, 1.0f };
        push_quad_4c( _quads, sq_x, sq_y, sq_s, sq_s, c_tl, c_bl, c_tr, c_br );

        //	Hue bar : 6 vertical bands hitting the cardinal hues. We
        //	approximate the continuous gradient with 6 quad strips.
        int const hue_bands = 6;
        for( int i = 0; i < hue_bands; ++i )
        {
            float const t0 = (float) i       / hue_bands;
            float const t1 = (float)( i + 1 ) / hue_bands;
            float ar, ag, ab, br, bg, bb2;
            hsv_to_rgb( t0, 1.0f, 1.0f, ar, ag, ab );
            hsv_to_rgb( t1, 1.0f, 1.0f, br, bg, bb2 );
            Color4f const ca{ ar, ag, ab, 1.0f };
            Color4f const cb{ br, bg, bb2, 1.0f };
            push_quad_4c( _quads,
                          hue_x, hue_y + t0 * hue_h, hue_w, ( t1 - t0 ) * hue_h,
                          ca, cb, ca, cb );
        }

        //	SV crosshair (small white box on top of the gradient at
        //	st.s / st.v position).
        float const cross_x = sq_x + st.s * sq_s - 2.0f;
        float const cross_y = sq_y + ( 1.0f - st.v ) * sq_s - 2.0f;
        Color4f const cross_col{ 1.0f, 1.0f, 1.0f, 1.0f };
        push_quad( _quads, cross_x, cross_y, 4.0f, 4.0f, cross_col );

        //	Hue indicator (thin horizontal bar at st.h position on the bar).
        float const hi_y = hue_y + st.h * hue_h - 1.0f;
        push_quad( _quads, hue_x - 2.0f, hi_y, hue_w + 4.0f, 2.0f, cross_col );

        //	Alpha strip : black-to-white gradient ; thumb at st.a position.
        Color4f const a_black{ 0.0f, 0.0f, 0.0f, 1.0f };
        Color4f const a_white{ 1.0f, 1.0f, 1.0f, 1.0f };
        push_quad_4c( _quads, sq_x, alpha_y, alpha_w, kHsvAlphaBarH,
                      a_black, a_black, a_white, a_white );
        float const a_thumb_x = sq_x + st.a * alpha_w - 2.0f;
        Color4f const a_thumb{ 0.95f, 0.30f, 0.30f, 1.0f };
        push_quad( _quads, a_thumb_x, alpha_y - 2.0f, 4.0f, kHsvAlphaBarH + 4.0f, a_thumb );

        //	Readout band : solid swatch of the current RGB color (alpha
        //	composited against the panel background by the blend pass).
        Color4f const readout{ rr, gg, bb, st.a };
        push_quad( _quads, sq_x, readout_y, readout_w, kHsvReadoutH, readout );

        //	Advance the cursor to account for the picker's full height
        //	since we drew much taller than kWidgetHeight.
        _cursor_y = readout_y + kHsvReadoutH + kWidgetSpacing;

        ++_widget_count;

        Color4f out;
        out.r = rr; out.g = gg; out.b = bb; out.a = st.a;
        return out;
    }

    //	---- c148-A v2 Phase 3 : modal dialog ------------------------------
    //	begin_modal centers a (w,h) rect on the viewport. The modal's
    //	open state survives across frames until OK/Cancel fires.
    WidgetSystem::ModalResult begin_modal( std::string const& title,
                                           float w, float h )
    {
        WidgetSystem::ModalResult res;
        res.open = _modal_state.is_open
                   && ( _modal_state.title == title );
        if( !res.open ) return res;

        //	Dim background : a viewport-spanning translucent quad.
        Color4f const dim{ 0.0f, 0.0f, 0.0f, 0.55f };
        push_quad( _quads, 0.0f, 0.0f, float( _w ), float( _h ), dim );

        //	Modal rect.
        float const mx = ( float( _w ) - w ) * 0.5f;
        float const my = ( float( _h ) - h ) * 0.5f;
        Color4f const bg{ 0.13f, 0.14f, 0.18f, 0.98f };
        Color4f const hdr{ 0.22f, 0.24f, 0.32f, 1.0f };
        push_quad( _quads, mx, my, w, h, bg );
        push_quad( _quads, mx, my, w, kModalTitleH, hdr );

        //	Reposition the layout cursor INSIDE the modal so subsequent
        //	widget calls land within it. Save the previous panel state
        //	but we don't restore -- end_modal does that.
        _active_panel = true;
        _panel_x = mx;
        _panel_y = my;
        _panel_w = w;
        _panel_h = h;
        _cursor_y = my + kModalTitleH + kWidgetSpacing;

        //	Footer with OK + Cancel buttons. We render them inline so the
        //	caller's begin_modal/end_modal block doesn't need to emit
        //	them. Layout : both at bottom-right, OK rightmost.
        float const fy   = my + h - kModalButtonH - 6.0f;
        float const ok_x = mx + w - kModalButtonW - 8.0f;
        float const ca_x = ok_x - kModalButtonW - 6.0f;

        //	OK button hit-test.
        bool const ok_hover = point_in_rect(
            _mouse_x, _mouse_y, ok_x, fy, kModalButtonW, kModalButtonH );
        if( _mouse_pressed_edge && ok_hover ) _modal_state.ok_armed = true;
        if( _mouse_released_edge )
        {
            if( _modal_state.ok_armed && ok_hover )
            {
                _modal_state.ok_fired   = true;
                _modal_state.is_open    = false;
                res.ok_clicked          = true;
                res.open                = false;
                _had_interaction        = true;
            }
            _modal_state.ok_armed = false;
        }

        //	Cancel button hit-test.
        bool const ca_hover = point_in_rect(
            _mouse_x, _mouse_y, ca_x, fy, kModalButtonW, kModalButtonH );
        if( _mouse_pressed_edge && ca_hover ) _modal_state.cancel_armed = true;
        if( _mouse_released_edge )
        {
            if( _modal_state.cancel_armed && ca_hover )
            {
                _modal_state.cancel_fired = true;
                _modal_state.is_open      = false;
                res.cancel_clicked        = true;
                res.open                  = false;
                _had_interaction          = true;
            }
            _modal_state.cancel_armed = false;
        }

        Color4f const ok_col = ok_hover
            ? Color4f{ 0.30f, 0.65f, 0.40f, 1.0f }
            : Color4f{ 0.22f, 0.50f, 0.32f, 1.0f };
        Color4f const ca_col = ca_hover
            ? Color4f{ 0.65f, 0.35f, 0.35f, 1.0f }
            : Color4f{ 0.45f, 0.25f, 0.28f, 1.0f };
        push_quad( _quads, ok_x, fy, kModalButtonW, kModalButtonH, ok_col );
        push_quad( _quads, ca_x, fy, kModalButtonW, kModalButtonH, ca_col );

        //	Title bar tint (Phase 1 widgets stay no-label per the
        //	"label deferral" note in the header ; this just provides
        //	the visual frame). Cite : title text rendered by the host's
        //	c61 glyph atlas path when the script calls draw_hud_text.
        (void) title;

        return res;
    }

    void end_modal()
    {
        //	No-op : the modal's chrome (dim + bg + buttons) was emitted
        //	at begin_modal. We leave _active_panel + cursor as-is so the
        //	host's regular end_panel / next-widget calls continue to
        //	work outside the modal frame. (Lua scripts that call
        //	begin_modal expect to call end_modal regardless of open
        //	state, matching the begin_panel/end_panel discipline.)
    }

    void show_modal( std::string const& title )
    {
        _modal_state.is_open = true;
        _modal_state.title   = title;
        _modal_state.ok_fired     = false;
        _modal_state.cancel_fired = false;
    }

    //	---- c148-A v2 Phase 3 : single-line text input --------------------
    //	The widget shows the current value (or _text_buffer when this
    //	input is focused). Click within the rect to focus ; the widget
    //	system's _focused_text_input_id tracks the active input. The
    //	host (AAASeedInputView) calls on_text_input() with the codepoint
    //	for each printable / backspace / enter / escape event.
    std::string text_input( std::string const& label,
                            std::string const& current_value,
                            std::size_t max_length )
    {
        if( _suppress_widgets ) return current_value;

        float const w = effective_widget_width();
        float const x = effective_widget_x();
        float const y = next_widget_y();

        std::uint32_t const key = hash_label( label );
        TextInputState& st = _text_input_state[ key ];

        bool const focused = ( _focused_text_input_id == key );

        //	First-frame init : buffer mirrors the caller's value. We
        //	keep `primed` so a script that re-calls each frame with the
        //	same value doesn't clobber the user's in-progress edit.
        if( !st.primed )
        {
            st.buffer   = current_value;
            st.original = current_value;
            st.primed   = true;
        }
        //	When NOT focused, allow the caller's value to win (so the
        //	script can update the value externally). When focused, the
        //	user's edit buffer is sacred.
        if( !focused )
        {
            st.buffer   = current_value;
            st.original = current_value;
        }

        //	Hit-test : click inside the rect focuses this input.
        bool const hovering = point_in_rect(
            _mouse_x, _mouse_y, x, y, w, kWidgetHeight );

        if( _mouse_pressed_edge )
        {
            if( hovering )
            {
                _focused_text_input_id = key;
                st.original = st.buffer;
                _had_interaction = true;
            }
            else if( focused )
            {
                //	Click outside : commit (unfocus, leave buffer as-is).
                _focused_text_input_id = 0;
            }
        }

        //	Clamp buffer to max_length defensively (insertion logic in
        //	on_text_input already enforces this, but a caller passing a
        //	longer current_value should still see a clamp).
        if( st.buffer.size() > max_length )
        {
            st.buffer.resize( max_length );
        }

        //	Chrome : background (brighter when focused).
        Color4f const bg = focused
            ? Color4f{ 0.22f, 0.24f, 0.30f, 0.95f }
            : Color4f{ 0.15f, 0.16f, 0.20f, 0.85f };
        push_quad( _quads, x, y, w, kWidgetHeight, bg );

        //	Caret : a thin vertical bar at the end of the buffer. Blinks
        //	on/off via the frame counter (30-frame on, 30-frame off).
        bool const blink_on = ( _frame_count % 60 ) < 30;
        if( focused && blink_on )
        {
            //	Approximate text width : 7 px per char (matches the
            //	host's c87 glyph atlas mono pitch closely enough for
            //	caret positioning ; real text uses the host's atlas).
            float const caret_x = x + 4.0f + (float) st.buffer.size() * 7.0f;
            Color4f const caret_col{ 1.0f, 1.0f, 1.0f, 0.95f };
            push_quad( _quads, caret_x, y + 3.0f, 1.5f, kWidgetHeight - 6.0f, caret_col );
        }

        ++_widget_count;
        return st.buffer;
    }

    void on_text_input( std::uint32_t codepoint )
    {
        //	c150-A v4 : when a text_area is focused (and no single-line
        //	input is), route to the multi-line dispatcher. text_input and
        //	text_area are mutually exclusive (focus latches one or the
        //	other), so this branch fires only when text_area is active.
        if( _focused_text_input_id == 0 && _focused_text_area_id != 0 )
        {
            on_text_input_text_area( codepoint );
            return;
        }
        if( _focused_text_input_id == 0 ) return;
        auto it = _text_input_state.find( _focused_text_input_id );
        if( it == _text_input_state.end() ) return;
        TextInputState& st = it->second;

        if( codepoint == 0x08 )   //  backspace
        {
            if( !st.buffer.empty() ) st.buffer.pop_back();
        }
        else if( codepoint == 0x0A || codepoint == 0x0D )   //  enter
        {
            //	Commit + unfocus.
            _focused_text_input_id = 0;
        }
        else if( codepoint == 0x1B )   //  escape
        {
            //	Revert + unfocus.
            st.buffer = st.original;
            _focused_text_input_id = 0;
        }
        else if( codepoint >= 0x20 && codepoint <= 0x7E )   //  printable ASCII
        {
            //	Look up the input's max_length is not stored in state ;
            //	we use a 256-char ceiling as a hard upper bound. The
            //	per-call max_length clamp at text_input() time runs every
            //	frame so the buffer can't exceed it once the script
            //	calls text_input again with its max_length argument.
            if( st.buffer.size() < 256 )
            {
                st.buffer.push_back( (char) codepoint );
            }
        }
    }

    std::uint32_t focused_text_input_id() const { return _focused_text_input_id; }

    //	---- c150-A v4 : multi-line text_area --------------------------
    //	A text_area renders a multi-line edit surface. Each line is a
    //	std::string entry in TextAreaState.lines. Cursor lives at
    //	(cursor_row, cursor_col) ; Enter splits the current line ;
    //	backspace at col 0 joins with the previous line ; backspace
    //	elsewhere removes the previous char. Scrolling kicks in when
    //	the cursor leaves the visible window.
    //
    //	Layout : the visible area is `visible_lines * kWidgetHeight`
    //	pixels tall by `effective_widget_width()` wide. Each visible
    //	line gets one row of chrome ; the active line is brighter when
    //	the area is focused.
    std::string text_area( std::string const& label,
                           std::string const& current_value,
                           int                visible_lines,
                           int                width_chars,
                           std::size_t        max_length )
    {
        if( _suppress_widgets ) return current_value;
        if( visible_lines < 1 ) visible_lines = 1;
        if( width_chars   < 1 ) width_chars   = 1;

        float const w = effective_widget_width();
        float const x = effective_widget_x();
        float const y = next_widget_y();
        float const total_h = float( visible_lines ) * kWidgetHeight;

        std::uint32_t const key = hash_label( label );
        TextAreaState& st = _text_area_state[ key ];

        bool const focused = ( _focused_text_area_id == key );

        //	First-frame init OR external-update path (when unfocused).
        //	Splits `current_value` by '\n' into the lines vector. We keep
        //	`primed` to avoid clobbering an in-progress edit when the
        //	script re-passes the same value each frame.
        auto split_lines = []( std::string const& s ) {
            std::vector< std::string > out;
            std::string cur;
            for( char c : s )
            {
                if( c == '\n' )
                {
                    out.push_back( cur );
                    cur.clear();
                }
                else
                {
                    cur.push_back( c );
                }
            }
            out.push_back( cur );
            return out;
        };

        if( !st.primed )
        {
            st.lines      = split_lines( current_value );
            st.cursor_row = 0;
            st.cursor_col = 0;
            st.scroll_top = 0;
            st.original_joined = current_value;
            st.primed     = true;
        }
        else if( !focused )
        {
            //	External update : caller's value wins when unfocused.
            st.lines = split_lines( current_value );
            st.original_joined = current_value;
            if( st.cursor_row >= (int) st.lines.size() )
                st.cursor_row = (int) st.lines.size() - 1;
            if( st.cursor_row < 0 ) st.cursor_row = 0;
            int const cur_len = (int) st.lines[ st.cursor_row ].size();
            if( st.cursor_col > cur_len ) st.cursor_col = cur_len;
        }

        //	Hit-test : click inside the box focuses + steals from text_input.
        bool const hovering = point_in_rect(
            _mouse_x, _mouse_y, x, y, w, total_h );

        if( _mouse_pressed_edge )
        {
            if( hovering )
            {
                _focused_text_area_id  = key;
                _focused_text_input_id = 0;
                st.original_joined     = join_lines( st.lines );
                _had_interaction       = true;
            }
            else if( focused )
            {
                //	Click outside : commit (unfocus, keep buffer).
                _focused_text_area_id = 0;
            }
        }

        //	Adjust scroll_top so the cursor's row is visible.
        if( st.cursor_row < st.scroll_top )
            st.scroll_top = st.cursor_row;
        if( st.cursor_row >= st.scroll_top + visible_lines )
            st.scroll_top = st.cursor_row - visible_lines + 1;
        if( st.scroll_top < 0 ) st.scroll_top = 0;

        //	Enforce max_length defensively : trim the joined buffer.
        {
            std::string joined = join_lines( st.lines );
            if( joined.size() > max_length )
            {
                joined.resize( max_length );
                st.lines = split_lines( joined );
                if( st.cursor_row >= (int) st.lines.size() )
                    st.cursor_row = (int) st.lines.size() - 1;
                int const cur_len = (int) st.lines[ st.cursor_row ].size();
                if( st.cursor_col > cur_len ) st.cursor_col = cur_len;
            }
        }

        //	Draw chrome : background quad ; brighter when focused. One
        //	row per visible line ; the active row gets a subtle highlight.
        Color4f const bg = focused
            ? Color4f{ 0.20f, 0.22f, 0.28f, 0.95f }
            : Color4f{ 0.14f, 0.15f, 0.19f, 0.85f };
        push_quad( _quads, x, y, w, total_h, bg );

        if( focused )
        {
            //	Active row highlight (relative to scroll).
            int const visible_row = st.cursor_row - st.scroll_top;
            if( visible_row >= 0 && visible_row < visible_lines )
            {
                float const row_y = y + (float) visible_row * kWidgetHeight;
                Color4f const row_hi{ 0.30f, 0.34f, 0.42f, 0.55f };
                push_quad( _quads, x, row_y, w, kWidgetHeight, row_hi );
            }

            //	Caret : blinking thin vertical bar at the cursor column,
            //	7px per char (matches text_input's atlas mono pitch).
            bool const blink_on = ( _frame_count % 60 ) < 30;
            int const visible_row2 = st.cursor_row - st.scroll_top;
            if( blink_on && visible_row2 >= 0 && visible_row2 < visible_lines )
            {
                float const caret_x = x + 4.0f + (float) st.cursor_col * 7.0f;
                float const caret_y = y + (float) visible_row2 * kWidgetHeight + 3.0f;
                Color4f const caret_col{ 1.0f, 1.0f, 1.0f, 0.95f };
                push_quad( _quads, caret_x, caret_y, 1.5f,
                           kWidgetHeight - 6.0f, caret_col );
            }
        }

        //	Marked-text composition underline : if any IME composition is
        //	active AND this is the focused area, draw a horizontal-rule
        //	underline beneath the (visual) row hosting the cursor. The
        //	composing string would render via the host's c87 atlas in a
        //	real frame ; here we emit a thin colored bar so tests can
        //	observe the geometric overlay.
        if( focused && _marked_text.active && !_marked_text.composing.empty() )
        {
            int const visible_row3 = st.cursor_row - st.scroll_top;
            if( visible_row3 >= 0 && visible_row3 < visible_lines )
            {
                float const u_x = x + 4.0f + (float) st.cursor_col * 7.0f;
                float const u_y = y + (float) visible_row3 * kWidgetHeight
                                  + kWidgetHeight - 3.0f;
                int const composing_chars = utf8_char_count( _marked_text.composing );
                float const u_w = (float) composing_chars * 7.0f;
                Color4f const u_col{ 0.95f, 0.75f, 0.10f, 1.0f };
                push_quad( _quads, u_x, u_y, u_w, 1.5f, u_col );
            }
        }

        //	Advance the layout cursor past the whole multi-line block.
        _cursor_y = y + total_h + kWidgetSpacing;
        ++_widget_count;

        return join_lines( st.lines );
    }

    //	c150-A v4 : feed an on_text_input codepoint into the focused
    //	text_area. Mirrors on_text_input's text_input branch but with
    //	row/col cursor semantics. Backspace at col 0 joins with previous
    //	line ; Enter splits the current line at the cursor.
    void on_text_input_text_area( std::uint32_t codepoint )
    {
        auto it = _text_area_state.find( _focused_text_area_id );
        if( it == _text_area_state.end() ) return;
        TextAreaState& st = it->second;
        if( st.lines.empty() ) st.lines.push_back( std::string() );
        if( st.cursor_row < 0 ) st.cursor_row = 0;
        if( st.cursor_row >= (int) st.lines.size() )
            st.cursor_row = (int) st.lines.size() - 1;

        if( codepoint == 0x08 )   //  backspace
        {
            std::string& cur = st.lines[ st.cursor_row ];
            if( st.cursor_col > 0 )
            {
                cur.erase( (std::size_t) st.cursor_col - 1, 1 );
                --st.cursor_col;
            }
            else if( st.cursor_row > 0 )
            {
                //	Join with previous line.
                int const prev_len = (int) st.lines[ st.cursor_row - 1 ].size();
                st.lines[ st.cursor_row - 1 ] += cur;
                st.lines.erase( st.lines.begin() + st.cursor_row );
                --st.cursor_row;
                st.cursor_col = prev_len;
            }
        }
        else if( codepoint == 0x0A || codepoint == 0x0D )   //  enter
        {
            std::string& cur = st.lines[ st.cursor_row ];
            std::string tail = cur.substr( (std::size_t) st.cursor_col );
            cur.resize( (std::size_t) st.cursor_col );
            st.lines.insert( st.lines.begin() + st.cursor_row + 1, tail );
            ++st.cursor_row;
            st.cursor_col = 0;
        }
        else if( codepoint == 0x1B )   //  escape
        {
            //	Revert + unfocus.
            std::vector< std::string > rev;
            std::string cur;
            for( char c : st.original_joined )
            {
                if( c == '\n' ) { rev.push_back( cur ); cur.clear(); }
                else cur.push_back( c );
            }
            rev.push_back( cur );
            st.lines      = rev;
            st.cursor_row = 0;
            st.cursor_col = 0;
            _focused_text_area_id = 0;
        }
        else if( codepoint >= 0x20 && codepoint <= 0x7E )   //  printable ASCII
        {
            std::string& cur = st.lines[ st.cursor_row ];
            cur.insert( (std::size_t) st.cursor_col, 1, (char) codepoint );
            ++st.cursor_col;
        }
    }

    //	c150-A v4 : marked-text composition update from NSTextInputClient.
    //	The composing string replaces the prior buffer wholesale (the IME
    //	system is the authority on what the user is currently typing).
    void on_marked_text( std::string const& composing,
                         int                selection_start,
                         int                selection_length )
    {
        _marked_text.composing        = composing;
        _marked_text.selection_start  = selection_start;
        _marked_text.selection_length = selection_length;
        _marked_text.active           = !composing.empty();
    }

    //	c150-A v4 : commit the marked-text buffer into the focused field.
    //	If a text_input is focused, the composing string is appended to
    //	its buffer respecting its hardcoded 256-char ceiling. If a
    //	text_area is focused, the string is inserted at cursor with
    //	embedded newlines preserved.
    void on_text_committed( std::string const& final_text )
    {
        std::string const payload = final_text.empty()
            ? _marked_text.composing
            : final_text;

        if( _focused_text_input_id != 0 )
        {
            auto it = _text_input_state.find( _focused_text_input_id );
            if( it != _text_input_state.end() )
            {
                TextInputState& st = it->second;
                //	256-char ceiling matches on_text_input's bound.
                std::size_t const room = ( st.buffer.size() < 256 )
                    ? ( 256 - st.buffer.size() ) : 0;
                std::string add = payload;
                if( add.size() > room ) add.resize( room );
                st.buffer += add;
            }
        }
        else if( _focused_text_area_id != 0 )
        {
            auto it = _text_area_state.find( _focused_text_area_id );
            if( it != _text_area_state.end() )
            {
                TextAreaState& st = it->second;
                if( st.lines.empty() ) st.lines.push_back( std::string() );
                if( st.cursor_row < 0 ) st.cursor_row = 0;
                if( st.cursor_row >= (int) st.lines.size() )
                    st.cursor_row = (int) st.lines.size() - 1;

                //	Insert payload char-by-char so embedded newlines split
                //	lines correctly + cursor advances naturally.
                for( char c : payload )
                {
                    if( c == '\n' )
                    {
                        std::string& cur = st.lines[ st.cursor_row ];
                        std::string tail = cur.substr( (std::size_t) st.cursor_col );
                        cur.resize( (std::size_t) st.cursor_col );
                        st.lines.insert( st.lines.begin() + st.cursor_row + 1,
                                         tail );
                        ++st.cursor_row;
                        st.cursor_col = 0;
                    }
                    else
                    {
                        std::string& cur = st.lines[ st.cursor_row ];
                        cur.insert( (std::size_t) st.cursor_col, 1, c );
                        ++st.cursor_col;
                    }
                }
            }
        }
        //	Always clear marked state regardless of focus.
        _marked_text.composing.clear();
        _marked_text.selection_start  = 0;
        _marked_text.selection_length = 0;
        _marked_text.active           = false;
    }

    //	c150-A v4 : test seam accessors.
    std::string current_marked_text() const { return _marked_text.composing; }
    bool        has_marked_text()     const { return _marked_text.active && !_marked_text.composing.empty(); }
    int         marked_text_selection_start()  const { return _marked_text.selection_start; }
    int         marked_text_selection_length() const { return _marked_text.selection_length; }

    std::uint32_t focused_text_area_id() const { return _focused_text_area_id; }

    std::string text_area_value( std::string const& label ) const
    {
        auto it = _text_area_state.find( hash_label( label ) );
        if( it == _text_area_state.end() ) return std::string();
        return join_lines( it->second.lines );
    }

    int text_area_line_count( std::string const& label ) const
    {
        auto it = _text_area_state.find( hash_label( label ) );
        if( it == _text_area_state.end() ) return 0;
        return (int) it->second.lines.size();
    }

    int text_area_visual_row_count( std::string const& label,
                                    int width_chars ) const
    {
        if( width_chars < 1 ) width_chars = 1;
        auto it = _text_area_state.find( hash_label( label ) );
        if( it == _text_area_state.end() ) return 0;
        int total = 0;
        for( std::string const& line : it->second.lines )
        {
            int const n = (int) line.size();
            if( n == 0 ) { total += 1; continue; }
            //	ceil(n / width_chars).
            total += ( n + width_chars - 1 ) / width_chars;
        }
        return total;
    }

    int text_area_cursor_row( std::string const& label ) const
    {
        auto it = _text_area_state.find( hash_label( label ) );
        return ( it == _text_area_state.end() ) ? 0 : it->second.cursor_row;
    }
    int text_area_cursor_col( std::string const& label ) const
    {
        auto it = _text_area_state.find( hash_label( label ) );
        return ( it == _text_area_state.end() ) ? 0 : it->second.cursor_col;
    }
    int text_area_scroll_top( std::string const& label ) const
    {
        auto it = _text_area_state.find( hash_label( label ) );
        return ( it == _text_area_state.end() ) ? 0 : it->second.scroll_top;
    }

    //	Helper : join a vector<string> with "\n" separators. Used by
    //	text_area's return path + the test seam.
    static std::string join_lines( std::vector< std::string > const& lines )
    {
        std::string out;
        for( std::size_t i = 0; i < lines.size(); ++i )
        {
            if( i > 0 ) out.push_back( '\n' );
            out += lines[ i ];
        }
        return out;
    }

    //	Helper : count UTF-8 codepoints in a byte string (used for marked-
    //	text underline width). Counts continuation bytes ; treats invalid
    //	sequences as 1 byte each (defensive).
    static int utf8_char_count( std::string const& s )
    {
        int n = 0;
        for( unsigned char b : s )
        {
            if( ( b & 0xC0 ) != 0x80 ) ++n;
        }
        return n;
    }

    //	---- c148-A v2 Phase 4 : hot-reload button -------------------------
    //	Visually like a regular button + a refresh-arrow icon glyph
    //	drawn as two small triangle quads in the right edge of the
    //	button. On click, calls _meu_runner->reload() if non-null.
    bool hot_reload_button( std::string const& label )
    {
        if( _suppress_widgets ) return false;

        float const w = effective_widget_width();
        float const x = effective_widget_x();
        float const y = next_widget_y();

        std::uint32_t const key = hash_label( label );
        WidgetState& st = _state[ key ];

        bool const hovering = point_in_rect(
            _mouse_x, _mouse_y, x, y, w, kWidgetHeight );

        bool clicked = false;
        if( _mouse_pressed_edge && hovering )
        {
            st.armed = true;
            _had_interaction = true;
        }
        if( _mouse_released_edge )
        {
            if( st.armed && hovering )
            {
                clicked = true;
                _had_interaction = true;
                if( _reload_callback )
                {
                    _reload_callback();
                }
            }
            st.armed = false;
        }
        st.clicked_this_frame = clicked;

        //	Chrome : green tint to set it apart from generic buttons.
        Color4f color;
        if( st.armed && hovering )
            color = { 0.18f, 0.50f, 0.30f, 0.95f };
        else if( hovering )
            color = { 0.30f, 0.65f, 0.42f, 0.95f };
        else
            color = { 0.22f, 0.50f, 0.32f, 0.95f };
        push_quad( _quads, x, y, w, kWidgetHeight, color );

        //	Refresh arrow icon : drawn as two small triangles in the
        //	rightmost 18 px. Simple "rotating arrow" silhouette built
        //	from two stacked rectangles + a tail bar.
        float const ix = x + w - 20.0f;
        float const iy = y + 4.0f;
        Color4f const icon{ 0.95f, 0.95f, 0.95f, 1.0f };
        push_quad( _quads, ix,         iy + 2.0f, 12.0f, 2.0f, icon );   //  top bar
        push_quad( _quads, ix,         iy + 2.0f, 2.0f,  10.0f, icon );  //  left bar
        push_quad( _quads, ix + 10.0f, iy + 8.0f, 2.0f,  6.0f,  icon );  //  right tail

        (void) label;
        ++_widget_count;
        return clicked;
    }

    void set_reload_callback( std::function< void() > cb )
    {
        _reload_callback = std::move( cb );
    }

    //	---- c148-A v2 Phase 4 : collapsing panel --------------------------
    //	Reuses begin_panel's chrome but adds a chevron rect on the LEFT
    //	side of the title bar. Click toggles expanded ; collapsed state
    //	suppresses subsequent widget calls (defensive ; the caller is
    //	expected to skip them via the returned bool).
    bool begin_collapsing_panel( std::string const& title,
                                 float x, float y, float w, float h )
    {
        std::uint32_t const key = hash_label( title );
        PanelExpandedState& st = _panel_expanded_state[ key ];

        //	c149-A v3 Feature 5 : nesting. If a parent on the stack is
        //	collapsed, this nested panel inherits the suppression AND
        //	still shows its own chevron so the user can see + click
        //	it. The depth advance happens regardless ; the suppression
        //	bool is the union of parent-collapsed + own-collapsed.
        bool const parent_collapsed = !_collapsing_nest_stack.empty()
                                      && !_collapsing_nest_stack.back();
        ++_collapsing_nest_depth;
        //	Optional indent (8 px per parent depth level).
        float const indent_x = x + 8.0f * float( _collapsing_nest_depth - 1 );

        //	Hit-test the chevron rect (square on the title bar's left).
        bool const chev_hover = point_in_rect(
            _mouse_x, _mouse_y, indent_x, y, kChevronSide, kWidgetHeight );

        //	When the parent is collapsed we do NOT update interaction
        //	state -- nested panels are invisible-as-far-as-the-user-can-
        //	see ; clicking on their position would land on whatever the
        //	parent draws over.
        if( !parent_collapsed )
        {
            if( _mouse_pressed_edge && chev_hover )
            {
                st.armed = true;
                _had_interaction = true;
            }
            if( _mouse_released_edge )
            {
                if( st.armed && chev_hover )
                {
                    st.expanded = !st.expanded;
                    _had_interaction = true;
                }
                st.armed = false;
            }
        }

        //	Panel rect : when collapsed, draw ONLY the title bar (1 row).
        float const draw_h = st.expanded ? h : kWidgetHeight;

        //	When the parent is collapsed we still PUSH onto the stack
        //	(so end_collapsing_panel pops correctly) but skip drawing.
        if( parent_collapsed )
        {
            _collapsing_nest_stack.push_back( false );   //  inherits collapse
            _suppress_widgets = true;
            (void) title;
            return false;
        }

        _active_panel = true;
        _panel_x = indent_x;
        _panel_y = y;
        _panel_w = w - 8.0f * float( _collapsing_nest_depth - 1 );
        _panel_h = draw_h;
        _cursor_y = y + kWidgetHeight + kWidgetSpacing;

        Color4f const bg{ 0.10f, 0.10f, 0.12f, 0.80f };
        Color4f const hdr{ 0.20f, 0.22f, 0.30f, 0.95f };
        push_quad( _quads, indent_x, y, _panel_w, draw_h, bg );
        push_quad( _quads, indent_x, y, _panel_w, kWidgetHeight, hdr );

        //	Chevron : `v` (expanded, points down) drawn as two diagonal
        //	bars approximated by 2x2 rectangles ; `>` (collapsed, points
        //	right) drawn as two diagonal bars. Both inside the chevron
        //	rect on the left side of the title bar.
        Color4f const chev_col = chev_hover
            ? Color4f{ 1.0f, 1.0f, 1.0f, 1.0f }
            : Color4f{ 0.75f, 0.78f, 0.85f, 1.0f };
        float const cx = indent_x + 4.0f;
        float const cy = y + 5.0f;
        if( st.expanded )
        {
            //	`v` : two bars sloping inward.
            push_quad( _quads, cx,        cy,        4.0f, 4.0f, chev_col );
            push_quad( _quads, cx + 10.0f, cy,        4.0f, 4.0f, chev_col );
            push_quad( _quads, cx + 5.0f,  cy + 6.0f, 4.0f, 4.0f, chev_col );
        }
        else
        {
            //	`>` : two bars sloping outward.
            push_quad( _quads, cx,        cy,        4.0f, 4.0f, chev_col );
            push_quad( _quads, cx,        cy + 10.0f, 4.0f, 4.0f, chev_col );
            push_quad( _quads, cx + 6.0f, cy + 5.0f,  4.0f, 4.0f, chev_col );
        }

        //	Defensive belt-and-braces : suppress subsequent widget calls
        //	when collapsed. New widgets honor this gate ; Phase 1 widgets
        //	don't (no structural touch) -- but Lua scripts are expected
        //	to skip them via the returned bool anyway.
        _suppress_widgets = !st.expanded;
        _collapsing_nest_stack.push_back( st.expanded );

        (void) title;
        return st.expanded;
    }

    void end_collapsing_panel()
    {
        if( !_collapsing_nest_stack.empty() )
        {
            _collapsing_nest_stack.pop_back();
        }
        if( _collapsing_nest_depth > 0 )
        {
            --_collapsing_nest_depth;
        }
        //	c149-A v3 Feature 5 : nested panels. After popping, the new
        //	suppress state is the union of any remaining ancestor's
        //	collapsed flag. If the stack is empty we're outside ALL
        //	collapsing panels -- clear suppression.
        bool any_collapsed = false;
        for( bool expanded : _collapsing_nest_stack )
        {
            if( !expanded ) { any_collapsed = true; break; }
        }
        _suppress_widgets = any_collapsed;
        if( _collapsing_nest_stack.empty() )
        {
            _active_panel = false;
        }
    }

    bool is_modal_open() const { return _modal_state.is_open; }

    bool is_panel_expanded( std::string const& title ) const
    {
        auto it = _panel_expanded_state.find( hash_label( title ) );
        if( it == _panel_expanded_state.end() ) return true;   //  default
        return it->second.expanded;
    }

    //	---- c149-A v3 Feature 4 : serialization accessors ----------------

    int collapsing_nest_depth() const { return _collapsing_nest_depth; }

    float slider_value( std::string const& label ) const
    {
        auto it = _state.find( hash_label( label ) );
        return ( it == _state.end() ) ? 0.0f : it->second.last_v;
    }

    Color4f hsv_picker_value( std::string const& label ) const
    {
        auto it = _hsv_picker_state.find( hash_label( label ) );
        if( it == _hsv_picker_state.end() ) return Color4f{};
        Color4f out;
        float rr, gg, bb;
        hsv_to_rgb( it->second.h, it->second.s, it->second.v, rr, gg, bb );
        out.r = rr; out.g = gg; out.b = bb; out.a = it->second.a;
        return out;
    }

    int color_well_index( std::string const& label ) const
    {
        auto it = _state.find( hash_label( label ) );
        return ( it == _state.end() ) ? 0 : it->second.preset_ix;
    }

    std::string text_input_value( std::string const& label ) const
    {
        auto it = _text_input_state.find( hash_label( label ) );
        return ( it == _text_input_state.end() ) ? std::string() : it->second.buffer;
    }

    bool panel_expanded( std::string const& title ) const
    {
        return is_panel_expanded( title );
    }

    void set_slider_value( std::string const& label, float value )
    {
        WidgetState& st = _state[ hash_label( label ) ];
        st.last_v = value;
    }

    void set_hsv_picker_value( std::string const& label, Color4f rgba )
    {
        HsvPickerState& st = _hsv_picker_state[ hash_label( label ) ];
        rgb_to_hsv( rgba.r, rgba.g, rgba.b, st.h, st.s, st.v );
        st.a = rgba.a;
        st.inited = true;
    }

    void set_color_well_index( std::string const& label, int index )
    {
        if( index < 0 ) index = 0;
        if( index >= kColorPresetCount ) index = kColorPresetCount - 1;
        WidgetState& st = _state[ hash_label( label ) ];
        st.preset_ix = index;
    }

    void set_text_input_value( std::string const& label, std::string const& value )
    {
        TextInputState& st = _text_input_state[ hash_label( label ) ];
        st.buffer   = value;
        st.original = value;
        st.primed   = true;
    }

    void set_panel_expanded( std::string const& title, bool expanded )
    {
        PanelExpandedState& st = _panel_expanded_state[ hash_label( title ) ];
        st.expanded = expanded;
    }

    void set_drop_target_highlight( bool on ) { _drop_target_highlight = on; }
    bool drop_target_highlight() const        { return _drop_target_highlight; }

    void end_frame()
    {
        if( _backend == nullptr ) return;

        //	c149-A v3 Feature 1 : drop-target highlight. Emit a 4px
        //	colored border around the viewport's edge whenever the host
        //	(MTKView's NSDraggingDestination delegate) has set the flag.
        //	Drawn LAST so it composites on top of any widget chrome.
        if( _drop_target_highlight )
        {
            float const W = float( _w );
            float const H = float( _h );
            float const T = 4.0f;
            Color4f const c{ 0.30f, 0.75f, 0.90f, 0.85f };
            push_quad( _quads, 0.0f, 0.0f,    W, T,        c );   //  top
            push_quad( _quads, 0.0f, H - T,   W, T,        c );   //  bottom
            push_quad( _quads, 0.0f, 0.0f,    T, H,        c );   //  left
            push_quad( _quads, W - T, 0.0f,   T, H,        c );   //  right
        }

        if( _quads.empty() ) return;

        ensure_gpu_resources();
        if( _program == GOL::kInvalidProgramId ) return;
        if( _vbuf    == GOL::kInvalidBufferId )  return;
        if( _sbuf    == GOL::kInvalidBufferId )  return;

        //	Upload screen size + vertices, then issue one draw_arrays call.
        ScreenSize ss{ float( _w ), float( _h ), 0.0f, 0.0f };
        _backend->buffer_data( _sbuf, sizeof( ss ), &ss,
                               GOL::BufferUsage::Static );

        //	Grow GPU vertex buffer if our queued count exceeds the
        //	last allocation. We over-allocate by 2x to avoid frequent
        //	re-uploads.
        std::size_t const want = _quads.size() * sizeof( QuadVertex );
        if( want > _vbuf_size )
        {
            _vbuf_size = want * 2;
            std::vector< QuadVertex > zeros( _vbuf_size / sizeof( QuadVertex ),
                                             QuadVertex{} );
            _backend->buffer_data( _vbuf, _vbuf_size, zeros.data(),
                                   GOL::BufferUsage::Dynamic );
        }
        _backend->buffer_data( _vbuf, want, _quads.data(),
                               GOL::BufferUsage::Dynamic );

        _backend->bind_program( _program );
        _backend->bind_vertex_buffer( _vbuf, 0, 0 );
        _backend->bind_vertex_buffer( _sbuf, 1, 0 );
        _backend->draw_arrays( GOL::PrimitiveType::Triangles, 0,
                               std::uint32_t( _quads.size() ) );
    }

    int  last_frame_widget_count() const   { return _widget_count;   }
    bool last_frame_had_interaction() const { return _had_interaction; }

    bool is_button_armed( std::string const& label ) const
    {
        auto it = _state.find( hash_label( label ) );
        return ( it == _state.end() ) ? false : it->second.armed;
    }

    float slider_drag_delta_pixels( std::string const& label ) const
    {
        auto it = _state.find( hash_label( label ) );
        return ( it == _state.end() ) ? 0.0f : it->second.drag_delta_px;
    }

private:
    bool rgba_matches_preset_zero( Color4f const& c )
    {
        Color4f const& p = kColorPresets[ 0 ];
        return ( c.r == p.r ) && ( c.g == p.g )
            && ( c.b == p.b ) && ( c.a == p.a );
    }

    float effective_widget_x() const
    {
        return _panel_x + kPanelPad;
    }

    float effective_widget_width() const
    {
        float w = _panel_w - 2.0f * kPanelPad;
        if( w < 40.0f ) w = 40.0f;
        return w;
    }

    float next_widget_y()
    {
        float const y = _cursor_y;
        _cursor_y += kWidgetHeight + kWidgetSpacing;
        return y;
    }

    void ensure_gpu_resources()
    {
        if( _backend == nullptr ) return;
        if( _program == GOL::kInvalidProgramId )
        {
            GOL::VertexAttribute attrs[ 2 ];
            attrs[ 0 ].shader_location = 0;
            attrs[ 0 ].buffer_slot     = 0;
            attrs[ 0 ].offset          = offsetof( QuadVertex, x );
            attrs[ 0 ].format          = GOL::VertexFormat::Float2;
            attrs[ 1 ].shader_location = 1;
            attrs[ 1 ].buffer_slot     = 0;
            attrs[ 1 ].offset          = offsetof( QuadVertex, r );
            attrs[ 1 ].format          = GOL::VertexFormat::Float4;

            GOL::VertexBufferLayout layouts[ 1 ];
            layouts[ 0 ].slot   = 0;
            layouts[ 0 ].stride = sizeof( QuadVertex );

            GOL::ProgramDesc pd;
            pd.msl_source       = kWidgetMsl;
            pd.vertex_entry     = "vs_main";
            pd.fragment_entry   = "fs_main";
            pd.target_color_fmt = GOL::TextureFormat::BGRA8;
            pd.attributes       = attrs;
            pd.attribute_nb     = 2;
            pd.layouts          = layouts;
            pd.layout_nb        = 1;
            pd.blend_mode       = GOL::BlendMode::AlphaBlend;
            _program = _backend->create_program( pd );
            if( _program == GOL::kInvalidProgramId )
            {
                NSLog( @"aaa::ui::widgets : program compile failed : %s",
                       _backend->get_last_error().c_str() );
            }
        }
        if( _vbuf == GOL::kInvalidBufferId )
        {
            _vbuf = _backend->gen_buffer();
        }
        if( _sbuf == GOL::kInvalidBufferId )
        {
            _sbuf = _backend->gen_buffer();
        }
    }

    void teardown_gpu()
    {
        if( _backend == nullptr ) return;
        if( _program != GOL::kInvalidProgramId )
        {
            _backend->delete_program( _program );
            _program = GOL::kInvalidProgramId;
        }
        if( _vbuf != GOL::kInvalidBufferId )
        {
            _backend->delete_buffer( _vbuf );
            _vbuf = GOL::kInvalidBufferId;
        }
        if( _sbuf != GOL::kInvalidBufferId )
        {
            _backend->delete_buffer( _sbuf );
            _sbuf = GOL::kInvalidBufferId;
        }
    }

private:
    GOL::Backend* _backend = nullptr;

    //	Per-frame state.
    std::uint32_t _w = 0;
    std::uint32_t _h = 0;
    double        _mouse_x = 0.0;
    double        _mouse_y = 0.0;
    bool          _mouse_held = false;
    bool          _mouse_pressed_edge  = false;
    bool          _mouse_released_edge = false;

    //	Active panel + layout cursor.
    bool          _active_panel  = false;
    float         _panel_x = 0.0f, _panel_y = 0.0f;
    float         _panel_w = 0.0f, _panel_h = 0.0f;
    float         _cursor_y = 0.0f;
    std::uint32_t _next_panel_id = 1;

    //	Per-frame draw queue + counters.
    std::vector< QuadVertex > _quads;
    int                       _widget_count   = 0;
    bool                      _had_interaction = false;

    //	Retained per-widget state (drag, armed, color preset index).
    std::unordered_map< std::uint32_t, WidgetState > _state;

    //	c148-A retained state for v2 Phase 2-4 widgets.
    std::unordered_map< std::uint32_t, HsvPickerState >    _hsv_picker_state;
    std::unordered_map< std::uint32_t, PanelExpandedState > _panel_expanded_state;
    std::unordered_map< std::uint32_t, TextInputState >    _text_input_state;
    ModalGlobalState                                       _modal_state;
    std::uint32_t                                          _focused_text_input_id = 0;

    //	c150-A v4 : text_area retained state + focus tracking + marked-text
    //	composition buffer. Focus is mutually exclusive with the single-
    //	line text input ; whichever was clicked last wins. Marked text is
    //	a single global (the macOS IME never composes into more than one
    //	field at a time).
    std::unordered_map< std::uint32_t, TextAreaState >     _text_area_state;
    std::uint32_t                                          _focused_text_area_id = 0;
    MarkedTextState                                        _marked_text;

    //	c148-A : reload callback for the hot-reload button. Indirection
    //	via std::function avoids a link cycle with aaaseed_meu_runner
    //	(which already PUBLIC-links the widget library).
    std::function< void() > _reload_callback;

    //	c148-A : frame counter for cursor-blink animation. Bumped on
    //	each begin_frame ; survives across frames.
    std::uint32_t _frame_count = 0;

    //	c148-A : when true, widget calls between begin/end skip emitting
    //	chrome. Lua scripts are expected to skip their widget calls
    //	entirely on the basis of begin_collapsing_panel's returned
    //	expanded bool, so this is a defensive belt-and-braces only ; it
    //	gates NEW widgets (HSV / text input / hot-reload) and leaves
    //	Phase 1 widgets alone (no structural touch).
    bool _suppress_widgets = false;

public:
    //	c149-A v3 Feature 5 : collapsing-panel nesting tracker.
    //	begin_collapsing_panel pushes onto _collapsing_nest_stack with
    //	the panel's chosen `expanded` flag. Nested panels inherit the
    //	parent's collapsed-suppression : if any frame on the stack is
    //	collapsed, ALL inner widgets are suppressed. end_collapsing_panel
    //	pops one frame.
    int   _collapsing_nest_depth = 0;
    std::vector< bool > _collapsing_nest_stack;

    //	c149-A v3 Feature 1 : drop-target highlight flag. When true,
    //	end_frame paints a 4px colored border on the viewport's edge.
    bool _drop_target_highlight = false;

private:
    //	GPU resources.
    GOL::ProgramId _program  = GOL::kInvalidProgramId;
    GOL::BufferId  _vbuf     = GOL::kInvalidBufferId;
    std::size_t    _vbuf_size = 0;
    GOL::BufferId  _sbuf     = GOL::kInvalidBufferId;
};

//	---- Public facade ----------------------------------------------------

WidgetSystem::WidgetSystem( GOL::Backend* backend )
    : _impl( new WidgetSystemImpl( backend ) )
{}

WidgetSystem::~WidgetSystem()
{
    delete _impl;
    _impl = nullptr;
}

void WidgetSystem::begin_frame( std::uint32_t width, std::uint32_t height,
                                double mouse_x, double mouse_y,
                                bool mouse_pressed_now, bool mouse_released_now )
{
    if( _impl ) _impl->begin_frame( width, height, mouse_x, mouse_y,
                                    mouse_pressed_now, mouse_released_now );
}

void WidgetSystem::end_frame()
{
    if( _impl ) _impl->end_frame();
}

PanelHandle WidgetSystem::begin_panel( std::string const& title,
                                       float x, float y, float w, float h )
{
    return _impl ? _impl->begin_panel( title, x, y, w, h ) : PanelHandle{};
}

void WidgetSystem::end_panel()
{
    if( _impl ) _impl->end_panel();
}

float WidgetSystem::slider( std::string const& label,
                            float value, float min, float max )
{
    return _impl ? _impl->slider( label, value, min, max ) : value;
}

bool WidgetSystem::button( std::string const& label )
{
    return _impl ? _impl->button( label ) : false;
}

Color4f WidgetSystem::color_well( std::string const& label, Color4f rgba )
{
    return _impl ? _impl->color_well( label, rgba ) : rgba;
}

int WidgetSystem::last_frame_widget_count() const
{
    return _impl ? _impl->last_frame_widget_count() : 0;
}

bool WidgetSystem::last_frame_had_interaction() const
{
    return _impl ? _impl->last_frame_had_interaction() : false;
}

bool WidgetSystem::is_button_armed( std::string const& label ) const
{
    return _impl ? _impl->is_button_armed( label ) : false;
}

float WidgetSystem::slider_drag_delta_pixels( std::string const& label ) const
{
    return _impl ? _impl->slider_drag_delta_pixels( label ) : 0.0f;
}

//	---- c148-A v2 Phase 2-4 facade dispatch ------------------------------

Color4f WidgetSystem::hsv_color_picker( std::string const& label, Color4f rgba )
{
    return _impl ? _impl->hsv_color_picker( label, rgba ) : rgba;
}

WidgetSystem::ModalResult WidgetSystem::begin_modal(
    std::string const& title, float w, float h )
{
    return _impl ? _impl->begin_modal( title, w, h ) : ModalResult{};
}

void WidgetSystem::end_modal()
{
    if( _impl ) _impl->end_modal();
}

void WidgetSystem::show_modal( std::string const& title )
{
    if( _impl ) _impl->show_modal( title );
}

std::string WidgetSystem::text_input( std::string const& label,
                                      std::string const& current_value,
                                      std::size_t max_length )
{
    return _impl ? _impl->text_input( label, current_value, max_length )
                 : current_value;
}

void WidgetSystem::on_text_input( std::uint32_t codepoint )
{
    if( _impl ) _impl->on_text_input( codepoint );
}

std::uint32_t WidgetSystem::focused_text_input_id() const
{
    return _impl ? _impl->focused_text_input_id() : 0;
}

bool WidgetSystem::hot_reload_button( std::string const& label )
{
    return _impl ? _impl->hot_reload_button( label ) : false;
}

void WidgetSystem::set_reload_callback( std::function< void() > cb )
{
    if( _impl ) _impl->set_reload_callback( std::move( cb ) );
}

bool WidgetSystem::begin_collapsing_panel( std::string const& title,
                                           float x, float y, float w, float h )
{
    return _impl ? _impl->begin_collapsing_panel( title, x, y, w, h ) : true;
}

void WidgetSystem::end_collapsing_panel()
{
    if( _impl ) _impl->end_collapsing_panel();
}

bool WidgetSystem::is_modal_open() const
{
    return _impl ? _impl->is_modal_open() : false;
}

bool WidgetSystem::is_panel_expanded( std::string const& title ) const
{
    return _impl ? _impl->is_panel_expanded( title ) : true;
}

//	---- c149-A v3 facade dispatch ---------------------------------------

int WidgetSystem::collapsing_nest_depth() const
{
    return _impl ? _impl->collapsing_nest_depth() : 0;
}

float WidgetSystem::slider_value( std::string const& label ) const
{
    return _impl ? _impl->slider_value( label ) : 0.0f;
}

Color4f WidgetSystem::hsv_picker_value( std::string const& label ) const
{
    return _impl ? _impl->hsv_picker_value( label ) : Color4f{};
}

int WidgetSystem::color_well_index( std::string const& label ) const
{
    return _impl ? _impl->color_well_index( label ) : 0;
}

std::string WidgetSystem::text_input_value( std::string const& label ) const
{
    return _impl ? _impl->text_input_value( label ) : std::string();
}

bool WidgetSystem::panel_expanded( std::string const& title ) const
{
    return _impl ? _impl->panel_expanded( title ) : true;
}

void WidgetSystem::set_slider_value( std::string const& label, float value )
{
    if( _impl ) _impl->set_slider_value( label, value );
}

void WidgetSystem::set_hsv_picker_value( std::string const& label, Color4f rgba )
{
    if( _impl ) _impl->set_hsv_picker_value( label, rgba );
}

void WidgetSystem::set_color_well_index( std::string const& label, int index )
{
    if( _impl ) _impl->set_color_well_index( label, index );
}

void WidgetSystem::set_text_input_value( std::string const& label, std::string const& value )
{
    if( _impl ) _impl->set_text_input_value( label, value );
}

void WidgetSystem::set_panel_expanded( std::string const& title, bool expanded )
{
    if( _impl ) _impl->set_panel_expanded( title, expanded );
}

void WidgetSystem::set_drop_target_highlight( bool on )
{
    if( _impl ) _impl->set_drop_target_highlight( on );
}

bool WidgetSystem::drop_target_highlight() const
{
    return _impl ? _impl->drop_target_highlight() : false;
}

//	---- c150-A v4 facade dispatch ---------------------------------------

std::string WidgetSystem::text_area( std::string const& label,
                                     std::string const& current_value,
                                     int                visible_lines,
                                     int                width_chars,
                                     std::size_t        max_length )
{
    return _impl ? _impl->text_area( label, current_value, visible_lines,
                                     width_chars, max_length )
                 : current_value;
}

void WidgetSystem::on_marked_text( std::string const& composing,
                                   int                selection_start,
                                   int                selection_length )
{
    if( _impl ) _impl->on_marked_text( composing, selection_start, selection_length );
}

void WidgetSystem::on_text_committed( std::string const& final_text )
{
    if( _impl ) _impl->on_text_committed( final_text );
}

std::string WidgetSystem::current_marked_text() const
{
    return _impl ? _impl->current_marked_text() : std::string();
}

bool WidgetSystem::has_marked_text() const
{
    return _impl ? _impl->has_marked_text() : false;
}

int WidgetSystem::marked_text_selection_start() const
{
    return _impl ? _impl->marked_text_selection_start() : 0;
}

int WidgetSystem::marked_text_selection_length() const
{
    return _impl ? _impl->marked_text_selection_length() : 0;
}

std::string WidgetSystem::text_area_value( std::string const& label ) const
{
    return _impl ? _impl->text_area_value( label ) : std::string();
}

int WidgetSystem::text_area_line_count( std::string const& label ) const
{
    return _impl ? _impl->text_area_line_count( label ) : 0;
}

int WidgetSystem::text_area_visual_row_count( std::string const& label,
                                              int width_chars ) const
{
    return _impl ? _impl->text_area_visual_row_count( label, width_chars ) : 0;
}

int WidgetSystem::text_area_cursor_row( std::string const& label ) const
{
    return _impl ? _impl->text_area_cursor_row( label ) : 0;
}

int WidgetSystem::text_area_cursor_col( std::string const& label ) const
{
    return _impl ? _impl->text_area_cursor_col( label ) : 0;
}

int WidgetSystem::text_area_scroll_top( std::string const& label ) const
{
    return _impl ? _impl->text_area_scroll_top( label ) : 0;
}

std::uint32_t WidgetSystem::focused_text_area_id() const
{
    return _impl ? _impl->focused_text_area_id() : 0;
}

}   //	namespace widgets
}   //	namespace ui
}   //	namespace aaa
