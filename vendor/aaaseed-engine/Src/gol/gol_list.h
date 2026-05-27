
#ifdef AAA_GOL_LIST_H
#error "GOL_LIST_H included more than once."
#endif
#define AAA_GOL_LIST_H 1


#ifndef AAA_GOL_BASE_H
#	include "gol_base.h"
#endif

namespace GOL	{

extern	bool	b_list_allow;
extern	GLuint	list_base_last;
extern	GLuint	gen_lists(		GLsizei CONST range );
extern	void	delete_lists(	GLuint CONST list_id, GLsizei CONST range );

FINLINE	void	new_list(		GLuint CONST list, GLenum CONST mode )
{
	glNewList( list, mode );
}
FINLINE	void	compile_list(	GLuint CONST list )
{
	new_list( list, GL_COMPILE );
}
FINLINE	void	end_list()
{
	glEndList();
}

FINLINE	void	call_list(		GLuint CONST list )
{
	if( list )
		glCallList( list );
}
FINLINE	void	call_lists(		GLsizei CONST n, GLenum CONST type, GLvoid CONST * lists )
{
	glCallLists( n, type, lists );
}

FINLINE	void	set_list_base(	GLuint CONST base )
{	
	if( is_state_cache_no() || list_base_last != base )
	{
		list_base_last = base;
		glListBase( base );
	}
}

}	//namespace GOL

