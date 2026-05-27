// src/gol/metal/metal_backend.h
//
// `GOL::MetalBackend` — concrete backend implementation using Apple's
// metal-cpp headers. Pure C++ ; no Objective-C++. NSWindow / MTKView host
// integration will land in `src/ui/macos/` as `.mm` files when Phase 4
// starts ; the backend itself stays platform-neutral C++.
//
// Backend lifetime contract :
//   1. Construct (cheap ; doesn't touch Metal).
//   2. Call init() — acquires MTL::Device + MTL::CommandQueue. Returns false
//      if no Metal-capable GPU is found.
//   3. Use the GOL::Backend interface methods.
//   4. Call deinit() (or destroy) — releases all owned MTL objects.

#pragma once

#include "src/gol/gol_backend.h"

#include <unordered_map>

//	Forward-declare metal-cpp types so this header does NOT pull in
//	Metal.hpp. Saves compile time for downstream consumers. The .cpp uses
//	the real types via the implementation-private include.
namespace MTL { class Device; class CommandQueue; class CommandBuffer; class RenderCommandEncoder;
                class Buffer; class Texture; class RenderPipelineState; class Function;
                class SamplerState; class DepthStencilState; }
namespace NS { class String; }
namespace CA { class MetalDrawable; }

namespace GOL
{
    class MetalBackend final : public Backend
    {
    public:
        MetalBackend();
        ~MetalBackend() override;

        //	Backend interface
        bool        init() override;
        void        deinit() override;
        DeviceInfo  get_device_info() const override;

        BufferId    gen_buffer() override;
        void        delete_buffer( BufferId id ) override;
        void        buffer_data(   BufferId id, std::size_t size, void const* data,
                                   BufferUsage usage ) override;

        TextureId   gen_texture_2d( std::uint32_t w, std::uint32_t h,
                                    TextureFormat fmt ) override;
        void        delete_texture( TextureId id ) override;
        void        texture_data_2d( TextureId id, void const* data,
                                     std::size_t bytes_per_row ) override;
        TextureId   gen_texture_3d( std::uint32_t w, std::uint32_t h, std::uint32_t d,
                                    TextureFormat fmt, void const* data ) override;
        void        bind_fragment_texture_3d( TextureId id, std::uint32_t slot ) override;

        ProgramId   create_program_msl( char const* msl_source,
                                        char const* vertex_entry,
                                        char const* fragment_entry,
                                        TextureFormat target_color_fmt = TextureFormat::RGBA8 ) override;
        ProgramId   create_program( ProgramDesc const& desc ) override;
        void        delete_program( ProgramId id ) override;
        std::string const& get_last_error() const override { return _last_error; }

        bool        begin_render_pass( RenderPassDescriptor const& desc ) override;
        void        end_render_pass() override;
        void        set_viewport( float x, float y, float w, float h ) override;
        void        bind_program( ProgramId id ) override;
        void        bind_vertex_buffer( BufferId id, std::uint32_t slot, std::uint32_t offset ) override;
        void        bind_fragment_buffer( BufferId id, std::uint32_t slot, std::uint32_t offset ) override;
        void        bind_fragment_texture( TextureId id, std::uint32_t slot ) override;
        void        draw_arrays( PrimitiveType prim, std::uint32_t first, std::uint32_t count ) override;
        void        draw_elements( PrimitiveType prim, std::uint32_t count, IndexType index_type,
                                   BufferId index_buffer, std::uint32_t index_offset ) override;
        void        read_texture_pixels( TextureId id,
                                         std::uint32_t x, std::uint32_t y,
                                         std::uint32_t w, std::uint32_t h,
                                         void* out_buffer,
                                         std::size_t bytes_per_row ) override;
        void        present() override;

        char const* backend_name() const override { return "Metal"; }

        //	Accessors used by tests and (later) by the ui/macos/ MTKView host
        //	to wire a CAMetalLayer to this device. Not part of the abstract
        //	Backend interface — backend-specific.
        MTL::Device*       get_device()        const { return _device; }
        MTL::CommandQueue* get_command_queue() const { return _queue;  }

        //	Window-target render-pass entry. The host (src/ui/macos/) calls
        //	this with the drawable acquired from MTKView's currentDrawable.
        //	Color attachment is the drawable's texture ; the
        //	RenderPassDescriptor's `color_attachment` field is ignored.
        //	On success, drives the same encoder lifecycle as
        //	begin_render_pass(). present_window() must be called at frame
        //	end (instead of present()).
        bool begin_window_render_pass( CA::MetalDrawable* drawable,
                                       RenderPassDescriptor const& desc );

        //	Counterpart to present() for window targets : ends the active
        //	encoder, schedules drawable presentation, commits the command
        //	buffer asynchronously (no waitUntilCompleted — frame pacing
        //	is the OS's job for window targets).
        void present_window( CA::MetalDrawable* drawable );

    private:
        MTL::Device*       _device = nullptr;
        MTL::CommandQueue* _queue  = nullptr;

        BufferId  _next_buffer_id  = 1;
        TextureId _next_texture_id = 1;
        ProgramId _next_program_id = 1;

        struct Program
        {
            MTL::Function*            vs  = nullptr;
            MTL::Function*            fs  = nullptr;
            MTL::RenderPipelineState* pso = nullptr;
            //	Per-program depth state. Null when the program disables
            //	depth entirely (depth_test_enabled == false AND
            //	depth_write_enabled == false). bind_program installs it
            //	on the encoder ; null leaves the encoder's depth state
            //	unchanged, which in Metal means "no depth test".
            MTL::DepthStencilState*   ds  = nullptr;
        };

        //	Owned MTL objects. The unordered_maps make resource lookup O(1)
        //	at the cost of one bucket / pointer per resource. Acceptable for
        //	a backend that holds ~hundreds of long-lived buffers and ~tens of
        //	textures.
        std::unordered_map< BufferId,  MTL::Buffer*  > _buffers;
        std::unordered_map< TextureId, MTL::Texture* > _textures;
        std::unordered_map< ProgramId, Program       > _programs;

        //	Active render pass state — non-owning pointers into per-frame
        //	objects allocated inside begin_render_pass and torn down in
        //	end_render_pass / present.
        MTL::CommandBuffer*        _current_cb      = nullptr;
        MTL::RenderCommandEncoder* _current_encoder = nullptr;

        //	Tracks whether the current render pass pushed a GPU debug group
        //	from RenderPassDescriptor::debug_label. end_render_pass /
        //	present_window emit the matching popDebugGroup only if true.
        bool                       _debug_label_was_pushed = false;

        //	Default sampler bound at fragment slot 0 every frame the
        //	user binds a fragment texture. Linear filter, repeat wrap.
        //	Allocated once in init(), released in deinit().
        MTL::SamplerState*         _default_sampler = nullptr;

        //	Last shader-compile or pipeline-build error. Cleared on each
        //	new create_program_msl call.
        std::string _last_error;

        //	Non-copyable, non-movable : owns Metal handles that must not be
        //	double-released.
        MetalBackend( MetalBackend const& )            = delete;
        MetalBackend& operator=( MetalBackend const& ) = delete;
        MetalBackend( MetalBackend&& )                 = delete;
        MetalBackend& operator=( MetalBackend&& )      = delete;
    };
}
