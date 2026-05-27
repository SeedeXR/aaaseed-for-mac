// Unused in AAASeed
#ifdef AAA_GL_FBO_H
#error "GL_FBO_H included more than once."
#endif
#define AAA_GL_FBO_H 1


#if !defined(_VECTOR_)
#	include <vector>
#endif
#if !defined(AAA_GL_ITEM_H)
#	include "Item.h"
#endif
#if !defined(AAA_GL_UTILS_H)
#	include "utils.h"
#endif


namespace gl {
	
	class fbo_slot final
	{
	public:
		uint32_t				_ogl_id 			= 0;
		e_texture_format		_format				= e_texture_format::RGBA;
		e_texture_data_type		_data_type			= e_texture_data_type::unsigned_byte;
		e_texture_filter		_filter				= e_texture_filter::linear;
		e_texture_wrap			_wrap				= e_texture_wrap::edge;
		float					_clear_color[4]		= { 0.0f };
	};

	class fbo_props final 
	{
	public:
		uint32_t				_width				= 0;
		uint32_t				_height				= 0;

		std::vector<fbo_slot>	_slots;

		bool					_use_depth			= false;
		uint32_t				_depth_ogl_id		= 0;
		e_texture_format		_depth_format		= e_texture_format::depth;
		e_texture_data_type		_depth_data_type	= e_texture_data_type::real;
		e_texture_filter		_depth_filter		= e_texture_filter::nearest;
		e_texture_wrap			_depth_wrap			= e_texture_wrap::repeat;

	public:
		void set_resolution( uint32_t const size_x, uint32_t const size_y );

		void add_slot(fbo_slot && slot);
		void add_slot(fbo_slot const & slot);
		void add_slot(
			e_texture_format const		format,
			e_texture_data_type const	data_type,
			e_texture_filter const		filter,
			e_texture_wrap const		wrap);

		void use_depth(
			e_texture_data_type const	data_type,
			e_texture_filter const		filter,
			e_texture_wrap const		wrap);
	};

	class fbo final : public item
	{
	private:
		uint32_t *				_buffer_ids = nullptr;
		fbo_props				_props;

	public:
		static fbo *	make( fbo_props const & props, bool const b_immediate );		
		static void		reclaim(	fbo * ptr, bool const b_immediate );

	public:
		static	UINT32*	get_nb_pt();
		static	UINT32*	get_nb_created_pt();

		fbo();
		~fbo();
		C_NO_CPY_MOVE(fbo)	

		virtual	C_PCHAR_C	get_type_name();

	private:
		void init( fbo_props const & props );
		void init_ogl() override final;
		void update_ogl() override final;
		void release_ogl() override final;

	public:
		void bind() const;
		void bind_and_clear() const;
		void unbind() const;

		void bind_slot_read(	uint32_t const slot_index,	uint32_t const bind_index ) const;
		void unbind_slot_read(	uint32_t const slot_index, 	uint32_t const bind_index ) const;

		void bind_depth_read(	uint32_t const bind_index	) const;
		void unbind_depth_read(	uint32_t const bind_index	) const;

	public:
		void update_resolution(	uint32_t const size_x,	uint32_t const size_y, bool const b_immediate );
	};

} // namespace gl
