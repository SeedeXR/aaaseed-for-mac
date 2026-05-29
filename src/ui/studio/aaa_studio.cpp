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

// c152-C : ImGui dependency dropped. The Studio is pure data ;
// rendering / IO lives on the Qt side now.

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <system_error>
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

aaa::ui::studio::Studio::~Studio()
{
    // c152-C : platform tear-down (NSEvent monitor, AVCapture, NSTask,
    // ImGui shutdown) all retired with the .mm file. The data-model
    // dtor is just an impl free.
    delete impl_;
}

float aaa::ui::studio::Studio::font_scale() const
{
    return impl_->font_scale_value;
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

    // c152-L : reject duplicate edges (same from→to already wired).
    for( auto const& existing : impl_->links )
        if( existing.from_pin.node_id == from_id &&
            existing.to_pin.node_id   == to_id )
            return 0;

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

void aaa::ui::studio::Studio::set_node_uniform( std::uint32_t id,
    std::string const& key, float value )
{
    if( key.empty() ) return;
    auto* n = impl_->find_node_by_id( id );
    if( !n ) return;
    n->uniforms[ key ] = value;
    impl_->project_dirty = true;
}

void aaa::ui::studio::Studio::erase_node_uniform( std::uint32_t id,
    std::string const& key )
{
    auto* n = impl_->find_node_by_id( id );
    if( !n ) return;
    if( n->uniforms.erase( key ) > 0 )
        impl_->project_dirty = true;
}

void aaa::ui::studio::Studio::clear_node_uniforms( std::uint32_t id )
{
    auto* n = impl_->find_node_by_id( id );
    if( !n ) return;
    if( !n->uniforms.empty() )
    {
        n->uniforms.clear();
        impl_->project_dirty = true;
    }
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
    // c152-C : auto-scroll flag dropped along with the ImGui Console
    // panel ; Qt's ConsolePanel listens to logLine signals and
    // positions its own ListView.
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
    impl_->font_scale_value = scale;
    // c152-C : ImGui IO touch removed. The Qt UI reads font_scale()
    // and applies it via QML's FontLoader/Style.
}

// ── Project file I/O (c151-C + c152-A) ─────────────────────────────────────
//
// Pure-C++ implementation : no Cocoa, no Qt. The Mac/Qt sides invoke
// these via the public Studio API. File dialogs (NSOpenPanel /
// QFileDialog) live in the platform sub-layers ; this file owns the
// serialisation format.

namespace
{
    // Wrap a string in a Lua [==[...]==] long-bracket. The double `==`
    // accepts at most two levels of nested `]==]` inside the payload ;
    // user-pasted source code rarely contains that pattern, and if it
    // does we widen the wrap pads.
    std::string lua_long_string( std::string const& s )
    {
        return "[==[" + s + "]==]";
    }
}

void aaa::ui::studio::Studio::new_project()
{
    impl_->nodes.clear();
    impl_->links.clear();
    impl_->assets.clear();
    impl_->editor_buf.clear();
    impl_->camera        = CameraState{};
    impl_->next_node_id  = 1;
    impl_->next_link_id  = 1;
    impl_->project_path.clear();
    impl_->project_name  = "Untitled";
    impl_->project_dirty = false;
    this->log( ConsoleEntry::INFO,
        "New project (in memory). Use Save As to persist." );
}

namespace
{
    // c152-O : shared serializer body used by both save_project_to
    // (file path) and serialize_to_string (in-memory snapshot for
    // undo/redo). Writes the same .aaaproj.lua format either way.
    void serialize_into(
        std::ostream& out,
        aaa::ui::studio::StudioImpl const* impl )
    {
        using namespace aaa::ui::studio;
        out << "-- AAASeed project file ; auto-generated.\n";
        out << "-- Hand-edit at your own risk.\n";
        out << "return {\n";
        out << "  version = 1,\n";
        out << "  name    = " << lua_long_string( impl->project_name ) << ",\n";
        out << "  nodes   = {\n";
        for( auto const& n : impl->nodes )
        {
            out << "    { id="    << n.id
                << ", label="     << lua_long_string( n.label )
                << ", x="         << n.pos_x
                << ", y="         << n.pos_y
                << ", shader="    << lua_long_string( n.shader_name )
                << ", script="    << lua_long_string( n.script_path )
                << ", enabled="   << ( n.enabled ? "true" : "false" );
            if( !n.uniforms.empty() )
            {
                out << ", uniforms={";
                bool first = true;
                for( auto const& kv : n.uniforms )
                {
                    if( !first ) out << ", ";
                    first = false;
                    out << "[ " << lua_long_string( kv.first ) << " ]="
                        << kv.second;
                }
                out << "}";
            }
            out << " },\n";
        }
        out << "  },\n";
        out << "  links   = {\n";
        for( auto const& l : impl->links )
        {
            out << "    { id="  << l.id
                << ", from="    << l.from_pin.node_id
                << ", to="      << l.to_pin.node_id
                << " },\n";
        }
        out << "  },\n";
        out << "  editor_text = "
            << lua_long_string( impl->editor_buf ) << ",\n";
        auto const& c = impl->camera;
        out << "  camera = { pos = {" << c.pos[0] << "," << c.pos[1] << "," << c.pos[2]
            << "}, look = {" << c.look[0] << "," << c.look[1] << "," << c.look[2]
            << "}, up = {"   << c.up[0]   << "," << c.up[1]   << "," << c.up[2]
            << "}, fov = "   << c.fov_deg
            << ", near = "   << c.near_z
            << ", far = "    << c.far_z   << " },\n";
        out << "  assets = {\n";
        for( auto const& a : impl->assets )
            out << "    " << lua_long_string( a ) << ",\n";
        out << "  },\n";
        out << "}\n";
    }
}

bool aaa::ui::studio::Studio::save_project_to( std::string const& path )
{
    std::ofstream out( path );
    if( !out )
    {
        this->log( ConsoleEntry::ERR,
            "Save failed : cannot open " + path );
        return false;
    }
    serialize_into( out, impl_ );

    impl_->project_path  = path;
    impl_->project_dirty = false;
    impl_->project_name  =
        std::filesystem::path( path ).stem().string();
    std::error_code ec;
    std::filesystem::create_directories(
        std::filesystem::path( path ).parent_path() / "Assets", ec );
    this->log( ConsoleEntry::INFO,
        "Saved project : " + path );
    return true;
}

std::string aaa::ui::studio::Studio::serialize_to_string() const
{
    std::ostringstream ss;
    serialize_into( ss, impl_ );
    return ss.str();
}

bool aaa::ui::studio::Studio::open_project( std::string const& path )
{
    lua_State* L = luaL_newstate();
    if( L == nullptr ) return false;
    luaL_openlibs( L );

    if( luaL_dofile( L, path.c_str() ) != 0 || !lua_istable( L, -1 ) )
    {
        std::string err = lua_isstring( L, -1 )
            ? std::string( lua_tostring( L, -1 ) )
            : std::string( "(not a project table)" );
        this->log( ConsoleEntry::ERR, "Open failed : " + err );
        lua_close( L );
        return false;
    }

    // Wipe current state.
    impl_->nodes.clear();
    impl_->links.clear();
    impl_->assets.clear();
    impl_->next_node_id = 1;
    impl_->next_link_id = 1;

    // name
    lua_getfield( L, -1, "name" );
    if( lua_isstring( L, -1 ) ) impl_->project_name = lua_tostring( L, -1 );
    lua_pop( L, 1 );

    // editor_text
    lua_getfield( L, -1, "editor_text" );
    if( lua_isstring( L, -1 ) ) impl_->editor_buf = lua_tostring( L, -1 );
    lua_pop( L, 1 );

    // camera
    lua_getfield( L, -1, "camera" );
    if( lua_istable( L, -1 ) )
    {
        auto read_vec3 = []( lua_State* L_, char const* k, float out[3] )
        {
            lua_getfield( L_, -1, k );
            if( lua_istable( L_, -1 ) )
            {
                for( int i = 0; i < 3; ++i )
                {
                    lua_rawgeti( L_, -1, i + 1 );
                    if( lua_isnumber( L_, -1 ) )
                        out[ i ] = (float) lua_tonumber( L_, -1 );
                    lua_pop( L_, 1 );
                }
            }
            lua_pop( L_, 1 );
        };
        read_vec3( L, "pos",  impl_->camera.pos  );
        read_vec3( L, "look", impl_->camera.look );
        read_vec3( L, "up",   impl_->camera.up   );
        auto read_num = [ L ]( char const* k, float& out )
        {
            lua_getfield( L, -1, k );
            if( lua_isnumber( L, -1 ) ) out = (float) lua_tonumber( L, -1 );
            lua_pop( L, 1 );
        };
        read_num( "fov",  impl_->camera.fov_deg );
        read_num( "near", impl_->camera.near_z  );
        read_num( "far",  impl_->camera.far_z   );
    }
    lua_pop( L, 1 );

    // nodes
    lua_getfield( L, -1, "nodes" );
    if( lua_istable( L, -1 ) )
    {
        int n_count = (int) lua_objlen( L, -1 );
        for( int i = 1; i <= n_count; ++i )
        {
            lua_rawgeti( L, -1, i );
            if( lua_istable( L, -1 ) )
            {
                StudioNode n;
                lua_getfield( L, -1, "id" );
                if( lua_isnumber( L, -1 ) )
                    n.id = (std::uint32_t) lua_tointeger( L, -1 );
                lua_pop( L, 1 );
                lua_getfield( L, -1, "label" );
                if( lua_isstring( L, -1 ) ) n.label = lua_tostring( L, -1 );
                lua_pop( L, 1 );
                lua_getfield( L, -1, "x" );
                if( lua_isnumber( L, -1 ) ) n.pos_x = (float) lua_tonumber( L, -1 );
                lua_pop( L, 1 );
                lua_getfield( L, -1, "y" );
                if( lua_isnumber( L, -1 ) ) n.pos_y = (float) lua_tonumber( L, -1 );
                lua_pop( L, 1 );
                lua_getfield( L, -1, "shader" );
                if( lua_isstring( L, -1 ) ) n.shader_name = lua_tostring( L, -1 );
                lua_pop( L, 1 );
                lua_getfield( L, -1, "script" );
                if( lua_isstring( L, -1 ) ) n.script_path = lua_tostring( L, -1 );
                lua_pop( L, 1 );
                lua_getfield( L, -1, "enabled" );
                if( lua_isboolean( L, -1 ) ) n.enabled = lua_toboolean( L, -1 );
                lua_pop( L, 1 );
                // c152-K : uniforms = { ["key1"]=val1, ["key2"]=val2, ... }
                lua_getfield( L, -1, "uniforms" );
                if( lua_istable( L, -1 ) )
                {
                    lua_pushnil( L );
                    while( lua_next( L, -2 ) != 0 )
                    {
                        if( lua_isstring( L, -2 ) && lua_isnumber( L, -1 ) )
                        {
                            n.uniforms[ lua_tostring( L, -2 ) ] =
                                (float) lua_tonumber( L, -1 );
                        }
                        lua_pop( L, 1 );
                    }
                }
                lua_pop( L, 1 );
                if( n.id >= impl_->next_node_id ) impl_->next_node_id = n.id + 1;
                impl_->nodes.push_back( std::move( n ) );
            }
            lua_pop( L, 1 );
        }
    }
    lua_pop( L, 1 );

    // links
    lua_getfield( L, -1, "links" );
    if( lua_istable( L, -1 ) )
    {
        int l_count = (int) lua_objlen( L, -1 );
        for( int i = 1; i <= l_count; ++i )
        {
            lua_rawgeti( L, -1, i );
            if( lua_istable( L, -1 ) )
            {
                NodeLink lk;
                lua_getfield( L, -1, "id" );
                if( lua_isnumber( L, -1 ) )
                    lk.id = (std::uint32_t) lua_tointeger( L, -1 );
                lua_pop( L, 1 );
                lua_getfield( L, -1, "from" );
                if( lua_isnumber( L, -1 ) )
                    lk.from_pin.node_id = (std::uint32_t) lua_tointeger( L, -1 );
                lua_pop( L, 1 );
                lua_getfield( L, -1, "to" );
                if( lua_isnumber( L, -1 ) )
                    lk.to_pin.node_id = (std::uint32_t) lua_tointeger( L, -1 );
                lua_pop( L, 1 );
                lk.from_pin.is_output = true;
                lk.to_pin.is_output   = false;
                if( lk.id >= impl_->next_link_id ) impl_->next_link_id = lk.id + 1;
                impl_->links.push_back( lk );
            }
            lua_pop( L, 1 );
        }
    }
    lua_pop( L, 1 );

    // assets
    lua_getfield( L, -1, "assets" );
    if( lua_istable( L, -1 ) )
    {
        int a_count = (int) lua_objlen( L, -1 );
        for( int i = 1; i <= a_count; ++i )
        {
            lua_rawgeti( L, -1, i );
            if( lua_isstring( L, -1 ) )
                impl_->assets.emplace_back( lua_tostring( L, -1 ) );
            lua_pop( L, 1 );
        }
    }
    lua_pop( L, 1 );

    lua_pop( L, 1 );    // pop the returned table
    lua_close( L );

    impl_->project_path  = path;
    impl_->project_dirty = false;
    this->log( ConsoleEntry::INFO,
        "Opened project : " + path );
    return true;
}

bool aaa::ui::studio::Studio::load_from_string( std::string const& blob )
{
    // c152-O : write the blob to a unique temp file then route through
    // open_project. This keeps the Lua stack handling consistent with
    // file-based loads without duplicating ~180 lines of parsing.
    // The path/dirty fields are preserved across the round-trip so an
    // undo snapshot doesn't accidentally rename the project to a temp.
    std::string const saved_path  = impl_->project_path;
    std::string const saved_name  = impl_->project_name;
    bool        const saved_dirty = impl_->project_dirty;

    std::filesystem::path const tmp =
        std::filesystem::temp_directory_path() /
        ( "aaa_snap_" + std::to_string(
            std::chrono::steady_clock::now().time_since_epoch().count() )
          + ".lua" );
    {
        std::ofstream out( tmp );
        if( !out ) return false;
        out << blob;
    }
    bool const ok = open_project( tmp.string() );
    std::error_code ec;
    std::filesystem::remove( tmp, ec );
    // Restore identity (open_project replaced these with the temp path).
    impl_->project_path  = saved_path;
    impl_->project_name  = saved_name;
    impl_->project_dirty = saved_dirty;
    return ok;
}

std::string aaa::ui::studio::Studio::project_name() const
{
    return impl_->project_name;
}

std::string aaa::ui::studio::Studio::project_path() const
{
    return impl_->project_path;
}

bool aaa::ui::studio::Studio::project_dirty() const
{
    return impl_->project_dirty;
}

std::vector<std::string> const&
aaa::ui::studio::Studio::assets() const
{
    return impl_->assets;
}

std::string
aaa::ui::studio::Studio::add_asset_from_file( std::string const& src )
{
    if( impl_->project_path.empty() )
    {
        this->log( ConsoleEntry::WARN,
            "Save the project first ; assets live next to the .lua." );
        return std::string();
    }
    std::filesystem::path const proj_root =
        std::filesystem::path( impl_->project_path ).parent_path();
    std::filesystem::path const assets_root = proj_root / "Assets";
    std::error_code ec;
    std::filesystem::create_directories( assets_root, ec );

    std::filesystem::path const src_path( src );
    std::filesystem::path const dst = assets_root / src_path.filename();
    std::filesystem::copy_file( src_path, dst,
        std::filesystem::copy_options::overwrite_existing, ec );
    if( ec )
    {
        this->log( ConsoleEntry::ERR,
            std::string( "Asset copy failed : " ) + ec.message() );
        return std::string();
    }
    std::string const rel = "Assets/" + src_path.filename().string();
    impl_->assets.push_back( rel );
    impl_->project_dirty = true;
    this->log( ConsoleEntry::INFO, "Imported asset : " + rel );
    return rel;
}
