
#ifdef AAA_GL_ITEM_H
#error "GL_ITEM_H included more than once."
#endif
#define AAA_GL_ITEM_H 1


#ifndef AAA_OBJ_H
#	include "infrastructure/obj/obj.h"
#endif
#if !defined(AAA_GL_UTILS_H)
#	include "utils.h"
#endif
#ifndef AAA_AAA_STR_H
#	include "aaa_str.h"
#endif
#ifndef AAA_AAA_MEM_H
#	include "aaa_mem.h"
#endif

namespace gl
{
	/**
	* @class gl::Item
	*
	* Abstract base gl drawable Item.
	* Manages OpenGL init / update / release requests
	*/
	class item : public c_obj
	{
	protected:
		UINT32	_gol_id;
		o_str	_o_name;

		item();
		C_NO_CPY_MOVE(item)	

		/** Request OpenGL init. */
		void request_init_ogl();
		/** Request OpenGL update. */
		void request_update_ogl();
		/** Request pointer release. */
		void request_release();

		void gen_id();
		void del_id();

	public:
		static	UINT32*			get_nb_pt();
		static	UINT32*			get_nb_created_pt();

		~item();

		template< class T >
		static void release_and_null( T* & pt )
		{
			if( pt )
			{ 
				pt->release_ogl();
				SAFE_DELETE(pt);
			}
		}



		FINLINE UINT32 get_gol_id()		{	return _gol_id;		}

				void		set_name( C_PCHAR_C name );
		virtual	C_PCHAR_C	get_type_name() = 0;

		/** Init OpenGL components. */
		virtual void init_ogl() = 0;
		/** Update OpenGL components. */
		virtual void update_ogl() = 0;
		/** \! Deallocate and release class members. */
		virtual void release_ogl() = 0;

		FINLINE void request_init_ogl( bool CONST b_immediate )
		{
			if( b_immediate )
				init_ogl();
			else
				request_init_ogl();
		}
		FINLINE void request_update_ogl( bool CONST b_immediate )
		{
			if( b_immediate )
				update_ogl();
			else
				request_update_ogl();		
		}
		FINLINE void request_release( bool CONST b_immediate )
		{
			if( b_immediate ) 
			{
				release_ogl();
				delete this;	//todo maa say really ?
			}
			else 
				request_release();
		}
	};

} // namespace gl
