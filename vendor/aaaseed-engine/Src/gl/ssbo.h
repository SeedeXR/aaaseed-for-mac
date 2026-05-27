
#ifdef AAA_GL_SSBO_H
#error "GL_SSBO_H included more than once."
#endif
#define AAA_GL_SSBO_H 1


#if !defined(AAA_GL_ITEM_H)
#	include "gl/Item.h"
#endif
#ifndef AAA_GOL_BASE_H
#	include "gol/gol_base.h"
#endif

namespace gl
{

	//class ssbo_props final
	//{
	//public:
	//	char *	_data	 = nullptr;
	//	UINT32	_size	 = 0;
	//	bool	_b_dynamic;		//todo extend because more than 2 in gl
	//};
	
	class ssbo final : public item
	{
	private:
		char *	_data;
		UINT32	_size;
		bool	_b_dynamic;		//todo extend because more than 2 in gl

	public:
		static ssbo * make(	UINT32 CONST		size,
							bool CONST			b_dynamic,
							char CONST * CONST	data,
							C_PCHAR_C			name = nullptr,
							bool CONST			b_immediate = true);
		//todo redo props
		//static ssbo * make(	ssbo_props CONST &	props, 
		//					bool CONST b_immediate = true );

		static void	reclaim(			ssbo * CONST pt,		bool CONST b_immediate	);

	public:
		static	UINT32*	get_nb_pt();
		static	UINT32*	get_nb_created_pt();

		ssbo();
		~ssbo();
		C_NO_CPY_MOVE(ssbo)	

		virtual	C_PCHAR_C	get_type_name();

	

	private:
		void init( UINT32 CONST size, bool CONST b_dynamic, char CONST * CONST data );


	public:
		//void init(ssbo_props CONST & p_props);
		void init_ogl()		override final;
		void update_ogl()	override final;
		void release_ogl()	override final;

		FINLINE void bind( UINT32 CONST binding_index ) CONST
		{
			GOL::bind_buffer_base( GL_SHADER_STORAGE_BUFFER, binding_index, _gol_id );
		}
		FINLINE void unbind( UINT32 CONST binding_index ) CONST
		{
			GOL::unbind_buffer_base( GL_SHADER_STORAGE_BUFFER, binding_index );
		}

		void *	map_write() CONST;
		void	unmap() CONST;

	public:
		void write( void CONST * data, UINT32 CONST	size, UINT32 CONST dst_offset, bool CONST b_immediate );

		void read ( void * output_data, UINT32 CONST size, UINT32 CONST dst_offset );
	};

} // namespace gl
