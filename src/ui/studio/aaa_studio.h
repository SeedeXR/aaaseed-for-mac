// src/ui/studio/aaa_studio.h
//
// AAASeed ImGui Studio — the full authoring surface.
//
// Philosophy (mirrors AAASeed's own philosophy.md + user design docs):
//   - Immediate mode, same cadence as the BU / MEU / GaBuZoMeu stack.
//   - One WidgetSystem call owns the entire Dear ImGui context for
//     the app lifetime; no external global ImGui state is touched
//     by consumers.
//   - The studio renders INSIDE the existing MTKView Metal render pass
//     (same doctr ine as c147-A WidgetSystem: no begin/end pass here).
//   - Everything is scriptable: the Lua MEU runner can push panels and
//     widgets into the studio via `aaa.studio.*` bindings.
//   - Cross-platform intent: all platform specifics are isolated in
//     aaa_studio_metal.mm; the public header is pure C++.
//
// Panels shipped in v1 authoring surface:
//   1.  Node Graph     — visual MEU connection + execution order editor.
//   2.  Code Editor    — write / hot-reload Lua MEU scripts inline.
//   3.  MEU Inspector  — inspect / edit the active MEU's BU parameters.
//   4.  Shader Catalog — browse 172+ MSL shader catalog, bind to MEU.
//   5.  Camera Panel   — position / orient the AAA camera live.
//   6.  Sound Panel    — placeholder for audio device + clip routing.
//   7.  Binary Manager — attach / detach engine + runtime sub-process.
//   8.  Console        — live Lua REPL + engine log drain.
//   9.  Performance    — FPS, GPU time, frame budget gauge.
//  10.  Preferences    — theme colors, DPI, font scale (GaBuZoMeu palette).
//
// Hermetic Mac sub-lib (feedback_hermetic_mac_sublibs.md):
//   - Pure C++ public header; ObjC++ only in aaa_studio_metal.mm.
//   - NO o_str / aaa_mem / aaa_str / aaaseed_code_utils.
//   - std::string / std::vector / std::unordered_map / <cstdint> only.

#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

// Forward declarations — consumers don't need full headers.
namespace GOL  { class Backend; }
namespace aaa  { namespace meu  { class Runner; } }
namespace aaa  { namespace ui   { namespace widgets { class WidgetSystem; } } }

namespace aaa
{
namespace ui
{
namespace studio
{

// ── Node graph data model ─────────────────────────────────────────────────

struct NodePin
{
    std::uint32_t node_id = 0;
    std::uint32_t pin_idx = 0;
    bool          is_output = false;
};

struct NodeLink
{
    std::uint32_t id       = 0;
    NodePin       from_pin;   // output pin
    NodePin       to_pin;     // input pin
};

struct StudioNode
{
    std::uint32_t id          = 0;
    std::string   label;
    float         pos_x       = 100.0f;
    float         pos_y       = 100.0f;
    bool          enabled     = true;
    bool          selected    = false;
    // Shader / Lua script bound to this node (empty = no binding)
    std::string   shader_name;
    std::string   script_path;
    // Per-node float uniform overrides keyed by uniform name.
    std::unordered_map<std::string, float> uniforms;
};

// ── Console log ──────────────────────────────────────────────────────────

struct ConsoleEntry
{
    enum Level { INFO, WARN, ERR, LUA };
    Level       level   = INFO;
    std::string text;
    std::uint32_t frame = 0;
};

// ── Camera state (mirrors AaaCamReal layout) ─────────────────────────────

struct CameraState
{
    float pos[3]   = { 0.0f, 0.0f, 5.0f };
    float look[3]  = { 0.0f, 0.0f, 0.0f };
    float up[3]    = { 0.0f, 1.0f, 0.0f };
    float fov_deg  = 60.0f;
    float near_z   = 0.01f;
    float far_z    = 1000.0f;
};

// ── Sound placeholder ─────────────────────────────────────────────────────

struct SoundDeviceInfo
{
    std::uint32_t id   = 0;
    std::string   name;
    bool          is_input  = false;
    bool          is_output = false;
};

// ── Main studio class ─────────────────────────────────────────────────────

class StudioImpl;

class Studio
{
public:
    // backend must outlive the Studio instance.
    // runner may be nullptr (studio degrades gracefully without a runner).
    explicit Studio( GOL::Backend* backend, aaa::meu::Runner* runner );
    ~Studio();

    Studio( Studio const& )            = delete;
    Studio& operator=( Studio const& ) = delete;

    // ── Lifecycle ─────────────────────────────────────────────────────

    // Call after the MTKView is ready. Initialises the Dear ImGui
    // context, uploads font atlas to GPU, wires platform callbacks.
    // Returns true on success.
    // color_pixel_format is the MTKView's static colorPixelFormat
    // (MTLPixelFormat as std::uint64_t to keep the header pure-C++).
    // It seeds the rpd that new_frame builds, so the ImGui Metal
    // pipeline state compiles correctly on the very first frame --
    // before currentDrawable is available. Pass 0 to default to
    // MTLPixelFormatBGRA8Unorm.
    bool init( void* ns_view, void* mtl_device,
               std::uint64_t color_pixel_format = 0 );

    // Call at the start of each render frame BEFORE the Metal render
    // pass begins. Advances ImGui's internal state machine.
    // drawable_width / drawable_height are in pixels.
    void new_frame( std::uint32_t drawable_width,
                    std::uint32_t drawable_height,
                    double delta_seconds );

    // Call INSIDE the active Metal render pass (after begin_window_render_pass
    // but before present_window). Builds ImGui draw lists + encodes into
    // the current command encoder.
    // cmd_encoder is a MTL::RenderCommandEncoder* passed as void* to keep
    // the header free of metal-cpp types (c134-A bridge-API doctrine).
    // cmd_buffer is the corresponding MTL::CommandBuffer* (also as void*).
    // It is required by imgui_impl_metal's RenderDrawData which derefs
    // commandBuffer.device internally to lazily compile its pipeline
    // state. Pass nullptr for both args to call EndFrame() without
    // submitting GPU work (test scaffolding / headless paths).
    void render( void* cmd_encoder, void* cmd_buffer = nullptr );

    // Call when NSApplication receives a native event BEFORE it is
    // dispatched to the MTKView. Returns true if ImGui consumed it.
    bool handle_ns_event( void* ns_event );

    // ── Node graph API ────────────────────────────────────────────────

    // Add a node; returns assigned id. Positions auto-layout if
    // pos_x == pos_y == 0.
    std::uint32_t add_node( std::string const& label,
                            float pos_x = 0.0f, float pos_y = 0.0f );

    void remove_node( std::uint32_t id );

    // Connect output pin of `from_node` to input pin of `to_node`.
    // Returns link id or 0 on failure.
    std::uint32_t link_nodes( std::uint32_t from_node, std::uint32_t to_node );

    void unlink( std::uint32_t link_id );

    // Fetch mutable reference to a node (nullptr if not found).
    StudioNode*       find_node( std::uint32_t id );
    StudioNode const* find_node( std::uint32_t id ) const;

    std::vector<StudioNode> const& nodes() const;
    std::vector<NodeLink>   const& links() const;

    // ── Code editor API ───────────────────────────────────────────────

    // Set the text displayed in the inline code editor panel.
    void set_editor_text( std::string const& text );

    // Returns the current editor buffer (may have been modified by user).
    std::string const& editor_text() const;

    // Called when the user presses Cmd+R or the "Run" toolbar button.
    // Callback receives the current editor text.
    void on_run_script( std::function<void(std::string const&)> cb );

    // ── Console API ───────────────────────────────────────────────────

    void log( ConsoleEntry::Level level, std::string const& text );

    // ── Camera API ───────────────────────────────────────────────────

    CameraState const& camera() const;
    void               set_camera( CameraState const& cam );

    // ── Sound API (placeholder v1) ───────────────────────────────────

    // Enumerate system audio devices. Returns a snapshot; call again
    // to refresh.
    std::vector<SoundDeviceInfo> enumerate_sound_devices() const;

    // ── Performance API ───────────────────────────────────────────────

    void push_perf_sample( float frame_ms );

    // ── Preferences API ──────────────────────────────────────────────

    // Font scale [0.5, 3.0]. Applied next frame.
    void set_font_scale( float scale );

    // Apply the GaBuZoMeu palette (7 named colours : dark-teal /
    // mid-teal / light-teal background tiers, plus red / green / yellow
    // / magenta as documented in ui/notes/philosophy.md) to the ImGui
    // style. Called automatically on init ; call again after changing
    // preferences. No-op when no ImGui context is live.
    void apply_gabuzoumeu_theme();

    // ── Lua scripting API (c151-B) ────────────────────────────────────

    // Install `aaa.studio.*` C bindings on the given lua_State. The
    // pointer is taken as `void*` to keep the header free of lua.h.
    // After this call, scripts can use :
    //   aaa.studio.log(level, text)          -- INFO if level omitted
    //   aaa.studio.add_node(label[, x, y])   -- returns node id
    //   aaa.studio.set_camera(px, py, pz, lx, ly, lz [, ux, uy, uz, fov])
    //   aaa.studio.set_font_scale(s)
    // The Studio* is stashed in the lua_State's registry, so multiple
    // states (one per Runner load_script) can each route back to this
    // Studio independently. When the MEU Runner is wired (Studio
    // constructed with a non-null runner), this is called automatically
    // by a hook on the runner each time load_script opens a fresh
    // state ; consumers that own their own lua_State can call this
    // directly.
    void install_lua_bindings( void* lua_state );

private:
    StudioImpl* impl_ = nullptr;
};

} // namespace studio
} // namespace ui
} // namespace aaa
