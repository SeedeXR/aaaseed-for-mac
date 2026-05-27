#include "gol/gol_list.h"

namespace GOL	{
	
GLuint	list_base_last = -1;
bool	b_list_allow = true;

GLuint	gen_lists( GLsizei CONST range )
{
	UINT32 ret = glGenLists( range );
	if( ret == 0 )
		debug_break( "glGenLists() didn't generate list" );
	return ret;
}
void	delete_lists( UINT32 CONST list_id, GLsizei CONST range )
{
	glDeleteLists( list_id, range );
}

}	//namespace GOL
