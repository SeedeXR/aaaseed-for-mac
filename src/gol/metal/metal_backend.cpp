// src/gol/metal/metal_backend.cpp
//
// `GOL::MetalBackend` implementation. This is the **single TU** in the Mac
// build that defines NS_PRIVATE_IMPLEMENTATION / MTL_PRIVATE_IMPLEMENTATION
// / CA_PRIVATE_IMPLEMENTATION — Apple's metal-cpp emits its selector and
// class symbols here. Other TUs that include Metal.hpp do NOT define these
// macros ; they get just the header declarations and link against the
// symbols emitted here.

#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION

#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>

#include "src/gol/metal/metal_backend.h"
#include "src/gol/gol_backend.h"

#include <cstring>      // std::memcpy

namespace
{
    MTL::PixelFormat to_mtl_pixel_format( GOL::TextureFormat fmt )
    {
        switch( fmt )
        {
        case GOL::TextureFormat::RGBA8:    return MTL::PixelFormatRGBA8Unorm;
        case GOL::TextureFormat::BGRA8:    return MTL::PixelFormatBGRA8Unorm;
        case GOL::TextureFormat::R8:       return MTL::PixelFormatR8Unorm;
        case GOL::TextureFormat::RG8:      return MTL::PixelFormatRG8Unorm;
        case GOL::TextureFormat::RGBA16F:  return MTL::PixelFormatRGBA16Float;
        case GOL::TextureFormat::R32F:     return MTL::PixelFormatR32Float;
        case GOL::TextureFormat::Depth32F: return MTL::PixelFormatDepth32Float;
        }
        return MTL::PixelFormatInvalid;
    }

    MTL::ResourceOptions to_mtl_resource_options( GOL::BufferUsage usage )
    {
        //	On Apple Silicon (unified memory), Static/Dynamic both make
        //	sense in shared storage. Stream uses .shared as well so the CPU
        //	can map and write each frame. .private would be faster for
        //	GPU-only resources but requires an explicit blit on upload — we
        //	defer that optimisation until profiling shows it matters.
        switch( usage )
        {
        case GOL::BufferUsage::Static:
        case GOL::BufferUsage::Dynamic:
        case GOL::BufferUsage::Stream:
            return MTL::ResourceStorageModeShared;
        }
        return MTL::ResourceStorageModeShared;
    }

    MTL::PrimitiveType to_mtl_primitive( GOL::PrimitiveType prim )
    {
        switch( prim )
        {
        case GOL::PrimitiveType::Points:        return MTL::PrimitiveTypePoint;
        case GOL::PrimitiveType::Lines:         return MTL::PrimitiveTypeLine;
        case GOL::PrimitiveType::LineStrip:     return MTL::PrimitiveTypeLineStrip;
        case GOL::PrimitiveType::Triangles:     return MTL::PrimitiveTypeTriangle;
        case GOL::PrimitiveType::TriangleStrip: return MTL::PrimitiveTypeTriangleStrip;
        }
        return MTL::PrimitiveTypeTriangle;
    }

    MTL::LoadAction to_mtl_load_action( GOL::LoadAction action )
    {
        switch( action )
        {
        case GOL::LoadAction::DontCare: return MTL::LoadActionDontCare;
        case GOL::LoadAction::Load:     return MTL::LoadActionLoad;
        case GOL::LoadAction::Clear:    return MTL::LoadActionClear;
        }
        return MTL::LoadActionClear;
    }

    MTL::VertexFormat to_mtl_vertex_format( GOL::VertexFormat fmt )
    {
        switch( fmt )
        {
        case GOL::VertexFormat::Float:      return MTL::VertexFormatFloat;
        case GOL::VertexFormat::Float2:     return MTL::VertexFormatFloat2;
        case GOL::VertexFormat::Float3:     return MTL::VertexFormatFloat3;
        case GOL::VertexFormat::Float4:     return MTL::VertexFormatFloat4;
        case GOL::VertexFormat::UChar4Norm: return MTL::VertexFormatUChar4Normalized;
        }
        return MTL::VertexFormatInvalid;
    }

    MTL::CompareFunction to_mtl_compare( GOL::DepthCompare c )
    {
        switch( c )
        {
        case GOL::DepthCompare::Never:        return MTL::CompareFunctionNever;
        case GOL::DepthCompare::Less:         return MTL::CompareFunctionLess;
        case GOL::DepthCompare::Equal:        return MTL::CompareFunctionEqual;
        case GOL::DepthCompare::LessEqual:    return MTL::CompareFunctionLessEqual;
        case GOL::DepthCompare::Greater:      return MTL::CompareFunctionGreater;
        case GOL::DepthCompare::NotEqual:     return MTL::CompareFunctionNotEqual;
        case GOL::DepthCompare::GreaterEqual: return MTL::CompareFunctionGreaterEqual;
        case GOL::DepthCompare::Always:       return MTL::CompareFunctionAlways;
        }
        return MTL::CompareFunctionLessEqual;
    }
}

namespace GOL
{
    MetalBackend::MetalBackend() = default;

    MetalBackend::~MetalBackend()
    {
        deinit();
    }

    bool MetalBackend::init()
    {
        if( _device != nullptr )
            return true;    //	idempotent : already initialised

        _device = MTL::CreateSystemDefaultDevice();
        if( _device == nullptr )
            return false;

        _queue = _device->newCommandQueue();
        if( _queue == nullptr )
        {
            _device->release();
            _device = nullptr;
            return false;
        }

        //	Default sampler : linear filter on min/mag/mip, repeat wrap.
        //	Bound to fragment slot 0 every time the user binds a fragment
        //	texture, so shaders can `tex.sample(sampler, uv)` without the
        //	caller having to plumb sampler state. Future enhancement :
        //	expose a create_sampler() API for custom states.
        MTL::SamplerDescriptor* sdesc = MTL::SamplerDescriptor::alloc()->init();
        sdesc->setMinFilter( MTL::SamplerMinMagFilterLinear );
        sdesc->setMagFilter( MTL::SamplerMinMagFilterLinear );
        sdesc->setMipFilter( MTL::SamplerMipFilterLinear );
        sdesc->setSAddressMode( MTL::SamplerAddressModeRepeat );
        sdesc->setTAddressMode( MTL::SamplerAddressModeRepeat );
        _default_sampler = _device->newSamplerState( sdesc );
        sdesc->release();

        return true;
    }

    void MetalBackend::deinit()
    {
        //	If a frame was left open, close it gracefully before tearing
        //	down owned resources.
        if( _current_encoder )
        {
            _current_encoder->endEncoding();
            _current_encoder->release();
            _current_encoder = nullptr;
        }
        if( _current_cb )
        {
            _current_cb->release();
            _current_cb = nullptr;
        }

        for( auto& [ id, buffer ] : _buffers )
        {
            if( buffer )
                buffer->release();
        }
        _buffers.clear();

        for( auto& [ id, texture ] : _textures )
        {
            if( texture )
                texture->release();
        }
        _textures.clear();

        for( auto& [ id, prog ] : _programs )
        {
            if( prog.pso ) prog.pso->release();
            if( prog.vs  ) prog.vs->release();
            if( prog.fs  ) prog.fs->release();
            if( prog.ds  ) prog.ds->release();
        }
        _programs.clear();

        if( _default_sampler )
        {
            _default_sampler->release();
            _default_sampler = nullptr;
        }

        if( _queue )
        {
            _queue->release();
            _queue = nullptr;
        }
        if( _device )
        {
            _device->release();
            _device = nullptr;
        }

        _next_buffer_id  = 1;
        _next_texture_id = 1;
        _next_program_id = 1;
        _last_error.clear();
    }

    DeviceInfo MetalBackend::get_device_info() const
    {
        DeviceInfo info{};
        if( !_device )
            return info;

        NS::String* name = _device->name();
        if( name )
            info.name = name->utf8String();

        info.has_unified_memory = _device->hasUnifiedMemory();

        //	Detect the Apple GPU family floor. Apple7 = M1, Apple8 = M2,
        //	Apple9 = M3, Apple10 = M4. Walk from the highest known family
        //	downward to find the maximum the device supports.
        info.supports_apple_family = false;
        info.apple_family_min      = 0;
        if( _device->supportsFamily( MTL::GPUFamilyApple9 ) )
        {
            info.supports_apple_family = true;
            info.apple_family_min      = 9;
        }
        else if( _device->supportsFamily( MTL::GPUFamilyApple8 ) )
        {
            info.supports_apple_family = true;
            info.apple_family_min      = 8;
        }
        else if( _device->supportsFamily( MTL::GPUFamilyApple7 ) )
        {
            info.supports_apple_family = true;
            info.apple_family_min      = 7;
        }
        return info;
    }

    BufferId MetalBackend::gen_buffer()
    {
        if( !_device )
            return kInvalidBufferId;

        BufferId const id = _next_buffer_id++;
        _buffers[ id ] = nullptr;   //	allocated on first buffer_data() call
        return id;
    }

    void MetalBackend::delete_buffer( BufferId id )
    {
        auto it = _buffers.find( id );
        if( it == _buffers.end() )
            return;
        if( it->second )
            it->second->release();
        _buffers.erase( it );
    }

    void MetalBackend::buffer_data( BufferId id, std::size_t size, void const* data, BufferUsage usage )
    {
        auto it = _buffers.find( id );
        if( it == _buffers.end() || !_device )
            return;

        //	Release the previous allocation if any ; sizes may change between
        //	calls (e.g. when an MEU resizes its vertex buffer).
        if( it->second )
        {
            it->second->release();
            it->second = nullptr;
        }

        MTL::ResourceOptions opts = to_mtl_resource_options( usage );
        MTL::Buffer* buf = ( data != nullptr )
            ? _device->newBuffer( data, size, opts )
            : _device->newBuffer( size, opts );

        it->second = buf;
    }

    TextureId MetalBackend::gen_texture_2d( std::uint32_t w, std::uint32_t h, TextureFormat fmt )
    {
        if( !_device || w == 0 || h == 0 )
            return kInvalidTextureId;

        MTL::PixelFormat mtl_fmt = to_mtl_pixel_format( fmt );
        if( mtl_fmt == MTL::PixelFormatInvalid )
            return kInvalidTextureId;

        MTL::TextureDescriptor* desc = MTL::TextureDescriptor::texture2DDescriptor(
            mtl_fmt, w, h, /*mipmapped*/ false );
        desc->setStorageMode( MTL::StorageModeShared );
        desc->setUsage( MTL::TextureUsageShaderRead | MTL::TextureUsageShaderWrite |
                        MTL::TextureUsageRenderTarget );

        MTL::Texture* tex = _device->newTexture( desc );
        //	IMPORTANT : do NOT call `desc->release()` here. `texture2DDescriptor`
        //	is a class-method factory that follows Apple's ObjC convention --
        //	the returned object is already autoreleased. Calling release() is
        //	a double-release that only surfaces inside AppKit's run-loop
        //	autorelease pool (continuation 40 root cause for the
        //	`aaaseed_app_smoke` SEGFAULT discovered in continuation 39).
        //	Standalone gtest's pool drains differently and tolerated the
        //	bug ; the .app's per-event-loop drain catches it.

        if( !tex )
            return kInvalidTextureId;

        TextureId const id = _next_texture_id++;
        _textures[ id ] = tex;
        return id;
    }

    void MetalBackend::delete_texture( TextureId id )
    {
        auto it = _textures.find( id );
        if( it == _textures.end() )
            return;
        if( it->second )
            it->second->release();
        _textures.erase( it );
    }

    void MetalBackend::texture_data_2d( TextureId id, void const* data, std::size_t bytes_per_row )
    {
        auto it = _textures.find( id );
        if( it == _textures.end() || !it->second || !data )
            return;

        MTL::Texture* tex = it->second;
        MTL::Region region = MTL::Region::Make2D( 0, 0, tex->width(), tex->height() );
        tex->replaceRegion( region, /*level*/ 0, data, bytes_per_row );
    }

    TextureId MetalBackend::gen_texture_3d( std::uint32_t w, std::uint32_t h, std::uint32_t d,
                                            TextureFormat fmt, void const* data )
    {
        if( !_device || w == 0 || h == 0 || d == 0 )
            return kInvalidTextureId;

        MTL::PixelFormat mtl_fmt = to_mtl_pixel_format( fmt );
        if( mtl_fmt == MTL::PixelFormatInvalid )
            return kInvalidTextureId;

        //	No factory class method for 3D in metal-cpp — build the
        //	descriptor explicitly. Pattern parallels the ObjC
        //	`MTLTextureDescriptor` defaults : mipmapLevelCount = 1,
        //	sampleCount = 1, arrayLength = 1, no mipmaps.
        MTL::TextureDescriptor* desc = MTL::TextureDescriptor::alloc()->init();
        desc->setTextureType( MTL::TextureType3D );
        desc->setPixelFormat( mtl_fmt );
        desc->setWidth(  w );
        desc->setHeight( h );
        desc->setDepth(  d );
        desc->setMipmapLevelCount( 1 );
        desc->setStorageMode( MTL::StorageModeShared );
        desc->setUsage( MTL::TextureUsageShaderRead );

        MTL::Texture* tex = _device->newTexture( desc );
        //	`alloc()->init()` IS owned by us (unlike the texture2DDescriptor
        //	class method which returns autoreleased) ; balance with release.
        desc->release();

        if( !tex )
            return kInvalidTextureId;

        //	Optional initial upload. Metal 3D textures require
        //	bytesPerImage (slice stride) on replaceRegion ; for tightly
        //	packed RGBA8 that's bytes_per_row * h. Bytes per pixel comes
        //	from the format. We only handle the formats actually wired
        //	via to_mtl_pixel_format ; if a caller passes Depth32F here
        //	the upload silently no-ops (depth-as-3D makes no sense).
        if( data )
        {
            std::size_t bpp = 0;
            switch( fmt )
            {
            case TextureFormat::RGBA8:   bpp = 4; break;
            case TextureFormat::BGRA8:   bpp = 4; break;
            case TextureFormat::R8:      bpp = 1; break;
            case TextureFormat::RG8:     bpp = 2; break;
            case TextureFormat::RGBA16F: bpp = 8; break;
            case TextureFormat::R32F:    bpp = 4; break;
            case TextureFormat::Depth32F: bpp = 0; break;  // not meaningful for 3D
            }
            if( bpp > 0 )
            {
                std::size_t const bytes_per_row   = bpp * w;
                std::size_t const bytes_per_image = bytes_per_row * h;
                MTL::Region region = MTL::Region::Make3D( 0, 0, 0, w, h, d );
                tex->replaceRegion( region, /*level*/ 0, /*slice*/ 0,
                                    data, bytes_per_row, bytes_per_image );
            }
        }

        TextureId const id = _next_texture_id++;
        _textures[ id ] = tex;
        return id;
    }

    void MetalBackend::bind_fragment_texture_3d( TextureId id, std::uint32_t slot )
    {
        //	Metal binds 2D and 3D textures into the same fragment-texture
        //	slot space ; the MSL declared type (texture2d vs texture3d) is
        //	what the GPU uses to interpret the binding at sample time.
        //	Reuse the existing bind path so the default linear-wrap
        //	sampler at fragment slot 0 also gets installed.
        bind_fragment_texture( id, slot );
    }

    ProgramId MetalBackend::create_program_msl( char const* msl_source,
                                                char const* vertex_entry,
                                                char const* fragment_entry,
                                                TextureFormat target_color_fmt )
    {
        ProgramDesc desc;
        desc.msl_source       = msl_source;
        desc.vertex_entry     = vertex_entry;
        desc.fragment_entry   = fragment_entry;
        desc.target_color_fmt = target_color_fmt;
        //	No vertex attributes / layouts ; shader must use [[vertex_id]]
        //	or similar to get its vertex data.
        return create_program( desc );
    }

    ProgramId MetalBackend::create_program( ProgramDesc const& desc )
    {
        _last_error.clear();
        if( !_device || !desc.msl_source || !desc.vertex_entry || !desc.fragment_entry )
        {
            _last_error = "MetalBackend::create_program: null arg or backend not initialised";
            return kInvalidProgramId;
        }

        NS::String* src = NS::String::string( desc.msl_source, NS::UTF8StringEncoding );
        NS::Error*  err = nullptr;
        MTL::Library* lib = _device->newLibrary( src, /*options*/ nullptr, &err );
        if( !lib )
        {
            if( err && err->localizedDescription() )
                _last_error = err->localizedDescription()->utf8String();
            else
                _last_error = "MTL::Device::newLibrary returned nullptr";
            return kInvalidProgramId;
        }

        NS::String* vs_name = NS::String::string( desc.vertex_entry,   NS::UTF8StringEncoding );
        NS::String* fs_name = NS::String::string( desc.fragment_entry, NS::UTF8StringEncoding );

        Program p;
        p.vs = lib->newFunction( vs_name );
        p.fs = lib->newFunction( fs_name );
        lib->release();

        if( !p.vs || !p.fs )
        {
            _last_error = "vertex or fragment entry function not found in MSL source";
            if( p.vs ) p.vs->release();
            if( p.fs ) p.fs->release();
            return kInvalidProgramId;
        }

        MTL::RenderPipelineDescriptor* rpd = MTL::RenderPipelineDescriptor::alloc()->init();
        rpd->setVertexFunction(   p.vs );
        rpd->setFragmentFunction( p.fs );

        //	MRT (continuation 43) : configure slots 0..N-1 with the same
        //	pixel format and blend state. `color_attachment_count` defaults
        //	to 1 (backward-compat). When the shader emits a FragOut struct
        //	with `[[color(N)]]` fields, the caller bumps the count and the
        //	pipeline desc gets the matching number of slots configured.
        std::uint32_t const color_slots =
            ( desc.color_attachment_count > 0 ) ? desc.color_attachment_count : 1;
        auto* color_attach = rpd->colorAttachments()->object( 0 );
        for( std::uint32_t s = 0; s < color_slots; ++s )
        {
            auto* ca = rpd->colorAttachments()->object( s );
            ca->setPixelFormat( to_mtl_pixel_format( desc.target_color_fmt ) );
        }

        //	Depth attachment format on the pipeline descriptor MUST match
        //	the render pass's depth attachment format at draw time, or
        //	Metal validation throws. If depth_test/write are both off,
        //	we leave it at Invalid (no depth attachment expected).
        if( desc.depth_test_enabled || desc.depth_write_enabled )
            rpd->setDepthAttachmentPixelFormat( to_mtl_pixel_format( desc.depth_format ) );

        //	Blend state per program. Metal locks blend into the PSO ;
        //	can't be toggled mid-frame without a separate pipeline state.
        switch( desc.blend_mode )
        {
        case BlendMode::None:
            color_attach->setBlendingEnabled( false );
            break;
        case BlendMode::AlphaBlend:
            color_attach->setBlendingEnabled( true );
            color_attach->setSourceRGBBlendFactor( MTL::BlendFactorSourceAlpha );
            color_attach->setDestinationRGBBlendFactor( MTL::BlendFactorOneMinusSourceAlpha );
            color_attach->setRgbBlendOperation( MTL::BlendOperationAdd );
            color_attach->setSourceAlphaBlendFactor( MTL::BlendFactorOne );
            color_attach->setDestinationAlphaBlendFactor( MTL::BlendFactorOneMinusSourceAlpha );
            color_attach->setAlphaBlendOperation( MTL::BlendOperationAdd );
            break;
        case BlendMode::Additive:
            color_attach->setBlendingEnabled( true );
            color_attach->setSourceRGBBlendFactor( MTL::BlendFactorOne );
            color_attach->setDestinationRGBBlendFactor( MTL::BlendFactorOne );
            color_attach->setRgbBlendOperation( MTL::BlendOperationAdd );
            color_attach->setSourceAlphaBlendFactor( MTL::BlendFactorOne );
            color_attach->setDestinationAlphaBlendFactor( MTL::BlendFactorOne );
            color_attach->setAlphaBlendOperation( MTL::BlendOperationAdd );
            break;
        }

        //	Build the vertex descriptor when the caller provided attributes
        //	or layouts. Skipped entirely for full-screen [[vertex_id]]
        //	shaders so we don't pay the cost or risk validation churn.
        MTL::VertexDescriptor* vd = nullptr;
        if( desc.attribute_nb > 0 || desc.layout_nb > 0 )
        {
            vd = MTL::VertexDescriptor::alloc()->init();
            for( std::uint32_t i = 0; i < desc.attribute_nb; ++i )
            {
                auto const& a = desc.attributes[ i ];
                auto* attr = vd->attributes()->object( a.shader_location );
                attr->setFormat( to_mtl_vertex_format( a.format ) );
                attr->setOffset( a.offset );
                attr->setBufferIndex( a.buffer_slot );
            }
            for( std::uint32_t i = 0; i < desc.layout_nb; ++i )
            {
                auto const& l = desc.layouts[ i ];
                auto* layout = vd->layouts()->object( l.slot );
                layout->setStride( l.stride );
                layout->setStepFunction( MTL::VertexStepFunctionPerVertex );
                layout->setStepRate( 1 );
            }
            rpd->setVertexDescriptor( vd );
        }

        err = nullptr;
        p.pso = _device->newRenderPipelineState( rpd, &err );
        rpd->release();
        if( vd ) vd->release();

        if( !p.pso )
        {
            if( err && err->localizedDescription() )
                _last_error = err->localizedDescription()->utf8String();
            else
                _last_error = "newRenderPipelineState returned nullptr";
            p.vs->release();
            p.fs->release();
            return kInvalidProgramId;
        }

        //	Build the depth-stencil state when depth test or write is
        //	requested. Otherwise leave p.ds = nullptr ; bind_program will
        //	not touch the encoder's depth state, which keeps Metal's
        //	default ("no depth test, always write") in effect.
        if( desc.depth_test_enabled || desc.depth_write_enabled )
        {
            MTL::DepthStencilDescriptor* dsd = MTL::DepthStencilDescriptor::alloc()->init();
            dsd->setDepthCompareFunction( desc.depth_test_enabled
                                          ? to_mtl_compare( desc.depth_compare )
                                          : MTL::CompareFunctionAlways );
            dsd->setDepthWriteEnabled( desc.depth_write_enabled );
            p.ds = _device->newDepthStencilState( dsd );
            dsd->release();
        }

        ProgramId const id = _next_program_id++;
        _programs[ id ] = p;
        return id;
    }

    void MetalBackend::delete_program( ProgramId id )
    {
        auto it = _programs.find( id );
        if( it == _programs.end() )
            return;
        if( it->second.pso ) it->second.pso->release();
        if( it->second.vs  ) it->second.vs->release();
        if( it->second.fs  ) it->second.fs->release();
        if( it->second.ds  ) it->second.ds->release();
        _programs.erase( it );
    }

    bool MetalBackend::begin_render_pass( RenderPassDescriptor const& desc )
    {
        if( !_queue || _current_encoder )
            return false;

        auto tex_it = _textures.find( desc.color_attachment );
        if( tex_it == _textures.end() || !tex_it->second )
            return false;

        MTL::RenderPassDescriptor* rpd = MTL::RenderPassDescriptor::alloc()->init();
        auto* color = rpd->colorAttachments()->object( 0 );
        color->setTexture( tex_it->second );
        color->setLoadAction(  to_mtl_load_action( desc.load_action ) );
        color->setStoreAction( MTL::StoreActionStore );
        color->setClearColor( MTL::ClearColor::Make( desc.clear_color[ 0 ],
                                                     desc.clear_color[ 1 ],
                                                     desc.clear_color[ 2 ],
                                                     desc.clear_color[ 3 ] ) );

        //	MRT (continuation 43) : bind additional color attachments at
        //	slots 1..3. All share load_action + clear_color with slot 0.
        for( int i = 0; i < RenderPassDescriptor::kMaxAdditionalColorAttachments; ++i )
        {
            TextureId const aid = desc.additional_color_attachments[ i ];
            if( aid == kInvalidTextureId )
                continue;
            auto aux_it = _textures.find( aid );
            if( aux_it == _textures.end() || !aux_it->second )
                continue;
            auto* aux = rpd->colorAttachments()->object( i + 1 );
            aux->setTexture( aux_it->second );
            aux->setLoadAction(  to_mtl_load_action( desc.load_action ) );
            aux->setStoreAction( MTL::StoreActionStore );
            aux->setClearColor( MTL::ClearColor::Make( desc.clear_color[ 0 ],
                                                       desc.clear_color[ 1 ],
                                                       desc.clear_color[ 2 ],
                                                       desc.clear_color[ 3 ] ) );
        }

        //	Optional depth attachment.
        if( desc.depth_attachment != kInvalidTextureId )
        {
            auto depth_it = _textures.find( desc.depth_attachment );
            if( depth_it != _textures.end() && depth_it->second )
            {
                auto* depth = rpd->depthAttachment();
                depth->setTexture( depth_it->second );
                depth->setLoadAction(  to_mtl_load_action( desc.depth_load_action ) );
                depth->setStoreAction( MTL::StoreActionStore );
                depth->setClearDepth(  desc.clear_depth );
            }
        }

        _current_cb      = _queue->commandBuffer();
        _current_cb->retain();
        _current_encoder = _current_cb->renderCommandEncoder( rpd );
        _current_encoder->retain();
        rpd->release();

        //	GPU debug-marker integration. When desc.debug_label is non-null
        //	we set it on (a) the command buffer (groups all encoded work
        //	under the label in Frame Capture) AND (b) push it as an
        //	encoder debug group (gives the label its own band on the GPU
        //	timeline). end_render_pass emits the matching popDebugGroup
        //	before endEncoding. Labels propagate to Instruments' Metal
        //	System Trace as well. Pairs with the CPU os_signpost path
        //	from NSightEvents.h per the perf-profiling doctrine.
        _debug_label_was_pushed = false;
        if( desc.debug_label && _current_encoder )
        {
            NS::String* label = NS::String::string( desc.debug_label,
                                                    NS::StringEncoding::UTF8StringEncoding );
            _current_cb->setLabel( label );
            _current_encoder->pushDebugGroup( label );
            _debug_label_was_pushed = true;
        }

        return _current_encoder != nullptr;
    }

    void MetalBackend::end_render_pass()
    {
        if( !_current_encoder )
            return;
        if( _debug_label_was_pushed )
        {
            _current_encoder->popDebugGroup();
            _debug_label_was_pushed = false;
        }
        _current_encoder->endEncoding();
        _current_encoder->release();
        _current_encoder = nullptr;
    }

    void MetalBackend::set_viewport( float x, float y, float w, float h )
    {
        if( !_current_encoder )
            return;
        MTL::Viewport vp = { x, y, w, h, 0.0, 1.0 };
        _current_encoder->setViewport( vp );
    }

    void MetalBackend::bind_program( ProgramId id )
    {
        if( !_current_encoder )
            return;
        auto it = _programs.find( id );
        if( it == _programs.end() || !it->second.pso )
            return;
        _current_encoder->setRenderPipelineState( it->second.pso );
        if( it->second.ds )
            _current_encoder->setDepthStencilState( it->second.ds );
    }

    void MetalBackend::bind_vertex_buffer( BufferId id, std::uint32_t slot, std::uint32_t offset )
    {
        if( !_current_encoder )
            return;
        auto it = _buffers.find( id );
        if( it == _buffers.end() || !it->second )
            return;
        _current_encoder->setVertexBuffer( it->second, offset, slot );
    }

    void MetalBackend::bind_fragment_buffer( BufferId id, std::uint32_t slot, std::uint32_t offset )
    {
        if( !_current_encoder )
            return;
        auto it = _buffers.find( id );
        if( it == _buffers.end() || !it->second )
            return;
        _current_encoder->setFragmentBuffer( it->second, offset, slot );
    }

    void MetalBackend::bind_fragment_texture( TextureId id, std::uint32_t slot )
    {
        if( !_current_encoder )
            return;
        auto it = _textures.find( id );
        if( it == _textures.end() || !it->second )
            return;
        _current_encoder->setFragmentTexture( it->second, slot );
        //	Bind the default sampler at fragment slot 0 so MSL code can
        //	`tex.sample( s, uv )` against a sampler at [[sampler(0)]].
        //	Idempotent — same sampler bound every call, cheap.
        if( _default_sampler )
            _current_encoder->setFragmentSamplerState( _default_sampler, 0 );
    }

    void MetalBackend::draw_arrays( PrimitiveType prim, std::uint32_t first, std::uint32_t count )
    {
        if( !_current_encoder )
            return;
        _current_encoder->drawPrimitives( to_mtl_primitive( prim ),
                                          NS::UInteger( first ),
                                          NS::UInteger( count ) );
    }

    void MetalBackend::draw_elements( PrimitiveType prim, std::uint32_t count, IndexType index_type,
                                      BufferId index_buffer, std::uint32_t index_offset )
    {
        if( !_current_encoder )
            return;
        auto it = _buffers.find( index_buffer );
        if( it == _buffers.end() || !it->second )
            return;
        MTL::IndexType mtl_idx = ( index_type == IndexType::UInt16 )
            ? MTL::IndexTypeUInt16
            : MTL::IndexTypeUInt32;
        _current_encoder->drawIndexedPrimitives( to_mtl_primitive( prim ),
                                                 NS::UInteger( count ),
                                                 mtl_idx,
                                                 it->second,
                                                 NS::UInteger( index_offset ) );
    }

    void MetalBackend::read_texture_pixels( TextureId id,
                                            std::uint32_t x, std::uint32_t y,
                                            std::uint32_t w, std::uint32_t h,
                                            void* out_buffer,
                                            std::size_t bytes_per_row )
    {
        if( !out_buffer )
            return;
        auto it = _textures.find( id );
        if( it == _textures.end() || !it->second )
            return;
        //	Shared-storage textures (the default for our gen_texture_2d)
        //	let the CPU read directly via getBytes — no blit needed. For
        //	private storage we would have to schedule a blit-to-staging
        //	first ; not exposed yet.
        MTL::Region region = MTL::Region::Make2D( x, y, w, h );
        it->second->getBytes( out_buffer, bytes_per_row, region, /*level*/ 0 );
    }

    void MetalBackend::present()
    {
        if( _current_encoder )
            end_render_pass();
        if( !_current_cb )
            return;
        _current_cb->commit();
        _current_cb->waitUntilCompleted();
        _current_cb->release();
        _current_cb = nullptr;
    }

    bool MetalBackend::begin_window_render_pass( CA::MetalDrawable* drawable,
                                                  RenderPassDescriptor const& desc )
    {
        if( !_queue || _current_encoder || !drawable )
            return false;

        MTL::Texture* drawable_tex = drawable->texture();
        if( !drawable_tex )
            return false;

        MTL::RenderPassDescriptor* rpd = MTL::RenderPassDescriptor::alloc()->init();
        auto* color = rpd->colorAttachments()->object( 0 );
        color->setTexture( drawable_tex );
        color->setLoadAction(  to_mtl_load_action( desc.load_action ) );
        color->setStoreAction( MTL::StoreActionStore );
        color->setClearColor( MTL::ClearColor::Make( desc.clear_color[ 0 ],
                                                     desc.clear_color[ 1 ],
                                                     desc.clear_color[ 2 ],
                                                     desc.clear_color[ 3 ] ) );

        //	Optional depth attachment (looked up from the texture map like
        //	the offscreen path — host owns the depth texture lifetime).
        if( desc.depth_attachment != kInvalidTextureId )
        {
            auto depth_it = _textures.find( desc.depth_attachment );
            if( depth_it != _textures.end() && depth_it->second )
            {
                auto* depth = rpd->depthAttachment();
                depth->setTexture( depth_it->second );
                depth->setLoadAction(  to_mtl_load_action( desc.depth_load_action ) );
                depth->setStoreAction( MTL::StoreActionStore );
                depth->setClearDepth(  desc.clear_depth );
            }
        }

        _current_cb      = _queue->commandBuffer();
        _current_cb->retain();
        _current_encoder = _current_cb->renderCommandEncoder( rpd );
        _current_encoder->retain();
        rpd->release();

        //	Same GPU debug-marker integration as begin_render_pass.
        //	present_window emits the matching popDebugGroup via the
        //	end_render_pass call it makes.
        _debug_label_was_pushed = false;
        if( desc.debug_label && _current_encoder )
        {
            NS::String* label = NS::String::string( desc.debug_label,
                                                    NS::StringEncoding::UTF8StringEncoding );
            _current_cb->setLabel( label );
            _current_encoder->pushDebugGroup( label );
            _debug_label_was_pushed = true;
        }

        return _current_encoder != nullptr;
    }

    void MetalBackend::present_window( CA::MetalDrawable* drawable )
    {
        if( _current_encoder )
            end_render_pass();
        if( !_current_cb )
            return;
        if( drawable )
            _current_cb->presentDrawable( drawable );
        //	Async commit ; do NOT wait. Frame pacing is the OS's job for
        //	window targets — blocking here pegs the GPU and kills
        //	display-link cadence.
        _current_cb->commit();
        _current_cb->release();
        _current_cb = nullptr;
    }

    // c148 : bridge accessor — returns the active MTL::RenderCommandEncoder as
    // void* per the c134-A doctrine (no ObjC/MTL types in public C++ headers).
    void* MetalBackend::get_active_encoder() const
    {
        return static_cast< void* >( _current_encoder );
    }

    // c151-A : bridge accessor — returns the active MTL::CommandBuffer as
    // void* per the c134-A doctrine. ImGui's RenderDrawData() needs this
    // separately from the encoder so it can lazily compile its pipeline
    // state using commandBuffer.device.
    void* MetalBackend::get_active_command_buffer() const
    {
        return static_cast< void* >( _current_cb );
    }

    // c153 (second_todo.md S8) : native MTL::Texture* for a TextureId, as a
    // void* per the c134-A bridge doctrine. nullptr if unknown.
    void* MetalBackend::native_texture( TextureId id ) const
    {
        auto it = _textures.find( id );
        if( it == _textures.end() )
            return nullptr;
        return static_cast< void* >( it->second );
    }

    Backend* create_default_backend()
    {
        return new MetalBackend();
    }
}
