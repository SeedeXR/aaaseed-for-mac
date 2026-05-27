// src/ui/widgets/aaa_widgets_mac.h
//
// c147-A : Mac-native immediate-mode widget UI rendered INSIDE the
// MTKView. v2 Phase 1 of the authoring surface. Closes the c146 ship-
// gate "Authoring Surface DEFERRED v2" open item per c145-C Option A.
//
// Three widget primitives this phase ships :
//   - SLIDER     : drag a thumb across a horizontal bar -> value in
//                   [min, max].
//   - BUTTON     : click + release inside a rect -> returns true once.
//   - COLOR-WELL : click cycles through 8 preset RGBA colors.
//                  (Full picker is v2 Phase 2.)
//
// Hermetic Mac sub-lib per feedback_hermetic_mac_sublibs.md :
//   - Pure C++ header (no ObjC types) ; .mm impl uses Foundation +
//     metal-cpp + GOL::Backend + c87 text helpers only.
//   - NO o_str / aaa_mem.h / aaa_str.h.
//   - std::string / std::vector / std::unordered_map / <cstdint> only.
//
// Lifetime contract :
//   - Caller constructs WidgetSystem with a non-owning Backend pointer
//     (the backend must outlive the widget system).
//   - Each frame : begin_frame -> widget calls (return interaction
//     state) -> end_frame (emits draw calls against the active encoder).
//   - The widget system does NOT begin / end the render pass and does
//     NOT call present_window -- those stay with the host (c140 doctrine).
//
// Retained state :
//   - Each widget is keyed by a hash of its label string. Drag state
//     for sliders + click latches for buttons persist across frames in
//     an unordered_map keyed by that hash.

#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace GOL
{
    class Backend;
}

namespace aaa
{
namespace ui
{
namespace widgets
{

//	Handle returned by begin_panel. `valid` is false when the call was
//	rejected (e.g. begin_panel called outside begin_frame / end_frame).
struct PanelHandle
{
    std::uint32_t id    = 0;
    bool          valid = false;
};

//	RGBA in [0,1]. Mirrors GOL's clear_color shape but lives in widget
//	namespace so consumers don't need to pull in gol_backend.h.
struct Color4f
{
    float r = 1.0f;
    float g = 1.0f;
    float b = 1.0f;
    float a = 1.0f;
};

class WidgetSystemImpl;

//	The widget system. One instance per MTKView. Constructed AFTER the
//	GOL::Backend has been initialised. The host (AAASeedMTKView)
//	maintains the lifetime ; the MEU runner holds a non-owning pointer
//	via set_widget_system() so Lua bindings can route into it.
class WidgetSystem
{
public:
    explicit WidgetSystem( GOL::Backend* backend );
    ~WidgetSystem();

    WidgetSystem( WidgetSystem const& )            = delete;
    WidgetSystem& operator=( WidgetSystem const& ) = delete;

    //	Lifecycle -------------------------------------------------------

    //	Begin a new frame. width/height are the drawable size in pixels.
    //	mouse_x/mouse_y are in the same coordinate space the host
    //	forwards (top-left origin, pixel units). mouse_pressed_now /
    //	mouse_released_now are EDGE flags : true only on the frame the
    //	host saw the press / release event. Held state is tracked
    //	internally between events.
    void begin_frame( std::uint32_t width, std::uint32_t height,
                      double mouse_x, double mouse_y,
                      bool mouse_pressed_now, bool mouse_released_now );

    //	Emit queued draws against the active GOL render pass + flush
    //	per-frame state. Safe to call when begin_frame returned no
    //	widgets (no-op).
    void end_frame();

    //	Panels (containers) --------------------------------------------

    PanelHandle begin_panel( std::string const& title,
                             float x, float y, float w, float h );
    void        end_panel();

    //	Widgets (return value mutates on interaction) ------------------

    //	Horizontal slider. Returns the post-interaction value clamped to
    //	[min, max]. When the user drags the thumb, the returned value
    //	moves proportionally to the cursor's horizontal position over
    //	the slider's track.
    float slider( std::string const& label, float value, float min, float max );

    //	Button. Returns true exactly on the frame the user clicked AND
    //	released inside the button's rect. Subsequent frames return
    //	false until the next click+release cycle.
    bool button( std::string const& label );

    //	Color well. Click cycles through 8 preset colors :
    //	  red / orange / yellow / green / cyan / blue / magenta / white.
    //	Returns the current color. For v2 Phase 2 this becomes a full
    //	picker ; the cycle-through-presets behaviour stays as an
    //	authoring shortcut.
    Color4f color_well( std::string const& label, Color4f rgba );

    //	c148-A v2 Phase 2 : HSV color picker. Visual surface :
    //	  - 2D saturation-value square (~128 x 128 px), drag updates SV.
    //	  - 1D hue bar (~16 x 128 px), drag updates hue.
    //	  - alpha strip + RGB readout band.
    //	Returns the post-interaction RGBA. Retained HSV state lives in
    //	_hsv_picker_state keyed by hash(label). Algorithm : Smith 1978
    //	"Color Gamut Transform Pairs" RGB<->HSV (see .mm for inline cite).
    Color4f hsv_color_picker( std::string const& label, Color4f rgba );

    //	c148-A v2 Phase 3 : modal dialog. begin_modal returns the modal's
    //	open/ok/cancel state. The modal is centered on the viewport at
    //	width w + height h ; widget calls between begin_modal/end_modal
    //	are draw INSIDE the modal frame. Modal opens via show_modal()
    //	and closes when ok / cancel buttons fire.
    struct ModalResult
    {
        bool open          = false;   //  true on every frame the modal is showing
        bool ok_clicked    = false;   //  edge : true on the frame OK was clicked
        bool cancel_clicked = false;  //  edge : true on the frame Cancel clicked
    };
    ModalResult begin_modal( std::string const& title, float w, float h );
    void        end_modal();

    //	Open the named modal. Called from script (`aaa.ui.show_modal("x")`).
    //	After this fires, subsequent begin_modal calls with the same title
    //	return open=true until ok/cancel closes it.
    void show_modal( std::string const& title );

    //	c148-A v2 Phase 3 : single-line text input. Returns the buffer
    //	post-edit. Click to focus ; printable ASCII inserts ; backspace
    //	deletes the trailing char ; Enter commits + unfocuses ; Escape
    //	reverts to the original value + unfocuses. Bounded by max_length
    //	(default 64). Cursor blinks (frame_count % 60 < 30).
    std::string text_input( std::string const& label,
                            std::string const& current_value,
                            std::size_t max_length = 64 );

    //	Forward a keyboard codepoint into the focused text input. The
    //	host (AAASeedInputView) calls this from insertText: /
    //	deleteBackward: / insertNewline: / cancelOperation: with the
    //	matching codepoint (printable ASCII 0x20-0x7E, backspace 0x08,
    //	enter 0x0A, escape 0x1B). No-op if no text input is focused.
    void on_text_input( std::uint32_t codepoint );

    //	c150-A v4 : multi-line text widget. Word-wraps within `width_chars`
    //	visual columns ; allows newlines via Enter ; max_length covers
    //	total characters across all lines. Click to focus ; same key
    //	dispatch as text_input (printable, backspace, newline, escape).
    //	When focused, the widget receives keystrokes via on_text_input ;
    //	0x08 at column 0 joins with the previous line. Returns the
    //	current buffer (with embedded "\n" between lines).
    std::string text_area( std::string const& label,
                           std::string const& current_value,
                           int                visible_lines = 4,
                           int                width_chars   = 32,
                           std::size_t        max_length    = 512 );

    //	c150-A v4 : NSTextInputClient marked-text (IME composition) path.
    //	The host (AAASeedInputView) calls on_marked_text whenever the
    //	macOS Text Input System forwards an updated composition string
    //	(e.g. a half-typed Japanese hiragana, or a pinyin spelling-out
    //	before pinyin->Hanzi commit). on_text_committed flushes the
    //	current marked-text buffer into the focused text_input or
    //	text_area at cursor position + clears the marked state.
    //
    //	`composing` is UTF-8 ; `selection_start` / `selection_length` are
    //	character offsets into the composing string (used by the renderer
    //	to draw the IME's caret-style cursor inside the composition).
    void on_marked_text( std::string const& composing,
                         int                selection_start,
                         int                selection_length );
    void on_text_committed( std::string const& final_text );

    //	c150-A v4 : test seam + protocol-query support. The InputView's
    //	hasMarkedText / markedRange / selectedRange protocol methods read
    //	these. ASCII-only callers + IME bypass scripts can ignore.
    std::string current_marked_text()      const;
    bool        has_marked_text()          const;
    int         marked_text_selection_start() const;
    int         marked_text_selection_length() const;

    //	c150-A v4 : id of the focused text_area widget (0 = none). The
    //	host (AAASeedInputView) reads this to expand the NSTextInputContext
    //	gate so the IME chain routes through interpretKeyEvents: whenever
    //	any text widget is focused.
    std::uint32_t focused_text_area_id() const;

    //	c150-A v4 test seam : query a text_area widget's current value
    //	and the number of visual rows it would render given a width-chars
    //	wrap setting. Used by unit tests that need to verify wrap +
    //	multi-line behaviour without driving the GPU draw.
    std::string text_area_value( std::string const& label ) const;
    int         text_area_line_count( std::string const& label ) const;
    int         text_area_visual_row_count( std::string const& label,
                                            int width_chars ) const;
    int         text_area_cursor_row( std::string const& label ) const;
    int         text_area_cursor_col( std::string const& label ) const;
    int         text_area_scroll_top( std::string const& label ) const;

    //	Test-only : query the focused text input's hash id. Returns 0
    //	when no input is focused. Used by integration tests.
    std::uint32_t focused_text_input_id() const;

    //	c148-A v2 Phase 4 : hot-reload button. Looks like a regular
    //	button but with a refresh-arrow icon. On click, calls
    //	`_meu_runner->reload()` (if set). Returns true on click frame.
    bool hot_reload_button( std::string const& label = "Reload MEU" );

    //	Install a callback for the hot-reload button's "on click"
    //	action. The host (AAASeedMTKView) wraps its `Runner::reload()`
    //	in a `[runner]{ runner->reload(); }` lambda + passes it here ;
    //	this indirection avoids a static-archive link cycle between
    //	aaaseed_ui_widgets_mac and aaaseed_meu_runner (the runner
    //	library already PUBLIC-links the widget library). Set to a
    //	default-constructed function to clear -- the button still
    //	draws + reports clicks, the reload simply becomes a no-op.
    void set_reload_callback( std::function< void() > cb );

    //	c148-A v2 Phase 4 : collapsing panel. Like begin_panel but with
    //	a chevron next to the title that toggles expanded/collapsed on
    //	click. Returns true when EXPANDED (the caller should run its
    //	widget calls). Returns false when collapsed (caller skips). The
    //	collapsed state persists across frames via _panel_expanded_state.
    bool begin_collapsing_panel( std::string const& title,
                                 float x, float y, float w, float h );
    void end_collapsing_panel();

    //	Diagnostics (used by tests + the HUD) ---------------------------

    //	Number of widgets emitted during the most recent
    //	begin_frame .. end_frame pair. Resets at begin_frame.
    int  last_frame_widget_count() const;

    //	True when ANY widget reported interaction (hover hit-test OR
    //	click) during the most recent frame. Used by the HUD to indicate
    //	"UI is responsive" and by tests to verify mouse routing.
    bool last_frame_had_interaction() const;

    //	Test-only : query a button's "armed" state (mouse was pressed
    //	inside but not yet released). Exposed for the
    //	`ButtonClickReturnsTrueOnce` unit test.
    bool is_button_armed( std::string const& label ) const;

    //	Test-only : observed drag delta for a slider since its last
    //	begin_frame. Returns 0 when the slider isn't being dragged or
    //	hasn't been created yet. Used by SliderDragsOnHorizontalMouseMove.
    float slider_drag_delta_pixels( std::string const& label ) const;

    //	c148-A test-only : is the modal open ? Returns true between
    //	show_modal() and the frame OK/Cancel fires that closes it.
    bool is_modal_open() const;

    //	c148-A test-only : is a collapsing panel currently expanded ?
    //	Default (never-clicked) panels start EXPANDED. Returns true if
    //	the panel keyed by hash(title) is in the expanded state.
    bool is_panel_expanded( std::string const& title ) const;

    //	c149-A v3 Feature 5 : current collapsing-panel nesting depth.
    //	0 outside any begin_collapsing_panel ; 1 inside one ; 2 inside a
    //	nested begin_collapsing_panel inside another, etc. Used by
    //	tests + diagnostic HUDs.
    int  collapsing_nest_depth() const;

    //	c149-A v3 Feature 4 : serialization accessors for preset save/load.
    //	Returns COPIES of the retained widget state, keyed by the
    //	user-supplied label string. Sliders + text inputs use the
    //	per-widget retained value ; HSV pickers + color wells return the
    //	current color in RGBA ; panel-expanded tracks the collapsing-panel
    //	state map. Pure read-only ; the widget renderer is not affected.
    //
    //	Labels are reconstructed by hashing rather than reverse-hashing :
    //	the widget system retains state by hash(label), so the
    //	serialization caller (Runner::save_preset) passes IN the label
    //	list it knows about (from its own bookkeeping). To keep the
    //	WidgetSystem API simple, we expose per-label getters instead --
    //	the Runner caches each (label, kind) tuple as scripts call into
    //	the widget bindings, so it can replay them all here.
    float                                slider_value     ( std::string const& label ) const;
    Color4f                              hsv_picker_value ( std::string const& label ) const;
    int                                  color_well_index ( std::string const& label ) const;
    std::string                          text_input_value ( std::string const& label ) const;
    bool                                 panel_expanded   ( std::string const& title ) const;

    //	c149-A : direct setters used by load_preset to RESTORE retained
    //	state without going through the widget's interaction loop. The
    //	widget system silently creates the state entry if missing ; the
    //	caller's label must match a future widget call (otherwise the
    //	state is dead-allocated but harmless).
    void                                 set_slider_value     ( std::string const& label, float value );
    void                                 set_hsv_picker_value ( std::string const& label, Color4f rgba );
    void                                 set_color_well_index ( std::string const& label, int index );
    void                                 set_text_input_value ( std::string const& label, std::string const& value );
    void                                 set_panel_expanded   ( std::string const& title, bool expanded );

    //	c149-A v3 Feature 1 : drag-drop visual feedback. When set true,
    //	end_frame() emits a 4px colored border on the viewport's edge so
    //	the user sees the drop is accepted. Toggled by AAASeedMTKView's
    //	NSDraggingDestination callbacks ; reset to false on
    //	performDragOperation completion.
    void set_drop_target_highlight( bool on );
    bool drop_target_highlight() const;

private:
    WidgetSystemImpl* _impl;   //	owned ; raw to keep header pimpl-free
};

}   //	namespace widgets
}   //	namespace ui
}   //	namespace aaa
