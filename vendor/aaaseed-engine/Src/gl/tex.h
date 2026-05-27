// Unused in AAASeed
#ifdef AAA_GL_TEX_H
#error "GL_TEX_H included more than once."
#endif
#define AAA_GL_TEX_H 1


#if !defined(AAA_GL_ITEM_H)
#	include "gl/Item.h"
#endif



namespace gl {

	enum class e_texture_data_type 
	{
		unknown,
		unsigned_byte,
		real,
		num
	};
	uint32_t get_gl_texture_data_type( gl::e_texture_data_type const p_in) ;

	enum class e_texture_filter 
	{
		unknown,
		linear,
		nearest,
		num
	};
	uint32_t get_gl_texture_filter( gl::e_texture_filter const p_in, bool const p_enable_mipmap );

	enum class e_texture_wrap 
	{
		unknown,
		border,
		edge,
		repeat,
		mirror,
		mirroredge,
		num
	};
	uint32_t get_gl_texture_wrap( gl::e_texture_wrap const p_in );

	enum class e_texture_format 
	{
		unknown,
		gray_scale,         gray_scale_alpha,
		RGB,    RGB16F,     RGB32F,
		RGBA,   RGBA16F,    RGBA32F,
		BGR,    BGR16F,     BGR32F,
		BGRA,   BGRA16F,    BGRA32F,
		ARGB,   ARGB16F,    ARGB32F,
		depth,  depth16,    depth24,    depth32,    depth24_stencil8,
		num
	};

	class tex_props final 
	{
	public:
		char const *				_data		{ nullptr };
		uint32_t					_width		{ 0 };
		uint32_t					_height		{ 0 };
		gl::e_texture_format		_format	{ gl::e_texture_format::RGBA };
		gl::e_texture_data_type		_data_type	{ gl::e_texture_data_type::unsigned_byte };
		gl::e_texture_filter		_filter		{ gl::e_texture_filter::linear };
		gl::e_texture_wrap			_wrap		{ gl::e_texture_wrap::edge };
		bool                        _generate_mipmaps { false };
	};

	class tex final : public item
	{
	private:
		gl::tex_props				_props;
		uint32_t					_gl_internal_fmt	{ 0 };

	public:
		static gl::tex * make( gl::tex_props const & p_props,	bool const b_immediate );
		static void	reclaim(   gl::tex * const p_ptr,					bool const b_immediate );

	public:
		tex() = default;
		~tex() = default;
		
		virtual	C_PCHAR_C	get_type_name();

		C_NO_CPY_MOVE(tex)

	private:
		void init( gl::tex_props const &props );
		void init_ogl() override final;
		void update_ogl() override final;
		void release_ogl() override final;

	public:
		void bind(						uint32_t const bind_index) const;
		void unbind(					uint32_t const bind_index) const;

		void bind_compute_read(			uint32_t const bind_index) const;
		void unbind_compute_read(		uint32_t const bind_index) const;

		void bind_compute_write(		uint32_t const bind_index) const;
		void unbind_compute_write(		uint32_t const bind_index) const;

		void bind_compute_read_write(	uint32_t const bind_index) const;
		void unbind_compute_read_write(	uint32_t const bind_index) const;

	public:
		void update( char const* data, bool const b_immediate);
	};

	uint32_t get_gl_texture_internal_format(    gl::e_texture_format const p_in );
	uint32_t get_gl_texture_format(				gl::e_texture_format const p_in );
	uint32_t get_gl_texture_attachment(         gl::e_texture_format const p_in );


} // namespace gl
