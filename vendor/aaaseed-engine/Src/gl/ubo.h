
#ifdef AAA_GL_UBO_H
#error "GL_UBO_H included more than once."
#endif
#define AAA_GL_UBO_H 1


#if !defined(AAA_GL_ITEM_H)
#	include "gl/Item.h"
#endif
#ifndef AAA_GOL_BASE_H
#	include "gol/gol_base.h"
#endif

namespace gl {

	class ubo_props final
	{
	public:
		void *	_data		= nullptr;
		UINT32	_size		= 0;
		bool	_b_dynamic	= false;		//todo extend because more than 2 in gl
	};

	class ubo final : public item
	{
	private:
		void *	_data;
		UINT32	_size;		
		bool	_b_dynamic;

	//	void init(ubo_props CONST & p_props);
	public:
		static	UINT32*			get_nb_pt();
		static	UINT32*			get_nb_created_pt();

		ubo();
		~ubo();
		C_NO_CPY_MOVE(ubo)	

		void init_ogl()		override final;
		void update_ogl()	override final;
		void release_ogl()	override final;

		static ubo * make(		UINT32 CONST		size,
								bool CONST			b_dynamic = false,
								char CONST * CONST	data = nullptr,
								C_PCHAR_C			name = nullptr,
								bool CONST			b_immediate = true
						);
		//todo redo props
		//	static ubo *	make(		ubo_props CONST & props,	bool CONST b_immediate );
		static void	reclaim(			ubo * CONST pt,				bool CONST b_immediate );


		
		virtual	C_PCHAR_C	get_type_name();

	


		FINLINE	void bind( UINT32 const binding_index ) const
				{
					GOL::bind_buffer_base( GL_UNIFORM_BUFFER, binding_index, _gol_id );
				}

		FINLINE	void unbind( UINT32 const binding_index ) const
				{
					GOL::unbind_buffer_base( GL_UNIFORM_BUFFER, binding_index );
				}
				void bind_and_write(	UINT32 CONST binding_index,	void CONST * data,	UINT32 size	);
		FINLINE void bind_and_write(	UINT32 CONST binding_index,	void CONST * data	)
				{
					bind_and_write( binding_index, data, _size );
				}
		void * map_write() CONST;
		void   unmap() CONST;

		void write(				void CONST *	data, UINT32 CONST size, UINT32 CONST dst_offset,	bool CONST b_immediate );
		void write_all(			void CONST *	data,												bool CONST b_immediate );

		FINLINE UINT32 get_size() CONST		{	return _size;	}
	};

} // namespace gl
