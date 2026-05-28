// src/ui/studio/aaa_studio.cpp
//
// c151-B : platform-neutral data-model bodies for the AAASeed ImGui
// Studio. Compiles on any platform that has a C++20 toolchain + Dear
// ImGui (mainline v1.91+).
//
// What lives HERE (pure C++) :
//   - Studio::Studio(...) constructor (sets impl fields)
//   - Node-graph mutations : add_node / remove_node / link_nodes /
//     unlink / find_node / nodes() / links()
//   - Code-editor data : set_editor_text / editor_text / on_run_script
//   - Console : log
//   - Camera : camera() / set_camera()
//   - Perf : push_perf_sample
//   - Preferences : set_font_scale (light ImGui touch ; ImGui is portable)
//
// What lives in `aaa_studio_metal.mm` (Mac-specific) :
//   - Studio::~Studio (tears down AVCapture + NSTask + ImGui backends)
//   - init / new_frame / render / handle_ns_event (ObjC + Metal + Cocoa)
//   - apply_gabuzoumeu_theme (ImGui-portable, kept on the platform side
//     for now since it's called from init())
//   - draw_* panel bodies (call ImGui, but the file also references
//     ObjC for the AVCapture + NSTask panels)
//   - enumerate_sound_devices (Core Audio)
//   - install_lua_bindings + the actual aaa.studio.* C functions
//
// On a future Windows / Linux port, this `.cpp` is the SHARED backbone ;
// a sibling `aaa_studio_dx12.cpp` / `aaa_studio_vulkan.cpp` would supply
// the platform-specific bodies the .mm currently owns.

#include "src/ui/studio/aaa_studio_impl.h"

#include "imgui.h"

#include <algorithm>
#include <chrono>
#include <utility>

// ── Studio ctor ─────────────────────────────────────────────────────────────

aaa::ui::studio::Studio::Studio( GOL::Backend* backend,
                                  aaa::meu::Runner* runner )
    : impl_( new StudioImpl )
{
    impl_->backend = backend;
    impl_->runner  = runner;
    impl_->owner   = this;
}

// ── Node graph ──────────────────────────────────────────────────────────────

std::uint32_t aaa::ui::studio::Studio::add_node( std::string const& label,
                                                  float px, float py )
{
    StudioNode n;
    n.id    = impl_->next_node_id++;
    n.label = label;
    if( px == 0.0f && py == 0.0f )
        impl_->auto_layout_pos( n.pos_x, n.pos_y );
    else
    {
        n.pos_x = px;
        n.pos_y = py;
    }
    impl_->nodes.push_back( std::move( n ) );
    return impl_->nodes.back().id;
}

void aaa::ui::studio::Studio::remove_node( std::uint32_t id )
{
    auto& ns = impl_->nodes;
    auto& ls = impl_->links;
    ls.erase( std::remove_if( ls.begin(), ls.end(),
        [ id ]( NodeLink const& l ){
            return l.from_pin.node_id == id ||
                   l.to_pin.node_id   == id; } ),
        ls.end() );
    ns.erase( std::remove_if( ns.begin(), ns.end(),
        [ id ]( StudioNode const& n ){ return n.id == id; } ),
        ns.end() );
}

std::uint32_t aaa::ui::studio::Studio::link_nodes( std::uint32_t from_id,
                                                    std::uint32_t to_id )
{
    if( !impl_->find_node_by_id( from_id ) ) return 0;
    if( !impl_->find_node_by_id( to_id   ) ) return 0;
    if( from_id == to_id ) return 0;

    NodeLink lnk;
    lnk.id                 = impl_->next_link_id++;
    lnk.from_pin.node_id   = from_id;
    lnk.from_pin.is_output = true;
    lnk.to_pin.node_id     = to_id;
    lnk.to_pin.is_output   = false;
    impl_->links.push_back( lnk );
    return lnk.id;
}

void aaa::ui::studio::Studio::unlink( std::uint32_t link_id )
{
    auto& ls = impl_->links;
    ls.erase( std::remove_if( ls.begin(), ls.end(),
        [ link_id ]( NodeLink const& l ){ return l.id == link_id; } ),
        ls.end() );
}

aaa::ui::studio::StudioNode*
aaa::ui::studio::Studio::find_node( std::uint32_t id )
{
    return impl_->find_node_by_id( id );
}

aaa::ui::studio::StudioNode const*
aaa::ui::studio::Studio::find_node( std::uint32_t id ) const
{
    for( auto const& n : impl_->nodes )
        if( n.id == id ) return &n;
    return nullptr;
}

std::vector< aaa::ui::studio::StudioNode > const&
aaa::ui::studio::Studio::nodes() const
{
    return impl_->nodes;
}

std::vector< aaa::ui::studio::NodeLink > const&
aaa::ui::studio::Studio::links() const
{
    return impl_->links;
}

// ── Code editor ─────────────────────────────────────────────────────────────

void aaa::ui::studio::Studio::set_editor_text( std::string const& text )
{
    impl_->editor_buf = text;
}

std::string const& aaa::ui::studio::Studio::editor_text() const
{
    return impl_->editor_buf;
}

void aaa::ui::studio::Studio::on_run_script(
    std::function< void( std::string const& ) > cb )
{
    impl_->on_run_cb = std::move( cb );
}

// ── Console ─────────────────────────────────────────────────────────────────

void aaa::ui::studio::Studio::log( ConsoleEntry::Level level,
                                    std::string const& text )
{
    ConsoleEntry e;
    e.level = level;
    e.text  = text;
    e.frame = static_cast< std::uint32_t >( impl_->console_log.size() );
    if( impl_->console_log.size() >= StudioImpl::kMaxLogLines )
        impl_->console_log.pop_front();
    impl_->console_log.push_back( std::move( e ) );
    impl_->console_scroll_bottom = true;
}

// ── Camera ──────────────────────────────────────────────────────────────────

aaa::ui::studio::CameraState const&
aaa::ui::studio::Studio::camera() const
{
    return impl_->camera;
}

void aaa::ui::studio::Studio::set_camera( CameraState const& cam )
{
    impl_->camera = cam;
}

// ── Perf ────────────────────────────────────────────────────────────────────

void aaa::ui::studio::Studio::push_perf_sample( float frame_ms )
{
    impl_->perf.push( frame_ms );
}

// ── Preferences ─────────────────────────────────────────────────────────────

void aaa::ui::studio::Studio::set_font_scale( float scale )
{
    impl_->font_scale = scale;
    // ImGui::GetIO() asserts without an active context. Only apply the
    // live value when init() has succeeded ; the impl_->font_scale is
    // still updated above so the next apply_gabuzoumeu_theme() (or a
    // re-issue of this call after init) picks it up.
    if( impl_->imgui_ok && ImGui::GetCurrentContext() != nullptr )
        ImGui::GetIO().FontGlobalScale = scale;
}
