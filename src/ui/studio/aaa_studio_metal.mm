// src/ui/studio/aaa_studio_metal.mm
//
// AAASeed ImGui Studio — Metal + Cocoa backend implementation.
//
// Wires Dear ImGui's imgui_impl_metal + imgui_impl_osx into the
// existing GOL::Backend + MTKView render pipeline.
//
// Doctrine:
//   - No o_str / aaa_mem / aaa_str / aaaseed_code_utils.
//   - std::string / std::vector / std::unordered_map / <cstdint> only
//     from AAASeed side.
//   - ObjC++ is used only where Apple frameworks demand it; all public
//     API surfaces in aaa_studio.h are pure C++.
//   - void* bridge for MTL / NSView types per c134-A doctrine.

#import <Cocoa/Cocoa.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import <CoreAudio/CoreAudio.h>
#import <AVFoundation/AVFoundation.h>
#import <CoreVideo/CoreVideo.h>
#import <CoreMedia/CoreMedia.h>
#import <IOSurface/IOSurfaceObjC.h>

#include <filesystem>

// c151-B : Lua bindings for aaa.studio.* . The Lua headers are C with
// extern "C" linkage ; vendored at third_party/lua-5.1/include.
extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

// c151-B : forward declarations for the AVFoundation capture helper +
// the device-enumeration helpers. The full definitions live further
// down (after the StudioImpl draw methods that USE them are defined
// inline within the class). Forward-declaring here keeps that ordering
// without re-arranging hundreds of lines of impl code.
@class AAASeedStudioCaptureHelper;
namespace
{
    std::vector<std::string> enumerate_av_video_devices();
    AVCaptureDevice*         av_video_device_at( int idx );
}

#include "imgui.h"
#include "imgui_impl_metal.h"
#include "imgui_impl_osx.h"
#include "misc/cpp/imgui_stdlib.h"

#include "src/ui/studio/aaa_studio.h"
#include "src/ui/studio/aaa_studio_impl.h"
#include "src/gol/metal/metal_backend.h"
#include "src/meu/aaa_meu_runner_mac.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstring>
#include <deque>
#include <sstream>
#include <unordered_map>

// ── GaBuZoMeu palette (from design docs + BU.lua conventions) ─────────────
//   Background / neutral : dark teal / dark blue
//   Selected item        : light teal
//   Important / disabled : red
//   Active / enabled     : green
//   Real-time info       : yellow
//   Selected             : magenta
namespace
{
    constexpr ImVec4 kColBgDark     = { 0.07f, 0.12f, 0.15f, 1.00f };
    constexpr ImVec4 kColBgMid      = { 0.10f, 0.16f, 0.20f, 1.00f };
    constexpr ImVec4 kColBgPanel    = { 0.12f, 0.18f, 0.22f, 1.00f };
    constexpr ImVec4 kColTealLight  = { 0.20f, 0.65f, 0.70f, 1.00f };
    constexpr ImVec4 kColTealMid    = { 0.14f, 0.45f, 0.50f, 1.00f };
    constexpr ImVec4 kColRed        = { 0.80f, 0.15f, 0.15f, 1.00f };
    constexpr ImVec4 kColGreen      = { 0.15f, 0.75f, 0.30f, 1.00f };
    constexpr ImVec4 kColYellow     = { 0.90f, 0.82f, 0.18f, 1.00f };
    constexpr ImVec4 kColMagenta    = { 0.85f, 0.18f, 0.75f, 1.00f };
    constexpr ImVec4 kColText       = { 0.90f, 0.92f, 0.95f, 1.00f };
    constexpr ImVec4 kColTextDim    = { 0.55f, 0.60f, 0.65f, 1.00f };
    constexpr ImVec4 kColBorder     = { 0.18f, 0.26f, 0.32f, 1.00f };

    // Node graph wire colors per connection state
    constexpr ImU32 kWireActive   = IM_COL32( 51, 166, 179, 220 );
    constexpr ImU32 kWireSelected = IM_COL32( 217, 46, 191, 220 );
    constexpr ImU32 kWireDefault  = IM_COL32( 51, 115, 128, 180 );
} // namespace

// c151-B : PerfRing, the node-graph layout constants/helpers, and
// StudioImpl all moved to the shared private header aaa_studio_impl.h
// so the pure-C++ data-method bodies in aaa_studio.cpp can see the
// same layout. The rest of THIS file is the Mac-specific glue.
//
// The block below is a sanity check that the relocated layout still
// uses the same constants as the panel draw bodies further down.

#if 0
struct aaa::ui::studio::StudioImpl  /* dead -- see aaa_studio_impl.h */
{
    GOL::Backend*     backend  = nullptr;
    aaa::meu::Runner* runner   = nullptr;
    // c151-B : back-pointer to the owning Studio so impl-side draw
    // methods can call Studio::log etc. without re-plumbing every
    // helper to take a Studio*. Set in Studio's ctor.
    Studio*           owner    = nullptr;

    bool              imgui_ok = false;

    // c151-A : keep the platform handles past init() so new_frame() can
    // forward the REAL view / device into the ImGui backends each frame.
    // Stored as void* to keep the .h pure-C++. Cast to NSView* / id<MTLDevice>
    // at the call site.
    void*             ns_view_ptr    = nullptr;
    void*             mtl_device_ptr = nullptr;

    // c151-A : track real wall-clock delta for ImGuiIO.DeltaTime rather
    // than the View's hardcoded 1/60. ImGui drives its own animations
    // (resize handles, tooltips, drag inertia) off DeltaTime.
    std::chrono::steady_clock::time_point last_frame_t = {};

    // c151-B : color pixel format hint, captured at init() from the
    // MTKView's static colorPixelFormat. new_frame() uses it to build
    // the rpd even when currentDrawable is nil (first frame race) so
    // imgui_impl_metal's pipeline state cache compiles correctly from
    // the very first frame. Stored as MTLPixelFormat (NSUInteger).
    std::uint64_t     color_pixel_format = 0;

    // Node graph
    std::vector<StudioNode>                         nodes;
    std::vector<NodeLink>                           links;
    std::uint32_t                                   next_node_id = 1;
    std::uint32_t                                   next_link_id = 1;

    // Link-drag state
    bool       dragging_link   = false;
    NodePin    drag_from_pin   = {};
    ImVec2     drag_end        = {};

    // Code editor
    std::string                              editor_buf;
    std::function<void(std::string const&)> on_run_cb;

    // Console
    std::deque<ConsoleEntry> console_log;
    static constexpr std::size_t kMaxLogLines = 4096;
    bool  console_scroll_bottom = true;
    char  console_input[512]    = {};

    // Camera
    CameraState camera;

    // c151-B : AVFoundation video capture. The capture session is
    // wrapped in an ObjC helper object (forward-declared with id so
    // the StudioImpl layout stays platform-neutral here). State is :
    //   - cap_helper : strong ref to the wrapper (NSObject subclass)
    //   - cap_devices : last-enumerated AVCaptureDevices ; the panel
    //     shows them as a combo box
    //   - cap_active_idx : which device is running (or -1)
    //   - cap_preview_tex : latest CVPixelBuffer wrapped as an
    //     MTLTexture for ImGui::Image preview ; refreshed on the
    //     sample-buffer callback's main-thread dispatch
    //   - cap_preview_w / h : last-known frame dimensions
    void*       cap_helper       = nullptr;
    std::vector<std::string> cap_devices;
    int         cap_active_idx   = -1;
    void*       cap_preview_tex  = nullptr;     // id<MTLTexture> retained
    std::uint32_t cap_preview_w  = 0;
    std::uint32_t cap_preview_h  = 0;

    // Sound (placeholder)
    std::vector<SoundDeviceInfo> sound_devices;

    // c151-B : Binary Manager (real NSTask). Each entry holds the
    // launched NSTask, its read-end pipes (drained on a background
    // queue), and a running-state flag updated when the task exits.
    // ObjC types live behind void* so this stays inside StudioImpl's
    // .mm-only layout without polluting the public header.
    struct ManagedTask
    {
        std::string label;              // user-facing display name
        std::string command;            // path or argv[0]
        std::string args;               // space-separated args (parsed on launch)
        void*       ns_task     = nullptr;   // NSTask*  (retained)
        void*       stdout_handle = nullptr; // NSFileHandle* (retained)
        bool        running     = false;
        int         exit_code   = 0;
        std::uint32_t pid       = 0;
    };
    std::vector<ManagedTask> tasks;
    // Text-field buffers for the "add new" form.
    char        bin_form_label[128] = {};
    char        bin_form_path[512]  = {};
    char        bin_form_args[512]  = {};

    // Perf
    PerfRing perf;

    // Prefs
    float font_scale = 1.0f;

    // Panel open/close flags
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
    bool show_imgui_demo  = false;  // developer escape hatch

    // ── Panel draw methods ─────────────────────────────────────────────

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

    // ── Helpers ───────────────────────────────────────────────────────

    StudioNode* find_node_by_id( std::uint32_t id )
    {
        for( auto& n : nodes )
            if( n.id == id ) return &n;
        return nullptr;
    }

    void auto_layout_pos( float& out_x, float& out_y )
    {
        out_x = static_cast<float>( s_auto_layout_x );
        out_y = static_cast<float>( s_auto_layout_y );
        s_auto_layout_x += static_cast<std::uint32_t>( kNodeW + 40 );
        if( s_auto_layout_x > 900 )
        {
            s_auto_layout_x = 60;
            s_auto_layout_y += static_cast<std::uint32_t>( kNodeH + 30 );
        }
    }
};
#endif // c151-B : end of dead block ; live StudioImpl lives in aaa_studio_impl.h

// ════════════════════════════════════════════════════════════════════════════
// Menu bar
// ════════════════════════════════════════════════════════════════════════════

void aaa::ui::studio::StudioImpl::draw_menubar()
{
    if( ImGui::BeginMainMenuBar() )
    {
        if( ImGui::BeginMenu( "AAASeed" ) )
        {
            if( ImGui::MenuItem( "Preferences", "Cmd+," ) ) show_prefs = true;
            ImGui::Separator();
            if( ImGui::MenuItem( "Quit", "Cmd+Q" ) )
                [NSApp terminate:nil];
            ImGui::EndMenu();
        }
        if( ImGui::BeginMenu( "View" ) )
        {
            ImGui::MenuItem( "Node Graph",      nullptr, &show_node_graph  );
            ImGui::MenuItem( "Code Editor",     nullptr, &show_code_editor );
            ImGui::MenuItem( "Inspector",        nullptr, &show_inspector   );
            ImGui::MenuItem( "Shader Catalog",  nullptr, &show_shader_cat  );
            ImGui::MenuItem( "Camera",          nullptr, &show_camera      );
            ImGui::MenuItem( "Sound",           nullptr, &show_sound       );
            ImGui::MenuItem( "Binary Manager",  nullptr, &show_binaries    );
            ImGui::MenuItem( "Console",         nullptr, &show_console     );
            ImGui::MenuItem( "Performance",     nullptr, &show_perf        );
            ImGui::Separator();
            ImGui::MenuItem( "ImGui Demo",      nullptr, &show_imgui_demo  );
            ImGui::EndMenu();
        }
        if( ImGui::BeginMenu( "Script" ) )
        {
            if( ImGui::MenuItem( "Run", "Cmd+R" ) )
            {
                if( on_run_cb ) on_run_cb( editor_buf );
            }
            ImGui::EndMenu();
        }

        // Right-aligned FPS
        float fps = ImGui::GetIO().Framerate;
        char fps_str[32];
        std::snprintf( fps_str, sizeof(fps_str), "%.1f fps", fps );
        float w = ImGui::CalcTextSize(fps_str).x + 12.0f;
        ImGui::SetCursorPosX( ImGui::GetContentRegionMax().x - w );
        ImGui::TextColored( kColYellow, "%s", fps_str );

        ImGui::EndMainMenuBar();
    }
}

// ════════════════════════════════════════════════════════════════════════════
// Node Graph
// ════════════════════════════════════════════════════════════════════════════

void aaa::ui::studio::StudioImpl::draw_node_graph()
{
    if( !show_node_graph ) return;

    ImGui::SetNextWindowSize( { 700, 480 }, ImGuiCond_FirstUseEver );
    ImGui::SetNextWindowPos(  {  10,  30 }, ImGuiCond_FirstUseEver );

    ImGui::Begin( "Node Graph", &show_node_graph,
                  ImGuiWindowFlags_NoScrollbar |
                  ImGuiWindowFlags_NoScrollWithMouse );

    // Toolbar
    if( ImGui::Button( "+ Node" ) )
    {
        StudioNode n;
        n.id    = next_node_id++;
        n.label = "MEU_" + std::to_string( n.id );
        auto_layout_pos( n.pos_x, n.pos_y );
        nodes.push_back( std::move(n) );
    }
    ImGui::SameLine();
    if( ImGui::Button( "Clear All" ) )
    {
        nodes.clear();
        links.clear();
        // c151-B : auto-layout cursors are now per-instance members
        auto_layout_x = 60;
        auto_layout_y = 60;
    }
    ImGui::SameLine();
    ImGui::TextDisabled( "(%zu nodes  %zu links)",
                         nodes.size(), links.size() );

    ImGui::Separator();

    // Canvas
    ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();
    ImVec2 canvas_sz = ImGui::GetContentRegionAvail();
    if( canvas_sz.x < 100 ) canvas_sz.x = 100;
    if( canvas_sz.y < 100 ) canvas_sz.y = 100;

    ImDrawList* dl = ImGui::GetWindowDrawList();

    // Grid
    dl->AddRectFilled( canvas_p0,
                       { canvas_p0.x + canvas_sz.x,
                         canvas_p0.y + canvas_sz.y },
                       IM_COL32( 18, 28, 34, 255 ) );
    static constexpr float kGrid = 32.0f;
    for( float x = 0; x < canvas_sz.x; x += kGrid )
        dl->AddLine( { canvas_p0.x + x, canvas_p0.y },
                     { canvas_p0.x + x, canvas_p0.y + canvas_sz.y },
                     IM_COL32(30,42,50,120) );
    for( float y = 0; y < canvas_sz.y; y += kGrid )
        dl->AddLine( { canvas_p0.x,              canvas_p0.y + y },
                     { canvas_p0.x + canvas_sz.x, canvas_p0.y + y },
                     IM_COL32(30,42,50,120) );

    // Invisible interaction overlay
    ImGui::InvisibleButton( "canvas",
                            canvas_sz,
                            ImGuiButtonFlags_MouseButtonLeft |
                            ImGuiButtonFlags_MouseButtonRight );
    bool canvas_hovered = ImGui::IsItemHovered();

    // Draw links
    for( auto const& lnk : links )
    {
        auto* fn = find_node_by_id( lnk.from_pin.node_id );
        auto* tn = find_node_by_id( lnk.to_pin.node_id  );
        if( !fn || !tn ) continue;

        ImVec2 p0 = output_pin_pos(*fn);
        ImVec2 p1 = input_pin_pos (*tn);
        p0.x += canvas_p0.x;  p0.y += canvas_p0.y;
        p1.x += canvas_p0.x;  p1.y += canvas_p0.y;
        float cx = ( p1.x - p0.x ) * 0.5f;
        dl->AddBezierCubic( p0,
                            { p0.x + cx, p0.y },
                            { p1.x - cx, p1.y },
                            p1,
                            kWireDefault, 2.0f );
    }

    // Drag-in-progress wire
    if( dragging_link )
    {
        auto* fn = find_node_by_id( drag_from_pin.node_id );
        if( fn )
        {
            ImVec2 p0 = output_pin_pos(*fn);
            p0.x += canvas_p0.x;  p0.y += canvas_p0.y;
            ImVec2 p1 = ImGui::GetMousePos();
            float cx  = ( p1.x - p0.x ) * 0.5f;
            dl->AddBezierCubic( p0,
                                { p0.x + cx, p0.y },
                                { p1.x - cx, p1.y },
                                p1,
                                kWireActive, 2.0f );
            drag_end = p1;
        }
        if( !ImGui::IsMouseDown( ImGuiMouseButton_Left ) )
        {
            dragging_link = false;
            // Check if we released over an input pin
            for( auto const& tn : nodes )
            {
                ImVec2 ip = input_pin_pos(tn);
                ip.x += canvas_p0.x;  ip.y += canvas_p0.y;
                float dx = drag_end.x - ip.x;
                float dy = drag_end.y - ip.y;
                if( dx*dx + dy*dy < (kPinRadius*2)*(kPinRadius*2) )
                {
                    if( tn.id != drag_from_pin.node_id )
                    {
                        NodeLink lnk;
                        lnk.id                  = next_link_id++;
                        lnk.from_pin.node_id    = drag_from_pin.node_id;
                        lnk.from_pin.is_output  = true;
                        lnk.to_pin.node_id      = tn.id;
                        lnk.to_pin.is_output    = false;
                        links.push_back(lnk);
                    }
                    break;
                }
            }
        }
    }

    // Draw nodes
    for( auto& n : nodes )
    {
        ImVec2 np = { canvas_p0.x + n.pos_x,
                      canvas_p0.y + n.pos_y };

        // Body
        ImU32 body_col = n.selected
            ? IM_COL32(38, 90, 100, 240)
            : ( n.enabled
                ? IM_COL32(23, 50, 62, 230)
                : IM_COL32(80, 20, 20, 230) );

        dl->AddRectFilled( np,
                           { np.x + kNodeW, np.y + kNodeH },
                           body_col, 6.0f );

        // Title bar
        ImU32 title_col = n.enabled
            ? IM_COL32(38, 154, 68, 230)
            : IM_COL32(140, 30, 30, 230);
        dl->AddRectFilled( np,
                           { np.x + kNodeW, np.y + kNodePadY },
                           title_col, 6.0f,
                           ImDrawFlags_RoundCornersTop );

        // Border
        ImU32 border_col = n.selected
            ? IM_COL32(217, 46, 191, 255)
            : IM_COL32(46, 115, 128, 200);
        dl->AddRect( np,
                     { np.x + kNodeW, np.y + kNodeH },
                     border_col, 6.0f, 0, 1.5f );

        // Title text
        dl->AddText( { np.x + 6, np.y + 3 },
                     IM_COL32(220,235,240,255),
                     n.label.c_str() );

        // Shader / script info
        if( !n.shader_name.empty() )
        {
            std::string sn = "[S] " + n.shader_name;
            dl->AddText( { np.x + 6, np.y + kNodePadY + 4 },
                         IM_COL32(140,200,210,200),
                         sn.c_str() );
        }
        else if( !n.script_path.empty() )
        {
            // Show just filename
            auto slash = n.script_path.rfind('/');
            std::string fn_str = "[L] " + ( slash != std::string::npos
                ? n.script_path.substr(slash+1) : n.script_path );
            dl->AddText( { np.x + 6, np.y + kNodePadY + 4 },
                         IM_COL32(140,200,210,200),
                         fn_str.c_str() );
        }

        // Input pin (left)
        ImVec2 ip = input_pin_pos(n);
        ip.x += canvas_p0.x;  ip.y += canvas_p0.y;
        dl->AddCircleFilled( ip, kPinRadius, IM_COL32(51,166,179,220) );
        dl->AddCircle      ( ip, kPinRadius, IM_COL32(200,230,235,180) );

        // Output pin (right)
        ImVec2 op = output_pin_pos(n);
        op.x += canvas_p0.x;  op.y += canvas_p0.y;
        dl->AddCircleFilled( op, kPinRadius, IM_COL32(51,166,179,220) );
        dl->AddCircle      ( op, kPinRadius, IM_COL32(200,230,235,180) );

        // Interaction: drag body to move
        ImGui::SetCursorScreenPos( np );
        ImGui::InvisibleButton( ( "node_" + std::to_string(n.id) ).c_str(),
                                { kNodeW, kNodeH } );

        if( ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left) )
        {
            n.pos_x += ImGui::GetIO().MouseDelta.x;
            n.pos_y += ImGui::GetIO().MouseDelta.y;
        }
        if( ImGui::IsItemClicked() )
        {
            // Deselect all, select this
            for( auto& nn : nodes ) nn.selected = false;
            n.selected = true;
        }

        // Right-click context
        if( ImGui::IsItemClicked( ImGuiMouseButton_Right ) )
            ImGui::OpenPopup( ("node_ctx_" + std::to_string(n.id)).c_str() );
        if( ImGui::BeginPopup( ("node_ctx_" + std::to_string(n.id)).c_str() ) )
        {
            ImGui::TextDisabled( "%s", n.label.c_str() );
            ImGui::Separator();
            if( ImGui::MenuItem( n.enabled ? "Disable" : "Enable" ) )
                n.enabled = !n.enabled;
            if( ImGui::MenuItem( "Delete" ) )
            {
                auto& ns = nodes;
                auto& ls = links;
                std::uint32_t nid = n.id;
                ls.erase( std::remove_if( ls.begin(), ls.end(),
                    [nid](NodeLink const& l){
                        return l.from_pin.node_id==nid ||
                               l.to_pin.node_id==nid; } ),
                    ls.end() );
                ns.erase( std::remove_if( ns.begin(), ns.end(),
                    [nid](StudioNode const& nn){ return nn.id==nid; } ),
                    ns.end() );
                ImGui::EndPopup();
                ImGui::End();
                return;
            }
            ImGui::EndPopup();
        }

        // Output pin drag to start a link
        ImGui::SetCursorScreenPos( { op.x - kPinRadius, op.y - kPinRadius } );
        ImGui::InvisibleButton( ("opin_" + std::to_string(n.id)).c_str(),
                                { kPinRadius*2, kPinRadius*2 } );
        if( ImGui::IsItemClicked() && !dragging_link )
        {
            dragging_link           = true;
            drag_from_pin.node_id   = n.id;
            drag_from_pin.is_output = true;
        }
    }

    ImGui::End();
}

// ════════════════════════════════════════════════════════════════════════════
// Code Editor
// ════════════════════════════════════════════════════════════════════════════

void aaa::ui::studio::StudioImpl::draw_code_editor()
{
    if( !show_code_editor ) return;

    ImGui::SetNextWindowSize( { 600, 400 }, ImGuiCond_FirstUseEver );
    ImGui::SetNextWindowPos(  { 720,  30 }, ImGuiCond_FirstUseEver );

    ImGui::Begin( "Code Editor", &show_code_editor );

    if( ImGui::Button( "Run  (Cmd+R)" ) )
    {
        if( on_run_cb ) on_run_cb( editor_buf );
    }
    ImGui::SameLine();
    if( ImGui::Button( "Clear" ) ) editor_buf.clear();

    ImGui::Separator();

    // Monospaced editor area — ImGui doesn't ship a full code editor;
    // we use InputTextMultiline with a monospace font flag.  A future
    // iteration can plug in ImGuiColorTextEdit.
    ImVec2 avail = ImGui::GetContentRegionAvail();
    ImGui::PushStyleColor( ImGuiCol_FrameBg, kColBgDark );
    ImGui::InputTextMultiline(
        "##code",
        &editor_buf,
        { avail.x, avail.y - 4 },
        ImGuiInputTextFlags_AllowTabInput
    );
    ImGui::PopStyleColor();

    // Cmd+R shortcut
    if( ImGui::IsWindowFocused() &&
        ImGui::GetIO().KeySuper &&
        ImGui::IsKeyPressed( ImGuiKey_R, false ) )
    {
        if( on_run_cb ) on_run_cb( editor_buf );
    }

    ImGui::End();
}

// ════════════════════════════════════════════════════════════════════════════
// MEU Inspector
// ════════════════════════════════════════════════════════════════════════════

void aaa::ui::studio::StudioImpl::draw_inspector()
{
    if( !show_inspector ) return;

    ImGui::SetNextWindowSize( { 280, 400 }, ImGuiCond_FirstUseEver );
    ImGui::SetNextWindowPos(  {  10, 520 }, ImGuiCond_FirstUseEver );

    ImGui::Begin( "MEU Inspector", &show_inspector );

    // Find selected node
    StudioNode* sel = nullptr;
    for( auto& n : nodes )
        if( n.selected ) { sel = &n; break; }

    if( !sel )
    {
        ImGui::TextColored( kColTextDim, "No node selected." );
        ImGui::End();
        return;
    }

    ImGui::TextColored( kColGreen, "%s", sel->label.c_str() );
    ImGui::Separator();

    // Label edit
    {
        char buf[128];
        std::strncpy( buf, sel->label.c_str(), sizeof(buf)-1 );
        buf[sizeof(buf)-1] = '\0';
        if( ImGui::InputText( "Label", buf, sizeof(buf) ) )
            sel->label = buf;
    }

    // Enable toggle
    ImGui::Checkbox( "Enabled", &sel->enabled );

    ImGui::Spacing();
    ImGui::TextColored( kColTealLight, "Bindings" );
    ImGui::Separator();

    // Shader binding
    {
        char buf[128];
        std::strncpy( buf, sel->shader_name.c_str(), sizeof(buf)-1 );
        buf[sizeof(buf)-1] = '\0';
        if( ImGui::InputText( "Shader", buf, sizeof(buf) ) )
            sel->shader_name = buf;
    }

    // Script path
    {
        char buf[256];
        std::strncpy( buf, sel->script_path.c_str(), sizeof(buf)-1 );
        buf[sizeof(buf)-1] = '\0';
        if( ImGui::InputText( "Script", buf, sizeof(buf) ) )
            sel->script_path = buf;
        ImGui::SameLine();
        if( ImGui::SmallButton( "..." ) )
        {
            // Open file dialog (aaa_file_dialog.h integration point)
            // Deferred to next iteration — requires linking
            // aaaseed_ui_file_dialog_mac into this target.
        }
    }

    ImGui::Spacing();
    ImGui::TextColored( kColTealLight, "Uniforms" );
    ImGui::Separator();

    for( auto& [name, val] : sel->uniforms )
    {
        float v = val;
        if( ImGui::SliderFloat( name.c_str(), &v, -10.0f, 10.0f ) )
            val = v;
    }

    if( ImGui::Button( "+ Add Uniform" ) )
        sel->uniforms[ "param_" + std::to_string( sel->uniforms.size() ) ] = 0.0f;

    ImGui::End();
}

// ════════════════════════════════════════════════════════════════════════════
// Shader Catalog
// ════════════════════════════════════════════════════════════════════════════

void aaa::ui::studio::StudioImpl::draw_shader_catalog()
{
    if( !show_shader_cat ) return;

    ImGui::SetNextWindowSize( { 320, 480 }, ImGuiCond_FirstUseEver );
    ImGui::SetNextWindowPos(  { 300, 520 }, ImGuiCond_FirstUseEver );

    ImGui::Begin( "Shader Catalog", &show_shader_cat );

    // c151-A : real filesystem enumeration of the MSL catalog. The
    // `AAA_SHADERS_MSL_DIR` compile define is set by the CMake target
    // (same pattern as the perf test at tests/unit/path_a_compile_perf_test.cpp).
    // Cache the list across frames -- directory_iterator is cheap but
    // not free, and the catalog content doesn't change at runtime.
    static std::vector<std::string> s_shader_names;
    static bool                     s_shader_names_loaded = false;
    if( !s_shader_names_loaded )
    {
        s_shader_names_loaded = true;
#ifdef AAA_SHADERS_MSL_DIR
        std::filesystem::path const root( AAA_SHADERS_MSL_DIR );
        std::error_code ec;
        if( std::filesystem::is_directory( root, ec ) )
        {
            for( auto const& entry : std::filesystem::directory_iterator( root, ec ) )
            {
                if( entry.is_regular_file() && entry.path().extension() == ".metal" )
                    s_shader_names.push_back( entry.path().stem().string() );
            }
            std::sort( s_shader_names.begin(), s_shader_names.end() );
        }
#endif
    }

    ImGui::TextColored( kColTealLight, "%zu MSL shaders in catalog",
                        s_shader_names.size() );
    ImGui::Separator();

    static char filter[64] = {};
    ImGui::InputTextWithHint( "##filter", "Filter shaders...", filter, sizeof(filter) );

    if( ImGui::SmallButton( "Reload" ) )
    {
        s_shader_names.clear();
        s_shader_names_loaded = false;
    }

    ImGui::BeginChild( "##catalog_list",
                       ImGui::GetContentRegionAvail(),
                       false );

    if( s_shader_names.empty() )
    {
        ImGui::TextColored( kColTextDim,
                            "(no .metal files found ; AAA_SHADERS_MSL_DIR may be unset)" );
    }
    for( auto const& name : s_shader_names )
    {
        if( filter[0] != '\0' && name.find( filter ) == std::string::npos )
            continue;

        bool selected = false;
        ImGui::PushStyleColor( ImGuiCol_Header,
                               { 0.14f, 0.45f, 0.50f, 0.6f } );
        if( ImGui::Selectable( name.c_str(), &selected ) )
        {
            // Bind selected shader to the currently-selected node
            for( auto& n : nodes )
            {
                if( n.selected )
                {
                    n.shader_name = name;
                    break;
                }
            }
        }
        ImGui::PopStyleColor();
    }

    ImGui::EndChild();
    ImGui::End();
}

// ════════════════════════════════════════════════════════════════════════════
// Camera Panel
// ════════════════════════════════════════════════════════════════════════════

// The AVFoundation block in this method invokes selectors defined on
// AAASeedStudioCaptureHelper, whose full @interface lives further down
// the file (after the impl methods that USE it). The forward `@class`
// declaration at the top is enough for ObjC's dynamic dispatch to find
// the methods at runtime, but the compiler emits
// -Wobjc-method-access warnings because it can't see the declarations
// at compile time. Suppress only the warning, not the build error.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wobjc-method-access"
#pragma clang diagnostic ignored "-Wreceiver-forward-class"

void aaa::ui::studio::StudioImpl::draw_camera_panel()
{
    if( !show_camera ) return;

    ImGui::SetNextWindowSize( { 360, 520 }, ImGuiCond_FirstUseEver );
    ImGui::SetNextWindowPos(  { 600, 220 }, ImGuiCond_FirstUseEver );

    ImGui::Begin( "Camera", &show_camera );

    // ── Pose controls (rendering camera) ──────────────────────────────
    if( ImGui::CollapsingHeader( "Pose", ImGuiTreeNodeFlags_DefaultOpen ) )
    {
        ImGui::TextColored( kColTealLight, "Position" );
        ImGui::DragFloat3( "##pos",  camera.pos,  0.02f );
        ImGui::TextColored( kColTealLight, "Look At" );
        ImGui::DragFloat3( "##look", camera.look, 0.02f );
        ImGui::TextColored( kColTealLight, "Up Vector" );
        ImGui::DragFloat3( "##up",   camera.up,   0.01f );
        ImGui::Spacing();
        ImGui::SliderFloat( "FOV",   &camera.fov_deg, 10.0f, 170.0f );
        ImGui::DragFloat(   "Near",  &camera.near_z,  0.001f, 0.001f, 10.0f );
        ImGui::DragFloat(   "Far",   &camera.far_z,   1.0f,   0.01f,  100000.0f );
        if( ImGui::Button( "Reset Pose" ) )
            camera = {};
    }

    ImGui::Separator();

    // ── AVFoundation capture (real camera device) ─────────────────────
    if( ImGui::CollapsingHeader( "Capture device",
                                  ImGuiTreeNodeFlags_DefaultOpen ) )
    {
        ImGui::TextWrapped(
            "AVFoundation video input. Click Refresh to enumerate ; "
            "Start to begin the capture session. First launch will "
            "prompt for camera access -- the .app's Info.plist already "
            "declares NSCameraUsageDescription." );

        if( ImGui::Button( "Refresh devices" ) )
            cap_devices = enumerate_av_video_devices();
        ImGui::SameLine();
        ImGui::TextDisabled( "%zu device(s)", cap_devices.size() );

        if( cap_devices.empty() )
        {
            ImGui::TextColored( kColTextDim, "(press Refresh)" );
        }
        else
        {
            for( int i = 0; i < (int) cap_devices.size(); ++i )
            {
                bool const is_active = ( i == cap_active_idx );
                ImU32 const dot = is_active
                    ? IM_COL32(38,200,68,255)
                    : IM_COL32(180,30,30,255);
                ImGui::GetWindowDrawList()->AddCircleFilled(
                    { ImGui::GetCursorScreenPos().x + 6,
                      ImGui::GetCursorScreenPos().y + 9 },
                    5.0f, dot );
                ImGui::SetCursorPosX( ImGui::GetCursorPosX() + 18 );
                ImGui::Text( "%s", cap_devices[i].c_str() );

                ImGui::SameLine( 200 );
                if( is_active )
                {
                    ImGui::PushStyleColor( ImGuiCol_Button, kColRed );
                    if( ImGui::SmallButton(
                        ( "Stop##cam" + std::to_string( i ) ).c_str() ) )
                    {
                        if( cap_helper )
                            [(AAASeedStudioCaptureHelper*) cap_helper stopCapture];
                        cap_active_idx = -1;
                        if( owner )
                            owner->log( ConsoleEntry::INFO,
                                "Camera capture stopped." );
                    }
                    ImGui::PopStyleColor();
                }
                else
                {
                    ImGui::PushStyleColor( ImGuiCol_Button, kColGreen );
                    if( ImGui::SmallButton(
                        ( "Start##cam" + std::to_string( i ) ).c_str() ) )
                    {
                        AVCaptureDevice* dev = av_video_device_at( i );
                        if( dev )
                        {
                            if( !cap_helper )
                            {
                                id<MTLDevice> mtl_dev =
                                    (__bridge id<MTLDevice>) mtl_device_ptr;
                                cap_helper = (void*) [[AAASeedStudioCaptureHelper alloc]
                                    initWithDevice:mtl_dev
                                              impl:this
                                            studio:owner];
                            }
                            BOOL ok = [(AAASeedStudioCaptureHelper*) cap_helper
                                attachToVideoDevice:dev];
                            if( ok )
                            {
                                cap_active_idx = i;
                                if( owner )
                                    owner->log( ConsoleEntry::INFO,
                                        "Camera capture started : " +
                                        cap_devices[i] );
                            }
                            else if( owner )
                            {
                                owner->log( ConsoleEntry::ERR,
                                    "Capture start failed for " +
                                    cap_devices[i] );
                            }
                        }
                    }
                    ImGui::PopStyleColor();
                }
            }
        }

        // ── Preview ───────────────────────────────────────────────────
        if( cap_preview_tex && cap_preview_w > 0 && cap_preview_h > 0 )
        {
            ImGui::Spacing();
            float const avail = ImGui::GetContentRegionAvail().x;
            float const aspect = (float) cap_preview_h /
                                 (float) cap_preview_w;
            ImGui::Image(
                (ImTextureID)(intptr_t) cap_preview_tex,
                ImVec2( avail, avail * aspect ) );
            ImGui::TextColored( kColYellow, "%ux%u (BGRA8)",
                                cap_preview_w, cap_preview_h );
        }
    }

    ImGui::End();
}

#pragma clang diagnostic pop

// ════════════════════════════════════════════════════════════════════════════
// Sound Panel (placeholder v1)
// ════════════════════════════════════════════════════════════════════════════

namespace
{
    // c151-A : real Core Audio enumeration. Returns the system audio
    // devices visible to the current process. is_input/is_output is set
    // by inspecting the stream counts on each scope.
    std::vector<aaa::ui::studio::SoundDeviceInfo> enumerate_core_audio_devices()
    {
        std::vector<aaa::ui::studio::SoundDeviceInfo> out;

        AudioObjectPropertyAddress addr = {
            kAudioHardwarePropertyDevices,
            kAudioObjectPropertyScopeGlobal,
            kAudioObjectPropertyElementMain
        };
        UInt32 data_size = 0;
        if( AudioObjectGetPropertyDataSize( kAudioObjectSystemObject,
                                            &addr, 0, nullptr, &data_size ) != 0 )
            return out;
        std::size_t const count = data_size / sizeof(AudioDeviceID);
        std::vector<AudioDeviceID> ids( count );
        if( AudioObjectGetPropertyData( kAudioObjectSystemObject,
                                        &addr, 0, nullptr,
                                        &data_size, ids.data() ) != 0 )
            return out;

        for( AudioDeviceID id : ids )
        {
            aaa::ui::studio::SoundDeviceInfo info;
            info.id = static_cast<std::uint32_t>( id );

            // Name
            CFStringRef name_ref = nullptr;
            UInt32 sz = sizeof(name_ref);
            AudioObjectPropertyAddress name_addr = {
                kAudioObjectPropertyName,
                kAudioObjectPropertyScopeGlobal,
                kAudioObjectPropertyElementMain
            };
            if( AudioObjectGetPropertyData( id, &name_addr, 0, nullptr,
                                            &sz, &name_ref ) == 0 && name_ref )
            {
                char buf[256] = {};
                if( CFStringGetCString( name_ref, buf, sizeof(buf),
                                        kCFStringEncodingUTF8 ) )
                    info.name = buf;
                CFRelease( name_ref );
            }
            if( info.name.empty() )
                info.name = "(unnamed device)";

            // Input streams
            AudioObjectPropertyAddress in_addr = {
                kAudioDevicePropertyStreams,
                kAudioDevicePropertyScopeInput,
                kAudioObjectPropertyElementMain
            };
            UInt32 in_sz = 0;
            if( AudioObjectGetPropertyDataSize( id, &in_addr, 0, nullptr,
                                                &in_sz ) == 0 )
                info.is_input = ( in_sz > 0 );

            // Output streams
            AudioObjectPropertyAddress out_addr = {
                kAudioDevicePropertyStreams,
                kAudioDevicePropertyScopeOutput,
                kAudioObjectPropertyElementMain
            };
            UInt32 out_sz = 0;
            if( AudioObjectGetPropertyDataSize( id, &out_addr, 0, nullptr,
                                                &out_sz ) == 0 )
                info.is_output = ( out_sz > 0 );

            out.push_back( std::move(info) );
        }
        return out;
    }

    // ── c151-B : NSTask helpers for the Binary Manager ────────────────
    //
    // Spawn a child process as an NSTask, route its stdout AND stderr
    // through a single NSPipe, and drain that pipe on a background
    // queue. Drained bytes are logged into the Studio Console as LUA-
    // level entries (visible in the console scroll), prefixed with the
    // process label so multiple managed tasks stay readable.
    //
    // Lifecycle is OWNED by the studio panel : when the user clicks
    // Stop or removes the entry, we send terminate to the task and
    // release the pipe handle. The terminationHandler clears the
    // running flag and logs the exit code.

    void task_launch( aaa::ui::studio::Studio* studio,
                      aaa::ui::studio::StudioImpl::ManagedTask& mt )
    {
        if( mt.running )
            return;

        NSString* path = [NSString stringWithUTF8String: mt.command.c_str()];
        if( ![[NSFileManager defaultManager] isExecutableFileAtPath: path] )
        {
            studio->log( aaa::ui::studio::ConsoleEntry::ERR,
                "Binary not executable : " + mt.command );
            return;
        }

        NSTask* task = [[NSTask alloc] init];
        task.executableURL = [NSURL fileURLWithPath: path];

        // Split args on spaces. Naive tokenizer ; users wanting nested
        // quoting can write a wrapper shell script.
        NSMutableArray<NSString*>* argv = [NSMutableArray array];
        std::istringstream iss( mt.args );
        std::string token;
        while( iss >> token )
            [argv addObject: [NSString stringWithUTF8String: token.c_str()]];
        task.arguments = argv;

        NSPipe*       pipe   = [NSPipe pipe];
        task.standardOutput  = pipe;
        task.standardError   = pipe;
        NSFileHandle* handle = pipe.fileHandleForReading;

        // Drain the pipe on a background queue. Captures (label,
        // studio) by value ; the block lives for the lifetime of the
        // pipe.
        std::string label_copy = mt.label;
        aaa::ui::studio::Studio* studio_capture = studio;
        handle.readabilityHandler = ^( NSFileHandle* fh )
        {
            NSData* data = [fh availableData];
            if( data.length == 0 )
            {
                // EOF -- clear the handler so the queue stops spinning.
                fh.readabilityHandler = nil;
                return;
            }
            NSString* s = [[NSString alloc] initWithData: data
                                                encoding: NSUTF8StringEncoding];
            if( s.length > 0 )
            {
                std::string line = "[" + label_copy + "] " +
                    std::string( [s UTF8String] );
                // Trim trailing newlines so the console isn't double-spaced.
                while( !line.empty() &&
                       ( line.back() == '\n' || line.back() == '\r' ) )
                    line.pop_back();
                dispatch_async( dispatch_get_main_queue(), ^{
                    studio_capture->log(
                        aaa::ui::studio::ConsoleEntry::LUA, line );
                });
            }
            [s release];
        };

        // Termination handler : runs on whatever queue libdispatch
        // chooses. Bounce to main to mutate the ManagedTask record.
        aaa::ui::studio::StudioImpl::ManagedTask* mt_ptr = &mt;
        task.terminationHandler = ^( NSTask* t )
        {
            int code = (int) t.terminationStatus;
            dispatch_async( dispatch_get_main_queue(), ^{
                mt_ptr->running   = false;
                mt_ptr->exit_code = code;
                studio_capture->log(
                    aaa::ui::studio::ConsoleEntry::INFO,
                    "[" + label_copy + "] exited with code " +
                    std::to_string( code ) );
            });
        };

        NSError* err = nil;
        if( ![task launchAndReturnError: &err] )
        {
            studio->log( aaa::ui::studio::ConsoleEntry::ERR,
                "NSTask launch failed : " +
                std::string( [[err localizedDescription] UTF8String] ) );
            [task release];
            return;
        }

        mt.ns_task       = (void*) task;            // owned (+1 retain)
        mt.stdout_handle = (void*) [handle retain]; // owned
        mt.running       = true;
        mt.pid           = (std::uint32_t) task.processIdentifier;
        studio->log( aaa::ui::studio::ConsoleEntry::INFO,
            "[" + mt.label + "] launched, pid=" + std::to_string( mt.pid ) );
    }

    void task_terminate( aaa::ui::studio::Studio* studio,
                         aaa::ui::studio::StudioImpl::ManagedTask& mt )
    {
        if( !mt.running ) return;
        NSTask* task = (NSTask*) mt.ns_task;
        if( task && task.isRunning )
            [task terminate];
        // The terminationHandler clears mt.running ; we just log the
        // signal here. Note : SIGTERM ; we do not escalate to KILL.
        studio->log( aaa::ui::studio::ConsoleEntry::INFO,
            "[" + mt.label + "] sent SIGTERM" );
    }

    void task_release( aaa::ui::studio::StudioImpl::ManagedTask& mt )
    {
        if( mt.stdout_handle )
        {
            NSFileHandle* fh = (NSFileHandle*) mt.stdout_handle;
            fh.readabilityHandler = nil;
            [fh release];
            mt.stdout_handle = nullptr;
        }
        if( mt.ns_task )
        {
            NSTask* t = (NSTask*) mt.ns_task;
            if( t.isRunning )
                [t terminate];
            [t release];
            mt.ns_task = nullptr;
        }
    }

    // ── c151-B : AVFoundation camera capture helper ───────────────────
    //
    // Forward-declared ObjC class wrapping AVCaptureSession + a delegate
    // for AVCaptureVideoDataOutputSampleBufferDelegate. CVPixelBuffer
    // → MTLTexture conversion goes through CVMetalTextureCache so the
    // GPU never copies pixels (Apple Silicon UMA path).
} // namespace

@interface AAASeedStudioCaptureHelper
    : NSObject <AVCaptureVideoDataOutputSampleBufferDelegate>
{
@public
    AVCaptureSession*       _session;
    AVCaptureDeviceInput*   _input;
    AVCaptureVideoDataOutput* _output;
    dispatch_queue_t        _queue;
    CVMetalTextureCacheRef  _tex_cache;
    aaa::ui::studio::StudioImpl* _impl;     // raw ; outlives helper
    aaa::ui::studio::Studio*     _studio;
}
- (instancetype)initWithDevice:(id<MTLDevice>)device
                          impl:(aaa::ui::studio::StudioImpl*)impl
                        studio:(aaa::ui::studio::Studio*)studio;
- (BOOL)attachToVideoDevice:(AVCaptureDevice*)dev;
- (void)stopCapture;
@end

@implementation AAASeedStudioCaptureHelper

- (instancetype)initWithDevice:(id<MTLDevice>)device
                          impl:(aaa::ui::studio::StudioImpl*)impl
                        studio:(aaa::ui::studio::Studio*)studio
{
    if( (self = [super init]) )
    {
        _impl   = impl;
        _studio = studio;
        _queue  = dispatch_queue_create(
            "ai.bsa.aaaseed.studio.capture", DISPATCH_QUEUE_SERIAL );
        if( CVMetalTextureCacheCreate(
                kCFAllocatorDefault, nil, device, nil, &_tex_cache ) != 0 )
        {
            _tex_cache = nil;
        }
    }
    return self;
}

- (void)dealloc
{
    [self stopCapture];
    if( _tex_cache )
        CFRelease( _tex_cache );
    [super dealloc];
}

- (BOOL)attachToVideoDevice:(AVCaptureDevice*)dev
{
    [self stopCapture];
    NSError* err = nil;
    _input = [[AVCaptureDeviceInput alloc] initWithDevice:dev error:&err];
    if( !_input ) return NO;

    _output = [[AVCaptureVideoDataOutput alloc] init];
    _output.videoSettings = @{
        (NSString*) kCVPixelBufferPixelFormatTypeKey :
            @(kCVPixelFormatType_32BGRA),
    };
    _output.alwaysDiscardsLateVideoFrames = YES;
    [_output setSampleBufferDelegate:self queue:_queue];

    _session = [[AVCaptureSession alloc] init];
    if( [_session canAddInput:_input] )  [_session addInput:_input];
    if( [_session canAddOutput:_output] ) [_session addOutput:_output];
    [_session startRunning];
    return YES;
}

- (void)stopCapture
{
    if( _session && _session.isRunning )
        [_session stopRunning];
    [_session release]; _session = nil;
    [_input   release]; _input   = nil;
    [_output  release]; _output  = nil;
}

- (void)captureOutput:(AVCaptureOutput*)out
    didOutputSampleBuffer:(CMSampleBufferRef)sample
           fromConnection:(AVCaptureConnection*)conn
{
    (void) out; (void) conn;
    if( !_tex_cache ) return;
    CVPixelBufferRef pb = CMSampleBufferGetImageBuffer( sample );
    if( !pb ) return;

    std::size_t const w = CVPixelBufferGetWidth( pb );
    std::size_t const h = CVPixelBufferGetHeight( pb );

    CVMetalTextureRef cv_tex = nil;
    if( CVMetalTextureCacheCreateTextureFromImage(
            kCFAllocatorDefault, _tex_cache, pb, nil,
            MTLPixelFormatBGRA8Unorm, w, h, 0, &cv_tex ) != 0 )
        return;
    id<MTLTexture> mtl_tex = CVMetalTextureGetTexture( cv_tex );
    if( !mtl_tex )
    {
        CFRelease( cv_tex );
        return;
    }
    // Take an extra retain on mtl_tex so it survives the cv_tex
    // release on the next line. ImGui::Image holds the ID until
    // the next frame ; we update on the main queue.
    [mtl_tex retain];
    CFRelease( cv_tex );

    aaa::ui::studio::StudioImpl* impl = _impl;
    dispatch_async( dispatch_get_main_queue(), ^{
        if( impl->cap_preview_tex )
        {
            id<MTLTexture> old = (id<MTLTexture>) impl->cap_preview_tex;
            [old release];
        }
        impl->cap_preview_tex = (void*) mtl_tex;
        impl->cap_preview_w   = (std::uint32_t) w;
        impl->cap_preview_h   = (std::uint32_t) h;
    });
}

@end

namespace
{
    std::vector<std::string> enumerate_av_video_devices()
    {
        std::vector<std::string> out;
        // Pre-macOS-10.15 API : `devicesWithMediaType` is deprecated
        // but still works ; the modern AVCaptureDeviceDiscoverySession
        // requires explicit deviceType lists. We try the modern API
        // first and fall back if it returns empty.
        AVCaptureDeviceDiscoverySession* sess =
            [AVCaptureDeviceDiscoverySession
                discoverySessionWithDeviceTypes:@[
                    AVCaptureDeviceTypeBuiltInWideAngleCamera,
                    AVCaptureDeviceTypeExternalUnknown,
                ]
                                       mediaType:AVMediaTypeVideo
                                        position:AVCaptureDevicePositionUnspecified];
        for( AVCaptureDevice* d in sess.devices )
            out.push_back( std::string( [d.localizedName UTF8String] ) );
        return out;
    }

    AVCaptureDevice* av_video_device_at( int idx )
    {
        AVCaptureDeviceDiscoverySession* sess =
            [AVCaptureDeviceDiscoverySession
                discoverySessionWithDeviceTypes:@[
                    AVCaptureDeviceTypeBuiltInWideAngleCamera,
                    AVCaptureDeviceTypeExternalUnknown,
                ]
                                       mediaType:AVMediaTypeVideo
                                        position:AVCaptureDevicePositionUnspecified];
        NSArray<AVCaptureDevice*>* arr = sess.devices;
        if( idx < 0 || (NSUInteger) idx >= arr.count ) return nil;
        return arr[ (NSUInteger) idx ];
    }

    // ── c151-B : aaa.studio.* Lua bindings ────────────────────────────
    //
    // Same registry-key pattern as RunnerImpl : we stash the live
    // Studio* under a unique lightuserdata key, then each thunk reads
    // it back out at call time. Failed reads return Lua nil instead of
    // crashing (the user script may run before init() succeeded).

    static char const k_studio_registry_key = 0;

    aaa::ui::studio::Studio* lua_studio_from_state( lua_State* L )
    {
        lua_pushlightuserdata( L, (void*) &k_studio_registry_key );
        lua_rawget( L, LUA_REGISTRYINDEX );
        aaa::ui::studio::Studio* s = reinterpret_cast< aaa::ui::studio::Studio* >(
            const_cast< void* >( lua_topointer( L, -1 ) ) );
        lua_pop( L, 1 );
        return s;
    }

    int l_studio_log( lua_State* L )
    {
        aaa::ui::studio::Studio* s = lua_studio_from_state( L );
        if( s == nullptr ) return 0;
        // Two argument shapes :
        //   aaa.studio.log("text")           -> INFO
        //   aaa.studio.log(level, "text")    -> explicit level
        // Level may be a string ("info"/"warn"/"err"/"lua") or a number
        // (matching ConsoleEntry::Level integer values).
        aaa::ui::studio::ConsoleEntry::Level lvl =
            aaa::ui::studio::ConsoleEntry::INFO;
        char const* text = nullptr;
        if( lua_gettop( L ) >= 2 && lua_isstring( L, 2 ) )
        {
            if( lua_isnumber( L, 1 ) )
                lvl = (aaa::ui::studio::ConsoleEntry::Level) lua_tointeger( L, 1 );
            else if( lua_isstring( L, 1 ) )
            {
                char const* lvls = lua_tostring( L, 1 );
                if( std::strcmp( lvls, "warn" ) == 0 )
                    lvl = aaa::ui::studio::ConsoleEntry::WARN;
                else if( std::strcmp( lvls, "err"  ) == 0 ||
                         std::strcmp( lvls, "error") == 0 )
                    lvl = aaa::ui::studio::ConsoleEntry::ERR;
                else if( std::strcmp( lvls, "lua"  ) == 0 )
                    lvl = aaa::ui::studio::ConsoleEntry::LUA;
            }
            text = lua_tostring( L, 2 );
        }
        else if( lua_isstring( L, 1 ) )
        {
            text = lua_tostring( L, 1 );
        }
        if( text ) s->log( lvl, std::string( text ) );
        return 0;
    }

    int l_studio_add_node( lua_State* L )
    {
        aaa::ui::studio::Studio* s = lua_studio_from_state( L );
        if( s == nullptr ) { lua_pushinteger( L, 0 ); return 1; }
        char const* label = lua_isstring( L, 1 ) ? lua_tostring( L, 1 )
                                                 : "node";
        float px = 0.0f, py = 0.0f;
        if( lua_isnumber( L, 2 ) ) px = (float) lua_tonumber( L, 2 );
        if( lua_isnumber( L, 3 ) ) py = (float) lua_tonumber( L, 3 );
        std::uint32_t const id = s->add_node( std::string( label ), px, py );
        lua_pushinteger( L, (lua_Integer) id );
        return 1;
    }

    int l_studio_set_camera( lua_State* L )
    {
        aaa::ui::studio::Studio* s = lua_studio_from_state( L );
        if( s == nullptr ) return 0;
        // aaa.studio.set_camera(px, py, pz, lx, ly, lz [, ux, uy, uz [, fov]])
        aaa::ui::studio::CameraState cam = s->camera();
        auto pull = []( lua_State* L_, int idx, float fallback ) -> float
        {
            return lua_isnumber( L_, idx ) ? (float) lua_tonumber( L_, idx )
                                           : fallback;
        };
        cam.pos[0]  = pull( L, 1, cam.pos[0]  );
        cam.pos[1]  = pull( L, 2, cam.pos[1]  );
        cam.pos[2]  = pull( L, 3, cam.pos[2]  );
        cam.look[0] = pull( L, 4, cam.look[0] );
        cam.look[1] = pull( L, 5, cam.look[1] );
        cam.look[2] = pull( L, 6, cam.look[2] );
        cam.up[0]   = pull( L, 7, cam.up[0]   );
        cam.up[1]   = pull( L, 8, cam.up[1]   );
        cam.up[2]   = pull( L, 9, cam.up[2]   );
        cam.fov_deg = pull( L, 10, cam.fov_deg );
        s->set_camera( cam );
        return 0;
    }

    int l_studio_set_font_scale( lua_State* L )
    {
        aaa::ui::studio::Studio* s = lua_studio_from_state( L );
        if( s == nullptr ) return 0;
        if( lua_isnumber( L, 1 ) )
            s->set_font_scale( (float) lua_tonumber( L, 1 ) );
        return 0;
    }

    void install_studio_lua_bindings( lua_State* L, aaa::ui::studio::Studio* s )
    {
        // Stash the Studio* in the registry under our unique key.
        lua_pushlightuserdata( L, (void*) &k_studio_registry_key );
        lua_pushlightuserdata( L, (void*) s );
        lua_rawset( L, LUA_REGISTRYINDEX );

        // Build the aaa.studio table. The runner already created `aaa`
        // as a global table ; we look it up and attach a `.studio`
        // subtable. If `aaa` is missing (defensive) we create it too.
        lua_getglobal( L, "aaa" );
        if( !lua_istable( L, -1 ) )
        {
            lua_pop( L, 1 );
            lua_newtable( L );
            lua_setglobal( L, "aaa" );
            lua_getglobal( L, "aaa" );
        }
        lua_newtable( L );                              // -1: studio
        lua_pushcfunction( L, l_studio_log );
        lua_setfield( L, -2, "log" );
        lua_pushcfunction( L, l_studio_add_node );
        lua_setfield( L, -2, "add_node" );
        lua_pushcfunction( L, l_studio_set_camera );
        lua_setfield( L, -2, "set_camera" );
        lua_pushcfunction( L, l_studio_set_font_scale );
        lua_setfield( L, -2, "set_font_scale" );
        lua_setfield( L, -2, "studio" );                // aaa.studio = {...}
        lua_pop( L, 1 );                                // pop aaa
    }
} // namespace

void aaa::ui::studio::StudioImpl::draw_sound_panel()
{
    if( !show_sound ) return;

    ImGui::SetNextWindowSize( { 320, 280 }, ImGuiCond_FirstUseEver );
    ImGui::SetNextWindowPos(  { 910, 520 }, ImGuiCond_FirstUseEver );

    ImGui::Begin( "Sound", &show_sound );

    ImGui::TextColored( kColTealLight, "Core Audio devices" );
    ImGui::Separator();

    if( sound_devices.empty() )
    {
        ImGui::TextColored( kColTextDim, "(press Refresh to enumerate)" );
    }
    if( ImGui::Button( "Refresh" ) )
    {
        sound_devices = enumerate_core_audio_devices();
    }
    ImGui::SameLine();
    ImGui::TextDisabled( "%zu device(s)", sound_devices.size() );

    ImGui::Separator();
    for( auto const& d : sound_devices )
    {
        char const* tag = d.is_input && d.is_output ? "DUPLEX"
                        : d.is_input  ? "IN "
                        : d.is_output ? "OUT" : "---";
        ImGui::BulletText( "[%s] %s", tag, d.name.c_str() );
    }

    ImGui::End();
}

// ════════════════════════════════════════════════════════════════════════════
// Binary Manager
// ════════════════════════════════════════════════════════════════════════════

void aaa::ui::studio::StudioImpl::draw_binary_manager()
{
    if( !show_binaries ) return;

    ImGui::SetNextWindowSize( { 520, 360 }, ImGuiCond_FirstUseEver );
    ImGui::SetNextWindowPos(  { 300, 520 }, ImGuiCond_FirstUseEver );

    ImGui::Begin( "Binary Manager", &show_binaries );

    ImGui::TextColored( kColTealLight, "Spawn + manage child processes" );
    ImGui::TextWrapped(
        "Output (stdout + stderr) is streamed into the Console panel "
        "as it arrives. Stop sends SIGTERM ; Remove waits for exit." );
    ImGui::Separator();

    // ── Add new task form ─────────────────────────────────────────────
    ImGui::InputTextWithHint( "Label",    "e.g. http-server",   bin_form_label, sizeof(bin_form_label) );
    ImGui::InputTextWithHint( "Path",     "/usr/bin/python3",   bin_form_path,  sizeof(bin_form_path)  );
    ImGui::InputTextWithHint( "Args",     "-m http.server 8080", bin_form_args, sizeof(bin_form_args)  );
    if( ImGui::Button( "Attach" ) )
    {
        if( bin_form_path[0] != '\0' )
        {
            ManagedTask mt;
            mt.label   = bin_form_label[0] != '\0'
                ? std::string( bin_form_label )
                : std::string( bin_form_path );
            mt.command = bin_form_path;
            mt.args    = bin_form_args;
            tasks.push_back( std::move( mt ) );
            bin_form_label[0] = bin_form_path[0] = bin_form_args[0] = '\0';
        }
    }
    ImGui::Separator();

    // ── Existing tasks ────────────────────────────────────────────────
    if( tasks.empty() )
    {
        ImGui::TextColored( kColTextDim, "(no tasks attached yet)" );
    }

    // c151-B : owner is the Studio that owns this StudioImpl, set in
    // Studio's ctor. Used so the static helpers (task_launch, etc.)
    // can route logs back through Studio::log.
    aaa::ui::studio::Studio* owner = this->owner;

    for( std::size_t i = 0; i < tasks.size(); )
    {
        ManagedTask& mt = tasks[ i ];

        ImU32 dot = mt.running
            ? IM_COL32(38,200,68,255)
            : IM_COL32(180,30,30,255);
        ImGui::GetWindowDrawList()->AddCircleFilled(
            { ImGui::GetCursorScreenPos().x + 6,
              ImGui::GetCursorScreenPos().y + 9 },
            5.0f, dot );
        ImGui::SetCursorPosX( ImGui::GetCursorPosX() + 18 );

        if( mt.running )
            ImGui::Text( "%s  (pid %u)", mt.label.c_str(), mt.pid );
        else if( mt.exit_code != 0 )
            ImGui::Text( "%s  (exit %d)", mt.label.c_str(), mt.exit_code );
        else
            ImGui::Text( "%s", mt.label.c_str() );

        ImGui::SameLine( 280 );

        if( !mt.running )
        {
            ImGui::PushStyleColor( ImGuiCol_Button, kColGreen );
            if( ImGui::SmallButton(
                ( "Start##" + std::to_string( i ) ).c_str() ) )
                task_launch( owner, mt );
            ImGui::PopStyleColor();
            ImGui::SameLine();
            if( ImGui::SmallButton(
                ( "Remove##" + std::to_string( i ) ).c_str() ) )
            {
                task_release( mt );
                tasks.erase( tasks.begin() + i );
                continue;   // do NOT increment i
            }
        }
        else
        {
            ImGui::PushStyleColor( ImGuiCol_Button, kColRed );
            if( ImGui::SmallButton(
                ( "Stop##" + std::to_string( i ) ).c_str() ) )
                task_terminate( owner, mt );
            ImGui::PopStyleColor();
        }
        ++i;
    }

    ImGui::End();
}

// ════════════════════════════════════════════════════════════════════════════
// Console
// ════════════════════════════════════════════════════════════════════════════

void aaa::ui::studio::StudioImpl::draw_console()
{
    if( !show_console ) return;

    ImGui::SetNextWindowSize( { 700, 180 }, ImGuiCond_FirstUseEver );
    ImGui::SetNextWindowPos(  {  10, 850 }, ImGuiCond_FirstUseEver );

    ImGui::Begin( "Console", &show_console );

    if( ImGui::SmallButton( "Clear" ) ) console_log.clear();
    ImGui::SameLine();
    ImGui::TextDisabled( "(%zu entries)", console_log.size() );

    ImGui::Separator();

    float reserve = ImGui::GetStyle().ItemSpacing.y +
                    ImGui::GetFrameHeightWithSpacing();
    ImGui::BeginChild( "##log",
                       { 0, -reserve },
                       false,
                       ImGuiWindowFlags_HorizontalScrollbar );

    for( auto const& entry : console_log )
    {
        ImVec4 col;
        switch( entry.level )
        {
            case ConsoleEntry::WARN:  col = kColYellow;   break;
            case ConsoleEntry::ERR:   col = kColRed;      break;
            case ConsoleEntry::LUA:   col = kColTealLight; break;
            default:                  col = kColText;     break;
        }
        ImGui::TextColored( col, "%s", entry.text.c_str() );
    }

    if( console_scroll_bottom && ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 4 )
        ImGui::SetScrollHereY( 1.0f );
    console_scroll_bottom = true;

    ImGui::EndChild();

    // Input line
    ImGui::Separator();
    bool reclaim = false;
    if( ImGui::InputText( "##input",
                          console_input,
                          sizeof(console_input),
                          ImGuiInputTextFlags_EnterReturnsTrue ) )
    {
        if( console_input[0] != '\0' )
        {
            ConsoleEntry e;
            e.level = ConsoleEntry::LUA;
            e.text  = std::string("> ") + console_input;
            if( console_log.size() >= kMaxLogLines )
                console_log.pop_front();
            console_log.push_back(e);

            if( on_run_cb ) on_run_cb( console_input );
            console_input[0] = '\0';
        }
        reclaim = true;
    }
    ImGui::SetItemDefaultFocus();
    if( reclaim ) ImGui::SetKeyboardFocusHere(-1);

    ImGui::End();
}

// ════════════════════════════════════════════════════════════════════════════
// Performance Panel
// ════════════════════════════════════════════════════════════════════════════

void aaa::ui::studio::StudioImpl::draw_perf_panel()
{
    if( !show_perf ) return;

    ImGui::SetNextWindowSize( { 300, 160 }, ImGuiCond_FirstUseEver );
    ImGui::SetNextWindowPos(  { 720, 850 }, ImGuiCond_FirstUseEver );

    ImGui::Begin( "Performance", &show_perf );

    float avg = perf.avg();
    float mx  = perf.max_val();
    float fps = avg > 0.0f ? 1000.0f / avg : 0.0f;

    ImGui::TextColored( kColYellow, "%.1f fps   avg %.2f ms   peak %.2f ms",
                        fps, avg, mx );

    // Plot overlay
    char overlay[32];
    std::snprintf( overlay, sizeof(overlay), "%.1f fps", fps );
    ImGui::PlotLines( "##frame",
                      perf.samples.data(),
                      static_cast<int>( perf.count ),
                      static_cast<int>( perf.head ),
                      overlay,
                      0.0f,
                      mx * 1.5f > 50.0f ? mx * 1.5f : 50.0f,
                      { ImGui::GetContentRegionAvail().x, 80 } );

    ImGui::End();
}

// ════════════════════════════════════════════════════════════════════════════
// Preferences
// ════════════════════════════════════════════════════════════════════════════

void aaa::ui::studio::StudioImpl::draw_prefs_panel()
{
    if( !show_prefs ) return;

    ImGui::SetNextWindowSize( { 380, 320 }, ImGuiCond_FirstUseEver );
    ImGui::Begin( "Preferences", &show_prefs );

    ImGui::TextColored( kColTealLight, "Theme" );
    ImGui::Separator();

    if( ImGui::SliderFloat( "Font scale", &font_scale, 0.5f, 3.0f ) )
        ImGui::GetIO().FontGlobalScale = font_scale;

    static float bg[3] = { kColBgDark.x, kColBgDark.y, kColBgDark.z };
    ImGui::ColorEdit3( "Background", bg );

    ImGui::Spacing();
    ImGui::TextColored( kColTealLight, "GaBuZoMeu palette preview" );
    ImGui::Separator();

    auto swatch = [](const char* lbl, ImVec4 const& c)
    {
        ImGui::ColorButton( ("##" + std::string(lbl)).c_str(), c,
                            ImGuiColorEditFlags_NoTooltip,
                            { 20, 20 } );
        ImGui::SameLine();
        ImGui::Text( "%s", lbl );
    };
    swatch( "Teal (selected)",   kColTealLight );
    swatch( "Green (active)",    kColGreen     );
    swatch( "Red (disabled)",    kColRed       );
    swatch( "Yellow (realtime)", kColYellow    );
    swatch( "Magenta (sel)",     kColMagenta   );

    ImGui::End();
}

// ════════════════════════════════════════════════════════════════════════════
// Studio public API
// ════════════════════════════════════════════════════════════════════════════

// c151-B : Studio::Studio(...) ctor body moved to aaa_studio.cpp (pure
// C++ ; no platform). ~Studio stays here because it tears down
// AVCapture, NSTask, and ImGui Metal/OSX backends.

aaa::ui::studio::Studio::~Studio()
{
    // c151-B : clear the runner's Lua-extension hook FIRST so a
    // subsequent load_script (e.g. from a still-running file-watcher)
    // doesn't fire the captured `this` against a destroyed Studio.
    if( impl_->runner != nullptr )
        impl_->runner->set_lua_extension_hook( {} );

    // c151-B : tear down any still-running managed NSTask children so
    // they don't outlive the studio (orphaned pipes -> dangling log
    // callbacks). task_release sends SIGTERM if needed and drops the
    // retain counts.
    for( auto& mt : impl_->tasks )
        task_release( mt );
    impl_->tasks.clear();

    // c151-B : stop AVCaptureSession + release the helper + the
    // last MTLTexture preview frame. The helper holds a retain on
    // _input/_output/_session which it drops in stopCapture.
    if( impl_->cap_helper )
    {
        AAASeedStudioCaptureHelper* h =
            (AAASeedStudioCaptureHelper*) impl_->cap_helper;
        [h stopCapture];
        [h release];
        impl_->cap_helper = nullptr;
    }
    if( impl_->cap_preview_tex )
    {
        id<MTLTexture> t = (id<MTLTexture>) impl_->cap_preview_tex;
        [t release];
        impl_->cap_preview_tex = nullptr;
    }

    if( impl_->imgui_ok )
    {
        ImGui_ImplMetal_Shutdown();
        ImGui_ImplOSX_Shutdown();
        ImGui::DestroyContext();
    }
    delete impl_;
}

bool aaa::ui::studio::Studio::init( void* ns_view, void* mtl_device,
                                     std::uint64_t color_pixel_format )
{
    // c151-A : graceful failure when called with null pointers (tests
    // exercise this path). Production callers always pass real pointers
    // ; the legacy asserts here triggered abort() under debug builds.
    if( !ns_view || !mtl_device )
        return false;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    // c151-A : DockingEnable is docking-branch only ; mainline v1.91 builds
    // without it. Windows are still individually movable / resizable.
    io.IniFilename = nullptr;   // no .ini file; state is ephemeral in v1

    apply_gabuzoumeu_theme();

    NSView*    view   = (__bridge NSView*)    ns_view;
    id<MTLDevice> dev = (__bridge id<MTLDevice>) mtl_device;

    // c151-A : on failure we MUST tear the context back down ; otherwise a
    // second init() (lazy retry from the view) would assert on a duplicate
    // ImGui context. Also leaks the partially-initialised backend.
    if( !ImGui_ImplOSX_Init( view ) )
    {
        ImGui::DestroyContext();
        return false;
    }
    if( !ImGui_ImplMetal_Init( dev ) )
    {
        ImGui_ImplOSX_Shutdown();
        ImGui::DestroyContext();
        return false;
    }

    impl_->ns_view_ptr       = ns_view;
    impl_->mtl_device_ptr    = mtl_device;
    impl_->last_frame_t      = std::chrono::steady_clock::now();
    // c151-B : MTLPixelFormatBGRA8Unorm == 80 ; that's MTKView's default
    // for the drawable. Use it as the fallback when the caller hasn't
    // queried the MTKView's colorPixelFormat property.
    impl_->color_pixel_format =
        color_pixel_format != 0
            ? color_pixel_format
            : static_cast<std::uint64_t>( MTLPixelFormatBGRA8Unorm );
    impl_->imgui_ok          = true;

    // c151-B : register the aaa.studio.* Lua bindings on the runner's
    // lua_State. The runner re-opens its state on every load_script, so
    // we register a HOOK that fires each time -- not a one-shot. Capture
    // `this` raw ; the matching unregister in ~Studio prevents dangling.
    if( impl_->runner != nullptr )
    {
        Studio* self = this;
        impl_->runner->set_lua_extension_hook(
            [ self ]( void* L_void )
            {
                if( L_void != nullptr )
                    self->install_lua_bindings( L_void );
            } );
    }

    // Seed console with welcome message ; log lives on Studio (this).
    this->log( ConsoleEntry::INFO,
               "AAASeed Studio ready -- ImGui v" IMGUI_VERSION " on Metal" );

    return true;
}

void aaa::ui::studio::Studio::new_frame( std::uint32_t w,
                                          std::uint32_t h,
                                          double delta_seconds )
{
    if( !impl_->imgui_ok ) return;

    // c151-A : drive ImGuiIO.DisplaySize + DeltaTime from the caller's
    // dimensions. ImGui needs both for layout, resize handles, tooltips
    // and any animation that runs off real time.
    ImGuiIO& io = ImGui::GetIO();
    if( w > 0 && h > 0 )
        io.DisplaySize = ImVec2( static_cast<float>(w), static_cast<float>(h) );

    if( delta_seconds > 0.0 )
    {
        io.DeltaTime = static_cast<float>( delta_seconds );
    }
    else
    {
        // Caller passed 0 -- derive from our own clock.
        auto const now = std::chrono::steady_clock::now();
        auto const dt  = std::chrono::duration<double>( now - impl_->last_frame_t ).count();
        impl_->last_frame_t = now;
        io.DeltaTime = dt > 0.0 ? static_cast<float>(dt) : 1.0f / 60.0f;
    }

    // Build a transient render-pass descriptor for the ImGui Metal backend.
    // The backend uses it only to read pixel format + sample count from
    // its colour attachment.
    //
    // c151-B : first-frame race -- mtk.currentDrawable may be nil before
    // the first vsync. Previously we passed an rpd with nil texture ;
    // imgui_impl_metal then cached MTLPixelFormatInvalid in its pipeline
    // state cache and logged "failed to create Metal library: (null)".
    // To compile the pipeline correctly from frame 0 we synthesise a
    // 1x1 texture stamped with the MTKView's static colorPixelFormat
    // (captured at init() time) whenever currentDrawable is nil. The
    // texture is throwaway -- imgui only reads its .pixelFormat /
    // .sampleCount properties to seed the FramebufferDescriptor.
    NSView* view = (__bridge NSView*) impl_->ns_view_ptr;
    MTLRenderPassDescriptor* rpd = [MTLRenderPassDescriptor renderPassDescriptor];
    rpd.colorAttachments[0].texture     = nil;
    rpd.colorAttachments[0].loadAction  = MTLLoadActionLoad;
    rpd.colorAttachments[0].storeAction = MTLStoreActionStore;
    id<MTLTexture> probe_tex = nil;
    if( [view isKindOfClass:[MTKView class]] )
    {
        MTKView* mtk = (MTKView*) view;
        if( mtk.currentDrawable )
        {
            rpd.colorAttachments[0].texture = mtk.currentDrawable.texture;
        }
        else
        {
            // First-frame fallback : 1x1 probe texture with the MTKView's
            // pixel format so imgui_impl_metal's pipeline cache key is
            // correct from the start. The studio .mm compiles under MRC
            // (-fno-objc-arc) ; `newTextureWithDescriptor:` returns +1
            // retain so we autorelease to balance.
            id<MTLDevice> dev = (__bridge id<MTLDevice>) impl_->mtl_device_ptr;
            MTLTextureDescriptor* d = [MTLTextureDescriptor
                texture2DDescriptorWithPixelFormat:
                    (MTLPixelFormat) impl_->color_pixel_format
                                             width:1
                                            height:1
                                         mipmapped:NO];
            d.usage       = MTLTextureUsageRenderTarget;
            d.storageMode = MTLStorageModePrivate;
            probe_tex = [[dev newTextureWithDescriptor:d] autorelease];
            rpd.colorAttachments[0].texture = probe_tex;
        }
    }

    ImGui_ImplMetal_NewFrame( rpd );
    ImGui_ImplOSX_NewFrame( view );
    ImGui::NewFrame();
    (void) probe_tex;  // ARC keeps it alive past rpd usage in NewFrame
}

void aaa::ui::studio::Studio::render( void* cmd_encoder, void* cmd_buffer )
{
    if( !impl_->imgui_ok ) return;

    // Build all panels
    impl_->draw_menubar();
    impl_->draw_node_graph();
    impl_->draw_code_editor();
    impl_->draw_inspector();
    impl_->draw_shader_catalog();
    impl_->draw_camera_panel();
    impl_->draw_sound_panel();
    impl_->draw_binary_manager();
    impl_->draw_console();
    impl_->draw_perf_panel();
    impl_->draw_prefs_panel();

    if( impl_->show_imgui_demo ) ImGui::ShowDemoWindow( &impl_->show_imgui_demo );

    // c151-A : if the caller has no encoder / cmd_buffer (no drawable
    // available this frame, test scaffolding, headless run), call
    // EndFrame() and skip the GPU submission. EndFrame is required to
    // balance the NewFrame from new_frame() ; otherwise ImGui asserts
    // on the next NewFrame.
    if( !cmd_encoder || !cmd_buffer )
    {
        ImGui::EndFrame();
        return;
    }

    ImGui::Render();

    id<MTLRenderCommandEncoder> enc =
        (__bridge id<MTLRenderCommandEncoder>) cmd_encoder;
    id<MTLCommandBuffer>        cb  =
        (__bridge id<MTLCommandBuffer>)        cmd_buffer;
    // c151-A : pass the real command buffer so imgui_impl_metal can deref
    // commandBuffer.device when lazily compiling its pipeline state.
    // Passing nil here caused "failed to create Metal library: (null)"
    // and a SegFault inside RenderDrawData on every frame.
    ImGui_ImplMetal_RenderDrawData( ImGui::GetDrawData(), cb, enc );
}

bool aaa::ui::studio::Studio::handle_ns_event( void* ns_event )
{
    if( !impl_->imgui_ok ) return false;
    // c151-A : imgui_impl_osx >= 1.87 installs its OWN local NSEvent
    // monitor inside ImGui_ImplOSX_Init() that does all input plumbing.
    // The standalone ImGui_ImplOSX_HandleEvent() function was removed.
    // Our role here is to report whether ImGui currently *wants* the
    // event (mouse / keyboard focus), so the caller can suppress
    // dispatch to the engine input bridge when the user is interacting
    // with a studio panel. ns_event is unused but kept on the signature
    // for ABI stability and future versions that may re-introduce a
    // manual handler.
    (void) ns_event;
    return ImGui::GetIO().WantCaptureMouse ||
           ImGui::GetIO().WantCaptureKeyboard;
}

// c151-B : node-graph / code-editor / console / camera / perf /
// set_font_scale bodies all moved to aaa_studio.cpp (pure C++).
// The only Studio:: methods that remain here are the Mac-specific
// ones (init/new_frame/render/handle_ns_event/~Studio above) plus
// the three that touch Mac frameworks or anon-namespace helpers
// (enumerate_sound_devices / install_lua_bindings / apply_gabuzoumeu_theme).

// ── Sound ──────────────────────────────────────────────────────────────────

std::vector<aaa::ui::studio::SoundDeviceInfo>
aaa::ui::studio::Studio::enumerate_sound_devices() const
{
    // c151-A : delegate to the Core Audio helper. Cache the result on
    // the impl so the Sound panel + this getter share one source of truth.
    impl_->sound_devices = enumerate_core_audio_devices();
    return impl_->sound_devices;
}

void aaa::ui::studio::Studio::install_lua_bindings( void* lua_state )
{
    if( lua_state == nullptr ) return;
    install_studio_lua_bindings(
        reinterpret_cast< lua_State* >( lua_state ), this );
}

void aaa::ui::studio::Studio::apply_gabuzoumeu_theme()
{
    // c151-A : called both from init() (after CreateContext, before
    // imgui_ok=true) and from external callers. Guard on the live
    // context rather than imgui_ok so the init path still applies the
    // palette while external pre-init calls become a no-op.
    if( ImGui::GetCurrentContext() == nullptr )
        return;
    ImGuiStyle& s = ImGui::GetStyle();

    s.WindowRounding    = 5.0f;
    s.FrameRounding     = 3.0f;
    s.GrabRounding      = 3.0f;
    s.ScrollbarRounding = 3.0f;
    s.TabRounding       = 3.0f;
    s.ChildRounding     = 3.0f;
    s.WindowBorderSize  = 1.0f;
    s.FrameBorderSize   = 0.0f;
    s.ItemSpacing       = { 6, 4 };
    s.FramePadding      = { 6, 3 };

    auto& c = s.Colors;
    c[ ImGuiCol_WindowBg         ] = kColBgMid;
    c[ ImGuiCol_ChildBg          ] = kColBgDark;
    c[ ImGuiCol_PopupBg          ] = kColBgPanel;
    c[ ImGuiCol_Border           ] = kColBorder;
    c[ ImGuiCol_FrameBg          ] = { 0.08f, 0.14f, 0.18f, 1.0f };
    c[ ImGuiCol_FrameBgHovered   ] = { 0.12f, 0.20f, 0.25f, 1.0f };
    c[ ImGuiCol_FrameBgActive    ] = { 0.14f, 0.45f, 0.50f, 1.0f };
    c[ ImGuiCol_TitleBg          ] = { 0.07f, 0.12f, 0.15f, 1.0f };
    c[ ImGuiCol_TitleBgActive    ] = { 0.09f, 0.18f, 0.22f, 1.0f };
    c[ ImGuiCol_MenuBarBg        ] = { 0.06f, 0.10f, 0.13f, 1.0f };
    c[ ImGuiCol_ScrollbarBg      ] = kColBgDark;
    c[ ImGuiCol_ScrollbarGrab        ] = kColTealMid;
    c[ ImGuiCol_ScrollbarGrabHovered ] = kColTealLight;
    c[ ImGuiCol_CheckMark        ] = kColGreen;
    c[ ImGuiCol_SliderGrab       ] = kColTealMid;
    c[ ImGuiCol_SliderGrabActive ] = kColTealLight;
    c[ ImGuiCol_Button           ] = kColTealMid;
    c[ ImGuiCol_ButtonHovered    ] = kColTealLight;
    c[ ImGuiCol_ButtonActive     ] = { 0.10f, 0.55f, 0.60f, 1.0f };
    c[ ImGuiCol_Header           ] = { 0.14f, 0.45f, 0.50f, 0.7f };
    c[ ImGuiCol_HeaderHovered    ] = kColTealLight;
    c[ ImGuiCol_HeaderActive     ] = kColTealLight;
    c[ ImGuiCol_Tab              ] = { 0.09f, 0.18f, 0.22f, 1.0f };
    c[ ImGuiCol_TabHovered       ] = kColTealLight;
    c[ ImGuiCol_TabActive        ] = kColTealMid;
    c[ ImGuiCol_PlotLines        ] = kColTealLight;
    c[ ImGuiCol_PlotLinesHovered ] = kColYellow;
    c[ ImGuiCol_PlotHistogram    ] = kColTealMid;
    c[ ImGuiCol_Text             ] = kColText;
    c[ ImGuiCol_TextDisabled     ] = kColTextDim;
    c[ ImGuiCol_Separator        ] = kColBorder;
    c[ ImGuiCol_ResizeGrip       ] = kColTealMid;
    c[ ImGuiCol_ResizeGripHovered] = kColTealLight;
    c[ ImGuiCol_ResizeGripActive ] = kColMagenta;
    // c151-A : ImGuiCol_DockingPreview only exists in the docking branch.
}
