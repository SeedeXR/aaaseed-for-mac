// src/ui/studio/aaa_studio_impl.h
//
// c151-B : private internal header shared by aaa_studio.cpp (pure C++
// data model -- buildable on any platform with imgui) AND
// aaa_studio_metal.mm (Mac-specific ImGui + Metal + Cocoa + AVFoundation
// + Core Audio + NSTask glue). The public surface stays in
// src/ui/studio/aaa_studio.h.
//
// The split is :
//   - data model fields + per-instance node-layout counters
//   - draw_* method declarations (the BODIES live in the .mm because
//     they call ObjC / Cocoa / Metal / AV helpers)
//   - PerfRing : the 256-sample circular ring used by the Perf panel
//
// The .cpp contains the Studio:: data-method bodies (ctor, log, node
// graph mutations, camera state, perf push, ...). The .mm contains
// init / new_frame / render / handle_ns_event / draw_* / Lua-binding
// install / Core Audio / AVFoundation / NSTask glue. Both files
// compile into the SAME static lib (aaaseed_ui_studio_mac) and both
// see this header.
//
// Doctrine :
//   - This header is INTERNAL. Do not expose outside the studio lib.
//   - ImGui (imgui.h) IS cross-platform ; ok to include here.
//   - No ObjC types, no Apple frameworks, no metal-cpp.

#pragma once

#include "src/ui/studio/aaa_studio.h"

#include "imgui.h"

#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace aaa { namespace ui { namespace studio {

// ── 256-sample frame-time ring used by the Perf panel ──────────────────────
struct PerfRing
{
    static constexpr std::size_t kSize = 256;
    std::array< float, kSize > samples = {};
    std::size_t head  = 0;
    std::size_t count = 0;

    void push( float v )
    {
        samples[ head ] = v;
        head  = ( head + 1 ) % kSize;
        if( count < kSize ) ++count;
    }
    float avg() const
    {
        if( count == 0 ) return 0.0f;
        float s = 0.0f;
        for( std::size_t i = 0; i < count; ++i ) s += samples[i];
        return s / static_cast< float >( count );
    }
    float max_val() const
    {
        float m = 0.0f;
        for( std::size_t i = 0; i < count; ++i )
            if( samples[i] > m ) m = samples[i];
        return m;
    }
};

// ── Node-graph layout constants ────────────────────────────────────────────
constexpr float kNodeW     = 160.0f;
constexpr float kNodeH     = 80.0f;
constexpr float kPinRadius = 6.0f;
constexpr float kNodePadY  = 18.0f;   // title bar height

inline ImVec2 input_pin_pos( StudioNode const& n )
{
    return ImVec2( n.pos_x,            n.pos_y + kNodeH * 0.5f );
}
inline ImVec2 output_pin_pos( StudioNode const& n )
{
    return ImVec2( n.pos_x + kNodeW,   n.pos_y + kNodeH * 0.5f );
}

// ── StudioImpl : private state shared between the .cpp and .mm ─────────────
struct StudioImpl
{
    GOL::Backend*     backend  = nullptr;
    aaa::meu::Runner* runner   = nullptr;
    // Back-pointer to the owning Studio so impl-side draw methods can
    // call Studio::log etc. without re-plumbing every helper to take a
    // Studio*. Set in Studio's ctor.
    Studio*           owner    = nullptr;

    bool              imgui_ok = false;

    // Mac-specific platform handles, kept as void* so this header
    // stays platform-neutral. Cast at the call site on the .mm side.
    void*             ns_view_ptr    = nullptr;
    void*             mtl_device_ptr = nullptr;

    // Wall-clock delta tracker for ImGuiIO.DeltaTime.
    std::chrono::steady_clock::time_point last_frame_t = {};

    // MTKView colorPixelFormat (NSUInteger) captured at init() ; used
    // by new_frame to build a valid rpd before the first drawable is
    // available. 0 means "use BGRA8Unorm default".
    std::uint64_t     color_pixel_format = 0;

    // ── Node graph ─────────────────────────────────────────────────────
    std::vector< StudioNode > nodes;
    std::vector< NodeLink >   links;
    std::uint32_t             next_node_id = 1;
    std::uint32_t             next_link_id = 1;

    // Per-instance auto-layout cursor (was file-static in pre-c151-B
    // code -- now per-Studio so multi-Studio scenarios behave).
    std::uint32_t             auto_layout_x = 60;
    std::uint32_t             auto_layout_y = 60;

    // Link-drag UI state
    bool       dragging_link = false;
    NodePin    drag_from_pin = {};
    ImVec2     drag_end      = {};

    // ── Code editor ────────────────────────────────────────────────────
    std::string                                editor_buf;
    std::function< void( std::string const& ) > on_run_cb;

    // ── Console ────────────────────────────────────────────────────────
    std::deque< ConsoleEntry > console_log;
    static constexpr std::size_t kMaxLogLines = 4096;
    bool   console_scroll_bottom = true;
    char   console_input[ 512 ] = {};

    // ── Camera ─────────────────────────────────────────────────────────
    CameraState camera;

    // ── AVFoundation capture (Mac) ─────────────────────────────────────
    // ObjC types behind void* to keep the layout platform-neutral.
    void*                  cap_helper      = nullptr;   // AAASeedStudioCaptureHelper*
    std::vector< std::string > cap_devices;
    int                    cap_active_idx  = -1;
    void*                  cap_preview_tex = nullptr;   // id<MTLTexture> retained
    std::uint32_t          cap_preview_w   = 0;
    std::uint32_t          cap_preview_h   = 0;

    // ── Sound device snapshot ──────────────────────────────────────────
    std::vector< SoundDeviceInfo > sound_devices;

    // ── NSTask-backed binary manager ───────────────────────────────────
    struct ManagedTask
    {
        std::string   label;
        std::string   command;
        std::string   args;
        void*         ns_task       = nullptr;   // NSTask*       (retained)
        void*         stdout_handle = nullptr;   // NSFileHandle* (retained)
        bool          running       = false;
        int           exit_code     = 0;
        std::uint32_t pid           = 0;
    };
    std::vector< ManagedTask > tasks;
    char  bin_form_label[ 128 ] = {};
    char  bin_form_path [ 512 ] = {};
    char  bin_form_args [ 512 ] = {};

    // ── Perf ring ──────────────────────────────────────────────────────
    PerfRing perf;

    // ── Preferences ────────────────────────────────────────────────────
    float font_scale = 1.0f;

    // ── Panel toggles ──────────────────────────────────────────────────
    bool show_node_graph  = true;
    bool show_code_editor = true;
    bool show_inspector   = true;
    bool show_shader_cat  = false;
    bool show_camera      = false;
    bool show_sound       = false;
    bool show_binaries    = false;
    bool show_console     = true;
    bool show_perf        = true;
    bool show_prefs       = false;
    bool show_imgui_demo  = false;

    // ── Panel draw methods (BODIES live in aaa_studio_metal.mm) ────────
    void draw_menubar();
    void draw_node_graph();
    void draw_code_editor();
    void draw_inspector();
    void draw_shader_catalog();
    void draw_camera_panel();
    void draw_sound_panel();
    void draw_binary_manager();
    void draw_console();
    void draw_perf_panel();
    void draw_prefs_panel();

    // ── Pure-data helpers (cross-platform, defined in aaa_studio.cpp) ──
    StudioNode* find_node_by_id( std::uint32_t id )
    {
        for( auto& n : nodes )
            if( n.id == id ) return &n;
        return nullptr;
    }

    void auto_layout_pos( float& out_x, float& out_y )
    {
        out_x = static_cast< float >( auto_layout_x );
        out_y = static_cast< float >( auto_layout_y );
        auto_layout_x += static_cast< std::uint32_t >( kNodeW + 40 );
        if( auto_layout_x > 900 )
        {
            auto_layout_x = 60;
            auto_layout_y += static_cast< std::uint32_t >( kNodeH + 30 );
        }
    }
};

} } } // namespace aaa::ui::studio
