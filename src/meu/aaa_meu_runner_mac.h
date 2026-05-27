// src/meu/aaa_meu_runner_mac.h
//
// c143-A : Mac-native MEU runner shim. Bridges the Path A 172-shader
// catalog (src/shaders/msl/) to a Lua-driven per-frame entry point.
// Gives the .app real functionality WITHOUT needing the engine's layer
// subsystem (blocked by c119-A doctrine-limits wall).
//
// Architecture :
//   Lua script (.lua) calls into aaa.* C bindings ; the runner translates
//   those calls into MetalBackend operations on the supplied target.
//
//   On each frame the runner :
//     1. Bumps frame_index.
//     2. Calls `aaa.on_frame(width, height, frame_index)` in Lua.
//     3. Lua script selects shader + uniforms + bound textures via aaa.*
//     4. `aaa.draw_fullscreen_quad()` encodes a 3-vertex full-screen tri
//        through the currently-bound MetalBackend window pass.
//
// Hermetic Mac sub-lib per feedback_hermetic_mac_sublibs.md :
//   - Pure ObjC++ ; std::string / std::vector / std::unordered_map only.
//   - NO o_str, NO aaa_mem.h, NO aaaseed_code_utils link.
//   - Lua bindings use raw `lua_pushcfunction` (c124-A precedent), NOT
//     AAALUACALL macros.
//   - Bridge API : `void*` for ObjC types per c134-A doctrine
//     (feedback_bridge_api_standardization.md), but this header has no
//     ObjC types -- the Runner takes a `GOL::Backend*` pointer.
//
// Lifetime contract :
//   - Caller constructs Runner with a non-owning Backend pointer (the
//     backend must outlive the runner).
//   - `load_script` opens lua_State + registers bindings + runs the file.
//   - `render_frame` re-binds the per-frame target texture, then calls
//     into the script's `aaa.on_frame`.
//   - Destructor closes lua_State, releases all owned shader programs
//     + buffers + textures cached during load.

#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace GOL
{
    class Backend;
    using TextureId = std::uint32_t;
}

//	c147-A : forward-decl of the widget system. The Runner holds a
//	non-owning pointer (lifetime managed by AAASeedMTKView) and routes
//	`aaa.ui.*` Lua bindings into it. Defined in src/ui/widgets/.
namespace aaa { namespace ui { namespace widgets { class WidgetSystem; } } }

namespace aaa
{
namespace meu
{

class RunnerImpl;

//	Mac-native MEU runner. Owns a lua_State, a shader-program cache,
//	per-frame uniform / texture binding state, and a mirror of the
//	keyboard / mouse / scroll input surface (set via on_*_event,
//	queried by Lua via aaa.key_down / aaa.mouse_xy).
class Runner
{
public:
    explicit Runner( GOL::Backend* backend );
    ~Runner();

    Runner( Runner const& ) = delete;
    Runner& operator=( Runner const& ) = delete;

    //	Lifecycle -------------------------------------------------------

    //	Open lua_State, install aaa.* C bindings, load + run the .lua
    //	file at `lua_path`. Returns true on success ; false on file-read
    //	failure or Lua syntax / runtime error during initial chunk run.
    //	On failure the runner remains constructed but no script is
    //	active -- render_frame is a no-op until load_script succeeds.
    bool load_script( std::string const& lua_path );

    //	Re-run the currently-loaded script (file is re-read from disk so
    //	external edits land without restarting the app). Returns true if
    //	a script was loaded AND re-executed cleanly. False on no-script-
    //	loaded or re-load error. The C-binding aaa.* table survives
    //	(idempotent re-registration).
    bool reload();

    //	Tear down the Lua VM and clear cached resources. Idempotent --
    //	safe to call from a destructor path that doesn't know whether
    //	load_script was ever invoked.
    void unload();

    //	Render ----------------------------------------------------------

    //	Drive ONE frame. The host (typically AAASeedMTKView::drawInMTKView)
    //	has already called begin_window_render_pass + set_viewport ; the
    //	runner's `aaa.draw_fullscreen_quad()` binding emits draws against
    //	that active encoder. `target_color_attachment` is a hint for the
    //	Lua side (passed through `aaa.target()`) ; the runner does NOT
    //	begin / end the render pass itself.
    void render_frame( std::uint32_t width, std::uint32_t height,
                       GOL::TextureId target_color_attachment );

    //	Input dispatch --------------------------------------------------
    //
    //	c133-A view-side wiring forwards key / mouse events to these
    //	entry points BEFORE updating the view's local state, so a script
    //	polling via aaa.key_down() during the same drawInMTKView callback
    //	sees the event that triggered the frame.

    void on_key_event(   int key_code, bool down );
    void on_mouse_event( double x, double y, int button, bool down );
    void on_scroll_event( double dx, double dy );

    //	State queries (used by HUD + tests) ----------------------------

    //	Name of the shader most recently passed to aaa.use_shader() from
    //	Lua. Empty string when no shader has been selected yet. Useful
    //	for HUD overlay (display current effect name).
    std::string current_shader_name() const;

    //	Number of times render_frame has been called since load_script.
    //	Resets on reload(). Exposed to tests + the HUD.
    int frame_index() const;

    //	Catalog discovery : returns the list of .metal files found in the
    //	bundle's Resources/shaders/ (or the build-time path if running
    //	from a test binary). Each entry is the bare shader stem
    //	(e.g. "ps_Maa_add_scale" -- no extension, no directory).
    //	Stable across calls within a single Runner lifetime.
    std::vector< std::string > list_shaders() const;

    //	c146 : HUD overlay text queue. The Lua binding `aaa.draw_hud_text(s)`
    //	stashes its argument into the runner's per-frame _pending_hud_text
    //	field. After render_frame returns, the host (AAASeedMTKView) reads
    //	this value via get_pending_hud_text() and emits the text quads via
    //	the existing c61 glyph-atlas + text-MSL pipeline. The field is
    //	cleared at the START of each render_frame so a script that omits
    //	the call on a given frame produces an empty overlay (not the
    //	previous frame's stale text). Empty string == "no HUD this frame".
    //
    //	Why a queued string rather than direct draw : the runner has no
    //	knowledge of the host's glyph atlas / vertex buffer / Metal pipe ;
    //	keeping HUD text content under Lua control + rendering under the
    //	host's control preserves the c140 doctrine that the runner does
    //	NOT begin / end render passes and does NOT touch the framebuffer
    //	directly outside of the active encoder.
    std::string get_pending_hud_text() const;

    //	c147-A : install the host's WidgetSystem pointer. The Runner
    //	does NOT own the system -- the host (AAASeedMTKView) keeps the
    //	lifetime. Passing nullptr disables `aaa.ui.*` bindings (they
    //	become no-ops returning sensible defaults : slider returns its
    //	input value, button returns false, color_well returns its
    //	input rgba). Must be called BEFORE load_script() if the script
    //	relies on aaa.ui.* during its top-level run, but the runner
    //	tolerates being set after load too -- the next render_frame
    //	picks up the pointer.
    void set_widget_system( aaa::ui::widgets::WidgetSystem* ws );

    //	c149-A v3 Feature 4 : preset save / load. save_preset walks the
    //	WidgetSystem's retained state (per-label slider values / HSV
    //	picker RGBA / color-well preset index / text-input buffers /
    //	panel-expanded flags) and writes a Lua table file at `path`
    //	with the canonical shape :
    //	  return {
    //	    slider_state          = { ["label"] = 0.5, ... },
    //	    hsv_picker_state      = { ["label"] = {r=0.8,g=0.4,b=0.1,a=1.0}, ... },
    //	    color_well_state      = { ["label"] = 3, ... },
    //	    text_input_state      = { ["label"] = "value", ... },
    //	    panel_expanded_state  = { ["title"] = true, ... },
    //	  }
    //	Returns true on success, false on widget-system-not-installed or
    //	file write failure.
    //
    //	The Runner has no a priori knowledge of widget labels ; the Lua
    //	bindings track every label passed through aaa.ui.* during the
    //	current run + serialize THAT set. Labels that were never asked
    //	by the script are not serialized.
    bool save_preset( std::string const& path ) const;

    //	Load a preset file via luaL_dofile + read each field into the
    //	widget state maps via WidgetSystem::set_*. Returns true on
    //	success, false if :
    //	 - widget system not installed
    //	 - file missing / non-readable / Lua syntax error
    //	 - returned value is not a table
    bool load_preset( std::string const& path );

    //	c149-A v3 Feature 1 : drag-drop synthetic injection. Called by
    //	the .app's NSDraggingDestination delegate (AAASeedMTKView's
    //	performDragOperation:) AND by aaa.io.drop_file() Lua binding
    //	for tests. Routes ONLY .lua paths into load_script(); other
    //	extensions are ignored (returns false). Empty path is also
    //	rejected.
    bool drop_file( std::string const& path );

    //	c149-A v3 Feature 3 : file-watcher hot reload. After load_script
    //	the runner can optionally watch the loaded path on disk + call
    //	reload() on change. Disabled by default ; call enable_file_watch
    //	to opt in. Honors AAA_DISABLE_FILE_WATCH env var. Returns true
    //	on successful watch start.
    bool enable_file_watch();
    void disable_file_watch();
    bool is_file_watching() const;

private:
    std::unique_ptr< RunnerImpl > _impl;
};

}   //	namespace meu
}   //	namespace aaa
