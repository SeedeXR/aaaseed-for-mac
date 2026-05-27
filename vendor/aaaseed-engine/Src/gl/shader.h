// Unused in AAASeed
#ifdef AAA_GL_SHADER_H
#error "GL_SHADER_H included more than once."
#endif
#define AAA_GL_SHADER_H 1


#if !defined(AAA_GL_ITEM_H)
#	include "gl/Item.h"
#endif
#ifndef	AAA_AAA_GL_H
#	include "draw/aaa_gl.h"
#endif

namespace gl {

	enum class shader_pipeline_e {
		unknown,
		compute,
		pixel,
		geometry,
		tessellation
	};

	class shader_props final
	{
	public:
		gl::shader_pipeline_e	_pipeline;
		GLchar const *			_prgms[5];
	};

	class shader final : public item
	{
	private:
		uint32_t					_id				= 0;								
		uint32_t					_prgm_ids[5]	= { 0 };							
		gl::shader_pipeline_e		_pipeline		= gl::shader_pipeline_e::unknown;	
		GLchar const *				_prgms[5]		= { nullptr };						
		bool						_valid			= false;

	public:
		static gl::shader *	make(
			gl::shader_props const &	p_props, 
			bool const					p_immediate);
		
		static void	reclaim(
			gl::shader *				p_ptr, 
			bool const					p_immediate);

	public:
		shader() = default;
		~shader() = default;
		
		virtual	C_PCHAR_C	get_type_name();

		C_NO_CPY_MOVE(shader)

	private:
		void init(gl::shader_props const & p_props);

		static bool is_compiled(uint32_t const p_shader_id);
		void init_compute();
		void init_pixel();
		void init_geometry();
		void init_tessellation();
		void init_ogl() override final;
		void update_ogl() override final;
		void release_ogl() override final;

	public:
		void bind() const;
		void unbind() const;
		void dispatch(
			uint32_t const p_num_group_x,
			uint32_t const p_num_group_y,
			uint32_t const p_num_group_z) const;

	public:
		void update(
			GLchar const *	p_prgm1, 
			bool const		p_immediate);

		void update(
			GLchar const *	p_prgm1, 
			GLchar const *	p_prgm2, 
			bool const		p_immediate);

		void update(
			GLchar const *	p_prgm1, 
			GLchar const *	p_prgm2, 
			GLchar const *	p_prgm3, 
			bool const		p_immediate);

		void update(
			GLchar const *	p_prgm1, 
			GLchar const *	p_prgm2, 
			GLchar const *	p_prgm3, 
			GLchar const *	p_prgm4, 
			GLchar const *	p_prgm5, 
			bool const		p_immediate);

	public:
		inline bool const is_valid() const {
			return _valid;
		}
	};

} // namespace gl
