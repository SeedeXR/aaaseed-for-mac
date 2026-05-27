
#ifdef AAA_GL_IBO_H
#error "GL_IBO_H included more than once."
#endif
#define AAA_GL_IBO_H 1


#if !defined(AAA_GL_ITEM_H)
#	include "gl/Item.h"
#endif
#ifndef	AAA_GOL_DRAW_H
#	include "gol/gol_draw.h"
#endif

namespace gl {
	//todo extend to other type
	class ibo_props final
	{
	public:
		void CONST *	_data;
		GOL::IBO_TYPE	_type	{ GOL::IBO_TYPE::IBO_UINT32 };
		UINT32			_nb;
		bool			_b_dynamic;		//todo extend because more than 2 in gl

		ibo_props();
		ibo_props( INT32 CONST nb, void CONST * CONST data, bool CONST b_dynamic );
		ibo_props( GOL::IBO_TYPE CONST type, UINT32 CONST nb, void CONST * data, bool CONST b_dynamic );
	};


	class ibo final : public item
	{
	private:
		GLenum			_gl_type;
		void CONST *	_data;
		UINT32			_nb;
		UINT32			_nb_allocated;	// when updating slots, only recreate if new size is larger than previous allocation
		bool			_b_dynamic;		//todo extend because more than 2 in gl
		GOL::IBO_TYPE	_type;

	public:
		static gl::ibo * make( gl::ibo_props CONST & p_props, C_PCHAR_C name, bool CONST b_immediate = true );
		static void	reclaim(			gl::ibo * CONST	pt, bool CONST b_immediate );

	public:
		static	UINT32*			get_nb_pt();
		static	UINT32*			get_nb_created_pt();

		ibo();
		~ibo();
		C_NO_CPY_MOVE(ibo)

		virtual	C_PCHAR_C	get_type_name();



	private:
		UINT32 get_memory_size() CONST;
		void init(gl::ibo_props CONST & p_props);

	public:
		void init_ogl()    override final;
		void update_ogl()  override final;
		void release_ogl() override final;

		void draw_triangles(		) CONST;
		void draw_triangle_strip(	) CONST;
		void draw_lines(			) CONST;
		void draw_line_strip(		) CONST;
		void draw_points(			) CONST;

		void draw_triangles(		UINT32 CONST instance_nb ) CONST;
		void draw_triangle_strip(	UINT32 CONST instance_nb ) CONST;
		void draw_lines(			UINT32 CONST instance_nb ) CONST;
		void draw_line_strip(		UINT32 CONST instance_nb ) CONST;
		void draw_points(			UINT32 CONST instance_nb ) CONST;

	public:
		void update( INT32 CONST * CONST data,	bool CONST b_immediate );
		void update( ibo_props CONST & props,	bool CONST b_immediate );
	};

} // namespace gl
