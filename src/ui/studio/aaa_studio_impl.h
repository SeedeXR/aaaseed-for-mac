// src/ui/studio/aaa_studio_impl.h
//
// c151-B / c152-C : private internal header for the Studio data model.
// After ImGui retirement (c152-C) this carries pure-C++ state only --
// no ImVec2, no platform handles. The Qt6 UI reads the public Studio
// API ; this header stays internal to the studio static lib.

#pragma once

#include "src/ui/studio/aaa_studio.h"

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

// 256-sample frame-time ring used by Studio::push_perf_sample.
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

// Node-graph layout constants used by add_node() auto-layout.
constexpr float kNodeW = 160.0f;
constexpr float kNodeH = 80.0f;

struct StudioImpl
{
    GOL::Backend*     backend  = nullptr;
    aaa::meu::Runner* runner   = nullptr;
    Studio*           owner    = nullptr;

    std::vector< StudioNode > nodes;
    std::vector< NodeLink >   links;
    std::uint32_t             next_node_id = 1;
    std::uint32_t             next_link_id = 1;
    std::uint32_t             auto_layout_x = 60;
    std::uint32_t             auto_layout_y = 60;

    std::string                                 editor_buf;
    std::function< void( std::string const& ) > on_run_cb;

    std::deque< ConsoleEntry > console_log;
    static constexpr std::size_t kMaxLogLines = 4096;

    CameraState camera;

    std::vector< SoundDeviceInfo > sound_devices;
    std::vector< std::string >     assets;

    PerfRing perf;

    float font_scale_value = 1.0f;

    std::string project_path;
    std::string project_name  = "Untitled";
    bool        project_dirty = false;

    // Helpers
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
