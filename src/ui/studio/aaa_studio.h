// src/ui/studio/aaa_studio.h
//
// AAASeed Studio -- platform-neutral data model.
//
// c152-C : after ImGui retirement, this header is the SOLE public
// surface of the Studio. It carries no ObjC, no Cocoa, no Metal, no
// ImGui. The Qt6 UI in src/ui/qt/ binds to this surface ; future
// non-Mac ports build it as-is.
//
// History :
//   c148   : born as the Dear ImGui Studio's public header.
//   c151-B : data model extracted into aaa_studio.cpp ; .mm kept the
//            ImGui-specific lifecycle methods.
//   c152-C : ImGui retired ; init/new_frame/render/handle_ns_event /
//            install_lua_bindings / apply_gabuzoumeu_theme /
//            enumerate_sound_devices removed from the public surface.
//            Those will return on the Qt side as separate adapter
//            objects -- they don't belong on the data model.

#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace GOL  { class Backend; }
namespace aaa  { namespace meu  { class Runner; } }

namespace aaa
{
namespace ui
{
namespace studio
{

// ── Node graph data model ─────────────────────────────────────────────────

struct NodePin
{
    std::uint32_t node_id   = 0;
    std::uint32_t pin_idx   = 0;
    bool          is_output = false;
};

struct NodeLink
{
    std::uint32_t id = 0;
    NodePin       from_pin;   // output
    NodePin       to_pin;     // input
};

struct StudioNode
{
    std::uint32_t id          = 0;
    std::string   label;
    float         pos_x       = 100.0f;
    float         pos_y       = 100.0f;
    bool          enabled     = true;
    bool          selected    = false;
    std::string   shader_name;
    std::string   script_path;
    std::unordered_map< std::string, float > uniforms;
};

// ── Console log entry ─────────────────────────────────────────────────────

struct ConsoleEntry
{
    enum Level { INFO, WARN, ERR, LUA };
    Level         level = INFO;
    std::string   text;
    std::uint32_t frame = 0;
};

// ── Camera state ──────────────────────────────────────────────────────────

struct CameraState
{
    float pos[ 3 ]   = { 0.0f, 0.0f, 5.0f };
    float look[ 3 ]  = { 0.0f, 0.0f, 0.0f };
    float up[ 3 ]    = { 0.0f, 1.0f, 0.0f };
    float fov_deg    = 60.0f;
    float near_z     = 0.01f;
    float far_z      = 1000.0f;
};

// ── Sound device info (kept for future Sound panel ; populated by a
//    platform-specific helper -- absent on Qt v1) ───────────────────────────

struct SoundDeviceInfo
{
    std::uint32_t id        = 0;
    std::string   name;
    bool          is_input  = false;
    bool          is_output = false;
};

// ── Studio ────────────────────────────────────────────────────────────────

struct StudioImpl;

class Studio
{
public:
    explicit Studio( GOL::Backend* backend, aaa::meu::Runner* runner );
    ~Studio();

    Studio( Studio const& )            = delete;
    Studio& operator=( Studio const& ) = delete;

    // ── Node graph ────────────────────────────────────────────────────
    std::uint32_t add_node( std::string const& label,
                             float pos_x = 0.0f, float pos_y = 0.0f );
    void          remove_node( std::uint32_t id );
    std::uint32_t link_nodes( std::uint32_t from_node, std::uint32_t to_node );
    void          unlink( std::uint32_t link_id );
    StudioNode*       find_node( std::uint32_t id );
    StudioNode const* find_node( std::uint32_t id ) const;
    std::vector< StudioNode > const& nodes() const;
    std::vector< NodeLink >   const& links() const;

    // c152-K : per-node uniforms (key=value). Setting an empty key
    // erases ; setting any key flips the project-dirty flag.
    void  set_node_uniform( std::uint32_t id,
                             std::string const& key, float value );
    void  erase_node_uniform( std::uint32_t id, std::string const& key );
    void  clear_node_uniforms( std::uint32_t id );

    // ── Code editor buffer ────────────────────────────────────────────
    void               set_editor_text( std::string const& text );
    std::string const& editor_text() const;
    void               on_run_script(
        std::function< void( std::string const& ) > cb );

    // ── Console ───────────────────────────────────────────────────────
    void log( ConsoleEntry::Level level, std::string const& text );

    // ── Camera ────────────────────────────────────────────────────────
    CameraState const& camera() const;
    void               set_camera( CameraState const& cam );

    // ── Perf samples (rendering pipeline pushes one per frame) ────────
    void push_perf_sample( float frame_ms );

    // ── Preferences ───────────────────────────────────────────────────
    void  set_font_scale( float scale );
    float font_scale() const;

    // ── Project file I/O ──────────────────────────────────────────────
    void        new_project();
    bool        open_project( std::string const& path );
    bool        save_project_to( std::string const& path );
    // c152-O : in-memory variants used by undo/redo snapshots. These
    // do NOT touch the project_path / project_dirty flags ; they're a
    // pure round-trip of the Lua-serialized tree.
    std::string serialize_to_string() const;
    bool        load_from_string( std::string const& blob );
    std::string project_name()  const;
    std::string project_path()  const;
    bool        project_dirty() const;
    std::string add_asset_from_file( std::string const& src );
    std::vector< std::string > const& assets() const;

private:
    StudioImpl* impl_ = nullptr;
};

} // namespace studio
} // namespace ui
} // namespace aaa
