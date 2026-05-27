
#ifdef AAA_GL_VAO_H
#error "GL_VAO_H included more than once."
#endif
#define AAA_GL_VAO_H 1


#ifndef	AAA_GOL_DRAW_H
#	include "gol/gol_draw.h"
#endif
#if !defined(_VECTOR_)
#	include <vector>
#endif
#if !defined(AAA_GL_ITEM_H)
#	include "gl/Item.h"
#endif


namespace gl {

	class c_vao_slot;
	//class c_vao;

	class c_vao_slot_props final
	{
		friend c_vao_slot;
	private:
		GOL::VAO_TYPE	_type;
		void CONST *	_data;
		UINT32			_nb; 
		bool			_b_dynamic;
		GLuint			_index;
	public:
		c_vao_slot_props();
		c_vao_slot_props(	GOL::VAO_TYPE CONST type, void CONST * CONST data, UINT32 CONST nb, bool CONST b_dynamic=true );
		~c_vao_slot_props();

		void init(			GOL::VAO_TYPE CONST type, void CONST * CONST data, UINT32 CONST nb, bool CONST b_dynamic=true );

		void set_index( UINT32 i )	{ _index = i; }
		FINLINE UINT32 get_nb()	CONST { return _nb; } 
	
//todo provoke an error in fbx_gl_create_mesh ": no appropiate constructor
//		and or error when emplace_back() in c_vao::add_slot 
//		C_NO_CPY_MOVE(c_vao_slot_props);
	};



	class c_vao_props final
	{
//		friend c_vao_slot;
//	public:
		std::vector<c_vao_slot_props> _slots;

//todo provoke an error in fbx_gl_create_mesh ": no appropiate constructor
//		C_NO_CPY_MOVE(c_vao_props);
	public:
//		void add_slot(	gl::c_vao_slot_props && p_slot);
//		void add_slot(	gl::c_vao_slot_props CONST & p_slot);
		FINLINE UINT32 get_slot_nb()									CONST	{ return (UINT32)(_slots.size()); } 
		FINLINE c_vao_slot_props CONST & get_slot( INT32 CONST idx )	CONST	{ return _slots[idx];	}

		c_vao_slot_props* add_slot(	GOL::VAO_TYPE CONST type, void CONST * CONST data, UINT32 CONST nb, bool CONST b_dynamic );
	};



	class c_vao_slot final : public item
	{
//		friend c_vao_props;
//		friend c_vao;
	private:
		GOL::VAO_TYPE	_type;
		GLuint			_index;
		void CONST *	_data;
		UINT32			_nb; 
		UINT32			_nb_allocated;	// when updating slots, only recreate if new size is larger than previous allocation
		bool			_b_dynamic;		// todo extend because more than 2 in gl
		bool			_b_dirty;
//		UINT32			_byte_nb;
		//todo in fact GOL should have a struct
		UINT32			_elt_byte_nb;
		UINT32			_compo_nb;
		UINT32			_gl_type;

	public:
		static	UINT32*			get_nb_pt();
		static	UINT32*			get_nb_created_pt();

		c_vao_slot();
	//	c_vao_slot( GOL::VAO_TYPE CONST type, void CONST * CONST data, UINT32 CONST nb, bool CONST b_dynamic=true );
		~c_vao_slot();
		C_NO_CPY_MOVE(c_vao_slot);

		void init_ogl()    override final;
		void update_ogl()  override final;
		void release_ogl() override final;

		void init(  c_vao_slot_props CONST & src );
	
		virtual	C_PCHAR_C	get_type_name();

		void set_index( UINT32 i )	{ _index = i; }
		void set_data(	c_vao_slot_props CONST & src,				bool CONST b_immediate = true );
		void set_data(	void CONST * CONST data,	UINT32 size,	bool CONST b_immediate = true );
		void set_data(	void CONST * CONST data,					bool CONST b_immediate = true );

		FINLINE UINT32 get_nb()				CONST { return _nb; } 
		FINLINE UINT32 get_nb_allocated()	CONST { return _nb_allocated; }

	//	FINLINE UINT32 CONST get_size_byte() CONST		{	return _elt_byte_nb * _nb;	}
	};



	class c_vao final : public item
	{
	private:
		c_vao_slot*		_pt_slot;
		UINT32			_slot_nb;

	public:
		static c_vao * make( c_vao_props CONST & props, C_PCHAR_C name, bool CONST b_immediate = true  );		
		static void	reclaim(     c_vao * CONST pt,		bool CONST b_immediate = true );

	public:
		static	UINT32*			get_nb_pt();
		static	UINT32*			get_nb_created_pt();

		c_vao();
		~c_vao();
		C_NO_CPY_MOVE(c_vao)

		virtual	C_PCHAR_C	get_type_name();

	private:
		void init(gl::c_vao_props CONST & props );

	public:
		void init_ogl()    override final;
		void update_ogl()  override final;
		void release_ogl() override final;

		void bind() const;
		void unbind() const;

	public:
		void set_slot_data(	INT32 CONST idx, void CONST * CONST data,	bool CONST b_immediate = true );
		void update(		c_vao_props CONST & props,					bool CONST b_immediate = true );
	};

} // namespace gl
