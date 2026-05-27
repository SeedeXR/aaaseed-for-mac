// src/gol/gol_backend.h
//
// Abstract `GOL::Backend` interface. The Mac port implements Metal under it
// (`src/gol/metal/`) ; the existing Windows engine code will be wrapped in a
// `GOL::WindowsBackend` mirror later (separate workstream — keeps Windows
// behavior identical to upstream).
//
// MVP method set : enough to (a) initialise a graphics device, (b) create
// buffers and textures, (c) clear and present an offscreen render target.
// The interface grows as the engine demands more GOL methods. Adding a
// virtual here forces both backends to implement it — that's the lever that
// keeps the two paths aligned.
//
// Naming maps loosely to the existing GOL namespace functions in
// `vendor/aaaseed-engine/Src/gol/gol*.h`. We do NOT replicate every existing
// signature one-for-one ; we collapse where the same operation has many
// overloads in legacy GL.

#pragma once

#include <cstdint>
#include <cstddef>
#include <string>

namespace GOL
{
    //	Opaque resource handles. Backend-specific implementations map these
    //	to MTL::Buffer*, MTL::Texture* etc. Zero = invalid sentinel.
    using BufferId   = std::uint32_t;
    using TextureId  = std::uint32_t;
    using ProgramId  = std::uint32_t;

    static constexpr BufferId  kInvalidBufferId  = 0;
    static constexpr TextureId kInvalidTextureId = 0;
    static constexpr ProgramId kInvalidProgramId = 0;

    enum class BufferUsage : std::uint8_t
    {
        Static,    // upload once, read many ; backend may put in `.private` storage
        Dynamic,   // updated occasionally
        Stream,    // updated every frame ; backend keeps in `.shared` mapped memory
    };

    enum class TextureFormat : std::uint8_t
    {
        RGBA8,     // 8-bit unsigned normalised, 4 channels
        BGRA8,     // 8-bit unsigned normalised, 4 channels, BGRA byte order
        R8,        // 8-bit single channel
        RG8,       // 8-bit two-channel
        RGBA16F,   // 16-bit float, 4 channels (HDR)
        R32F,      // 32-bit float single channel
        Depth32F,  // 32-bit float depth
    };

    enum class PrimitiveType : std::uint8_t
    {
        Points,
        Lines,
        LineStrip,
        Triangles,
        TriangleStrip,
    };

    enum class IndexType : std::uint8_t
    {
        UInt16,
        UInt32,
    };

    enum class VertexFormat : std::uint8_t
    {
        Float,
        Float2,
        Float3,
        Float4,
        UChar4Norm,    // 4 bytes interpreted as RGBA in [0,1] — vertex colors
    };

    //	Describes one vertex attribute in a shader. The `shader_location`
    //	maps to MSL's `[[attribute(N)]]` (or GLSL `layout(location=N)`).
    //	`buffer_slot` is which bound vertex buffer this attribute reads
    //	from ; `offset` is the byte offset inside one vertex stride.
    struct VertexAttribute
    {
        std::uint32_t shader_location = 0;
        std::uint32_t buffer_slot     = 0;
        std::uint32_t offset          = 0;
        VertexFormat  format          = VertexFormat::Float;
    };

    //	Describes one vertex buffer's layout. `stride` is the byte
    //	distance between consecutive vertices in the buffer.
    struct VertexBufferLayout
    {
        std::uint32_t slot   = 0;
        std::uint32_t stride = 0;
    };

    enum class BlendMode : std::uint8_t
    {
        None,        // no blending ; src replaces dst
        AlphaBlend,  // src * src.a + dst * (1 - src.a)
        Additive,    // src + dst
    };

    enum class DepthCompare : std::uint8_t
    {
        Never,
        Less,
        Equal,
        LessEqual,
        Greater,
        NotEqual,
        GreaterEqual,
        Always,
    };

    //	Full program-creation descriptor. Use this when the shader needs
    //	vertex attributes (anything beyond a [[vertex_id]] full-screen
    //	pass). The existing `create_program_msl` wrapper is kept for
    //	full-screen shaders that don't need a vertex layout.
    struct ProgramDesc
    {
        char const* msl_source       = nullptr;
        char const* vertex_entry     = nullptr;
        char const* fragment_entry   = nullptr;
        TextureFormat target_color_fmt = TextureFormat::RGBA8;
        BlendMode     blend_mode       = BlendMode::None;

        VertexAttribute const*    attributes    = nullptr;
        std::uint32_t             attribute_nb  = 0;
        VertexBufferLayout const* layouts       = nullptr;
        std::uint32_t             layout_nb     = 0;

        //	Depth state. Both attachment format AND the test/write flags
        //	live on the program because Metal locks them into the
        //	MTL::DepthStencilState bound at draw time and the pipeline
        //	descriptor's depthAttachmentPixelFormat must match the render
        //	pass's depth attachment format.
        bool          depth_test_enabled  = false;
        bool          depth_write_enabled = true;
        DepthCompare  depth_compare        = DepthCompare::LessEqual;
        TextureFormat depth_format         = TextureFormat::Depth32F;

        //	MRT (multiple render targets) : when > 1, the pipeline desc
        //	configures the pixel-format on color attachments 0..N-1, all
        //	set to `target_color_fmt`. The render-pass-time
        //	RenderPassDescriptor must bind matching textures to those
        //	slots. Default 1 = single attachment (existing behaviour ;
        //	backward-compat). Continuation 43.
        std::uint32_t color_attachment_count = 1;
    };

    enum class LoadAction : std::uint8_t
    {
        DontCare,    // contents on entry are undefined ; cheapest on TBDR
        Load,        // preserve previous contents
        Clear,       // clear to the descriptor's clear_color
    };

    struct RenderPassDescriptor
    {
        static constexpr int kMaxAdditionalColorAttachments = 3;

        TextureId  color_attachment    = kInvalidTextureId;
        LoadAction load_action          = LoadAction::Clear;
        float      clear_color[4]       = { 0.f, 0.f, 0.f, 1.f };

        //	MRT slots 1..3 (slot 0 = `color_attachment` above). When
        //	kInvalidTextureId the slot is not bound. All bound slots
        //	share `load_action` and `clear_color` with slot 0 -- if a
        //	future test needs per-slot clear, expose
        //	`additional_clear_colors[N][4]` then. Continuation 43.
        TextureId additional_color_attachments[ kMaxAdditionalColorAttachments ]
            = { kInvalidTextureId, kInvalidTextureId, kInvalidTextureId };

        //	Optional depth attachment. If `kInvalidTextureId`, no depth
        //	buffer is bound — the render pass is color-only. Depth
        //	texture must have been created with TextureFormat::Depth32F.
        TextureId  depth_attachment     = kInvalidTextureId;
        LoadAction depth_load_action    = LoadAction::Clear;
        float      clear_depth          = 1.f;     // far plane, conventional GL/Metal default

        //	GPU debug label visible in Xcode Frame Capture. When non-null,
        //	the backend (a) sets the command buffer's label, (b) calls
        //	pushDebugGroup(label) on the render-command encoder, and (c)
        //	emits popDebugGroup at end_render_pass time. Pairs with the
        //	CPU os_signpost integration from NSightEvents.h per the
        //	performance-profiling doctrine (philosophy.md Part 2.95).
        //	Pointer must outlive the begin/end_render_pass call ; literal
        //	strings are fine.
        char const* debug_label         = nullptr;
    };

    struct DeviceInfo
    {
        std::string name;                  // e.g. "Apple M4"
        bool        has_unified_memory;    // Apple Silicon : always true
        bool        supports_apple_family; // Apple Silicon GPU family (M1+) detected
        std::uint32_t apple_family_min;    // 7 = M1, 8 = M2, 9 = M3, 10 = M4
    };

    //	Pure-virtual interface every backend implements. Construction is
    //	cheap ; init() does the heavy work and may fail. Use the static
    //	factory below to create a backend matching the host platform.
    class Backend
    {
    public:
        virtual ~Backend() = default;

        //	Lifecycle
        virtual bool init()   = 0;
        virtual void deinit() = 0;
        virtual DeviceInfo get_device_info() const = 0;

        //	Buffers
        virtual BufferId gen_buffer()                                                          = 0;
        virtual void     delete_buffer(  BufferId id )                                         = 0;
        virtual void     buffer_data(    BufferId id, std::size_t size, void const* data,
                                         BufferUsage usage )                                   = 0;

        //	Textures
        virtual TextureId gen_texture_2d( std::uint32_t w, std::uint32_t h,
                                          TextureFormat fmt )                                  = 0;
        virtual void      delete_texture( TextureId id )                                       = 0;
        virtual void      texture_data_2d( TextureId id, void const* data,
                                           std::size_t bytes_per_row )                         = 0;

        //	3D textures (volumetric). Used by vol3d_Plane / vol3d_Voxel
        //	shaders that sample a `texture3d<float>` for per-voxel data.
        //	`data` is optional ; if non-null, it is uploaded at creation
        //	time (the most common pattern — a static volume baked once and
        //	sampled many frames). The data layout is tightly packed :
        //	rows of width pixels, slices of (width * height) pixels, with
        //	no per-row or per-slice padding.
        //
        //	Metal requires `bytesPerImage` (slice stride) on 3D uploads ;
        //	the backend computes it as `bytes_per_row * h`. Callers must
        //	NOT pre-pad their data — pass tightly-packed bytes.
        virtual TextureId gen_texture_3d( std::uint32_t w, std::uint32_t h, std::uint32_t d,
                                          TextureFormat fmt, void const* data )                = 0;
        //	Bind a 3D texture for sampling in the fragment shader. Same
        //	`[[texture(N)]]` slot space as 2D — Metal disambiguates by the
        //	texture-type declared in the MSL (`texture3d<float>` vs
        //	`texture2d<float>`). The default linear-wrap sampler bound by
        //	bind_fragment_texture is reused so MSL can sample at
        //	[[sampler(0)]] without extra wiring.
        virtual void      bind_fragment_texture_3d( TextureId id, std::uint32_t slot )         = 0;

        //	Shader programs. The backend is responsible for compiling the
        //	platform-native source : MSL on Metal, GLSL on the Windows
        //	OpenGL backend. The vertex_entry / fragment_entry names tell
        //	the backend which functions in the source act as the entry
        //	points (Metal needs explicit per-function names ; GLSL uses
        //	"main" by convention).
        //	target_color_fmt locks the pipeline state to a render target
        //	format ; Metal PSOs are descriptor-bound and cannot be reused
        //	across pixel formats. Defaults to RGBA8 which is what the
        //	current MEU pipeline outputs.
        virtual ProgramId create_program_msl( char const* msl_source,
                                              char const* vertex_entry,
                                              char const* fragment_entry,
                                              TextureFormat target_color_fmt = TextureFormat::RGBA8 ) = 0;
        //	Full descriptor variant — required when the shader consumes
        //	vertex attributes via [[stage_in]] / [[attribute(N)]]. The
        //	wrapper above calls this with empty attribute / layout arrays.
        virtual ProgramId create_program(     ProgramDesc const& desc ) = 0;
        virtual void      delete_program(     ProgramId id ) = 0;
        //	Returns the last shader-compilation or pipeline-build error
        //	message, or an empty string if the most recent
        //	create_program_msl succeeded.
        virtual std::string const& get_last_error() const = 0;

        //	Render-pass lifecycle. Every draw call must happen between
        //	begin_render_pass() and end_render_pass(). Metal-native ;
        //	maps to MTLRenderCommandEncoder. The OpenGL backend will fake
        //	this by binding the descriptor's color attachment as the
        //	current framebuffer and applying clear / load state.
        virtual bool begin_render_pass( RenderPassDescriptor const& desc ) = 0;
        virtual void end_render_pass() = 0;

        //	State (must be called inside an active render pass)
        virtual void set_viewport(        float x, float y, float w, float h ) = 0;
        virtual void bind_program(        ProgramId id ) = 0;
        virtual void bind_vertex_buffer(  BufferId  id, std::uint32_t slot, std::uint32_t offset ) = 0;
        //	Same buffer family, bound for fragment-shader use. On Metal,
        //	vertex and fragment buffers share the same MTLBuffer pool but
        //	are routed to different shader stages.
        virtual void bind_fragment_buffer( BufferId id, std::uint32_t slot, std::uint32_t offset ) = 0;

        //	Bind a 2D texture for sampling in the fragment shader.
        //	Slot maps to MSL `[[texture(N)]]`. A default linear-wrap
        //	sampler is automatically bound at slot 0 by the backend so
        //	shaders can sample without callers explicitly creating one.
        virtual void bind_fragment_texture( TextureId id, std::uint32_t slot ) = 0;

        //	Draw (must be called inside an active render pass with a
        //	program bound).
        virtual void draw_arrays( PrimitiveType prim, std::uint32_t first, std::uint32_t count ) = 0;

        //	Indexed draw. `index_buffer` must have been populated via
        //	buffer_data() with uint16 or uint32 indices matching `index_type`.
        //	`index_offset` is in bytes into the index buffer.
        virtual void draw_elements( PrimitiveType prim, std::uint32_t count, IndexType index_type,
                                    BufferId index_buffer, std::uint32_t index_offset ) = 0;

        //	Read pixels back from a texture. The texture must be in
        //	shared / managed storage (the default for backend-created
        //	textures). `out_buffer` must be at least `bytes_per_row * h`
        //	bytes. For Metal this is `MTLTexture::getBytes`. For OpenGL
        //	this would be `glReadPixels` against a bound FBO.
        //	Synchronous : call after present() so the GPU has finished
        //	writing the texture before we read.
        virtual void read_texture_pixels( TextureId id,
                                          std::uint32_t x, std::uint32_t y,
                                          std::uint32_t w, std::uint32_t h,
                                          void* out_buffer,
                                          std::size_t bytes_per_row ) = 0;

        //	Commit any encoded commands to the GPU and (for offscreen
        //	targets) block until completion. For a real window target
        //	this will also present the drawable ; that wiring lands when
        //	the MTKView host is in place. For now, synchronous on
        //	completion so tests can read back pixels reliably.
        virtual void present() = 0;

        //	Backend self-identification — useful for tests and for
        //	conditional behaviour at engine-script level.
        virtual char const* backend_name() const = 0;
    };

    //	Factory : returns a Backend matching the host platform. On macOS
    //	always returns a GOL::MetalBackend. On Windows would return a
    //	GOL::WindowsBackend (not yet implemented).
    //
    //	The caller owns the returned pointer ; call delete to destroy. The
    //	backend's destructor calls deinit() if needed.
    Backend* create_default_backend();
}
