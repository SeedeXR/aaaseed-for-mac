// src/meu/aaa_meu_runner_mac.mm
//
// c143-A : Mac-native MEU runner shim. Implementation file. See header
// for design + hermetic doctrine notes.
//
// What this file owns :
//   - The lua_State* (one VM per Runner instance ; not shared).
//   - A shader-program cache : shader-stem -> GOL::ProgramId.
//   - Per-frame "current shader" + uniform buffer + texture bindings.
//   - The keyboard / mouse / scroll state surface queried by Lua.
//   - The catalog directory discovery (.app Resources/shaders/ or the
//     build-time AAA_SHADERS_MSL_DIR path passed by tests).
//
// What this file does NOT do :
//   - Begin or end MetalBackend render passes. The host (MTKView delegate
//     or test fixture) is responsible for the begin/end pair. The
//     runner's `aaa.draw_fullscreen_quad()` binding only emits encoder
//     commands against the active pass.
//   - Touch AppKit / Cocoa beyond NSBundle for resource lookup. The
//     runner is testable without an NSApplication ; NSBundle's
//     [NSBundle mainBundle] returns nil-ish but resourcePath callers
//     handle that gracefully.

#import "aaa_meu_runner_mac.h"

#import <Foundation/Foundation.h>

#include "src/gol/gol_backend.h"
//	c147-A : widget system header. Full type needed because the Lua
//	bindings dispatch into its public methods. Pulled in via .mm only --
//	the public Runner header still uses a forward-decl, matching the
//	pimpl pattern used throughout the runner.
#include "src/ui/widgets/aaa_widgets_mac.h"
//	c149-A v3 : file dialog adapter (Feature 2) + file watcher (Feature
//	3). Both full types live in their hermetic sub-libs ; the runner
//	dispatches into them from the aaa.io.* Lua bindings. The runner's
//	target_link_libraries gains both sub-libs (PUBLIC since the bindings
//	are exposed via the runner's surface).
#include "src/ui/macos/aaa_file_dialog.h"
#include "src/meu/aaa_file_watcher_mac.h"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <functional>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

extern "C" {
    #include "lua.h"
    #include "lauxlib.h"
    #include "lualib.h"
}

namespace aaa
{
namespace meu
{

namespace
{
    //	Engine-ABI uniform structs ; mirror tools/glsl_to_msl/glsl_to_msl.py
    //	output. Same shape as tests/regression/phase3_golden_frame_regression_test.cpp.
    struct AaaFuFloats { float        values[ 16 ];      };
    struct AaaFuVec4s  { float        values[ 16 ][ 4 ]; };
    struct AaaFuInts   { std::int32_t values[ 16 ];      };

    //	Load the entire contents of a path into a std::string. Returns
    //	empty on failure. Used both for the .lua script file AND for the
    //	on-demand .metal shader file slurping (catalog dispatch).
    std::string slurp_file( std::filesystem::path const& p )
    {
        std::ifstream in( p, std::ios::binary );
        if( !in ) return std::string();
        std::ostringstream ss;
        ss << in.rdbuf();
        return ss.str();
    }

    //	c157 : true iff the MSL source declares AaaFuVec4s at buffer(0)
    //	(the c39-era legacy ABI of ps_Maa_add_scale). The catalog norm is
    //	floats@0 / vec4s@1 / ints@2 ; the first buffer(0) declaration in
    //	the file decides. Window-scan backwards from "[[buffer(0)]]" for
    //	the struct name on the same declaration.
    bool msl_declares_vec4s_at_buffer0( std::string const& src )
    {
        std::size_t pos = src.find( "[[buffer(0)]]" );
        while( pos != std::string::npos )
        {
            std::size_t const from = pos > 80 ? pos - 80 : 0;
            std::string const win  = src.substr( from, pos - from );
            if( win.find( "AaaFuVec4s" )  != std::string::npos ) return true;
            if( win.find( "AaaFuFloats" ) != std::string::npos ) return false;
            if( win.find( "AaaFuInts" )   != std::string::npos ) return false;
            pos = src.find( "[[buffer(0)]]", pos + 1 );
        }
        return false;
    }

    //	Locate the bundle / build-time directory holding the .metal
    //	catalog. Preference order :
    //	  1. AAA_MEU_SHADERS_DIR env var (test override).
    //	  2. [NSBundle mainBundle] resourcePath + /shaders (when run from
    //	     inside AAASeed.app).
    //	  3. AAA_SHADERS_MSL_DIR compile define (test fallback when
    //	     building outside the .app bundle).
    //	  4. Empty path (caller treats as "no shaders" -- list_shaders
    //	     returns empty + use_shader fails gracefully).
    std::filesystem::path resolve_shaders_dir()
    {
        if( char const * env = std::getenv( "AAA_MEU_SHADERS_DIR" ) )
        {
            if( env[ 0 ] != '\0' ) return std::filesystem::path( env );
        }
        @autoreleasepool
        {
            NSBundle * bundle = [NSBundle mainBundle];
            NSString * rp = [bundle resourcePath];
            if( rp != nil )
            {
                std::filesystem::path candidate(
                    [rp UTF8String] );
                candidate /= "shaders";
                if( std::filesystem::is_directory( candidate ) )
                    return candidate;
            }
        }
#ifdef AAA_SHADERS_MSL_DIR
        {
            std::filesystem::path const candidate( AAA_SHADERS_MSL_DIR );
            if( std::filesystem::is_directory( candidate ) )
                return candidate;
        }
#endif
        return std::filesystem::path();
    }
}   //	anonymous

//	Real implementation -- pimpl pattern keeps the public header free of
//	Lua / GOL headers + Foundation imports.
class RunnerImpl
{
public:
    explicit RunnerImpl( GOL::Backend* backend )
        : _backend( backend )
        , _L( nullptr )
        , _shaders_dir( resolve_shaders_dir() )
        , _start_time( std::chrono::steady_clock::now() )
    {
        //	Populate uniform-buffer defaults to zero.
        std::memset( &_u_vec4s, 0, sizeof( _u_vec4s ) );
        std::memset( &_u_floats, 0, sizeof( _u_floats ) );
        std::memset( &_u_ints, 0, sizeof( _u_ints ) );
        //	Default scale to identity so a script that only sets offset
        //	still produces visible output.
        _u_vec4s.values[ 1 ][ 0 ] = 1.0f;
        _u_vec4s.values[ 1 ][ 1 ] = 1.0f;
        _u_vec4s.values[ 1 ][ 2 ] = 1.0f;
        _u_vec4s.values[ 1 ][ 3 ] = 1.0f;
        _u_floats.values[ 1 ] = 1.0f;
    }

    ~RunnerImpl()
    {
        unload();
    }

    bool load_script( std::string const& lua_path )
    {
        unload();
        if( !_backend ) return false;

        _L = luaL_newstate();
        if( _L == nullptr ) return false;
        luaL_openlibs( _L );

        install_aaa_bindings();

        // c151-B : fire the external extension hook so subsystems like
        // the ImGui Studio can register aaa.studio.* bindings on the
        // fresh state BEFORE the user script's top-level run.
        if( _lua_extension_hook )
            _lua_extension_hook( static_cast< void* >( _L ) );

        _script_path = lua_path;

        if( !run_loaded_script() )
        {
            //	Lua compile / runtime error -- leak the state so the
            //	error message stays on top of the stack for the test
            //	to lua_tostring(). On reload() we close + re-open.
            return false;
        }

        //	c155 : a script that never defines `aaa.on_frame` loads fine
        //	but renders NOTHING -- the classic "I ran my .lua and nothing
        //	happened" trap (e.g. pasting a pure library module that just
        //	`return`s a table). Surface it loudly instead of silently.
        if( !has_on_frame() )
        {
            NSLog( @"aaa::meu::Runner : WARNING -- '%s' loaded but defines "
                    "no aaa.on_frame(w, h, frame). Nothing will render. "
                    "See Resources/meu/Samples/perlin_noise/ for the "
                    "minimal pattern.",
                   lua_path.c_str() );
        }
        return true;
    }

    //	c155 : true iff the loaded state has `aaa.on_frame` as a function.
    bool has_on_frame() const
    {
        if( _L == nullptr ) return false;
        lua_getglobal( _L, "aaa" );
        if( !lua_istable( _L, -1 ) ) { lua_pop( _L, 1 ); return false; }
        lua_getfield( _L, -1, "on_frame" );
        bool const is_fn = lua_isfunction( _L, -1 );
        lua_pop( _L, 2 );
        return is_fn;
    }

    bool reload()
    {
        if( _L == nullptr || _script_path.empty() ) return false;
        //	Re-read the file from disk (catches external edits).
        return load_script( _script_path );
    }

    void unload()
    {
        if( _L != nullptr )
        {
            lua_close( _L );
            _L = nullptr;
        }
        //	Release every cached program + texture. The backend pointer
        //	may be null in a leaked-state scenario ; guard.
        if( _backend != nullptr )
        {
            for( auto const & kv : _program_cache )
            {
                if( kv.second != GOL::kInvalidProgramId )
                    _backend->delete_program( kv.second );
            }
            if( _uniform_vec4s_buf != GOL::kInvalidBufferId )
                _backend->delete_buffer( _uniform_vec4s_buf );
            if( _uniform_floats_buf != GOL::kInvalidBufferId )
                _backend->delete_buffer( _uniform_floats_buf );
            if( _uniform_ints_buf != GOL::kInvalidBufferId )
                _backend->delete_buffer( _uniform_ints_buf );
            if( _placeholder_tex != GOL::kInvalidTextureId )
                _backend->delete_texture( _placeholder_tex );
        }
        _program_cache.clear();
        _program_vec4_at0.clear();
        _uniform_vec4s_buf  = GOL::kInvalidBufferId;
        _uniform_floats_buf = GOL::kInvalidBufferId;
        _uniform_ints_buf   = GOL::kInvalidBufferId;
        _placeholder_tex    = GOL::kInvalidTextureId;
        _current_shader.clear();
        _current_program = GOL::kInvalidProgramId;
        _frame_index = 0;
        _pending_hud_text.clear();
    }

    void render_frame( std::uint32_t width, std::uint32_t height,
                       GOL::TextureId target_color_attachment )
    {
        _frame_w = width;
        _frame_h = height;
        _target_tex = target_color_attachment;

        //	c146 : clear the HUD text queue at the START of each frame so
        //	a Lua script that omits aaa.draw_hud_text() on a particular
        //	frame produces an empty overlay (not the previous frame's
        //	stale string). The host (AAASeedMTKView) reads this field
        //	via get_pending_hud_text() AFTER render_frame returns.
        _pending_hud_text.clear();

        if( _L == nullptr ) return;

        //	Push the script's `aaa.on_frame` if defined ; absent -> no-op.
        lua_getglobal( _L, "aaa" );
        if( !lua_istable( _L, -1 ) )
        {
            lua_pop( _L, 1 );
            ++_frame_index;
            return;
        }
        lua_getfield( _L, -1, "on_frame" );
        if( !lua_isfunction( _L, -1 ) )
        {
            lua_pop( _L, 2 );
            ++_frame_index;
            return;
        }

        lua_pushinteger( _L, (lua_Integer) width );
        lua_pushinteger( _L, (lua_Integer) height );
        lua_pushinteger( _L, (lua_Integer) _frame_index );

        int const status = lua_pcall( _L, 3, 0, 0 );
        if( status != 0 )
        {
            char const * msg = lua_tostring( _L, -1 );
            NSLog( @"aaa::meu::Runner : on_frame error : %s",
                   msg != nullptr ? msg : "(no message)" );
            lua_pop( _L, 1 );
        }
        //	Pop the `aaa` table that's still under the result.
        lua_pop( _L, 1 );

        ++_frame_index;
    }

    void on_key_event( int key_code, bool down )
    {
        if( down ) _keys_down.insert( key_code );
        else       _keys_down.erase(  key_code );
    }

    void on_mouse_event( double x, double y, int button, bool down )
    {
        _mouse_x = x;
        _mouse_y = y;
        if( button >= 0 )
        {
            if( down ) _buttons_down.insert( button );
            else       _buttons_down.erase(  button );
        }
    }

    void on_scroll_event( double dx, double dy )
    {
        _scroll_dx += dx;
        _scroll_dy += dy;
    }

    std::string current_shader_name() const { return _current_shader; }
    int         frame_index() const         { return _frame_index;    }
    std::string get_pending_hud_text() const { return _pending_hud_text; }

    //	c147-A : widget system pointer install/clear. Non-owning ; lifetime
    //	is the host's responsibility (AAASeedMTKView). nullptr is a valid
    //	value -- the aaa.ui.* bindings then become no-ops.
    void set_widget_system( aaa::ui::widgets::WidgetSystem* ws )
    {
        _widget_system = ws;
    }

    //	c151-B : external Lua-extension hook. Fires inside load_script
    //	after the runner's own bindings are installed and BEFORE the
    //	user script runs. The hook may register additional C functions
    //	on the fresh lua_State (e.g. aaa.studio.*) so the user script
    //	can call them straight away.
    void set_lua_extension_hook( std::function< void( void* ) > hook )
    {
        _lua_extension_hook = std::move( hook );
    }
    std::function< void( void* ) > _lua_extension_hook;

    std::vector< std::string > list_shaders() const
    {
        std::vector< std::string > out;
        if( _shaders_dir.empty() ) return out;
        std::error_code ec;
        if( !std::filesystem::is_directory( _shaders_dir, ec ) ) return out;
        for( auto const & entry : std::filesystem::directory_iterator( _shaders_dir, ec ) )
        {
            if( !entry.is_regular_file() ) continue;
            auto const & path = entry.path();
            if( path.extension() != ".metal" ) continue;
            out.push_back( path.stem().string() );
        }
        return out;
    }

    //	---- Lua-callable C thunks ---------------------------------------

    //	Helper : get the `this` pointer out of the Lua registry.
    static RunnerImpl* self_from_state( lua_State* L )
    {
        lua_pushlightuserdata( L, (void*) &kRegistryKey );
        lua_rawget( L, LUA_REGISTRYINDEX );
        RunnerImpl* p = reinterpret_cast< RunnerImpl* >(
            const_cast< void* >( lua_topointer( L, -1 ) ) );
        lua_pop( L, 1 );
        return p;
    }

    static int l_use_shader( lua_State* L )
    {
        RunnerImpl* self = self_from_state( L );
        if( self == nullptr ) { lua_pushboolean( L, 0 ); return 1; }
        if( !lua_isstring( L, 1 ) )
        {
            lua_pushboolean( L, 0 );
            return 1;
        }
        std::string const name = lua_tostring( L, 1 );
        bool const ok = self->ensure_program( name );
        lua_pushboolean( L, ok ? 1 : 0 );
        return 1;
    }

    static int l_set_uniform_float( lua_State* L )
    {
        RunnerImpl* self = self_from_state( L );
        if( self == nullptr ) return 0;
        int const idx = (int) lua_tointeger( L, 1 );
        double const val = lua_tonumber( L, 2 );
        if( idx >= 0 && idx < 16 )
            self->_u_floats.values[ idx ] = (float) val;
        return 0;
    }

    static int l_set_uniform_vec4( lua_State* L )
    {
        RunnerImpl* self = self_from_state( L );
        if( self == nullptr ) return 0;
        int const idx = (int) lua_tointeger( L, 1 );
        if( idx >= 0 && idx < 16 )
        {
            self->_u_vec4s.values[ idx ][ 0 ] = (float) lua_tonumber( L, 2 );
            self->_u_vec4s.values[ idx ][ 1 ] = (float) lua_tonumber( L, 3 );
            self->_u_vec4s.values[ idx ][ 2 ] = (float) lua_tonumber( L, 4 );
            self->_u_vec4s.values[ idx ][ 3 ] = (float) lua_tonumber( L, 5 );
        }
        return 0;
    }

    static int l_set_uniform_int( lua_State* L )
    {
        RunnerImpl* self = self_from_state( L );
        if( self == nullptr ) return 0;
        int const idx = (int) lua_tointeger( L, 1 );
        int const val = (int) lua_tointeger( L, 2 );
        if( idx >= 0 && idx < 16 )
            self->_u_ints.values[ idx ] = (std::int32_t) val;
        return 0;
    }

    static int l_set_bind_texture( lua_State* L )
    {
        RunnerImpl* self = self_from_state( L );
        if( self == nullptr ) return 0;
        int const slot = (int) lua_tointeger( L, 1 );
        //	Texture-name lookup is a forward extension hook ; today we
        //	always bind the runner's placeholder texture so the shader
        //	gets a valid sampler. Later sessions plug in a Resources/
        //	asset registry keyed by name (arg 2 string).
        (void) lua_tostring( L, 2 );
        if( slot >= 0 && slot < 8 )
        {
            self->_tex_bindings_active[ slot ] = true;
        }
        return 0;
    }

    static int l_draw_fullscreen_quad( lua_State* L )
    {
        RunnerImpl* self = self_from_state( L );
        if( self == nullptr ) return 0;
        self->draw_fullscreen_quad();
        return 0;
    }

    static int l_log( lua_State* L )
    {
        char const * msg = lua_tostring( L, 1 );
        if( msg != nullptr )
        {
            NSLog( @"aaa.log: %s", msg );
        }
        return 0;
    }

    //	c146 : aaa.draw_hud_text(text_string) -- queue a single HUD text
    //	overlay line for this frame. Stored on the RunnerImpl until the
    //	host reads it back via get_pending_hud_text(). Subsequent calls
    //	in the same frame OVERWRITE the previous text (a script that
    //	wants multi-line output concatenates with "\n" Lua-side -- the
    //	host renderer treats the full string as a single layout call).
    //	Non-string args are coerced via lua_tostring ; nil / unset args
    //	produce an empty queue (which renders as "no HUD this frame").
    static int l_draw_hud_text( lua_State* L )
    {
        RunnerImpl* self = self_from_state( L );
        if( self == nullptr ) return 0;
        char const * s = lua_tostring( L, 1 );
        if( s != nullptr )
        {
            self->_pending_hud_text = s;
        }
        else
        {
            self->_pending_hud_text.clear();
        }
        return 0;
    }

    static int l_frame_index( lua_State* L )
    {
        RunnerImpl* self = self_from_state( L );
        lua_pushinteger( L, self != nullptr ? (lua_Integer) self->_frame_index : 0 );
        return 1;
    }

    static int l_time( lua_State* L )
    {
        RunnerImpl* self = self_from_state( L );
        double t = 0.0;
        if( self != nullptr )
        {
            auto const now = std::chrono::steady_clock::now();
            std::chrono::duration< double > const dt = now - self->_start_time;
            t = dt.count();
        }
        lua_pushnumber( L, (lua_Number) t );
        return 1;
    }

    static int l_key_down( lua_State* L )
    {
        RunnerImpl* self = self_from_state( L );
        if( self == nullptr ) { lua_pushboolean( L, 0 ); return 1; }
        //	Two arg-shapes : integer keycode OR string keyname.
        bool down = false;
        if( lua_isnumber( L, 1 ) )
        {
            int const code = (int) lua_tointeger( L, 1 );
            down = self->_keys_down.count( code ) > 0;
        }
        else if( lua_isstring( L, 1 ) )
        {
            std::string const name = lua_tostring( L, 1 );
            //	Tiny well-known name->code table. Apple key codes are
            //	"hardware keyboard codes" from Carbon ; we only need a
            //	handful for the hello_world script.
            int code = -1;
            if( name == "space"  ) code = 49;
            if( name == "return" ) code = 36;
            if( name == "escape" ) code = 53;
            if( name == "left"   ) code = 123;
            if( name == "right"  ) code = 124;
            if( name == "down"   ) code = 125;
            if( name == "up"     ) code = 126;
            if( code >= 0 ) down = self->_keys_down.count( code ) > 0;
        }
        lua_pushboolean( L, down ? 1 : 0 );
        return 1;
    }

    static int l_mouse_xy( lua_State* L )
    {
        RunnerImpl* self = self_from_state( L );
        if( self == nullptr )
        {
            lua_pushnumber( L, 0.0 );
            lua_pushnumber( L, 0.0 );
            return 2;
        }
        lua_pushnumber( L, (lua_Number) self->_mouse_x );
        lua_pushnumber( L, (lua_Number) self->_mouse_y );
        return 2;
    }

    //	---- c147-A : aaa.ui.* widget bindings ---------------------------
    //	All five route into RunnerImpl::_widget_system. When the pointer
    //	is null (e.g. runner constructed outside a hosting MTKView, or
    //	host hasn't called set_widget_system yet), each binding becomes a
    //	pure no-op : slider returns its input value, button returns
    //	false, color_well returns its input rgba unchanged. This lets
    //	headless unit tests of the runner exercise the bindings without
    //	a backend.

    static int l_ui_begin_panel( lua_State* L )
    {
        RunnerImpl* self = self_from_state( L );
        std::string const title = lua_tostring( L, 1 )
                                  ? std::string( lua_tostring( L, 1 ) )
                                  : std::string();
        float const x = (float) lua_tonumber( L, 2 );
        float const y = (float) lua_tonumber( L, 3 );
        float const w = (float) lua_tonumber( L, 4 );
        float const h = (float) lua_tonumber( L, 5 );
        std::uint32_t handle_id = 0;
        if( self != nullptr && self->_widget_system != nullptr )
        {
            aaa::ui::widgets::PanelHandle const ph =
                self->_widget_system->begin_panel( title, x, y, w, h );
            handle_id = ph.id;
        }
        lua_pushinteger( L, (lua_Integer) handle_id );
        return 1;
    }

    static int l_ui_end_panel( lua_State* L )
    {
        RunnerImpl* self = self_from_state( L );
        if( self != nullptr && self->_widget_system != nullptr )
            self->_widget_system->end_panel();
        return 0;
    }

    static int l_ui_slider( lua_State* L )
    {
        RunnerImpl* self = self_from_state( L );
        std::string const label = lua_tostring( L, 1 )
                                  ? std::string( lua_tostring( L, 1 ) )
                                  : std::string();
        float const value = (float) lua_tonumber( L, 2 );
        float const lo    = (float) lua_tonumber( L, 3 );
        float const hi    = (float) lua_tonumber( L, 4 );
        float result = value;
        if( self != nullptr && self->_widget_system != nullptr )
        {
            result = self->_widget_system->slider( label, value, lo, hi );
        }
        //	c149-A : track label for save_preset.
        if( self != nullptr && !label.empty() )
            self->_ui_label_kinds[ label ] = LabelKind::Slider;
        lua_pushnumber( L, (lua_Number) result );
        return 1;
    }

    static int l_ui_button( lua_State* L )
    {
        RunnerImpl* self = self_from_state( L );
        std::string const label = lua_tostring( L, 1 )
                                  ? std::string( lua_tostring( L, 1 ) )
                                  : std::string();
        bool clicked = false;
        if( self != nullptr && self->_widget_system != nullptr )
        {
            clicked = self->_widget_system->button( label );
        }
        lua_pushboolean( L, clicked ? 1 : 0 );
        return 1;
    }

    static int l_ui_color_well( lua_State* L )
    {
        RunnerImpl* self = self_from_state( L );
        std::string const label = lua_tostring( L, 1 )
                                  ? std::string( lua_tostring( L, 1 ) )
                                  : std::string();
        aaa::ui::widgets::Color4f rgba;
        rgba.r = (float) lua_tonumber( L, 2 );
        rgba.g = (float) lua_tonumber( L, 3 );
        rgba.b = (float) lua_tonumber( L, 4 );
        rgba.a = (float) lua_tonumber( L, 5 );
        aaa::ui::widgets::Color4f out = rgba;
        if( self != nullptr && self->_widget_system != nullptr )
        {
            out = self->_widget_system->color_well( label, rgba );
        }
        if( self != nullptr && !label.empty() )
            self->_ui_label_kinds[ label ] = LabelKind::ColorWell;
        lua_pushnumber( L, (lua_Number) out.r );
        lua_pushnumber( L, (lua_Number) out.g );
        lua_pushnumber( L, (lua_Number) out.b );
        lua_pushnumber( L, (lua_Number) out.a );
        return 4;
    }

    //	---- c148-A v2 Phase 2-4 : new aaa.ui.* bindings ------------------

    //	aaa.ui.hsv_color_picker(label, r, g, b, a) -> r, g, b, a
    static int l_ui_hsv_color_picker( lua_State* L )
    {
        RunnerImpl* self = self_from_state( L );
        std::string const label = lua_tostring( L, 1 )
                                  ? std::string( lua_tostring( L, 1 ) )
                                  : std::string();
        aaa::ui::widgets::Color4f rgba;
        rgba.r = (float) lua_tonumber( L, 2 );
        rgba.g = (float) lua_tonumber( L, 3 );
        rgba.b = (float) lua_tonumber( L, 4 );
        rgba.a = (float) lua_tonumber( L, 5 );
        aaa::ui::widgets::Color4f out = rgba;
        if( self != nullptr && self->_widget_system != nullptr )
        {
            out = self->_widget_system->hsv_color_picker( label, rgba );
        }
        if( self != nullptr && !label.empty() )
            self->_ui_label_kinds[ label ] = LabelKind::HsvPicker;
        lua_pushnumber( L, (lua_Number) out.r );
        lua_pushnumber( L, (lua_Number) out.g );
        lua_pushnumber( L, (lua_Number) out.b );
        lua_pushnumber( L, (lua_Number) out.a );
        return 4;
    }

    //	aaa.ui.begin_modal(title, w, h) -> open:bool, ok:bool, cancel:bool
    static int l_ui_begin_modal( lua_State* L )
    {
        RunnerImpl* self = self_from_state( L );
        std::string const title = lua_tostring( L, 1 )
                                  ? std::string( lua_tostring( L, 1 ) )
                                  : std::string();
        float const w = (float) lua_tonumber( L, 2 );
        float const h = (float) lua_tonumber( L, 3 );
        aaa::ui::widgets::WidgetSystem::ModalResult mr;
        if( self != nullptr && self->_widget_system != nullptr )
        {
            mr = self->_widget_system->begin_modal( title, w, h );
        }
        lua_pushboolean( L, mr.open ? 1 : 0 );
        lua_pushboolean( L, mr.ok_clicked ? 1 : 0 );
        lua_pushboolean( L, mr.cancel_clicked ? 1 : 0 );
        return 3;
    }

    //	aaa.ui.end_modal()
    static int l_ui_end_modal( lua_State* L )
    {
        RunnerImpl* self = self_from_state( L );
        if( self != nullptr && self->_widget_system != nullptr )
            self->_widget_system->end_modal();
        return 0;
    }

    //	aaa.ui.show_modal(title)
    static int l_ui_show_modal( lua_State* L )
    {
        RunnerImpl* self = self_from_state( L );
        std::string const title = lua_tostring( L, 1 )
                                  ? std::string( lua_tostring( L, 1 ) )
                                  : std::string();
        if( self != nullptr && self->_widget_system != nullptr )
            self->_widget_system->show_modal( title );
        return 0;
    }

    //	aaa.ui.text_input(label, value, max_length?) -> new_value:string
    static int l_ui_text_input( lua_State* L )
    {
        RunnerImpl* self = self_from_state( L );
        std::string const label = lua_tostring( L, 1 )
                                  ? std::string( lua_tostring( L, 1 ) )
                                  : std::string();
        std::string const value = lua_tostring( L, 2 )
                                  ? std::string( lua_tostring( L, 2 ) )
                                  : std::string();
        //	max_length is optional ; default 64.
        std::size_t max_length = 64;
        if( lua_gettop( L ) >= 3 && lua_isnumber( L, 3 ) )
        {
            lua_Integer const ml = (lua_Integer) lua_tointeger( L, 3 );
            if( ml > 0 ) max_length = (std::size_t) ml;
        }
        std::string out = value;
        if( self != nullptr && self->_widget_system != nullptr )
        {
            out = self->_widget_system->text_input( label, value, max_length );
        }
        if( self != nullptr && !label.empty() )
            self->_ui_label_kinds[ label ] = LabelKind::TextInput;
        lua_pushlstring( L, out.data(), out.size() );
        return 1;
    }

    //	aaa.ui.hot_reload_button(label?) -> clicked:bool
    static int l_ui_hot_reload_button( lua_State* L )
    {
        RunnerImpl* self = self_from_state( L );
        std::string label = "Reload MEU";
        if( lua_gettop( L ) >= 1 && lua_isstring( L, 1 ) )
        {
            label = std::string( lua_tostring( L, 1 ) );
        }
        bool clicked = false;
        if( self != nullptr && self->_widget_system != nullptr )
        {
            clicked = self->_widget_system->hot_reload_button( label );
        }
        lua_pushboolean( L, clicked ? 1 : 0 );
        return 1;
    }

    //	aaa.ui.begin_collapsing_panel(title, x, y, w, h) -> expanded:bool
    static int l_ui_begin_collapsing_panel( lua_State* L )
    {
        RunnerImpl* self = self_from_state( L );
        std::string const title = lua_tostring( L, 1 )
                                  ? std::string( lua_tostring( L, 1 ) )
                                  : std::string();
        float const x = (float) lua_tonumber( L, 2 );
        float const y = (float) lua_tonumber( L, 3 );
        float const w = (float) lua_tonumber( L, 4 );
        float const h = (float) lua_tonumber( L, 5 );
        bool expanded = true;
        if( self != nullptr && self->_widget_system != nullptr )
        {
            expanded = self->_widget_system->begin_collapsing_panel(
                title, x, y, w, h );
        }
        if( self != nullptr && !title.empty() )
            self->_ui_label_kinds[ title ] = LabelKind::CollapsingPanel;
        lua_pushboolean( L, expanded ? 1 : 0 );
        return 1;
    }

    //	aaa.ui.end_collapsing_panel()
    static int l_ui_end_collapsing_panel( lua_State* L )
    {
        RunnerImpl* self = self_from_state( L );
        if( self != nullptr && self->_widget_system != nullptr )
            self->_widget_system->end_collapsing_panel();
        return 0;
    }

    //	---- c150 v4 : aaa.ui.text_area + aaa.ime.* bindings -------------
    //
    //	c150-A agent stalled before writing these bindings ; finished by
    //	the harness. Pattern matches the existing aaa.ui.text_input
    //	(c148-A) + aaa.ui.show_modal (c148-A) shapes.

    //	aaa.ui.text_area(label, value, visible_lines?, width_chars?,
    //	                 max_length?) -> new_value:string
    static int l_ui_text_area( lua_State* L )
    {
        RunnerImpl* self = self_from_state( L );
        char const* label_c = lua_tostring( L, 1 );
        char const* value_c = lua_tostring( L, 2 );
        std::string const label = label_c != nullptr ? label_c : "";
        std::string const value = value_c != nullptr ? value_c : "";
        int const    lines      = lua_isnumber( L, 3 )
                                    ? (int)lua_tointeger( L, 3 ) : 4;
        int const    width      = lua_isnumber( L, 4 )
                                    ? (int)lua_tointeger( L, 4 ) : 32;
        std::size_t const maxl  = lua_isnumber( L, 5 )
                                    ? (std::size_t)lua_tointeger( L, 5 )
                                    : (std::size_t)512;
        std::string out = value;
        if( self != nullptr && self->_widget_system != nullptr )
            out = self->_widget_system->text_area(
                label, value, lines, width, maxl );
        lua_pushlstring( L, out.data(), out.size() );
        return 1;
    }

    //	aaa.ime.set_marked_text(composing, sel_start, sel_len)
    //
    //	Synthetic injection path. The real NSTextInputClient protocol
    //	is wired separately in AAASeedInputView ; this binding lets
    //	tests + scripts drive composition without a real CJK keyboard.
    static int l_ime_set_marked_text( lua_State* L )
    {
        RunnerImpl* self = self_from_state( L );
        if( self == nullptr || self->_widget_system == nullptr ) return 0;
        char const* composing_c = lua_tostring( L, 1 );
        std::string const composing = composing_c != nullptr
                                        ? composing_c : "";
        int const sel_start = lua_isnumber( L, 2 )
                                ? (int)lua_tointeger( L, 2 ) : 0;
        int const sel_len   = lua_isnumber( L, 3 )
                                ? (int)lua_tointeger( L, 3 )
                                : (int)composing.size();
        self->_widget_system->on_marked_text(
            composing, sel_start, sel_len );
        return 0;
    }

    //	aaa.ime.commit_marked_text()
    static int l_ime_commit_marked_text( lua_State* L )
    {
        RunnerImpl* self = self_from_state( L );
        if( self == nullptr || self->_widget_system == nullptr ) return 0;
        std::string const composing =
            self->_widget_system->current_marked_text();
        self->_widget_system->on_text_committed( composing );
        return 0;
    }

    //	---- c149-A v3 : aaa.io.* bindings (drop_file / file dialogs /
    //	preset save+load) -------------------------------------------------

    //	aaa.io.drop_file(path) -> bool
    static int l_io_drop_file( lua_State* L )
    {
        RunnerImpl* self = self_from_state( L );
        if( self == nullptr ) { lua_pushboolean( L, 0 ); return 1; }
        char const* p = lua_tostring( L, 1 );
        if( p == nullptr ) { lua_pushboolean( L, 0 ); return 1; }
        bool const ok = self->drop_file( std::string( p ) );
        lua_pushboolean( L, ok ? 1 : 0 );
        return 1;
    }

    //	aaa.io.open_file_dialog(title, ext_array) -> path_or_nil
    static int l_io_open_file_dialog( lua_State* L )
    {
        char const* title_c = lua_tostring( L, 1 );
        std::string const title = title_c ? std::string( title_c ) : std::string();
        std::vector< aaa::ui::FileDialogFilter > filters;
        if( lua_istable( L, 2 ) )
        {
            aaa::ui::FileDialogFilter f;
            f.label = "Allowed files";
            lua_pushnil( L );
            while( lua_next( L, 2 ) != 0 )
            {
                if( lua_isstring( L, -1 ) )
                {
                    f.extensions.emplace_back( lua_tostring( L, -1 ) );
                }
                lua_pop( L, 1 );
            }
            if( !f.extensions.empty() ) filters.push_back( std::move( f ) );
        }
        std::optional< std::string > const r =
            aaa::ui::open_file_dialog( title, filters );
        if( r.has_value() )
            lua_pushlstring( L, r->data(), r->size() );
        else
            lua_pushnil( L );
        return 1;
    }

    //	aaa.io.save_file_dialog(title, default_filename, ext_array) -> path_or_nil
    static int l_io_save_file_dialog( lua_State* L )
    {
        char const* title_c = lua_tostring( L, 1 );
        std::string const title = title_c ? std::string( title_c ) : std::string();
        char const* def_c = lua_tostring( L, 2 );
        std::string const def = def_c ? std::string( def_c ) : std::string();
        std::vector< aaa::ui::FileDialogFilter > filters;
        if( lua_istable( L, 3 ) )
        {
            aaa::ui::FileDialogFilter f;
            f.label = "Allowed files";
            lua_pushnil( L );
            while( lua_next( L, 3 ) != 0 )
            {
                if( lua_isstring( L, -1 ) )
                {
                    f.extensions.emplace_back( lua_tostring( L, -1 ) );
                }
                lua_pop( L, 1 );
            }
            if( !f.extensions.empty() ) filters.push_back( std::move( f ) );
        }
        std::optional< std::string > const r =
            aaa::ui::save_file_dialog( title, def, filters );
        if( r.has_value() )
            lua_pushlstring( L, r->data(), r->size() );
        else
            lua_pushnil( L );
        return 1;
    }

    //	aaa.io.save_preset(path) -> bool
    static int l_io_save_preset( lua_State* L )
    {
        RunnerImpl* self = self_from_state( L );
        if( self == nullptr ) { lua_pushboolean( L, 0 ); return 1; }
        char const* p = lua_tostring( L, 1 );
        if( p == nullptr ) { lua_pushboolean( L, 0 ); return 1; }
        bool const ok = self->save_preset( std::string( p ) );
        lua_pushboolean( L, ok ? 1 : 0 );
        return 1;
    }

    //	aaa.io.load_preset(path) -> bool
    static int l_io_load_preset( lua_State* L )
    {
        RunnerImpl* self = self_from_state( L );
        if( self == nullptr ) { lua_pushboolean( L, 0 ); return 1; }
        char const* p = lua_tostring( L, 1 );
        if( p == nullptr ) { lua_pushboolean( L, 0 ); return 1; }
        bool const ok = self->load_preset( std::string( p ) );
        lua_pushboolean( L, ok ? 1 : 0 );
        return 1;
    }

    //	---- c149-A v3 Feature 1 : drop_file public dispatch --------------

    bool drop_file( std::string const& path )
    {
        if( path.empty() ) return false;
        //	Filter by .lua extension (case-insensitive). Other extensions
        //	are ignored quietly -- the user dropping a non-script is
        //	visually no-op'd by the host's drop-target highlight reverting.
        std::filesystem::path fp( path );
        std::string ext = fp.extension().string();
        std::transform( ext.begin(), ext.end(), ext.begin(),
                        []( unsigned char c ){ return (char) std::tolower( c ); } );
        if( ext != ".lua" ) return false;
        return load_script( path );
    }

    //	---- c149-A v3 Feature 4 : preset save / load ---------------------

    //	Helper : write a Lua-safe string literal. Escapes \, ", and any
    //	non-printable ASCII into \xHH so the read-back via luaL_dofile
    //	is round-trip-safe. Pure CPU ; no Foundation dependency.
    static std::string lua_string_literal( std::string const& s )
    {
        std::string out;
        out.reserve( s.size() + 2 );
        out.push_back( '"' );
        for( char c : s )
        {
            unsigned char uc = (unsigned char) c;
            if( c == '"' )       out += "\\\"";
            else if( c == '\\' ) out += "\\\\";
            else if( c == '\n' ) out += "\\n";
            else if( c == '\r' ) out += "\\r";
            else if( c == '\t' ) out += "\\t";
            else if( uc < 0x20 || uc == 0x7F )
            {
                char buf[ 8 ];
                std::snprintf( buf, sizeof( buf ), "\\x%02X", (int) uc );
                out += buf;
            }
            else
            {
                out.push_back( c );
            }
        }
        out.push_back( '"' );
        return out;
    }

    bool save_preset( std::string const& path ) const
    {
        if( _widget_system == nullptr ) return false;

        std::ofstream out( path );
        if( !out ) return false;
        out << "-- AAASeed preset (generated by aaa.io.save_preset)\n";
        out << "return {\n";

        out << "  slider_state = {\n";
        for( auto const& kv : _ui_label_kinds )
        {
            if( kv.second == LabelKind::Slider )
            {
                float const v = _widget_system->slider_value( kv.first );
                out << "    [" << lua_string_literal( kv.first ) << "] = "
                    << v << ",\n";
            }
        }
        out << "  },\n";

        out << "  hsv_picker_state = {\n";
        for( auto const& kv : _ui_label_kinds )
        {
            if( kv.second == LabelKind::HsvPicker )
            {
                aaa::ui::widgets::Color4f const c =
                    _widget_system->hsv_picker_value( kv.first );
                out << "    [" << lua_string_literal( kv.first ) << "] = "
                    << "{r=" << c.r << ",g=" << c.g << ",b=" << c.b
                    << ",a=" << c.a << "},\n";
            }
        }
        out << "  },\n";

        out << "  color_well_state = {\n";
        for( auto const& kv : _ui_label_kinds )
        {
            if( kv.second == LabelKind::ColorWell )
            {
                int const ix = _widget_system->color_well_index( kv.first );
                out << "    [" << lua_string_literal( kv.first ) << "] = "
                    << ix << ",\n";
            }
        }
        out << "  },\n";

        out << "  text_input_state = {\n";
        for( auto const& kv : _ui_label_kinds )
        {
            if( kv.second == LabelKind::TextInput )
            {
                std::string const v =
                    _widget_system->text_input_value( kv.first );
                out << "    [" << lua_string_literal( kv.first ) << "] = "
                    << lua_string_literal( v ) << ",\n";
            }
        }
        out << "  },\n";

        out << "  panel_expanded_state = {\n";
        for( auto const& kv : _ui_label_kinds )
        {
            if( kv.second == LabelKind::CollapsingPanel )
            {
                bool const b = _widget_system->panel_expanded( kv.first );
                out << "    [" << lua_string_literal( kv.first ) << "] = "
                    << ( b ? "true" : "false" ) << ",\n";
            }
        }
        out << "  },\n";

        out << "}\n";
        return (bool) out;
    }

    bool load_preset( std::string const& path )
    {
        if( _widget_system == nullptr ) return false;
        if( _L == nullptr ) return false;
        //	Sandbox the preset evaluation in a fresh stack frame.
        int const status = luaL_dofile( _L, path.c_str() );
        if( status != 0 )
        {
            char const* msg = lua_tostring( _L, -1 );
            NSLog( @"aaa::meu::Runner : load_preset error : %s",
                   msg != nullptr ? msg : "(no message)" );
            lua_pop( _L, 1 );
            return false;
        }
        if( !lua_istable( _L, -1 ) )
        {
            lua_pop( _L, 1 );
            return false;
        }
        //	Iterate each sub-table.
        auto read_float_table = [ & ]( char const* key,
            std::function< void ( std::string const&, float ) > setter )
        {
            lua_getfield( _L, -1, key );
            if( lua_istable( _L, -1 ) )
            {
                lua_pushnil( _L );
                while( lua_next( _L, -2 ) != 0 )
                {
                    //	stack : -2 key, -1 value
                    if( lua_isstring( _L, -2 ) && lua_isnumber( _L, -1 ) )
                    {
                        std::string k = lua_tostring( _L, -2 );
                        float v = (float) lua_tonumber( _L, -1 );
                        setter( k, v );
                    }
                    lua_pop( _L, 1 );   //  pop value, keep key for next iter
                }
            }
            lua_pop( _L, 1 );   //  pop sub-table or non-table value
        };

        read_float_table( "slider_state",
            [ & ]( std::string const& k, float v ) {
                _widget_system->set_slider_value( k, v );
                _ui_label_kinds[ k ] = LabelKind::Slider;
            } );

        //	hsv_picker_state : value is a table {r=,g=,b=,a=}
        lua_getfield( _L, -1, "hsv_picker_state" );
        if( lua_istable( _L, -1 ) )
        {
            lua_pushnil( _L );
            while( lua_next( _L, -2 ) != 0 )
            {
                if( lua_isstring( _L, -2 ) && lua_istable( _L, -1 ) )
                {
                    std::string k = lua_tostring( _L, -2 );
                    aaa::ui::widgets::Color4f c;
                    lua_getfield( _L, -1, "r" ); c.r = (float) lua_tonumber( _L, -1 ); lua_pop( _L, 1 );
                    lua_getfield( _L, -1, "g" ); c.g = (float) lua_tonumber( _L, -1 ); lua_pop( _L, 1 );
                    lua_getfield( _L, -1, "b" ); c.b = (float) lua_tonumber( _L, -1 ); lua_pop( _L, 1 );
                    lua_getfield( _L, -1, "a" ); c.a = (float) lua_tonumber( _L, -1 ); lua_pop( _L, 1 );
                    _widget_system->set_hsv_picker_value( k, c );
                    _ui_label_kinds[ k ] = LabelKind::HsvPicker;
                }
                lua_pop( _L, 1 );
            }
        }
        lua_pop( _L, 1 );

        //	color_well_state : value is an integer index
        lua_getfield( _L, -1, "color_well_state" );
        if( lua_istable( _L, -1 ) )
        {
            lua_pushnil( _L );
            while( lua_next( _L, -2 ) != 0 )
            {
                if( lua_isstring( _L, -2 ) && lua_isnumber( _L, -1 ) )
                {
                    std::string k = lua_tostring( _L, -2 );
                    int v = (int) lua_tointeger( _L, -1 );
                    _widget_system->set_color_well_index( k, v );
                    _ui_label_kinds[ k ] = LabelKind::ColorWell;
                }
                lua_pop( _L, 1 );
            }
        }
        lua_pop( _L, 1 );

        //	text_input_state : value is a string
        lua_getfield( _L, -1, "text_input_state" );
        if( lua_istable( _L, -1 ) )
        {
            lua_pushnil( _L );
            while( lua_next( _L, -2 ) != 0 )
            {
                if( lua_isstring( _L, -2 ) && lua_isstring( _L, -1 ) )
                {
                    std::string k = lua_tostring( _L, -2 );
                    std::string v = lua_tostring( _L, -1 );
                    _widget_system->set_text_input_value( k, v );
                    _ui_label_kinds[ k ] = LabelKind::TextInput;
                }
                lua_pop( _L, 1 );
            }
        }
        lua_pop( _L, 1 );

        //	panel_expanded_state : value is a bool
        lua_getfield( _L, -1, "panel_expanded_state" );
        if( lua_istable( _L, -1 ) )
        {
            lua_pushnil( _L );
            while( lua_next( _L, -2 ) != 0 )
            {
                if( lua_isstring( _L, -2 ) && lua_isboolean( _L, -1 ) )
                {
                    std::string k = lua_tostring( _L, -2 );
                    bool v = lua_toboolean( _L, -1 );
                    _widget_system->set_panel_expanded( k, v );
                    _ui_label_kinds[ k ] = LabelKind::CollapsingPanel;
                }
                lua_pop( _L, 1 );
            }
        }
        lua_pop( _L, 1 );

        lua_pop( _L, 1 );   //  pop the top-level table
        return true;
    }

    //	---- c149-A v3 Feature 3 : file-watcher dispatch ------------------

    bool enable_file_watch()
    {
        if( _script_path.empty() ) return false;
        if( _file_watcher.is_watching() ) return true;
        return _file_watcher.watch( _script_path,
            [ this ]{ this->reload(); } );
    }

    void disable_file_watch()
    {
        _file_watcher.unwatch();
    }

    bool is_file_watching() const { return _file_watcher.is_watching(); }

private:
    //	Registry-key sentinel : address of this static is unique per
    //	process and serves as a stable Lua registry key for storing the
    //	`this` pointer (see self_from_state).
    static char const kRegistryKey;

    void install_aaa_bindings()
    {
        if( _L == nullptr ) return;

        //	Stash `this` in the Lua registry so C thunks can recover it
        //	without needing closures (and without leaking ABI through
        //	luaL_register's `upvalue` mechanism, which uses index-1
        //	addressing that's awkward for tests).
        lua_pushlightuserdata( _L, (void*) &kRegistryKey );
        lua_pushlightuserdata( _L, (void*) this );
        lua_rawset( _L, LUA_REGISTRYINDEX );

        //	Ensure `aaa` is a table.
        lua_getglobal( _L, "aaa" );
        if( !lua_istable( _L, -1 ) )
        {
            lua_pop( _L, 1 );
            lua_newtable( _L );
            lua_setglobal( _L, "aaa" );
            lua_getglobal( _L, "aaa" );
        }

        //	Each binding : push the C function, set it as a field of the
        //	aaa table.
        auto bind = [ & ]( char const * name, lua_CFunction fn )
        {
            lua_pushcfunction( _L, fn );
            lua_setfield( _L, -2, name );
        };
        bind( "use_shader",            &RunnerImpl::l_use_shader            );
        bind( "set_uniform_float",     &RunnerImpl::l_set_uniform_float     );
        bind( "set_uniform_vec4",      &RunnerImpl::l_set_uniform_vec4      );
        bind( "set_uniform_int",       &RunnerImpl::l_set_uniform_int       );
        bind( "set_bind_texture",      &RunnerImpl::l_set_bind_texture      );
        bind( "draw_fullscreen_quad",  &RunnerImpl::l_draw_fullscreen_quad  );
        bind( "log",                   &RunnerImpl::l_log                   );
        bind( "draw_hud_text",         &RunnerImpl::l_draw_hud_text         );
        bind( "frame_index",           &RunnerImpl::l_frame_index           );
        bind( "time",                  &RunnerImpl::l_time                  );
        bind( "key_down",              &RunnerImpl::l_key_down              );
        bind( "mouse_xy",              &RunnerImpl::l_mouse_xy              );

        //	c147-A : aaa.ui.* sub-table -- widget UI bindings. The `aaa`
        //	table is currently on stack-top ; we push a new table, set
        //	the five widget bindings on it, then set it as the `ui` field
        //	of `aaa` (so Lua scripts call `aaa.ui.slider(...)` etc.).
        lua_newtable( _L );   // <-- aaa.ui table on stack-top
        auto bind_ui = [ & ]( char const * name, lua_CFunction fn )
        {
            lua_pushcfunction( _L, fn );
            lua_setfield( _L, -2, name );
        };
        bind_ui( "begin_panel", &RunnerImpl::l_ui_begin_panel );
        bind_ui( "end_panel",   &RunnerImpl::l_ui_end_panel   );
        bind_ui( "slider",      &RunnerImpl::l_ui_slider      );
        bind_ui( "button",      &RunnerImpl::l_ui_button      );
        bind_ui( "color_well",  &RunnerImpl::l_ui_color_well  );
        //	c148-A v2 Phase 2-4 : 8 additional widget bindings.
        bind_ui( "hsv_color_picker",      &RunnerImpl::l_ui_hsv_color_picker      );
        bind_ui( "begin_modal",           &RunnerImpl::l_ui_begin_modal           );
        bind_ui( "end_modal",             &RunnerImpl::l_ui_end_modal             );
        bind_ui( "show_modal",            &RunnerImpl::l_ui_show_modal            );
        bind_ui( "text_input",            &RunnerImpl::l_ui_text_input            );
        bind_ui( "hot_reload_button",     &RunnerImpl::l_ui_hot_reload_button     );
        bind_ui( "begin_collapsing_panel",&RunnerImpl::l_ui_begin_collapsing_panel);
        bind_ui( "end_collapsing_panel",  &RunnerImpl::l_ui_end_collapsing_panel  );
        //	c150 v4 : multi-line text_area widget.
        bind_ui( "text_area",             &RunnerImpl::l_ui_text_area             );
        //	Set the aaa.ui table as the `ui` field of `aaa`. After this
        //	`lua_setfield` the ui table is popped ; aaa stays on stack-top.
        lua_setfield( _L, -2, "ui" );

        //	c149-A v3 : aaa.io.* sub-table -- file IO + drag-drop +
        //	preset save/load bindings.
        lua_newtable( _L );   // <-- aaa.io table on stack-top
        auto bind_io = [ & ]( char const* name, lua_CFunction fn )
        {
            lua_pushcfunction( _L, fn );
            lua_setfield( _L, -2, name );
        };
        bind_io( "drop_file",         &RunnerImpl::l_io_drop_file         );
        bind_io( "open_file_dialog",  &RunnerImpl::l_io_open_file_dialog  );
        bind_io( "save_file_dialog",  &RunnerImpl::l_io_save_file_dialog  );
        bind_io( "save_preset",       &RunnerImpl::l_io_save_preset       );
        bind_io( "load_preset",       &RunnerImpl::l_io_load_preset       );
        lua_setfield( _L, -2, "io" );

        //	c150 v4 : aaa.ime.* sub-table — synthetic-injection path for
        //	IME composition + commit (NSTextInputClient protocol routes
        //	real CJK input through these too via AAASeedInputView).
        lua_newtable( _L );   // <-- aaa.ime table on stack-top
        auto bind_ime = [ & ]( char const* name, lua_CFunction fn )
        {
            lua_pushcfunction( _L, fn );
            lua_setfield( _L, -2, name );
        };
        bind_ime( "set_marked_text",    &RunnerImpl::l_ime_set_marked_text    );
        bind_ime( "commit_marked_text", &RunnerImpl::l_ime_commit_marked_text );
        lua_setfield( _L, -2, "ime" );

        //	Pop the aaa table.
        lua_pop( _L, 1 );
    }

    bool run_loaded_script()
    {
        if( _L == nullptr || _script_path.empty() ) return false;
        int const status = luaL_dofile( _L, _script_path.c_str() );
        if( status != 0 )
        {
            char const * msg = lua_tostring( _L, -1 );
            NSLog( @"aaa::meu::Runner : load_script error : %s",
                   msg != nullptr ? msg : "(no message)" );
            return false;
        }
        return true;
    }

    //	Find-or-compile a catalog shader by its bare stem (e.g.
    //	"ps_Maa_add_scale"). On success, _current_shader is updated and
    //	_current_program is bound for the next draw_fullscreen_quad call.
    bool ensure_program( std::string const & name )
    {
        if( _backend == nullptr ) return false;

        auto it = _program_cache.find( name );
        if( it != _program_cache.end() )
        {
            _current_shader  = name;
            _current_program = it->second;
            return _current_program != GOL::kInvalidProgramId;
        }

        std::filesystem::path msl_path = _shaders_dir / ( name + ".metal" );
        std::string const src = slurp_file( msl_path );
        if( src.empty() )
        {
            _program_cache[ name ] = GOL::kInvalidProgramId;
            return false;
        }

        //	c157 : per-shader uniform-ABI detection. The catalog's dominant
        //	ABI is floats@buffer(0) / vec4s@buffer(1) / ints@buffer(2)
        //	(~168 shaders) ; the c39-era ps_Maa_add_scale alone declares
        //	vec4s@buffer(0). The runner historically bound ONLY vec4s to
        //	slot 0, which silently no-op'ed every set_uniform_float /
        //	set_uniform_int and fed vec4 bytes into the trio shaders'
        //	floats[]. We detect the legacy shape from the source we are
        //	already holding and bind accordingly at draw time -- legacy
        //	shaders keep their exact historical bytes, trio shaders get
        //	all three uniform arrays for the first time.
        _program_vec4_at0[ name ] = msl_declares_vec4s_at_buffer0( src );

        GOL::ProgramId prog = _backend->create_program_msl(
            src.c_str(), "vs_main", "fs_main", GOL::TextureFormat::BGRA8 );
        _program_cache[ name ] = prog;
        if( prog == GOL::kInvalidProgramId )
        {
            NSLog( @"aaa::meu::Runner : compile failed for '%s' : %s",
                   name.c_str(), _backend->get_last_error().c_str() );
            return false;
        }
        _current_shader  = name;
        _current_program = prog;
        return true;
    }

    void ensure_uniform_buffers()
    {
        if( _backend == nullptr ) return;
        if( _uniform_vec4s_buf == GOL::kInvalidBufferId )
            _uniform_vec4s_buf  = _backend->gen_buffer();
        if( _uniform_floats_buf == GOL::kInvalidBufferId )
            _uniform_floats_buf = _backend->gen_buffer();
        if( _uniform_ints_buf == GOL::kInvalidBufferId )
            _uniform_ints_buf   = _backend->gen_buffer();
    }

    void ensure_placeholder_texture()
    {
        if( _backend == nullptr ) return;
        if( _placeholder_tex != GOL::kInvalidTextureId ) return;
        constexpr std::uint32_t W = 8, H = 8;
        _placeholder_tex = _backend->gen_texture_2d( W, H, GOL::TextureFormat::RGBA8 );
        if( _placeholder_tex == GOL::kInvalidTextureId ) return;
        std::vector< std::uint8_t > pixels( W * H * 4, 0 );
        for( std::uint32_t y = 0; y < H; ++y )
        {
            for( std::uint32_t x = 0; x < W; ++x )
            {
                std::size_t i = ( y * W + x ) * 4;
                pixels[ i + 0 ] = (std::uint8_t)( ( x * 255 ) / ( W - 1 ) );
                pixels[ i + 1 ] = (std::uint8_t)( ( y * 255 ) / ( H - 1 ) );
                pixels[ i + 2 ] = 128;
                pixels[ i + 3 ] = 255;
            }
        }
        _backend->texture_data_2d( _placeholder_tex, pixels.data(), W * 4 );
    }

    void draw_fullscreen_quad()
    {
        if( _backend == nullptr ) return;
        if( _current_program == GOL::kInvalidProgramId ) return;

        ensure_uniform_buffers();
        ensure_placeholder_texture();

        //	Upload current uniform snapshot. Vec4s is the most common
        //	layout in the catalog ; floats + ints uploaded too in case
        //	the active shader needs them (Metal will read from whatever
        //	buffer slot the shader's declaration binds).
        _backend->buffer_data( _uniform_vec4s_buf,  sizeof( _u_vec4s ),
                               &_u_vec4s,  GOL::BufferUsage::Dynamic );
        _backend->buffer_data( _uniform_floats_buf, sizeof( _u_floats ),
                               &_u_floats, GOL::BufferUsage::Dynamic );
        _backend->buffer_data( _uniform_ints_buf,   sizeof( _u_ints ),
                               &_u_ints,   GOL::BufferUsage::Dynamic );

        _backend->bind_program( _current_program );

        //	Bind placeholder texture into all 4 catalog-expected slots
        //	so Metal validation is satisfied (the catalog declares 4
        //	aaa_tex2d slots ; only slot 0 is typically sampled).
        for( std::uint32_t slot = 0; slot < 4; ++slot )
        {
            _backend->bind_fragment_texture( _placeholder_tex, slot );
        }

        //	c157 : bind per the shader's detected uniform ABI (see
        //	use_shader). Trio ABI (the catalog norm) : floats@0, vec4s@1,
        //	ints@2 -- makes aaa.set_uniform_float / _int functional.
        //	Legacy vec4@0 ABI (ps_Maa_add_scale) : vec4s@0, byte-identical
        //	to the historical binding so its established look is preserved.
        auto const abi_it = _program_vec4_at0.find( _current_shader );
        bool const legacy_vec4_at0 =
            ( abi_it != _program_vec4_at0.end() ) && abi_it->second;
        if( legacy_vec4_at0 )
        {
            _backend->bind_fragment_buffer( _uniform_vec4s_buf, 0, 0 );
        }
        else
        {
            _backend->bind_fragment_buffer( _uniform_floats_buf, 0, 0 );
            _backend->bind_fragment_buffer( _uniform_vec4s_buf,  1, 0 );
            _backend->bind_fragment_buffer( _uniform_ints_buf,   2, 0 );
        }

        _backend->draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
    }

private:
    GOL::Backend*     _backend;             // non-owning
    lua_State*        _L;
    std::string       _script_path;
    std::filesystem::path _shaders_dir;

    //	Program cache : shader-stem -> compiled ProgramId. Entries with
    //	kInvalidProgramId are negative cache (compile failed).
    std::unordered_map< std::string, GOL::ProgramId > _program_cache;

    //	c157 : per-shader uniform-ABI flag (true = legacy vec4s@buffer(0),
    //	false = trio floats@0/vec4s@1/ints@2). Filled at compile time from
    //	the MSL source ; consumed by draw_fullscreen_quad's bind block.
    std::unordered_map< std::string, bool > _program_vec4_at0;

    //	Currently-bound shader for the next draw call.
    std::string  _current_shader;
    GOL::ProgramId _current_program = GOL::kInvalidProgramId;

    //	Uniform shadow buffers + GPU mirror.
    AaaFuVec4s  _u_vec4s;
    AaaFuFloats _u_floats;
    AaaFuInts   _u_ints;
    GOL::BufferId _uniform_vec4s_buf  = GOL::kInvalidBufferId;
    GOL::BufferId _uniform_floats_buf = GOL::kInvalidBufferId;
    GOL::BufferId _uniform_ints_buf   = GOL::kInvalidBufferId;

    //	Per-frame state.
    std::uint32_t   _frame_w     = 0;
    std::uint32_t   _frame_h     = 0;
    GOL::TextureId  _target_tex  = GOL::kInvalidTextureId;
    int             _frame_index = 0;

    //	c146 : HUD text overlay queue. Set by aaa.draw_hud_text Lua
    //	binding ; cleared at the top of every render_frame so a script
    //	that skips the call on a given frame produces an empty overlay.
    //	Read by the host via get_pending_hud_text() AFTER render_frame.
    std::string _pending_hud_text;

    //	Texture-binding shadow : tracks which slots Lua has asked to bind
    //	(used by future asset registry ; today, all bind to placeholder).
    bool _tex_bindings_active[ 8 ] = { false, false, false, false,
                                       false, false, false, false };
    GOL::TextureId _placeholder_tex = GOL::kInvalidTextureId;

    //	Input state.
    //  c142-B finish-up : these were declared unordered_map but used like
    //  set (insert/erase/count on a key only). Switching to unordered_set
    //  matches the call sites in on_key_event / on_mouse_event / l_key_down.
    std::unordered_set< int > _keys_down;
    std::unordered_set< int > _buttons_down;
    double _mouse_x   = 0.0;
    double _mouse_y   = 0.0;
    double _scroll_dx = 0.0;
    double _scroll_dy = 0.0;

    //	Time base for aaa.time() : wall-clock from Runner construction.
    std::chrono::steady_clock::time_point _start_time;

    //	c147-A : non-owning pointer to the host's widget system. The
    //	aaa.ui.* Lua bindings route into this. Null until the host calls
    //	Runner::set_widget_system() ; in that null state the bindings
    //	short-circuit to no-op-with-default-return (slider returns its
    //	input, button returns false, color_well returns its input rgba).
    aaa::ui::widgets::WidgetSystem* _widget_system = nullptr;

public:
    //	c149-A v3 Feature 4 : label tracking for preset serialization.
    //	Each aaa.ui.* binding records the label it was called with so
    //	save_preset can walk all known labels (in the absence of an
    //	enumerate-everything API on WidgetSystem). LabelKind matches the
    //	5 categories save_preset emits. Latest call wins ; a script that
    //	uses the same label for two different widget kinds in different
    //	frames produces a "last-kind" entry.
    enum class LabelKind
    {
        Slider,
        HsvPicker,
        ColorWell,
        TextInput,
        CollapsingPanel,
    };
    std::unordered_map< std::string, LabelKind > _ui_label_kinds;

    //	c149-A v3 Feature 3 : file watcher. Owned ; lifetime tied to the
    //	RunnerImpl. Disabled by default ; enable_file_watch() opts in.
    aaa::meu::FileWatcher _file_watcher;
};

char const RunnerImpl::kRegistryKey = 0;

//	---- Public Runner facade ---------------------------------------------

Runner::Runner( GOL::Backend* backend )
    : _impl( std::make_unique< RunnerImpl >( backend ) )
{}

Runner::~Runner() = default;

bool Runner::load_script( std::string const& lua_path )
{
    return _impl ? _impl->load_script( lua_path ) : false;
}

bool Runner::reload()
{
    return _impl ? _impl->reload() : false;
}

void Runner::unload()
{
    if( _impl ) _impl->unload();
}

void Runner::render_frame( std::uint32_t width, std::uint32_t height,
                           GOL::TextureId target_color_attachment )
{
    if( _impl ) _impl->render_frame( width, height, target_color_attachment );
}

void Runner::on_key_event( int key_code, bool down )
{
    if( _impl ) _impl->on_key_event( key_code, down );
}

void Runner::on_mouse_event( double x, double y, int button, bool down )
{
    if( _impl ) _impl->on_mouse_event( x, y, button, down );
}

void Runner::on_scroll_event( double dx, double dy )
{
    if( _impl ) _impl->on_scroll_event( dx, dy );
}

std::string Runner::current_shader_name() const
{
    return _impl ? _impl->current_shader_name() : std::string();
}

int Runner::frame_index() const
{
    return _impl ? _impl->frame_index() : 0;
}

bool Runner::has_on_frame() const
{
    return _impl ? _impl->has_on_frame() : false;
}

std::vector< std::string > Runner::list_shaders() const
{
    return _impl ? _impl->list_shaders() : std::vector< std::string >();
}

std::string Runner::get_pending_hud_text() const
{
    return _impl ? _impl->get_pending_hud_text() : std::string();
}

void Runner::set_widget_system( aaa::ui::widgets::WidgetSystem* ws )
{
    if( _impl ) _impl->set_widget_system( ws );
}

void Runner::set_lua_extension_hook( std::function< void( void* ) > hook )
{
    if( _impl ) _impl->set_lua_extension_hook( std::move( hook ) );
}

bool Runner::save_preset( std::string const& path ) const
{
    return _impl ? _impl->save_preset( path ) : false;
}

bool Runner::load_preset( std::string const& path )
{
    return _impl ? _impl->load_preset( path ) : false;
}

bool Runner::drop_file( std::string const& path )
{
    return _impl ? _impl->drop_file( path ) : false;
}

bool Runner::enable_file_watch()
{
    return _impl ? _impl->enable_file_watch() : false;
}

void Runner::disable_file_watch()
{
    if( _impl ) _impl->disable_file_watch();
}

bool Runner::is_file_watching() const
{
    return _impl ? _impl->is_file_watching() : false;
}

}   //	namespace meu
}   //	namespace aaa
